/*
 * maestroControl.cpp
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "maestroControl.h"
#include "../servoSteps/servoSteps.h"

#define HIP_BASE_SPEED (200)
#define KNEE_BASE_SPEED (200)

typedef enum {WAIT_FOR_STOP, SET_KNEES, SET_HIPS, NEXT_SEQUENCE_STEP} servoControlSteps_t;
servoControlSteps_t servoControlStep;

/* in servoSteps.c */
extern legPositions_t sequenceLegRun_G[][2];
extern movement_t movement_G;

/* in proximitySensing.c */
extern int proxReadings_G[];

int sequenceStep;
int runningSequenceNum_G;

volatile void maestroControl_Init(void){
	uint8_t i;
	runningSequenceNum_G = 0;
	sequenceStep = 0;
	servoControlStep = WAIT_FOR_STOP;

	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(MAESTRO_SET_SPEED);
		Serial.write(i);
		Serial.write(50 & 0x7F);
		Serial.write((50 >> 7) & 0x7F);
	}

	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(MAESTRO_SET_SPEED);
		Serial.write(i+NUM_LEGS);
		Serial.write(50 & 0x7F);
		Serial.write((50 >> 7) & 0x7F);
	}
}


volatile void maestroControl_update(void){
	uint8_t i;
	int walkingSpeed;

	if(movement_G == RETREAT){
		walkingSpeed = HIP_BASE_SPEED;
	}
	else{
		walkingSpeed = (HIP_BASE_SPEED - (proxReadings_G[1]*2));
		if(walkingSpeed <= 0){
			walkingSpeed = 1;
		}
	}

	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(MAESTRO_SET_SPEED);
		Serial.write(i);
		Serial.write(walkingSpeed & 0x7F);
		Serial.write((walkingSpeed >> 7) & 0x7F);
	}

	switch(servoControlStep){

		default:
		case WAIT_FOR_STOP:
			Serial.write(MAESTRO_GET_STATE);
			if((Serial.read() != 0x00) || ((movement_G == STOP) && (sequenceStep == 0))){
				servoControlStep = WAIT_FOR_STOP;
			}
			else{
				servoControlStep = SET_KNEES;
			}
			break;

		case SET_KNEES:
			for(i = 0; i < NUM_LEGS; i++){
				Serial.write(MAESTRO_SET_TARGET);
				Serial.write(i + NUM_LEGS);
				Serial.write(sequenceLegRun_G[i][runningSequenceNum_G].knee[sequenceStep] & 0x7F);
				Serial.write((sequenceLegRun_G[i][runningSequenceNum_G].knee[sequenceStep] >> 7) & 0x7F);
			}

			servoControlStep = SET_HIPS;
			break;

		case SET_HIPS:
			for(i = 0; i < NUM_LEGS; i++){
				Serial.write(MAESTRO_SET_TARGET);
				Serial.write(i);
				Serial.write((sequenceLegRun_G[i][runningSequenceNum_G].hip[sequenceStep] & 0x7F));
				Serial.write((sequenceLegRun_G[i][runningSequenceNum_G].hip[sequenceStep] >> 7) & 0x7F);
			}

			servoControlStep = NEXT_SEQUENCE_STEP;
			break;

		case NEXT_SEQUENCE_STEP:
			if(++sequenceStep >= NUM_SEQ_STEPS){
				sequenceStep = 0;
				runningSequenceNum_G = (runningSequenceNum_G == 0) ? 1 : 0;
			}

			servoControlStep = WAIT_FOR_STOP;
			break;
	}

}



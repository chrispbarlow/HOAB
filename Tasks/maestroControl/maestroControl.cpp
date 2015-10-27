/*
 * maestroControl.cpp
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "maestroControl.h"
#include "../servoSteps/servoSteps.h"

#define HIP_BASE_SPEED (100)
#define KNEE_BASE_SPEED (200)

typedef enum {WAIT_FOR_STOP, SET_KNEES, SET_HIPS, NEXT_SEQUENCE_STEP} servoControlSteps_t;
volatile servoControlSteps_t servoControlStep;

/* in servoSteps.c */
extern legPositions_t sequenceLegRun_G[];
extern movement_t movement_G;

volatile int sequenceStep;
volatile bool usingSequence_G;

volatile void maestroControl_Init(void){
	uint8_t i;
	usingSequence_G = 0;
	sequenceStep = 0;
	servoControlStep = WAIT_FOR_STOP;
	usingSequence_G = false;

	Serial.write(0xA1);
	Serial.read();

	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(MAESTRO_SET_SPEED);
		Serial.write(i);
		Serial.write(HIP_BASE_SPEED & 0x7F);
		Serial.write((HIP_BASE_SPEED >> 7) & 0x7F);
	}

	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(MAESTRO_SET_SPEED);
		Serial.write(i+NUM_LEGS);
		Serial.write(KNEE_BASE_SPEED & 0x7F);
		Serial.write((KNEE_BASE_SPEED >> 7) & 0x7F);
	}
}


volatile void maestroControl_update(void){
	static int i;
	int walkingSpeed;
	static int stopCheck = 0;
//	if (movement_G != RESET){
//		if(movement_G == RETREAT){
//			walkingSpeed = HIP_BASE_SPEED;
//		}
//		else{
//			walkingSpeed = (HIP_BASE_SPEED - (proxReadings_G[1]*2));
//			if(walkingSpeed <= 0){
//				walkingSpeed = 1;
//			}
//		}
//
//		for(i = 0; i < (NUM_LEGS); i++){
//			Serial.write(MAESTRO_SET_SPEED);
//			Serial.write(i);
//			Serial.write(walkingSpeed & 0x7F);
//			Serial.write((walkingSpeed >> 7) & 0x7F);
//		}


//	}

	switch(servoControlStep){

		default:
		case WAIT_FOR_STOP:
			Serial.write(MAESTRO_GET_STATE);
			if((Serial.read() != 0x00) || ((movement_G == STOP) && (sequenceStep == 0))){
				servoControlStep = WAIT_FOR_STOP;
				stopCheck = 0;
			}
			else{
				if((++stopCheck >= 0) && (usingSequence_G == true)){

					servoControlStep = SET_KNEES;
				}
			}
			break;

		case SET_KNEES:
			for(i = 0; i < NUM_LEGS; i++){
				Serial.write(MAESTRO_SET_TARGET);
				Serial.write(i + NUM_LEGS);
				Serial.write(sequenceLegRun_G[i].knee[sequenceStep] & 0x7F);
				Serial.write((sequenceLegRun_G[i].knee[sequenceStep] >> 7) & 0x7F);
				Serial.read();
			}

			servoControlStep = SET_HIPS;
			break;

		case SET_HIPS:
			for(i = 0; i < NUM_LEGS; i++){
				Serial.write(MAESTRO_SET_TARGET);
				Serial.write(i);
				Serial.write((sequenceLegRun_G[i].hip[sequenceStep] & 0x7F));
				Serial.write((sequenceLegRun_G[i].hip[sequenceStep] >> 7) & 0x7F);
				Serial.read();
			}

			servoControlStep = NEXT_SEQUENCE_STEP;
			break;

		case NEXT_SEQUENCE_STEP:
			sequenceStep++;
			if(sequenceStep >= NUM_SEQ_STEPS){
				sequenceStep = 0;
				usingSequence_G = false;
			}

			servoControlStep = WAIT_FOR_STOP;
			break;
	}



}



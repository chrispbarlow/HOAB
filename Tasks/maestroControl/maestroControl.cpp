/*
 * maestroControl.cpp
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "maestroControl.h"
#include "../servoSteps/servoSteps.h"


typedef enum {WAIT_FOR_STOP, SET_KNEES, SET_HIPS, NEXT_SEQUENCE_STEP} servoControlSteps_t;
volatile servoControlSteps_t servoControlStep;
volatile int sequenceStep;
volatile bool usingSequence_G;

/* in servoSteps.c */
extern legPositions_t sequenceLegRun_G[];
extern int walkingSpeed_G;


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
		Serial.read();
	}

	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(MAESTRO_SET_SPEED);
		Serial.write(i+NUM_LEGS);
		Serial.write(KNEE_BASE_SPEED & 0x7F);
		Serial.write((KNEE_BASE_SPEED >> 7) & 0x7F);
		Serial.read();
	}

	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(MAESTRO_SET_ACCEL);
		Serial.write(i);
		Serial.write(50 & 0x7F);
		Serial.write((50 >> 7) & 0x7F);
		Serial.read();
	}

	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(MAESTRO_SET_ACCEL);
		Serial.write(i+NUM_LEGS);
		Serial.write(100 & 0x7F);
		Serial.write((100 >> 7) & 0x7F);
		Serial.read();
	}
}


volatile void maestroControl_update(void){
	static int i;
	static int stopCheck = 0;

	switch(servoControlStep){

		default:
		case WAIT_FOR_STOP:
			Serial.write(MAESTRO_GET_STATE);
			if(Serial.read() != 0x00){
				servoControlStep = WAIT_FOR_STOP;
			}
			else if(usingSequence_G == true){
				servoControlStep = SET_KNEES;
			}

			for(i = 0; i < (NUM_LEGS); i++){
				Serial.write(MAESTRO_SET_SPEED);
				Serial.write(i);
				Serial.write(walkingSpeed_G & 0x7F);
				Serial.write((walkingSpeed_G >> 7) & 0x7F);
				Serial.read();
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



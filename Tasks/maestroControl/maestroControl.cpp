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


/* in servoSteps.c */
extern legPositions_t sequenceLegRun_G[];
extern movement_t movement_G;

/* in proximitySensing.c */
extern int proxReadings_G[];

int sequenceStep;

volatile void maestroControl_Init(void){
	uint8_t i;

	sequenceStep = 0;

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

	Serial.write(MAESTRO_GET_STATE);
	if(Serial.read() == 0x00){
		for(i = 0; i < NUM_LEGS; i++){
			Serial.write(MAESTRO_SET_TARGET);
			Serial.write(i);
			Serial.write((sequenceLegRun_G[i].hip[sequenceStep] & 0x7F));
			Serial.write((sequenceLegRun_G[i].hip[sequenceStep] >> 7) & 0x7F);

			Serial.write(MAESTRO_SET_TARGET);
			Serial.write(i + NUM_LEGS);
			Serial.write(sequenceLegRun_G[i].knee[sequenceStep] & 0x7F);
			Serial.write((sequenceLegRun_G[i].knee[sequenceStep] >> 7) & 0x7F);
		}
//		if(movement_G != STOP){
		sequenceStep++;
			if(sequenceStep >= NUM_SEQ_STEPS){
				sequenceStep = 0;
			}
//		}
	}
}



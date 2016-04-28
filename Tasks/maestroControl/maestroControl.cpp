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

/* in servoSteps.cpp */
extern legPositions_t sequenceLegRun_G[];
extern int walkingSpeed_G;

#define HIP_SERVOS	(0)
#define KNEE_SERVOS	(NUM_LEGS)


void maestroCommandAllLegs(uint8_t offset, uint8_t cmd, uint16_t value){
	uint8_t i;
	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(cmd);
		Serial.write(i+offset);
		Serial.write(value & 0x7F);
		Serial.write((value >> 7) & 0x7F);
		Serial.read();
	}
}

uint8_t maestroGetState(void){
	uint8_t state = 0xFF;

	Serial.write(MAESTRO_GET_STATE);
	state = Serial.read();

	return state;
}

void maestroControl_Init(void){
	uint8_t i;
	usingSequence_G = 0;
	sequenceStep = 0;
	servoControlStep = WAIT_FOR_STOP;
	usingSequence_G = false;

	Serial.write(0xA1);
	Serial.read();

	maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_SPEED, HIP_BASE_SPEED);
	maestroCommandAllLegs(KNEE_SERVOS, MAESTRO_SET_SPEED, KNEE_BASE_SPEED);

	maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_ACCEL, 50);
	maestroCommandAllLegs(KNEE_SERVOS, MAESTRO_SET_ACCEL, 100);
}


void maestroControl_update(void){
	static int i;
	static int stopCheck = 0;
	uint8_t state;

	switch(servoControlStep){

		default:
		case WAIT_FOR_STOP:
			maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_SPEED, walkingSpeed_G);

			if((maestroGetState() == 0x00) && (usingSequence_G == true)){
				servoControlStep = SET_KNEES;
			}
			else{
				/* Either the servos are still moving or there are no new steps */
				servoControlStep = WAIT_FOR_STOP;
			}

			break;

		case SET_KNEES:
			maestroCommandAllLegs(KNEE_SERVOS, MAESTRO_SET_TARGET, sequenceLegRun_G[i].knee[sequenceStep]);
			servoControlStep = SET_HIPS;
			break;

		case SET_HIPS:
			maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_TARGET, sequenceLegRun_G[i].hip[sequenceStep]);
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



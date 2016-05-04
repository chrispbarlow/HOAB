/*
 * maestro.cpp
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "maestro.h"
#include "../motionControl/motionControl.h"

#define HIP_SERVOS	(0)
#define KNEE_SERVOS	(NUM_LEGS)

static servoControlSteps_t maestroControlStep;
static int sequenceStep;

typedef struct{
  legPositions_t *servoSequence;
  uint16_t walkingSpeed;
}motionParameters_t;

static motionParameters_t commandedMotion;

static void maestroCommandAllLegs(uint8_t offset, uint8_t cmd, uint16_t value){
	uint8_t i;
	for(i = 0; i < (NUM_LEGS); i++){
		Serial.write(cmd);
		Serial.write(i+offset);
		Serial.write(value & 0x7F);
		Serial.write((value >> 7) & 0x7F);
		Serial.read();
	}
}

static uint8_t maestroGetState(void){
	uint8_t state = 0xFF;

	Serial.write(MAESTRO_GET_STATE);
	state = Serial.read();

	return state;
}

void maestro_Init(void){
	Serial.write(0xA1);
	Serial.read();

	maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_SPEED, HIP_BASE_SPEED);
	maestroCommandAllLegs(KNEE_SERVOS, MAESTRO_SET_SPEED, KNEE_BASE_SPEED);

	maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_ACCEL, 50);
	maestroCommandAllLegs(KNEE_SERVOS, MAESTRO_SET_ACCEL, 100);

	maestroControlStep = SEQUENCE_FINISHED;
}


void maestro_update(void){
	static int i;
	uint8_t state;

	switch(maestroControlStep){
		default:
		case SEQUENCE_FINISHED:
			/* Do nothing, wait for new sequence */
			break;

		case NEW_SEQUENCE_READY:
		case SET_KNEES:
			/* Set command values are in 1/4 microseconds */
			maestroCommandAllLegs(KNEE_SERVOS, MAESTRO_SET_TARGET, (commandedMotion.servoSequence[i].knee[sequenceStep] * 4));
			maestroControlStep = SET_HIPS;
			break;

		case SET_HIPS:
			/* Set command values are in 1/4 microseconds */
			maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_TARGET, (commandedMotion.servoSequence[i].hip[sequenceStep] * 4));
			sequenceStep++;
			maestroControlStep = WAIT_FOR_STOP;
			break;

		case WAIT_FOR_STOP:
			maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_SPEED, commandedMotion.walkingSpeed);

			state = maestroGetState();
			if((sequenceStep >= NUM_SEQ_STEPS) && (state == 0x00)){
				maestroControlStep = SEQUENCE_FINISHED;
			}
			else if(state == 0x00){
				maestroControlStep = SET_KNEES;
			}
			break;
	}
}

/* Helpers for external tasks */
servoControlSteps_t maestro_checkUpdateStatus(void){
	return maestroControlStep;
}

void maestro_runSequence(void){
	if(maestroControlStep == SEQUENCE_FINISHED){
		sequenceStep = 0;
		maestroControlStep = NEW_SEQUENCE_READY;
	}
}

void maestro_setMotion(void *sequence, uint16_t speed){
	commandedMotion.servoSequence = (legPositions_t*)sequence;
	commandedMotion.walkingSpeed = speed;
}


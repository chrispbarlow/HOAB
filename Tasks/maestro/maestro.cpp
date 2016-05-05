/*
 * maestro.cpp
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include <Tasks.h>
#include "../motionControl/motionControl.h"

#include "maestro.h"

#define HIP_SERVOS	(0)
#define KNEE_SERVOS	(NUM_LEGS)

MaestroPlugin maestro;

void maestro_init(void){
	maestro.init();
}

void maestro_update(void){
	maestro.update();
}

TaskPlugin Maestro("Servo Controller", maestro_init, maestro_update, 100);


/* Use these values to tune the position of each joint */
legPositions_t servoTuningValues = {
	{0, 0, 0, 0, 0, 0},	/* Hips */
	{0, 0, 0, 0, 0, 0}	/* Knees */
};


void MaestroPlugin::maestroCommandLeg(uint8_t servo, uint8_t cmd, uint16_t value){
	Serial.write(cmd);
	Serial.write(servo);
	Serial.write(value & 0x7F);
	Serial.write((value >> 7) & 0x7F);
	Serial.read();
}

void MaestroPlugin::maestroCommandAllLegs(uint8_t offset, uint8_t cmd, uint16_t value){
	uint8_t i;
	for(i = 0; i < NUM_LEGS; i++){
		maestroCommandLeg(i+offset,cmd,value);
	}
}

uint8_t MaestroPlugin::maestroGetState(void){
	uint8_t state = 0xFF;

	Serial.write(MAESTRO_GET_STATE);
	state = Serial.read();

	return state;
}

void MaestroPlugin::init(void){
	int i;
	Serial.write(0xA1);
	Serial.read();

	maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_SPEED, HIP_BASE_SPEED);
	maestroCommandAllLegs(KNEE_SERVOS, MAESTRO_SET_SPEED, KNEE_BASE_SPEED);

	maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_ACCEL, 50);
	maestroCommandAllLegs(KNEE_SERVOS, MAESTRO_SET_ACCEL, 100);

	maestroControlStep = SEQUENCE_FINISHED;
}


void MaestroPlugin::update(void){
	int i, tunedValue;
	uint8_t state;

	switch(maestroControlStep){
		default:
		case SEQUENCE_FINISHED:
			/* Do nothing, wait for new sequence */
			break;

		case SET_KNEES:
			/* Set command values are in 1/4 microseconds */
			for(i = 0; i < NUM_LEGS; i++){
				tunedValue = ((commandedMotion.servoSequence[sequenceStep].knee[i] * 4) + servoTuningValues.knee[i]);
				if(tunedValue < 0){
					tunedValue = 0;
				}
				maestroCommandLeg((KNEE_SERVOS+i), MAESTRO_SET_TARGET, (uint16_t)tunedValue);
			}
			maestroControlStep = SET_HIPS;
			break;

		case SET_HIPS:
			/* Set command values are in 1/4 microseconds */
			for(i = 0; i < NUM_LEGS; i++){
				tunedValue = ((commandedMotion.servoSequence[sequenceStep].hip[i] * 4) + servoTuningValues.hip[i]);
				if(tunedValue < 0){
					tunedValue = 0;
				}
				maestroCommandLeg((HIP_SERVOS+i), MAESTRO_SET_TARGET, (uint16_t)tunedValue);
			}
			maestroControlStep = WAIT_FOR_STOP;
			break;

		case WAIT_FOR_STOP:
			maestroCommandAllLegs(HIP_SERVOS, MAESTRO_SET_SPEED, commandedMotion.walkingSpeed);

			state = maestroGetState();
			if((sequenceStep >= (NUM_SEQ_STEPS-1)) && (state == 0x00)){
				maestroControlStep = SEQUENCE_FINISHED;
			}
			else if(state == 0x00){
				sequenceStep++;
				maestroControlStep = SET_KNEES;
			}
			break;
	}
}

/* Helpers for external tasks */
servoControlSteps_t MaestroPlugin::checkUpdateStatus(void){
	return maestroControlStep;
}

void MaestroPlugin::startNewSequence(void *sequence){
	if(maestroControlStep == SEQUENCE_FINISHED){
		commandedMotion.servoSequence = (legPositions_t*)sequence;
		sequenceStep = 0;
		maestroControlStep = SET_KNEES;
	}
	else{
		/* TODO: sequence queueing? */
	}
}

void MaestroPlugin::setWalkingSpeed(uint16_t speed){
	commandedMotion.walkingSpeed = speed;
}


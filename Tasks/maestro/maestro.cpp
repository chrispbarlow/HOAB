/*
 * maestro.cpp
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include <Tasks.h>
#include "../motionControl/motionControl.h"

#include "maestro.h"


MaestroPlugin maestro;

/* Private member variables */
servoControlSteps_t MaestroPlugin::maestroControlStep;
uint16_t MaestroPlugin::stepCount;
uint16_t MaestroPlugin::servoSpeeds[NUM_SERVOS];
uint16_t MaestroPlugin::servoAccels[NUM_SERVOS];
uint16_t MaestroPlugin::sequenceStep;
uint16_t MaestroPlugin::servoNum;
int16_t *MaestroPlugin::servoSequence;
int16_t *MaestroPlugin::servoTuningValues;

/* Public member functions */
void MaestroPlugin::init(void){
	int i;
	Serial.write(0xA1);
	Serial.read();

	for(i = 0; i < NUM_SERVOS; i++){
		servoSpeeds[i] = 0;
		servoAccels[i] = 0;
	}
	sequenceStep = 0;
	servoNum = 0;
	maestroControlStep = SEQUENCE_FINISHED;
}

servoControlSteps_t MaestroPlugin::getUpdateStatus(void){
	return maestroControlStep;
}

void MaestroPlugin::startNewSequence(int16_t *sequence, uint16_t count){
	if(maestroControlStep == SEQUENCE_FINISHED){
		servoSequence = sequence;
		stepCount = count;

		sequenceStep = 0;
		maestroControlStep = SENDING_SEQUENCE;
	}
	else{
		/* TODO: sequence queueing? */
	}
}

void MaestroPlugin::setSpeeds(uint16_t speeds[]){
	static bool speedSet = false;
	int i;

	if(speedSet == false){
		for(i = 0; i < NUM_SERVOS; i++){
			servoSpeeds[i] = 0xFFFF;
		}
		speedSet = true;
	}

	for(i = 0; i < NUM_SERVOS; i++){
		if(speeds[i] != servoSpeeds[i]){
			maestroCommandLeg(i, MAESTRO_SET_SPEED, speeds[i]);
			servoSpeeds[i] = speeds[i];
		}
	}
}

void MaestroPlugin::setAccelerations(uint16_t accels[]){
	static bool accelSet = false;
	int i;

	if(accelSet == false){
		for(i = 0; i < NUM_SERVOS; i++){
			servoAccels[i] = 0xFFFF;
		}
		accelSet = true;
	}

	for(i = 0; i < NUM_SERVOS; i++){
		if(accels[i] != servoAccels[i]){
			maestroCommandLeg(i, MAESTRO_SET_ACCEL, accels[i]);
			servoAccels[i] = accels[i];
		}
	}
}

void MaestroPlugin::setServoTuning(int16_t *tuningValues){
	servoTuningValues = tuningValues;
}



/* Private member functions */
uint16_t MaestroPlugin::tunedPosition(int16_t positionValue, int16_t tuningValue){
	int16_t tunedValue;

	if(positionValue == 0){
		tunedValue = 0;
	}
	else{
		tunedValue = (positionValue + tuningValue);
		tunedValue *= 4;	/* Set command values are in 1/4 microseconds */
		if(tunedValue <= 0){
			tunedValue = 1;
		}
		else if(tunedValue > MAESTRO_TWOBYTE_MAX){
			tunedValue = MAESTRO_TWOBYTE_MAX;
		}
	}

	return (uint16_t)tunedValue;
}

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

void MaestroPlugin::pluginUpdate(void){
	int sequencePosition = 0;
	uint8_t state;

	switch(maestroControlStep){
		default:
		case SEQUENCE_FINISHED:
			/* Do nothing, wait for new sequence */
			break;

		case SENDING_SEQUENCE:
			sequencePosition = servoNum + (sequenceStep*NUM_SERVOS);
			maestroCommandLeg(servoNum, MAESTRO_SET_TARGET, tunedPosition(servoSequence[sequencePosition],servoTuningValues[servoNum]));

			if(++servoNum >= NUM_SERVOS){
				maestroControlStep = WAIT_FOR_STOP;
				servoNum = 0;
			}
			break;

		case WAIT_FOR_STOP:

#ifdef PRETEND_TO_BE_STOPPED
#warning "test mode won't wait for servos to stop"
			state = 0;
#else
			state = maestroGetState();
#endif

			if((state == 0x00) && (++sequenceStep >= stepCount)){
				sequenceStep = 0;
				maestroControlStep = SEQUENCE_FINISHED;
			}
			else if(state == 0x00){
				maestroControlStep = SENDING_SEQUENCE;
			}

			break;
	}
}

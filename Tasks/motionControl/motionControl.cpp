/*
 * motionControl.cpp
 *      Author: chris.barlow
 */
#include <arduino.h>
#include <plugins/maestro.h>

#include "motionControl.h"
#include "controlScripts.h"

#include "../proximitySensing/proximitySensing.h"


/* Array to run the sequence from */
legPositions_t sequenceLegRun[NUM_SEQ_STEPS];

/* Directions */
directions_t directionOffset_G;
volatile movement_t movement_G;

uint16_t speeds[NUM_SERVOS];
uint16_t accels[NUM_SERVOS];
int16_t servoTuning[NUM_SERVOS] = {0,0,0,0,0,0,0,0,0,0,0,0};

void motionControl_Init(void){
	int i;
	directionOffset_G = DIR_A;

	maestro.init();

	for(i = 0; i < NUM_LEGS; i++){
		speeds[HIP_SERVOS+i] = HIP_BASE_SPEED;
		accels[HIP_SERVOS+i] = HIP_ACCELERATION;
		speeds[KNEE_SERVOS+i] = KNEE_BASE_SPEED;
		accels[KNEE_SERVOS+i] = KNEE_ACCELERATION;
	}

	maestro.setSpeeds(speeds);
	maestro.setAccelerations(accels);
	maestro.setServoTuning(servoTuning);
	maestro.startNewSequence((int16_t*)resetLegScript, 7);
	movement_G = STOP;
}


void motionControl_update(void){
	int legNum, step, offsetLegNum, proximity, i;
	uint16_t newWalkingSpeed;
	servoControlSteps_t maestroStatus;

	proximity = proximity_getAverage();
	maestroStatus = maestro.getUpdateStatus();

	switch(movement_G){
	default:
	case RESET:
		/* Do nothing */
		break;

	case STOP:
	// Serial.print("S");
		if((maestroStatus == SEQUENCE_FINISHED) && ((proximity < OBJECT_TOO_CLOSE-STOP_DISTANCE)||(proximity > OBJECT_REALLY_CLOSE+STOP_DISTANCE))){
			movement_G = WALK;
		}
		break;

	case WALK:
	// Serial.print("W");
		/* Simple speed control - calculate forwards speed based on proximity, stop if too close and reverse if really close */
		if(proximity <= OBJECT_TOO_CLOSE){
			directionOffset_G = DIR_A;

			newWalkingSpeed = (HIP_BASE_SPEED - (proximity*(HIP_BASE_SPEED/100)));
			if(newWalkingSpeed <= 0){
				newWalkingSpeed = 1;
			}

			for(i = 0; i < 6; i++){
				speeds[i] = newWalkingSpeed;
			}
			maestro.setSpeeds(speeds);
		}
		else if(proximity <= OBJECT_REALLY_CLOSE){
				for(i = 0; i < 6; i++){
					speeds[i] = HIP_BASE_SPEED;
				}
				maestro.setSpeeds(speeds);
				maestro.startNewSequence((int16_t*)resetLegScript, 7);
				movement_G = STOP;
		}
		else{
			directionOffset_G = DIR_D;

			for(i = 0; i < 6; i++){
				speeds[i] = HIP_BASE_SPEED;
			}
			maestro.setSpeeds(speeds);
		}

		if((movement_G != STOP)&&(maestroStatus == SEQUENCE_FINISHED)){
			for(legNum = 0; legNum < NUM_LEGS; legNum++){

				/* apply offset */
				offsetLegNum = legNum + (int)directionOffset_G;

				/* wrap-around legs (hehe) */
				if(offsetLegNum >= NUM_LEGS){
					offsetLegNum -= NUM_LEGS;
				}

				/* load sequence values into 'run' array */
				for(step = 0; step < NUM_SEQ_STEPS; step++){
					Serial.println(step);
					sequenceLegRun[step].hip[legNum] = sequenceLegScript[step].hip[offsetLegNum];
					sequenceLegRun[step].knee[legNum] = sequenceLegScript[step].knee[offsetLegNum];
				}
			}

			maestro.startNewSequence((int16_t*)sequenceLegRun, NUM_SEQ_STEPS);
		}
		break;

	case ROTATE_L:
		// maestro.startNewSequence((int16_t*)rotateLeftScript, NUM_SEQ_STEPS);
		break;

	case ROTATE_R:
		// maestro.startNewSequence((int16_t*)rotateRightScript, NUM_SEQ_STEPS);
		break;
	}

}

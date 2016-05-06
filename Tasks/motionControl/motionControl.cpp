/*
 * motionControl.cpp
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "motionControl.h"
#include "controlScripts.h"

#include "../maestro/maestro.h"
#include "../proximitySensing/proximitySensing.h"


/* Array to run the sequence from */
legPositions_t sequenceLegRun[NUM_LEGS];

/* Directions */
directions_t directionOffset_G;
volatile movement_t movement_G;

uint16_t speeds[NUM_SERVOS];
uint16_t accels[NUM_SERVOS];
int16_t servoTuning[NUM_SERVOS] = {0,2,-2,5,-5,0,0,2,-2,5,-5,0};

void motionControl_Init(void){
	int i;
	movement_G = WALK;
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
	maestro.startNewSequence((int16_t*)resetLegScript, NUM_SEQ_STEPS);
}


void motionControl_update(void){
	int legNum, step, offsetLegNum, proximity, i;
	uint16_t newWalkingSpeed;

	proximity = proximity_getAverage();

	/* Simple speed control - calculate forwards speed based on proximity, stop if too close and reverse if really close */
	if(proximity <= OBJECT_TOO_CLOSE){
		movement_G = WALK;
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
		movement_G = STOP;
	}
	else{
		movement_G = WALK;
		directionOffset_G = DIR_D;

		for(i = 0; i < 6; i++){
			speeds[i] = HIP_BASE_SPEED;
		}

		maestro.setSpeeds(speeds);
	}

	if(maestro.checkUpdateStatus() == SEQUENCE_FINISHED){
		switch(movement_G){
		default:
		case STOP:
		case RESET:
			/* Do nothing */
			break;

		case WALK:
			for(legNum = 0; legNum < NUM_LEGS; legNum++){

				/* apply offset */
				offsetLegNum = legNum + (int)directionOffset_G;

				/* wrap-around legs (hehe) */
				if(offsetLegNum >= NUM_LEGS){
					offsetLegNum -= NUM_LEGS;
				}

				/* load sequence values into 'run' array */
				for(step = 0; step < NUM_SEQ_STEPS; step++){
					sequenceLegRun[step].hip[legNum] = sequenceLegScript[step].hip[offsetLegNum];
					sequenceLegRun[step].knee[legNum] = sequenceLegScript[step].knee[offsetLegNum];
				}
			}

			maestro.startNewSequence((int16_t*)sequenceLegRun, NUM_SEQ_STEPS);
			break;

		case ROTATE_L:
			maestro.startNewSequence((int16_t*)rotateLeftScript, NUM_SEQ_STEPS);
			break;

		case ROTATE_R:
			maestro.startNewSequence((int16_t*)rotateRightScript, NUM_SEQ_STEPS);
			break;
		}
	}

}



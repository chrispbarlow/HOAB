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


void motionControl_Init(void){
	movement_G = WALK;
	directionOffset_G = DIR_A;

	maestro_startNewSequence((void*)resetLegScript);
}


void motionControl_update(void){
	int legNum, step, offsetLegNum, proximity;
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
		maestro_setWalkingSpeed(newWalkingSpeed);
	}
	else if(proximity <= OBJECT_REALLY_CLOSE){
		movement_G = STOP;
	}
	else{
		movement_G = WALK;
		directionOffset_G = DIR_D;
		maestro_setWalkingSpeed(HIP_BASE_SPEED);
	}

	if(maestro_checkUpdateStatus() == SEQUENCE_FINISHED){
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

			maestro_startNewSequence(sequenceLegRun);
			break;

		case ROTATE_L:
			maestro_startNewSequence((void*)rotateLeftScript);
			break;

		case ROTATE_R:
			maestro_startNewSequence((void*)rotateRightScript);
			break;
		}
	}

}



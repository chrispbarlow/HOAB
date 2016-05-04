/*
 * motionControl.cpp
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "motionControl.h"
#include "../maestro/maestro.h"

/* in proximitySensing.cpp */
extern volatile int proxReadings_G[];


/* Array of legs containing the movement sequence (direction A) */
legPositions_t resetLegScript[NUM_LEGS] = {
				/* Leg0 */      {
				/* Hip0 - servo0 */     {   0,    0,    0,    0,    0, 1495},
				/* Knee0- servo6 */     {2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg1 */      {
				/* Hip1 - servo1 */     {   0,    0, 1495, 1495, 1495, 1495},
				/* Knee1- servo7 */     {2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg2 */      {
				/* Hip2 - servo2 */     {   0,    0,    0,    0,    0, 1495},
				/* Knee2- servo8 */     {2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg3 */      {
				/* Hip3 - servo3 */     {   0,    0, 1495, 1495, 1495, 1495},
				/* Knee3- servo9 */     {2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg4 */      {
				/* Hip4 - servo4 */     {   0,    0,    0,    0,    0, 1495},
				/* Knee4- servo10 */    {2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg5 */      {
				/* Hip5 - servo5 */     {   0,    0, 1495, 1495, 1495, 1495},
				/* Knee4- servo11 */    {2000,  992,  992, 2000, 2000, 2000}
								}
							};

/* Array of legs containing the movement sequence (direction A) */
const legPositions_t sequenceLegScript[NUM_LEGS] =  {
				/* Leg0 */      {
				/* Hip0 - servo0 */     {1240, 1240, 1240, 2000, 2000, 2000},
				/* Knee0- servo6 */     { 992, 2000, 2000, 2000, 2000,  992}
								},
				/* Leg1 */      {
				/* Hip1 - servo1 */     {1920, 1920, 1920,  992,  992,  992},
				/* Knee1- servo7 */     {2000, 2000,  992,  992, 2000, 2000}
								},
				/* Leg2 */      {
				/* Hip2 - servo2 */     { 992,  992,  992, 1755, 1755, 1755},
				/* Knee2- servo8 */     { 992, 2000, 2000, 2000, 2000,  992}
								},
				/* Leg3 */      {
				/* Hip3 - servo3 */     {1255, 1255, 1255, 2000, 2000, 2000},
				/* Knee3- servo9 */     {2000, 2000,  992,  992, 2000, 2000}
								},
				/* Leg4 */      {
				/* Hip4 - servo4 */     {1920, 1920, 1920,  992,  992,  992},
				/* Knee4- servo10 */    { 992, 2000, 2000, 2000, 2000,  992}
								},
				/* Leg5 */      {
				/* Hip5 - servo5 */     { 992,  992,  992, 1760, 1760, 1760},
				/* Knee4- servo11 */    {2000, 2000,  992,  992, 2000, 2000}
								}
							};

/* Array to run the sequence from */
legPositions_t sequenceLegRun[NUM_LEGS];

/* Directions */
directions_t directionOffset_G;
volatile movement_t movement_G;
int newWalkingSpeed;


void motionControl_Init(void){
	maestro_setMotion(resetLegScript, HIP_BASE_SPEED);
	maestro_startNewSequence();

	movement_G = WALK;
	directionOffset_G = DIR_A;
}


void motionControl_update(void){
	int legNum, step, offsetLegNum;

	/* Simple speed control - calculate forwards speed based on proximity, stop if too close and reverse if really close */
	if(proxReadings_G[1] <= 100){
		movement_G = WALK;
		directionOffset_G = DIR_A;
		newWalkingSpeed = (HIP_BASE_SPEED - (proxReadings_G[1]*(HIP_BASE_SPEED/100)));
		if(newWalkingSpeed <= 0){
			newWalkingSpeed = 1;
		}
	}
	else if(proxReadings_G[1] <= 175){
		newWalkingSpeed = 0;
		movement_G = STOP;
	}
	else{
		movement_G = WALK;
		directionOffset_G = DIR_D;
		newWalkingSpeed = (HIP_BASE_SPEED);
	}

	switch(movement_G){
	default:
	case STOP:
	case RESET:
		/* Do nothing */
		break;

	case WALK:
		if(maestro_checkUpdateStatus() == SEQUENCE_FINISHED){
			for(legNum = 0; legNum < NUM_LEGS; legNum++){

				/* apply offset */
				offsetLegNum = legNum + (int)directionOffset_G;

				/* wrap-around legs (hehe) */
				if(offsetLegNum >= NUM_LEGS){
					offsetLegNum -= NUM_LEGS;
				}

				/* load sequence values into 'run' array */
				for(step = 0; step < NUM_SEQ_STEPS; step++){
					sequenceLegRun[legNum].hip[step] = sequenceLegScript[offsetLegNum].hip[step];
					sequenceLegRun[legNum].knee[step] = sequenceLegScript[offsetLegNum].knee[step];
				}
			}
		}

		break;

	case ROTATE_L:

		break;

	case ROTATE_R:

		break;
	}

	maestro_setMotion(sequenceLegRun,newWalkingSpeed);
	maestro_runSequence();

}



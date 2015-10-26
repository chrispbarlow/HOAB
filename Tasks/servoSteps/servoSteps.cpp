/*
 * servoSteps.cpp
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "servoSteps.h"

/* in proximitySensing.c */
extern int proxReadings_G[];

/* in maestroControl.c */
extern int runningSequenceNum_G;


/* Array of legs containing the movement sequence (direction A) */
const legPositions_t resetLegScript[NUM_LEGS] =	{
				/* Leg0 */		{
				/* Hip0 - servo0 */		{   0,    0, 1495, 1495, 1495, 1495},
				/* Knee0- servo6 */		{2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg1 */		{
				/* Hip1 - servo1 */		{   0,    0,    0,    0,    0, 1495},
				/* Knee1- servo7 */		{2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg2 */		{
				/* Hip2 - servo2 */		{   0,    0, 1495, 1495, 1495, 1495},
				/* Knee2- servo8 */		{2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg3 */		{
				/* Hip3 - servo3 */		{   0,    0,    0,    0,    0, 1495},
				/* Knee3- servo9 */		{2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg4 */		{
				/* Hip4 - servo4 */		{   0,    0, 1495, 1495, 1495, 1495},
				/* Knee4- servo10 */	{2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg5 */		{
				/* Hip5 - servo5 */		{   0,    0,    0,    0,    0, 1495},
				/* Knee4- servo11 */	{2000, 2000, 2000, 2000,  992,  992}
								}
							};

/* Array of legs containing the movement sequence (direction A) */
const legPositions_t sequenceLegScript[NUM_LEGS] =	{
				/* Leg0 */		{
				/* Hip0 - servo0 */		{1240, 1240, 2000, 2000, 2000, 1240},
				/* Knee0- servo6 */		{2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg1 */		{
				/* Hip1 - servo1 */		{1920, 1920,  992,  992,  992, 1920},
				/* Knee1- servo7 */		{2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg2 */		{
				/* Hip2 - servo2 */		{ 992,  992, 1755, 1755, 1755,  992},
				/* Knee2- servo8 */		{2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg3 */		{
				/* Hip3 - servo3 */		{1240, 1240, 2000, 2000, 2000, 1240},
				/* Knee3- servo9 */		{2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg4 */		{
				/* Hip4 - servo4 */		{1920, 1920,  992,  992,  992, 1920},
				/* Knee4- servo10 */	{2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg5 */		{
				/* Hip5 - servo5 */		{ 992,  992, 1755, 1755, 1755,  992},
				/* Knee4- servo11 */	{2000,  992,  992, 2000, 2000, 2000}
								}
							};

/* Array to run the sequence from */
legPositions_t sequenceLegRun_G[NUM_LEGS][2];

/* Directions */
directions_t directionOffset_G;
movement_t movement_G;

volatile void servoSteps_Init(void){
	movement_G = WALK;
	directionOffset_G = DIR_D;
}


volatile void servoSteps_update(void){
	int legNum, step, offsetLegNum;
	static int nextSequenceNum = 0;

	if(proxReadings_G[1] <= 50){
//		Serial.print("W ");
		movement_G = WALK;
		directionOffset_G = DIR_D;
	}
	else if(proxReadings_G[1] <= 100){
//		Serial.print("S ");
		movement_G = STOP;
	}
	else{
//		Serial.print("R ");
		movement_G = RETREAT;
		directionOffset_G = DIR_A;
	}

	nextSequenceNum = ((~runningSequenceNum_G) & 1);

	switch(movement_G){
	default:
	case STOP:

		break;

	case WALK:
	case RETREAT:
		for(legNum = 0; legNum < NUM_LEGS; legNum++){

			/* apply offset */
			offsetLegNum = legNum + (int)directionOffset_G;

			/* wrap-around legs (hehe) */
			if(offsetLegNum >= NUM_LEGS){
				offsetLegNum -= NUM_LEGS;
			}

			/* load sequence values into 'run' array */
			for(step = 0; step < NUM_SEQ_STEPS; step++){
				sequenceLegRun_G[legNum][nextSequenceNum].hip[step] = (sequenceLegScript[offsetLegNum].hip[step] * 4); /* Actual transmitted values are in 1/4 microseconds */
				sequenceLegRun_G[legNum][nextSequenceNum].knee[step] = (sequenceLegScript[offsetLegNum].knee[step] * 4);
			}
		}

		break;

	case ROTATE_L:

		break;

	case ROTATE_R:

		break;
	}

}



/*
 * servoSteps.cpp
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "servoSteps.h"

/* in proximitySensing.c */
extern volatile int proxReadings_G[];

/* in maestroControl.c */
extern volatile int usingSequence_G;


/* Array of legs containing the movement sequence (direction A) */
const legPositions_t resetLegScript[NUM_LEGS] =	{
				/* Leg0 */		{
				/* Hip0 - servo0 */		{   0,    0,    0,    0,    0, 1495},
				/* Knee0- servo6 */		{2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg1 */		{
				/* Hip1 - servo1 */		{   0,    0, 1495, 1495, 1495, 1495},
				/* Knee1- servo7 */		{2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg2 */		{
				/* Hip2 - servo2 */		{   0,    0,    0,    0,    0, 1495},
				/* Knee2- servo8 */		{2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg3 */		{
				/* Hip3 - servo3 */		{   0,    0, 1495, 1495, 1495, 1495},
				/* Knee3- servo9 */		{2000,  992,  992, 2000, 2000, 2000}
								},
				/* Leg4 */		{
				/* Hip4 - servo4 */		{   0,    0,    0,    0,    0, 1495},
				/* Knee4- servo10 */	{2000, 2000, 2000, 2000,  992,  992}
								},
				/* Leg5 */		{
				/* Hip5 - servo5 */		{   0,    0, 1495, 1495, 1495, 1495},
				/* Knee4- servo11 */	{2000,  992,  992, 2000, 2000, 2000}
								}
							};

/* Array of legs containing the movement sequence (direction A) */
const legPositions_t sequenceLegScript[NUM_LEGS] =	{
 				/* Leg0 */		{
				/* Hip0 - servo0 */		{1240, 1240, 1240, 2000, 2000, 2000},
				/* Knee0- servo6 */		{ 992, 2000, 2000, 2000, 2000,  992}
 								},
 				/* Leg1 */		{
				/* Hip1 - servo1 */		{1920, 1920, 1920,  992,  992,  992},
				/* Knee1- servo7 */		{2000, 2000,  992,  992, 2000, 2000}
 								},
 				/* Leg2 */		{
				/* Hip2 - servo2 */		{ 992,  992,  992, 1755, 1755, 1755},
				/* Knee2- servo8 */		{ 992, 2000, 2000, 2000, 2000,  992}
 								},
 				/* Leg3 */		{
				/* Hip3 - servo3 */		{1255, 1255, 1255, 2000, 2000, 2000},
				/* Knee3- servo9 */		{2000, 2000,  992,  992, 2000, 2000}
 								},
 				/* Leg4 */		{
				/* Hip4 - servo4 */		{1920, 1920, 1920,  992,  992,  992},
				/* Knee4- servo10 */	{ 992, 2000, 2000, 2000, 2000,  992}
 								},
 				/* Leg5 */		{
				/* Hip5 - servo5 */		{ 992,  992,  992, 1760, 1760, 1760},
				/* Knee4- servo11 */	{2000, 2000,  992,  992, 2000, 2000}
 								}
 							};

/* Array to run the sequence from */
volatile legPositions_t sequenceLegRun_G[NUM_LEGS];

/* Directions */
directions_t directionOffset_G;
volatile movement_t movement_G;

volatile void servoSteps_Init(void){
	int step, legNum;
	/* load reset sequence values into 'run' array */
//	for(legNum = 0; legNum < NUM_LEGS; legNum++){
//		for(step = 0; step < NUM_SEQ_STEPS; step++){
//			sequenceLegRun_G[legNum].hip[step] = (resetLegScript[legNum].hip[step] * 4); /* Actual transmitted values are in 1/4 microseconds */
//			sequenceLegRun_G[legNum].knee[step] = (resetLegScript[legNum].knee[step] * 4);
//		}
//	}

	movement_G = WALK;
	directionOffset_G = DIR_A;
}


volatile void servoSteps_update(void){
	int legNum, step, offsetLegNum;
	static int nextSequenceNum = 0;

//	if ((movement_G != RESET) && (usingSequence_G == false)){
	if(proxReadings_G[1] <= 50){
//		Serial.print("W ");
		movement_G = WALK;
		directionOffset_G = DIR_A;
	}
	else if(proxReadings_G[1] <= 100){
//		Serial.print("S ");
		movement_G = STOP;
	}
	else{
//		Serial.print("R ");
		movement_G = RETREAT;
		directionOffset_G = DIR_D;
	}


	switch(movement_G){
	default:
	case STOP:
	case RESET:

		break;

	case WALK:
	case RETREAT:
		if(usingSequence_G == false){
			for(legNum = 0; legNum < NUM_LEGS; legNum++){

				/* apply offset */
				offsetLegNum = legNum + (int)directionOffset_G;

				/* wrap-around legs (hehe) */
				if(offsetLegNum >= NUM_LEGS){
					offsetLegNum -= NUM_LEGS;
				}

				/* load sequence values into 'run' array */
				for(step = 0; step < NUM_SEQ_STEPS; step++){
					sequenceLegRun_G[legNum].hip[step] = (sequenceLegScript[offsetLegNum].hip[step] * 4); /* Actual transmitted values are in 1/4 microseconds */
					sequenceLegRun_G[legNum].knee[step] = (sequenceLegScript[offsetLegNum].knee[step] * 4);
				}
			}
			usingSequence_G = true;
		}

		break;

	case ROTATE_L:

		break;

	case ROTATE_R:

		break;
	}

}



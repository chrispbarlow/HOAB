/*
 * motionControl.cpp
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "motionControl.h"
#include "../maestro/maestro.h"
#include "../proximitySensing/proximitySensing.h"

#define OBJECT_TOO_CLOSE	(100)
#define OBJECT_REALLY_CLOSE	(175)

/* in proximitySensing.cpp */
extern sensorReadings_t proxReadings_G;


/* Array of legs containing the movement sequence (direction A) */
const legPositions_t resetLegScript[NUM_SEQ_STEPS] = {
				/* Step 0 */ 	{
				/* Hips */			{   0,    0,    0,    0,    0,    0},
				/* Knees */			{2000, 2000, 2000, 2000, 2000, 2000}
								},
				/* Step 1 */ 	{
				/* Hips */			{   0,    0,    0,    0,    0,    0},
				/* Knees */			{2000,  992, 2000,  992, 2000,  992}
								},
				/* Step 2 */ 	{
				/* Hips */			{   0, 1495,    0, 1495,    0, 1495},
				/* Knees */			{2000,  992, 2000,  992, 2000,  992}
								},
				/* Step 3 */ 	{
				/* Hips */			{   0, 1495,    0, 1495,    0, 1495},
				/* Knees */			{2000, 2000, 2000, 2000, 2000, 2000}
								},
				/* Step 4 */ 	{
				/* Hips */			{   0, 1495,    0, 1495,    0, 1495},
				/* Knees */			{ 992, 2000,  992, 2000,  992, 2000}
								},
				/* Step 5 */	{
				/* Hips */			{1495, 1495, 1495, 1495, 1495, 1495},
				/* Knees */			{ 992, 2000,  992, 2000,  992, 2000}
								},
						};

/* Array of legs containing the movement sequence (direction A) */
const legPositions_t sequenceLegScript[NUM_SEQ_STEPS] =  {
				/* Step 0 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{ 992, 2000,  992, 2000,  992, 2000}
								},
				/* Step 1 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{2000, 2000, 2000, 2000, 2000, 2000}
								},
				/* Step 2 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{2000,  992, 2000,  992, 2000,  992}
								},
				/* Step 3 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{2000,  992, 2000,  992, 2000,  992}
								},
				/* Step 4 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{2000, 2000, 2000, 2000, 2000, 2000}
								},
				/* Step 5 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{ 992, 2000,  992, 2000,  992, 2000}
								},
							};

/* TODO: Update these values */
const legPositions_t rotateLeftScript[NUM_SEQ_STEPS] =  {
				/* Step 0 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{ 992, 2000,  992, 2000,  992, 2000}
								},
				/* Step 1 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{2000, 2000, 2000, 2000, 2000, 2000}
								},
				/* Step 2 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{2000,  992, 2000,  992, 2000,  992}
								},
				/* Step 3 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{2000,  992, 2000,  992, 2000,  992}
								},
				/* Step 4 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{2000, 2000, 2000, 2000, 2000, 2000}
								},
				/* Step 5 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{ 992, 2000,  992, 2000,  992, 2000}
								},
							};

/* TODO: Update these values */
const legPositions_t rotateRightScript[NUM_SEQ_STEPS] =  {
				/* Step 0 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{ 992, 2000,  992, 2000,  992, 2000}
								},
				/* Step 1 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{2000, 2000, 2000, 2000, 2000, 2000}
								},
				/* Step 2 */	{
				/* Hips */			{1240, 1920,  992, 1255, 1920,  992},
				/* Knees */			{2000,  992, 2000,  992, 2000,  992}
								},
				/* Step 3 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{2000,  992, 2000,  992, 2000,  992}
								},
				/* Step 4 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{2000, 2000, 2000, 2000, 2000, 2000}
								},
				/* Step 5 */	{
				/* Hips */			{2000,  992, 1755, 2000,  992, 1760},
				/* Knees */			{ 992, 2000,  992, 2000,  992, 2000}
								},
							};

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
	int legNum, step, offsetLegNum;
	uint16_t newWalkingSpeed;

	/* Simple speed control - calculate forwards speed based on proximity, stop if too close and reverse if really close */
	if(proxReadings_G.average <= OBJECT_TOO_CLOSE){
		movement_G = WALK;
		directionOffset_G = DIR_A;
		newWalkingSpeed = (HIP_BASE_SPEED - (proxReadings_G.average*(HIP_BASE_SPEED/100)));
		if(newWalkingSpeed <= 0){
			newWalkingSpeed = 1;
		}
		maestro_setWalkingSpeed(newWalkingSpeed);
	}
	else if(proxReadings_G.average <= OBJECT_REALLY_CLOSE){
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



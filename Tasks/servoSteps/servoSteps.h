/*
 * servoSteps.h
 *
 *  Created on: 4 Apr 2014
 *      Author: chris.barlow
 */

#ifndef servoSteps_H_
#define servoSteps_H_

#define NUM_SEQ_STEPS 	(6)
#define NUM_LEGS	(6)

/* 6-step sequences for hips and knees */
typedef struct {
	int hip[NUM_SEQ_STEPS];
	int knee[NUM_SEQ_STEPS];
} legPositions_t;


typedef enum{DIR_A = 0, DIR_B = 1, DIR_C = 2, DIR_D = 3, DIR_E  = 4, DIR_F = 5}directions_t;
typedef enum{STOP, WALK, ROTATE_L, ROTATE_R, RETREAT}movement_t;

volatile void servoSteps_Init(void);
volatile void servoSteps_update(void);


#endif /* servoSteps_H_ */

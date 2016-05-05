/*
 * maestro.h
 *
 *  Created on: 4 Apr 2014
 *      Author: chris.barlow
 */

#ifndef maestro_H_
#define maestro_H_
#include <Tasks.h>
#include "../motionControl/motionControl.h"

#define MAESTRO_SET_TARGET			(0x84)
#define MAESTRO_SET_MULTI_TARGET	(0x9F)
#define MAESTRO_SET_SPEED			(0x87)
#define MAESTRO_SET_ACCEL			(0x89)
#define MAESTRO_GET_STATE			(0x93)

typedef enum {SEQUENCE_FINISHED, SET_KNEES, SET_HIPS, WAIT_FOR_STOP} servoControlSteps_t;

class MaestroPlugin{
public:
	void init(void);
	void update(void);
	servoControlSteps_t checkUpdateStatus(void);
	void startNewSequence(void *sequence);
	void setWalkingSpeed(uint16_t speed);

	MaestroPlugin() : pluginTask("Servo Controller", (task_function_t)&MaestroPlugin::init, (task_function_t)&MaestroPlugin::update, 100){}
	TaskPlugin pluginTask;

private:
	void maestroCommandLeg(uint8_t servo, uint8_t cmd, uint16_t value);
	void maestroCommandAllLegs(uint8_t offset, uint8_t cmd, uint16_t value);
	uint8_t maestroGetState(void);

	typedef struct{
  		legPositions_t* servoSequence;
  		uint16_t walkingSpeed;
	}motionParameters_t;
	motionParameters_t commandedMotion;

	servoControlSteps_t maestroControlStep;
	int sequenceStep;
};

extern MaestroPlugin maestro;

#endif /* maestro_H_ */

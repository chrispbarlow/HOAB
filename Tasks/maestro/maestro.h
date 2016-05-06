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

#define NUM_SERVOS					(12)

#define MAESTRO_SET_TARGET			(0x84)
#define MAESTRO_SET_MULTI_TARGET	(0x9F)
#define MAESTRO_SET_SPEED			(0x87)
#define MAESTRO_SET_ACCEL			(0x89)
#define MAESTRO_GET_STATE			(0x93)

#define PRETEND_TO_BE_STOPPED

typedef enum {SEQUENCE_FINISHED=0x0A, SENDING_SEQUENCE=0x0B, WAIT_FOR_STOP=0x0C} servoControlSteps_t;

class MaestroPlugin{
public:
	void init(void);
	void update(void);
	servoControlSteps_t checkUpdateStatus(void);
	void startNewSequence(uint16_t *sequence, uint16_t count);
	void setSpeeds(uint16_t speeds[]);
	void setAccelerations(uint16_t accels[]);
	MaestroPlugin() : pluginTask("Servo Controller", (task_function_t)&MaestroPlugin::update){}
	TaskPlugin pluginTask;
private:
	void maestroCommandLeg(uint8_t servo, uint8_t cmd, uint16_t value);
	void maestroCommandAllLegs(uint8_t offset, uint8_t cmd, uint16_t value);
	void compareSet(uint8_t cmd, uint16_t *newArray, uint16_t *oldArray, bool setflag);
	uint8_t maestroGetState(void);

	static servoControlSteps_t maestroControlStep;
	static uint16_t stepCount;
	static uint16_t *servoSequence;
	static uint16_t servoSpeeds[12];
	static uint16_t servoAccels[12];
	static uint16_t sequenceStep;
	static uint16_t servoNum;
};

extern MaestroPlugin maestro;

#endif /* maestro_H_ */

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

/* 1 bit of every byte is lost because the msb is always left clear */
#define MAESTRO_TWOBYTE_MAX			(0x3FFF)

// #define PRETEND_TO_BE_STOPPED

typedef enum {SEQUENCE_FINISHED=0x0A, SENDING_SEQUENCE=0x0B, WAIT_FOR_STOP=0x0C} servoControlSteps_t;

class MaestroPlugin{
public:
	void init(void);
	servoControlSteps_t getUpdateStatus(void);
	void startNewSequence(int16_t *sequence, uint16_t count);
	void setSpeeds(uint16_t speeds[]);
	void setAccelerations(uint16_t accels[]);
	void setServoTuning(int16_t *tuningValues);
	MaestroPlugin() : pluginTask("Servo Controller", (task_function_t)&MaestroPlugin::pluginUpdate){}
	TaskPlugin pluginTask;
private:
	uint16_t tunedPosition(int16_t positionValue, int16_t tuningValue);
	void maestroCommandLeg(uint8_t servo, uint8_t cmd, uint16_t value);
	void maestroCommandAllLegs(uint8_t offset, uint8_t cmd, uint16_t value);
	uint8_t maestroGetState(void);
	void pluginUpdate(void);

	static servoControlSteps_t maestroControlStep;
	static uint16_t stepCount;
	static uint16_t servoSpeeds[];
	static uint16_t servoAccels[];
	static uint16_t sequenceStep;
	static uint16_t servoNum;
	static int16_t *servoSequence;
	static int16_t *servoTuningValues;
};

extern MaestroPlugin maestro;

#endif /* maestro_H_ */

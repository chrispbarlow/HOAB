/*
 * proximitySensing.h
 *
 *      Author: chris.barlow
 */

#ifndef proximitySensing_H_
#define proximitySensing_H_

#define DEBUG_SENSORS


void proximitySensing_Init(void);
void proximitySensing_update(void);

typedef struct{
	int left;
	int right;
	int average;
}sensorReadings_t;

#endif /* proximitySensing_H_ */


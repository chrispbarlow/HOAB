/*
 * proximitySensing.h
 *
 *      Author: chris.barlow
 */

#ifndef proximitySensing_H_
#define proximitySensing_H_

#define OBJECT_TOO_CLOSE	(100)
#define OBJECT_REALLY_CLOSE	(175)
// #define DEBUG_SENSORS


void proximitySensing_Init(void);
void proximitySensing_update(void);

int proximity_getAverage(void);

#endif /* proximitySensing_H_ */


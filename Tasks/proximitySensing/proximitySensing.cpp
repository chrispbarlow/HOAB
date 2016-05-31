/*
 * proximitySensing.cpp
 *
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "proximitySensing.h"

typedef struct{
	int left;
	int right;
}sensorReadings_t;
sensorReadings_t proxReadings;

void proximitySensing_Init(void){
	proxReadings.left = 0;
	proxReadings.right = 0;
}


void proximitySensing_update(void){
	proxReadings.left = analogRead(A0);
	proxReadings.right = analogRead(A1);

#ifdef DEBUG_SENSORS
#warning "Don't debug sensors on the live robot"
	Serial.print("\n");
	Serial.print(proxReadings.left);
	Serial.print(",");
	Serial.print(proxReadings.right);
	Serial.print(",");
  Serial.println(proximity_getAverage());
#endif
}

int proximity_getAverage(void){
	return (proxReadings.left + proxReadings.right) / 2;
}

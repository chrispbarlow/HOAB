/*
 * proximitySensing.cpp
 *
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "proximitySensing.h"

sensorReadings_t proxReadings_G;

void proximitySensing_Init(void){
	proxReadings_G.left = 0;
	proxReadings_G.right = 0;
	proxReadings_G.average = 0;
}


void proximitySensing_update(void){
	proxReadings_G.left = analogRead(A0);
	proxReadings_G.right = analogRead(A1);

	proxReadings_G.average = (proxReadings_G.left + proxReadings_G.right) / 2;

#ifdef DEBUG_SENSORS
#warning "Don't debug sensors on the live robot"
	Serial.print(proxReadings_G.left);
	Serial.print(",");
	Serial.print(proxReadings_G.right);
	Serial.print(",");
	Serial.println(proxReadings_G.average);
#endif
}



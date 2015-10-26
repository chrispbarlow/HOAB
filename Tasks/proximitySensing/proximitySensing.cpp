/*
 * proximitySensing.cpp
 *
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "proximitySensing.h"

int proxReadings_G[3];

volatile void proximitySensing_Init(void){
	proxReadings_G[0] = 0;
	proxReadings_G[1] = 0;
	proxReadings_G[2] = 0;
}


volatile void proximitySensing_update(void){
	proxReadings_G[0] = analogRead(A0);
	proxReadings_G[2] = analogRead(A1);

	proxReadings_G[1] = (proxReadings_G[0] + proxReadings_G[2]) / 2;

//	Serial.println(proxReadings_G[1]);
}



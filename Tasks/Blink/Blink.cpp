/*
 * Blink.cpp
 *
 * A simple TT task example, replicating the classic Arduino 'Blink' project:
 * Turns on an LED on for one second, then off for one second, repeatedly.
 *
 * Works the same as:
 *
 * void loop() {
 *	 digitalWrite(13, HIGH);   // set the LED on
 * 	 delay(1000);              // wait for a second
 *	 digitalWrite(13, LOW);    // set the LED off
 * 	 delay(1000);              // wait for a second
 * }
 *
 * but allows other processing to happen during the delays.
 *
 *  Created on: 4 Apr 2014
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "../servoSteps/servoSteps.h"

/* In servoSteps.cpp */
extern movement_t movement_G;
extern directions_t directionOffset_G;

void blink_Init(void){
	pinMode(13, OUTPUT);
}


void blink_update(void){
	static uint16_t blinkCount = 0, pinState = LOW;
	uint16_t flashRate;
	// note, we don't use delay(), as this will block the task.
	// A counter is used to keep track of time, to allow other tasks to run in the mean time.

	if(directionOffset_G == DIR_D){
		pinState = HIGH;
		blinkCount = 0;
	}
	else {
		if(movement_G == STOP){
			flashRate = 10;
		}
		else{
			flashRate = 50;
		}

		if(++blinkCount >= flashRate){
			switch(pinState){
			case LOW:
				pinState = HIGH;	// set the LED on
				break;

			case HIGH:
				pinState = LOW;		// set the LED off
				break;

			default:
				break;
			}

			blinkCount = 0;			// reset counter
		}
	}

	digitalWrite(13, pinState);  // Write pinState to pin
}



/*
 * maestroControl.cpp
 *
 *      Author: chris.barlow
 */
#include <arduino.h>
#include "maestroControl.h"
#include "../servoSteps/servoSteps.h"

/* in servoSteps.c */
extern legPositions_t sequenceLegRun_G[];

int sequenceStep;

volatile void maestroControl_Init(void){
	Serial.begin(9600);
	sequenceStep = 0;
	Serial.write(MAESTRO_SET_TARGET);
	Serial.write(1);
	Serial.write(0);//(1600 & 0x7F);
	Serial.write(0);//((1600 >> 7) & 0x7F);
}


volatile void maestroControl_update(void){
	uint8_t i;
	char sendString[8];
	/* TODO: check if servos have stopped moving */

	for(i = 0; i < NUM_LEGS; i++){
		Serial.write(MAESTRO_SET_TARGET);
		Serial.write(i);
		Serial.write((sequenceLegRun_G[i].hip[sequenceStep] & 0x7F));
		Serial.write((sequenceLegRun_G[i].hip[sequenceStep] >> 7) & 0x7F);

		Serial.write(MAESTRO_SET_TARGET);
		Serial.write(i + NUM_LEGS);
		Serial.write(sequenceLegRun_G[i].knee[sequenceStep] & 0x7F);
		Serial.write((sequenceLegRun_G[i].knee[sequenceStep] >> 7) & 0x7F);
	}
	if(++sequenceStep >= NUM_SEQ_STEPS){
		sequenceStep = 0;
	}
}



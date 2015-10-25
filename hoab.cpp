// Do not remove the include below
#include "hoab.h"
#include "libTTduino/libTTduino.h"

#include "Tasks/Blink/Blink.h"
#include "Tasks/servoSteps/servoSteps.h"
#include "Tasks/proximitySensing/proximitySensing.h"
#include "Tasks/maestroControl/maestroControl.h"

//The setup function is called once at startup of the sketch
void setup(){
	Serial.begin(115200);
	TTduino_addTask(blink_Init, blink_update, 10, 0);
	TTduino_addTask(proximitySensing_Init, proximitySensing_update, 100, 1);
	TTduino_addTask(servoSteps_Init, servoSteps_update, 100, 2);
	TTduino_addTask(maestroControl_Init, maestroControl_update, 10, 5);
	TTduino_start(1000);
}

// The loop function is called in an endless loop
void loop(){
	TTduino_runScheduledTasks();
}

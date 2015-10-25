// Do not remove the include below
#include "hoab.h"
#include "libTTduino/libTTduino.h"

#include "Tasks/Blink/Blink.h"
#include "Tasks/servoSteps/servoSteps.h"

//The setup function is called once at startup of the sketch
void setup(){
	TTduino_addTask(blink_Init, blink_update, 500, 0);
	TTduino_addTask(servoSteps_Init, servoSteps_update, 500, 10);
	TTduino_start(1000);
}

// The loop function is called in an endless loop
void loop(){
	TTduino_runScheduledTasks();
}

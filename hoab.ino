/* Using Tasks for Arduino */
#include <Tasks.h>

/* Tasks are in self-contained folders for cleanliness */
#include "Tasks/Blink/Blink.h"
#include "Tasks/servoSteps/servoSteps.h"
#include "Tasks/proximitySensing/proximitySensing.h"
#include "Tasks/maestroControl/maestroControl.h"

/* Schedule configuration and initialisation */
void setup(){
  Schedule.begin(4);
  
	Serial.begin(115200);
  
  blink_Init();
	Schedule.addTask("blink", blink_update, 0, 10);

  proximitySensing_Init();
	Schedule.addTask("IR sensors", proximitySensing_update, 1, 500);

  servoSteps_Init();
	Schedule.addTask("Step scripting", servoSteps_update, 3, 10);

  maestroControl_Init();
	Schedule.addTask("Maestro controller", maestroControl_update, 4, 100);

  /* 1 ms ticks */
	Schedule.startTicks(1);
}

/* Run tasks according to the configured schedule */
void loop(){
	Schedule.runTasks();
}

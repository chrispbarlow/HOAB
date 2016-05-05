/* Using Tasks for Arduino */
#include <Tasks.h>

/* Tasks are in self-contained folders for cleanliness */
#include "Tasks/Blink/Blink.h"
#include "Tasks/motionControl/motionControl.h"
#include "Tasks/proximitySensing/proximitySensing.h"
#include "Tasks/maestro/maestro.h"

/* Schedule configuration and initialisation */
void setup(){
  Schedule.begin(4);
  
	Serial.begin(115200);
  Serial.println("Hello");
  
  blink_Init();
	Schedule.addTask("blink", blink_update, 0, 10);
  Serial.print(Schedule.lastAddedTask());

  proximitySensing_Init();
	Schedule.addTask("IR sensors", proximitySensing_update, 1, 500);
  Serial.print(Schedule.lastAddedTask());

  Schedule.addPlugin(maestro.pluginTask, 4);
  Serial.print(Schedule.lastAddedTask());
  
  motionControl_Init();
	Schedule.addTask("Motion control", motionControl_update, 3, 100);
  Serial.println(Schedule.lastAddedTask());

  /* 1 ms ticks */
	Schedule.startTicks(1);
}

/* Run tasks according to the configured schedule */
void loop(){
	Schedule.runTasks();
}

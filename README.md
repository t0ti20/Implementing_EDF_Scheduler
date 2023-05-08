# Earliest Deadline First (EDF) Scheduler Implementation #
Contained in this repository is my implementation of the EDF Scheduler on FreeRTOS. 
In order to utilize this implementation, you must substitute the tasks.c and task.h files in the FreeRTOS source code with the ones provided in this repository. 
Additionally, a new configuration parameter `${configUSE_EDF_SCHEDULER}` must be added to the FreeRTOSConfig.h configuration header and set to 1 to activate the EDF Scheduler.
To create new tasks under the EDF Scheduler, a new function `${xTaskCreatePeriodic}` has been implemented in tasks.c .

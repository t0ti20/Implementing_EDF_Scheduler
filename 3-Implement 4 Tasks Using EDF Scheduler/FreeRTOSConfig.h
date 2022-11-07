#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H
#include <lpc21xx.h>
/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE. 
 *
 * See http://www.freertos.org/a00110.html
 *----------------------------------------------------------*/
#define configUSE_PREEMPTION							1
#define configUSE_TIME_SLICING						0
#define configUSE_IDLE_HOOK							0
#define configUSE_TICK_HOOK							1
#define configCPU_CLOCK_HZ							((unsigned long)60000000)
#define configTICK_RATE_HZ							((TickType_t)1000)
#define configMAX_PRIORITIES							(4)
#define configMINIMAL_STACK_SIZE						((unsigned short)90)
#define configTOTAL_HEAP_SIZE							((size_t)13*1024)
#define configMAX_TASK_NAME_LEN						(8)
#define configUSE_TRACE_FACILITY						0
#define configUSE_16_BIT_TICKS						0
#define configIDLE_SHOULD_YIELD						1
#define configQUEUE_REGISTRY_SIZE 						0
#define configUSE_APPLICATION_TASK_TAG					1
/* EDF */
#define configUSE_EDF_SCHEDULER						1
#define configIDLE_PERIOD							(300)
/* Co-routine definitions. */
#define configUSE_CO_ROUTINES 						0
#define configMAX_CO_ROUTINE_PRIORITIES 				2
/* STATS */
/*
#define configGENERATE_RUN_TIME_STATS					0
#define configUSE_STATS_FORMATTING_FUNCTIONS				0
#define portCONFIGURE_TIMER_FOR_RUN_TIME_STATS()
#define portGET_RUN_TIME_COUNTER_VALUE()				T1TC
*/
/* TRACE HOOKS */
/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */
#define INCLUDE_xTaskGetIdleTaskHandle					1
#define INCLUDE_xTaskGetCurrentTaskHandle 				1
#define INCLUDE_vTaskPrioritySet						0
#define INCLUDE_uxTaskPriorityGet						0
#define INCLUDE_vTaskDelete							1
#define INCLUDE_vTaskCleanUpResources					0
#define INCLUDE_vTaskSuspend							1
#define INCLUDE_vTaskDelayUntil						1
#define INCLUDE_vTaskDelay							1
#define INCLUDE_xSemaphoreGetMutexHolder 				1
/* Trace Program */
#define traceTASK_SWITCHED_IN()			switch((int)(pxCurrentTCB->pxTaskTag))													\
									{																				\
										case(0) :GPIO_write(PORT_0,PIN1,PIN_IS_HIGH);break;									\
										case(18):GPIO_write(PORT_0,PIN2,PIN_IS_HIGH);break;									\
										case(19):GPIO_write(PORT_0,PIN3,PIN_IS_HIGH);break;									\
										case(20):GPIO_write(PORT_0,PIN4,PIN_IS_HIGH);break;									\
										case(21):GPIO_write(PORT_0,PIN5,PIN_IS_HIGH);break;									\
										case(22):GPIO_write(PORT_0,PIN6,PIN_IS_HIGH);break;									\
										case(23):GPIO_write(PORT_0,PIN7,PIN_IS_HIGH);break;									\
									}
#define traceTASK_SWITCHED_OUT()			do																				\
									{																				\
										GPIO_write(PORT_0,PIN1,PIN_IS_LOW);												\
										GPIO_write(PORT_0,PIN3,PIN_IS_LOW);												\
										GPIO_write(PORT_0,PIN2,PIN_IS_LOW);												\
										GPIO_write(PORT_0,PIN4,PIN_IS_LOW);												\
										GPIO_write(PORT_0,PIN5,PIN_IS_LOW);												\
										GPIO_write(PORT_0,PIN6,PIN_IS_LOW);												\
										GPIO_write(PORT_0,PIN7,PIN_IS_LOW);												\
									}while(0)	
#endif /* FREERTOS_CONFIG_H */

/* Standard includes. */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "lpc21xx.h"
#include "serial.h"
#include "GPIO.h"
/*-----------------------------------------------------------*/
#define mainBUS_CLK_FULL	( ( unsigned char ) 0x01 )
#define mainCOM_TEST_BAUD_RATE	( ( unsigned long ) 115200 )
/*-----------------------------------------------------------*/
static void prvSetupHardware( void );
/*-----------------------------------------------------------*/
TaskHandle_t Pin_18_Handler =NULL;			 /* Button_1_Monitor */
QueueHandle_t Queue_1 = NULL;
void Pin_18(void * pvParameters)
{
	signed char Edge_Flag;
	pinState_t Current_State;
	pinState_t Previous_State=GPIO_read(PORT_1,PIN0);
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL,(void*)18);
	for(;;)
	{
		Current_State=GPIO_read(PORT_1,PIN0);
		if(Current_State>Previous_State)Edge_Flag='+';
		else if(Current_State<Previous_State)Edge_Flag='-';
		else Edge_Flag='.';
		xQueueOverwrite(Queue_1,&Edge_Flag);
		Previous_State = Current_State;
		vTaskDelayUntil(&xLastWakeTime,50);
	}
}
/*-----------------------------------------------------------*/
TaskHandle_t Pin_19_Handler =NULL;			 /* Button_2_Monitor */
QueueHandle_t Queue_2 = NULL;
void Pin_19(void * pvParameters)
{
	signed char Edge_Flag;
	pinState_t Current_State;
	pinState_t Previous_State=GPIO_read(PORT_1,PIN1);
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL,(void*)19);
	for(;;)
	{
		Current_State=GPIO_read(PORT_1,PIN1);
		if(Current_State>Previous_State)Edge_Flag='+';
		else if(Current_State<Previous_State)Edge_Flag='-';
		else Edge_Flag='.';
		xQueueOverwrite(Queue_2,&Edge_Flag);
		Previous_State = Current_State;
		vTaskDelayUntil(&xLastWakeTime,50);
	}
}
/*-----------------------------------------------------------*/
TaskHandle_t Pin_20_Handler =NULL;	 /* Periodic_Transmitter */
QueueHandle_t Queue_3 = NULL;
void Pin_20(void * pvParameters)
{
	unsigned char Counter;
	char String[15]="--------\n";
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag( NULL,(void*)20);
	for(;;)
	{
		for(Counter=0;Counter<15;Counter++)xQueueSend(Queue_3,String+Counter,100);
		vTaskDelayUntil(&xLastWakeTime,100);
	}
}
/*-----------------------------------------------------------*/
TaskHandle_t Pin_21_Handler =NULL;	 			/* Send_State_UART */
void Pin_21(void * pvParameters)
{
	signed char Button,Counter;
	char String[15];
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL,(void*)21);
	for(;;)
	{	
		xQueueReceive(Queue_1,&Button,0);
		xSerialPutChar('-');
		xSerialPutChar('B');
		xSerialPutChar('1');
		xSerialPutChar(' ');
		xSerialPutChar(':');
		xSerialPutChar(' ');
		xSerialPutChar(Button);
		xSerialPutChar('\n');
		/* ------------------- */
		xQueueReceive(Queue_2,&Button,0);
		xSerialPutChar('-');
		xSerialPutChar('B');
		xSerialPutChar('2');
		xSerialPutChar(' ');
		xSerialPutChar(':');
		xSerialPutChar(' ');
		xSerialPutChar(Button);
		xSerialPutChar('\n');
		/* ------------------- */
		if(uxQueueMessagesWaiting(Queue_3)!=0)
		{
			for(Counter=0;Counter<15;Counter++)xQueueReceive(Queue_3,(String+Counter),0);
			vSerialPutString((signed char*)String,strlen(String));
			xQueueReset(Queue_3);
		}
		vTaskDelayUntil(&xLastWakeTime,20);
	}
}
/*-----------------------------------------------------------*/
TaskHandle_t Pin_22_Handler =NULL;			  			/* Load_5_MS */
void Pin_22(void * pvParameters)
{
	unsigned int Counter;
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL,(void*)22);
	for(;;)
	{	
		for(Counter=0;Counter<=37269;Counter++){;}
		vTaskDelayUntil(&xLastWakeTime,10);
	}
}
/*-----------------------------------------------------------*/
TaskHandle_t Pin_23_Handler =NULL;						 /* Load_12_MS */
void Pin_23(void * pvParameters)
{
	unsigned int Counter;
	TickType_t xLastWakeTime=xTaskGetTickCount();
	vTaskSetApplicationTaskTag(NULL,(void*)23);
	for(;;)
	{	
		for(Counter=0;Counter<=89445;Counter++){;}
		vTaskDelayUntil(&xLastWakeTime,100);
	}
}
/*-----------------------------------------------------------*/
int main( void )
{
	prvSetupHardware();
	Queue_1=xQueueCreate(1,sizeof(char));
	Queue_2=xQueueCreate(1,sizeof(char));
	Queue_3=xQueueCreate(15,sizeof(char));
  /* ------------------- */
	xTaskCreate(Pin_18,"Button_1_Monitor",100,NULL,1,50,&Pin_18_Handler);					//{Periodicity: 50 , Deadline: 50 }
	xTaskCreate(Pin_19,"Button_2_Monitor",100,NULL,1,50,&Pin_19_Handler);					//{Periodicity: 50 , Deadline: 50 } 
	xTaskCreate(Pin_20,"Periodic_Transmitter",100,NULL,1,100,&Pin_20_Handler);		//{Periodicity: 100, Deadline: 100}
	xTaskCreate(Pin_21,"Uart_Receiver",100,NULL,1,20,&Pin_20_Handler);						//{Periodicity: 20 , Deadline: 20 }
	xTaskCreate(Pin_22,"Load_1_Simulation",100,NULL,1,10,&Pin_22_Handler);				//{Periodicity: 10 , Deadline: 10, Execution time: 5ms}
	//xTaskCreate(Pin_23,"Load_2_Simulation",100,NULL,1,100,&Pin_23_Handler);				//{Periodicity: 20 , Deadline: 20, Execution time: 12ms}
	/* ------------------- */	
	vTaskStartScheduler();
	for(;;);
}
/*-----------------------------------------------------------*/
void vApplicationTickHook()
{
	GPIO_write(PORT_0,PIN0,PIN_IS_HIGH);
	GPIO_write(PORT_0,PIN0,PIN_IS_LOW);
}
/*-----------------------------------------------------------*/
void timer1Reset(void)
{
	T1TCR |= 0x2;
	T1TCR &= ~0x2;
}
/* ------------------- */	
void configTimer1(void)
{
	T1PR = 1000;
	T1TCR |= 0x1;
}
/* ------------------- */	
static void prvSetupHardware( void )
{
	xSerialPortInitMinimal(mainCOM_TEST_BAUD_RATE);
	GPIO_init();
	configTimer1();
	VPBDIV = mainBUS_CLK_FULL;
}


/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*========================================*/
/********** CONFIGURE .H FILE  ************/
/*========================================*/
#if ( configSUPPORT_DYNAMIC_ALLOCATION == 1 )
    BaseType_t xTaskCreate
                         (
                              /* ----------------------- */
                              #if (configUSE_EDF_SCHEDULER==1)
                              {
                                   TickType_t Period,
                              }
                              /* ----------------------- */
                              #endif
                         ) PRIVILEGED_FUNCTION;
#endif
/*========================================*/
/************* EDF READYLIST  *************/
/*========================================*/
#if (configUSE_EDF_SCHEDULER==1)
PRIVILEGED_DATA static List_t xReadyTasksListEDF;
#endif
/*========================================*/
/********* INITIALIZE  READYLIST  *********/
/*========================================*/
static void prvInitialiseTaskLists( void )
{
     /* ----------------------- */
     #if (configUSE_EDF_SCHEDULER==1)
	{
		vListInitialise(&xReadyTasksListEDF);
	}
	#endif
     /* ----------------------- */
}
/*========================================*/
/******** ADD TASKS TO  READYLIST  ********/
/*========================================*/
#if (configUSE_EDF_SCHEDULER==1)
	#define prvAddTaskToReadyList( pxTCB )                                                                     \
          do                                                                                                    \
          {                                                                                                     \
               traceMOVED_TASK_TO_READY_STATE(pxTCB);												  \
               taskRECORD_READY_PRIORITY((pxTCB)->uxPriority);                                                  \
               listSET_LIST_ITEM_VALUE(&((pxTCB)->xStateListItem),(pxTCB)->xTaskPeriod+xTaskGetTickCount());    \
               vListInsert(&(xReadyTasksListEDF),&((pxTCB)->xStateListItem));					            \
               tracePOST_MOVED_TASK_TO_READY_STATE( pxTCB );                                                    \
          }while(0)                                         
#else
     #define prvAddTaskToReadyList(pxTCB)                                                                       \
          do                                                                                                    \
          {                                                                                                     \
               traceMOVED_TASK_TO_READY_STATE(pxTCB);                                                           \
               listINSERT_END(&(pxReadyTasksLists[(pxTCB)->uxPriority]),&((pxTCB)->xStateListItem));            \
          }while(0)
#endif
/*========================================*/
/******* CONFIGURE PERIOD IN TCB  *******/
/*========================================*/
typedef struct tskTaskControlBlock
{
     #if (configUSE_EDF_SCHEDULER==1)
          TickType_t xTaskPeriod;
     #endif
}
/*========================================*/
/***** ADD DEADLINE TO CREATION API  ******/
/*========================================*/
BaseType_t xTaskCreate
                              (
                                   /* ----------------------- */
                                   #if (configUSE_EDF_SCHEDULER==1)
                                        TickType_t Period,
                                   #endif
                                   /* ----------------------- */
                              )
{
     /* ----------------------- */
     if( pxNewTCB != NULL )
     {
          /* ----------------------- */
          #if (configUSE_EDF_SCHEDULER==1)
          pxNewTCB->xTaskPeriod = Period;
          listSET_LIST_ITEM_VALUE(&((pxNewTCB)->xStateListItem),(pxNewTCB)->xTaskPeriod+xTickCount);	
          #endif
          /* ----------------------- */
     }
}
/*========================================*/
/****** CONFIGURE IDLE TASK PERIOD  *******/
/*========================================*/
void vTaskStartScheduler( void )
{
     TickType_t Init_IDLE_Period = 300;
     /* ----------------------- */
     xReturn = xTaskCreate
                         (
                              /* ----------------------- */
                              #if (configUSE_EDF_SCHEDULER==1)
                                         Init_IDLE_Period,
                              #endif
                              /* ----------------------- */
                         )
}
/*========================================*/
/******* CONFIGURE CONTEXT SWITCH  ********/
/*========================================*/
void vTaskSwitchContext(void)
{
     /* ----------------------- */
     #if(configUSE_EDF_SCHEDULER==1)
     {
          pxCurrentTCB=(TCB_t*)listGET_OWNER_OF_HEAD_ENTRY(&(xReadyTasksListEDF));		
     }
     #else
     {
          taskSELECT_HIGHEST_PRIORITY_TASK();
	
     }
     #endif
     /*traceTASK_SWITCHED_IN();*/		
     /* ----------------------- */
}

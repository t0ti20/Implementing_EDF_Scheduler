/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
void vTaskStartScheduler( void )
{
    BaseType_t xReturn;
		TickType_t Init_IDLE_Period = configIDLE_PERIOD;
    /* Add the idle task at the lowest priority. */
    #if ( configSUPPORT_STATIC_ALLOCATION == 1 )
        {
            StaticTask_t * pxIdleTaskTCBBuffer = NULL;
            StackType_t * pxIdleTaskStackBuffer = NULL;
            uint32_t ulIdleTaskStackSize;

            /* The Idle task is created using user provided RAM - obtain the
             * address of the RAM then create the idle task. */
            vApplicationGetIdleTaskMemory( &pxIdleTaskTCBBuffer, &pxIdleTaskStackBuffer, &ulIdleTaskStackSize );
            xIdleTaskHandle = xTaskCreateStatic( prvIdleTask,
                                                 configIDLE_TASK_NAME,
                                                 ulIdleTaskStackSize,
                                                 ( void * ) NULL,       /*lint !e961.  The cast is not redundant for all compilers. */
                                                 portPRIVILEGE_BIT,     /* In effect ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), but tskIDLE_PRIORITY is zero. */
                                                 pxIdleTaskStackBuffer,
                                                 pxIdleTaskTCBBuffer ); /*lint !e961 MISRA exception, justified as it is not a redundant explicit cast to all supported compilers. */

            if( xIdleTaskHandle != NULL )
            {
                xReturn = pdPASS;
            }
            else
            {
                xReturn = pdFAIL;
            }
        }
    #else /* if ( configSUPPORT_STATIC_ALLOCATION == 1 ) */
        {
            /* The Idle task is being created using dynamically allocated RAM. */
            xReturn = xTaskCreate( prvIdleTask,
                                   configIDLE_TASK_NAME,
                                   configMINIMAL_STACK_SIZE,
                                   ( void * ) NULL,
                                   portPRIVILEGE_BIT,  /* In effect ( tskIDLE_PRIORITY | portPRIVILEGE_BIT ), but tskIDLE_PRIORITY is zero. */
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*#original*//*Configure Ideal Task Period*/
																	 #if (configUSE_EDF_SCHEDULER==1)
                                         Init_IDLE_Period,
																	 #endif
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
																	 &xIdleTaskHandle ); /*lint !e961 MISRA exception, justified as it is not a redundant explicit cast to all supported compilers. */
        }
    #endif /* configSUPPORT_STATIC_ALLOCATION */
    #if ( configUSE_TIMERS == 1 )
        {
            if( xReturn == pdPASS )
            {
                xReturn = xTimerCreateTimerTask();
            }
            else
            {
                mtCOVERAGE_TEST_MARKER();
            }
        }
    #endif /* configUSE_TIMERS */

    if( xReturn == pdPASS )
    {
        /* freertos_tasks_c_additions_init() should only be called if the user
         * definable macro FREERTOS_TASKS_C_ADDITIONS_INIT() is defined, as that is
         * the only macro called by the function. */
        #ifdef FREERTOS_TASKS_C_ADDITIONS_INIT
            {
                freertos_tasks_c_additions_init();
            }
        #endif

        /* Interrupts are turned off here, to ensure a tick does not occur
         * before or during the call to xPortStartScheduler().  The stacks of
         * the created tasks contain a status word with interrupts switched on
         * so interrupts will automatically get re-enabled when the first task
         * starts to run. */
        portDISABLE_INTERRUPTS();

        #if ( configUSE_NEWLIB_REENTRANT == 1 )
            {
                /* Switch Newlib's _impure_ptr variable to point to the _reent
                 * structure specific to the task that will run first.
                 * See the third party link http://www.nadler.com/embedded/newlibAndFreeRTOS.html
                 * for additional information. */
                _impure_ptr = &( pxCurrentTCB->xNewLib_reent );
            }
        #endif /* configUSE_NEWLIB_REENTRANT */

        xNextTaskUnblockTime = portMAX_DELAY;
        xSchedulerRunning = pdTRUE;
        xTickCount = ( TickType_t ) configINITIAL_TICK_COUNT;

        /* If configGENERATE_RUN_TIME_STATS is defined then the following
         * macro must be defined to configure the timer/counter used to generate
         * the run time counter time base.   NOTE:  If configGENERATE_RUN_TIME_STATS
         * is set to 0 and the following line fails to build then ensure you do not
         * have portCONFIGURE_TIMER_FOR_RUN_TIME_STATS() defined in your
         * FreeRTOSConfig.h file. */
        portCONFIGURE_TIMER_FOR_RUN_TIME_STATS();

        traceTASK_SWITCHED_IN();

        /* Setting up the timer tick is hardware specific and thus in the
         * portable interface. */
        if( xPortStartScheduler() != pdFALSE )
        {
            /* Should not reach here as if the scheduler is running the
             * function will not return. */
        }
        else
        {
            /* Should only reach here if a task calls xTaskEndScheduler(). */
        }
    }
    else
    {
        /* This line will only be reached if the kernel could not be started,
         * because there was not enough FreeRTOS heap to create the idle task
         * or the timer task. */
        configASSERT( xReturn != errCOULD_NOT_ALLOCATE_REQUIRED_MEMORY );
    }

    /* Prevent compiler warnings if INCLUDE_xTaskGetIdleTaskHandle is set to 0,
     * meaning xIdleTaskHandle is not used anywhere else. */
    ( void ) xIdleTaskHandle;

    /* OpenOCD makes use of uxTopUsedPriority for thread debugging. Prevent uxTopUsedPriority
     * from getting optimized out as it is no longer used by the kernel. */
    ( void ) uxTopUsedPriority;
}
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
BaseType_t xTaskIncrementTick( void )
{
    TCB_t * pxTCB;
    TickType_t xItemValue;
    BaseType_t xSwitchRequired = pdFALSE;

    /* Called by the portable layer each time a tick interrupt occurs.
     * Increments the tick then checks to see if the new tick value will cause any
     * tasks to be unblocked. */
    traceTASK_INCREMENT_TICK( xTickCount );

    if( uxSchedulerSuspended == ( UBaseType_t ) pdFALSE )
    {
        /* Minor optimisation.  The tick count cannot change in this
         * block. */
        const TickType_t xConstTickCount = xTickCount + ( TickType_t ) 1;

        /* Increment the RTOS tick, switching the delayed and overflowed
         * delayed lists if it wraps to 0. */
        xTickCount = xConstTickCount;

        if( xConstTickCount == ( TickType_t ) 0U ) /*lint !e774 'if' does not always evaluate to false as it is looking for an overflow. */
        {
            taskSWITCH_DELAYED_LISTS();
        }
        else
        {
            mtCOVERAGE_TEST_MARKER();
        }	
        /* See if this tick has made a timeout expire.  Tasks are stored in
         * the  queue in the order of their wake time - meaning once one task
         * has been found whose block time has not expired there is no need to
         * look any further down the list. */
        if( xConstTickCount >= xNextTaskUnblockTime )
        {
            for( ; ; )
            {
                if( listLIST_IS_EMPTY( pxDelayedTaskList ) != pdFALSE )
                {
                    /* The delayed list is empty.  Set xNextTaskUnblockTime
                     * to the maximum possible value so it is extremely
                     * unlikely that the
                     * if( xTickCount >= xNextTaskUnblockTime ) test will pass
                     * next time through. */
                    xNextTaskUnblockTime = portMAX_DELAY; /*lint !e961 MISRA exception as the casts are only redundant for some ports. */
                    break;
                }
                else
                {
                    /* The delayed list is not empty, get the value of the
                     * item at the head of the delayed list.  This is the time
                     * at which the task at the head of the delayed list must
                     * be removed from the Blocked state. */
                    pxTCB = listGET_OWNER_OF_HEAD_ENTRY( pxDelayedTaskList ); /*lint !e9079 void * is used as this macro is used with timers and co-routines too.  Alignment is known to be fine as the type of the pointer stored and retrieved is the same. */
                    xItemValue = listGET_LIST_ITEM_VALUE( &( pxTCB->xStateListItem ) );

                    if( xConstTickCount < xItemValue )
                    {
                        /* It is not time to unblock this item yet, but the
                         * item value is the time at which the task at the head
                         * of the blocked list must be removed from the Blocked
                         * state -  so record the item value in
                         * xNextTaskUnblockTime. */
                        xNextTaskUnblockTime = xItemValue;
                        break; /*lint !e9011 Code structure here is deemed easier to understand with multiple breaks. */
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }

                    /* It is time to remove the item from the Blocked state. */
                    listREMOVE_ITEM( &( pxTCB->xStateListItem ) );

                    /* Is the task waiting on an event also?  If so remove
                     * it from the event list. */
                    if( listLIST_ITEM_CONTAINER( &( pxTCB->xEventListItem ) ) != NULL )
                    {
                        listREMOVE_ITEM( &( pxTCB->xEventListItem ) );
                    }
                    else
                    {
                        mtCOVERAGE_TEST_MARKER();
                    }
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*#original*//*UPDATE ALL TASKS DEADLINES*/
										#if (configUSE_EDF_SCHEDULER==1)
										listSET_LIST_ITEM_VALUE(&((pxTCB)->xStateListItem),(pxTCB)->xTaskPeriod+xTaskGetTickCount());
										#endif
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
                    prvAddTaskToReadyList( pxTCB );
                    /* A task being unblocked cannot cause an immediate
                     * context switch if preemption is turned off. */
                    #if ( configUSE_PREEMPTION == 1 )
                        {
                            /* Preemption is on, but a context switch should
                             * only be performed if the unblocked task has a
                             * priority that is equal to or higher than the
                             * currently executing task. */
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*#original*//*CHECK FOR DEADLINE AND CONTENT SWITCH IF REQUIRED*/
														#if (configUSE_EDF_SCHEDULER==1)
															if( pxTCB->xTaskPeriod <= pxCurrentTCB->xTaskPeriod )
															{
																	xSwitchRequired = pdTRUE;
															}
															else
															{
																	mtCOVERAGE_TEST_MARKER();
															}
														#else
															if( pxTCB->uxPriority >= pxCurrentTCB->uxPriority )
															{
																	xSwitchRequired = pdTRUE;
															}
															else
															{
																	mtCOVERAGE_TEST_MARKER();
															}
														#endif
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
                        }
                    #endif /* configUSE_PREEMPTION */
                }
            }
        }

        /* Tasks of equal priority to the currently running task will share
         * processing time (time slice) if preemption is on, and the application
         * writer has not explicitly turned time slicing off. */
        #if ( ( configUSE_PREEMPTION == 1 ) && ( configUSE_TIME_SLICING == 1 ) )
            {
                if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ pxCurrentTCB->uxPriority ] ) ) > ( UBaseType_t ) 1 )
                {
                    xSwitchRequired = pdTRUE;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        #endif /* ( ( configUSE_PREEMPTION == 1 ) && ( configUSE_TIME_SLICING == 1 ) ) */

        #if ( configUSE_TICK_HOOK == 1 )
            {
                /* Guard against the tick hook being called when the pended tick
                 * count is being unwound (when the scheduler is being unlocked). */
                if( xPendedTicks == ( TickType_t ) 0 )
                {
                    vApplicationTickHook();
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        #endif /* configUSE_TICK_HOOK */

        #if ( configUSE_PREEMPTION == 1 )
            {
                if( xYieldPending != pdFALSE )
                {
                    xSwitchRequired = pdTRUE;
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        #endif /* configUSE_PREEMPTION */
    }
    else
    {
        ++xPendedTicks;

        /* The tick hook gets called at regular intervals, even if the
         * scheduler is locked. */
        #if ( configUSE_TICK_HOOK == 1 )
            {
                vApplicationTickHook();
            }
        #endif
    }

    return xSwitchRequired;
}
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
static portTASK_FUNCTION( prvIdleTask, pvParameters )
{
    /* Stop warnings. */
    ( void ) pvParameters;

    /** THIS IS THE RTOS IDLE TASK - WHICH IS CREATED AUTOMATICALLY WHEN THE
     * SCHEDULER IS STARTED. **/

    /* In case a task that has a secure context deletes itself, in which case
     * the idle task is responsible for deleting the task's secure context, if
     * any. */
    portALLOCATE_SECURE_CONTEXT( configMINIMAL_SECURE_STACK_SIZE );

    for( ; ; )
    {
        /* See if any tasks have deleted themselves - if so then the idle task
         * is responsible for freeing the deleted task's TCB and stack. */
        prvCheckTasksWaitingTermination();

        #if ( configUSE_PREEMPTION == 0 )
            {
                /* If we are not using preemption we keep forcing a task switch to
                 * see if any other task has become available.  If we are using
                 * preemption we don't need to do this as any task becoming available
                 * will automatically get the processor anyway. */
                taskYIELD();
            }
        #endif /* configUSE_PREEMPTION */
        #if ( ( configUSE_PREEMPTION == 1 ) && ( configIDLE_SHOULD_YIELD == 1 ) )
            {
                /* When using preemption tasks of equal priority will be
                 * timesliced.  If a task that is sharing the idle priority is ready
                 * to run then the idle task should yield before the end of the
                 * timeslice.
                 *
                 * A critical region is not required here as we are just reading from
                 * the list, and an occasional incorrect value will not matter.  If
                 * the ready list at the idle priority contains more than one task
                 * then a task other than the idle task is ready to execute. */
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*#original*//*UPDATE IDEAL TASK DEADLINE*/
								#if( configUSE_EDF_SCHEDULER == 1)
								{
									listSET_LIST_ITEM_VALUE(&((pxCurrentTCB)->xStateListItem),(pxCurrentTCB)->xTaskPeriod+xTaskGetTickCount());
									listINSERT_END(&(xReadyTasksListEDF),&((pxCurrentTCB)->xStateListItem));
								}
								#else
								if( listCURRENT_LIST_LENGTH( &( pxReadyTasksLists[ tskIDLE_PRIORITY ] ) ) > ( UBaseType_t ) 1 )
                {
                    taskYIELD();
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
								#endif		
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
            }
        #endif /* ( ( configUSE_PREEMPTION == 1 ) && ( configIDLE_SHOULD_YIELD == 1 ) ) */
        #if ( configUSE_IDLE_HOOK == 1 )
            {
                extern void vApplicationIdleHook( void );

                /* Call the user defined function from within the idle task.  This
                 * allows the application designer to add background functionality
                 * without the overhead of a separate task.
                 * NOTE: vApplicationIdleHook() MUST NOT, UNDER ANY CIRCUMSTANCES,
                 * CALL A FUNCTION THAT MIGHT BLOCK. */
                vApplicationIdleHook();
            }
        #endif /* configUSE_IDLE_HOOK */

        /* This conditional compilation should use inequality to 0, not equality
         * to 1.  This is to ensure portSUPPRESS_TICKS_AND_SLEEP() is called when
         * user defined low power mode  implementations require
         * configUSE_TICKLESS_IDLE to be set to a value other than 1. */
        #if ( configUSE_TICKLESS_IDLE != 0 )
            {
                TickType_t xExpectedIdleTime;

                /* It is not desirable to suspend then resume the scheduler on
                 * each iteration of the idle task.  Therefore, a preliminary
                 * test of the expected idle time is performed without the
                 * scheduler suspended.  The result here is not necessarily
                 * valid. */
                xExpectedIdleTime = prvGetExpectedIdleTime();

                if( xExpectedIdleTime >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
                {
                    vTaskSuspendAll();
                    {
                        /* Now the scheduler is suspended, the expected idle
                         * time can be sampled again, and this time its value can
                         * be used. */
                        configASSERT( xNextTaskUnblockTime >= xTickCount );
                        xExpectedIdleTime = prvGetExpectedIdleTime();

                        /* Define the following macro to set xExpectedIdleTime to 0
                         * if the application does not want
                         * portSUPPRESS_TICKS_AND_SLEEP() to be called. */
                        configPRE_SUPPRESS_TICKS_AND_SLEEP_PROCESSING( xExpectedIdleTime );

                        if( xExpectedIdleTime >= configEXPECTED_IDLE_TIME_BEFORE_SLEEP )
                        {
                            traceLOW_POWER_IDLE_BEGIN();
                            portSUPPRESS_TICKS_AND_SLEEP( xExpectedIdleTime );
                            traceLOW_POWER_IDLE_END();
                        }
                        else
                        {
                            mtCOVERAGE_TEST_MARKER();
                        }
                    }
                    ( void ) xTaskResumeAll();
                }
                else
                {
                    mtCOVERAGE_TEST_MARKER();
                }
            }
        #endif /* configUSE_TICKLESS_IDLE */
    }
}
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
/*===============================================================================================*/
/*************************************************************************************************/
/*===============================================================================================*/
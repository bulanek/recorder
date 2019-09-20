
#include "recorder_inc.h"
#include "tskma_tasks.h"
#include "i2s/_com/i2s_com.h"
#include "nv/_com/nv_com.h" 
#include "hal/_com/hal_com.h"
#include "trace/_com/trace_com.h"

#include <stdio.h>

// CONSTANTS

void vApplicationStackOverflowHook( TaskHandle_t pxTask, char *pcTaskName )
{
	( void ) pcTaskName;
	( void ) pxTask;

	/* Run time stack overflow checking is performed if
	configCHECK_FOR_STACK_OVERFLOW is defined to 1 or 2.  This hook
	function is called if a stack overflow is detected. */
	taskDISABLE_INTERRUPTS();
	for( ;; );
}

/** For debug purpose of FreeRTOS
*/
void vAssertCalled(const char* pFileName, const int line)
{
    while (1) {}
}

int main(void)
{
    if (hal_init() == false)
    {
        return 1;
    }

    if (tskma_initialize() == false)
    {
        return 1;
    }

    vTaskStartScheduler();
    for (;;);
}


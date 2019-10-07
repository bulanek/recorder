#include "tskma_tasks.h"
#include "i2s_com.h"
#include "nv_com.h" 
#include "hal_com.h"
#include "trace_com.h"

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


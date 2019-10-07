#include "./_com/tskma_tasks.h"

#include "recorder_inc.h"
#include "i2s_com.h"
#include "trace_com.h"
#include "trace_out.hpp"



extern "C" void task_i2s(void* pParameters)
{
    TaskQueueI2S queue;
    QueueHandle_t xQueue = *(QueueHandle_t*)pParameters;
    TickType_t xTicksToWait = 0;

    static int counterOut = 0;
    static int counterIn = 0;

    TRACE_00(TRACE_LEVEL_LOG, "i2s task started");

    if (i2s_init() == false)
    {
        TRACE_00(TRACE_LEVEL_ERROR, "i2s init failed");
        return;
    }

    for (;; )
    {

    }
    //{
    //    if (SPI2->SR & SPI_SR_RXNE)
    //    {
    //        ++counterOut;
    //        volatile uint32_t data = SPI2->DR;
    //        i2s_receive_data(data);
    //    }
    //}
}
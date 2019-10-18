#include "./_com/tskma_tasks.h"

#include "recorder_inc.h"
#include "i2s_com.h"
#include "trace_com.h"
#include "trace_out.hpp"


static void print_help(void)
{
    printf("Debug choices:\n");
    printf("\th - print help\n");
    printf("\t1 - pdm start\n");
    printf("\t2 - pdm stop\n");
    printf("\t1 - pcm start\n");
    printf("\t2 - pcm stop\n");
}

static void command(const char commandChar)
{
    TaskQueueNV nvTask;

    switch (commandChar)
    {
    case 'h':
    {
        print_help();
    }
    break;
    case '1':
    {
        nvTask._opcode = NV_PDM_START;
        if (tskma_send_to_nv(&nvTask) == false)
        {
            TRACE_00(TRACE_LEVEL_ERROR, "nv_pdm_start send to nv failed");
            break;
        }
        I2S_START_MIC();
    }
    break;
    case '2':
    {
        nvTask._opcode = NV_PDM_STOP;
        if (tskma_send_to_nv(&nvTask) == false)
        {
            TRACE_00(TRACE_LEVEL_ERROR, "nv_pdm_stop send to nv failed");
            break;
        }
        I2S_STOP_MIC();
    }
    break;
    case '3':
    {
        nvTask._opcode = NV_PCM_START;
        if (tskma_send_to_nv(&nvTask) == false)
        {
            TRACE_00(TRACE_LEVEL_ERROR, "nv_pcm_start send to nv failed");
            break;
        }
    }
    break;
    case '4':
    {
        nvTask._opcode = NV_PCM_STOP;
        if (tskma_send_to_nv(&nvTask) == false)
        {
            TRACE_00(TRACE_LEVEL_ERROR, "nv_pcm_stop send to nv failed");
            break;
        }
        break;
    }
    default:
        TRACE_01(TRACE_LEVEL_ERROR, "Unexpected choice: %c", commandChar);
        break;
    }
}

static char getOneChar(void)
{
    int bytes_read;
    while ((USART2->SR & USART_SR_RXNE) == 0U);
    char pData = (char)USART2->DR;
    return pData;
}

extern "C" void task_i2s(void* pParameters)
{
    TaskQueueI2S queue;
    QueueHandle_t xQueue = *(QueueHandle_t*)pParameters;
    TickType_t xTicksToWait = 0;

    static int counterOut = 0;
    static int counterIn = 0;
    TRACE_00(TRACE_LEVEL_LOG, "i2s task started (not for i2s, but main task)");

    print_help();
    do
    {
        char commandChar = getOneChar();
        command(commandChar);
    } while (1);
}
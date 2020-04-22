#include "./_com/tskma_tasks.h"

#include "recorder_inc.h"
#include "i2s_com.h"
#include "trace_com.h"
#include "trace_out.hpp"

static void print_help(void)
{
    printf("Debug choices:\n"
        "\th - print help\n"
        "\t1 - pdm start\n"
        "\t2 - pdm stop\n"
        "\t3 - pcm start\n"
        "\t4 - pcm stop\n"
        "\t5 - i2s dma start\n"
        "\t6 - i2s dma stop\n"
        "\t7 - nv stop\n" );
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
        TRACE_00(TRACE_LEVEL_LOG, "nv pcm start");
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
        TRACE_00(TRACE_LEVEL_LOG, "nv pcm stop");
        if (tskma_send_to_nv(&nvTask) == false)
        {
            TRACE_00(TRACE_LEVEL_ERROR, "nv_pcm_stop send to nv failed");
            break;
        }
        break;
    }
    case '5':
    {
        I2S_START_MIC();
        TRACE_00(TRACE_LEVEL_LOG, "I2s dma mic start");
    }
    break;
    case '6':
    {
        I2S_STOP_MIC();
        TRACE_00(TRACE_LEVEL_LOG, "I2s dma mic stop");
    }
    break;
    case '7':
    {
        nvTask._opcode = NV_STOP;
        TRACE_00(TRACE_LEVEL_LOG, "NV stop");
        if (tskma_send_to_nv(&nvTask) == false)
        {
            break;
        }
    }
    break;
    default:
        TRACE_01(TRACE_LEVEL_ERROR, "Unexpected choice: %c", commandChar);
        break;
    }
}

static int write( char *pData, int len)
{
    char* pTmpData = pData;
    int bytes_written;
    for (bytes_written = 0; bytes_written < len; ++bytes_written)
    {
        while ((USART2->SR & USART_SR_TXE) == 0U);
        volatile uint8_t data = pTmpData[bytes_written];
        USART2->DR = data;
        if (pTmpData[bytes_written] == '\n')
        {
            while ((USART2->SR & USART_SR_TXE) == 0U);
            USART2->DR = '\r';
        }
    }
    return len;
}

extern "C" void task_uart(void* pParameters)
{
    TaskQueueUART queue;
    QueueHandle_t xQueue = *static_cast<QueueHandle_t*>(pParameters);
    TickType_t xTicksToWait = portMAX_DELAY;

    TaskQueueNV queueDataNv;

    TRACE_00(TRACE_LEVEL_LOG, "main task started");
    print_help();

    for (;; )
    {
        if (xQueueReceive(xQueue, &queue, xTicksToWait) == pdPASS)
        {
            switch (queue._opcode)
            {
            case UART_RCV:
            {
                char commandChar = *reinterpret_cast<char*>(queue._data);
                command(commandChar);
              
            }
            break;
            case UART_TR:
            {
                write(reinterpret_cast<char*>(queue._data), queue._dataLength);
                queueDataNv._pDataTrace = queue._data;
                queueDataNv._dataLength = queue._dataLength; 
                queueDataNv._opcode = NV_OPCODE_WRITE_TRACE_DATA;
                tskma_send_to_nv(&queueDataNv);
                //delete queue._data;
            }
            break;
            default:
                TRACE_01(TRACE_LEVEL_ERROR, "Unexpected opcode: %i", queue._opcode);
                break;
            }
        }
    }
}
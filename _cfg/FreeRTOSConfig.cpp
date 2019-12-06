#include "FreeRTOSConfig.h"
#include "trace_com.h"
#include "trace_out.hpp"

#include "stdint.h"
#include "stdio.h"

int write(const char* pData, int len)
{
    const char* pTmpData = pData;
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
            break;
        }
        if (pTmpData[bytes_written] == '\0')
        {
            while ((USART2->SR & USART_SR_TXE) == 0U);
            USART2->DR = '\n';
            while ((USART2->SR & USART_SR_TXE) == 0U);
            USART2->DR = '\r';
        }
    }
    return len;
}

/** For debug purpose of FreeRTOS
*/
void vAssertCalled(const char* pFileName, const int line)
{
    write("Assert failed file:",0xFFFF);
    write(pFileName,0xFFFF);
    while (1) {}
}

void fTraceQUEUE_SEND_FAILED(const char* pFileName, const int line, void* queue)
{
//    TRACE_01(TRACE_LEVEL_ERROR, "Queue send failed: %i", reinterpret_cast<uint32_t>(queue));
    printf("Trace in %s, level error, traceQUEUE_SEND_FAILED, line %i: %i\n", pFileName, line, queue);
}

void fTraceQUEUE_RECEIVE_FAILED(const char* pFileName, const int line, void* queue)
{
    printf("Trace in %s, level error, traceQUEUE_RECEIVE_FAILED, line %i: %i\n", pFileName, line, queue);
}

void fTraceQUEUE_SEND_FROM_ISR_FAILED(const char* pFileName, const int line, void* queue)
{
    write("Trace in ",0xFFFF); 
    write((char*)pFileName, 0xFFFF);
    write(", level error, traceQUEUE_SEND_FROM_ISR_FAILED, line: \n", 0xFFFF);
    while (1);
}


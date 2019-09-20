#include "hal_com.h"
#include "hal.h"
#include "recorder_inc.h"
#include "rtc_stm32f4.h"

void HAL_ASSERT(bool condition)
{
    while (1) {}
}

    /*
    GPIO A0 - Configuration of PDM->PCM mode. Pull down input.
    */
void HAL_PDM_PCM_MODE_INIT(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD0_1; /* Pull down*/
    GPIOA->MODER &= ~GPIO_MODER_MODE0; /* Input */

    EXTI->IMR |= 0x01; /* interrupt mask reg*/
    EXTI->RTSR |= 0x01;

	IRQn_Type type = EXTI0_IRQn;
	NVIC_EnableIRQ(type);
	NVIC_SetPriority(type, 15U);
}

/*
GPIO A 2(Tx) 3(Rx), USART2, 115200
*/
void HAL_INIT_CONFIG_UART(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;
    RCC->APB1ENR |= RCC_APB1ENR_USART2EN;

    GPIOA->MODER |= GPIO_MODER_MODE2_0; /* Pin2(Tx) output*/
    GPIOA->AFR[0] |= (GPIO_AFRL_AFRL2_0 | GPIO_AFRL_AFRL2_1 | GPIO_AFRL_AFRL2_2);
    GPIOA->AFR[0] |= (GPIO_AFRL_AFRL3_0 | GPIO_AFRL_AFRL3_1 | GPIO_AFRL_AFRL3_2); /* Usart */

    //enable uart
    USART2->CR1 |= USART_CR1_UE;
    /*BRR = 16 MHz / (required UART clock * 8x (2-OVER8))*/
    USART2->BRR = (uint32_t)(SystemCoreClock / (16 * 115200U));
    /* Configure USART1 */
    /* 8 data bit, 1 start bit, 1 stop bit; no parity; transmit enable;
     * over-sampling 16 */
    USART2->CR1 |= USART_CR1_TE;
}


int _read(int file, char *pData, int len)
{
    //    if (file != STDIN_FILENO)
    //    {
    //        errno = EBADF;
    //        return -1;
    //    }
    int bytes_read;
    for (bytes_read = 0; bytes_read < len; ++bytes_read)
    {
        while ((USART2->SR & USART_SR_RXNE) == 0U);
        *pData = (char)USART2->DR;
        ++pData;
    }
    return bytes_read;
}

int _write(int file, char *pData, int len)
{
    //    if (file != STDOUT_FILENO)
    //    {
    //        errno = EBADF;
    //        return -1;
    //    }CONFIG_UART
    char* pTmpData = pData;
    int bytes_written;
    for (bytes_written = 0; bytes_written < len; ++bytes_written)
    {
        while ((USART2->SR & USART_SR_TXE) == 0U);
        USART2->DR = pTmpData[bytes_written];
        if (pTmpData[bytes_written] == '\n')
        {
            while ((USART2->SR & USART_SR_TXE) == 0U);
            USART2->DR = '\r';
            break;
        }
    }
    return bytes_written;
}


/*---------------------------------------------------------*/
/* User provided RTC function for FatFs module             */
/*---------------------------------------------------------*/
/* This is a real time clock service to be called back     */
/* from FatFs module.                                      */

#if !FF_FS_NORTC && !FF_FS_READONLY
DWORD get_fattime (void)
{
	RTCTIME rtc;

	/* Get local time */
	if (!rtc_gettime(&rtc)) return 0;

	/* Pack date and time into a DWORD variable */
	return	  ((DWORD)(rtc.year - 1980) << 25)
			| ((DWORD)rtc.month << 21)
			| ((DWORD)rtc.mday << 16)
			| ((DWORD)rtc.hour << 11)
			| ((DWORD)rtc.min << 5)
			| ((DWORD)rtc.sec >> 1);
}
#endif

//int _fstat(int file, struct stat *st)
//{
////    st->st_mode = S_IFCHR;
//    return 0;
//}

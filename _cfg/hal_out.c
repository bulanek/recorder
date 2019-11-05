#include "hal_com.h"
#include "hal.h"
#include "recorder_inc.h"
#include "rtc_stm32f4.h"
#include "tskma_tasks.h"

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

    USART2->CR1 = 0;
    USART2->CR2 = 0;
    USART2->CR3 = 0;

    GPIOA->MODER |= (GPIO_MODER_MODE2_1 ) | (GPIO_MODER_MODE3_1 ) ; /* Pin2,3 alternate function mode*/
    GPIOA->AFR[0] |= (GPIO_AFRL_AFRL2_0 | GPIO_AFRL_AFRL2_1 | GPIO_AFRL_AFRL2_2);
    GPIOA->AFR[0] |= (GPIO_AFRL_AFRL3_0 | GPIO_AFRL_AFRL3_1 | GPIO_AFRL_AFRL3_2); /* Usart */

    GPIOA->OSPEEDR |= (GPIO_OSPEEDER_OSPEEDR2_1 | GPIO_OSPEEDER_OSPEEDR2_0);
    GPIOA->PUPDR |= GPIO_PUPDR_PUPD2_0;

    //enable uart
    USART2->CR1 |= USART_CR1_UE;
    /*USARDIV = 16 MHz / (required UART clock * 8x (2-OVER8))  OVER8 = 0*/
    /* UPDATE FREQ!  fr 14.5 MHz -> value USARDIV = 7.8668 ->  Mantisa = 7, Fraction*16 = 14 */
    USART2->BRR = 0x07E; /* 8.6875 -> Mantisa(USARDIV)=8 <4 | Fraction*16 (0.6875*16 = 11)/

    /* Configure USART1 */
    /* 8 data bit, 1 start bit, 1 stop bit; no parity; transmit enable;
     * over-sampling 16 */
    USART2->CR1 |= USART_CR1_TE | USART_CR1_RE ;
    USART2->CR1 |= USART_CR1_RXNEIE; /* Enable rx interrupt*/

    IRQn_Type type = USART2_IRQn;
    NVIC_EnableIRQ(type);
    NVIC_SetPriority(type, 15U);

    while ((USART2->SR & USART_SR_TC) == 0);
}

int _read(int file, char *pData, int len)
{
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
    uint8_t* pDataSend = pvPortMalloc(len);
    if (pDataSend == NULL)
    {
        return 0;
    }
    memcpy(pDataSend, pData, len);

    TaskQueueUART taskQueue;
    taskQueue._data = pDataSend;
    taskQueue._dataLength = len;
    taskQueue._opcode = UART_TR;

    tskma_send_to_uart_irt(&taskQueue);

    return len;
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

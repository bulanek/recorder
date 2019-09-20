/*
 * recorder.c
 *
 *  Created on: 19. 3. 2017
 *      Author: boris
 */

//#include <stm32f4xx_ll_gpio.h>
//#include <system_stm32f4xx.h>
//#include <stm32f401xe.h>
//#include <stm32f4xx.h>

//#include <assert.h>
#include "recorder_inc.h"
#include <recorder.h>
#include <stdint.h>


volatile uint32_t f_TicksPerUs = 0;
#pragma GCC push_options
#pragma GCC optimize("O3")
void TM_DelayUs(const uint32_t timeUs)
{
	volatile uint32_t cycles = f_TicksPerUs/1000000L * timeUs;
	volatile uint32_t start = DWT->CYCCNT;
	do {
	} while(DWT->CYCCNT - start < cycles);
}
#pragma GCC pop_options

////////////////////////////////////////////////////////////////////////////////
/// Initialize SPI.
////////////////////////////////////////////////////////////////////////////////
void InitializeGPIO(void)
{
	// GPIO clock enable.
	SET_REGISTER_VALUE(RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN, 1);
	SET_REGISTER_VALUE(RCC->AHB1ENR, RCC_AHB1ENR_GPIOBEN, 1);
	SET_REGISTER_VALUE(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN, 1);

//	// Config pin
//	gpioInit.Pin =  CONFIG_PIN;
//	gpioInit.Mode = LL_GPIO_MODE_INPUT;
//	gpioInit.Pull = LL_GPIO_PULL_DOWN;
//
//    ASSERT_ALL(LL_GPIO_Init(CONFIG_PORT, &gpioInit) == SUCCESS);
//
//    // UART PINS
//
//    gpioInit.Pin = CONFIG_UART_TX_PIN;
//    gpioInit.Mode = LL_GPIO_MODE_OUTPUT;
//    gpioInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
//    gpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//
//    ASSERT_ALL(LL_GPIO_Init(CONFIG_UART_PORT, &gpioInit) == SUCCESS);
//
//    gpioInit.Pin = CONFIG_UART_RX_PIN;
//    gpioInit.Mode = LL_GPIO_MODE_INPUT;
//    gpioInit.Pull = LL_GPIO_PULL_UP;
//    gpioInit.Speed = LL_GPIO_SPEED_FREQ_HIGH;
//
//    ASSERT_ALL(LL_GPIO_Init(CONFIG_UART_PORT, &gpioInit) == SUCCESS);
}

uint8_t IsConfPinOn(void)
{
    return CONFIG_PORT->IDR & CONFIG_PIN;
}

//void InitializeConfigUART(void)
//{
//    // TODO BB: in case of change USART1 -> different!!
//    RCC->APB2ENR |=  RCC_APB2ENR_USART1EN;
//
//    //Disable UART (in case the boot loader left it on)
//    CONFIG_UART->CR1 = 0;
//    CONFIG_UART->CR2 = 0;
//    CONFIG_UART->CR3 = 0;
//
//    /* Configure USART3 */
//    /* 8 data bit, 1 start bit, 1 stop bit; no parity; receive enable;
//     * over-sampling 16 */
//    CONFIG_UART->CR1 = USART_CR1_RE | USART_CR1_TE;
//    //BRR = 12 MHz / required UART clock
//    CONFIG_UART->BRR = (uint16_t) (SystemCoreClock / BAUD_RATE);
//    //enable uart
//    CONFIG_UART->CR1 |= USART_CR1_UE;
//}



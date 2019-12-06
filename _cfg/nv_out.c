#include "recorder_inc.h"
#include "nv_com.h"



void NV_INIT(void)
{
    /* SPI3 for sd card comm. Master mode */
    /* Pins:
        PC10 - SCK
        PC11 - MISO     (Pull up)
        PC12 - MOSI
        PA4  - NSS
    */

    RCC->APB1ENR |= RCC_APB1ENR_SPI3EN;


    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN;
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;

    GPIOC->MODER |= GPIO_MODER_MODE12_1 | GPIO_MODER_MODE11_1 | GPIO_MODER_MODE10_1; /* PC 10-12. - SCK MOSI */
    GPIOA->MODER |= GPIO_MODER_MODE4_0;

    GPIOA->PUPDR |= GPIO_PUPDR_PUPD11_0; /* Pull up for MISO*/


    GPIOC->AFR[1] |= GPIO_AFRH_AFSEL10_1 | GPIO_AFRH_AFSEL10_2;
    GPIOC->AFR[1] |= GPIO_AFRH_AFSEL11_1 | GPIO_AFRH_AFSEL11_2;
    GPIOC->AFR[1] |= GPIO_AFRH_AFSEL12_1 | GPIO_AFRH_AFSEL12_2;

    SPI3->CR1 = SPI_CR1_SSM | SPI_CR1_SSI | SPI_CR1_SPE | SPI_CR1_MSTR;					/* Enable SPI3 */
    SPI3->CR2 |= SPI_CR2_SSOE;              /* SS output enable */
}

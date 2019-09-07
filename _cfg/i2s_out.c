
#include "recorder_inc.h"
#include "i2s_com.h"


static void i2s_init_gpio(void)
{
    /* I2S pins:
        CK - PB13, 
        WS - PB12,
        SD - PB15 */

    uint32_t testAHB1 = RCC->AHB1ENR;
    uint32_t testPointerVal = (uint32_t)(&RCC->AHB1ENR);
    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

    // I2S (MEMS) configuration
    GPIOB->MODER |= GPIO_MODER_MODE12_1 | GPIO_MODER_MODE13_1 | /*GPIO_MODER_MODE14_1 |*/ GPIO_MODER_MODE15_1;  /* Alternate mode*/

    // GPIOB->PUPDR |= GPIO_PUPDR_PUPD15_0; /* Pull up*/

    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED13_1 | /*GPIO_OSPEEDR_OSPEED14_1 |*/ GPIO_OSPEEDR_OSPEED15_1; /* High speed*/

    GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL12_2 | GPIO_AFRH_AFSEL12_0); /* Alternate function 5 - SPI*/
    GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL13_2 | GPIO_AFRH_AFSEL13_0); /* Alternate function 5 - SPI*/
    GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_0); /* Alternate function 5 - SPI*/
}

static void i2s_init_spi_reg(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

	// Disable I2S
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_I2SE;

	// Set 16kHz sampling (assume  data length 16bit) (ref.note : 590)
	SET_REGISTER_VALUE(SPI2->I2SPR, SPI_I2SPR_I2SDIV, 62);
	SET_REGISTER_VALUE(SPI2->I2SPR, SPI_I2SPR_ODD, 0x01);

	// Interrupts
    //SPI2->CR2 |= SPI_CR2_RXNEIE;
    //SPI2->CR2 |= SPI_CR2_ERRIE;
	// Set I2S mod on SPI2
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
	// Set as master - receive
	SET_REGISTER_VALUE(SPI2->I2SCFGR, SPI_I2SCFGR_I2SCFG, 0x03);
	// Standard: MSB
	SET_REGISTER_VALUE(SPI2->I2SCFGR, SPI_I2SCFGR_I2SSTD, 0x01);
	// Set steady state (high level)
	SET_REGISTER_VALUE(SPI2->I2SCFGR, SPI_I2SCFGR_CKPOL, 0x01);
	// Data length to be transfered (16 bit)
	SET_REGISTER_VALUE(SPI2->I2SCFGR, SPI_I2SCFGR_DATLEN, 0x00);
	// Set number of bits per channel (16 bit)
	SET_REGISTER_VALUE(SPI2->I2SCFGR, SPI_I2SCFGR_CHLEN, 0x00);

    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE;
}

static void i2s_init_dma(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;

    DMA1_Stream3->CR |= DMA_SxCR_CHSEL_1 | DMA_SxCR_CHSEL_0; /* channel 3 of Stream 3 */
    DMA1_Stream3->CR &= ~DMA_SxCR_DIR; /* Peripheral to memory. */
    DMA1_Stream3->PAR = SPI2->DR;
//    DMA1_Stream3->M0AR = 

}


static void i2s_init_irt(void)
{
	IRQn_Type type = SPI2_IRQn;
	NVIC_EnableIRQ(type);
	NVIC_SetPriority(type, 15U);
}

static void i2s_init_clock(void)
{
    // Set clock
    RCC->CR &= ~RCC_CR_PLLI2SON;
    RCC->CR &= ~RCC_CR_PLLON;
    // STM32f041 reference note 105: VCO=PLL_input_f/PLLM; VCO in <1,2>MHz
	//                                 recommended 2MHz to suppress jitter.
    // for 16 MHz, PPLM = 8
    SET_REGISTER_VALUE(RCC->PLLCFGR, RCC_PLLCFGR_PLLM, 8);
    // see reference note 590: table for PLLI2SN PLLI2SR
	SET_REGISTER_VALUE(RCC->PLLI2SCFGR, RCC_PLLI2SCFGR_PLLI2SN, 192);
	SET_REGISTER_VALUE(RCC->PLLI2SCFGR, RCC_PLLI2SCFGR_PLLI2SR, 3);
}

static void i2s_start_clock(void)
{
    RCC->CR |= RCC_CR_PLLI2SON;
    RCC->CR |= RCC_CR_PLLON;
}


void I2S_INIT_MIC(void)
{
    i2s_init_gpio();
    i2s_init_irt();
    i2s_init_clock();
    i2s_init_spi_reg();
    i2s_start_clock();
}



#include "recorder_inc.h"
#include "i2s_com.h"

static void _i2s_init_gpio(void)
{
    /* I2S pins:
        CK - PB13, 
        WS - PB12,
        SD - PB15 */

    RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN | RCC_AHB1ENR_GPIOBEN;

    unsigned long gpiobReg = (unsigned long)(GPIOB);
    // I2S (MEMS) configuration
    GPIOB->MODER |= GPIO_MODER_MODE12_1 | GPIO_MODER_MODE13_1 | /*GPIO_MODER_MODE14_1 |*/ GPIO_MODER_MODE15_1;  /* Alternate mode*/

     GPIOB->PUPDR |= GPIO_PUPDR_PUPD15_0; /* Pull up*/

    GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED13_1 | /*GPIO_OSPEEDR_OSPEED14_1 |*/ GPIO_OSPEEDR_OSPEED15_1; /* High speed*/

    GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL12_2 | GPIO_AFRH_AFSEL12_0); /* Alternate function 5 - SPI*/
    GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL13_2 | GPIO_AFRH_AFSEL13_0); /* Alternate function 5 - SPI*/
    GPIOB->AFR[1] |= (GPIO_AFRH_AFSEL15_2 | GPIO_AFRH_AFSEL15_0); /* Alternate function 5 - SPI*/
}

static void _i2s_init_spi_reg(void)
{
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;
	// Disable I2S
    SPI2->I2SCFGR &= ~SPI_I2SCFGR_I2SE;

    /* I2S_SPR: 
        MCKOE: 0 - Master clock enable
        ODD: 1 - Odd factor for the prescaler 0: real divider value is = I2SDIV *2 + 1
    */
	// Set sampling (for data length 16bit) (ref.note : 590)
	SET_REGISTER_VALUE(SPI2->I2SPR, SPI_I2SPR_I2SDIV, RECORDER_I2S_I2SSPR_DIV);
	SET_REGISTER_VALUE(SPI2->I2SPR, SPI_I2SPR_ODD, RECORDER_I2S_I2SSPR_ODD);

    /* I2SCFGR defaults: 
        I2SSTD: philips standard, 
        CLPOL: low level polarity, 
        DATLEN: 16-bit datalength, 
        CHLEN: 16-bit wide channel length. */
	// Interrupts
	// Set I2S mode
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SMOD;
	// Set as master - receive
    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SCFG_0 | SPI_I2SCFGR_I2SCFG_1;

    SPI2->CR2 |= SPI_CR2_RXDMAEN; /* Enable Rx DMA*/

    SPI2->I2SCFGR |= SPI_I2SCFGR_I2SE;
}

static void _i2s_init_dma(void)
{
    RCC->AHB1ENR |= RCC_AHB1ENR_DMA1EN;
    RCC->APB1ENR |= RCC_APB1ENR_SPI2EN;

    DMA1_Stream3->CR &= ~DMA_SxCR_EN;
    while((DMA1_Stream3->CR & DMA_SxCR_EN) != 0);

    DMA1_Stream3->PAR = &(SPI2->DR);
    DMA1_Stream3->M0AR = (uint32_t)i2s_get_buffer1();
    DMA1_Stream3->M1AR = (uint32_t)i2s_get_buffer2();
    DMA1_Stream3->NDTR = i2s_get_buffer_size_word();

    DMA1_Stream3->CR &= ~(DMA_SxCR_CHSEL  /* Channel 0 (SPI2_Rx) */
        | DMA_SxCR_DIR      /* Peripheral to memory. 00 */
        | DMA_SxCR_PL);     /* Priority */

    DMA1_Stream3->CR |= DMA_SxCR_DBM; /* Double buffer switching */
    DMA1_Stream3->CR |= DMA_SxCR_CIRC; /* Circ mode */
	DMA1_Stream3->CR |= DMA_SxCR_MINC;		/* Memory address pointer is incremented after each data transfer. */

    DMA1_Stream3->CR |= DMA_SxCR_MSIZE_0; /* 16 bit size of memory */
    DMA1_Stream3->CR |= DMA_SxCR_PSIZE_0; /* 16 bit size of peripheral. */
    DMA1_Stream3->CR |= DMA_SxCR_TCIE; /*  Transfer complete interrupt enable */
    DMA1_Stream3->CR |= DMA_SxCR_TEIE; /* Transfer error interrupt enable */
    DMA1_Stream3->CR |= DMA_SxCR_DMEIE; /* Direct mode error  */
    DMA1_Stream3->FCR |= DMA_SxFCR_FEIE; /* FiFo error  */

    DMA1_Stream3->FCR &= ~(DMA_SxFCR_DMDIS);
}

static void _i2s_init_irt(void)
{
    IRQn_Type type = DMA1_Stream3_IRQn;
    NVIC_EnableIRQ(type);
    NVIC_SetPriority(type, 15U);
}

static void _i2s_init_clock(void)
{
    // Disable clock
    RCC->CR &= ~RCC_CR_PLLI2SON;
    RCC->CR &= ~RCC_CR_PLLON;

    // see reference note 590: table for PLLI2SN PLLI2SR
	SET_REGISTER_VALUE(RCC->PLLI2SCFGR, RCC_PLLI2SCFGR_PLLI2SN, RECORDER_I2S_PLLCFGR_PLLN);
	SET_REGISTER_VALUE(RCC->PLLI2SCFGR, RCC_PLLI2SCFGR_PLLI2SR, RECORDER_I2S_PLLCFGR_PLLR);
}

static void _i2s_start_clock(void)
{
    RCC->CR |= RCC_CR_PLLON;
    RCC->CR |= RCC_CR_PLLI2SON;
}

void I2S_INIT_MIC(void)
{
    _i2s_init_gpio();
    _i2s_init_irt();
    _i2s_init_clock();
    _i2s_init_spi_reg();
    _i2s_init_dma();
    _i2s_start_clock();
}

void I2S_START_MIC(void)
{
    vPortEnterCritical();
    DMA1_Stream3->CR |= DMA_SxCR_EN;
    vPortExitCritical();
}

void I2S_STOP_MIC(void)
{
    vPortEnterCritical();
    DMA1_Stream3->CR &= ~DMA_SxCR_EN;
    vPortExitCritical();
}


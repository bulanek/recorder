#include "recorder_inc.h"
#include "i2s_com.h"
#include "i2s_service.hpp"

I2S::Service f_I2SService;

extern "C" bool i2s_init(void)
{
    return f_I2SService.Init();
}

extern "C" uint16_t* i2s_get_buffer1(void)
{
    return f_I2SService._bufferForDMA1;
}

extern "C" uint16_t* i2s_get_buffer2(void)
{
    return f_I2SService._bufferForDMA2;
}

extern "C" uint16_t i2s_get_buffer_size_word(void)
{
    return f_I2SService.BUFFER_SIZE_WORDS;
}



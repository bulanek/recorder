#ifndef I2S_COM_



#ifdef __cplusplus
extern "C"
{
#endif

bool i2s_init(void);

uint16_t* i2s_get_buffer1(void);
uint16_t* i2s_get_buffer2(void);
uint16_t i2s_get_buffer_size_word(void);


#ifndef I2S_INIT_MIC
void I2S_INIT_MIC(void);
#endif

#ifndef I2S_START_MIC
void I2S_START_MIC(void);
#endif

#ifndef I2S_STOP_MIC
void I2S_STOP_MIC(void);
#endif


#ifdef __cplusplus
}
#endif

#endif // !I2S_COM_

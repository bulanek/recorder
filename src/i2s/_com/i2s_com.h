#ifndef I2S_COM_



#ifdef __cplusplus
extern "C"
{
#endif

bool i2s_init(void);
void i2s_receive_data(const uint16_t data);

#ifndef I2S_INIT_MIC
void I2S_INIT_MIC(void);
#endif /* I2S_INIT_MIC */

#ifdef __cplusplus
}
#endif

#endif // !I2S_COM_

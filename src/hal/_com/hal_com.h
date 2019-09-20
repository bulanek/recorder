#ifndef HAL_COM_H_
#define HAL_COM_H_

#include "recorder_inc.h"

typedef struct {
	uint16_t	year;	/* 1970..2106 */
	uint8_t		month;	/* 1..12 */
	uint8_t		mday;	/* 1..31 */
	uint8_t		hour;	/* 0..23 */
	uint8_t		min;	/* 0..59 */
	uint8_t		sec;	/* 0..59 */
	uint8_t		wday;	/* 0..6 (Sun..Sat) */
} RTCTIME;

typedef enum
{
    HAL_MODE_RECORD,
    HAL_MODE_PDM_PCM
} HalMode;

#ifndef STRLEN_STATIC
 #error("static assert not defined")
#endif 

#ifndef FATAL_ERROR
#error("Fatal error not defined")
#endif

#ifdef __cplusplus
extern "C"
{
#endif

bool hal_init(void);

HalMode hal_get_mode(void);

#ifndef HAL_PDM_PCM_MODE_INIT
void HAL_PDM_PCM_MODE_INIT(void);
#endif

/** Start Debug console - uart. */
#ifndef HAL_INIT_CONFIG_UART
void HAL_INIT_CONFIG_UART(void);
#endif

#ifndef HAL_RTC_INIT
bool HAL_RTC_INIT(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* HAL_COM_H_*/


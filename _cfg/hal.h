#ifndef _HAL_H_
#define _HAL_H_

#include "recorder_inc.h"

#define _STRLEN_STATIC(s) (sizeof(s)/sizeof(s[0]) - 1 )

#define STRLEN_STATIC(s) _STRLEN_STATIC(s)

#define FATAL_ERROR(msg) { TRACE_00(TRACE_LEVEL_ERROR, msg); while(1){} }

#define RECORDER_PLLCFGR_PLLN 174U  /*< Main PLL (PLL) multiplication factor for VCO. */

#define RECORDER_PLLCFGR_PLLM  8U  /*<Division factor for the main PLL (PLL) and audio PLL (PLLI2S) input clock
Set and cleared by software to divide the PLL and PLLI2S input clock before the VCO.
These bits can be written only when the PLL and PLLI2S are disabled. */


#define RECORDER_I2S_PLLCFGR_PLLN 384U

#define RECORDER_I2S_PLLCFGR_PLLR 5U

#define RECORDER_I2S_I2SSPR_DIV 12U

#define RECORDER_I2S_I2SSPR_ODD 1U



#endif /* _HAL_H_ */
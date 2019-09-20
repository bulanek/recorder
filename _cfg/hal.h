#ifndef _HAL_H_
#define _HAL_H_

#define _STRLEN_STATIC(s) (sizeof(s)/sizeof(s[0]) - 1 )

#define STRLEN_STATIC(s) _STRLEN_STATIC(s)

#define FATAL_ERROR(msg) { TRACE_00(TRACE_ERROR, msg); trace_write(); while(1){} }

#endif /* _HAL_H_ */
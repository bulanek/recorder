#ifndef TRACE_CFG_H
#define TRACE_CFG_H

#define TRACE_00(level, msg) trace_entry_00(level, __LINE__,0)
#define TRACE_01(level, msg, par1) trace_entry_01(level, __LINE__,0,par1)
#define TRACE_02(level, msg, par1,par2) trace_entry_02(level, __LINE__,0,par1,par2)

#endif // !TRACE_CFG_H
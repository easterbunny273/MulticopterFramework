#ifndef __DEBUG_H_
#define __DEBUG_H_

#include "config.h"

template<typename T> void debug_print(T message)
{
#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	LOWLEVELCONFIG_DEBUG_UART.print(message);
#endif
};

template<typename T> void debug_println(T message)
{
#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	LOWLEVELCONFIG_DEBUG_UART.println(message);
#endif
};

#endif
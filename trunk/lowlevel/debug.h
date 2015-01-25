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

int freeMemory() {
	uint32_t stackTop;
	uint32_t heapTop;

	// current position of the stack.
	stackTop = (uint32_t) &stackTop;

	// current position of heap.
	void* hTop = malloc(1);
	heapTop = (uint32_t) hTop;
	free(hTop);

	// The difference is the free, available ram.
	return stackTop - heapTop;
}

#endif
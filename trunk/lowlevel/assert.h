#ifndef __ASSERT_H_
#define __ASSERT_H_

#include "config.h"
#include "debug.h"

// To output assert message
#include "SerialDebugDisplay20x4.h"

void itl_s_assert_handler(int iLine, const char *szFile, char *szAssertExpression);
void assert_update_led();
void inject_display(SerialDebugDisplay20x4 *_pDisplay);

#if !defined(LOWLEVELCONFIG_ENABLE_DEBUGGING) || !LOWLEVELCONFIG_ENABLE_DEBUGGING
#define assert( expr ) ((void)0)
#else
#define assert( exp ) \
    ( (exp) ? (void)0 : itl_s_assert_handler( __LINE__, __FILE__, #exp))
#endif 

#endif

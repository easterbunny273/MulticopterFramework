#ifndef __ASSERT_H_
#define __ASSERT_H_

#include "config.h"
#include "debug.h"

#if !defined(LOWLEVELCONFIG_ENABLE_DEBUGGING) || !LOWLEVELCONFIG_ENABLE_DEBUGGING
#define assert( expr ) ((void)0)
#else
#define assert( exp ) \
    ( (exp) ? (void)0 : itl_s_assert_handler( __LINE__, __FILE__, "#exp"))

static bool g_AssertFired = false;
static unsigned long g_lTimeOfLastLedUpdate = 0;
static bool g_bLastLEDState = false;

/// Internal handler, should only be used by assert() macro
static void itl_s_assert_handler(int iLine, const char *szFile, char *szAssertExpression)
{
	// Build message
	char szBuffer[512];
	sprintf(szBuffer, "Assertion \"%s\" failed on %s:%d", szAssertExpression, szFile, iLine);

	debug_println(szBuffer);

	// Set flag so that LED starts flashing (if set in config.h, and assert_update_led() is periodically called)
	g_AssertFired = true;
}

// This method should be called at the end of each loop() call. It let blink the defined
// debug led if an assertion has failed until the board is restarted. 
static void assert_update_led()
{
#ifdef LOWLEVELCONFIG_DEBUG_ASSERT_LED
	if (g_AssertFired)
	{
		unsigned long lCurrentTime = millis();

		if ((g_lTimeOfLastLedUpdate == 0) || (g_lTimeOfLastLedUpdate - lCurrentTime > 100))
		{
			pinMode(13, OUTPUT);
			digitalWrite(13, g_bLastLEDState);

			g_bLastLEDState = !g_bLastLEDState;
			g_lTimeOfLastLedUpdate = lCurrentTime;
		}
	}
#endif
}

#endif


#endif
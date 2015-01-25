#include "assert.h"

bool g_AssertFired = false;
unsigned long g_lTimeOfLastLedUpdate = 0;
bool g_bLastLEDState = false;

static SerialDebugDisplay20x4 * pDebugDisplay = NULL;

void inject_display(SerialDebugDisplay20x4 *_pDisplay) { pDebugDisplay = _pDisplay; }

/// Internal handler, should only be used by assert() macro
void itl_s_assert_handler(int iLine, const char *szFile, char *szAssertExpression)
{
	// Build message
	/*char szBuffer[128];
	if (szFile != NULL && szAssertExpression != NULL)
	sprintf(szBuffer, "Assertion \"%s\" failed on %s:%d", szAssertExpression, szFile, iLine);
	else
	sprintf(szBuffer, "Assertion method failure");*/

	debug_print("Assertion \"");
	debug_print(szAssertExpression);
	debug_print("\" failed on ");
	debug_print(szFile);
	debug_print(":");
	debug_println(iLine);
	//debug_println(szBuffer);

	//if (pDebugDisplay != NULL) pDebugDisplay->SetAssertMessage(szBuffer);

	// Set flag so that LED starts flashing (if set in config.h, and assert_update_led() is periodically called)
	g_AssertFired = true;
}

// This method should be called at the end of each loop() call. It let blink the defined
// debug led if an assertion has failed until the board is restarted. 
void assert_update_led()
{
#ifdef LOWLEVELCONFIG_DEBUG_ASSERT_LED
	if (g_AssertFired)
	{
		unsigned long lCurrentTime = millis();

		if ((g_lTimeOfLastLedUpdate == 0) || (lCurrentTime - g_lTimeOfLastLedUpdate > 100))
		{
			digitalWrite(13, g_bLastLEDState ? HIGH : LOW);

			g_bLastLEDState = !g_bLastLEDState;
			g_lTimeOfLastLedUpdate = lCurrentTime;
		}
	}
#endif
}
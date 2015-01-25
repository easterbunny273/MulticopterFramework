// cmoellinger, 17.01.2015

#ifndef __SERIALDEBUGDISPLAY20x4_H_
#define __SERIALDEBUGDISPLAY20x4_H_

#include <Stream.h>
#include "NauticalOrientation.h"

class SerialDebugDisplay20x4
{
public:
	SerialDebugDisplay20x4(Stream &rSerialOutputDevice);

	void Print_Orientations_PID_And_MotorValues(const NauticalOrientation& CurrentOrientation,
												const NauticalOrientation& DesiredOrientation,
												float dPIDResultYawNormalized, //< [-1, 1]
												float dPIDResultPitchNormalized,
												float dPIDResultRollNormalized,
												float dMotor1Result, //< [0, 1]
												float dMotor2Result,
												float dMotor3Result,
												float dMotor4Result);

	void PrintSettingsOverview(float fP, float fD, float fI);

	void SetAssertMessage(const char * szMessage);

private:
	void ItlPrintSettings(int iStartingPosition, float fP, float fD, float fI);
	void ItlPrintYawPitchRollLine(int iStartingPosition, char cOrientationIdentificator, float dCurrentValue, float dDesiredValue, float dPIDResult);
	void ItlPrintResultingMotorValues(int iStartingPosition, float d1, float d2, float d3, float d4);

	void ItlMoveToPosition(int iPosition);	//< TODO: refactor to own class (e.g., IDisplayDriver and $DisplayName$Driver)
	void ItlClearDisplay();					//< TODO: refactor to own class (e.g., IDisplayDriver and $DisplayName$Driver)

	Stream & m_rSerialOutputDevice;

	char m_szDebugMessage[61];
};

#endif
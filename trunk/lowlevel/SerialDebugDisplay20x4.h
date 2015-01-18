// cmoellinger, 17.01.2015

#ifndef __SERIALDEBUGDISPLAY20x4_H_
#define __SERIALDEBUGDISPLAY20x4_H_

#include <Stream.h>

class SerialDebugDisplay20x4
{
public:
	SerialDebugDisplay20x4(Stream &rSerialOutputDevice) : m_rSerialOutputDevice(rSerialOutputDevice) { for (unsigned int n = 0; n < 60; n++) m_szDebugMessage[n] = ' '; m_szDebugMessage[60] = 0; };

	void Print_Orientations_PID_And_MotorValues(double dCurrentYawInDegrees, //< [0, 360[
												double dCurrentPitchInDegrees,
												double dCurrentRollInDegrees,
												double dDesiredYawInDegrees,
												double dDesiredPitchInDegrees,
												double dDesiredRollInDegrees,
												double dPIDResultYawNormalized, //< [-1, 1]
												double dPIDResultPitchNormalized,
												double dPIDResultRollNormalized,
												double dMotor1Result, //< [0, 1]
												double dMotor2Result,
												double dMotor3Result,
												double dMotor4Result);

	void PrintSettingsOverview(float fP, float fD, float fI);

	void SetAssertMessage(const char * szMessage);

private:
	void ItlPrintSettings(int iStartingPosition, float fP, float fD, float fI);
	void ItlPrintYawPitchRollLine(int iStartingPosition, char cOrientationIdentificator, double dCurrentValue, double dDesiredValue, double dPIDResult);
	void ItlPrintResultingMotorValues(int iStartingPosition, double d1, double d2, double d3, double d4);

	void ItlMoveToPosition(int iPosition);	//< TODO: refactor to own class (e.g., IDisplayDriver and $DisplayName$Driver)
	void ItlClearDisplay();					//< TODO: refactor to own class (e.g., IDisplayDriver and $DisplayName$Driver)

	Stream & m_rSerialOutputDevice;

	char m_szDebugMessage[61];
};

#endif
// cmoellinger, 17.01.2015

#include "SerialDebugDisplay20x4.h"


void SerialDebugDisplay20x4::Print_Orientations_PID_And_MotorValues(double dCurrentYawInDegrees, /*< [0, 360[ */ 
																	double dCurrentPitchInDegrees, 
																	double dCurrentRollInDegrees, 
																	double dDesiredYawInDegrees, 
																	double dDesiredPitchInDegrees, 
																	double dDesiredRollInDegrees, 
																	double dPIDResultYawNormalized, /*< [-1, 1] */ 
																	double dPIDResultPitchNormalized, 
																	double dPIDResultRollNormalized,
																	double dMotor1Result, /*< [0, 1] */ 
																	double dMotor2Result, 
																	double dMotor3Result, 
																	double dMotor4Result)
{
	ItlPrintYawPitchRollLine(0, 'Y', dCurrentYawInDegrees, dDesiredYawInDegrees, dPIDResultYawNormalized);
	ItlPrintYawPitchRollLine(64, 'P', dCurrentPitchInDegrees, dDesiredPitchInDegrees, dPIDResultPitchNormalized);
	ItlPrintYawPitchRollLine(20, 'R', dCurrentRollInDegrees, dDesiredRollInDegrees, dPIDResultRollNormalized);

	ItlPrintResultingMotorValues(84, dMotor1Result, dMotor2Result, dMotor3Result, dMotor4Result);
}

void SerialDebugDisplay20x4::ItlPrintYawPitchRollLine(int iStartingPosition, char cOrientationIdentificator, double dCurrentValue, double dDesiredValue, double dPIDResult)
{
	static char sDoubleToStringPuffer_1[25];
	static char sDoubleToStringPuffer_2[25];
	static char sDoubleToStringPuffer_3[25];
	static char sLinePuffer[25];

	static char *sAdditionalSignedCharacterPuffer = NULL;

	// Create strings from given values
	dtostrf(dCurrentValue, 6, 1, sDoubleToStringPuffer_1);
	dtostrf(dDesiredValue, 6, 1, sDoubleToStringPuffer_2);
	dtostrf(dPIDResult, 4, 2, sDoubleToStringPuffer_3);

	// Create sign character if needed
	if (dPIDResult >= 0.0)
		sAdditionalSignedCharacterPuffer = "+";
	else
		sAdditionalSignedCharacterPuffer = ""; //< in this case, the sign character is added to the stringified value automatically

	// Combine line
	sprintf(sLinePuffer, "%c%s %s %s%s\0", cOrientationIdentificator, sDoubleToStringPuffer_1, sDoubleToStringPuffer_2, sAdditionalSignedCharacterPuffer, sDoubleToStringPuffer_3);

	// Write line
	ItlMoveToPosition(iStartingPosition);
	m_rSerialOutputDevice.write(sLinePuffer);               // write out line
}

void SerialDebugDisplay20x4::ItlPrintResultingMotorValues(int iStartingPosition, double d1, double d2, double d3, double d4)
{
	static char sBuffer1[20], sBuffer2[20], sBuffer3[20], sBuffer4[20];
	const int nNumMinWidth = 4;
	const int nNumDecimals = 1;
	dtostrf(d1, nNumMinWidth, nNumDecimals + 1, sBuffer1);
	dtostrf(d2, nNumMinWidth, nNumDecimals + 1, sBuffer2);
	dtostrf(d3, nNumMinWidth, nNumDecimals + 1, sBuffer3);
	dtostrf(d4, nNumMinWidth, nNumDecimals + 1, sBuffer4);

	static char sLinePuffer[25];
	sprintf(sLinePuffer, "M%s %s %s %s\0", sBuffer1, sBuffer2, sBuffer3, sBuffer4); // create strings from the numbers    

	// Write line

	m_rSerialOutputDevice.write(sLinePuffer);               // write out line
}

void SerialDebugDisplay20x4::PrintSettingsOverview(float fP, float fD, float fI)
{
	//ItlClearDisplay();
	ItlPrintSettings(0, fP, fD, fI);

	ItlMoveToPosition(20);
	m_rSerialOutputDevice.write(m_szDebugMessage);
}

void SerialDebugDisplay20x4::ItlMoveToPosition(int iPosition)
{
	m_rSerialOutputDevice.write(254);                 // Activate command mode (next byte is the command)
	m_rSerialOutputDevice.write(128 + iPosition);     // Send command 128 ("Set Cursor") plus data value (=position, see reference)
}

void SerialDebugDisplay20x4::ItlClearDisplay()
{
	m_rSerialOutputDevice.write(254);    // Activate command mode (next byte is the command)
	m_rSerialOutputDevice.write(1);      // Send command 1 ("Clear display")
}

void SerialDebugDisplay20x4::ItlPrintSettings(int iStartingPosition, float fP, float fD, float fI)
{
	static char sBuffer1[20], sBuffer2[20], sBuffer3[20];
	dtostrf(fP, 0, 3, sBuffer1);
	dtostrf(fD, 0, 3, sBuffer2);
	dtostrf(fI, 0, 3, sBuffer3);

	static char sLinePuffer[25];
	sprintf(sLinePuffer, "P%sD%sI%s", sBuffer1, sBuffer2, sBuffer3); // create strings from the numbers    

	ItlMoveToPosition(iStartingPosition);
	m_rSerialOutputDevice.write(sLinePuffer);
}

void SerialDebugDisplay20x4::SetAssertMessage(const char * szMessage)
{
	if (szMessage != m_szDebugMessage) 
	{
		size_t lenOfMessage = strlen(szMessage);

		strncpy(m_szDebugMessage, szMessage, lenOfMessage < 60 ? lenOfMessage : 60);
	}		
}

// cmoellinger, 17.01.2015

#include "SerialDebugDisplay20x4.h"
#include "assert.h"

void SerialDebugDisplay20x4::Print_Orientations_PID_And_MotorValues(float dCurrentYawInDegrees, /*< [0, 360[ */
	float dCurrentPitchInDegrees,
	float dCurrentRollInDegrees,
	float dDesiredYawInDegrees,
	float dDesiredPitchInDegrees,
	float dDesiredRollInDegrees,
	float dPIDResultYawNormalized, /*< [-1, 1] */
	float dPIDResultPitchNormalized,
	float dPIDResultRollNormalized,
	float dMotor1Result, /*< [0, 1] */
	float dMotor2Result,
	float dMotor3Result,
	float dMotor4Result)
{
	ItlPrintYawPitchRollLine(0, 'Y', dCurrentYawInDegrees, dDesiredYawInDegrees, dPIDResultYawNormalized);
	ItlPrintYawPitchRollLine(64, 'P', dCurrentPitchInDegrees, dDesiredPitchInDegrees, dPIDResultPitchNormalized);
	ItlPrintYawPitchRollLine(20, 'R', dCurrentRollInDegrees, dDesiredRollInDegrees, dPIDResultRollNormalized);

	ItlPrintResultingMotorValues(84, dMotor1Result, dMotor2Result, dMotor3Result, dMotor4Result);
}

void SerialDebugDisplay20x4::ItlPrintYawPitchRollLine(int iStartingPosition, char cOrientationIdentificator, float dCurrentValue, float dDesiredValue, float dPIDResult)
{
	char sDoubleToStringPuffer_1[25] = "";
	char sDoubleToStringPuffer_2[25] = "";;
	char sDoubleToStringPuffer_3[25] = "";;
	char sLinePuffer[25] = "";;

	char *sAdditionalSignedCharacterPuffer = NULL;

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
	assert(strlen(sLinePuffer) <= 20);

	// Write line
	ItlMoveToPosition(iStartingPosition);
	m_rSerialOutputDevice.write(sLinePuffer);               // write out line
}

void SerialDebugDisplay20x4::ItlPrintResultingMotorValues(int iStartingPosition, float d1, float d2, float d3, float d4)
{
	char	sBuffer1[20] = "", 
			sBuffer2[20] = "",
			sBuffer3[20] = "",
			sBuffer4[20] = "";
	const int nNumMinWidth = 4;
	const int nNumDecimals = 1;
	dtostrf(d1, nNumMinWidth, nNumDecimals + 1, sBuffer1);
	dtostrf(d2, nNumMinWidth, nNumDecimals + 1, sBuffer2);
	dtostrf(d3, nNumMinWidth, nNumDecimals + 1, sBuffer3);
	dtostrf(d4, nNumMinWidth, nNumDecimals + 1, sBuffer4);

	char sLinePuffer[25];
	sprintf(sLinePuffer, "M%s %s %s %s\0", sBuffer1, sBuffer2, sBuffer3, sBuffer4); // create strings from the numbers    
	assert(strlen(sLinePuffer) <= 20);

	// Write line
	ItlMoveToPosition(iStartingPosition);
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
	assert(strlen(sLinePuffer) <= 20);

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

SerialDebugDisplay20x4::SerialDebugDisplay20x4(Stream &rSerialOutputDevice) 
	: m_rSerialOutputDevice(rSerialOutputDevice) 
{ 
	for (unsigned int n = 0; n < 60; n++) 
		m_szDebugMessage[n] = 'a'; 
	m_szDebugMessage[60] = 0; 

	rSerialOutputDevice.write(0x7C);
	rSerialOutputDevice.write(140);
};

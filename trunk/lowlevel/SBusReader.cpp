// ch.moellinger@gmail.com, 05/2014
#include <HardwareSerial.h>

#include "SBusReader.h"
#include "config.h"
#include "debug.h"
#include "assert.h"

#define SBUS_BAUDRATE 100000
#define SBUS_DEVICE LOWLEVELCONFIG_SBUS_DEVICE

#define SBUS_PAYLOAD_STARTBYTE  0x0F
#define SBUS_PAYLOAD_ENDBYTE    0x00


void SBusReader::begin()
{
	// set configuration for serial port 
	// (SBUS uses 8 data bits, 2 stop bits, even parity, and an inverted signal)
	SBUS_DEVICE.begin(SBUS_BAUDRATE, SERIAL_9O1_RXINV);

	// clear buffer for read data
	memset(m_pReadSBusData, NULL, 25);

	m_bDataAvailable = false;
	m_iCurBufferIndex = 0;
	m_bIsReadingPayload = false;

	// add some counters for evaluation
	m_iCorrectBytes = 0;
	m_iWrongBytes = 0;

	// reset memory for signal quality evaluation
	memset(m_pbLastSignalStates, false, NUM_FRAMES_USED_FOR_QUALITY_EVALUATION * sizeof(bool));
	m_iCurPtrForLastSignalStates = 0;
}

void printHex(int num, int precision) {
	char tmp[16];
	char format[128];

	sprintf(format, "0x%%.%dX", precision);

	sprintf(tmp, format, num);
	LOWLEVELCONFIG_DEBUG_UART.print(tmp);
}

bool SBusReader::FetchChannelData(int16_t *pTarget, uint8_t &rStatusByte)
{
	if (m_bDataAvailable)
	{
		ItlUpdateChannelValuesFromReadSBUSData();
		m_bDataAvailable = false;
	}

	memcpy(pTarget, m_pLastChannelValues, sizeof(int16_t) * NUM_CHANNELS);
	rStatusByte = m_nLastStatusByte;

	return true;
}

void SBusReader::ProcessInput(void)
{
	if (SBUS_DEVICE.available() > 24)
	{
		//debug_print("SBUS data available: ");
		//debug_println(SBUS_DEVICE.available());

		while (SBUS_DEVICE.available() > 0)
		{
			uint8_t cReadByte = SBUS_DEVICE.read();

			if (m_bIsReadingPayload)
			{
				m_iCurBufferIndex++;
				m_pTempInBuffer[m_iCurBufferIndex] = cReadByte;

				if (m_iCurBufferIndex == 24)
				{
					if (m_pTempInBuffer[0] == SBUS_PAYLOAD_STARTBYTE &&
						m_pTempInBuffer[24] == SBUS_PAYLOAD_ENDBYTE)
					{
						// Start and end byte match, we can assume that we had
						// read the complete line successfully
						ItlFinishReadingPayload();
					}
					else
					{
						bool bShifted = false;

						// We assume that the current start byte actually was a
						// data byte, not a start byte. So loop through buffer for next
						// start byte and shift data (e.g., if 0F is the start byte:
						// (0F 00 01 02 03 0F 0A 0B 0C) --> (0F 0A 0B [...])

						for (unsigned int n = 1; n < 24 && !bShifted; ++n)
						{
							if (m_pTempInBuffer[n] == SBUS_PAYLOAD_STARTBYTE)
							{
								bShifted = true;

								for (unsigned int nn = 0; nn < n; ++nn)
									m_pTempInBuffer[nn] = m_pTempInBuffer[n + nn];

								m_iCurBufferIndex -= n;
								assert(m_iCurBufferIndex >= 0);

								m_iWrongBytes += n;
							}

						}

						if (bShifted == false)
						{
							debug_println("ABORT READING PAYLOAD");

							// something went wrong, abort
							ItlAbortReadingPayload();
						}
					}
				}
			}
			else
			{
				bool bIsStartByte = (cReadByte == SBUS_PAYLOAD_STARTBYTE);
				bool bResyncedToStartByte = (bIsStartByte == false) ? ItlResyncTo(SBUS_PAYLOAD_STARTBYTE) : false;

				if (bIsStartByte || bResyncedToStartByte)
				{
					if (bResyncedToStartByte)
						debug_println("RESYNCED_TO_STARTBYTE");

					ItlStartReadingPayload();
				}
				else
				{
					debug_println("ABORTED");

					ItlAbortReadingPayload();
				}
			}
		}
	}

	debug_println(m_iCorrectBytes / (float) (m_iWrongBytes + m_iCorrectBytes));
}

bool SBusReader::ItlResyncTo(uint8_t nSearchedByte)
{
	// read until we find the searched byte to resync or until 
	// no more data is available
	while (SBUS_DEVICE.available() > 0)
	{
		m_iWrongBytes += 1;

		uint8_t cReadByte = SBUS_DEVICE.read();
		if (cReadByte == nSearchedByte)
			return true;
	}

	return false;
}

void SBusReader::ItlStartReadingPayload()
{
	m_iCurBufferIndex = 0;
	m_pTempInBuffer[0] = 0x0F;
	m_pTempInBuffer[24] = 0xFF;
	m_bIsReadingPayload = true;
}

void SBusReader::ItlAbortReadingPayload()
{
	memset(m_pTempInBuffer, NULL, sizeof(uint8_t) * 25);
	m_bIsReadingPayload = false;

	m_iWrongBytes += m_iCurBufferIndex;
}

void SBusReader::ItlFinishReadingPayload()
{
	// stop reading payload
	m_bIsReadingPayload = false;

	// copy read payload
	memcpy(m_pReadSBusData, m_pTempInBuffer, sizeof(uint8_t) * 25);

	ItlUpdateSignalQualityCounter();

	// set flag to let data be fetched
	m_iCorrectBytes		+= 25;
	m_bDataAvailable	= true;
}

void SBusReader::ItlUpdateSignalQualityCounter()
{
	// read status byte and add state in signal-quality-ringbuffer
	m_pbLastSignalStates[m_iCurPtrForLastSignalStates++] = (m_pReadSBusData[23] == SBUS_SIGNAL_OK);
	if (m_iCurPtrForLastSignalStates == NUM_FRAMES_USED_FOR_QUALITY_EVALUATION)
		m_iCurPtrForLastSignalStates = 0;
}

void SBusReader::ItlUpdateChannelValuesFromReadSBUSData()
{
	if (m_bDataAvailable)
	{
		static_assert(NUM_CHANNELS == 7, "Channel conversion only available for 7 channels");
		m_pLastChannelValues[0] = ((m_pReadSBusData[1] | m_pReadSBusData[2] << 8) & 0x07FF);
		m_pLastChannelValues[1] = ((m_pReadSBusData[2] >> 3 | m_pReadSBusData[3] << 5) & 0x07FF);
		m_pLastChannelValues[2] = ((m_pReadSBusData[3] >> 6 | m_pReadSBusData[4] << 2 | m_pReadSBusData[5] << 10) & 0x07FF);
		m_pLastChannelValues[3] = ((m_pReadSBusData[5] >> 1 | m_pReadSBusData[6] << 7) & 0x07FF);
		m_pLastChannelValues[4] = ((m_pReadSBusData[6] >> 4 | m_pReadSBusData[7] << 4) & 0x07FF);
		m_pLastChannelValues[5] = ((m_pReadSBusData[7] >> 7 | m_pReadSBusData[8] << 1 | m_pReadSBusData[9] << 9) & 0x07FF);
		m_pLastChannelValues[6] = ((m_pReadSBusData[9] >> 2 | m_pReadSBusData[10] << 6) & 0x07FF);

		// Failsafe
		m_nLastStatusByte = SBUS_SIGNAL_OK;
		if (m_pReadSBusData[23] & (1 << 2))
			m_nLastStatusByte |= SBUS_SIGNAL_LOST;
		if (m_pReadSBusData[23] & (1 << 3))
			m_nLastStatusByte |= SBUS_SIGNAL_FAILSAFE;

		float fQuality = 0.0f;
		for (unsigned int n = 0; n < NUM_FRAMES_USED_FOR_QUALITY_EVALUATION; ++n)
			fQuality += (m_pbLastSignalStates[n]) ? 1.0f / NUM_FRAMES_USED_FOR_QUALITY_EVALUATION : 0.0f;
	}
}

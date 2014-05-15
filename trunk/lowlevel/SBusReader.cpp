// ch.moellinger@gmail.com, 05/2014

#include "SBusReader.h"
#include "config.h"

#define SBUS_BAUDRATE 100000
#define SBUS_DEVICE LOWLEVELCONFIG_SBUS_DEVICE

#define SBUS_PAYLOAD_STARTBYTE  0x0F
#define SBUS_PAYLOAD_ENDBYTE    0x00

void SBusReader::begin()
{
        // set configuration for serial port (on Arduino Due, no preconfigured 
        // settings for Serial.begin() are available)
        // S-BUS uses an even parity and 2 stop bits
       	SBUS_DEVICE.begin(SBUS_BAUDRATE);
        USART3->US_MR = US_MR_USART_MODE_NORMAL | US_MR_USCLKS_MCK | US_MR_CHRL_8_BIT | US_MR_PAR_EVEN |
                US_MR_NBSTOP_2_BIT | US_MR_CHMODE_NORMAL;
                

        memset(m_pReadSBusData, NULL, 25);

	m_bDataAvailable = false;
	m_iCurBufferIndex=0;
	m_bIsReadingPayload = false;
}

bool SBusReader::FetchChannelData(int16_t *pTarget, uint8_t &rStatusByte)
{
  pTarget[0]  = ((m_pReadSBusData[1]|m_pReadSBusData[2]<< 8) & 0x07FF);
  pTarget[1]  = ((m_pReadSBusData[2]>>3|m_pReadSBusData[3]<<5) & 0x07FF);
  pTarget[2]  = ((m_pReadSBusData[3]>>6|m_pReadSBusData[4]<<2|m_pReadSBusData[5]<<10) & 0x07FF);
  pTarget[3]  = ((m_pReadSBusData[5]>>1|m_pReadSBusData[6]<<7) & 0x07FF);
  pTarget[4]  = ((m_pReadSBusData[6]>>4|m_pReadSBusData[7]<<4) & 0x07FF);
  pTarget[5]  = ((m_pReadSBusData[7]>>7|m_pReadSBusData[8]<<1|m_pReadSBusData[9]<<9) & 0x07FF);
  pTarget[6]  = ((m_pReadSBusData[9]>>2|m_pReadSBusData[10]<<6) & 0x07FF);
  
  // Failsafe
  rStatusByte = SBUS_SIGNAL_OK;
  if (m_pReadSBusData[23] & (1<<2))
    rStatusByte |= SBUS_SIGNAL_LOST;
  if (m_pReadSBusData[23] & (1<<3))
    rStatusByte |= SBUS_SIGNAL_FAILSAFE;
  
  m_bDataAvailable = false;
/*  channels[7]  = ((sbusData[10]>>5|sbusData[11]<<3) & 0x07FF); // & the other 8 + 2 channels if you need them
  #ifdef ALL_CHANNELS
  channels[8]  = ((sbusData[12]|sbusData[13]<< 8) & 0x07FF);
  channels[9]  = ((sbusData[13]>>3|sbusData[14]<<5) & 0x07FF);
  channels[10] = ((sbusData[14]>>6|sbusData[15]<<2|sbusData[16]<<10) & 0x07FF);
  channels[11] = ((sbusData[16]>>1|sbusData[17]<<7) & 0x07FF);
  channels[12] = ((sbusData[17]>>4|sbusData[18]<<4) & 0x07FF);
  channels[13] = ((sbusData[18]>>7|sbusData[19]<<1|sbusData[20]<<9) & 0x07FF);
  channels[14] = ((sbusData[20]>>2|sbusData[21]<<6) & 0x07FF);
    channels[15] = ((sbusData[21]>>5|sbusData[22]<<3) & 0x07FF);
  #endif*/
}

void SBusReader::ProcessInput(void)
{
  if (SBUS_DEVICE.available() > 24)
  {
    while(SBUS_DEVICE.available() > 0)
    {
      uint8_t cReadByte = SBUS_DEVICE.read();
      
      if (m_bIsReadingPayload)
      {
        m_iCurBufferIndex ++;
        m_pTempInBuffer[m_iCurBufferIndex] = cReadByte;

        if (m_iCurBufferIndex == 24)
        {
          if (m_pTempInBuffer[0]  ==  SBUS_PAYLOAD_STARTBYTE && 
              m_pTempInBuffer[24] ==  SBUS_PAYLOAD_ENDBYTE)
          {
            // Start and end byte match, we can assume that we had
            // read the complete line successfully
            ItlFinishReadingPayload();
          }
          else
          {
            // something went wrong, abort
            ItlAbortReadingPayload();
          }
        }
      }
      else
      {
        if (cReadByte == SBUS_PAYLOAD_STARTBYTE || ItlResyncTo(SBUS_PAYLOAD_STARTBYTE))
          ItlStartReadingPayload();
        else
          ItlAbortReadingPayload();
      }
    }
  }
}

bool SBusReader::ItlResyncTo(uint8_t nSearchedByte)
{
  // read until we find the searched byte to resync or until 
  // no more data is available
  while(SBUS_DEVICE.available() > 0)
  {
    uint8_t cReadByte = SBUS_DEVICE.read();
    if (cReadByte == nSearchedByte)
      return true;
  }
  
  return false;
}

void SBusReader::ItlStartReadingPayload()
{
  m_iCurBufferIndex = 0;
  m_pTempInBuffer[0]  = 0x0F;
  m_pTempInBuffer[24] = 0xFF;
  m_bIsReadingPayload = true;
}

void SBusReader::ItlAbortReadingPayload()
{
 // memset(m_pTempInBuffer, NULL, 25);
  m_bIsReadingPayload = false;
}

void SBusReader::ItlFinishReadingPayload()
{
  // stop reading payload
  m_bIsReadingPayload = false;
  
  // copy read payload
  memcpy(m_pReadSBusData,m_pTempInBuffer,25);

  // set flag to let data be fetched
  m_bDataAvailable = true;
}

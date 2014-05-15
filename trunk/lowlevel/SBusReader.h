// ch.moellinger@gmail.com, 05/2014

#ifndef _SBUSREADER_H_
#define _SBUSREADER_H_

#include <Arduino.h>


#define SBUS_SIGNAL_OK          0x00
#define SBUS_SIGNAL_LOST        0x01
#define SBUS_SIGNAL_FAILSAFE    0x02

class SBusReader
{
  public:
    void begin(void);

    void ProcessInput(void);
    bool FetchChannelData(int16_t *pTarget, uint8_t &rStatusByte);

    bool IsDataAvailable() const { return m_bDataAvailable; }
  
  private:
    void ItlStartReadingPayload();
    void ItlAbortReadingPayload();
    void ItlFinishReadingPayload();
    void ItlSuccessfullReadPayload();
    
    /// Tries to resync by reading bytes until the searched byte is found and consumed
    /// returns true if searched byte was found and consumed, false if no data is left and it was not found
    bool ItlResyncTo(uint8_t nSearchedByte);
    
    bool m_bDataAvailable;
    bool m_bIsReadingPayload;
    
    int  m_iCurBufferIndex;
    
    // A temporary buffer, used to 
    // process the input data. If the line is 
    // completely received and valid, it is copied to m_pReadSBusData
    uint8_t m_pTempInBuffer[25];
    
    // The latest read sbus data
    uint8_t m_pReadSBusData[25];
                

};

#endif //_SBUSREADER_H_

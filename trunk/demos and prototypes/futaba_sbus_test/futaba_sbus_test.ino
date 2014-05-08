#include "FUTABA_SBUS.h"


FUTABA_SBUS sBus;

void setup(){

  sBus.begin();
  Serial.begin(9600);
  Serial.println("Initialized!");
}

float NormalizeChannelData(int16_t iInput)
{
  return (iInput - 1024) / 2048.0f + 0.5f;
}

void loop(){
  sBus.ProcessInput();
  
  if (sBus.IsDataAvailable())
  {
    int16_t pChannelData[7];
    uint8_t nStatusByte;
    
    sBus.FetchChannelData(pChannelData, nStatusByte);

    Serial.print(nStatusByte);
    
    Serial.print("# [1]");
    Serial.print(NormalizeChannelData(pChannelData[0]));
    Serial.print(" [2]");
    Serial.print(NormalizeChannelData(pChannelData[1]));
    Serial.print(" [3]");
    Serial.print(NormalizeChannelData(pChannelData[2]));
    Serial.print(" [4]");
    Serial.print(NormalizeChannelData(pChannelData[3]));
    Serial.print(" [5]");
    Serial.print(NormalizeChannelData(pChannelData[4]));
    Serial.print(" [6]");
    Serial.print(NormalizeChannelData(pChannelData[5]));
    Serial.print(" [7]");
    Serial.print(NormalizeChannelData(pChannelData[6]));
    Serial.println();
  }
}

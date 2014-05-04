#include "FUTABA_SBUS.h"
#include <Streaming.h>


FUTABA_SBUS sBus;
bool bSet = true;

void setup(){
  pinMode(13, OUTPUT);
  digitalWrite(13, LOW);
  delay(500);
  digitalWrite(13, HIGH);
  sBus.begin();
  delay(500);
  digitalWrite(13, LOW);
  
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(4, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(8, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
 // Serial.begin(9600);
}

void loop(){
  sBus.FeedLine();
  if (sBus.toChannels == 1){
    sBus.UpdateServos();
    sBus.UpdateChannels();
    sBus.toChannels = 0;
   // Serial<<sBus.channels[0]<<","<<sBus.channels[1]<<","<<sBus.channels[2]<<"\r\n";
  }

  for (int pin = 2; pin <=10; pin++)
    digitalWrite(pin, LOW);
    
  int iActivePin = (sBus.channels[0] / 2048.0) * 11 + 1;
  digitalWrite(iActivePin, HIGH);
//  Serial << analogRead(0) << "," << analogRead(1) << "," << analogRead(2) << "\r\n";
  //Serial.println(analogRead(0));

}

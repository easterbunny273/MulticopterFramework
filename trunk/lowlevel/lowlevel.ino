// Include necessary arduino libraries
// Please note that it seems that they must 
// always be included in the *.ino file
#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(16, 6, NEO_GRB + NEO_KHZ800);

// Include project libraries
#include "SBusReader.h"
#include "IGyroReader.h"
#include "GyroReader_MPU6050.h"
#include "helper_3dmath.h"
#include "NauticalOrientation.h"

SBusReader sBus;
IGyroReader * pGyroReader = NULL;

NauticalOrientation SollLage, IstLage;
bool bSollInitialized = false;

void resetColorRing(bool bShow = true)
{
	  for(uint16_t i=0; i<strip.numPixels(); i++) 
	  {
      strip.setPixelColor(i, 0);
	  }

	  if (bShow)
		strip.show();
}


// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) 
  {
	  //resetColorRing();
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}


void setup()
{
	strip.begin();
	strip.show(); // Initialize all pixels to 'off'
	resetColorRing();

	// 0.0) Setup Debug Device and LED 13 (for debugging)
#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	LOWLEVELCONFIG_DEBUG_DEVICE.begin(115200);
#endif
	pinMode(13, OUTPUT);
	pinMode(5, OUTPUT);

	colorWipe(strip.Color(0, 0, 20), 20); // Blue
	delay(100);
	
	digitalWrite(5, LOW);
	delay(1000);
	digitalWrite(5, HIGH);
	delay(200);
	
	// 0.1) Setup I2C
	Wire.begin();

	// 1) Setup SBus
	sBus.begin();

	// 2) Setup Gyro
	pGyroReader = new GyroReader_MPU6050;
	bool bGyroInitialized = pGyroReader->begin();

	resetColorRing();
	if (bGyroInitialized)
		colorWipe(strip.Color(0, 20, 0), 40); // Green
	else
		colorWipe(strip.Color(20, 0, 0), 40); // Red

	// 3) Setup serial connection to high level device
	
}


void loop()
{

	bool		bIstIsValid = false;

	// 0) Read HighLevel Input, if connected


	// 1) Process Gyro Input -> calculate quaternion (="q_ist")
	bool bProcessed = pGyroReader->processData();

	// cm 16.05 Debug Output, not really working yet
	if (bProcessed)
	{
		float fYaw, fPitch, fRoll;
		pGyroReader->getYawPitchRoll(fYaw, fPitch, fRoll);

		IstLage.pitch	= fPitch * 180.0f / 3.1415f;
		IstLage.yaw		= fYaw  * 180.0f / 3.1415f;
		IstLage.roll	= fRoll  * 180.0f / 3.1415f;

		// If "SollLage" is not initialized yet, set Yaw to current Yaw
		if (bSollInitialized == false)
		{
			SollLage.yaw = fYaw;
			bSollInitialized = true;
		}

	/*	debug_print("yaw("); debug_print(IstLage.yaw); debug_print("["); debug_print(SollLage.yaw); debug_print("]) ");
		debug_print("pitch("); debug_print(IstLage.pitch); debug_print("["); debug_print(SollLage.pitch); debug_print("]) ");
		debug_print("roll("); debug_print(IstLage.roll); debug_print("["); debug_print(SollLage.roll); debug_println("]) ");*/

		//resetColorRing();
		int iPixel = ((IstLage.yaw-SollLage.yaw) / 360.0) * 16.0;
		while (iPixel < 0) iPixel += 16;
		while (iPixel >= 16) iPixel -= 16;
		resetColorRing(false);
		if (iPixel!=0)
			strip.setPixelColor(iPixel, 20, 0, 0);
		else
			strip.setPixelColor(iPixel, 20, 20, 20);
		strip.show();
		//#if LOWLEVELCONFIG_ENABLE_DEBUGGING
		digitalWrite(13, (fPitch > 0) ? HIGH : LOW);
		//#endif
	}

	bIstIsValid = bProcessed;



	// 2) Process SBus Input
	// 2.1) extract "mode" from an input channel, build quaternion "q_rc" from two input channels (nick+roll)
	// 2.2) if mode == RC_ABSOLUTE, set quaternion "q_soll" = "q_rc"
	//		if mode == RC_RELATIVE, set quaternion "q_soll" = "q_soll" + "q_rc"
	//		if mode == RC_HIGHLEVEL, set quaternion "q_soll" from highlevel input

	sBus.ProcessInput();

	int16_t pChannels[7];
	uint8_t nStatus;
	sBus.FetchChannelData(pChannels, nStatus);
	/*debug_print(nStatus);
	debug_print("-");*/

	if (nStatus == 0)
	{
		SollLage.roll += (pChannels[0] - 1024) / 100;
		SollLage.pitch += (pChannels[1] - 1024) / 100;
		SollLage.yaw += (pChannels[3] - 1024) / 100;
	}


	// 3) calculate difference between "q_rc" and "q_soll"

	// 4) calculate outputs for ESCs
}

// Include necessary arduino libraries
// Please note that it seems that they must 
// always be included in the *.ino file
#include <Arduino.h>
#include <Wire.h>
#include <Servo.h>

// Include project libraries
#include "SBusReader.h"
#include "IGyroReader.h"
#include "GyroReader_MPU6050.h"
#include "helper_3dmath.h"
#include "NauticalOrientation.h"
#include "ThrottleCalculator_Quadro.h"

SBusReader sBus;
IGyroReader * pGyroReader = NULL;

NauticalOrientation SollLage, IstLage;
bool bSollInitialized = false;
int16_t gLastChannelValues[7];

ThrottleCalculator_Quadro ThrottleCalculator;

Servo ESC_FrontLeft, ESC_FrontRight, ESC_RearLeft, ESC_RearRight;

void setup()
{
	// 0.0) Setup Debug Device and PINS 
#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	LOWLEVELCONFIG_DEBUG_DEVICE.begin(57400);
#endif

	// We use pin 13 for debug stuff (ON = Gyro works, OFF = Gyro doesnt work)
	pinMode(13, OUTPUT);
	digitalWrite(13, LOW);

	// We use pin 5 do cut-off gyro (fixing initializing problems)
	pinMode(5, OUTPUT);
		
	// Shut down Gyro for a few milliseconds.
	// Pin 5 should be connected to a corresponding transistor 
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

	if (bGyroInitialized)
		digitalWrite(13, HIGH);
	else
		digitalWrite(13, LOW);
	
	// 3) Setup output pins for ESCs
	ESC_FrontLeft.attach(OUTPUT_PIN_ESC_FRONT_LEFT);
	ESC_FrontRight.attach(OUTPUT_PIN_ESC_FRONT_RIGHT);
	ESC_RearLeft.attach(OUTPUT_PIN_ESC_REAR_LEFT);
	ESC_RearRight.attach(OUTPUT_PIN_ESC_REAR_RIGHT);

	// 4) Setup serial connection to high level device
	
}


void loop()
{
	bool		bIstIsValid = false;

	// 0) Read HighLevel Input, if connected
	//    TODO

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
	}

	bIstIsValid = bProcessed;



	// 2) Process SBus Input
	// 2.1) extract "mode" from an input channel, build quaternion "q_rc" from two input channels (nick+roll)
	// 2.2) if mode == RC_ABSOLUTE, set quaternion "q_soll" = "q_rc"
	//		if mode == RC_RELATIVE, set quaternion "q_soll" = "q_soll" + "q_rc"
	//		if mode == RC_HIGHLEVEL, set quaternion "q_soll" from highlevel input

	sBus.ProcessInput();
	

	if (sBus.IsDataAvailable())
	{
		int16_t pChannels[7];
		uint8_t nStatus;
		sBus.FetchChannelData(pChannels, nStatus);

		if (nStatus == 0)
		{
			SollLage.roll += (pChannels[0] - 1024) / 100;
			SollLage.pitch += (pChannels[1] - 1024) / 100;
			SollLage.yaw += (pChannels[3] - 1024) / 100;

			// QUICK HACK, see below
			memcpy(gLastChannelValues, pChannels, sizeof(int16_t) * 7);
		}
	}
	
	// 3) calculate difference between "q_rc" and "q_soll"
	// 
	// Calculate fPitch, fRoll, fYaw in [-1.0; 1.0].
	// Those variables should describe the wanted motion, fPitch > 0.0f should force the copter to 
	// put the nose up, while when fPitch = fRoll = fYaw = 0.0f, 
	// all 4 motors should run at the same speed.

	// QUICK_HACK: While we have no PID governor and no feedback loop from the gyro,
	// we simply use the input from the radio receiver to set these values

	float fPitch = (gLastChannelValues[1] - 1024) / 1024.0;
	float fRoll = (gLastChannelValues[0] - 1024) / 1024.0;
	float fYaw = (gLastChannelValues[3] - 1024) / 1024.0;

	// the target throttle value
	float fThrottle = gLastChannelValues[2] / 2048.0;

	// 4) calculate outputs for ESCs
	float fThrottleFrontLeft, fThrottleFrontRight, fThrottleRearLeft, fThrottleRearRight;

	ThrottleCalculator.Calculate(fPitch, fRoll, fYaw, fThrottle, fThrottleFrontLeft, fThrottleFrontRight, fThrottleRearLeft, fThrottleRearRight);

	ESC_FrontLeft.write(map(fThrottleFrontLeft * 1000, 0, 1000, 0, 179));
	ESC_FrontRight.write(map(fThrottleFrontRight * 1000, 0, 1000, 0, 179));
	ESC_RearLeft.write(map(fThrottleRearLeft * 1000, 0, 1000, 0, 179));
	ESC_RearRight.write(map(fThrottleRearRight * 1000, 0, 1000, 0, 179));

	debug_print("lf: "); debug_print(fThrottleFrontLeft); debug_print(" rf: "); debug_print(fThrottleFrontRight);
	debug_print(" lr: "); debug_print(fThrottleRearLeft); debug_print(" rr: "); debug_println(fThrottleRearRight);
}

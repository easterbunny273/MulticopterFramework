// Include necessary arduino libraries
// Please note that it seems that they must 
// always be included in the *.ino file
#include <Arduino.h>
#include <Wire.h>

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

void setup()
{
	// 0.0) Setup Debug Device and LED 13 (for debugging)
#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	LOWLEVELCONFIG_DEBUG_DEVICE.begin(9600);
	pinMode(13, OUTPUT);
#endif

	// 0.1) Setup I2C
	Wire.begin();

	// 1) Setup SBus
	sBus.begin();

	// 2) Setup Gyro
	pGyroReader = new GyroReader_MPU6050;
	pGyroReader->begin();

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

		IstLage.pitch	= fPitch;
		IstLage.yaw		= fYaw;
		IstLage.roll	= fRoll;

		// If "SollLage" is not initialized yet, set Yaw to current Yaw
		if (bSollInitialized == false)
		{
			SollLage.yaw = fYaw;
			bSollInitialized = true;
		}

		debug_print("yaw("); debug_print(IstLage.yaw); debug_print("["); debug_print(SollLage.yaw); debug_print("]) ");
		debug_print("pitch("); debug_print(IstLage.pitch); debug_print("["); debug_print(SollLage.pitch); debug_print("]) ");
		debug_print("roll("); debug_print(IstLage.roll); debug_print("["); debug_print(SollLage.roll); debug_println("]) ");

		/*debug_print("q_ist: w("); debug_print(q_ist.w); debug_print(") ");
		debug_print(" x("); debug_print(q_ist.x); debug_print(") ");
		debug_print(" y("); debug_print(q_ist.y); debug_print(") ");
		debug_print(" z("); debug_print(q_ist.z); debug_println(")");*/
	}

	bIstIsValid = bProcessed;

#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	digitalWrite(13, bIstIsValid ? HIGH : LOW);
#endif

	// 2) Process SBus Input
	// 2.1) extract "mode" from an input channel, build quaternion "q_rc" from two input channels (nick+roll)
	// 2.2) if mode == RC_ABSOLUTE, set quaternion "q_soll" = "q_rc"
	//		if mode == RC_RELATIVE, set quaternion "q_soll" = "q_soll" + "q_rc"
	//		if mode == RC_HIGHLEVEL, set quaternion "q_soll" from highlevel input

	// 3) calculate difference between "q_rc" and "q_soll"

	// 4) calculate outputs for ESCs
}

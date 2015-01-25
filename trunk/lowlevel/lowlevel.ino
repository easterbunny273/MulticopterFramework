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
#include "PIDRegler.h"
#include "Utilities.h"
#include "debug.h"
#include "SerialDebugDisplay20x4.h"

SBusReader sBus;
IGyroReader * pGyroReader = NULL;

NauticalOrientation SollLage, IstLage;
bool bSollInitialized = false;


ThrottleCalculator_Quadro ThrottleCalculator;
SerialDebugDisplay20x4 *pDisplay = NULL;

#define PID_ROLL_NICK_P 0.6f
#define PID_ROLL_NICK_I 0.001f
#define PID_ROLL_NICK_D 100.0f
#define PID_YAW_P 4.0f
#define PID_YAW_I 0.0f
#define PID_YAW_D 0.0f
#define PID_HERTZ 100

#define NUM_CHANNELS 7
#define RC_CHANNEL_ROLL 0
#define RC_CHANNEL_NICK 1
#define RC_CHANNEL_THROTTLE 2
#define RC_CHANNEL_YAW 3
#define RC_CHANNEL_DEBUG_1 4
#define RC_CHANNEL_PITCH 5
#define RC_CHANNEL_USE_INTEGRAL 6

PIDRegler PIDRegler_Roll(PID_ROLL_NICK_P, PID_ROLL_NICK_I, PID_ROLL_NICK_D, PID_HERTZ);
PIDRegler PIDRegler_Pitch(PID_ROLL_NICK_P, PID_ROLL_NICK_I, PID_ROLL_NICK_D, PID_HERTZ);
PIDRegler PIDRegler_Yaw(PID_YAW_P, PID_YAW_I, PID_YAW_D, PID_HERTZ);

Servo ESC_FrontLeft, ESC_FrontRight, ESC_RearLeft, ESC_RearRight;

unsigned long nStartupTime = 0;

int16_t gLastChannelValues[NUM_CHANNELS];

void setup()
{
	nStartupTime=millis();
	// 0.0) Setup Debug Device and PINS 
#if LOWLEVELCONFIG_ENABLE_DEBUGGING
	LOWLEVELCONFIG_DEBUG_UART.begin(9600);
#endif

	// setup serial device for display (used by debug methods)
	/*Serial2.begin(9600);
	delay(1500);
	Serial2.print(124);
	Serial2.print(0x10);
	delay(3000);*/
	Serial2.begin(9600);
	delay(500);

	pDisplay = new SerialDebugDisplay20x4(Serial2);
	inject_display(pDisplay);

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

	// notification to check if new version was uploaded to teensy
	/*for (unsigned int n = 0; n < 10; n++)
	{
		digitalWrite(13, HIGH);
		delay(200);
		digitalWrite(13, LOW);
		delay(200);
	}*/
	
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

		IstLage.pitch	= fPitch	* 180.0f / 3.141593f;
		IstLage.yaw		= fYaw		* 180.0f / 3.141593f;
		IstLage.roll	= fRoll		* 180.0f / 3.141593f;

		// If "SollLage" is not initialized yet, set Yaw to current Yaw
		if (bSollInitialized == false)
		{
			SollLage.yaw = fYaw;
			bSollInitialized = true;
		}
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
		int16_t pChannels[NUM_CHANNELS];
		uint8_t nStatus;
		sBus.FetchChannelData(pChannels, nStatus);

		int iModus = 0;
		if (gLastChannelValues[RC_CHANNEL_PITCH] > 950 && gLastChannelValues[RC_CHANNEL_PITCH] < 1100) iModus = 1;
		if (gLastChannelValues[RC_CHANNEL_PITCH] > 100 && gLastChannelValues[RC_CHANNEL_PITCH] < 200) iModus = 2;
		if (nStatus == 0)
		{
			if (iModus == 0)
			{
				SollLage.roll = 0;
				SollLage.pitch = 0;
			}
			else if (iModus == 1)
			{
				SollLage.roll = (pChannels[RC_CHANNEL_ROLL] - 1024) / 20.0f;
				SollLage.pitch = (pChannels[RC_CHANNEL_NICK] - 1024) / 20.0f;
			}
			else if (iModus == 2)
			{
				SollLage.roll += (pChannels[RC_CHANNEL_ROLL] - 1024) / 500.0f;
				SollLage.pitch += (pChannels[RC_CHANNEL_NICK] - 1024) / 500.0f;
			}

			float fYawSignal = (pChannels[RC_CHANNEL_YAW] - 1024) / 300.0f;
			SollLage.yaw += (abs(fYawSignal) > 0.02f) ? fYawSignal : 0.0f;
			

			// QUICK HACK, see below
			memcpy(gLastChannelValues, pChannels, sizeof(int16_t) * NUM_CHANNELS);
		}
	}
	
	// 3) calculate difference between "q_rc" and "q_soll"
	// 
	// Calculate fPitch, fRoll, fYaw in [-1.0; 1.0].
	// Those variables should describe the wanted motion, fPitch > 0.0f should force the copter to 
	// put the nose up, while when fPitch = fRoll = fYaw = 0.0f, 
	// all 4 motors should run at the same speed.

	Utilities::Math::Clamp(SollLage.roll, -MAX_ANGLE_SOLL_ROLL, MAX_ANGLE_SOLL_ROLL);
	Utilities::Math::Clamp(SollLage.pitch, -MAX_ANGLE_SOLL_PITCH, MAX_ANGLE_SOLL_PITCH);

	bool bUseIntegral = gLastChannelValues[RC_CHANNEL_USE_INTEGRAL] > 500 ? true : false;
	float fRollDiff = SollLage.roll - IstLage.roll;
	float fPitchDiff = SollLage.pitch - IstLage.pitch;
	float fYawDiff = SollLage.yaw - IstLage.yaw;

	float fRollDiffNormalized = fRollDiff / 180;
	float fPitchDiffNormalized = fPitchDiff / 180;
	float fYawDiffNormalized = fYawDiff / 180;

	float fMagicMultiplier = 2.0f;

	float fReglerOutput_Roll = PIDRegler_Roll.Process(fRollDiffNormalized, bUseIntegral) * fMagicMultiplier;
	float fReglerOutput_Pitch = PIDRegler_Pitch.Process(fPitchDiffNormalized, bUseIntegral) * fMagicMultiplier;
	float fReglerOutput_Yaw = PIDRegler_Yaw.Process(fYawDiffNormalized, bUseIntegral) * fMagicMultiplier;

	// the target throttle value
	float fThrottle = (gLastChannelValues[RC_CHANNEL_THROTTLE] / 2048.0f - 0.18f) * 1.0f / (1.0f - 2.0f*0.18f);

	// 4) calculate outputs for ESCs
	float fThrottleFrontLeft, fThrottleFrontRight, fThrottleRearLeft, fThrottleRearRight;

	//debug_println("Calculate motor values");
	ThrottleCalculator.Calculate(fReglerOutput_Pitch, fReglerOutput_Roll, fReglerOutput_Yaw, fThrottle, fThrottleFrontLeft, fThrottleFrontRight, fThrottleRearLeft, fThrottleRearRight);

	// Map values from [0.0, 1.0] to [0, 179] and send to ESCs
	ESC_FrontLeft.write(map(fThrottleFrontLeft * 1000, 0, 1000, 0, 179));
	ESC_FrontRight.write(map(fThrottleFrontRight * 1000, 0, 1000, 0, 179));
	ESC_RearLeft.write(map(fThrottleRearLeft * 1000, 0, 1000, 0, 179));
	ESC_RearRight.write(map(fThrottleRearRight * 1000, 0, 1000, 0, 179));

	bool bShowDebug = gLastChannelValues[RC_CHANNEL_DEBUG_1] < 500 ? true : false;

	
	if (pDisplay != NULL)
	{
		if (bShowDebug)
			pDisplay->Print_Orientations_PID_And_MotorValues(IstLage, SollLage, fReglerOutput_Yaw, fReglerOutput_Pitch, fReglerOutput_Roll, fThrottleFrontLeft, fThrottleFrontRight, fThrottleRearRight, fThrottleRearLeft);
		else
			pDisplay->PrintSettingsOverview(PID_ROLL_NICK_P, PID_ROLL_NICK_D, bUseIntegral ? PID_ROLL_NICK_I : 0);
	}
	
	//debug_print("freeMemory=");
	//debug_println(freeMemory());

	assert_update_led();
}

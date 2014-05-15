#include "SBusReader.h"

SBusReader sBus;

void setup()
{
	// 1) Setup SBus
	// 2) Setup Gyro
	// 3) Setup serial connection to high level device
}

void loop()
{
	// 0) Read HighLevel Input, if connected

	// 1) Process Gyro Input -> calculate quaternion (="q_ist")

	// 2) Process SBus Input
	// 2.1) extract "mode" from an input channel, build quaternion "q_rc" from two input channels (nick+roll)
	// 2.2) if mode == RC_ABSOLUTE, set quaternion "q_soll" = "q_rc"
	//		if mode == RC_RELATIVE, set quaternion "q_soll" = "q_soll" + "q_rc"
	//		if mode == RC_HIGHLEVEL, set quaternion "q_soll" from highlevel input

	// 3) calculate difference between "q_rc" and "q_soll"

	// 4) calculate outputs for ESCs
}

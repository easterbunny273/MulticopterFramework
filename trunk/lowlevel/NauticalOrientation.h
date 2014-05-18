//chmoellinger 18.05.14

#ifndef _NAUTICALORIENTATION_H_
#define _NAUTICALORIENTATION_H_

struct NauticalOrientation
{
	NauticalOrientation(float fYaw, float fPitch, float fRoll) : yaw(fYaw), pitch(fPitch), roll(fRoll) {};
	NauticalOrientation() : yaw(0), pitch(0), roll(0) {};

	float yaw, pitch, roll;
};

#endif
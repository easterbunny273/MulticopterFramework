// TODO Comment

#include "Utilities.h"

class ThrottleCalculator_Quadro
{
public:
	void Calculate(	float fPitch, 
					float fRoll, 
					float fYaw,
					float fInputThrottle,
					float &rfThrottleFrontLeft, 
					float &rfThrottleFrontRight,
					float &rfThrottleRearLeft, 
					float &rfThrottleRearRight)
	{
		// Based on the 3 input values (fPitch, fRoll, fYaw), calculate the output values
		/*float fBaseThrottle = 4 * fInputThrottle;
		rfThrottleFrontLeft = fBaseThrottle * (0.25f + 0.25f * (fRoll + fPitch - fYaw) / 3.0f);
		rfThrottleFrontRight = fBaseThrottle * (0.25f + 0.25f * (-fRoll + fPitch + fYaw) / 3.0f);
		rfThrottleRearLeft = fBaseThrottle * (0.25f + 0.25f * (fRoll - fPitch + fYaw) / 3.0f);
		rfThrottleRearRight = fBaseThrottle * (0.25f + 0.25f * (-fRoll - fPitch - fYaw) / 3.0f);*/

		float fClampedBaseThrottle = fInputThrottle < 0.8f ? fInputThrottle : 0.8f; //< clamp the input throttle, otherwise the PID does not work at high throttle values

		float fMagicPIDScaling = 0.125f; //< a constant which has been found to scale the PID values to a useable range
		rfThrottleFrontLeft = fClampedBaseThrottle + fMagicPIDScaling*(fRoll + fPitch - fYaw) / 3.0f;
		rfThrottleFrontRight = fClampedBaseThrottle + fMagicPIDScaling*(-fRoll + fPitch + fYaw) / 3.0f;
		rfThrottleRearLeft = fClampedBaseThrottle + fMagicPIDScaling*(fRoll - fPitch + fYaw) / 3.0f;
		rfThrottleRearRight = fClampedBaseThrottle + fMagicPIDScaling*(-fRoll - fPitch - fYaw) / 3.0f;

		// Make sure that all output values are in [0.0f, 1.0f]
		Utilities::Math::Clamp(rfThrottleFrontLeft, 0.0f, 1.0f);
		Utilities::Math::Clamp(rfThrottleFrontRight, 0.0f, 1.0f);
		Utilities::Math::Clamp(rfThrottleRearLeft, 0.0f, 1.0f);
		Utilities::Math::Clamp(rfThrottleRearRight, 0.0f, 1.0f);
	}
};
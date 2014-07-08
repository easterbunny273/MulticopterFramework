// TODO Comment

class ThrottleCalculator_Quadro
{
public:
	void Calculate(float fPitch, float fRoll, float fYaw,
		float fInputThrottle,
		float &rfThrottleFrontLeft, 
		float &rfThrottleFrontRight,
		float &rfThrottleRearLeft, 
		float &rfThrottleRearRight)
	{
		float fBaseThrottle = 4 * fInputThrottle;

		rfThrottleFrontLeft = fBaseThrottle * (0.25f + 0.25f * (fRoll + fPitch - fYaw) / 3.0);
		rfThrottleFrontRight = fBaseThrottle * (0.25f + 0.25f * (-fRoll + fPitch + fYaw) / 3.0);
		rfThrottleRearLeft = fBaseThrottle * (0.25f + 0.25f * (fRoll - fPitch + fYaw) / 3.0);
		rfThrottleRearRight = fBaseThrottle * (0.25f + 0.25f * (-fRoll - fPitch - fYaw) / 3.0);
	}
};
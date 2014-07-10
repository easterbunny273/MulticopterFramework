// TODO: Documentation, ...

class PIDRegler
{
public:
	PIDRegler(float fP, float fI, float fD, unsigned int nSamplingFrequencyInHertz)
	{
		m_fP_Factor = fP;
		m_fI_Factor = fI;
		m_fD_Factor = fD;

		m_fLastError = 0.0f;
		m_fSummedError = 0.0f;
		m_nLastProcessedTime = 0;

		m_nMilliSecondsBetweenProcessSteps = 1000 / nSamplingFrequencyInHertz;
		m_fLastOutput = 0.0f;
	};

	float Process(float fCurrentError)
	{
		unsigned int nCurrentTimestamp_Millis = millis();
		unsigned long nTimeStepInMilliseconds = (m_nLastProcessedTime>0) ? (nCurrentTimestamp_Millis - m_nLastProcessedTime) : 1;

		// consider our sample frequency
		if ((nTimeStepInMilliseconds > m_nMilliSecondsBetweenProcessSteps) || 
			(m_nLastProcessedTime == 0))
		{
			m_fSummedError += fCurrentError * nTimeStepInMilliseconds;

			float fP = m_fP_Factor * fCurrentError;
			float fD = m_fD_Factor * ((fCurrentError - m_fLastError) / nTimeStepInMilliseconds);
			float fI = m_fI_Factor * m_fSummedError;

			// calculate output
			float fOutput = fP + fD + fI;

			// clamp output
			if (fOutput < -1.0f)
				fOutput = -1.0f;
			if (fOutput > 1.0f)
				fOutput = 1.0f;

			// update member values
			m_nLastProcessedTime	= nCurrentTimestamp_Millis;
			m_fLastError			= fCurrentError;
			m_fLastOutput			= fOutput;

			return fOutput;
		}
		else
		{
			// do not process values, simply return previous output value
			return m_fLastOutput;
		}

	};

	float Process(float fSoll, float fIst)
	{
		return Process(fSoll - fIst);
	};

private:
	float m_fLastError;		
	float m_fSummedError;
	float m_fP_Factor, m_fD_Factor, m_fI_Factor;
	float m_fLastOutput;

	unsigned long m_nMilliSecondsBetweenProcessSteps;
	unsigned long m_nLastProcessedTime;
};
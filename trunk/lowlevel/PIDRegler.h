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

	float Process(float fCurrentError, bool bUseIntegral)
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
			float fI = (bUseIntegral) ? m_fI_Factor * m_fSummedError : 0;
			if (bUseIntegral == false) m_fSummedError = 0;

                        // clamp p, d and i
                        if (fP < -1.0f) fP = -1.0f;
                        if (fP > 1.0f) fP = 1.0f;
                        if (fI < -1.0f) fI = -1.0f;
                        if (fI > 1.0f) fI = 1.0f;
                        if (fD < -1.0f) fD = -1.0f;
                        if (fD > 1.0f) fD = 1.0f;
                        
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

	float Process(float fSoll, float fIst, bool bUseIntegral)
	{
		return Process(fSoll - fIst, bUseIntegral);
	};

private:
	float m_fLastError;		
	float m_fSummedError;
	float m_fP_Factor, m_fD_Factor, m_fI_Factor;
	float m_fLastOutput;

	unsigned long m_nMilliSecondsBetweenProcessSteps;
	unsigned long m_nLastProcessedTime;
};

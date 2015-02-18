#include "config.h"
#include "Adafruit_NeoPixel.h"

#define NUM_PIXELS 8

class Pixels
{
public:
	enum TBlinkingMode
	{
		BLINKING_MODE_IDLE,
		BLINKING_MODE_DISARMING,
		BLINKING_MODE_DISARMED,
		BLINKING_MODE_INITIALIZED_ERROR,
		BLINKING_MODE_INITIALIZED_SUCCESS,
		BLINKING_MODE_INITIALIZING,

		NUM_BLINKING_MODES
	};

	Pixels() 
	{
		m_pStripFrontLeft = new Adafruit_NeoPixel(NUM_PIXELS, OUTPUT_PIN_NEOPIXELS_FRONT_LEFT);
		m_pStripFrontRight = new Adafruit_NeoPixel(NUM_PIXELS, OUTPUT_PIN_NEOPIXELS_FRONT_RIGHT);
		m_pStripRearLeft = new Adafruit_NeoPixel(NUM_PIXELS, OUTPUT_PIN_NEOPIXELS_REAR_LEFT);
		m_pStripRearRight = new Adafruit_NeoPixel(NUM_PIXELS, OUTPUT_PIN_NEOPIXELS_REAR_RIGHT);

		m_pStripFrontLeft->begin();
		m_pStripFrontRight->begin();
		m_pStripRearLeft->begin();
		m_pStripRearRight->begin();

		for (unsigned int n = 0; n < NUM_PIXELS; ++n)
		{
			m_pStripFrontLeft->setPixelColor(n, 255, 0, 0);
			m_pStripFrontRight->setPixelColor(n, 0, 255, 0);
			m_pStripRearLeft->setPixelColor(n, 0, 0, 255);
			m_pStripRearRight->setPixelColor(n, 255, 255, 0);
		}

		m_pStripFrontLeft->show();
		m_pStripFrontRight->show();
		m_pStripRearLeft->show();
		m_pStripRearRight->show();
	}

	~Pixels()
	{
		delete m_pStripFrontLeft;
		delete m_pStripFrontRight;
		delete m_pStripRearLeft;
		delete m_pStripRearRight;
	}

	void SetMode(TBlinkingMode eMode) { m_eBlinkingMode = eMode;  };
	void Update();

private:
	TBlinkingMode m_eBlinkingMode;
	unsigned long m_nLastUpdate = 0;
	Adafruit_NeoPixel *m_pStripFrontLeft;
	Adafruit_NeoPixel *m_pStripFrontRight;
	Adafruit_NeoPixel *m_pStripRearLeft;
	Adafruit_NeoPixel *m_pStripRearRight;
};

void Pixels::Update()
{
	bool bUpdated = false;

	if (m_nLastUpdate == 0) m_nLastUpdate = millis();
	else
	{
		if (m_eBlinkingMode == BLINKING_MODE_IDLE)
		{
			if (millis() - m_nLastUpdate > 100)
			{
				static int iFlag = 0;
				static bool bFlag	= true;
				static bool bFlag2	= true;

				if (iFlag % 2 == 0 && iFlag % 6 != 0)
				{
					for (unsigned int n = 0; n < 3; ++n)
					{
						// Red on the left side
						m_pStripFrontLeft->setPixelColor(n, 255, 0, 0);
						m_pStripRearLeft->setPixelColor(n, 255, 0, 0);

						// Green on the right side
						m_pStripFrontRight->setPixelColor(n, 0, 255, 0);
						m_pStripRearRight->setPixelColor(n, 0, 255, 0);
					}
				}
				else
				{
					for (unsigned int n = 0; n < 3; ++n)
					{
						m_pStripFrontLeft->setPixelColor(n, 0, 0, 0);
						m_pStripRearLeft->setPixelColor(n, 0, 0, 0);
						m_pStripFrontRight->setPixelColor(n, 0, 0, 0);
						m_pStripRearRight->setPixelColor(n, 0, 0, 0);
					}
				}
			
				for (unsigned int n = 3; n < NUM_PIXELS; ++n)
				{
					m_pStripFrontLeft->setPixelColor(n, 0, 0, 0);
					m_pStripRearLeft->setPixelColor(n, 0, 0, 0);
					m_pStripFrontRight->setPixelColor(n, 0, 0, 0);
					m_pStripRearRight->setPixelColor(n, 0, 0, 0);
				}

				if (iFlag % 6 == 0)
				{
					m_pStripFrontLeft->setPixelColor(6, 255, 255, 255);
					m_pStripFrontRight->setPixelColor(6, 255, 255, 255);
					m_pStripFrontLeft->setPixelColor(7, 255, 255, 255);
					m_pStripFrontRight->setPixelColor(7, 255, 255, 255);
				}

				m_nLastUpdate = millis();
				iFlag++;
				bUpdated = true;
			}
		}
		else if (m_eBlinkingMode == BLINKING_MODE_DISARMED)
		{
			if (millis() - m_nLastUpdate > 700)
			{
				static bool bFlag = true;

				for (unsigned int n = 0; n < NUM_PIXELS; ++n)
				{
					if ((n < 4 && bFlag) || (n >= 4 && !bFlag))
					{
						m_pStripFrontLeft->setPixelColor(n, 255, 102, 0);
						m_pStripRearLeft->setPixelColor(n, 255, 102, 0);
						m_pStripFrontRight->setPixelColor(n, 255, 102, 0);
						m_pStripRearRight->setPixelColor(n, 255, 102, 0);
					}
					else
					{
						m_pStripFrontLeft->setPixelColor(n, 0, 0, 0);
						m_pStripRearLeft->setPixelColor(n, 0, 0, 0);
						m_pStripFrontRight->setPixelColor(n, 0, 0, 0);
						m_pStripRearRight->setPixelColor(n, 0, 0, 0);
					}

				}

				bUpdated = true;

				m_nLastUpdate = millis();
				bFlag = !bFlag;
			}
		}
		else if (m_eBlinkingMode == BLINKING_MODE_INITIALIZED_SUCCESS)
		{
			if (millis() - m_nLastUpdate > 700)
			{
				static bool bFlag = true;

				for (unsigned int n = 0; n < NUM_PIXELS; ++n)
				{
					if (bFlag)
					{
						m_pStripFrontLeft->setPixelColor(n, 0, 255, 0);
						m_pStripRearLeft->setPixelColor(n, 0, 255, 0);
						m_pStripFrontRight->setPixelColor(n, 0, 255, 0);
						m_pStripRearRight->setPixelColor(n, 0, 255, 0);
					}
					else
					{
						m_pStripFrontLeft->setPixelColor(n, 0, 0, 0);
						m_pStripRearLeft->setPixelColor(n, 0, 0, 0);
						m_pStripFrontRight->setPixelColor(n, 0, 0, 0);
						m_pStripRearRight->setPixelColor(n, 0, 0, 0);
					}

				}

				bUpdated = true;

				m_nLastUpdate = millis();
				bFlag = !bFlag;
			}
		}
		else if (m_eBlinkingMode == BLINKING_MODE_INITIALIZED_ERROR)
		{
			if (millis() - m_nLastUpdate > 700)
			{
				static bool bFlag = true;

				for (unsigned int n = 0; n < NUM_PIXELS; ++n)
				{
					if (bFlag)
					{
						m_pStripFrontLeft->setPixelColor(n, 255, 0, 0);
						m_pStripRearLeft->setPixelColor(n, 255, 0, 0);
						m_pStripFrontRight->setPixelColor(n, 255, 0, 0);
						m_pStripRearRight->setPixelColor(n, 255, 0, 0);
					}
					else
					{
						m_pStripFrontLeft->setPixelColor(n, 0, 0, 0);
						m_pStripRearLeft->setPixelColor(n, 0, 0, 0);
						m_pStripFrontRight->setPixelColor(n, 0, 0, 0);
						m_pStripRearRight->setPixelColor(n, 0, 0, 0);
					}

				}

				bUpdated = true;

				m_nLastUpdate = millis();
				bFlag = !bFlag;
			}
		}
		else if (m_eBlinkingMode == BLINKING_MODE_INITIALIZING)
		{
			for (unsigned int n = 0; n < NUM_PIXELS; ++n)
			{
				m_pStripFrontLeft->setPixelColor(n, 50, 50, 50);
				m_pStripRearLeft->setPixelColor(n, 50, 50, 50);
				m_pStripFrontRight->setPixelColor(n, 50, 50, 50);
				m_pStripRearRight->setPixelColor(n, 50, 50, 50);
				
				m_pStripFrontLeft->show();
				m_pStripFrontRight->show();
				m_pStripRearLeft->show();
				m_pStripRearRight->show();

				delay(200);
			}
		}
	}

	if (bUpdated)
	{
		m_pStripFrontLeft->show();
		m_pStripFrontRight->show();
		m_pStripRearLeft->show();
		m_pStripRearRight->show();
	}
}

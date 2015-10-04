#include "config.h"
#include "Adafruit_NeoPixel.h"

#define NUM_PIXELS_BOOM 8
#define NUM_PIXELS_FRONT_CENTER 5
#define NUM_PIXELS_TAIL 6

class Pixels
{
public:
	enum TBlinkingMode
	{
		BLINKING_MODE_BOOM_CALIBRATION = -1, //< not used for productive use, only used to determinate correct pins for booms
		BLINKING_MODE_DISARMED = 0,			
		BLINKING_MODE_ARMED = 1,
		BLINKING_MODE_UNUSED1 = 2,
		BLINKING_MODE_UNUSED2 = 3,

		NUM_BLINKING_MODES
	};

	Pixels() 
	{
		m_nLastUpdate = 0;

		m_pStripFrontLeft = new Adafruit_NeoPixel(NUM_PIXELS_BOOM, OUTPUT_PIN_NEOPIXELS_FRONT_LEFT);
		m_pStripFrontRight = new Adafruit_NeoPixel(NUM_PIXELS_BOOM, OUTPUT_PIN_NEOPIXELS_FRONT_RIGHT);
		m_pStripRearLeft = new Adafruit_NeoPixel(NUM_PIXELS_BOOM, OUTPUT_PIN_NEOPIXELS_REAR_LEFT);
		m_pStripRearRight = new Adafruit_NeoPixel(NUM_PIXELS_BOOM, OUTPUT_PIN_NEOPIXELS_REAR_RIGHT);

		m_pStripFrontCenter = new Adafruit_NeoPixel(NUM_PIXELS_FRONT_CENTER, OUTPUT_PIN_NEOPIXELS_FRONT_CENTER);
		m_pStripTail = new Adafruit_NeoPixel(NUM_PIXELS_TAIL, OUTPUT_PIN_NEOPIXELS_TAIL);

		m_pStripFrontLeft->begin();
		m_pStripFrontRight->begin();
		m_pStripRearLeft->begin();
		m_pStripRearRight->begin();
		m_pStripFrontCenter->begin();
		m_pStripTail->begin();

		for (unsigned int n = 0; n < NUM_PIXELS_BOOM; ++n)
		{
			m_pStripFrontLeft->setPixelColor(n, 255, 0, 0);
			m_pStripFrontRight->setPixelColor(n, 0, 255, 0);
			m_pStripRearLeft->setPixelColor(n, 0, 0, 255);
			m_pStripRearRight->setPixelColor(n, 255, 255, 0);

			m_pStripFrontCenter->setPixelColor(n, 255, 255, 255);
			m_pStripTail->setPixelColor(n, 255, 255, 255);
		}

		m_pStripFrontLeft->show();
		m_pStripFrontRight->show();
		m_pStripRearLeft->show();
		m_pStripRearRight->show();
		m_pStripFrontCenter->show();
		m_pStripTail->show();
	}

	~Pixels()
	{
		delete m_pStripFrontLeft;
		delete m_pStripFrontRight;
		delete m_pStripRearLeft;
		delete m_pStripRearRight;
		delete m_pStripFrontCenter;
		delete m_pStripTail;
	}

	void SetMode(TBlinkingMode eMode) { m_eBlinkingMode = eMode;  };
	void Update();

private:
	TBlinkingMode m_eBlinkingMode;
	unsigned long m_nLastUpdate;
	Adafruit_NeoPixel *m_pStripFrontLeft;
	Adafruit_NeoPixel *m_pStripFrontRight;
	Adafruit_NeoPixel *m_pStripRearLeft;
	Adafruit_NeoPixel *m_pStripRearRight;
	Adafruit_NeoPixel *m_pStripFrontCenter;
	Adafruit_NeoPixel *m_pStripTail;
};

void Pixels::Update()
{
	bool bUpdated = false;

	if (m_nLastUpdate == 0) m_nLastUpdate = millis();
	else
	{
		if (m_eBlinkingMode == BLINKING_MODE_ARMED)
		{
			if (millis() - m_nLastUpdate > 100)
			{
				static int iFlag = 0;
				static bool bFlag	= true;
				static bool bFlag2	= true;

				if (iFlag % 2 == 0 && iFlag % 6 != 0)
				{
					for (unsigned int n = 0; n < 5; ++n)
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
					for (unsigned int n = 0; n < 5; ++n)
					{
						m_pStripFrontLeft->setPixelColor(n, 0, 0, 0);
						m_pStripRearLeft->setPixelColor(n, 0, 0, 0);
						m_pStripFrontRight->setPixelColor(n, 0, 0, 0);
						m_pStripRearRight->setPixelColor(n, 0, 0, 0);
					}
				}
			
				for (unsigned int n = 5; n < NUM_PIXELS_BOOM; ++n)
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

				for (unsigned int n = 0; n < m_pStripFrontCenter->numPixels(); ++n)
				{
					if (iFlag%6==n)
						m_pStripFrontCenter->setPixelColor(n, 255, 255, 255);
					else
						m_pStripFrontCenter->setPixelColor(n, 0, 0, 0);
				}

				for (unsigned int n = 0; n < NUM_PIXELS_TAIL; ++n)
				{
					if (n<3)
						m_pStripTail->setPixelColor(n, 0, 255, 0);
					else
						m_pStripTail->setPixelColor(n, 255, 0, 0);
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

				for (unsigned int n = 0; n < NUM_PIXELS_BOOM; ++n)
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

				for (unsigned int n = 0; n < m_pStripFrontCenter->numPixels(); ++n)
				{
					if (bFlag)
						m_pStripFrontCenter->setPixelColor(n, 255, 102, 0);
					else
						m_pStripFrontCenter->setPixelColor(n, 0, 0, 0);
				}

				for (unsigned int n = 0; n < NUM_PIXELS_TAIL; ++n)
				{
					if (bFlag)
						m_pStripTail->setPixelColor(n, 255, 102, 0);
					else
						m_pStripTail->setPixelColor(n, 0, 0, 0);
				}

				bUpdated = true;

				m_nLastUpdate = millis();
				bFlag = !bFlag;
			}
		}
		else if (m_eBlinkingMode == BLINKING_MODE_BOOM_CALIBRATION)
		{
			for (unsigned int n = 0; n < NUM_PIXELS_BOOM; ++n)
			{
				m_pStripFrontLeft->setPixelColor(n, 255, 0, 0);
				m_pStripFrontRight->setPixelColor(n, 0, 0, 255);
				m_pStripRearLeft->setPixelColor(n, 0, 255, 0);
				m_pStripRearRight->setPixelColor(n, 255, 255, 255);
			}

			bUpdated = true;
		}
		
	}

	if (bUpdated)
	{
		m_pStripFrontLeft->show();
		m_pStripFrontRight->show();
		m_pStripRearLeft->show();
		m_pStripRearRight->show();
		m_pStripFrontCenter->show();
		m_pStripTail->show();
	}
}

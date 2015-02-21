#include "config.h"
#include "Pixels.h"


Pixels *pNeoPixelsController = NULL;

void setup()
{
	pinMode(INPUT_PIN1_PIXELS_MODE, INPUT);
	pinMode(INPUT_PIN2_PIXELS_MODE, INPUT);
	pNeoPixelsController = new Pixels();

	pinMode(2, OUTPUT);
	digitalWrite(2, HIGH);
}

void loop()
{
	//static bool bLed = false;
	bool bModeBit1 = digitalRead(INPUT_PIN1_PIXELS_MODE) == HIGH;
	bool bModeBit2 = digitalRead(INPUT_PIN2_PIXELS_MODE) == HIGH;

	Pixels::TBlinkingMode eMode = (Pixels::TBlinkingMode) ((bModeBit1 ? 2 : 0) + (bModeBit2 ? 1 : 0));
	pNeoPixelsController->SetMode(eMode);
	pNeoPixelsController->Update();
}
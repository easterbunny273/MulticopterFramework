#include "GyroReader_MPU6050.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "debug.h"

#include <Arduino.h>
#include <Wire.h>

#define MPU6050_I2C_ADDRESS MPU6050_DEFAULT_ADDRESS //< is 0x68 by default, can also be 0x69 depending on board

// single instance of this class
GyroReader_MPU6050 * GyroReader_MPU6050::s_pInstance = NULL;
bool GyroReader_MPU6050::s_bInterruptHigh = false;

GyroReader_MPU6050::GyroReader_MPU6050() : 
	m_pDevice(new MPU6050(MPU6050_I2C_ADDRESS)),
	m_bDmpReady(false),
	m_nInterruptStatus(0),
	m_nPacketSize(0),
	m_nFifoCount(0),
	m_pCurrentQuaternion(new Quaternion)
{
	if (s_pInstance == NULL)
		s_pInstance = this;
	else
	{
		// TODO: proper error detection,
		// multiple instances of GyroReader_MPU6050 are
		// not allowed (yet) due to static methods for 
		// interrupt handling!

		debug_println("Error: Multiple instances of GyroReader_MPU6050 detected!");
	}
}

GyroReader_MPU6050::~GyroReader_MPU6050()
{
	delete m_pDevice;
	m_pDevice = NULL;

	delete m_pCurrentQuaternion;
	m_pCurrentQuaternion = NULL;

	s_pInstance = NULL;
}

bool GyroReader_MPU6050::ItlInitializeGyro()
{
	//Wire.begin();

	// Initialize gyro
	debug_println(F("Initializing MPU6050 gyro ..."));
	m_pDevice->initialize();

	// verify connection
    debug_println(F("Testing device connections..."));
	bool bTestResult = m_pDevice->testConnection();
	debug_println(bTestResult ? F("MPU6050 connection successful") : F("MPU6050 connection failed"));

	return bTestResult;
}

bool GyroReader_MPU6050::ItlInitializeDMP()
{
	// initialize DMP
	uint8_t devStatus = m_pDevice->dmpInitialize();
	
    
    // make sure it worked (returns 0 if so)
    if (devStatus == 0) 
	{
        // turn on the DMP, now that it's ready
        debug_println(F("Enabling DMP..."));
        m_pDevice->setDMPEnabled(true);

        // enable Arduino interrupt detection

#if LOWLEVELCONFIG_GYRO_USE_INTERRUPT
#ifdef _VARIANT_ARDUINO_DUE_X_
		// Use the same pin as on Arduino UNO
		// Note that on Arduino Due, we define the PIN (see http://arduino.cc/en/Reference/attachInterrupt)

		debug_println(F("Enabling interrupt detection (Arduino pin 2)..."));
		attachInterrupt(2, StaticOnInterrupt, RISING);
#else
		// Use interrupt0 (which is connected to pin 2)
		debug_println(F("Enabling interrupt detection (Arduino external interrupt 0)..."));
		attachInterrupt(0, StaticOnInterrupt, RISING);	
#endif
#endif
        m_nInterruptStatus = m_pDevice->getIntStatus();

        // set our DMP Ready flag so the main loop() function knows it's okay to use it
        debug_println(F("DMP ready!"));
		m_bDmpReady = true;

        // get expected DMP packet size for later comparison
        m_nPacketSize = m_pDevice->dmpGetFIFOPacketSize();
    } else {
        // ERROR!
        // 1 = initial memory load failed
        // 2 = DMP configuration updates failed
        // (if it's going to break, usually the code will be 1)
        debug_print(F("DMP Initialization failed (code "));
        debug_print(devStatus);
        debug_println(F(")"));
    }

	return m_bDmpReady;
}

bool GyroReader_MPU6050::begin()
{
	bool bOk = ItlInitializeGyro();

	if (bOk)
	{
		bOk = ItlInitializeDMP();
	}

	return bOk;
}

bool GyroReader_MPU6050::processData()
{
	bool bProcessedData = false;

#if LOWLEVELCONFIG_GYRO_USE_INTERRUPT
	if (m_bDmpReady && s_bInterruptHigh)
#else
	if (m_bDmpReady)
#endif
	{
		// reset interrupt flag and get INT_STATUS byte
		s_bInterruptHigh = false;
		uint8_t nInterruptStatus = m_pDevice->getIntStatus();

		// get current FIFO count
		m_nFifoCount = m_pDevice->getFIFOCount();

		// check for overflow (this should never happen unless our code is too inefficient)
		if ((nInterruptStatus & 0x10) || m_nFifoCount >= 1024) 
		{
			// reset so we can continue cleanly
			m_pDevice->resetFIFO();
			debug_println(F("FIFO overflow!"));

		// otherwise, check for DMP data ready interrupt (this should happen frequently)
		} 
		else if (nInterruptStatus & 0x02) 
		{
			// wait for correct available data length, should be a VERY short wait
			while (m_nFifoCount < m_nPacketSize) m_nFifoCount = m_pDevice->getFIFOCount();

			while (m_nFifoCount > m_nPacketSize)
			{
				// read a packet from FIFO
				m_pDevice->getFIFOBytes(m_pFifoBuffer, m_nPacketSize);
        
				// track FIFO count here in case there is > 1 packet available
				// (this lets us immediately read more without waiting for an interrupt)
				m_nFifoCount -= m_nPacketSize;
			}
			
			// read quaternion and gravity
			m_pDevice->dmpGetQuaternion(m_pCurrentQuaternion, m_pFifoBuffer);			

			bProcessedData = true;
		}
	}

	return bProcessedData;
}

void GyroReader_MPU6050::getQuaternion(Quaternion &rQuaternion) const
{
	rQuaternion = *m_pCurrentQuaternion;
}

void GyroReader_MPU6050::getYawPitchRoll(float &rYaw, float &rPitch, float &rRoll) const
{
	VectorFloat vGravity;
	m_pDevice->dmpGetGravity(&vGravity, m_pCurrentQuaternion);

	float pData[3];
	m_pDevice->dmpGetYawPitchRoll(pData, m_pCurrentQuaternion, &vGravity);

	rYaw = pData[0];
	rPitch = pData[1];
	rRoll = pData[2];
}
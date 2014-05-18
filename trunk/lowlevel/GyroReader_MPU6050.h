#ifndef _GYROREADER_MPU6050_H_
#define _GYROREADER_MPU6050_H_

#include "config.h"
#include "IGyroReader.h"

// Forward declarations
class MPU6050;
class Quaternion;

class GyroReader_MPU6050 : public IGyroReader
{
public:
	GyroReader_MPU6050();
	virtual ~GyroReader_MPU6050();

	virtual bool begin();

	virtual bool processData();

	virtual void getQuaternion(Quaternion &rQuaternion) const;

private:
	bool ItlInitializeGyro();
	bool ItlInitializeDMP();

	static void StaticOnInterrupt() { s_bInterruptHigh = true; };
	static bool s_bInterruptHigh;
	static GyroReader_MPU6050 * s_pInstance;

	bool		m_bDmpReady;
	uint8_t		m_nInterruptStatus;
	uint16_t	m_nPacketSize;    // expected DMP packet size (default is 42 bytes)
	uint16_t	m_nFifoCount;     // count of all bytes currently in FIFO
	uint8_t		m_pFifoBuffer[64]; // FIFO storage buffer
	Quaternion *  m_pCurrentQuaternion;

	MPU6050 * m_pDevice;
};

#endif
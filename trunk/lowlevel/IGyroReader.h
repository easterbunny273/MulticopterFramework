// chmoellinger, 16.05.14

#ifndef _I_GYROREADER_H_
#define _I_GYROREADER_H_

class IGyroReader
{
public:
	virtual ~IGyroReader() {};

	/// Setup, initializes the reader. Returns true on success.
	virtual bool begin() = 0;

	virtual bool processData() = 0;

	virtual void getQuaternion(float &rfW, float &rfX, float &rfY, float &rfZ) = 0;
};

#endif
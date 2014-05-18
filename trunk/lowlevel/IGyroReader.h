// chmoellinger, 16.05.14

#ifndef _I_GYROREADER_H_
#define _I_GYROREADER_H_

// Forward declarations
class Quaternion;

class IGyroReader
{
public:
	virtual ~IGyroReader() {};

	/// Setup, initializes the reader. Returns true on success.
	virtual bool begin() = 0;

	virtual bool processData() = 0;

	virtual void getQuaternion(Quaternion &rQuaternion) const = 0;
};

#endif
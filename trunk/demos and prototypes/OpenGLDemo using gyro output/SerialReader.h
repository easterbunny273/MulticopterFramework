#ifndef SERIALREADER_H
#define SERIALREADER_H

#include <cstdio>
#include <string>
#include <QObject>


class SerialReader : public QObject
{
	Q_OBJECT
public:
	SerialReader();

	bool open(const std::string &sFilename);
	void close();

	void Read();

signals:
	void rotationReceived(float fW, float fX, float fY, float fZ, int fAx, int fAy, int fAz);

private:
	int m_pFileHandle;
};

#endif // CONSOLEREADER_H

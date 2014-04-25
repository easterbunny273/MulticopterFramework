#include "SerialReader.h"

#include <iostream>
#include <cassert>

#define nullptr NULL

#include <errno.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <fcntl.h>
#include <queue>
#include <deque>
#include <cmath>
#include <iomanip>  // ::std::setw

//#include <QQmlContext>

SerialReader::SerialReader()
{
	m_pFileHandle = nullptr;
}

bool SerialReader::open(const std::string &sFilename)
{
	// open interface
	m_pFileHandle = ::open (sFilename.c_str(), O_RDWR | O_NOCTTY);

	// wait for the arduino to reboot (also consider needed calibration time)
	usleep(500000);

	// cm: set some parameters for the serial device, code copied
	// from the internet

	// set new parameters to the serial device
	   struct termios newtio;

	   fcntl(m_pFileHandle, F_SETFL, 0);
	   // set everything to 0
	   bzero(&newtio, sizeof(newtio));

	   // again set everything to 0
	   bzero(&newtio, sizeof(newtio));

	   newtio.c_cflag |= B115200; // Set Baudrate first time
	   newtio.c_cflag |= CLOCAL; // Local line - do not change "owner" of port
	   newtio.c_cflag |= CREAD; // Enable receiver

	   newtio.c_cflag &= ~ECHO; // Disable echoing of input characters
	   newtio.c_cflag &= ~ECHOE;

	   // set to 8N1
	   newtio.c_cflag &= ~PARENB; // no parentybyte
	   newtio.c_cflag &= ~CSTOPB; // 1 stop bit
	   newtio.c_cflag &= ~CSIZE; // Mask the character size bits
	   newtio.c_cflag |= CS8; // 8 data bits

	   // output mode to
	   newtio.c_oflag = 0;
	   //newtio.c_oflag |= OPOST;


	   // Set teh baudrate for sure
	   cfsetispeed(&newtio, B115200);
	   cfsetospeed(&newtio, B115200);

	   newtio.c_cc[VTIME] = 10; /* inter-character timer  */
	   newtio.c_cc[VMIN] = 5; /* blocking read until  */

	   tcflush(m_pFileHandle, TCIFLUSH); // flush pending data

	   // set the new defined settings
	   if (tcsetattr(m_pFileHandle, TCSANOW, &newtio)) {
		   std::cerr << "could not set the serial settings!" << std::endl;
		   return -99;
	   }

	return (m_pFileHandle >= 0);
}

void SerialReader::close()
{
	if (m_pFileHandle != nullptr)
	{
		//close(m_pFileHandle);
		m_pFileHandle = nullptr;
	}
}

void SerialReader::Read()
{
	assert (m_pFileHandle != nullptr);

	char * szTempBuffer = new char [1];
	char cInput = 'a';

	// always remember the last three bytes to identify magic start+end bytes
	unsigned char pLastThreeReadBytes[3];

	// vector for current line of received data
	std::vector<unsigned char> vCurrentLine;

	bool bReadingLine = false;
	int iCount =0;

	while(cInput != 'q')
	{
		// cm: this loop reads byte for byte and always remembers the last three
		// read bytes. Once the start signature (13 and 37) is received, it appends received
		// bytes to a vector (vCurrentLine) and processes it, as soon as the stop signature (42 and 13) is received.
		// Note: This is a quick hack. It is test code and has many quality issues (error handling, memory leak, ...).

		// read byte
		int iResult = read(m_pFileHandle, szTempBuffer, sizeof(unsigned char));

		if (iResult > 0)
		{
			assert (iResult == 1);

			pLastThreeReadBytes[2] = pLastThreeReadBytes[1];
			pLastThreeReadBytes[1] = pLastThreeReadBytes[0];
			pLastThreeReadBytes[0] = szTempBuffer[0];

			if (bReadingLine)
			{
				// add read byte to current line
				vCurrentLine.push_back(szTempBuffer[0]);

				// if we got the magic end bytes, process current line
				if (pLastThreeReadBytes[2] == 13 && pLastThreeReadBytes[1] == 37)
				{
					bReadingLine = false;

					float * pQuaternion = (float*) &vCurrentLine[0];
					int16_t * pAcceleration = (int16_t *) &vCurrentLine[16];
					float * pGravity = (float*) &vCurrentLine[22];

					//std::cout << pAcceleration[0] << ":" << pAcceleration[1] << ":" << pAcceleration[2] << "##" << pGravity[0] << ":" << pGravity[1] << ":" << pGravity[2] << std::endl;

					// cm: we send the new data to the render thread by using Qt's signal+slot mechanism, which implicitely handles
					// thread synchronization (rendering is done in an other thread!). Unfortunately,
					// it seems that too many signals per second create a signal jam and stuff doesn't work.

					// As a workaround, discarding the half of the read data does the trick. And at least for the quaternion, it is allowed
					// (it describes the full rotation, not only a relative change of rotation).

					if (iCount++ == 2)
					{
						emit rotationReceived(pQuaternion[0], pQuaternion[1], pQuaternion[2], pQuaternion[3], pAcceleration[0], pAcceleration[1], pAcceleration[2]);
						iCount = 0;
					}


					vCurrentLine.clear();
				}
			}
			else
			{
				// if we got the magic start bytes, start reading
				if (pLastThreeReadBytes[2] == 42 && pLastThreeReadBytes[1] == 13)
				{
					bReadingLine = true;
					vCurrentLine.push_back(szTempBuffer[0]);
				}
			}
		}
	}
}

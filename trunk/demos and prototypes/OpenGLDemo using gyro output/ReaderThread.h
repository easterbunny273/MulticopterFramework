#ifndef READERTHREADDERTHREAD_H
#define READERTHREADDERTHREAD_H

#include <QThread>
#include "SerialReader.h"
#include "mainwidget.h"

#include <iostream>

class ReaderThread : public QThread
{
	Q_OBJECT

private:
MainWidget * m_pMainWidget;

public slots:
	void SetMainWidget(MainWidget * pWidget) { m_pMainWidget = pWidget; }

	void run()  {
		  QString result;


		  SerialReader Reader;
		  bool bOpened = Reader.open("/dev/ttyUSB1h");


		  if (bOpened)
		  {
			  std::cout << "Opened interface!" << std::endl;

			  // connect signal of receiving data from the reader to the slot of the renderer
			  QObject::connect(&Reader, SIGNAL(rotationReceived(float,float,float,float, int, int, int)), m_pMainWidget, SLOT(setRotation(float,float,float,float, int, int, int)));

			  Reader.Read();
		  }
		  else
			  std::cout << "Could not open interface!" << std::endl;


		  emit resultReady(result);
	  }

signals:
	void resultReady(const QString &result);
};

#endif // READERTHREADDERTHREAD_H

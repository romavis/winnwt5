/***************************************************************************
                          modem.h  -  description
                             -------------------
    begin                : Wed Feb 14 2001
    copyright            : (C) 2001 by Lars Schnake
    email                : mail@lars-schnake.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef MODEM_H
#define MODEM_H

#include "modem.hpp"

// //#############################################################################
// //#############################################################################
// //#############################################################################
// //#############################################################################
// //#############################################################################

// // Quelltext fuer Windows
// //#############################################################################
// //#############################################################################
// //#############################################################################
// //#############################################################################
// //#############################################################################
// /*

// #include <qobject.h>
// #include <windows.h>
// #include <QString>

// //#include <qdir.h>

// //#include <sys/types.h>
// //#include <termios.h>
// //#include <unistd.h>

// //#include <qsocketnotifier.h>

// //#include <config.h>

// //modem class for serial communication
// //  *@author Lars Schnake
// //  *@author DL4JAL Andreas Lindenau Aenderungen fuer NWT7 + HFM9
// //

// enum wBaud{
//   wBaud_9600=9600,
//   wBaud_57600=57600,
//   wBaud_115200=115200
// };

// enum wParity{
//   wPARITY_NONE,
//   wPARITY_ODD,
//   wPARITY_EVEN,
//   wPARITY_MARK,
//   wPARITY_SPACE
// };

// enum wSerialLineState{
//   wSERIAL_LINESTATE_DCD = 0x040,
//   wSERIAL_LINESTATE_CTS = 0x020,
//   wSERIAL_LINESTATE_DSR = 0x100,
//   wSERIAL_LINESTATE_DTR = 0x002,
//   wSERIAL_LINESTATE_RING = 0x080,
//   wSERIAL_LINESTATE_RTS = 0x004,
//   wSERIAL_LINESTATE_NULL = 0x000,
// };

// struct SerialPort_DCS{
//   wBaud baud;
//   wParity parity;
//   unsigned char wordlen;
//   unsigned char stopbits;
//   bool rtscts;
//   bool xonxoff;
//   SerialPort_DCS(){
//     baud = wBaud_57600;
//     parity = wPARITY_NONE;
//     stopbits = 1;
//     rtscts = false;
//     xonxoff = false;
//   };
// };

// struct SerialPort_EINFO{
//   int brk;
//   int frame;
//   int overrun;
//   int parity;
//   SerialPort_EINFO(){
//     brk = frame = overrun = parity = 0;
//   };
// };

// class Modem : public QObject  {
// Q_OBJECT
// public:
//   Modem();
//   ~Modem();

// // lock and open serial port with settings of ConfigData
//   bool opentty(QString seriell_dev);
// // unlock and close serial port
//   bool closetty();
// // write a char to port
//   bool writeChar(unsigned char);
// // write a char[] to port
//   bool writeLine(const char *);
// // returns the last modem (error?)message 
// //  const QString modemMessage();
// // disconnect the notify connection ( see notify )
// //  void stop();
// // simple read from port 
// //  int rs232_read(void *, int);
// //  int getFD();
// // neue funktion fuer den NWT7
//   int readttybuffer(void *, int);
//   static Modem *modem;

//   HANDLE fd;
//   OVERLAPPED ov;
//   SerialPort_DCS dcs;
//   SerialPort_EINFO einfo;

//   int CloseDevice();
//   int OpenDevice(const char* devname, void* dcs);

// signals:
// // is emitted if data was received
// //  void charWaiting(unsigned char);
//   void setTtyText(QString); 

// public slots:

// 	// notify if chars arriving from ptc
// //  void startNotifier();
// 	// stop the notifier
// //  void stopNotifier();
// 	// read from port
// //  void readtty(int);

// private:
// //  speed_t modemspeed();  // parse modem speed
//   bool modem_is_locked;
//   int modemfd;
// //  QSocketNotifier *sn;
//   bool data_mode;
//   QString errmsg;
// //  QString seriell_dev;
// //  struct termios initial_tty;
// //  struct termios tty;
//   unsigned char dataMask;
// //  bool lock_device();
// //  bool unlock_device();
//   QString qdev;
// };
// */
// //#############################################################################
// //#############################################################################
// //#############################################################################
// //#############################################################################
// //#############################################################################
// // Quelltext fuer Linux
// //#############################################################################
// //#############################################################################
// //#############################################################################
// //#############################################################################
// //#############################################################################
// #include <qobject.h>

// #include <qdir.h>

// #include <sys/types.h>
// //#include <termios.h>
// #include <unistd.h>

// #include <qsocketnotifier.h>

// #include "constdef.h"

// //#include <config.h>

// //modem class for serial communication
// //  *@author Lars Schnake
// //  *@author DL4JAL Andreas Lindenau Aenderungen fuer NWT7 + HFM9 + NWT FA

// class Modem : public QObject  {

// Q_OBJECT
// public:
//   Modem();
//   ~Modem();

// // lock and open serial port with settings of ConfigData
//   bool opentty(QString seriell_dev);
// // unlock and close serial port
//   bool closetty();
// // write a char to port
//   bool writeChar(unsigned char);
// // write a char[] to port
//   bool writeLine(const char *);
// // returns the last modem (error?)message
//   const QString modemMessage();
// // disconnect the notify connection ( see notify )
//   void stop();
// // simple read from port
//   int rs232_read(void *, int);
//   int getFD();
// // neue funktion fuer den NWT7
//   int readttybuffer(void *, int);


// public:
//   static Modem *modem;

// signals:
// // is emitted if data was received
//   void charWaiting(unsigned char);
//   void setTtyText(QString);

// public slots:

// 	// notify if chars arriving from ptc
//   void startNotifier();
// 	// stop the notifier
//   void stopNotifier();
// 	// read from port
//   void readtty(int);

// private:
//   speed_t modemspeed();  // parse modem speed
//   bool modem_is_locked;
//   int modemfd;
//   QSocketNotifier *sn;
//   bool data_mode;
//   QString errmsg;
// //  QString seriell_dev;
//   struct termios initial_tty;
//   struct termios tty;
//   unsigned char dataMask;
//   bool lock_device();
//   bool unlock_device();
//   QString qdev;
// };

#endif


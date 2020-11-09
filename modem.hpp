/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef H_MODEM_HPP_
#define H_MODEM_HPP_

#include <qobject.h>
//#include <windows.h>
#include <QString>


enum wBaud{
  wBaud_9600=9600,
  wBaud_57600=57600,
  wBaud_115200=115200
};

enum wParity{
  wPARITY_NONE,
  wPARITY_ODD,
  wPARITY_EVEN,
  wPARITY_MARK,
  wPARITY_SPACE
};

enum wSerialLineState{
  wSERIAL_LINESTATE_DCD = 0x040,
  wSERIAL_LINESTATE_CTS = 0x020,
  wSERIAL_LINESTATE_DSR = 0x100,
  wSERIAL_LINESTATE_DTR = 0x002,
  wSERIAL_LINESTATE_RING = 0x080,
  wSERIAL_LINESTATE_RTS = 0x004,
  wSERIAL_LINESTATE_NULL = 0x000,
};

struct SerialPort_DCS{
  wBaud baud;
  wParity parity;
  unsigned char wordlen;
  unsigned char stopbits;
  bool rtscts;
  bool xonxoff;
  SerialPort_DCS(){
    baud = wBaud_57600;
    parity = wPARITY_NONE;
    stopbits = 1;
    rtscts = false;
    xonxoff = false;
  };
};

struct SerialPort_EINFO{
  int brk;
  int frame;
  int overrun;
  int parity;
  SerialPort_EINFO(){
    brk = frame = overrun = parity = 0;
  };
};

class Modem : public QObject {
  Q_OBJECT
 public:
  //  Modem();
  virtual ~Modem() {}

  /**
   * @brief Returns platform-specific modem singleton
   * Defined in platform-specific modem file.
   */
  static Modem *GetModem();

  /** lock and open serial port with settings of ConfigData*/
  virtual bool opentty(QString seriell_dev) = 0;
  /** unlock and close serial port */
  virtual bool closetty() = 0;
  /** write a char to port */
  virtual bool writeChar(unsigned char) = 0;
  /** write a char[] to port*/
  virtual bool writeLine(QString &ln) = 0;
  /** returns the last modem (error?)message */
  //  const QString modemMessage();
  /** disconnect the notify connection ( see notify )*/
  //  void stop();
  /** simple read from port */
  //  int rs232_read(void *, int);
  //  int getFD();
  // neue funktion fuer den NWT7
  virtual int readttybuffer(void *, int) = 0;
  //static Modem *modem;

  // HANDLE fd;
  // OVERLAPPED ov;
  //SerialPort_DCS dcs;
  //SerialPort_EINFO einfo;

  // virtual int CloseDevice() = 0;
  // virtual int OpenDevice(const char *devname, void *dcs) = 0;

 signals:
  /** is emitted if data was received */
  //  void charWaiting(unsigned char);
  void setTtyText(QString);

 public slots:

  /** notify if chars arriving from ptc */
  //  void startNotifier();
  /** stop the notifier */
  //  void stopNotifier();
  /** read from port */
  //  void readtty(int);

 private:
  //  speed_t modemspeed();  // parse modem speed
  // bool modem_is_locked;
  // int modemfd;
  // //  QSocketNotifier *sn;
  // bool data_mode;
  // QString errmsg;
  // //  QString seriell_dev;
  // //  struct termios initial_tty;
  // //  struct termios tty;
  // unsigned char dataMask;
  // //  bool lock_device();
  // //  bool unlock_device();
  // QString qdev;
};

#endif // H_MODEM_HPP_
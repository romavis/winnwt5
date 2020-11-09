//#############################################################################
// Quelltext fuer Windows
//#############################################################################
#include <QDebug>
#include <QSerialPort>
#include <memory>
#include <cstring>

#include "modem.hpp"

/**
 * @brief Modem implemented via QSerialPort for portability
 */
class ModemQSerialPort: public Modem {
 public:
  ModemQSerialPort() {
  }

  virtual bool opentty(QString seriell_dev) {
    qDebug() << "ModemQSerialPort::opentty: " << seriell_dev;
    port_.close();
    port_.setPortName(seriell_dev);
    bool res = true;
    if(!port_.open(QIODevice::ReadWrite)) {
      qWarning() << "QSerialPort::open(" << seriell_dev << ") failed";
      return false;
    }
    /*
      dcb.BaudRate = CBR_57600;
  dcb.fBinary = 1;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  dcb.ByteSize = 8; 
  dcb.fOutxCtsFlow = false;
  dcb.fDtrControl = DTR_CONTROL_ENABLE;
  dcb.fRtsControl = RTS_CONTROL_ENABLE;
  */
    if(!port_.setBaudRate(57600)) {
      qWarning("QSerialPort::setBaudRate() failed");
      return false;
    }
    if(!port_.setParity(QSerialPort::NoParity)) {
      qWarning("QSerialPort::setParity() failed");
      return false;
    }
    if(!port_.setStopBits(QSerialPort::OneStop)) {
      qWarning("QSerialPort::setStopBits() failed");
      return false;
    }
    if(!port_.setDataBits(QSerialPort::Data8)) {
      qWarning("QSerialPort::setDataBits() failed");
      return false;
    }
    if(!port_.setFlowControl(QSerialPort::NoFlowControl)) {
      qWarning("QSerialPort::setFlowControl() failed");
      return false;
    }
    if(!port_.setRequestToSend(true)) {
      qWarning("QSerialPort::setRequestToSend() failed");
      return false;
    }
    if(!port_.setDataTerminalReady(true)) {
      qWarning("QSerialPort::setDataTerminalReady() failed");
      return false;
    }
    return true;
  }
  /** unlock and close serial port */
  virtual bool closetty(){
    qDebug() << "ModemQSerialPort::closetty";
    port_.close();
    return true;
  }
  /** write a char to port */
  virtual bool writeChar(unsigned char uc){
    qDebug() << "ModemQSerialPort::writeChar: " << uc;
    char c = uc;
    port_.write(&c, 1);
    return true;
  }
  /** write a char[] to port*/
  virtual bool writeLine(QString &ln){
    qDebug() << "ModemQSerialPort::writeLine: " << ln;
    QByteArray ba = ln.toLatin1();
    port_.write(ba);
    return true;
  }
  // neue funktion fuer den NWT7
  virtual int readttybuffer(void * p, int n){
    qDebug() << "ModemQSerialPort::readttybuffer: " << p << " " << n << "...";

    QByteArray ba = port_.read(n);
    std::memcpy(p, ba.data(), ba.length());
    qDebug() << "... read: " << ba;
    return ba.length();
  }

 private:
  QSerialPort port_;
};

static ModemQSerialPort static_modem;

Modem* Modem::GetModem() {
  return &static_modem;
}

/*
#include "modem.h"
#include <string.h>
#include <stdio.h>
#include <iostream.h>
#include <QtGlobal>


#define SERIALPORT_BUFSIZE 4096

#ifndef max
	#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
	#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

Modem *Modem::modem = 0;

//Modem::Modem(): modemfd(-1),sn(0L), data_mode(false),dataMask(0xFF)
Modem::Modem(): modemfd(-1), data_mode(false),dataMask(0xFF)
{
  assert(modem==0);
  modem = this;
  modem_is_locked = false;
  qdev = "";
}

Modem::~Modem() {
  modem = 0;
}

//speed_t Modem::modemspeed() {
// convert the string modem speed to a t_speed type
  // to set the modem.  The constants here should all be ifdef'd because
  // other systems may not have them

//	int i = 576;
//  switch(i) {
//  case 24:
//    return B2400;
//    break;
//  case 96:
//    return B9600;
//    break;
//  case 192:
//    return B19200;
//    break;
//  case 384:
//    return B38400;
//    break;
//  case 576:
//    return B57600;
//    break;
//#ifdef B115200
//  case 1152:
//    return B115200;
//    break;
//#endif
//
//#ifdef B230400
//  case 2304:
//    return B230400;
//    break;
//#endif
//
//#ifdef B460800
//  case 4608:
//    return 4608;
//    break;
//#endif
//
//  default:
//    return B38400;
//    break;
//  }
//
////}

// open serial port with settings of ConfigData
bool Modem::opentty(QString seriell_dev) {
  DCB dcb;

  const char *name;
  QString qs;
  qs = seriell_dev;
  //COM ab COM10 muessen gewandelt werden in \\.\COM10
  if(qs.length() > 4)qs = "\\\\.\\" + qs;
  //Wandlung in *char fuer API-Funktionen
  name = qPrintable(qs);
  //alles auf 0 setzen
  memset(&dcb, 0, sizeof(dcb));
  dcb.DCBlength = sizeof(dcb);
  dcb.BaudRate = CBR_57600;
  dcb.fBinary = 1;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  dcb.ByteSize = 8; 
  dcb.fOutxCtsFlow = false;
  dcb.fDtrControl = DTR_CONTROL_ENABLE;
  dcb.fRtsControl = RTS_CONTROL_ENABLE;

  fd = CreateFileA(name, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
  if(fd == INVALID_HANDLE_VALUE){
    emit setTtyText("?");
    return false;
  }
  if(!SetCommState(fd,&dcb)){
    emit setTtyText("?");
    return false;
  }
  ov.hEvent = CreateEvent(NULL, true, true, NULL);
  if(ov.hEvent == INVALID_HANDLE_VALUE){
    emit setTtyText("?");
    return false;
  }
  COMMTIMEOUTS cto = {MAXDWORD, 0, 0, 0, 0};
  if(!SetCommTimeouts(fd, &cto)){
    emit setTtyText("?");
    return false;
  }
  if(!SetupComm(fd, SERIALPORT_BUFSIZE, SERIALPORT_BUFSIZE)){
    emit setTtyText("?");
    return false;
  }
  memset(&einfo,0, sizeof(einfo));
  emit setTtyText(seriell_dev);
  return true;
};

//  // get device from config file
//  qdev = seriell_dev;
//  // lock the device:
//  if ( lock_device() == false ) {
//    qDebug("Error by device locking");
//    return false;
//  }
//  //  int flags;
//  modemfd = open(qdev, O_RDWR | O_NOCTTY );
//  if(modemfd  <0) {
//    qDebug("Sorry, can't open modem.");
//    return false;
//  }
//  tcdrain (modemfd);
//  tcflush (modemfd, TCIOFLUSH);
//  if(tcgetattr(modemfd, &tty) < 0){
//    // this helps in some cases
//    tcsendbreak(modemfd, 0);
//    sleep(1);
//    if(tcgetattr(modemfd, &tty) < 0){
//      qDebug("Sorry, the modem is busy.");
//      ::close(modemfd);
//      modemfd = -1;
//      return false;
//    }
//  }
//  memset(&initial_tty,'\0',sizeof(initial_tty));
//  initial_tty = tty;
//  
//  tty.c_cc[VMIN] = 0; // nonblocking
//  tty.c_cc[VTIME] = 0;
//  tty.c_oflag = 0;
//  tty.c_lflag = 0;
//  tty.c_cflag &= ~(CSIZE | CSTOPB | PARENB);
//  tty.c_cflag |= CS8 | CREAD;
//  tty.c_cflag |= CLOCAL;                   // ignore modem status lines
//  tty.c_iflag = IGNBRK | IGNPAR // | ISTRIP ;
//  tty.c_lflag &= ~ICANON;                  // non-canonical mode
//  tty.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHOKE);
//
//  cfsetospeed(&tty, modemspeed());
//  cfsetispeed(&tty, modemspeed());
//  tcdrain(modemfd);
//  if(tcsetattr(modemfd, TCSANOW, &tty) < 0){
//    qDebug("Sorry, the modem is busy.");
//    ::close(modemfd);
//    modemfd=-1;
//    return false;
//  }
//  qDebug("Modem Ready.");
//
////  return true;
////}

bool Modem::closetty() {
  if(fd != INVALID_HANDLE_VALUE){
    CloseHandle(ov.hEvent);
    CloseHandle(fd);
    fd = INVALID_HANDLE_VALUE;
  }
  emit setTtyText("?");
  return true;
}

//    if(modemfd >=0 ) {
//    stop();
//    // discard data not read or transmitted
//    tcflush(modemfd, TCIOFLUSH);
//    if(tcsetattr(modemfd, TCSANOW, &initial_tty) < 0){
//      qDebug("Can't restore tty settings: tcsetattr()\n");
//      ::close(modemfd);
//      modemfd = -1;
//      return false;
//    }
//    ::close(modemfd);
//    modemfd = -1;
//  }
//  if ( unlock_device() == false )  {
//    qDebug("cannot unlock device !");
//    return false;
//  }
//
////  return true;
////}
//
//void Modem::readtty(int) {
//
//  char buffer[50];
//  unsigned char c;
//  int len;
//
//  // read data in chunks of up to 50 bytes
//  if((len = ::read(modemfd, buffer, 50)) > 0) {
//    // split buffer into single characters for further processing
//    for(int i = 0; i < len; i++) {
//      c = buffer[i] & dataMask;
//      emit charWaiting(c);
//    }
//  }
//}
//
//void Modem::stop() {
//  disconnect(SIGNAL(charWaiting(unsigned char)));
//  stopNotifier();
//}
//
//void Modem::startNotifier() {
//  if(modemfd >= 0) {
//    if(sn == 0) {
//      sn = new QSocketNotifier(modemfd, QSocketNotifier::Read, this);
//      connect(sn, SIGNAL(activated(int)), SLOT(readtty(int)));
//      qDebug("QSocketNotifier started!");
//    } else {
//      sn->setEnabled(true);
//    }
//  }
//}
//
//void Modem::stopNotifier() {
//  if(sn != 0) {
//    sn->setEnabled(false);
//    disconnect(sn);
//    delete sn;
//    sn = 0;
//    qDebug("QSocketNotifier stopped!");
//  }
//}
//
bool Modem::writeChar(unsigned char c) {
//  return write(modemfd, &c, 1) == 1;
  DWORD dw;

  WriteFile(fd, &c, 1, &dw, NULL);
  return (dw > 0);
}

bool Modem::writeLine(const char *buf) {
//  write(modemfd, buf, strlen(buf));
//Let's send an "enter"
//  write(modemfd, "\r", 1);
  DWORD dw;
  int len;

  len = strlen(buf);
  WriteFile(fd, buf, len, &dw, NULL);
  return (int(dw) == len);
}


//bool Modem :: lock_device()
//{
//  const char * device;
////  device = qdev.data();
//  char	lckf[128];
//  int	lfh;
//  pid_t	lckpid;
//  char	*devicename;
//  char	lckpidstr[20];
//  int	nb;
//  struct stat buf;
//  devicename = strrchr(device, '/');
//  sprintf(lckf, "%s/%s%s", LF_PATH, LF_PREFIX, (devicename ? (devicename + 1) : device));
//  //Check if there's already a LCK..* file
//  if (stat(lckf, &buf) == 0) {
//  //we must now expend effort to learn if it's stale or not.
//    if ((lfh = open(lckf, O_RDONLY)) != -1) {
//      nb = read(lfh, &lckpidstr, min(20, buf.st_size));
//      if (nb > 0) {
//        lckpidstr[nb] = 0;
//        sscanf(lckpidstr, "%d", &lckpid);
//        if (kill(lckpid, 0) == 0) {
//          qDebug("Device is locked by process ");
//          return false;
//        }
//      //The lock file is stale. Remove it.
//        if (unlink(lckf)) {
//          qDebug("Unable to unlink stale lock file: ");	
//          return false;
//        }
//      } else {
//        qDebug("Cannot read from lock file: ");	
//        return false;
//      }
//    } else {
//      qDebug("Cannot open existing lock file: ");
//      return false;
//    }
//  }
//  if ((lfh = open(lckf, O_WRONLY | O_CREAT | O_EXCL,  S_IWRITE | S_IREAD | S_IRGRP | S_IROTH)) < 0) {
//    qDebug("Cannot create lockfile. Sorry.");
//    return false;
//  }
//  sprintf(lckpidstr, "%10d\n", getpid());
//  write(lfh, lckpidstr, strlen(lckpidstr));
//  close(lfh);
//  modem_is_locked = true;
//  return true;
//
//}
//
//
//bool Modem :: unlock_device()
//{
//
//  if(! modem_is_locked  && qdev=="") {
//    qDebug("confused by unlock device, sorry !\n");
//    return false;
//  }
//  const char *device;
//
////  device = qdev.data();
//  char lckf[128];
//  char *devicename;
//
//  devicename = strrchr(device, '/');
//  sprintf(lckf, "%s/%s%s", LF_PATH, LF_PREFIX, (devicename ? (devicename + 1) : device));
//
//  if (unlink(lckf)) {
//    qDebug("Unable to unlink lock file: ");
//    return false;
//  }
//  return true;
//}
//
//
//
//int Modem::getFD() {
//  return modemfd;	
//}
//
// //Read from RS232 interface
//
//int Modem::rs232_read( void *bp, int maxlen)
//{
//
//  fd_set set;
//  struct timeval timeout;
//  int Max;
//  int endloop;
//  int res;
//
//  Max = 0;
//  endloop = 0;
//  do {
//    timeout.tv_sec = 0;
//    timeout.tv_usec = 500000; // 0.5 seconds
//    FD_ZERO(&set);
//    FD_SET(modemfd, &set);
//    res = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
//    if (0 == res) {
//      qDebug("ERROR: timed out!\n");
//      endloop = 1;
//    } else if (-1 == res) {
//      qDebug("rs232_read select");
//      exit(10);
//    } else {
//      Max += read(modemfd, bp, maxlen);
//    }
//    } while ((Max < maxlen) && !endloop);
//  return res;
//}
//
int Modem::readttybuffer( void *bp, int maxlen)
{
//
//  int res = 0;
//
//  if(modemfd > 0){
//    res += read(modemfd, bp, maxlen);
//  }
//  return res;
//
  DWORD dw=0;
  ReadFile(fd,bp,maxlen,&dw,NULL);
  return (int)dw;
}
*/
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
// #include <errno.h>            // do i really need all this ??
// #include <stdlib.h>
// #include <fcntl.h>            // ..
// #include <signal.h>
// #include <sys/ioctl.h>
// #include <setjmp.h>
// #include <qregexp.h>
// #include <qtimer.h>
// #include <assert.h>

// #include "modem_lin.h"

// #include <termios.h>
// #include <sys/types.h>
// #include <sys/stat.h>
// #include <unistd.h>
// #include <stdio.h>
// #include <signal.h>
// #include <sys/time.h>

// //  defaults for UUCP style lock files :
// #define LF_PATH             "/var/lock"
// #define LF_PREFIX           "LCK.."


// #ifndef max
// 	#define max(a,b) (((a) > (b)) ? (a) : (b))
// #endif
// #ifndef min
// 	#define min(a,b) (((a) < (b)) ? (a) : (b))
// #endif

// Modem *Modem::modem = 0;

// Modem::Modem(): modemfd(-1),sn(0L), data_mode(false),dataMask(0xFF)
// {
// //  assert(modem==0);
// //  modem = this;
//   modem_is_locked = false;
//   qdev = "";
// }

// Modem::~Modem() {
//   modem = NULL;
// }

// speed_t Modem::modemspeed() {
//   // convert the string modem speed to a t_speed type
//   // to set the modem.  The constants here should all be ifdef'd because
//   // other systems may not have them
//  int i = 576;
//   switch(i) {
//   case 24:
//     return B2400;
//     break;
//   case 96:
//     return B9600;
//     break;
//   case 192:
//     return B19200;
//     break;
//   case 384:
//     return B38400;
//     break;
//   case 576:
//     return B57600;
//     break;
// #ifdef B115200
//   case 1152:
//     return B115200;
//     break;
// #endif

// #ifdef B230400
//   case 2304:
//     return B230400;
//     break;
// #endif

// #ifdef B460800
//   case 4608:
//     return 4608;
//     break;
// #endif

//   default:
//     return B38400;
//     break;
//   }
// }

// // open serial port with settings of ConfigData
// bool Modem::opentty(QString seriell_dev) {

//   emit setTtyText("?");

// /*

//   // get device from config file
//   qdev = seriell_dev;
//   // lock the device:
//   if ( lock_device() == false ) {
// #ifdef LDEBUG
//     qDebug("Error by device locking");
// #endif    
//     return false;
//   }
//   //  int flags;
//   modemfd = open(qPrintable(qdev), O_RDWR | O_NOCTTY );
//   if(modemfd  <0) {
// #ifdef LDEBUG
//     qDebug("Sorry, can't open modem.");
// #endif
//     return false;
//   }
//   tcdrain (modemfd);
//   tcflush (modemfd, TCIOFLUSH);
//   if(tcgetattr(modemfd, &tty) < 0){
//     // this helps in some cases
//     tcsendbreak(modemfd, 0);
//     sleep(1);
//     if(tcgetattr(modemfd, &tty) < 0){
// #ifdef LDEBUG
//       qDebug("Sorry, the modem is busy.");
// #endif
//       ::close(modemfd);
//       modemfd = -1;
//       return false;
//     }
//   }
//   memset(&initial_tty,'\0',sizeof(initial_tty));
//   initial_tty = tty;
  
//   tty.c_cc[VMIN] = 0; // nonblocking
//   tty.c_cc[VTIME] = 0;
//   tty.c_oflag = 0;
//   tty.c_lflag = 0;
//   tty.c_cflag &= ~(CSIZE | CSTOPB | PARENB);
//   tty.c_cflag |= CS8 | CREAD;
//   tty.c_cflag |= CLOCAL;                   // ignore modem status lines
// //  tty.c_iflag = IGNBRK | IGNPAR | ISTRIP;  // 7BIT
//   tty.c_iflag = IGNBRK | IGNPAR;  // 8BIT
//   tty.c_lflag &= ~ICANON;                  // non-canonical mode
//   tty.c_lflag &= ~(ECHO|ECHOE|ECHOK|ECHOKE);

//   cfsetospeed(&tty, modemspeed());
//   cfsetispeed(&tty, modemspeed());
//   tcdrain(modemfd);
//   if(tcsetattr(modemfd, TCSANOW, &tty) < 0){
// #ifdef LDEBUG
//     qDebug("Sorry, the modem is busy.");
// #endif
//     ::close(modemfd);
//     modemfd=-1;
//     return false;
//   }
// #ifdef LDEBUG  
//   qDebug("Modem Ready.");
// #endif
// */

//   int flags=O_RDWR;

//   modemfd=open(seriell_dev.toLatin1(), flags | O_NDELAY);
//   if (modemfd<0)
//   {
//      return false;
//   }
//   tcflush(modemfd, TCIOFLUSH);

//   struct termios newtio;
//   if (tcgetattr(modemfd, &newtio)!=0)
//   {
//     return false;
//   }
//   //_baud=B57600;

//   cfsetospeed(&newtio, B57600);
//   cfsetispeed(&newtio, B57600);

//   newtio.c_cflag = (newtio.c_cflag & ~CSIZE) | CS8;//8 Databit
//   newtio.c_cflag |= CLOCAL | CREAD;
//   newtio.c_cflag &= ~(PARENB | PARODD);//no parity
//   newtio.c_cflag &= ~CRTSCTS;//kein hardware handshake
//   //newtio.c_cflag |= CSTOPB; //2 Stopbit
//   newtio.c_cflag &= ~CSTOPB; //1 Stopbit
//   newtio.c_iflag=IGNBRK;
//   newtio.c_iflag &= ~(IXON|IXOFF|IXANY); //kein Softwarehandshake
//   newtio.c_lflag=0;
//   newtio.c_oflag=0;
//   newtio.c_cc[VTIME]=0;
//   newtio.c_cc[VMIN]=0;

//   if (tcsetattr(modemfd, TCSANOW, &newtio)!=0)
//   {
//     return false;
//   }

//   int mcs=0;
//   ioctl(modemfd, TIOCMGET, &mcs);
//   mcs |= TIOCM_RTS;
//   ioctl(modemfd, TIOCMSET, &mcs);

//   if (tcgetattr(modemfd, &newtio)!=0)
//   {
//     return false;
//   }
//   //newtio.c_cflag &= ~CRTSCTS; //kein hardware handshake
//   //if (tcsetattr(modemfd, TCSANOW, &newtio)!=0)
//   //{
//   //  return false;
//   //}

//   emit setTtyText(seriell_dev);
//   return true;
// }

// bool Modem::closetty() {
//   if(modemfd >=0 ) {
//     stop();
//     // discard data not read or transmitted 
//     tcflush(modemfd, TCIOFLUSH);
//     if(tcsetattr(modemfd, TCSANOW, &initial_tty) < 0){
// #ifdef LDEBUG
//       qDebug("Can't restore tty settings: tcsetattr()\n");
// #endif
//       ::close(modemfd);
//       modemfd = -1;
//       return false;
//     }
//     ::close(modemfd);
//     modemfd = -1;
//   }
//   if ( unlock_device() == false )  {
// #ifdef LDEBUG
//     qDebug("cannot unlock device !");
// #endif
//     return false;
//   }
//   return true;
// }

// void Modem::readtty(int) {
//   char buffer[50];
//   unsigned char c;
//   int len;

//   // read data in chunks of up to 50 bytes
//   if((len = ::read(modemfd, buffer, 50)) > 0) {
//     // split buffer into single characters for further processing
//     for(int i = 0; i < len; i++) {
//       c = buffer[i] & dataMask;
//       emit charWaiting(c);
//     }
//   }
// }

// void Modem::stop() {
//   disconnect(SIGNAL(charWaiting(unsigned char)));
//   stopNotifier();
// }

// void Modem::startNotifier() {
//   if(modemfd >= 0) {
//     if(sn == 0) {
//       sn = new QSocketNotifier(modemfd, QSocketNotifier::Read, this);
//       connect(sn, SIGNAL(activated(int)), SLOT(readtty(int)));
// #ifdef LDEBUG
//       qDebug("QSocketNotifier started!");
// #endif
//     } else {
//       sn->setEnabled(true);
//     }
//   }
// }

// void Modem::stopNotifier() {
//   if(sn != 0) {
//     sn->setEnabled(false);
//     disconnect(sn);
//     delete sn;
//     sn = 0;
// #ifdef LDEBUG
//     qDebug("QSocketNotifier stopped!");
// #endif
//   }
// }

// bool Modem::writeChar(unsigned char c) {
//   return write(modemfd, &c, 1) == 1;
// }

// bool Modem::writeLine(QString *buf) {
//   size_t a,b;
//   a = strlen(buf);
//   b = write(modemfd, buf, a);
// //Let's send an "enter"
// //  write(modemfd, "\r", 1);
//   if(a==b){
//     return true;
//   }else{
//     return false;
//   }
// }


// bool Modem :: lock_device()
// {
//   const char *device = qPrintable(qdev);
// //  device = qdev;
//   char	lckf[128];
//   int	lfh;
//   pid_t	lckpid;
//   char	lckpidstr[20];
//   int	nb;
//   struct stat buf;
//   const char *devicename = strrchr(device, '/');
//   sprintf(lckf, "%s/%s%s", LF_PATH, LF_PREFIX, (devicename ? (devicename + 1) : device));
//   //
//   // Check if there's already a LCK..* file
  
//   if (stat(lckf, &buf) == 0) {
//   //
//   // we must now expend effort to learn if it's stale or not.
//   //
//     if ((lfh = open(lckf, O_RDONLY)) != -1) {
//       nb = read(lfh, &lckpidstr, min(20, buf.st_size));
//       if (nb > 0) {
//         lckpidstr[nb] = 0;
//         sscanf(lckpidstr, "%d", &lckpid);
//         if (kill(lckpid, 0) == 0) {
// #ifdef LDEBUG
//           qDebug("Device is locked by process ");
// #endif
//           return false;
//         }
//       //
//       // The lock file is stale. Remove it.
//       //
//         if (unlink(lckf)) {
// #ifdef LDEBUG
//           qDebug("Unable to unlink stale lock file: ");	
// #endif
//           return false;
//         }
//       } else {
// #ifdef LDEBUG
//         qDebug("Cannot read from lock file: ");	
// #endif
//         return false;
//       }
//     } else {
// #ifdef LDEBUG
//       qDebug("Cannot open existing lock file: ");
// #endif
//       return false;
//     }
//   }
//   if ((lfh = open(lckf, O_WRONLY | O_CREAT | O_EXCL,  S_IWRITE | S_IREAD | S_IRGRP | S_IROTH)) < 0) {
// #ifdef LDEBUG
//     qDebug("Cannot create lockfile. Sorry.");
// #endif
//     return false;
//   }
//   sprintf(lckpidstr, "%10d\n", getpid());
//   size_t a = strlen(lckpidstr);
//   size_t b = write(lfh, lckpidstr, a);
//   close(lfh);
//   if(a != b)return false;
//   modem_is_locked = true;
//   return true;
// }

// bool Modem :: unlock_device()
// {

//   if(! modem_is_locked  && qdev=="") {
// #ifdef LDEBUG
//     qDebug("confused by unlock device, sorry !\n");
// #endif
//     return false;
//   }
//   const char *device = qPrintable(qdev);
//   char lckf[128];
//   const char *devicename = strrchr(device, '/');
//   sprintf(lckf, "%s/%s%s", LF_PATH, LF_PREFIX, (devicename ? (devicename + 1) : device));

//   if (unlink(lckf)) {
// #ifdef LDEBUG
//     qDebug("Unable to unlink lock file: ");
// #endif
//     return false;
//   }
//   return true;
// }



// int Modem::getFD() {
//   return modemfd;	
// }

// //
// // Read from RS232 interface
// //
// int Modem::rs232_read( void *bp, int maxlen)
// {

//   fd_set set;
//   struct timeval timeout;
//   int Max;
//   int endloop;
//   int res;

//   Max = 0;
//   endloop = 0;
//   do {
//     timeout.tv_sec = 0;
//     timeout.tv_usec = 500000; // 0.5 seconds 
//     FD_ZERO(&set);
//     FD_SET(modemfd, &set);
//     res = select(FD_SETSIZE, &set, NULL, NULL, &timeout);
//     if (0 == res) {
// #ifdef LDEBUG
//       qDebug("ERROR: timed out!\n");
// #endif
//       endloop = 1;
//     } else if (-1 == res) {
// #ifdef LDEBUG
//       qDebug("rs232_read select");
// #endif
//       exit(10);
//     } else {
//       Max += read(modemfd, bp, maxlen);
//     }
//     } while ((Max < maxlen) && !endloop);
//   return res;
// }

// int Modem::readttybuffer( void *bp, int maxlen)
// {
//   int res = 0;

//   if(modemfd > 0){
//     res += read(modemfd, bp, maxlen);
//   }
//   return res;
// }



// const QString Modem::modemMessage() {
//   return errmsg;
// }

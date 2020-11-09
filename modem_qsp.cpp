//#############################################################################
// Quelltext fuer Windows
//#############################################################################
#include <QDebug>
#include <QSerialPort>
#include <cstring>
#include <memory>

#include "modem.hpp"

/**
 * @brief Modem implemented via QSerialPort for portability
 */
class ModemQSP : public Modem {
 public:
  ModemQSP() {}

  virtual bool opentty(QString seriell_dev) {
    qDebug() << "ModemQSP::opentty: " << seriell_dev;
    port_.close();
    port_.setPortName(seriell_dev);
    bool res = true;
    if (!port_.open(QIODevice::ReadWrite)) {
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
    if (!port_.setBaudRate(57600)) {
      qWarning("QSerialPort::setBaudRate() failed");
      return false;
    }
    if (!port_.setParity(QSerialPort::NoParity)) {
      qWarning("QSerialPort::setParity() failed");
      return false;
    }
    if (!port_.setStopBits(QSerialPort::OneStop)) {
      qWarning("QSerialPort::setStopBits() failed");
      return false;
    }
    if (!port_.setDataBits(QSerialPort::Data8)) {
      qWarning("QSerialPort::setDataBits() failed");
      return false;
    }
    if (!port_.setFlowControl(QSerialPort::NoFlowControl)) {
      qWarning("QSerialPort::setFlowControl() failed");
      return false;
    }
    if (!port_.setRequestToSend(true)) {
      qWarning("QSerialPort::setRequestToSend() failed");
      return false;
    }
    if (!port_.setDataTerminalReady(true)) {
      qWarning("QSerialPort::setDataTerminalReady() failed");
      return false;
    }
    return true;
  }
  /** unlock and close serial port */
  virtual bool closetty() {
    qDebug() << "ModemQSP::closetty";
    port_.close();
    return true;
  }
  /** write a char to port */
  virtual bool writeChar(unsigned char uc) {
    qDebug() << "ModemQSP::writeChar: " << uc;
    char c = uc;
    port_.write(&c, 1);
    return true;
  }
  /** write a char[] to port*/
  virtual bool writeLine(QString& ln) {
    qDebug() << "ModemQSP::writeLine: " << ln;
    QByteArray ba = ln.toLatin1();
    port_.write(ba);
    return true;
  }
  // neue funktion fuer den NWT7
  virtual int readttybuffer(void* p, int n) {
    qDebug() << "ModemQSP::readttybuffer: " << p << " " << n << "...";

    QByteArray ba = port_.read(n);
    std::memcpy(p, ba.data(), ba.length());
    qDebug() << "... read: " << ba;
    return ba.length();
  }

 private:
  QSerialPort port_;
};

static ModemQSP static_modem;

Modem* Modem::GetModem() {
  //
  return &static_modem;
}

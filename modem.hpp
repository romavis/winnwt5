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

#include <QString>

/**
 * @brief Abstract class for GUI-UART interface
 */
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
  // neue funktion fuer den NWT7
  virtual int readttybuffer(void *, int) = 0;
};

#endif  // H_MODEM_HPP_
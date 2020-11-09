/***************************************************************************
    firmware.cpp

    firmwiget.cpp  -  description
    ----------------------------------------------------
    begin                : 2005
    copyright            : (C) 2005 by Andreas Lindenau
    email                : DL4JAL@darc.de

    firmwiget.cpp have been refactored to firmware.cpp QT5
    ----------------------------------------------------
    begin                : 2013
    copyright            : (C) 2013 by Andrey Sobol
    email                : andrey.sobol.nn@gmail.com

 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef FIRMWIDGET_H
#define FIRMWIDGET_H

#include <QDialog>
#include <QLabel>
#include <QString>

#include "constdef.h"

class FirmwareDlg : public QDialog
{
    Q_OBJECT

public:
  FirmwareDlg(QWidget* parent=0, Qt::WindowFlags f=0);

public slots:
  void setText(QString s);

private:
  QLabel *label1;
};

#endif // FIRMWIDGET_H

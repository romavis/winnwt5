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

#include "firmwaredlg.h"


//#####################################################################################
// Class FirmwareDlg
//#####################################################################################
FirmwareDlg::FirmwareDlg(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
  this->resize(450,100);
  label1 = new QLabel(this);
  label1->setGeometry(10,10,400,30);
  label1->setText(tr("Firmware Update",""));
  label1->setAlignment(Qt::AlignHCenter);
}

void FirmwareDlg::setText(QString s)
{
  setFocus();
  label1->setText(s);
  repaint();
}

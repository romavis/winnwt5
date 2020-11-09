/***************************************************************************

    sondedlg.cpp  -  description
    ----------------------------------------------------
    begin                : 2008
    copyright            : (C) 2005 by Andreas Lindenau
    email                : DL4JAL@darc.de

    sondedlg.cpp  have been refactored to optiondlg.cpp QT5
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

#ifndef SONDEDLG_H
#define SONDEDLG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qstring.h>
#include <QScrollArea>

#include "constdef.h"

class ProbeDlg : public QDialog
{
    Q_OBJECT

public:
  ProbeDlg(QWidget* parent=0, Qt::WindowFlags f=0);
  ~ProbeDlg();
  void setdaten(const TProbe &asonde);
  TProbe getdaten();
  void tip(bool);

private slots:

private:
  TProbe osonde;
  
  QScrollArea *scrollBar;
  QPushButton *ok;
  QPushButton *cancel;
  //QGroupBox *gkopf;
  //QLabel *label;
  QLabel *labelstr;
  QLabel *labeloff;
  //QLineEdit *sname;
  QGroupBox *goffset;
  QLineEdit *stroffset[30];
  QLineEdit *offset[30];
};

#endif // SONDEDLG_H

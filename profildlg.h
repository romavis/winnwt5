/***************************************************************************

    profildlg.cpp  -  description
    ----------------------------------------------------
    begin                : 2005
    copyright            : (C) 2005 by Andreas Lindenau
    email                : DL4JAL@darc.de

    profildlg.cpp have been refactored to profildlg.cpp QT5
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

#ifndef PROFILDLG_H
#define PROFILDLG_H

#include <qdialog.h>
#include <qpushbutton.h>
#include <qdialog.h>
#include <qgroupbox.h>
#include <qcombobox.h>
/*#include <q3listbox.h>*/
#include <QListWidget>
#include <qlabel.h>
#include <qcheckbox.h>
#include <qlineedit.h>
#include <qstring.h>

#include "constdef.h"

class ProfileDlg : public QDialog
{
    Q_OBJECT

public:
  ProfileDlg(QWidget * parent = 0, Qt::WindowFlags f = 0);
  void setprofil(TProfile aprofil,int index);
  TProfile getprofil(int);
  void tip(bool atip);
  
private slots:
  QString linenormalisieren(const QString &);
  void allespruefen();
  void allesloeschen();

private:
//  tprofil profilarray[profilanz];
  
  QPushButton *ok;
  QPushButton *cancel;
  QPushButton *pruefen;
  QPushButton *loeschen;
  
  QLineEdit *eanfang[profilanz];
  QLineEdit *eende[profilanz];
  QLineEdit *eschritte[profilanz];
  QLineEdit *ebeschreibung[profilanz];
  QLabel *labelbeschreibung;
  QLabel *labelanfang;
  QLabel *labelende;
  QLabel *labelschritte;
  
};

#endif // PROFILDLG_H

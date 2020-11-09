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

#include <qdialog.h>
#include <QString>
#include "profildlg.h"
//#include "modem.h"


//#####################################################################################
// Class fuer Option
//#####################################################################################
ProfileDlg::ProfileDlg(QWidget * parent, Qt::WindowFlags f): QDialog(parent, f)
{
  int a = 32;

  this->resize(375,10+a*22);
  this->setMinimumSize(350,350);
  this->setWindowTitle(tr("Set Profile",""));
  ok = new QPushButton("OK", this);
  ok->setGeometry(10,10+a*21,80,30);
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  cancel = new QPushButton(tr("Cancel",""), this);
  cancel->setGeometry(95,10+a*21,80,30);
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));
  pruefen = new QPushButton(tr("Normal",""), this);
  pruefen->setGeometry(180,10+a*21,90,30);
  connect(pruefen, SIGNAL(clicked()), SLOT(allespruefen()));
  loeschen = new QPushButton(tr("Delete",""), this);
  loeschen->setGeometry(275,10+a*21,80,30);
  connect(loeschen, SIGNAL(clicked()), SLOT(allesloeschen()));
  labelbeschreibung = new QLabel(tr("Name",""), this);
  labelbeschreibung->setGeometry(50,10,100,30);
  labelanfang = new QLabel(tr("Start",""), this);
  labelanfang->setGeometry(180,10,100,30);
  labelende = new QLabel(tr("Stop",""), this);
  labelende->setGeometry(270,10,100,30);
  labelschritte = new QLabel(tr("Samples",""), this);
  labelschritte->setGeometry(320,10,100,30);
  int i;
  QString qs;
  for(i=0;i<profilanz;i++){
    ebeschreibung[i] = new QLineEdit(this);
    ebeschreibung[i]->setText("");
    ebeschreibung[i]->setGeometry(10,40+i*32,140,30);
    eanfang[i] = new QLineEdit(this);
    eanfang[i]->setGeometry(155,40+i*32,80,30);
    eanfang[i]->setText("");
    eende[i] = new QLineEdit(this);
    eende[i]->setGeometry(240,40+i*32,80,30);
    eende[i]->setText("");
  }
  for(i=0;i<profilanz;i++){
    eschritte[i] = new QLineEdit(this);
    eschritte[i]->setGeometry(325,40+i*32,40,30);
    eschritte[i]->setText("");
  }
};

void ProfileDlg::setprofil(TProfile aprofil, int index)
{
  QString qs;
  
  if(index < profilanz){
    eanfang[index]->setText(aprofil.fqstart);
    eende[index]->setText(aprofil.fqend);
    eschritte[index]->setText(aprofil.cntpoints);
    ebeschreibung[index]->setText(aprofil.beschr);
  }
}

TProfile ProfileDlg::getprofil(int a)
{ 
  TProfile cprofil;
  QString qs;
  
  if(a < profilanz){
    eanfang[a]->setText(linenormalisieren(eanfang[a]->text()));
    eende[a]->setText(linenormalisieren(eende[a]->text()));
    cprofil.fqstart = eanfang[a]->text();
    cprofil.fqend = eende[a]->text();
    qs = ebeschreibung[a]->text();
    qs.toUpper();
    if(qs.isEmpty()){
      cprofil.beschr = "NN";
    }else{
      cprofil.beschr = ebeschreibung[a]->text();
    }
    qs = eschritte[a]->text();
    if(qs.isEmpty()){
      cprofil.cntpoints = "1001";
    }else{
      cprofil.cntpoints = eschritte[a]->text();
    }
  }else{
    cprofil.beschr = "NN";
  }
  return cprofil;
}

//Eingabeumwandlung mit "m" und "k"
QString ProfileDlg::linenormalisieren(const QString &line)
{
  bool ok;
  int pos, l;
  double faktor=1.0;
  double ergebnis = 0.0;
  
  QString aline(line);
  aline.toLower();	//alles auf Kleinschreibung
  l = aline.length();
  pos = aline.indexOf('g',0,Qt::CaseInsensitive);
  if(pos != -1){
    if((l-1) == pos){
      aline.remove(pos,1);
    }else{
      aline.replace(pos,1,'.');
    }  
    faktor = 1000000000.0;
  }
  pos = aline.indexOf('m',0,Qt::CaseInsensitive);
  if(pos != -1){
    if((l-1) == pos){
      aline.remove(pos,1);
    }else{
      aline.replace(pos,1,'.');
    }  
    faktor = 1000000.0;
  }
  pos = aline.indexOf('k',0,Qt::CaseInsensitive);
  if(pos != -1){
    if((l-1) == pos){
      aline.remove(pos,1);
    }else{
      aline.replace(pos,1,'.');
    }  
    faktor = 1000.0;
  }
  ergebnis = aline.toDouble(&ok);
  ergebnis *= faktor;
  aline.sprintf("%1.0f",ergebnis);
  return aline;
}

void ProfileDlg::allespruefen()
{
  int i;
  QString qs;
  
  for(i=0;i<profilanz;i++){
    qs = ebeschreibung[i]->text();
    if(!qs.isEmpty()){
      eanfang[i]->setText(linenormalisieren(eanfang[i]->text()));
      eende[i]->setText(linenormalisieren(eende[i]->text()));
    }
  }  
}

void ProfileDlg::allesloeschen()
{
  int i;
  QString qs;
  
  for(i=0;i<profilanz;i++){
    eanfang[i]->setText("");
    eende[i]->setText("");
    ebeschreibung[i]->setText("");
    eschritte[i]->setText("");
  }  
}

void ProfileDlg::tip(bool atip)
{
  int i;
  QString tip_schritte = ProfileDlg::tr("<b>Samples :</b><br><br>If not Set, then 1001 Samples are used automatically<br><br>","");
  QString tip_edit = ProfileDlg::tr("<b> Input: </b> <br> 3m5 or 3.5m or 3.5m = 3 500 000 Hz <br> <b> Input: </b> <br> 350k5 or 350,5k or 350.5k = 350 500 Hz","");
  QString tip_name = ProfileDlg::tr("<br><b>Description :</b><br><br>This Field is used for the Description of the profile.<br>You can clear the Profile with \"NN\".<br>This string appears in the ComboBox \"Profile\"<br>of the Sweepmode Tab.<br>","");
  if(atip){
    for(i=0;i<profilanz;i++){
      ebeschreibung[i]->setToolTip(tip_name);
      eanfang[i]->setToolTip(tip_edit);
      eende[i]->setToolTip(tip_edit);
      eschritte[i]->setToolTip(tip_schritte);
    }
  }else{
    for(i=0;i<profilanz;i++){
      ebeschreibung[i]->setToolTip("");
      eanfang[i]->setToolTip("");
      eende[i]->setToolTip("");
      eschritte[i]->setToolTip("");
    }
  }
}

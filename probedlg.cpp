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

#include <qdialog.h>

#include "probedlg.h"
//#include "tooltipsonde.h"


//#####################################################################################
// Class fuer Option
//#####################################################################################
ProbeDlg::ProbeDlg(QWidget * parent, Qt::WindowFlags f): QDialog(parent, f)
{
  this->resize(450,1090);
  this->setMinimumSize(0,0);
  ok = new QPushButton(tr("Save",""), this);
  ok->setGeometry(10,10,100,30);
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  cancel = new QPushButton(tr("Exit",""), this);
  cancel->setGeometry(120,10,100,30);
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));
//  gkopf = new QGroupBox(this);
//  gkopf->setGeometry(150,10,270,60);
//  gkopf->setTitle(tr("Messkopf","Sonde Dialog"));
//  label = new QLabel(gkopf);
//  label->setGeometry(10,25,60,30);
//  label->setText(tr("Datei","Sonde Dialog"));
//  label->setAlignment(Qt::AlignRight);
//  sname = new QLineEdit(gkopf);
//  sname->setReadOnly(true);
//  sname->setGeometry(80,20,170,30);
  goffset = new QGroupBox(this);
  goffset->setGeometry(10,60,430,1020);
  goffset->setTitle(tr("Wattmeter Error",""));
  goffset->setAlignment(Qt::AlignCenter);
  int h = 30;
  int h1 = 31;
  labelstr = new QLabel(goffset);
  labelstr->setGeometry(10,h,250,30);
  labelstr->setAlignment(Qt::AlignCenter);
  labelstr->setText(tr("Text in the ComboBox",""));
  labeloff = new QLabel(goffset);
  labeloff->setGeometry(270,h,150,30);
  labeloff->setAlignment(Qt::AlignCenter);
  labeloff->setText(tr("Deviation in dB",""));
  h += h1;
  for(int i=0;i<30;i++){
    stroffset[i] = new QLineEdit(goffset);
    stroffset[i]->setGeometry(10,h,300,30);
    stroffset[i]->setAlignment(Qt::AlignCenter);
    offset[i] = new QLineEdit(goffset);
    offset[i]->setGeometry(320,h,100,30);
    offset[i]->setAlignment(Qt::AlignCenter);
    h += h1;
  }
  scrollBar = new QScrollArea;
  scrollBar->setWidget(this);
  scrollBar->resize(500,500);
  scrollBar->show();
};

ProbeDlg::~ProbeDlg()
{
  scrollBar->close();
}


void ProbeDlg::setdaten(const TProbe &asonde){
  QString s;
  osonde = asonde;
  scrollBar->setWindowTitle(tr("%1 - [%2]","").arg(tr("HFM9/NWT","")).arg(osonde.dname));
  //sname->setText(osonde.sname);
  for(int i=0;i<30;i++){
    stroffset[i]->setText(osonde.stroffset[i]);
    s.sprintf("%2.2f", osonde.offset[i]);
    offset[i]->setText(s);
  }
};

TProbe ProbeDlg::getdaten(){
  QString s;
  
  //osonde.sname = sname->text();
  for(int i=0;i<30;i++){
    s = offset[i]->text();
    osonde.offset[i] = s.toDouble();
    osonde.stroffset[i] = stroffset[i]->text();
  }
  return osonde;
};

void ProbeDlg::tip(bool btip)
{
  

QString tip_sondenname = tr("<br><b> Name of the Channel placed in the Graphical Display :</b> <br><br> This Name  describes the Channel.<br>This is meant,  above all,   for the External sensor,<br>so that we know which Channel is used.",""
  );
QString tip_soffsetstr = tr("<br><b> String to display the error :</b> <br><br> The Channels have several errors depending on the Frequency.<br>This string displays the Frequency and the Error <br>",""
  );
QString tip_soffset = tr("<br><b>Correction value :</B> <br> <br>Enter here the correction Value.<br><br>If the channel has eg. a<b> negative</b> error of 10dB at 500MHz, <br> then here a <b>positive</b> value shall be entered as the correction.<br> In this case it is <b> 10.0 </b> for the Correction.<br>",""
  );
  
  if(btip){
    //sname->setToolTip(tip_sondenname);
    for(int i=0;i<30;i++){
      stroffset[i]->setToolTip(tip_soffsetstr);
      offset[i]->setToolTip(tip_soffset);
    }
  }else{
    for(int i=0;i<30;i++){
      stroffset[i]->setToolTip("");
      offset[i]->setToolTip("");
    }
  }
}

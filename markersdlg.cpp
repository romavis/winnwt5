/***************************************************************************
    markersdlg.cpp

    wkmanager.cpp  -  description
    ----------------------------------------------------
    begin                : 2007
    copyright            : (C) 2007 by Andreas Lindenau
    email                : DL4JAL@darc.de

    markersdlg.cpp  -  description
    wkmanager.cpp  have been refactored to MarkersDlg.cpp QT5
    ----------------------------------------------------
    begin                : 2014
    copyright            : (C) 2014 by Andrey Sobol
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

#include <QColorDialog>
#include <QColor>
#include <QIcon>
#include <QFileDialog>
#include <QTextStream>
#include <QFile>
#include <QDateTime>
#include <QPen>

#include "configfile.h"
#include "markersdlg.h"
#include "modem.hpp"
//#include "tooltipwkm.h"
//#include "tooltipodlg.h"


//#####################################################################################
// Class fuer Option
//#####################################################################################
MarkersDlg::MarkersDlg(QWidget * parent, Qt::WindowFlags f): QWidget(parent, f)
{
  this->resize(450,330);
  this->setMinimumSize(0,0);
  this->setWindowTitle(tr("Graph Manager",""));
  this->setWindowIcon(QIcon("appicon.png"));

  labelk1 = new QLabel(tr("Graph 1",""),this);
  labelk1->setGeometry(180, 10, 60, 20);
  labelk2 = new QLabel(tr("Graph 2",""),this);
  labelk2->setGeometry(240, 10, 60, 20);
  labelk3 = new QLabel(tr("Graph 3",""),this);
  labelk3->setGeometry(300, 10, 60, 20);
  labelk4 = new QLabel(tr("Graph 4",""),this);
  labelk4->setGeometry(360, 10, 60, 20);
  labelk5 = new QLabel(tr("Graph 5",""),this);
  labelk5->setGeometry(420, 10, 60, 20);
  labelk6 = new QLabel(tr("Graph 6",""),this);
  labelk6->setGeometry(480, 10, 60, 20);

  labelopen = new QLabel(tr("Load Graph",""),this);
  labelopen->setGeometry(10, 45, 150, 20);
  labelopen->setAlignment(Qt::AlignRight);
  
  labelsave = new QLabel(tr("Save Graph",""),this);
  labelsave->setGeometry(10, 70, 150, 20);
  labelsave->setAlignment(Qt::AlignRight);
  
  labeluebernahme = new QLabel(tr("Get Graph",""),this);
  labeluebernahme->setGeometry(10, 95, 150, 20);
  labeluebernahme->setAlignment(Qt::AlignRight);
  
  labelcolor = new QLabel(tr("Graph Color",""),this);
  labelcolor->setGeometry(10, 120, 150, 20);
  labelcolor->setAlignment(Qt::AlignRight);
  
  labelkanal = new QLabel(tr("Activate Channel",""),this);
  labelkanal->setGeometry(10, 145, 150, 20);
  labelkanal->setAlignment(Qt::AlignRight);
  
  labeleinblendung = new QLabel(tr("Show Graph",""),this);
  labeleinblendung->setGeometry(10, 170, 150, 20);
  labeleinblendung->setAlignment(Qt::AlignRight);
  
  open1 = new QPushButton(tr("Load",""), this);
  open1->setGeometry(180,40,60,25);
  connect(open1, SIGNAL(clicked()), SLOT(sopen1()));
  
  open2 = new QPushButton(tr("Load",""), this);
  open2->setGeometry(240,40,60,25);
  connect(open2, SIGNAL(clicked()), SLOT(sopen2()));
  
  open3 = new QPushButton(tr("Load",""), this);
  open3->setGeometry(300,40,60,25);
  connect(open3, SIGNAL(clicked()), SLOT(sopen3()));
  
  open4 = new QPushButton(tr("Load",""), this);
  open4->setGeometry(360,40,60,25);
  connect(open4, SIGNAL(clicked()), SLOT(sopen4()));

  open5 = new QPushButton(tr("Load",""), this);
  open5->setGeometry(420,40,60,25);
  connect(open5, SIGNAL(clicked()), SLOT(sopen5()));

  open6 = new QPushButton(tr("Load",""), this);
  open6->setGeometry(480,40,60,25);
  connect(open6, SIGNAL(clicked()), SLOT(sopen6()));

  save1 = new QPushButton(tr("Save",""), this);
  save1->setGeometry(180,65,60,25);
  save1->setEnabled(false);
  connect(save1, SIGNAL(clicked()), SLOT(ssave1()));
  
  save2 = new QPushButton(tr("Save",""), this);
  save2->setGeometry(240,65,60,25);
  save2->setEnabled(false);
  connect(save2, SIGNAL(clicked()), SLOT(ssave2()));
  
  save3 = new QPushButton(tr("Save",""), this);
  save3->setGeometry(300,65,60,25);
  save3->setEnabled(false);
  connect(save3, SIGNAL(clicked()), SLOT(ssave3()));
  
  save4 = new QPushButton(tr("Save",""), this);
  save4->setGeometry(360,65,60,25);
  save4->setEnabled(false);
  connect(save4, SIGNAL(clicked()), SLOT(ssave4()));

  save5 = new QPushButton(tr("Save",""), this);
  save5->setGeometry(420,65,60,25);
  save5->setEnabled(false);
  connect(save5, SIGNAL(clicked()), SLOT(ssave5()));

  save6 = new QPushButton(tr("Save",""), this);
  save6->setGeometry(480,65,60,25);
  save6->setEnabled(false);
  connect(save6, SIGNAL(clicked()), SLOT(ssave6()));

  uebernahme1 = new QPushButton(tr("Get",""), this);
  uebernahme1->setGeometry(180,90,60,25);
  connect(uebernahme1, SIGNAL(clicked()), SLOT(sholen1()));
  
  uebernahme2 = new QPushButton(tr("Get",""), this);
  uebernahme2->setGeometry(240,90,60,25);
  connect(uebernahme2, SIGNAL(clicked()), SLOT(sholen2()));
  
  uebernahme3 = new QPushButton(tr("Get",""), this);
  uebernahme3->setGeometry(300,90,60,25);
  connect(uebernahme3, SIGNAL(clicked()), SLOT(sholen3()));
  
  uebernahme4 = new QPushButton(tr("Get",""), this);
  uebernahme4->setGeometry(360,90,60,25);
  connect(uebernahme4, SIGNAL(clicked()), SLOT(sholen4()));

  uebernahme5 = new QPushButton(tr("Get",""), this);
  uebernahme5->setGeometry(420,90,60,25);
  connect(uebernahme5, SIGNAL(clicked()), SLOT(sholen5()));
  
  uebernahme6 = new QPushButton(tr("Get",""), this);
  uebernahme6->setGeometry(480,90,60,25);
  connect(uebernahme6, SIGNAL(clicked()), SLOT(sholen6()));

  color11 = new QPushButton("C1", this);
  color11->setGeometry(180,115,30,25);
  connect(color11, SIGNAL(clicked()), SLOT(scolor11()));
  color12 = new QPushButton("C2", this);
  color12->setGeometry(210,115,30,25);
  connect(color12, SIGNAL(clicked()), SLOT(scolor12()));
  
  color21 = new QPushButton("C1", this);
  color21->setGeometry(240,115,30,25);
  connect(color21, SIGNAL(clicked()), SLOT(scolor21()));
  color22 = new QPushButton("C2", this);
  color22->setGeometry(270,115,30,25);
  connect(color22, SIGNAL(clicked()), SLOT(scolor22()));
  
  color31 = new QPushButton("C1", this);
  color31->setGeometry(300,115,30,25);
  connect(color31, SIGNAL(clicked()), SLOT(scolor31()));
  color32 = new QPushButton("C2", this);
  color32->setGeometry(330,115,30,25);
  connect(color32, SIGNAL(clicked()), SLOT(scolor32()));
  
  color41 = new QPushButton("C1", this);
  color41->setGeometry(360,115,30,25);
  connect(color41, SIGNAL(clicked()), SLOT(scolor41()));
  color42 = new QPushButton("C2", this);
  color42->setGeometry(390,115,30,25);
  connect(color42, SIGNAL(clicked()), SLOT(scolor42()));
  
  color51 = new QPushButton("C1", this);
  color51->setGeometry(420,115,30,25);
  connect(color51, SIGNAL(clicked()), SLOT(scolor51()));
  color52 = new QPushButton("C2", this);
  color52->setGeometry(450,115,30,25);
  connect(color52, SIGNAL(clicked()), SLOT(scolor52()));

  color61 = new QPushButton("C1", this);
  color61->setGeometry(480,115,30,25);
  connect(color61, SIGNAL(clicked()), SLOT(scolor61()));
  color62 = new QPushButton("C2", this);
  color62->setGeometry(510,115,30,25);
  connect(color62, SIGNAL(clicked()), SLOT(scolor62()));

  checkboxk11 = new QCheckBox("1", this);
  checkboxk11->setGeometry(180,140,30,25);
  checkboxk11->setEnabled(false);
  connect(checkboxk11, SIGNAL(clicked()), this, SLOT(saktivk11()));
  checkboxk12 = new QCheckBox("2", this);
  checkboxk12->setGeometry(210,140,30,25);
  checkboxk12->setEnabled(false);
  connect(checkboxk12, SIGNAL(clicked()), this, SLOT(saktivk12()));
  
  checkboxk21 = new QCheckBox("1", this);
  checkboxk21->setGeometry(240,140,30,25);
  checkboxk21->setEnabled(false);
  connect(checkboxk21, SIGNAL(clicked()), this, SLOT(saktivk21()));
  checkboxk22 = new QCheckBox("2", this);
  checkboxk22->setGeometry(270,140,30,25);
  checkboxk22->setEnabled(false);
  connect(checkboxk22, SIGNAL(clicked()), this, SLOT(saktivk22()));
  
  checkboxk31 = new QCheckBox("1", this);
  checkboxk31->setGeometry(300,140,30,25);
  checkboxk31->setEnabled(false);
  connect(checkboxk31, SIGNAL(clicked()), this, SLOT(saktivk31()));
  checkboxk32 = new QCheckBox("2", this);
  checkboxk32->setGeometry(330,140,30,25);
  checkboxk32->setEnabled(false);
  connect(checkboxk32, SIGNAL(clicked()), this, SLOT(saktivk32()));
  
  checkboxk41 = new QCheckBox("1", this);
  checkboxk41->setGeometry(360,140,30,25);
  checkboxk41->setEnabled(false);
  connect(checkboxk41, SIGNAL(clicked()), this, SLOT(saktivk41()));
  checkboxk42 = new QCheckBox("2", this);
  checkboxk42->setGeometry(390,140,30,25);
  checkboxk42->setEnabled(false);
  connect(checkboxk42, SIGNAL(clicked()), this, SLOT(saktivk42()));
  
  checkboxk51 = new QCheckBox("1", this);
  checkboxk51->setGeometry(420,140,30,25);
  checkboxk51->setEnabled(false);
  connect(checkboxk51, SIGNAL(clicked()), this, SLOT(saktivk51()));
  checkboxk52 = new QCheckBox("2", this);
  checkboxk52->setGeometry(450,140,30,25);
  checkboxk52->setEnabled(false);
  connect(checkboxk52, SIGNAL(clicked()), this, SLOT(saktivk52()));

  checkboxk61 = new QCheckBox("1", this);
  checkboxk61->setGeometry(480,140,30,25);
  checkboxk61->setEnabled(false);
  connect(checkboxk61, SIGNAL(clicked()), this, SLOT(saktivk61()));
  checkboxk62 = new QCheckBox("2", this);
  checkboxk62->setGeometry(510,140,30,25);
  checkboxk62->setEnabled(false);
  connect(checkboxk62, SIGNAL(clicked()), this, SLOT(saktivk62()));


  aktiv1 = new QCheckBox(tr("Active",""), this);
  aktiv1->setGeometry(180,165,60,25);
  aktiv1->setEnabled(false);
  connect(aktiv1, SIGNAL(clicked()), this, SLOT(saktiv1()));
  
  aktiv2 = new QCheckBox(tr("Active",""), this);
  aktiv2->setGeometry(240,165,60,25);
  aktiv2->setEnabled(false);
  connect(aktiv2, SIGNAL(clicked()), SLOT(saktiv2()));
  
  aktiv3 = new QCheckBox(tr("Active",""), this);
  aktiv3->setGeometry(300,165,60,25);
  aktiv3->setEnabled(false);
  connect(aktiv3, SIGNAL(clicked()), SLOT(saktiv3()));
  
  aktiv4 = new QCheckBox(tr("Active",""), this);
  aktiv4->setGeometry(360,165,60,25);
  aktiv4->setEnabled(false);
  connect(aktiv4, SIGNAL(clicked()), SLOT(saktiv4()));

  aktiv5 = new QCheckBox(tr("Active",""), this);
  aktiv5->setGeometry(420,165,60,25);
  aktiv5->setEnabled(false);
  connect(aktiv5, SIGNAL(clicked()), SLOT(saktiv5()));

  aktiv6 = new QCheckBox(tr("Active",""), this);
  aktiv6->setGeometry(480,165,60,25);
  aktiv6->setEnabled(false);
  connect(aktiv6, SIGNAL(clicked()), SLOT(saktiv6()));
  
  labelkanal1 = new QLabel(tr("Graph 1 empty",""),this);
  labelkanal1->setGeometry(10, 210, 700, 20);
  labelkanal2 = new QLabel(tr("Graph 2 empty",""),this);
  labelkanal2->setGeometry(10, 230, 700, 20);
  labelkanal3 = new QLabel(tr("Graph 3 empty",""),this);
  labelkanal3->setGeometry(10, 250, 700, 20);
  labelkanal4 = new QLabel(tr("Graph 4 empty",""),this);
  labelkanal4->setGeometry(10, 270, 700, 20);
  labelkanal5 = new QLabel(tr("Graph 5 empty",""),this);
  labelkanal5->setGeometry(10, 290, 700, 20);
  labelkanal6 = new QLabel(tr("Graph 6 empty",""),this);
  labelkanal6->setGeometry(10, 310, 700, 20);
//  inherited widget (parent, name, modal);
};

MarkersDlg::~MarkersDlg()
{
}

void MarkersDlg::psetfontsize(int s)
{
  QFont font("Helvetica", s);

  labelk1->setFont(font);
  labelk4->setFont(font);
  labelk2->setFont(font);
  labelk3->setFont(font);
  labelk5->setFont(font);
  labelk6->setFont(font);

  labelopen->setFont(font);
  labelsave->setFont(font);
  labeluebernahme->setFont(font);
  labelcolor->setFont(font);
  labelkanal->setFont(font);
  labeleinblendung->setFont(font);

  open1->setFont(font);
  open2->setFont(font);
  open3->setFont(font);
  open4->setFont(font);
  open5->setFont(font);
  open6->setFont(font);

  save1->setFont(font);
  save2->setFont(font);
  save3->setFont(font);
  save4->setFont(font);
  save5->setFont(font);
  save6->setFont(font);

  uebernahme1->setFont(font);
  uebernahme2->setFont(font);
  uebernahme3->setFont(font);
  uebernahme4->setFont(font);
  uebernahme5->setFont(font);
  uebernahme6->setFont(font);

  color11->setFont(font);
  color12->setFont(font);
  color21->setFont(font);
  color22->setFont(font);
  color31->setFont(font);
  color32->setFont(font);
  color41->setFont(font);
  color42->setFont(font);
  color51->setFont(font);
  color52->setFont(font);
  color61->setFont(font);
  color62->setFont(font);

  checkboxk11->setFont(font);
  checkboxk12->setFont(font);
  checkboxk21->setFont(font);
  checkboxk22->setFont(font);
  checkboxk31->setFont(font);
  checkboxk32->setFont(font);
  checkboxk41->setFont(font);
  checkboxk42->setFont(font);
  checkboxk51->setFont(font);
  checkboxk52->setFont(font);
  checkboxk61->setFont(font);
  checkboxk62->setFont(font);

  aktiv1->setFont(font);
  aktiv2->setFont(font);
  aktiv3->setFont(font);
  aktiv4->setFont(font);
  aktiv5->setFont(font);
  aktiv6->setFont(font);

  labelkanal1->setFont(font);
  labelkanal2->setFont(font);
  labelkanal3->setFont(font);
  labelkanal4->setFont(font);
  labelkanal5->setFont(font);
  labelkanal6->setFont(font);
}


void MarkersDlg::tip(bool btip)
{
QString tip_open = tr("<br><b>Loading a Graph-file :</b><br><br> Loading a Saved graph.<br><br>With the button \"Active\" you can <br> put it  in the background.<br>","");
QString tip_holen = tr("<br><b>Get a Graph-file :</b> <br><br> Getting the Graph from the Graphical Display. <br><br>With the \"active\" button you can put it in the background <br>","");
QString tip_wkmcolor = tr("<br><b>Setting  the Color :</b> <br> <br>This allows to set the Color of each channel individually. <br>The values are saved in the file \"wkm.cfg\"<br> automatically.<br>","");
QString tip_save = tr(" <br><b>Saving the Graph :</b> <br><br> This allows to save the Graph after sweeping.<br><br>The operation is the same as clicking \"Save Graph\" <br> in the Menu \"Graph\" in Sweepmode.<br> The Graph can be Loaded by clicking \" Load Graph\"<br> in the same menue.<br>","");
 
  if(btip){
    open1->setToolTip(tip_open);
    open2->setToolTip(tip_open);
    open3->setToolTip(tip_open);
    open4->setToolTip(tip_open);
    open5->setToolTip(tip_open);
    open6->setToolTip(tip_open);

    uebernahme1->setToolTip(tip_holen);
    uebernahme2->setToolTip(tip_holen);
    uebernahme3->setToolTip(tip_holen);
    uebernahme4->setToolTip(tip_holen);
    uebernahme5->setToolTip(tip_holen);
    uebernahme6->setToolTip(tip_holen);

    color11->setToolTip(tip_wkmcolor);
    color12->setToolTip(tip_wkmcolor);
    color21->setToolTip(tip_wkmcolor);
    color22->setToolTip(tip_wkmcolor);
    color31->setToolTip(tip_wkmcolor);
    color32->setToolTip(tip_wkmcolor);
    color41->setToolTip(tip_wkmcolor);
    color42->setToolTip(tip_wkmcolor);
    color51->setToolTip(tip_wkmcolor);
    color52->setToolTip(tip_wkmcolor);
    color61->setToolTip(tip_wkmcolor);
    color62->setToolTip(tip_wkmcolor);

    save1->setToolTip(tip_save);
    save2->setToolTip(tip_save);
    save3->setToolTip(tip_save);
    save4->setToolTip(tip_save);
    save5->setToolTip(tip_save);
    save6->setToolTip(tip_save);

  }else{
    open1->setToolTip("");
    open2->setToolTip("");
    open3->setToolTip("");
    open4->setToolTip("");
    open5->setToolTip("");
    open6->setToolTip("");

    uebernahme1->setToolTip("");
    uebernahme2->setToolTip("");
    uebernahme3->setToolTip("");
    uebernahme4->setToolTip("");
    uebernahme5->setToolTip("");
    uebernahme6->setToolTip("");

    color11->setToolTip("");
    color12->setToolTip("");
    color21->setToolTip("");
    color22->setToolTip("");
    color31->setToolTip("");
    color32->setToolTip("");
    color41->setToolTip("");
    color42->setToolTip("");
    color51->setToolTip("");
    color52->setToolTip("");
    color61->setToolTip("");
    color62->setToolTip("");

    save1->setToolTip("");
    save2->setToolTip("");
    save3->setToolTip("");
    save4->setToolTip("");
    save5->setToolTip("");
    save6->setToolTip("");

  }
}

void MarkersDlg::beenden()
{
  QColor pencolor;
  int r,g,b,a;
  
  ConfigFile *nwtconfigfile = new ConfigFile();
  nwtconfigfile->open(homedir.filePath("wkm.cfg"));

  pencolor = wobbeldaten1.penkanal1.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm11r", r);
  nwtconfigfile->writeInteger("wkm11g", g);
  nwtconfigfile->writeInteger("wkm11b", b);
  nwtconfigfile->writeInteger("wkm11a", a);
  pencolor = wobbeldaten1.penkanal2.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm12r", r);
  nwtconfigfile->writeInteger("wkm12g", g);
  nwtconfigfile->writeInteger("wkm12b", b);
  nwtconfigfile->writeInteger("wkm12a", a);
  
  pencolor = wobbeldaten2.penkanal1.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm21r", r);
  nwtconfigfile->writeInteger("wkm21g", g);
  nwtconfigfile->writeInteger("wkm21b", b);
  nwtconfigfile->writeInteger("wkm21a", a);
  pencolor = wobbeldaten2.penkanal2.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm22r", r);
  nwtconfigfile->writeInteger("wkm22g", g);
  nwtconfigfile->writeInteger("wkm22b", b);
  nwtconfigfile->writeInteger("wkm22a", a);
  
  pencolor = wobbeldaten3.penkanal1.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm31r", r);
  nwtconfigfile->writeInteger("wkm31g", g);
  nwtconfigfile->writeInteger("wkm31b", b);
  nwtconfigfile->writeInteger("wkm31a", a);
  pencolor = wobbeldaten3.penkanal2.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm32r", r);
  nwtconfigfile->writeInteger("wkm32g", g);
  nwtconfigfile->writeInteger("wkm32b", b);
  nwtconfigfile->writeInteger("wkm32a", a);
  
  pencolor = wobbeldaten4.penkanal1.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm41r", r);
  nwtconfigfile->writeInteger("wkm41g", g);
  nwtconfigfile->writeInteger("wkm41b", b);
  nwtconfigfile->writeInteger("wkm41a", a);
  pencolor = wobbeldaten4.penkanal2.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm42r", r);
  nwtconfigfile->writeInteger("wkm42g", g);
  nwtconfigfile->writeInteger("wkm42b", b);
  nwtconfigfile->writeInteger("wkm42a", a);

  pencolor = wobbeldaten5.penkanal1.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm51r", r);
  nwtconfigfile->writeInteger("wkm51g", g);
  nwtconfigfile->writeInteger("wkm51b", b);
  nwtconfigfile->writeInteger("wkm51a", a);
  pencolor = wobbeldaten5.penkanal2.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm52r", r);
  nwtconfigfile->writeInteger("wkm52g", g);
  nwtconfigfile->writeInteger("wkm52b", b);
  nwtconfigfile->writeInteger("wkm2a", a);

  pencolor = wobbeldaten6.penkanal1.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm61r", r);
  nwtconfigfile->writeInteger("wkm61g", g);
  nwtconfigfile->writeInteger("wkm61b", b);
  nwtconfigfile->writeInteger("wkm61a", a);
  pencolor = wobbeldaten6.penkanal2.color();
  pencolor.getRgb(&r, &g, &b, &a);
  nwtconfigfile->writeInteger("wkm62r", r);
  nwtconfigfile->writeInteger("wkm62g", g);
  nwtconfigfile->writeInteger("wkm62b", b);
  nwtconfigfile->writeInteger("wkm62a", a);

  nwtconfigfile->close();
}

void MarkersDlg::save(int nr)
{
  QFile f;
  TSweep w;
  
  switch(nr){
    case 1:
      w = wobbeldaten1;
      break;
    case 2:
      w = wobbeldaten2;
      break;
    case 3:
      w = wobbeldaten3;
      break;
    case 4:
      w = wobbeldaten4;
      break;
    case 5:
      w = wobbeldaten5;
      break;
    case 6:
      w = wobbeldaten6;
      break;

  }
  emit savekurve(w);
}

void MarkersDlg::sopen1()
{
  emit loadkurve(1);
  checkboxk11->setChecked(false);
  checkboxk12->setChecked(false);
  aktiv1->setChecked(false);
};

void MarkersDlg::sopen2()
{
  emit loadkurve(2);
  checkboxk21->setChecked(wobbeldaten2.bkanal1);
  checkboxk22->setChecked(wobbeldaten2.bkanal2);
};

void MarkersDlg::sopen3()
{
  emit loadkurve(3);
  checkboxk31->setChecked(wobbeldaten3.bkanal1);
  checkboxk32->setChecked(wobbeldaten3.bkanal2);
};

void MarkersDlg::sopen4()
{
  emit loadkurve(4);
  checkboxk41->setChecked(wobbeldaten4.bkanal1);
  checkboxk42->setChecked(wobbeldaten4.bkanal2);
};

void MarkersDlg::sopen5()
{
  emit loadkurve(5);
  checkboxk51->setChecked(wobbeldaten5.bkanal1);
  checkboxk52->setChecked(wobbeldaten5.bkanal2);
};

void MarkersDlg::sopen6()
{
  emit loadkurve(6);
  checkboxk61->setChecked(wobbeldaten6.bkanal1);
  checkboxk62->setChecked(wobbeldaten6.bkanal2);
};

void MarkersDlg::ssave1()
{
  save(1);
};

void MarkersDlg::ssave2()
{
  save(2);
};

void MarkersDlg::ssave3()
{
  save(3);
};

void MarkersDlg::ssave4()
{
  save(4);
};

void MarkersDlg::ssave5()
{
  save(5);
};

void MarkersDlg::ssave6()
{
  save(6);
};

void MarkersDlg::sholen1()
{
  emit wkmgetwobbel(1);
};

void MarkersDlg::sholen2()
{
  emit wkmgetwobbel(2);
};

void MarkersDlg::sholen3()
{
  emit wkmgetwobbel(3);
};

void MarkersDlg::sholen4()
{
  emit wkmgetwobbel(4);
};

void MarkersDlg::sholen5()
{
  emit wkmgetwobbel(5);
};

void MarkersDlg::sholen6()
{
  emit wkmgetwobbel(6);
};


void MarkersDlg::scolor11()
{
  bool ok;
  QColor color = wobbeldaten1.penkanal1.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor11");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten1.penkanal1.setColor(color);
    QPalette palette = color11->palette();
    palette.setColor(QPalette::ButtonText, color);
    color11->setPalette(palette);
  }
  saktiv1();
};

void MarkersDlg::scolor12()
{
  bool ok;
  QColor color = wobbeldaten1.penkanal2.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor12");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten1.penkanal2.setColor(color);
    QPalette palette = color12->palette();
    palette.setColor(QPalette::ButtonText, color);
    color12->setPalette(palette);
  }
  saktiv1();
};

void MarkersDlg::scolor21()
{
  bool ok;
  QColor color = wobbeldaten2.penkanal1.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor21");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten2.penkanal1.setColor(color);
    QPalette palette = color21->palette();
    palette.setColor(QPalette::ButtonText, color);
    color21->setPalette(palette);
  }
};

void MarkersDlg::scolor22()
{
  bool ok;
  QColor color = wobbeldaten2.penkanal2.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor22");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten2.penkanal2.setColor(color);
    QPalette palette = color22->palette();
    palette.setColor(QPalette::ButtonText, color);
    color22->setPalette(palette);
  }
};

void MarkersDlg::scolor31()
{
  bool ok;
  QColor color = wobbeldaten3.penkanal1.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor31");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten3.penkanal1.setColor(color);
    QPalette palette = color31->palette();
    palette.setColor(QPalette::ButtonText, color);
    color31->setPalette(palette);
  }
};

void MarkersDlg::scolor32()
{
  bool ok;
  QColor color = wobbeldaten3.penkanal2.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor32");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten3.penkanal2.setColor(color);
    QPalette palette = color32->palette();
    palette.setColor(QPalette::ButtonText, color);
    color32->setPalette(palette);
  }
};

void MarkersDlg::scolor41()
{
  bool ok;
  QColor color = wobbeldaten4.penkanal1.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor41");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten4.penkanal1.setColor(color);
    QPalette palette = color41->palette();
    palette.setColor(QPalette::ButtonText, color);
    color41->setPalette(palette);
  }
};

void MarkersDlg::scolor42()
{
  bool ok;
  QColor color = wobbeldaten4.penkanal2.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor42");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten4.penkanal2.setColor(color);
    QPalette palette = color42->palette();
    palette.setColor(QPalette::ButtonText, color);
    color42->setPalette(palette);
  }
};

void MarkersDlg::scolor51()
{
  bool ok;
  QColor color = wobbeldaten5.penkanal1.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor51");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten5.penkanal1.setColor(color);
    QPalette palette = color51->palette();
    palette.setColor(QPalette::ButtonText, color);
    color51->setPalette(palette);
  }
};

void MarkersDlg::scolor52()
{
  bool ok;
  QColor color = wobbeldaten5.penkanal2.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor52");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten5.penkanal2.setColor(color);
    QPalette palette = color52->palette();
    palette.setColor(QPalette::ButtonText, color);
    color52->setPalette(palette);
  }
};

void MarkersDlg::scolor61()
{
  bool ok;
  QColor color = wobbeldaten6.penkanal1.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor61");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten6.penkanal1.setColor(color);
    QPalette palette = color61->palette();
    palette.setColor(QPalette::ButtonText, color);
    color61->setPalette(palette);
  }
};

void MarkersDlg::scolor62()
{
  bool ok;
  QColor color = wobbeldaten6.penkanal2.color();
  QRgb rgb;
  rgb = color.rgb();
  //qDebug("scolor62");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    wobbeldaten6.penkanal2.setColor(color);
    QPalette palette = color62->palette();
    palette.setColor(QPalette::ButtonText, color);
    color62->setPalette(palette);
  }
};

void MarkersDlg::saktiv1()
{
  if(aktiv1->isChecked()){
    wobbeldaten1.mess.daten_enable = true;
  }else{
    wobbeldaten1.mess.daten_enable = false;
  }
  //emit warneichkorr();
  emit wkmsendwobbel(wobbeldaten1);
};

void MarkersDlg::saktiv2()
{
  if(aktiv2->isChecked()){
    wobbeldaten2.mess.daten_enable = true;
  }else{
    wobbeldaten2.mess.daten_enable = false;
  }
  //emit warneichkorr();
  emit wkmsendwobbel(wobbeldaten2);
};

void MarkersDlg::saktiv3()
{
  if(aktiv3->isChecked()){
    wobbeldaten3.mess.daten_enable = true;
  }else{
    wobbeldaten3.mess.daten_enable = false;
  }
  //emit warneichkorr();
  emit wkmsendwobbel(wobbeldaten3);
};

void MarkersDlg::saktiv4()
{
  if(aktiv4->isChecked()){
    wobbeldaten4.mess.daten_enable = true;
  }else{
    wobbeldaten4.mess.daten_enable = false;
  }
  //emit warneichkorr();
  emit wkmsendwobbel(wobbeldaten4);
};

void MarkersDlg::saktiv5()
{
  if(aktiv5->isChecked()){
    wobbeldaten5.mess.daten_enable = true;
  }else{
    wobbeldaten5.mess.daten_enable = false;
  }
  //emit warneichkorr();
  emit wkmsendwobbel(wobbeldaten5);
};

void MarkersDlg::saktiv6()
{
  if(aktiv6->isChecked()){
    wobbeldaten6.mess.daten_enable = true;
  }else{
    wobbeldaten6.mess.daten_enable = false;
  }
  //emit warneichkorr();
  emit wkmsendwobbel(wobbeldaten6);
};

void MarkersDlg::suebergabe(const TSweep &awobbel, const QString &dateiname)
{
  QPen pen1, pen2;
  QString s, sz;
  QDateTime zeit(QDateTime::currentDateTime());
  wobbeldaten = awobbel;
  //wobbeldaten.dbshift1 = 0.0;
  //wobbeldaten.dbshift2 = 0.0;
  //keine Eichkorrektur
  //wobbeldaten.beichkorrk1 = false;
  //wobbeldaten.beichkorrk2 = false;
  //===================
  wobbeldaten.mess.daten_enable = true;
  wobbeldaten.bhintergrund = true;
  //  wobbeldaten.bhintergrund = false;
  //  Beschreibung
  if(dateiname.isEmpty()){
    sz = zeit.toString("dd.MM.yyyy hh:mm:ss");
  }else{
    sz = dateiname;
  }
  switch(wobbeldaten.kurvennr){
    case 1:
      pen1 = wobbeldaten1.penkanal1;
      pen2 = wobbeldaten1.penkanal2;
      wobbeldaten1 = wobbeldaten;
      wobbeldaten1.penkanal1 = pen1;
      wobbeldaten1.penkanal2 = pen2;
      s = tr("Graph 1: ") + sz;
      labelkanal1->setText(s);
      checkboxk11->setEnabled(true);
      checkboxk12->setEnabled(true);
      aktiv1->setEnabled(true);
      save1->setEnabled(true);
      checkboxk11->setChecked(wobbeldaten.bkanal1);
      checkboxk12->setChecked(wobbeldaten.bkanal2);
      saktiv1();
    break;
    case 2:
      pen1 = wobbeldaten2.penkanal1;
      pen2 = wobbeldaten2.penkanal2;
      wobbeldaten2 = wobbeldaten;
      wobbeldaten2.penkanal1 = pen1;
      wobbeldaten2.penkanal2 = pen2;
      s = tr("Graph 2: ") + sz;
      labelkanal2->setText(s);
      checkboxk21->setEnabled(true);
      checkboxk22->setEnabled(true);
      aktiv2->setEnabled(true);
      save2->setEnabled(true);
      checkboxk21->setChecked(wobbeldaten.bkanal1);
      checkboxk22->setChecked(wobbeldaten.bkanal2);
      saktiv2();
    break;
    case 3:
      pen1 = wobbeldaten3.penkanal1;
      pen2 = wobbeldaten3.penkanal2;
      wobbeldaten3 = wobbeldaten;
      wobbeldaten3.penkanal1 = pen1;
      wobbeldaten3.penkanal2 = pen2;
      s = tr("Graph 3: ") + sz;
      labelkanal3->setText(s);
      checkboxk31->setEnabled(true);
      checkboxk32->setEnabled(true);
      aktiv3->setEnabled(true);
      save3->setEnabled(true);
      checkboxk31->setChecked(wobbeldaten.bkanal1);
      checkboxk32->setChecked(wobbeldaten.bkanal2);
      saktiv3();
    break;
    case 4:
      pen1 = wobbeldaten4.penkanal1;
      pen2 = wobbeldaten4.penkanal2;
      wobbeldaten4 = wobbeldaten;
      wobbeldaten4.penkanal1 = pen1;
      wobbeldaten4.penkanal2 = pen2;
      s = tr("Graph 4: ") + sz;
      labelkanal4->setText(s);
      checkboxk41->setEnabled(true);
      checkboxk42->setEnabled(true);
      aktiv4->setEnabled(true);
      save4->setEnabled(true);
      checkboxk41->setChecked(wobbeldaten.bkanal1);
      checkboxk42->setChecked(wobbeldaten.bkanal2);
      saktiv4();
    break;
    case 5:
     pen1 = wobbeldaten5.penkanal1;
     pen2 = wobbeldaten5.penkanal2;
      wobbeldaten5 = wobbeldaten;
      wobbeldaten5.penkanal1 = pen1;
      wobbeldaten5.penkanal2 = pen2;
      s = tr("Graph 5: ") + sz;
      labelkanal5->setText(s);
      checkboxk51->setEnabled(true);
      checkboxk52->setEnabled(true);
      aktiv5->setEnabled(true);
      save5->setEnabled(true);
      checkboxk51->setChecked(wobbeldaten.bkanal1);
      checkboxk52->setChecked(wobbeldaten.bkanal2);
      saktiv5();
    break;
    case 6:
      pen1 = wobbeldaten6.penkanal1;
      pen2 = wobbeldaten6.penkanal2;
      wobbeldaten6 = wobbeldaten;
      wobbeldaten6.penkanal1 = pen1;
      wobbeldaten6.penkanal2 = pen2;
      s = tr("Graph 6: ") + sz;
      labelkanal6->setText(s);
      checkboxk61->setEnabled(true);
      checkboxk62->setEnabled(true);
      aktiv6->setEnabled(true);
      save6->setEnabled(true);
      checkboxk61->setChecked(wobbeldaten.bkanal1);
      checkboxk62->setChecked(wobbeldaten.bkanal2);
      saktiv6();
    break;
  }
};

void MarkersDlg::saktivk11()
{
  wobbeldaten1.bkanal1 = checkboxk11->isChecked();
  saktiv1();
}

void MarkersDlg::saktivk12()
{
  wobbeldaten1.bkanal2 = checkboxk12->isChecked();
  saktiv1();
}

void MarkersDlg::saktivk21()
{
  wobbeldaten2.bkanal1 = checkboxk21->isChecked();
  saktiv2();
}

void MarkersDlg::saktivk22()
{
  wobbeldaten2.bkanal2 = checkboxk22->isChecked();
  saktiv2();
}

void MarkersDlg::saktivk31()
{
  wobbeldaten3.bkanal1 = checkboxk31->isChecked();
  saktiv3();
}

void MarkersDlg::saktivk32()
{
  wobbeldaten3.bkanal2 = checkboxk32->isChecked();
  saktiv3();
}

void MarkersDlg::saktivk41()
{
  wobbeldaten4.bkanal1 = checkboxk41->isChecked();
  saktiv4();
}

void MarkersDlg::saktivk42()
{
  wobbeldaten4.bkanal2 = checkboxk42->isChecked();
  saktiv4();
}

void MarkersDlg::saktivk51()
{
  wobbeldaten5.bkanal1 = checkboxk51->isChecked();
  saktiv5();
}

void MarkersDlg::saktivk52()
{
  wobbeldaten5.bkanal2 = checkboxk52->isChecked();
  saktiv5();
}

void MarkersDlg::saktivk61()
{
  wobbeldaten6.bkanal1 = checkboxk61->isChecked();
  saktiv6();
}

void MarkersDlg::saktivk62()
{
  wobbeldaten6.bkanal2 = checkboxk62->isChecked();
  saktiv6();
}

/*
void owkmanager::setkdir(QString s)
{
  kurvendir.setPath(s);
}
*/
void MarkersDlg::setHomePath(QDir &dir)
{
  homedir = dir;
}

void MarkersDlg::loadConfig()
{
  ConfigFile nwtconfigfile;
  QColor pencolor;
  int r,g,b,a;
  QPalette palette;
  
  nwtconfigfile.open(homedir.filePath("wkm.cfg"));
  
  r = nwtconfigfile.readInteger("wkm11r", 185);
  g = nwtconfigfile.readInteger("wkm11g", 1);
  b = nwtconfigfile.readInteger("wkm11b", 185);
  a = nwtconfigfile.readInteger("wkm11a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten1.penkanal1.setColor(pencolor);
  palette = color11->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color11->setPalette(palette);
  r = nwtconfigfile.readInteger("wkm12r", 255);
  g = nwtconfigfile.readInteger("wkm12g", 2);
  b = nwtconfigfile.readInteger("wkm12b", 255);
  a = nwtconfigfile.readInteger("wkm12a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten1.penkanal2.setColor(pencolor);
  palette = color12->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color12->setPalette(palette);
  
  r = nwtconfigfile.readInteger("wkm21r", 255);
  g = nwtconfigfile.readInteger("wkm21g", 170);
  b = nwtconfigfile.readInteger("wkm21b", 0);
  a = nwtconfigfile.readInteger("wkm21a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten2.penkanal1.setColor(pencolor);
  palette = color21->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color21->setPalette(palette);
  r = nwtconfigfile.readInteger("wkm22r", 188);
  g = nwtconfigfile.readInteger("wkm22g", 125);
  b = nwtconfigfile.readInteger("wkm22b", 0);
  a = nwtconfigfile.readInteger("wkm22a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten2.penkanal2.setColor(pencolor);
  palette = color22->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color22->setPalette(palette);
  
  r = nwtconfigfile.readInteger("wkm31r", 128);
  g = nwtconfigfile.readInteger("wkm31g", 255);
  b = nwtconfigfile.readInteger("wkm31b", 0);
  a = nwtconfigfile.readInteger("wkm31a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten3.penkanal1.setColor(pencolor);
  palette = color31->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color31->setPalette(palette);
  r = nwtconfigfile.readInteger("wkm32r", 88);
  g = nwtconfigfile.readInteger("wkm32g", 177);
  b = nwtconfigfile.readInteger("wkm32b", 0);
  a = nwtconfigfile.readInteger("wkm32a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten3.penkanal2.setColor(pencolor);
  palette = color32->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color32->setPalette(palette);
  
  r = nwtconfigfile.readInteger("wkm41r", 0);
  g = nwtconfigfile.readInteger("wkm41g", 170);
  b = nwtconfigfile.readInteger("wkm41b", 255);
  a = nwtconfigfile.readInteger("wkm41a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten4.penkanal1.setColor(pencolor);
  palette = color41->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color41->setPalette(palette);
  r = nwtconfigfile.readInteger("wkm42r", 0);
  g = nwtconfigfile.readInteger("wkm42g", 99);
  b = nwtconfigfile.readInteger("wkm42b", 148);
  a = nwtconfigfile.readInteger("wkm42a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten4.penkanal2.setColor(pencolor);
  palette = color42->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color42->setPalette(palette);

  r = nwtconfigfile.readInteger("wkm51r", 128);
  g = nwtconfigfile.readInteger("wkm51g", 255);
  b = nwtconfigfile.readInteger("wkm51b", 0);
  a = nwtconfigfile.readInteger("wkm51a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten5.penkanal1.setColor(pencolor);
  palette = color51->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color51->setPalette(palette);
  r = nwtconfigfile.readInteger("wkm52r", 88);
  g = nwtconfigfile.readInteger("wkm52g", 177);
  b = nwtconfigfile.readInteger("wkm52b", 0);
  a = nwtconfigfile.readInteger("wkm52a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten5.penkanal2.setColor(pencolor);
  palette = color52->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color52->setPalette(palette);

  r = nwtconfigfile.readInteger("wkm61r", 0);
  g = nwtconfigfile.readInteger("wkm61g", 170);
  b = nwtconfigfile.readInteger("wkm61b", 255);
  a = nwtconfigfile.readInteger("wkm61a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten6.penkanal1.setColor(pencolor);
  palette = color61->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color61->setPalette(palette);
  r = nwtconfigfile.readInteger("wkm62r", 0);
  g = nwtconfigfile.readInteger("wkm62g", 99);
  b = nwtconfigfile.readInteger("wkm62b", 148);
  a = nwtconfigfile.readInteger("wkm62a", 255);
  pencolor.setRgb(r,g,b,a);
  wobbeldaten6.penkanal2.setColor(pencolor);
  palette = color62->palette();
  palette.setColor(QPalette::ButtonText, pencolor);
  color62->setPalette(palette);

  nwtconfigfile.close();
}

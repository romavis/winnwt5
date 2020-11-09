/***************************************************************************

    optiondlg.cpp  -  description
    ----------------------------------------------------
    begin                : 2005
    copyright            : (C) 2005 by Andreas Lindenau
    email                : DL4JAL@darc.de

    optiondlg.cpp  have been refactored to optiondlg.cpp QT5
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

#include <Qt>
#include <QDialog>
#include <QColorDialog>
#include <QColor>
#include <QPainter>
#include <QFont>
#include <QFileDialog>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <math.h>

#include "optiondlg.h"
#include "sweepcurvewidget.h"

#include "modem.hpp"


//#####################################################################################
// Class fuer Option
//#####################################################################################
OptionDlg::OptionDlg(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
  
  this->resize(620,520);
  this->setMinimumSize(0,0);
//  QFont font("Helvetica", pfontsize);

  ok = new QPushButton(tr("OK",""), this);
  ok->setGeometry(10,10,100,30);
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  cancel = new QPushButton(tr("Cancel",""), this);
  cancel->setGeometry(10,50,100,30);
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));
  
  tabwidget = new QTabWidget(this);
  tabwidget->setGeometry(120,0,500,510);
  wwobbel = new QWidget();
  tabwidget->addTab(wwobbel, tr("General data / Sweep",""));
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  kalibrierfrequ = new QGroupBox(wwobbel);
  kalibrierfrequ->setGeometry(10,10,270,90);
  kalibrierfrequ->setTitle(tr("Calibration frequency (Math.-correction only)",""));
  labelanfang = new QLabel(kalibrierfrequ);
  labelanfang->setGeometry(10,25,120,30);
  labelanfang->setText(tr("Startfrequency (Hz)",""));
  labelanfang->setAlignment(Qt::AlignRight);
  labelende = new QLabel(kalibrierfrequ);
  labelende->setGeometry(10,55,120,30);
  labelende->setText(tr("Stopfrequency (Hz)",""));
  labelende->setAlignment(Qt::AlignRight);
  eanfang = new QLineEdit(kalibrierfrequ);
  eanfang->setGeometry(140,20,110,25);
  eende = new QLineEdit(kalibrierfrequ);
  eende->setGeometry(140,50,110,25);
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  ddstakt = new QGroupBox(wwobbel);
  ddstakt->setGeometry(10,105,270,85);
  ddstakt->setTitle(tr("DDS Clockfrequency",""));
  labelddstakt = new QLabel(ddstakt);
  labelddstakt->setGeometry(10,25,100,30);
  labelddstakt->setText(tr("DDS Clock (HZ)",""));
  labelddstakt->setAlignment(Qt::AlignRight);
  eddstakt = new QLineEdit(ddstakt);
  eddstakt->setGeometry(130,20,120,25);
  labelcpll = new QLabel(ddstakt);
  labelcpll->setGeometry(10,55,100,30);
  labelcpll->setText(tr("DDS Mode"));
  labelcpll->setAlignment(Qt::AlignRight);
  cpll = new QComboBox(ddstakt);
  cpll->setGeometry(130,50,120,25);
  cpll->addItem("");
  cpll->addItem(tr("No PLL"));
  cpll->addItem("PLL 2x");
  cpll->addItem("PLL 3x");
  cpll->addItem("PLL 4x");
  cpll->addItem("PLL 5x");
  cpll->addItem("PLL 6x");
  cpll->addItem("PLL 7x");
  cpll->addItem("PLL 8x");
  cpll->addItem("PLL 9x");
  cpll->addItem("PLL 10x");
  cpll->addItem("PLL 11x");
  cpll->addItem("PLL 12x");
  cpll->addItem("PLL 13x");
  cpll->addItem("PLL 14x");
  cpll->addItem("PLL 15x");
  cpll->addItem("PLL 16x");
  cpll->addItem("PLL 17x");
  cpll->addItem("PLL 18x");
  cpll->addItem("PLL 19x");
  cpll->addItem("PLL 20x");
  labelcpll->setEnabled(false);
  cpll->setEnabled(false);
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  gtty = new QGroupBox(wwobbel);
  gtty->setGeometry(10,195,270,60);
  gtty->setTitle(tr("Serial interface",""));
  labelctty = new QLabel(gtty);
  labelctty->setGeometry(10,20,120,30);
  labelctty->setText(tr("Interface :",""));
  ctty = new QComboBox(gtty);
  ctty->setGeometry(130,20,120,25);
  ctty->addItem("");
#ifdef Q_OS_WIN
  int i;
  QString qs;
  for(i=1;i<=256;i++){
    qs.sprintf("COM%i",i);
    ctty->addItem(qs);
  }
#else
  ctty->setEditable(true);
  ctty->addItem(schnittstelle1);
  ctty->addItem(schnittstelle2);
  ctty->addItem(schnittstelle3);
  ctty->addItem(schnittstelle4);
  ctty->addItem(schnittstelle5);
  ctty->addItem(schnittstelle6);
  ctty->addItem(schnittstelle7);
  ctty->addItem(schnittstelle8);
#endif
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  fname = new QGroupBox(wwobbel);
  fname->setGeometry(10,260,270,115);
  fname->setTitle(tr("Default Filename",""));
  labelsonde1 = new QLabel(fname);
  labelsonde1->setGeometry(10,25,120,30);
  labelsonde1->setText(tr("Channel 1 (Log)",""));
  labelsonde1->setAlignment(Qt::AlignRight);
  esonde1 = new QLineEdit(fname);
  esonde1->setGeometry(135,20,120,25);
  labelesonde1lin = new QLabel(fname);
  labelesonde1lin->setGeometry(10,55,120,30);
  labelesonde1lin->setText(tr("Channel 1 (Lin)",""));
  labelesonde1lin->setAlignment(Qt::AlignRight);
  esonde1lin = new QLineEdit(fname);
  esonde1lin->setGeometry(135,50,120,25);
  labelsonde2 = new QLabel(fname);
  labelsonde2->setGeometry(10,85,120,30);
  labelsonde2->setText(tr("Channel 2 (Log)",""));
  labelsonde2->setAlignment(Qt::AlignRight);
  esonde2 = new QLineEdit(fname);
  esonde2->setGeometry(135,80,120,25);
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  fgrenzen = new QGroupBox(wwobbel);
  fgrenzen->setGeometry(10,380,270,85);
  fgrenzen->setTitle(tr("Frequency limits",""));
  labelwobmax = new QLabel(fgrenzen);
  labelwobmax->setGeometry(10,25,120,30);
  labelwobmax->setText(tr("max. Sweep (Hz)",""));
  labelwobmax->setAlignment(Qt::AlignRight);
  wobmax = new QLineEdit(fgrenzen);
  wobmax->setGeometry(135,20,120,25);
  cbfrqfaktor = new QComboBox(fgrenzen);
  cbfrqfaktor->addItem("1");
  cbfrqfaktor->addItem("2");
  cbfrqfaktor->addItem("3");
  cbfrqfaktor->addItem("4");
  cbfrqfaktor->addItem("5");
  cbfrqfaktor->addItem("6");
  cbfrqfaktor->addItem("7");
  cbfrqfaktor->addItem("8");
  cbfrqfaktor->addItem("9");
  cbfrqfaktor->addItem("10");
  cbfrqfaktor->addItem("16");
  cbfrqfaktor->addItem("32");
  cbfrqfaktor->addItem("64");
  labelfrqfaktor = new QLabel(tr("Frequency mulitiply",""),fgrenzen);
  cbfrqfaktor->setGeometry(215, 50, 40, 20);
  labelfrqfaktor->setGeometry(35, 50, 160, 20);
  labelfrqfaktor->setAlignment(Qt::AlignRight);

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  gkalibrier = new QGroupBox(wwobbel);
  gkalibrier->setGeometry(290,10,190,150);
  gkalibrier->setTitle(tr("Attenuator",""));

  rbAttenStandard = new QRadioButton(gkalibrier);
  rbAttenStandard->setGeometry(10,20,150,25);
  rbAttenStandard->setText(tr("Standard (0-50dB)",""));

  rbAttenFa = new QRadioButton(gkalibrier);
  rbAttenFa->setGeometry(10,50,150,25);
  rbAttenFa->setText(tr("FA (0-66dB)",""));

  rbAttenCustom = new QRadioButton(gkalibrier);
  rbAttenCustom->setGeometry(10,80,150,25);
  rbAttenCustom->setText(tr("Custom:",""));
  connect(rbAttenCustom, SIGNAL(toggled(bool)), this, SLOT(rbAttenCustom_toggled(bool)));

  edtAttenConfig = new QLineEdit(gkalibrier);
  edtAttenConfig->setGeometry(30,110,150,25);
  connect(edtAttenConfig, SIGNAL(textChanged(const QString&)), this, SLOT(edtAttenConfig_textChanged(const QString&)));

  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  geinkanalig = new QGroupBox(wwobbel);
  geinkanalig->setGeometry(290,190,190,60);
  geinkanalig->setTitle(tr("Channels",""));
  boxeinkanalig = new QCheckBox(geinkanalig);
  boxeinkanalig->setGeometry(10,20,150,30);
  boxeinkanalig->setText(tr("One channel",""));
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  gswriteration = new QGroupBox(wwobbel);
  gswriteration->setGeometry(290,255,190,60);
  gswriteration->setTitle(tr("SWR Iteration",""));
  boxiteration = new QCheckBox(gswriteration);
  boxiteration->setGeometry(10,20,150,30);
  boxiteration->setText(tr("Math correction",""));
  //:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
  gconfigcontrol = new QGroupBox(wwobbel);
  gconfigcontrol->setGeometry(290,320,190,60);
  gconfigcontrol->setTitle(tr("Params control",""));
  sendalways = new QCheckBox(gconfigcontrol);
  sendalways->setGeometry(10,20,150,30);
  sendalways->setText(tr("Send always",""));
//#############################################################################
//#############################################################################
//Grundaten fuer den Spekrumanalyser
//#############################################################################
//#############################################################################
  
  wspectrum = new QWidget();
  tabwidget->addTab(wspectrum, tr("SA (1)",""));
  
  gallgemein = new QGroupBox(wspectrum);
  gallgemein->setGeometry(10,10,220,80);
  gallgemein->setTitle(tr("General",""));
  boxsastatus = new QCheckBox(gallgemein);
  boxsastatus->setGeometry(10,20,200,30);
  boxsastatus->setText(tr("HW-Feedback from SA",""));
  QObject::connect( boxsastatus, SIGNAL( toggled (bool)), this, SLOT(checkboxsastatus_checked(bool)));
  
  boxschrittkorr = new QCheckBox(gallgemein);
  boxschrittkorr->setGeometry(10,50,200,30);
  boxschrittkorr->setText(tr("Automatic Step-correction",""));
  QObject::connect( boxschrittkorr, SIGNAL( toggled (bool)), this, SLOT(checkboxschrittkorr_checked(bool)));
  
  gsakw = new QGroupBox(wspectrum);
  gsakw->setGeometry(10,100,220,150);
  gsakw->setTitle(tr("Frequencyrange 1/3",""));
  
  labelfrqa1 = new QLabel(gsakw);
  labelfrqa1->setGeometry(10,22,110,30);
  labelfrqa1->setText(tr("Lower Freq. (Hz)",""));
  labelfrqa1->setAlignment(Qt::AlignRight);
  efrqa1 = new QLineEdit(gsakw);
  efrqa1->setGeometry(130,20,80,25);
  labelfrqb1 = new QLabel(gsakw);
  labelfrqb1->setGeometry(10,52,110,30);
  labelfrqb1->setText(tr("Upper Freq. (Hz)",""));
  labelfrqb1->setAlignment(Qt::AlignRight);
  efrqb1 = new QLineEdit(gsakw);
  efrqb1->setGeometry(130,50,80,25);
  labelfrqzf1 = new QLabel(gsakw);
  labelfrqzf1->setGeometry(10,82,110,30);
  labelfrqzf1->setText(tr("IF + (Hz)",""));
  labelfrqzf1->setAlignment(Qt::AlignRight);
  efrqzf1 = new QLineEdit(gsakw);
  efrqzf1->setGeometry(130,80,80,25);
  labelfrqshift = new QLabel(gsakw);
  labelfrqshift->setGeometry(10,112,110,30);
  labelfrqshift->setText(tr("Shift - (Hz)",""));
  labelfrqshift->setAlignment(Qt::AlignRight);
  efrqshift = new QLineEdit(gsakw);
  efrqshift->setGeometry(130,110,80,25);
  
  gsaukw = new QGroupBox(wspectrum);
  gsaukw->setGeometry(10,260,220,120);
  gsaukw->setTitle(tr("Frequencyrange 2",""));
  
  labelfrqa2 = new QLabel(gsaukw);
  labelfrqa2->setGeometry(10,22,110,30);
  labelfrqa2->setText(tr("Lower Freq. (Hz)",""));
  labelfrqa2->setAlignment(Qt::AlignRight);
  efrqa2 = new QLineEdit(gsaukw);
  efrqa2->setGeometry(130,20,80,25);
  labelfrqb2 = new QLabel(gsaukw);
  labelfrqb2->setGeometry(10,52,110,30);
  labelfrqb2->setText(tr("Upper Freq. (Hz)",""));
  labelfrqb2->setAlignment(Qt::AlignRight);
  efrqb2 = new QLineEdit(gsaukw);
  efrqb2->setGeometry(130,50,80,25);
  labelfrqzf2 = new QLabel(gsaukw);
  labelfrqzf2->setGeometry(10,82,110,30);
  labelfrqzf2->setText(tr("IF - (Hz)",""));
  labelfrqzf2->setAlignment(Qt::AlignRight);
  efrqzf2 = new QLineEdit(gsaukw);
  efrqzf2->setGeometry(130,80,80,25);
  
  gpegelkorr = new QGroupBox(wspectrum);
  gpegelkorr->setGeometry(240,10,200,210);
  gpegelkorr->setTitle(tr("Levelcorrection for dBm display",""));
  labelabsolut1 = new QLabel(gpegelkorr);
  labelabsolut1->setGeometry(10,22,80,30);
  labelabsolut1->setText(tr("Range1 (dB)",""));
  labelabsolut1->setAlignment(Qt::AlignRight);
  eabsolut1 = new QLineEdit(gpegelkorr);
  eabsolut1->setGeometry(100,20,70,25);
  labelabsolut2 = new QLabel(gpegelkorr);
  labelabsolut2->setGeometry(10,52,80,30);
  labelabsolut2->setText(tr("Range2 (dB)",""));
  labelabsolut2->setAlignment(Qt::AlignRight);
  eabsolut2 = new QLineEdit(gpegelkorr);
  eabsolut2->setGeometry(100,50,70,25);
  labelabsolut3 = new QLabel(gpegelkorr);
  labelabsolut3->setGeometry(10,82,80,30);
  labelabsolut3->setText(tr("Range3 (dB)",""));
  labelabsolut3->setAlignment(Qt::AlignRight);
  eabsolut3 = new QLineEdit(gpegelkorr);
  eabsolut3->setGeometry(100,80,70,25);
  labelb300 = new QLabel(gpegelkorr);
  labelb300->setGeometry(10,112,80,30);
  labelb300->setText(tr("B300Hz (dB)",""));
  labelb300->setAlignment(Qt::AlignRight);
  eb300 = new QLineEdit(gpegelkorr);
  eb300->setGeometry(100,110,70,25);
  labelb7k = new QLabel(gpegelkorr);
  labelb7k->setGeometry(10,142,80,30);
  labelb7k->setText(tr("B7kHz (dB)",""));
  labelb7k->setAlignment(Qt::AlignRight);
  eb7k = new QLineEdit(gpegelkorr);
  eb7k->setGeometry(100,140,70,25);
  labelb30k = new QLabel(gpegelkorr);
  labelb30k->setGeometry(10,172,80,30);
  labelb30k->setText(tr("B30kHz (dB)",""));
  labelb30k->setAlignment(Qt::AlignRight);
  eb30k = new QLineEdit(gpegelkorr);
  eb30k->setGeometry(100,170,70,25);

  groupsavinfo = new QGroupBox(tr(" InfoWindow Cursordisplay",""), wspectrum);
  groupsavinfo->setGeometry(240,230,200,90);
  //groupsavinfo->setAlignment(Qt::AlignCenter);
  savdbm = new QCheckBox(groupsavinfo);
  savdbm->setGeometry(10,20,80,30);
  savdbm->setText(tr("dBm",""));
  savdbuv = new QCheckBox(groupsavinfo);
  savdbuv->setGeometry(90,20,80,30);
  savdbuv->setText(tr("dBuV",""));
  savuv = new QCheckBox(groupsavinfo);
  savuv->setGeometry(10,50,80,30);
  savuv->setText(tr("Volts",""));
  savwatt = new QCheckBox(groupsavinfo);
  savwatt->setGeometry(90,50,80,30);
  savwatt->setText(tr("Watts",""));

  groupsavfehler = new QGroupBox(tr("Measure-accuracy/Limits",""), wspectrum);
  groupsavfehler->setGeometry(240,330,200,110);
  labelsafehlermax = new QLabel(groupsavfehler);
  labelsafehlermax->setGeometry(10,22,100,30);
  labelsafehlermax->setText(tr("Acc. max(dBm)",""));
  labelsafehlermax->setAlignment(Qt::AlignRight);
  esafehlermax = new QLineEdit(groupsavfehler);
  esafehlermax->setGeometry(120,20,60,25);
  labelsafehlermin = new QLabel(groupsavfehler);
  labelsafehlermin->setGeometry(10,52,100,30);
  labelsafehlermin->setText(tr("Acc. min(dBm)",""));
  labelsafehlermin->setAlignment(Qt::AlignRight);
  esafehlermin = new QLineEdit(groupsavfehler);
  esafehlermin->setGeometry(120,50,60,25);
  labelsaminschritte = new QLabel(groupsavfehler);
  labelsaminschritte->setGeometry(10,82,100,30);
  labelsaminschritte->setText(tr("Nbr of steps min",""));
  labelsaminschritte->setAlignment(Qt::AlignRight);
  esaminschritte = new QLineEdit(groupsavfehler);
  esaminschritte->setGeometry(120,80,60,25);

  buttonsacal = new QPushButton(tr("calibration",""), wspectrum);
  buttonsacal->setGeometry(240,450,100,30);
  connect(buttonsacal, SIGNAL(clicked()), SLOT(sacal()));
  
  buttonsacalreset = new QPushButton(tr("Cal = 0.0",""), wspectrum);
  buttonsacalreset->setGeometry(120,450,100,30);
  connect(buttonsacalreset, SIGNAL(clicked()), SLOT(sacalreset()));
  
  sastandart = new QPushButton(tr("Set default",""), wspectrum);
  sastandart->setGeometry(10,450,100,30);
  connect(sastandart, SIGNAL(clicked()), SLOT(setStandart()));

  wspectrum2 = new QWidget();
  tabwidget->addTab(wspectrum2, tr("SA (2)",""));

  groupsavgrenzen = new QGroupBox(tr("Frequencies for automatic stepcorrection",""), wspectrum2);
  groupsavgrenzen->setGeometry(10,10,280,140);
  labelgsabmin = new QLabel(groupsavgrenzen);
  labelgsabmin->setGeometry(120,22,100,30);
  labelgsabmin->setText(tr("min.(Hz)",""));
  labelgsabmax = new QLabel(groupsavgrenzen);
  labelgsabmax->setGeometry(190,22,100,30);
  labelgsabmax->setText(tr("max.(Hz)",""));
  labelgsab300 = new QLabel(groupsavgrenzen);
  labelgsab300->setGeometry(10,52,100,30);
  labelgsab300->setText(tr("Bandwidth 300Hz",""));
  labelgsab300->setAlignment(Qt::AlignRight);
  egsab300min = new QLineEdit(groupsavgrenzen);
  egsab300min->setGeometry(120,50,60,25);
  egsab300max = new QLineEdit(groupsavgrenzen);
  egsab300max->setGeometry(190,50,60,25);
  labelgsab7k = new QLabel(groupsavgrenzen);
  labelgsab7k->setGeometry(10,82,100,30);
  labelgsab7k->setText(tr("Bandwidth 7kHz",""));
  labelgsab7k->setAlignment(Qt::AlignRight);
  egsab7kmin = new QLineEdit(groupsavgrenzen);
  egsab7kmin->setGeometry(120,80,60,25);
  egsab7kmax = new QLineEdit(groupsavgrenzen);
  egsab7kmax->setGeometry(190,80,60,25);
  labelgsab30k = new QLabel(groupsavgrenzen);
  labelgsab30k->setGeometry(10,112,100,30);
  labelgsab30k->setText(tr("Bandwidth 30kHz",""));
  labelgsab30k->setAlignment(Qt::AlignRight);
  egsab30kmin = new QLineEdit(groupsavgrenzen);
  egsab30kmin->setGeometry(120,110,60,25);
  egsab30kmax = new QLineEdit(groupsavgrenzen);
  egsab30kmax->setGeometry(190,110,60,25);

  groupsawarnung = new QGroupBox(tr("Warnings",""), wspectrum2);
  groupsawarnung->setGeometry(10,160,160,80);
  boxwarnsavbw = new QCheckBox(groupsawarnung);
  boxwarnsavbw->setGeometry(10,20,200,30);
  boxwarnsavbw->setText(tr("SA Step control",""));
  boxwarnsavbwmax = new QCheckBox(groupsawarnung);
  boxwarnsavbwmax->setGeometry(10,50,200,30);
  boxwarnsavbwmax->setText(tr("SA Steps max.",""));

  wallgemein = new QWidget();
  tabwidget->addTab(wallgemein, tr("General",""));
  
  
  grouppen = new QGroupBox(tr("Pen/Color",""), wallgemein);
  grouppen->setGeometry(10,10,110,150);
  buttoncolorh = new QPushButton(tr("Background",""), grouppen);
  buttoncolorh->setGeometry(10,20,90,30);
  connect(buttoncolorh, SIGNAL(clicked()), SLOT(setColorh()));
  buttoncolor1 = new QPushButton(tr("Ch 1 Color",""), grouppen);
  buttoncolor1->setGeometry(10,50,90,30);
  connect(buttoncolor1, SIGNAL(clicked()), SLOT(setColor1()));
  buttoncolor2 = new QPushButton(tr("Ch 2 Color",""), grouppen);
  buttoncolor2->setGeometry(10,80,90,30);
  connect(buttoncolor2, SIGNAL(clicked()), SLOT(setColor2()));
  labelspwidth = new QLabel(tr("Width",""), grouppen);
  labelspwidth->setGeometry(10,117,40,20);
  labelspwidth->setAlignment(Qt::AlignLeft);
  spwidth = new QSpinBox(grouppen);
  spwidth->setGeometry(50,110,50,30);
  spwidth->setRange(1, 3);

  groupspinpfsize = new QGroupBox(tr("Program",""), wallgemein);
  groupspinpfsize->setGeometry(10,165,110,85);
  groupspinpfsize->setAlignment(Qt::AlignCenter);
  labelspinpfsize = new QLabel(tr("Font-size",""), groupspinpfsize);
  labelspinpfsize->setGeometry(20,20,90,20);
  labelspinpfsize->setAlignment(Qt::AlignLeft);
  spinpfsize = new QSpinBox(groupspinpfsize);
  spinpfsize->setGeometry(20,45,50,30);
  spinpfsize->setRange(7, 12);
  QObject::connect( spinpfsize, SIGNAL( valueChanged(int)), this, SLOT(psetfontsize(int)));

  groupwmprezision = new QGroupBox(tr("Wattmeter",""), wallgemein);
  groupwmprezision->setGeometry(10,255,110,55);
  groupwmprezision->setAlignment(Qt::AlignCenter);
  wmprezision = new QCheckBox(groupwmprezision);
  wmprezision->setGeometry(10,20,200,30);
  wmprezision->setText(tr("Precision 2",""));


  groupfocus = new QGroupBox(tr("Focus Switch",""), wallgemein);
  groupfocus->setGeometry(130,10,160,120);
  groupfocus->setAlignment(Qt::AlignCenter);
  fwobbeln = new QCheckBox(groupfocus);
  fwobbeln->setGeometry(10,20,200,30);
  fwobbeln->setText(tr("Button \"Continuous\"",""));
  feinmal = new QCheckBox(groupfocus);
  feinmal->setGeometry(10,50,200,30);
  feinmal->setText(tr("Button \"Single\"",""));
  fstop = new QCheckBox(groupfocus);
  fstop->setGeometry(10,80,200,30);
  fstop->setText(tr("Button \"Stop\"",""));

  groupnwtruhe = new QGroupBox(tr("NWT Not Active ",""), wallgemein);
  groupnwtruhe->setGeometry(130,140,160,60);
  groupnwtruhe->setAlignment(Qt::AlignCenter);
  chset0hz = new QCheckBox(groupnwtruhe);
  chset0hz->setGeometry(10,20,200,30);
  chset0hz->setText(tr("SET 0 Hz",""));

  groupwdarstellung = new QGroupBox(tr("Sweep illustration",""), wallgemein);
  groupwdarstellung->setGeometry(130,200,160,60);
  groupwdarstellung->setAlignment(Qt::AlignCenter);
  chsetfliessend = new QCheckBox(groupwdarstellung);
  chsetfliessend->setGeometry(10,20,200,30);
  chsetfliessend->setText(tr("Flowing sent",""));

  groupaudio = new QGroupBox(tr("Sweep NF-Audio",""), wallgemein);
  groupaudio->setGeometry(130,270,160,145);
  groupaudio->setAlignment(Qt::AlignCenter);
  labelspinwtime = new QLabel(tr("Sweep time (sec)",""), groupaudio);
  labelspinwtime->setGeometry(20,20,120,20);
  labelspinwtime->setAlignment(Qt::AlignLeft);
  spinwtime = new QSpinBox(groupaudio);
  spinwtime->setGeometry(20,45,50,30);
  spinwtime->setRange(5, 30);
  QObject::connect( spinwtime, SIGNAL( valueChanged(int)), this, SLOT(setspinwtime(int)));
  labelspinwsens = new QLabel(tr("Curve control",""), groupaudio);
  labelspinwsens->setGeometry(20,80,120,20);
  labelspinwsens->setAlignment(Qt::AlignLeft);
  spinwsens = new QSpinBox(groupaudio);
  spinwsens->setGeometry(20,105,50,30);
  spinwsens->setRange(1, 99);



//#############################################################################
//#############################################################################
//Grundaten fuer den Spekrumanalyser
//#############################################################################
//#############################################################################
  
};

void OptionDlg::setspinwtime(int s){
  ogrunddaten.audioztime = 1000 * s;
}


void OptionDlg::psetfontsize(int s)
{
  QFont font("Helvetica", s);

  ok->setFont(font);
  cancel->setFont(font);
  tabwidget->setFont(font);
  kalibrierfrequ->setFont(font);
  labelanfang->setFont(font);
  labelende->setFont(font);
  ddstakt->setFont(font);
  labelddstakt->setFont(font);
  labelcpll->setFont(font);
  cpll->setFont(font);
  gtty->setFont(font);
  labelctty->setFont(font);
  ctty->setFont(font);
  fname->setFont(font);
  labelsonde1->setFont(font);
  labelsonde2->setFont(font);
  fgrenzen->setFont(font);
  labelwobmax->setFont(font);
  gkalibrier->setFont(font);
  rbAttenStandard->setFont(font);
  rbAttenFa->setFont(font);
  rbAttenCustom->setFont(font);
  geinkanalig->setFont(font);
  boxeinkanalig->setFont(font);
  gswriteration->setFont(font);
  boxiteration->setFont(font);
  buttoncolorh->setFont(font);
  buttoncolor1->setFont(font);
  buttoncolor2->setFont(font);
  tabwidget->setFont(font);
  gallgemein->setFont(font);
  gsakw->setFont(font);
  labelfrqa1->setFont(font);
  labelfrqb1->setFont(font);
  efrqb1->setFont(font);
  labelfrqzf1->setFont(font);
  efrqzf1->setFont(font);
  gsaukw->setFont(font);
  labelfrqa2->setFont(font);
  labelfrqb2->setFont(font);
  labelfrqzf2->setFont(font);
  sastandart->setFont(font);
  spinpfsize->setFont(font);
  groupspinpfsize->setFont(font);
  labelspinpfsize->setFont(font);
}

void OptionDlg::setdaten(TBaseData agrunddaten){
  QString s;

  ogrunddaten = agrunddaten;
  s.sprintf("%1.0f", ogrunddaten.calibrate_start_freq);
  eanfang->setText(s);
  s.sprintf("%1.0f", ogrunddaten.calibrate_end_freq);
  eende->setText(s);
  s.sprintf("%9.0f", ogrunddaten.dds_core_freq);
  eddstakt->setText(s);
  s.sprintf("%9.0f", ogrunddaten.max_sweep_freq);
  wobmax->setText(s);
  s.sprintf("%i", ogrunddaten.freq_faktor);
  cbfrqfaktor->setCurrentText(s);
  ctty->setCurrentText(ogrunddaten.str_tty);

  switch (ogrunddaten.attenuatorType)
  {
  case AttenuatorType_Fa:
      rbAttenFa->setChecked(true);
      rbAttenStandard->setChecked(false);
      rbAttenCustom->setChecked(false);
      break;
  case AttenuatorType_Custom:
      rbAttenCustom->setChecked(true);
      rbAttenStandard->setChecked(false);
      rbAttenFa->setChecked(false);
      break;
  default:
      rbAttenStandard->setChecked(true);
      rbAttenFa->setChecked(false);
      rbAttenCustom->setChecked(false);
      break;
  }
  edtAttenConfig->setEnabled(rbAttenCustom->isChecked());
  edtAttenConfig->setText(ogrunddaten.attenuatorCustomConfig);

  fwobbeln->setChecked(ogrunddaten.grperwobbeln);
  feinmal->setChecked(ogrunddaten.grpereinmal);
  fstop->setChecked(ogrunddaten.grperstop);
  chset0hz->setChecked(ogrunddaten.bset0hz);
  chsetfliessend->setChecked(ogrunddaten.bfliessend);
  boxwarnsavbw->setChecked(ogrunddaten.bwarnsavbw);
  boxwarnsavbwmax->setChecked(ogrunddaten.bwarnsavbwmax);
  savdbm->setChecked(ogrunddaten.bsavdbm);
  savuv->setChecked(ogrunddaten.bsavuv);
  savwatt->setChecked(ogrunddaten.bsavwatt);
  savdbuv->setChecked(ogrunddaten.bsavdbuv);
  boxiteration->setChecked(ogrunddaten.bswriteration);
  boxeinkanalig->setChecked(ogrunddaten.numberchanel);
  wmprezision->setChecked(ogrunddaten.wmprezision == 2);
  sendalways->setChecked(ogrunddaten.bsendalways);

  esonde1->setText(ogrunddaten.strsonde1);
  esonde1lin->setText(ogrunddaten.strsonde1lin);
  esonde2->setText(ogrunddaten.strsonde2);
  cpll->setCurrentIndex(ogrunddaten.pll);
  if(ogrunddaten.pllmodeenable){
    labelcpll->setText(tr("DDS Mode :",""));
    labelcpll->setEnabled(true);
    cpll->setEnabled(true);
  }else{
    labelcpll->setText(tr("Not Active !",""));
  }
  // Button Farbr setzen
  QColor color = ogrunddaten.penkanal1.color();
  QPalette palette = buttoncolor1->palette();
  palette.setColor(QPalette::ButtonText, color);
  buttoncolor1->setPalette(palette);
  
  color = ogrunddaten.penkanal2.color();
  palette = buttoncolor2->palette();
  palette.setColor(QPalette::ButtonText, color);
  buttoncolor2->setPalette(palette);
  spwidth->setValue(ogrunddaten.penwidth);
  spinwtime->setValue(ogrunddaten.audioztime / 1000);
  spinwsens->setValue(ogrunddaten.audiosens);

  //SA
  s.sprintf("%0.0f", ogrunddaten.frqa1);
  efrqa1->setText(s);
  s.sprintf("%0.0f", ogrunddaten.frqb1);
  efrqb1->setText(s);
  s.sprintf("%0.0f", ogrunddaten.frqa2);
  efrqa2->setText(s);
  s.sprintf("%0.0f", ogrunddaten.frqb2);
  efrqb2->setText(s);
  s.sprintf("%0.0f", ogrunddaten.frqzf1);
  efrqzf1->setText(s);
  s.sprintf("%0.0f", ogrunddaten.frqzf2);
  efrqzf2->setText(s);
  s.sprintf("%0.0f", ogrunddaten.frqshift);
  efrqshift->setText(s);
  s.sprintf("%0.2f", ogrunddaten.psavabs1);
  eabsolut1->setText(s);
  s.sprintf("%0.2f", ogrunddaten.psavabs2);
  eabsolut2->setText(s);
  s.sprintf("%0.2f", ogrunddaten.psavabs3);
  eabsolut3->setText(s);
  s.sprintf("%0.2f", ogrunddaten.psav300);
  eb300->setText(s);
  s.sprintf("%0.2f", ogrunddaten.psav7k);
  eb7k->setText(s);
  s.sprintf("%0.2f", ogrunddaten.psav30k);
  eb30k->setText(s);
  
  s.sprintf("%0.0f", ogrunddaten.bw300_max);
  egsab300max->setText(s);
  s.sprintf("%0.0f", ogrunddaten.bw300_min);
  egsab300min->setText(s);
  s.sprintf("%0.0f", ogrunddaten.bw7kHz_max);
  egsab7kmax->setText(s);
  s.sprintf("%0.0f", ogrunddaten.bw7kHz_min);
  egsab7kmin->setText(s);
  s.sprintf("%0.0f", ogrunddaten.bw30kHz_max);
  egsab30kmax->setText(s);
  s.sprintf("%0.0f", ogrunddaten.bw30kHz_min);
  egsab30kmin->setText(s);
  
  s.sprintf("%i", ogrunddaten.safehlermax);
  esafehlermax->setText(s);
  s.sprintf("%i", ogrunddaten.safehlermin);
  esafehlermin->setText(s);
  s.sprintf("%i", ogrunddaten.saminschritte);
  esaminschritte->setText(s);
  boxsastatus->setChecked(ogrunddaten.sastatus);
  boxschrittkorr->setChecked(ogrunddaten.bschrittkorr);
  spinpfsize->setValue(ogrunddaten.pfsize);
  if(ogrunddaten.fwWrong){
    boxsastatus->setEnabled(false);
    boxschrittkorr->setEnabled(false);
    eabsolut1->setEnabled(false);
    labelabsolut1->setEnabled(false);
    eabsolut2->setEnabled(false);
    labelabsolut2->setEnabled(false);
    eabsolut3->setEnabled(false);
    labelabsolut3->setEnabled(false);
    eb300->setEnabled(false);
    labelb300->setEnabled(false);
    eb7k->setEnabled(false);
    labelb7k->setEnabled(false);
    eb30k->setEnabled(false);
    labelb30k->setEnabled(false);
  }else{
    if(!boxsastatus->isChecked()){
      eb300->setEnabled(false);
      labelb300->setEnabled(false);
      eb7k->setEnabled(false);
      labelb7k->setEnabled(false);
      eb30k->setEnabled(false);
      labelb30k->setEnabled(false);
    }
    if(!boxschrittkorr->isChecked()){
      esaminschritte->setEnabled(false);
      labelsaminschritte->setEnabled(false);
    }
  }
};

TBaseData OptionDlg::getdaten(){
  QString s;
  bool ok;

  s = eanfang->text();
  ogrunddaten.calibrate_start_freq = s.toDouble(&ok);
  if(!ok)ogrunddaten.calibrate_start_freq = 100000.0;
  
  s = eende->text();
  ogrunddaten.calibrate_end_freq = s.toDouble(&ok);
  if(!ok)ogrunddaten.calibrate_end_freq = 150000000.0;
  
  s = eddstakt->text();
  ogrunddaten.dds_core_freq = s.toDouble(&ok);
  if(!ok)ogrunddaten.dds_core_freq = 400000000.0;
  
  s = wobmax->text();
  ogrunddaten.max_sweep_freq = s.toDouble(&ok);
  if(!ok)ogrunddaten.max_sweep_freq = 200000000.0;
  
  s = cbfrqfaktor->currentText();
  ogrunddaten.freq_faktor = s.toInt(&ok);
  if(!ok)ogrunddaten.freq_faktor = 1;
  
  ogrunddaten.str_tty = ctty->currentText();

  // Attenuator
  if (rbAttenFa->isChecked()) ogrunddaten.attenuatorType = AttenuatorType_Fa;
  else if (rbAttenCustom->isChecked()) ogrunddaten.attenuatorType = AttenuatorType_Custom;
  else ogrunddaten.attenuatorType = AttenuatorType_Standard;

  if (AttenuatorLoader::IsCustomConfigValid(edtAttenConfig->text()))
  {
      ogrunddaten.attenuatorCustomConfig = edtAttenConfig->text();
  }
  else
  {
      ogrunddaten.attenuatorCustomConfig = AttenuatorLoader::GetEmptyCustomConfig();
  }
  AttenuatorLoader::Load(ogrunddaten.attenuator, ogrunddaten.attenuatorType, ogrunddaten.attenuatorCustomConfig);

  ogrunddaten.numberchanel = boxeinkanalig->isChecked();
  ogrunddaten.bswriteration = boxiteration->isChecked();
  ogrunddaten.grperwobbeln = fwobbeln->isChecked();
  ogrunddaten.grpereinmal = feinmal->isChecked();
  ogrunddaten.grperstop = fstop->isChecked();
  ogrunddaten.bset0hz = chset0hz->isChecked();
  ogrunddaten.bwarnsavbw = boxwarnsavbw->isChecked();
  ogrunddaten.bwarnsavbwmax = boxwarnsavbwmax->isChecked();
  ogrunddaten.bsavdbm = savdbm->isChecked();
  ogrunddaten.bsavuv = savuv->isChecked();
  ogrunddaten.bsavwatt = savwatt->isChecked();
  ogrunddaten.bsavdbuv = savdbuv->isChecked();
  ogrunddaten.bsendalways = sendalways->isChecked();

  if(wmprezision->isChecked()){
    ogrunddaten.wmprezision = 2;
  }else{
    ogrunddaten.wmprezision = 1;
  }
  ogrunddaten.strsonde1 = esonde1->text();
  ogrunddaten.strsonde1lin = esonde1lin->text();
  ogrunddaten.strsonde2 = esonde2->text();
  ogrunddaten.pll = cpll->currentIndex();
  ogrunddaten.pfsize = spinpfsize->value();
  ogrunddaten.audioztime = spinwtime->value() * 1000;
  ogrunddaten.audiosens = spinwsens->value();
  ogrunddaten.penwidth = spwidth->value();

  //***************************************************************
  //SA Einstellungen speichern
  //***************************************************************
  s = efrqa1->text();
  ogrunddaten.frqa1 = s.toDouble(&ok);
  if(!ok)ogrunddaten.frqa1 = 1000000.0;
  
  s = efrqb1->text();
  ogrunddaten.frqb1 = s.toDouble(&ok);
  if(!ok)ogrunddaten.frqb1 = 72000000.0;
  
  s = efrqa2->text();
  ogrunddaten.frqa2 = s.toDouble(&ok);
  if(!ok)ogrunddaten.frqa2 = 135000000.0;
  
  s = efrqb2->text();
  ogrunddaten.frqb2 = s.toDouble(&ok);
  if(!ok)ogrunddaten.frqb2 = 149000000.0;
  
  s = efrqzf1->text();
  ogrunddaten.frqzf1 = s.toDouble(&ok);
  if(!ok)ogrunddaten.frqzf1 = 85300000.0;
  
  s = efrqzf2->text();
  ogrunddaten.frqzf2 = s.toDouble(&ok);
  if(!ok)ogrunddaten.frqzf2 = -85300000.0;
  
  s = efrqshift->text();
  ogrunddaten.frqshift = s.toDouble(&ok);
  if(!ok)ogrunddaten.frqshift = -410000000.0;
  
  s = eabsolut1->text();
  ogrunddaten.psavabs1 = s.toDouble(&ok);
  if(!ok)ogrunddaten.psavabs1 = 0.0;
  
  s = eabsolut2->text();
  ogrunddaten.psavabs2 = s.toDouble(&ok);
  if(!ok)ogrunddaten.psavabs2 = 0.0;
  
  s = eabsolut3->text();
  ogrunddaten.psavabs3 = s.toDouble(&ok);
  if(!ok)ogrunddaten.psavabs3 = 0.0;
  
  s = eb300->text();
  ogrunddaten.psav300 = s.toDouble(&ok);
  if(!ok)ogrunddaten.psav300 = 0.0;
  
  s = eb7k->text();
  ogrunddaten.psav7k = s.toDouble(&ok);
  if(!ok)ogrunddaten.psav7k = 0.0;
  
  s = eb30k->text();
  ogrunddaten.psav30k = s.toDouble(&ok);
  if(!ok)ogrunddaten.psav30k = 0.0;
  
  ogrunddaten.sastatus = boxsastatus->isChecked();
  ogrunddaten.bschrittkorr = boxschrittkorr->isChecked();
  s = esafehlermax->text();
  ogrunddaten.safehlermax = s.toInt(&ok);
  if(!ok)ogrunddaten.safehlermax = -5;
  
  s = esafehlermin->text();
  ogrunddaten.safehlermin = s.toInt(&ok);
  if(!ok)ogrunddaten.safehlermin = -60;
  
  s = esaminschritte->text();
  ogrunddaten.saminschritte = s.toInt(&ok);
  if(!ok)ogrunddaten.saminschritte = 401;
  
  s = egsab300max->text();
  ogrunddaten.bw300_max = s.toDouble(&ok);
  if(!ok)ogrunddaten.bw300_max = 100.0;
  
  s = egsab300min->text();
  ogrunddaten.bw300_min = s.toDouble(&ok);
  if(!ok)ogrunddaten.bw300_min = 50.0;
  
  s = egsab7kmax->text();
  ogrunddaten.bw7kHz_max = s.toDouble(&ok);
  if(!ok)ogrunddaten.bw7kHz_max = 2500.0;
  
  s = egsab7kmin->text();
  ogrunddaten.bw7kHz_min = s.toDouble(&ok);
  if(!ok)ogrunddaten.bw7kHz_min = 1250.0;
  
  s = egsab30kmax->text();
  ogrunddaten.bw30kHz_max = s.toDouble(&ok);
  if(!ok)ogrunddaten.bw30kHz_max = 10000.0;
  
  s = egsab30kmin->text();
  ogrunddaten.bw30kHz_min = s.toDouble(&ok);
  if(!ok)ogrunddaten.bw30kHz_min = 5000.0;
  
  if(ogrunddaten.bw300_min >= ogrunddaten.bw300_max)ogrunddaten.bw300_min = ogrunddaten.bw300_max - 1.0;
  if(ogrunddaten.bw7kHz_min >= ogrunddaten.bw7kHz_max)ogrunddaten.bw7kHz_min = ogrunddaten.bw7kHz_max - 1.0;
  if(ogrunddaten.bw30kHz_min >= ogrunddaten.bw30kHz_max)ogrunddaten.bw30kHz_min = ogrunddaten.bw30kHz_max - 1.0;
//***************************************************************
  return ogrunddaten;
};

void OptionDlg::tip(bool btip)
{
QString tip_einkanalig = tr("<br><b> Activate when operating one Channel  only.</b> <br><br>The NWT01 has basically just one Channel,<br>(Channel_1 , Lin or Log) .<br> When a second channel is available,  Pin 7 of the PIC<br> serves as Measuring-input .<br>When de-activated, the second channel is activated. <br>","");
QString tip_mathkorr = tr("<br><b>Mathemetical smooting of the SWR graph :</b><br><br>With small dB-Changes, the SWR changes in big steps . (at high SWR only).<br>  To smooth the display, from 3 measurements the average  is calculated and displayed.<br>The same Mathemetical procedure is used when the SWR Calibration-file is made.<br>","");
QString tip_defdatei = tr("<br><b> Default Filename of the calibrationfile </b> <br><br>This File-name is used as the default setting. <br>It is to be entered without .ext .<br>The .ext  is added automatically by the software. <br>","");
QString tip_fmaxwobbel = tr("<br><b> Maximum Frequency in Sweepmode :</b> <br><br> This is the max. Frequency that can be used in Sweepmode <br><br> <em> NWT01-FA </em>.....about 200MHz.<br> <em> HFM9 </em>...........about 200MHz. <br> <em> NWT7 </em>.......... with DDS_Clock, 180MHz about 70MHz.  <br> <em> NWT500 </em>....... with DDS_ Clock 1200MHz a,bout 500MHz. <br>","");
QString tip_serielle = tr("<br><b> Serial Interface : </b> <br><br> Setting the COM port of the PC. <br>When the COM port does not react, restart the Program. <br>The behavior is Operating System dependent.<br>Under Linux the command is editable.<br> Under Windows  you can select  COM1 to COM256.<br>","");
QString tip_pll = tr("<br><b> Setting the PLL in the AD9951 DDS :</b> <br><br>This item is  disabled normally.<br>When NWT01 is used,  this setting is<b> not</b> necessary. <br>In that case the PLL is set in the PIC .<br><br> When this setting is enabled, appropriate Firmware in the PIC<br> is required.<br>","");
QString tip_ddstakt = tr("<br><b> Setting the Clockfrequency of the DDS :</b> <br><br> Here you can calibrate the Outputfrequency of the Networktester.<br> This figure will be stored in the PIC after pressing \"OK\"  and is available after each power ON.<br><br><b> <em> Calibration :</b> </em><br> <br> 1) Set VFO at 10MHz. <br> 2) Measure the actual Outputfrequency. <br> 3)  F(new Clockfrequency) = F(old Clockfrequency)* F(Measured frequency) / 10 000 000). <br> 4) Enter F(new Clockfrequency) and press OK,  the Firmware causes a \"warm\" re-start.<br>5) Switch over to Sweepmode and return to VFO-mode, this causes a reload of the VFO setting.<br><br> <b>Now the Outputfrequency must be 10 MHz exactly.<br>","");
QString tip_kalibrierend = tr("<br><b>Setting the Upper frequency for Calibration-correction :</b><br><br>Here the max. Frequency for calibration is set.<br>This is necessary for the Number Samples of the Mathemetical correction.<br><br>The second function of this Stopfrequency is for the<br> Frequencyshift, when often the \"plus\" is pressed there must<br> be an Upper-limit.<br>","");
QString tip_kalibrieranf = tr("<br><b>Setting the Startfrequency for Calibration-correction :</b><br><br>Here the min. Frequency for calibration-correction is set<br>This is necessary for the Number of Samples of the Mathematical correction.<br><br>The second function of this Startfrequency is for the<br> Frequencyshift,when often the \"minus\" is pressed there must be a Lower-limit.<br>","");
QString tip_color = tr("<br><b> Setting the Color :</b> <br><br> The Colorsettings are saved  in the \"*. HFC\"  file . <br>","");
QString tip_rbAttenStandard = tr("<br><b> Standard attenuator (10-20-20 dB)</b> <br><br> <em>When using this attenuator please be aware of the new wiring starting from the firmware version 1.10. </em></b><br>","");
QString tip_rbAttenFa = tr("<br><b> FA attenuator (0-66 dB)</b> <br><br> Firmware version 1.10 or later required.<br>","");
QString tip_rbAttenCustom = tr("<br><b> Custom attenuator</b> <br><br> The custom attenuator should be connected to attenuator pins of the microsontroller. <br> Firmware version 1.10 or later required.<br>","");
QString tip_edtAttenConfig = tr("<br><b> Custom attenuator configuration:</b> <br><br> Type the attenuation of the attenuator stage connected to attenuator pins of the microcontroller (starting from first used bit) separated by &quot;;&quot;. <br> Keep coefficients of unused port bits empty, e.g: <br><b> &quot;2;4;;16;16;&quot; </b<rb>","");
QString tip_hwrueck = tr("<br><b>Feedback from Spectrumanalyser software :</b><br><br>When a Spectrumanalyser Unit is connected to<br>the FA-NWT, it is advisable to activate this item. <br> This will return all Switch-positions of the Spectrumanalyser<br> to the software. <br><br>Condition is appropriate Firmware in the FA-NWT.<br>This works  with the NWT from \"Funkamateur\" only.<br>","");
QString tip_autoschritt = tr("<br><b>Automatic Stepcorrection :</b><br><br>When the HW-feedback is active, this item can be used<br> to correct the Stepsize automatically.<br><br>Example :  Is  the Stepsize in the NWT set to 10kHz<br>and the  Spectrumanalyser is set to 7kHz, it would<br>lead to measuring-errors. <br><br>When the Item is active the Number of samples is raised<br>to the point that the Stepsize is smaller then 3,5 kHz.<br> When this is not possible a Warning pops up.<br>","");
QString tip_sabereich1 = tr("<b><br>SA Range 1-3 settings :</b><br><br>Here are set  the minimal-, maximal-Frequency and the IF<br> for the \"FunkAmateur\" SpectrumAnalyser unit.<br>The max. frequency is determined by the first IF of the SA<br>Shift is for \"Spectr.Freq-shift\" , shift of the displayed frequency in the display.","");
QString tip_sabereich2 = tr("<br><b> Spectrumanalyser Range 2 settings :</b> <br><br>Enter here the Minimum frequency ,<br> Maximum frequency and the IF for the<br>Spectrumanalyser Unit from \"FunkAmateur\" .<br> The Minimum frequency is defined by the<br> first IF of the Spectrumanalyser.<br>The VFO frequency here is below the first IF.<br>","");
QString tip_sastandart = tr("<br><b>SpectrumAnalyse Default values :</b><br><br>If incorrect values  are entered you can reset all to Default.","");
QString tip_sapegelabs = tr("<b><br> SpectrumAnalyser Calibration :</b><br><br>Levelshift Range 1-3<br>","");
QString tip_sapegelbw = tr("<br><b>SpectrumAnalyser Calibration :</b><br><br>Equalising the levels of the several Bandwidths.<br>","");
QString tip_sacalnull = tr("<b><br>SpectrumAnalyser Calibration :</b><br><br>Clear CalibrationArray.<br>","");
QString tip_sacal = tr("<b><br>SpectrumAnalyser Calibration :</b><br><br>Optimising CalibrationArray with GraphData.<br>The attenuation of LP-Filter range 1 and BP-Filter range 2 is calculated into the calibration.<br>(see the manual)<br>","");
QString tip_sabwunten = tr("<b><br>SpectrumAnalyser automatic stepcorrection :</b><br><br>Here the frequency limits are set where the number of samples are lowered.<br>This value shall be smaller then the max. value.","");
QString tip_sabwoben = tr("<b><br>SpectrumAnalyser automatic stepcorrection :</b><br><br>Here the frequency limits are set, where the stepsize is corrected by increasing the number of steps.<br>The value shall be set that high, that 2 measurements at least are within -3dB of the passband.<br>So, at 300Hz bandwith 100Hz shall be set.","");
QString tip_saungenau = tr("<b><br>SpectrumAnlyser Calibration :</b><br><br>The gray area of the inaccurate measuring range.<br>","");
QString tip_sawarnung = tr("<b><br>SpectrumAnalyser Warnings :</b><br><br>When the automatic stepcorrection is de-activated and the stepsize is too large,a recommandation follows.<br>This warning can be suppressed here.","");
QString tip_sawarnungmax = tr("<b><br>SpectrumAnalyser Warnings :</b><br><br>When the max.number of Samples is reached and the stepsize is still too large, then a recommandation is shown.<br>Is this ennoying, it can be suppressed here.","");
QString tip_saminschritt = tr("<b><br>SpectrumAnalyser Calibration :</b><br><br>The lower limit of number of samples at automatic stepcorrection.<br>","");
QString tip_schriftgroesse = tr("<br><b>Setting the Font size :</b><br><br>The fontsize can be set here. <br> It works on almost all the texts in the program.","");
QString tip_focusumschaltung = tr("<br><b>Automatic Focus-switch :</b><br><br>After re-organising the Graphical-display <br> the Control of the program is a bit more difficult. <br>A Simplification is the Automatic focus-transfer,<br> after the operation of a button.<br><br><b> Which button that is, can be set here.</b><br>","");
QString tip_nfaudio = tr("<b> Sweep NF-Audio </B> <BR> The <em> sweep time </em> 5 - 30 seconds <br> a sweep continuous coarse in seconds where the control is at wildly varying levels <br> <em> Wobbelkontrolle </em> not counting this time. <br> The <em> Sweep control</em> 1-99 <br> is the sensibility is <br> taken on the level fluctuations. Once a level fluctuation is detected, download the next 5 measuring points sweepped slowly. <br> The first 50 measurement points are always sweepped slowly, download as almost always with low frequencies below 1000 Hz is started. <br> 1 = very sensitive <br> 99 = least sensitive <br>","");
QString tip_prezision = tr("<b> Wattmeter </B> <BR> 2 decimal places in dBm display <br>","");
QString tip_configparamssendalways = tr("<br><b>Control config params :</b><br><br>"
                                        "Checked - then config params always is sent to device.<br>"
                                        "<b>For example</b> -  DDS const always is sent to device.<br><br>"
                                        "Not checked - only changed config params is sent to device.<br>"
                                        "", "");
  if(btip){
    boxeinkanalig->setToolTip(tip_einkanalig);
    boxiteration->setToolTip(tip_mathkorr);
    esonde1->setToolTip(tip_defdatei);
    esonde2->setToolTip(tip_defdatei);
    wobmax->setToolTip(tip_fmaxwobbel);
    ctty->setToolTip(tip_serielle);
    cpll->setToolTip(tip_pll);
    eddstakt->setToolTip(tip_ddstakt);
    eende->setToolTip(tip_kalibrierend);
    eanfang->setToolTip(tip_kalibrieranf);
    buttoncolor1->setToolTip(tip_color);
    buttoncolor2->setToolTip(tip_color);
    rbAttenStandard->setToolTip(tip_rbAttenStandard);
    rbAttenFa->setToolTip(tip_rbAttenFa);
    rbAttenCustom->setToolTip(tip_rbAttenCustom);
    edtAttenConfig->setToolTip(tip_edtAttenConfig);
    buttoncolorh->setToolTip(tip_color);
    boxsastatus->setToolTip(tip_hwrueck);
    boxschrittkorr->setToolTip(tip_autoschritt);
    gsakw->setToolTip(tip_sabereich1);
    gsaukw->setToolTip(tip_sabereich2);
    sastandart->setToolTip(tip_sastandart);
    eabsolut1->setToolTip(tip_sapegelabs);
    labelabsolut1->setToolTip(tip_sapegelabs);
    eabsolut2->setToolTip(tip_sapegelabs);
    labelabsolut2->setToolTip(tip_sapegelabs);
    eabsolut3->setToolTip(tip_sapegelabs);
    labelabsolut3->setToolTip(tip_sapegelabs);
    eb300->setToolTip(tip_sapegelbw);
    labelb300->setToolTip(tip_sapegelbw);
    eb7k->setToolTip(tip_sapegelbw);
    labelb7k->setToolTip(tip_sapegelbw);
    eb30k->setToolTip(tip_sapegelbw);
    labelb30k->setToolTip(tip_sapegelbw);
    buttonsacal->setToolTip(tip_sacal);
    buttonsacalreset->setToolTip(tip_sacalnull);
    esafehlermax->setToolTip(tip_saungenau);
    labelsafehlermax->setToolTip(tip_saungenau);
    esafehlermin->setToolTip(tip_saungenau);
    labelsafehlermin->setToolTip(tip_saungenau);
    esaminschritte->setToolTip(tip_saminschritt);
    labelsaminschritte->setToolTip(tip_saminschritt);
    groupspinpfsize->setToolTip(tip_schriftgroesse);
    groupwmprezision->setToolTip(tip_prezision);
    groupaudio->setToolTip(tip_nfaudio);
    groupfocus->setToolTip(tip_focusumschaltung);
    egsab300max->setToolTip(tip_sabwoben);
    egsab300min->setToolTip(tip_sabwunten);
    egsab7kmax->setToolTip(tip_sabwoben);
    egsab7kmin->setToolTip(tip_sabwunten);
    egsab30kmax->setToolTip(tip_sabwoben);
    egsab30kmin->setToolTip(tip_sabwunten);
    boxwarnsavbw->setToolTip(tip_sawarnung);
    boxwarnsavbwmax->setToolTip(tip_sawarnungmax);
    sendalways->setToolTip(tip_configparamssendalways);
  }else{
    boxeinkanalig->setToolTip("");
    boxiteration->setToolTip("");
    esonde1->setToolTip("");
    esonde2->setToolTip("");
    wobmax->setToolTip("");
    ctty->setToolTip("");
    cpll->setToolTip("");
    eddstakt->setToolTip("");
    eende->setToolTip("");
    eanfang->setToolTip("");
    buttoncolor1->setToolTip("");
    buttoncolor2->setToolTip("");
    rbAttenStandard->setToolTip("");
    rbAttenFa->setToolTip("");
    rbAttenCustom->setToolTip("");
    edtAttenConfig->setToolTip("");
    buttoncolorh->setToolTip("");
    boxsastatus->setToolTip("");
    boxschrittkorr->setToolTip("");
    gsakw->setToolTip("");
    gsaukw->setToolTip("");
    sastandart->setToolTip("");
    eabsolut1->setToolTip("");
    labelabsolut1->setToolTip("");
    eabsolut2->setToolTip("");
    labelabsolut2->setToolTip("");
    eabsolut3->setToolTip("");
    labelabsolut3->setToolTip("");
    eb300->setToolTip("");
    labelb300->setToolTip("");
    eb7k->setToolTip("");
    labelb7k->setToolTip("");
    eb30k->setToolTip("");
    labelb30k->setToolTip("");
    buttonsacal->setToolTip("");
    buttonsacalreset->setToolTip("");
    esafehlermax->setToolTip("");
    labelsafehlermax->setToolTip("");
    esafehlermin->setToolTip("");
    labelsafehlermin->setToolTip("");
    esaminschritte->setToolTip("");
    labelsaminschritte->setToolTip("");
    groupspinpfsize->setToolTip("");
    groupwmprezision->setToolTip("");
    groupaudio->setToolTip("");
    groupfocus->setToolTip("");
    egsab300max->setToolTip("");
    egsab300min->setToolTip("");
    egsab7kmax->setToolTip("");
    egsab7kmin->setToolTip("");
    egsab30kmax->setToolTip("");
    egsab30kmin->setToolTip("");
    boxwarnsavbw->setToolTip("");
    boxwarnsavbwmax->setToolTip("");
    sendalways->setToolTip("");
  }
}

void OptionDlg::setStandart(){
//  qDebug("setStandart()");
  efrqa1->setText("1000000");
  efrqb1->setText("72000000");
  efrqa2->setText("135000000");
  efrqb2->setText("149000000");
  efrqzf1->setText("85300000");
  efrqzf2->setText("-85300000");
  efrqshift->setText("0");
  eabsolut1->setText("0,00");
  eabsolut2->setText("0,00");
  eabsolut3->setText("0,00");
  eb300->setText("0,00");
  eb7k->setText("0,00");
  eb30k->setText("0,00");
  esafehlermax->setText("5");
  esafehlermin->setText("-60");
  esaminschritte->setText("401");
  egsab300max->setText("100");
  egsab300min->setText("50");
  egsab7kmax->setText("2500");
  egsab7kmin->setText("1250");
  egsab30kmax->setText("10000");
  egsab30kmin->setText("5000");
  savdbm->setChecked(true);
  savuv->setChecked(false);
  savwatt->setChecked(false);
  savdbuv->setChecked(false);
}

void OptionDlg::setColor1(){
  bool ok;
  QColor color = ogrunddaten.penkanal1.color();
  QRgb rgb;
  rgb = color.rgb();
//  qDebug("setColor1");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    ogrunddaten.penkanal1.setColor(color);
    QPalette palette = buttoncolor1->palette();
    palette.setColor(QPalette::Active, QPalette::Button, color);
    buttoncolor1->setPalette(palette);
  }
}

void OptionDlg::setColor2(){
  bool ok;
  QColor color = ogrunddaten.penkanal2.color();
  QRgb rgb;
  rgb = color.rgb();
//  qDebug("setColor2");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    ogrunddaten.penkanal2.setColor(color);
    QPalette palette = buttoncolor2->palette();
    palette.setColor(QPalette::Active, QPalette::Button, color);
    buttoncolor2->setPalette(palette);
  }
}

void OptionDlg::setColorh(){
  bool ok;
  QColor chd;
  QColor color = ogrunddaten.colorhintergrund;
  QRgb rgb;
  
  rgb = color.rgb();
//  qDebug("setColor2");
  color = QColorDialog::getRgba(rgb, &ok, this);
  if(ok){
    ogrunddaten.colorhintergrund = color;
  }
}

void OptionDlg::rbAttenCustom_toggled(bool newVal)
{
    edtAttenConfig->setEnabled(newVal);
}

void OptionDlg::edtAttenConfig_textChanged(const QString& newText)
{
    bool isValidInput = AttenuatorLoader::IsCustomConfigValid(newText);
    QString style = isValidInput
            ? "QLineEdit { }"
            : "QLineEdit { background: rgb(255, 100, 100); }";
    edtAttenConfig->setStyleSheet(style);
}

void OptionDlg::checkboxsastatus_checked(bool ok)
{
  if(ok){
    eb300->setEnabled(true);
    labelb300->setEnabled(true);
    eb7k->setEnabled(true);
    labelb7k->setEnabled(true);
    eb30k->setEnabled(true);
    labelb30k->setEnabled(true);
    if(boxschrittkorr->isChecked()){
      esaminschritte->setEnabled(true);
      labelsaminschritte->setEnabled(true);
    }
  }else{
    eb300->setEnabled(false);
    labelb300->setEnabled(false);
    eb7k->setEnabled(false);
    labelb7k->setEnabled(false);
    eb30k->setEnabled(false);
    labelb30k->setEnabled(false);
    esaminschritte->setEnabled(false);
    labelsaminschritte->setEnabled(false);
  }
}

void OptionDlg::checkboxschrittkorr_checked(bool ok)
{
  if(ok){
    esaminschritte->setEnabled(true);
    labelsaminschritte->setEnabled(true);
  }else{
    esaminschritte->setEnabled(false);
    labelsaminschritte->setEnabled(false);
  }
}

void OptionDlg::setsavcal(const TSavcalarray &asavcalarray){
  osavcalarray = asavcalarray;
}

TSavcalarray OptionDlg::getsavcal(){
  return osavcalarray;
}

void OptionDlg::sacal(){
  int i, j, k, calix, calixalt;
  bool v305 = false;
  //bool v404 = false;
  //bool v405 = false;
  QString zeile;
  QString qs;
  double faktor1, faktor2;
  double shift1, shift2;
  bool ok;
  TSweep w;
  QFile f;
  double fanfang;
  double fende;
  double frequenz;
  double pegel_0;
  double pegel, pegelalt;
  double pegelmin = 20.0;
  double pegelmax = -100.0;
  QString beschr;
  
  QDir kurvendir = ogrunddaten.kurvendir;
  QStringList filters;
  filters << "Image files (*.png *.xpm *.jpg)"
          << "Text files (*.txt)"
          << "Any files (*)";

  QString s = QFileDialog::getOpenFileName(this, tr("Loading Calibration Graph",""),
                                                 kurvendir.absolutePath(),
                                                 tr("NWT Graphfile (*.hfd)","")
                                                 );
  //String ist nicht leer
  if (!s.isNull())
  {
    kurvendir.setPath(s);
    qs = kurvendir.absolutePath();
    //qDebug(qs);
    //Filename setzen
    f.setFileName(s);
    if(f.open( QIODevice::ReadOnly )){
      QTextStream ts(&f);
      //erste Zeile lesen
      ts >> zeile;
      if(zeile == "#V3.05")v305 = true;
      if(zeile == "#V4.04"){
        //v404 = true;
        v305 = true;
      }
      if(zeile == "#V4.05"){
        //v405 = true;
        //v404 = true;
        v305 = true;
      }
      if(!v305){
        w.anfangsfrequenz = zeile.toLong(&ok);
      }else{
        //Es ist V3.05 Anfangsfrequenz kommt jetzt
        ts >> w.anfangsfrequenz;
      }
      //qDebug("a1 %li", wobdaten.anfangsfrequenz);
      ts >> w.schrittfrequenz;
      ts >> w.cntpoints;
      if(w.cntpoints > maxmesspunkte)w.cntpoints = maxmesspunkte;
      ts >> faktor1;
      ts >> shift1;
      ts >> faktor2;
      ts >> shift2;
      ts >> i;
      switch(i){
        //log
        case 0:
        case 1:
        case 3:
        case 4:
        case 5:
          if(!v305){
            w.faktor1 = faktor1 / 10.23;
            w.shift1 = ((((511.5 - shift1) / faktor1) * w.faktor1) + 40.0) * -1.0;
            w.faktor2 = faktor2 / 10.23;
            w.shift2 = ((((511.5 - shift2) / faktor2) * w.faktor2) + 40.0) * -1.0;
          }else{
            w.faktor1 = faktor1;
            w.shift1 = shift1;
            w.faktor2 = faktor2;
            w.shift2 = shift2;
          }
        break;
        //lin
        case 2:
          if(!v305){
            w.faktor1 = faktor1 / 9.3;
            w.shift1 = ((((465.0 - shift1) / faktor1) * w.faktor1) + 50.0) * -1.0;
            w.faktor2 = faktor2 / 9.3;
            w.shift2 = ((((465.0 - shift2) / faktor2) * w.faktor2) + 50.0) * -1.0;
          }else{
            w.faktor1 = faktor1;
            w.shift1 = shift1;
            w.faktor2 = faktor2;
            w.shift2 = shift2;
          }
        break;
      }
      switch(i){
        case 0:
          w.eoperatemode = eGainSweep;
          w.linear1 = false;
          w.linear2 = false;
        break;
        case 1:  
        case 2:  
        case 3:  
        case 4:  
        case 5:  
        //Falsche Kalibrierdaten
          QMessageBox::warning( this, tr("SA Calibrationfile",""), 
                                      tr("<b><br>This the wrong mode !</b><br><br>The Graphfile must be created in SweepMode  by the Log input.<br>",""));
        return;
        break;
      }
      for(i=0; i<w.cntpoints; i++){
        ts >> w.mess.k1[i];
      }
      for(i=0; i<w.cntpoints; i++){
        ts >> w.mess.k2[i];
      }
      f.close();
      beschr = tr("<b><br>Creating Calibrationfile :</b><br><br>Define Frequency limits !<br><br><br>Set Startfrequency : ","");
      if(w.anfangsfrequenz <= 2000000.0){
        frequenz = double (ogrunddaten.frqa1);
      }else{
        frequenz = double (ogrunddaten.frqa2);
      }
      //Ueberpruefung der Anfangsfrequenz in der Kurvendatei
      if(frequenz < w.anfangsfrequenz)frequenz = w.anfangsfrequenz;
      frequenz = QInputDialog::getDouble(this, tr("SA Calibration",""),
                                        beschr, frequenz, 0.0, 100000000000.0, 0, &ok); 
      if(ok){
        fanfang = frequenz;
      }else{
        return;
      }
      beschr = tr("<b><br>Creating Calibrationfile :</b><br><br>Define Frequency limits !<br><br><br>Set Stopfrequency : ","");
      if(w.anfangsfrequenz <= 2000000.0){
        frequenz = double (ogrunddaten.frqb1);
      }else{
        frequenz = double (ogrunddaten.frqb2);
      }
      if(frequenz > (w.anfangsfrequenz + (w.schrittfrequenz * w.cntpoints)))
        frequenz = w.anfangsfrequenz+ w.schrittfrequenz * w.cntpoints;
      frequenz = QInputDialog::getDouble(this, tr("SA Calibration",""),
                                        beschr, frequenz, 0.0, 100000000000.0, 0, &ok); 
      if(ok){
        fende = frequenz;
      }else{
        return;
      }
      j = 0;
      pegel_0 = 0.0;
      faktor1 = w.faktor1;
      shift1 = w.shift1;
      for(i=0; i<w.cntpoints; i++){
        frequenz = double (w.anfangsfrequenz + (w.schrittfrequenz * i));
        if((frequenz >= fanfang) and (frequenz <= fende)){
          pegel = w.mess.k1[i] * faktor1 + shift1 - getkalibrierwert(frequenz);
          j++;
          pegel_0 = pegel_0 + pegel;
          if(pegel < pegelmin)pegelmin = pegel;
          if(pegel > pegelmax)pegelmax = pegel;
        }
      }
      pegel_0 = pegel_0 / double(j);
      pegelmin = pegelmin - pegel_0;
      pegelmax = pegelmax - pegel_0;
      QString s1,s2,s3,s4;
      s1.sprintf(" %fdB", pegel_0);
      s2.sprintf(" %i", j);
      s3.sprintf(" %fdB", pegelmin);
      s4.sprintf(" %fdB", pegelmax);
      QMessageBox::information( this, tr("SA Calibrationfile",""), 
                                      tr("<b><br>The data is  collected and processed</b><br><br>","") +
                                      tr("Everage level","") + s1 + " <BR>"+
                                      tr("Number of used samples","") + s2 + " <BR>"+
                                      tr("Level deviation from everage","") + s3 + " <BR>"+
                                      tr("Level deviation from everage","") + s4 + " <BR><BR>"+
                                      tr("The data is saved into file \"sav.cal\"!",""));
      //qDebug("Pegeldurchschnitt %fdB ; Messpunkte %i; Pegelabweichung min %f; Pegelabweichung max %f", pegel_0, j, pegelmin, pegelmax);
      calixalt = 0;
      j = 1;
      pegelalt = 0.0;
      double pdelta;
      pegel = 0.0;
      for(i=0; i<w.cntpoints; i++){
        frequenz = double (w.anfangsfrequenz + (w.schrittfrequenz * i));
        if((frequenz >= fanfang) and (frequenz <= fende)){
          //qDebug("Frequenz %f", frequenz);
          calix = int(savcalkonst * log10(frequenz));
          //Grenzen einhalten
          if(calix < 0)calix = 0;
          if(calix > calmaxindex-1)calix = calmaxindex - 1;
          pegel = pegel + (w.mess.k1[i] * faktor1 + shift1 - pegel_0) - getkalibrierwert(frequenz);
          if(calix == calixalt){
            j++; 
          }else{
            pegel = pegel / j;
            pdelta = (pegel - pegelalt) / (calix - calixalt);
            //erster Durchlauf
            if(calixalt == 0){
              calixalt = calix;
              pegelalt = pegel;
            }
            //qDebug("calixalt %i ; calix %i; pegelalt %f; pegel %f; pegeldelta %f", calixalt, calix, pegelalt, pegel, pdelta);
            for(k=calixalt; k<=calix; k++){
              //qDebug("calindex %i; pegel %f; k-calixalt %i", k, pegelalt + pdelta * (k-calixalt), (k-calixalt));
              osavcalarray.p[k] = pegelalt + pdelta * (k-calixalt);
            }
            pegelalt = pegel;
            calixalt = calix;
            j = 1;
            pegel = 0.0;
          }
        }
      }
    }
  }
}

double OptionDlg::getkalibrierwert(double afrequenz)
{
  double j;
  int i;
  
  //qDebug("WidgetWobbeln::getkalibrierwertk1()");
  j = round(afrequenz / eteiler);
  i = int(j);
  if(i > (maxmesspunkte - 1))i = maxmesspunkte - 1; // maximaler index 
  return okalibrierarray.arrayk1[i];
}


void OptionDlg::setkalibrierarray(const TCalibrateArray &akalibrierarray){
  okalibrierarray = akalibrierarray;
}

void OptionDlg::sacalreset(){
  int i;
  
  for(i=0; i < calmaxindex; i++){
    osavcalarray.p[i] = 0.0;
  }
}

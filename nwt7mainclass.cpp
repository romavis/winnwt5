/***************************************************************************
    nwt7mainclass.cpp

    nwt7linux.cpp  -  description
    ----------------------------------------------------
    begin                : Don Aug  7 07:29:43 CEST 2003
    copyright            : (C) 2003 by Andreas Lindenau
    email                : DL4JAL@darc.de

    nwt7linux.cpp  have been refactored to Nwt7MainClass.cpp QT5
    ----------------------------------------------------
    begin                : 13 march 2013
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

#include <QPushButton>
#include <QMenuBar>
#include <QTimer>
#include <QLineEdit>
#include <QLabel>
#include <QSpinBox>
#include <QFileDialog>
#include <QPainter>
#include <QPixmap>
#include <QCheckBox>
#include <QProgressBar>
#include <QDir>
#include <QMessageBox>
#include <QInputDialog>
#include <QDialog>
#include <QPalette>
#include <QColor>
#include <QSettings>
#include <QSize>
#include <iostream>
#include <QScrollArea>
#include <QLCDNumber>


#include <cmath>
#include <math.h>
#include <stdlib.h>

#include "nwt7mainclass.h"
#include "sweepcurvewidget.h"
#include "configfile.h"
#include "optiondlg.h"
#include "fmarkedlg.h"
#include "profildlg.h"
//#include "firmwaredlg.h"
//#include "tooltip.h"
#include "probedlg.h"

extern int appstart;

void nwtDelay(int ms){
    QMutex localDelay;
    QWaitCondition sleepSimulator;
    localDelay.lock();
    sleepSimulator.wait(&localDelay, ms);
    localDelay.unlock();
};

Nwt7MainClass::Nwt7MainClass(QWidget *parent) : QTabWidget(parent)
{

  ///////////////////////////////////////////////////////////////////////////////
  //Generate interface to generate NWT7 for all widgets
  //  picmodem = new QextSerialPort("/dev/ttyS0");
  //  delete picmodem;
  fwportbalt = 0;
  picmodem = new Modem();
  //if(!ttyopen(1)) return;
  //QObject::connect( picmodem, SIGNAL(setTtyText(QString)), this, SLOT(setWindowTitle(QString)));
  bttyOpen = FALSE;
  tab_nwt7sweep=-1;
  tab_nwt7measure=-1;
  tab_nwt7vfo=-1;
  tab_wimpedanz_match=-1;
  tab_calculation=-1;
  tab_wdiagramm=-1;
  tab_wkmanager=-1;

  ///////////////////////////////////////////////////////////////////////////////
  // Set of basic sweepdata just in case
  ///////////////////////////////////////////////////////////////////////////////

  sweepdata.cntpoints = 1000;
  sweepdata.anfangsfrequenz = 1000000.0;
  sweepdata.schrittfrequenz = 29000.0;
  sweepdata.display_shift = 0; //No display correction
  sweepdata.swraufloesung = 4; //Resolution only for swr
  sweepdata.penkanal1 = QPen( Qt::red, 0, Qt::SolidLine);
  sweepdata.penkanal2 = QPen( Qt::green, 0, Qt::SolidLine);
  currentdata.stime = 50;
  currentdata.vtime = 50;
  currentdata.idletime = 300;
  messtime = 20;
  currentdata.pllmodeenable = FALSE;
  sweepdata.mousesperre = TRUE; //Active fields cursor lock
  sweepdata.colorhintergrund = Qt::white;
  KursorFrequenz = 0.0;
  sweepInvalid();

  ///////////////////////////////////////////////////////////////////////////////
  // SWEEP widget
  //////////////////////////////////////////////////////////////////////////////

  grafik = new SweepCurveWidget();
  grafik->hide();
  grafik->setSweepData(sweepdata);
  //Bring windows already in the right position
  readSettings();

  nwt7sweep = new QWidget(this);
  tab_nwt7sweep = addTab(nwt7sweep, tr("Sweep","Menue"));
  //SIGNAL Graphics window handed over to Wobbelfester
  QObject::connect( grafik, SIGNAL(wCurveDelete()), this, SLOT(nkurve_loeschen()));
  //SIGNAL wobble be handed over to the graphic window
  QObject::connect( this, SIGNAL(sendefrqmarken(const TFrqMarkers &)), grafik, SLOT(empfangfrqmarken(const TFrqMarkers &)));
  
  QObject::connect( this, SIGNAL(sendsweepdata(const TSweep&)), grafik, SLOT(SweepDataReceipt(const TSweep&)));
  QObject::connect( this, SIGNAL(sendsweepcurve(const TMessure &)), grafik, SLOT(SweepDataCurve(const TMessure &)));
  //Signal To print the graphic in graphic widget
  QObject::connect( this, SIGNAL(drucken(const QFont &, const QStringList &)), grafik, SLOT(printDiagramm(const QFont &, const QStringList &)));
  //Signal To print the graphic in the widget graphics in PDF
  QObject::connect( this, SIGNAL(druckenpdf(const QFont &, const QStringList &)), grafik, SLOT(printDiagramPdf(const QFont &, const QStringList &)));
  //Signal To save the graph in the widget graphics in PNG
  QObject::connect( this, SIGNAL(bildspeichern(const TBildinfo &, const QStringList &)), grafik, SLOT(bildspeichern(const TBildinfo &, const QStringList &)));
  
  gbSweep = new QGroupBox(tr("Sweep mode Setup ",""),nwt7sweep);
  editanfang = new QLineEdit(gbSweep);
  //All query and editing Syndicate
  QObject::connect( editanfang, SIGNAL(returnPressed ()), this, SLOT(SweepInit()));
  editende = new QLineEdit(gbSweep);
  QObject::connect( editende, SIGNAL(returnPressed ()), this, SLOT(SweepInit()));
  editschritte = new QLineEdit(gbSweep);
  QObject::connect( editschritte, SIGNAL(returnPressed ()), this, SLOT(SweepInit()));
  
  labelzwischenzeit = new QLabel(tr("Delay (Us)",""), gbSweep);
  boxzwischenzeit = new QComboBox(gbSweep);
  boxzwischenzeit->addItem("0");
  boxzwischenzeit->addItem("100");
  boxzwischenzeit->addItem("500");
  boxzwischenzeit->addItem("1000");
  boxzwischenzeit->addItem("2000");
  boxzwischenzeit->addItem("3000");
  boxzwischenzeit->addItem("4000");
  boxzwischenzeit->addItem("5000");
  boxzwischenzeit->addItem("6000");
  boxzwischenzeit->addItem("7000");
  boxzwischenzeit->addItem("8000");
  boxzwischenzeit->addItem("9000");
  boxzwischenzeit->addItem("9990");
  boxzwischenzeit->setCurrentIndex(0);
  QObject::connect( boxzwischenzeit, SIGNAL(currentIndexChanged(int)), this, SLOT(SweepInit()));
  boxzwischenzeit->setEnabled(FALSE);
  labelzwischenzeit->setEnabled(FALSE);
 
  editdisplay = new QLineEdit(gbSweep);
  QObject::connect( editdisplay, SIGNAL(returnPressed ()), this, SLOT(SweepInit()));
  labeldisplayshift = new QLabel(tr("Display-shift",""), gbSweep);
  
  boxprofil = new QComboBox(gbSweep);
  boxprofil->setDuplicatesEnabled (FALSE);
  QObject::connect( boxprofil, SIGNAL( activated(int)), this, SLOT(setProfil(int)));
  
  labelprofil = new QLabel(tr("Profile",""), gbSweep);
  int i;
  for(i=0;i<100;i++)aprofil[i].name = "NN";

  labelfrqfaktor= new QLabel(tr("Frequency multiplication x 1",""), gbSweep);
  
  editschrittweite = new QLineEdit(gbSweep);
  editschrittweite->setEnabled(FALSE);
  QObject::connect( editschrittweite, SIGNAL(returnPressed ()), this, SLOT(SweepInit()));

  labelanfang = new QLabel(tr("Start Freq.(Hz)",""), gbSweep);
  labelende = new QLabel(tr("Stop Freq. (Hz)",""), gbSweep);
  labelschrittweite = new QLabel(tr("Stepsize (Hz)",""), gbSweep);
  labelschritte = new QLabel(tr("Samples",""), gbSweep);
//  checkboxtime = new QCheckBox(groupwobbel);
//  checkboxtime->setText(tr("Scanzeit groesser (Sek)","CheckBox"));
//  checkboxtime->setChecked(FALSE);
//  QObject::connect( checkboxtime, SIGNAL( stateChanged (int)), this, SLOT(checkboxtime_checked(int)));


  //  labelscantime = new QLabel(tr("Scanzeit","label"), groupwobbel);
  
  groupkanal = new QGroupBox(tr("Channel",""),nwt7sweep);
  checkboxk1 = new QCheckBox(groupkanal);
  checkboxk1->setText(tr("Channel 1",""));
  checkboxk1->setChecked(TRUE);
  QObject::connect( checkboxk1, SIGNAL( toggled (bool)), this, SLOT(checkboxk1_checked(bool)));
  
  checkboxk2 = new QCheckBox(groupkanal);
  checkboxk2->setText(tr("Channel 2",""));
  checkboxk2->setChecked(TRUE);
  QObject::connect( checkboxk2, SIGNAL( toggled (bool)), this, SLOT(checkboxk2_checked(bool)));

  groupbandbreite = new QGroupBox(tr("Bandwidth",""),nwt7sweep);
  checkbox3db = new QCheckBox(groupbandbreite);
  checkbox3db->setText("3dB/Q");
  checkbox3db->setChecked(FALSE);
  QObject::connect( checkbox3db, SIGNAL( toggled (bool)), this, SLOT(checkbox3db_checked(bool)));

  checkbox6db = new QCheckBox(groupbandbreite);
  checkbox6db->setText("6dB/60dB/Shape");
  checkbox6db->setChecked(FALSE);
  QObject::connect( checkbox6db, SIGNAL( toggled (bool)), this, SLOT(checkbox6db_checked(bool)));

  checkboxgrafik = new QCheckBox(groupbandbreite);
  checkboxgrafik->setText(tr("Marker lines",""));
  checkboxgrafik->setChecked(FALSE);
  QObject::connect( checkboxgrafik, SIGNAL( toggled (bool)), this, SLOT(checkboxgrafik_checked(bool)));

  checkboxinvers = new QCheckBox(groupbandbreite);
  checkboxinvers->setText(tr("Inverse",""));
  checkboxinvers->setChecked(FALSE);
  QObject::connect( checkboxinvers, SIGNAL( toggled (bool)), this, SLOT(checkboxinvers_checked(bool)));

  gbOperatedStart = new QGroupBox(tr("Mode",""),nwt7sweep);
  cbOperatedMode = new QComboBox(gbOperatedStart);
  cbOperatedMode->addItem(tr("Sweep",""));
  cbOperatedMode->addItem(tr("SWR",""));
  cbOperatedMode->addItem(tr("SWR_Ant",""));
  cbOperatedMode->addItem(tr("Impedance-|Z|",""));
  cbOperatedMode->addItem(tr("Spectrumanalyser",""));
  cbOperatedMode->addItem(tr("Spectr.Freq.shift",""));
  QObject::connect( cbOperatedMode, SIGNAL( currentIndexChanged(int)), this, SLOT(setOperateMode(int)));
  labelbetriebsart = new QLabel("",gbOperatedStart);
  
  kalibrier1 = new QCheckBox(gbOperatedStart);
  kalibrier1->setText(tr("Math. Corr. Channel1",""));
  QObject::connect( kalibrier1, SIGNAL( toggled (bool)), this, SLOT(kalibrier1_checked(bool)));
  kalibrier2 = new QCheckBox(gbOperatedStart);
  kalibrier2->setText(tr("Math. Corr. Channel2",""));
  QObject::connect( kalibrier2, SIGNAL( toggled (bool)), this, SLOT(kalibrier2_checked(bool)));

  cbResolutionSWR = new QComboBox(gbOperatedStart);
  cbResolutionSWR->addItem(tr("default",""));
  cbResolutionSWR->addItem(tr("max SWR 2.0",""));
  cbResolutionSWR->addItem(tr("max SWR 3.0",""));
  cbResolutionSWR->addItem(tr("max SWR 4.0",""));
  cbResolutionSWR->addItem(tr("max SWR 5.0",""));
  cbResolutionSWR->addItem(tr("max SWR 6.0",""));
  cbResolutionSWR->addItem(tr("max SWR 7.0",""));
  cbResolutionSWR->addItem(tr("max SWR 8.0",""));
  cbResolutionSWR->addItem(tr("max SWR 9.0",""));
  cbResolutionSWR->addItem(tr("max SWR 10.0",""));
  QObject::connect( cbResolutionSWR, SIGNAL( currentIndexChanged(int)), this, SLOT(setAufloesung(int)));
  labelaufloesung = new QLabel(tr("SWR Range",""),gbOperatedStart);
  cbResolutionSWR->hide();
  labelaufloesung->hide();
  edita_100 = new QLineEdit(gbOperatedStart);
  QObject::connect( edita_100, SIGNAL(textChanged ( const QString & )), this, SLOT(swrantaendern()));
  edita_100->hide();
  labela_100 = new QLabel(tr("Attenuation (100m)",""),gbOperatedStart);
  labela_100->hide();
  editkabellaenge = new QLineEdit(gbOperatedStart);
  QObject::connect( editkabellaenge, SIGNAL(textChanged ( const QString & )), this, SLOT(swrantaendern()));
  editkabellaenge->hide();
  labelkabellaenge = new QLabel(tr("Cable length (m)",""),gbOperatedStart);
  labelkabellaenge->hide();
  groupsa = new QGroupBox(tr("Status",""),gbOperatedStart);
  groupsa->hide();
  labelsafrqbereich = new QLabel(tr("Range",""),groupsa);
  labelsafrqbereich->hide();
  labelsabandbreite = new QLabel(tr("Bandwidth:",""),groupsa);
  labelsabandbreite->hide();
  sabereich1 = new QRadioButton(tr("Range 1",""),groupsa);
  QObject::connect( sabereich1, SIGNAL(clicked()), this, SLOT(setSaBereich1() ));
  sabereich1->setChecked(TRUE);
  sabereich1->hide();
  sabereich2 = new QRadioButton(tr("Range 2",""),groupsa);
  QObject::connect( sabereich2, SIGNAL(clicked()), this, SLOT(setSaBereich2() ));
  sabereich2->setChecked(FALSE);
  sabereich2->hide();

  groupshift = new QGroupBox(tr("Y-axis Scale and Shift",""),nwt7sweep);
  labelboxydbmax = new QLabel(tr("Ymax dB",""),groupshift);
  labelboxydbmin = new QLabel(tr("Ymin dB",""),groupshift);
  boxydbmax = new QComboBox(groupshift);
  boxydbmax->addItem("30");
  boxydbmax->addItem("20");
  boxydbmax->addItem("10");
  boxydbmax->addItem("0");
  boxydbmax->addItem("-10");
  boxydbmax->addItem("-20");
  boxydbmax->addItem("-30");
  boxydbmax->addItem("-40");
  boxydbmax->addItem("-50");
  boxydbmax->addItem("-60");
  boxydbmax->addItem("-70");
  boxydbmin = new QComboBox(groupshift);
  boxydbmin->addItem("-10");
  boxydbmin->addItem("-20");
  boxydbmin->addItem("-30");
  boxydbmin->addItem("-40");
  boxydbmin->addItem("-50");
  boxydbmin->addItem("-60");
  boxydbmin->addItem("-70");
  boxydbmin->addItem("-80");
  boxydbmin->addItem("-90");
  boxydbmin->addItem("-100");
  boxydbmin->addItem("-110");
  boxydbmin->addItem("-120");
  boxydbmax->setCurrentIndex(2);
  boxydbmin->setCurrentIndex(7);
  QObject::connect( boxydbmax, SIGNAL(currentIndexChanged (const QString)), this, SLOT(setDisplayYmax(const QString)));
  QObject::connect( boxydbmin, SIGNAL(currentIndexChanged (const QString)), this, SLOT(setDisplayYmin(const QString)));
  labelboxdbshift1 = new QLabel(tr("Ch1-dB",""),groupshift);
  labelboxdbshift2 = new QLabel(tr("Ch2-dB",""),groupshift);
  boxdbshift1 = new QComboBox(groupshift);
  boxdbshift1->setEditable(TRUE);
  boxdbshift1->addItem("20");
  boxdbshift1->addItem("19");
  boxdbshift1->addItem("18");
  boxdbshift1->addItem("17");
  boxdbshift1->addItem("16");
  boxdbshift1->addItem("15");
  boxdbshift1->addItem("14");
  boxdbshift1->addItem("13");
  boxdbshift1->addItem("12");
  boxdbshift1->addItem("11");
  boxdbshift1->addItem("10");
  boxdbshift1->addItem("9");
  boxdbshift1->addItem("8");
  boxdbshift1->addItem("7");
  boxdbshift1->addItem("6");
  boxdbshift1->addItem("5");
  boxdbshift1->addItem("4");
  boxdbshift1->addItem("3");
  boxdbshift1->addItem("2");
  boxdbshift1->addItem("1");
  boxdbshift1->addItem("0");
  boxdbshift1->addItem("-1");
  boxdbshift1->addItem("-2");
  boxdbshift1->addItem("-3");
  boxdbshift1->addItem("-4");
  boxdbshift1->addItem("-5");
  boxdbshift1->addItem("-6");
  boxdbshift1->addItem("-7");
  boxdbshift1->addItem("-8");
  boxdbshift1->addItem("-9");
  boxdbshift1->addItem("-10");
  boxdbshift1->setCurrentIndex(20);
  boxdbshift2 = new QComboBox(groupshift);
  boxdbshift2->setEditable(TRUE);
  boxdbshift2->addItem("20");
  boxdbshift2->addItem("19");
  boxdbshift2->addItem("18");
  boxdbshift2->addItem("17");
  boxdbshift2->addItem("16");
  boxdbshift2->addItem("15");
  boxdbshift2->addItem("14");
  boxdbshift2->addItem("13");
  boxdbshift2->addItem("12");
  boxdbshift2->addItem("11");
  boxdbshift2->addItem("10");
  boxdbshift2->addItem("9");
  boxdbshift2->addItem("8");
  boxdbshift2->addItem("7");
  boxdbshift2->addItem("6");
  boxdbshift2->addItem("5");
  boxdbshift2->addItem("4");
  boxdbshift2->addItem("3");
  boxdbshift2->addItem("2");
  boxdbshift2->addItem("1");
  boxdbshift2->addItem("0");
  boxdbshift2->addItem("-1");
  boxdbshift2->addItem("-2");
  boxdbshift2->addItem("-3");
  boxdbshift2->addItem("-4");
  boxdbshift2->addItem("-5");
  boxdbshift2->addItem("-6");
  boxdbshift2->addItem("-7");
  boxdbshift2->addItem("-8");
  boxdbshift2->addItem("-9");
  boxdbshift2->addItem("-10");
  boxdbshift2->setCurrentIndex(20);
  QObject::connect( boxdbshift1, SIGNAL( currentIndexChanged(int)), this, SLOT(setShift()));
  QObject::connect( boxdbshift2, SIGNAL( currentIndexChanged(int)), this, SLOT(setShift()));
  
  //  groupkursor = new QGroupBox(tr("Kursor-Nummer","GroupBox"),nwt7wobbeln);
  labelkursornr = new QLabel(tr("Cursor #",""),groupshift);
  boxkursornr = new QComboBox(groupshift);
  boxkursornr->addItem("5");
  boxkursornr->addItem("4");
  boxkursornr->addItem("3");
  boxkursornr->addItem("2");
  boxkursornr->addItem("1");
  boxkursornr->setCurrentIndex(4);
  QObject::connect( boxkursornr, SIGNAL( currentIndexChanged(int)), grafik, SLOT(setWCursorNr(int)));
  QObject::connect( grafik, SIGNAL( resetCursor(int)), this, SLOT(resetCursor(int)));
  
  gbAttenuator = new QGroupBox(tr("Attenuation",""),nwt7sweep);
  cbAttenuator1 = new QComboBox(gbAttenuator);
  //QObject::connect( cbAttenuator1, SIGNAL( currentIndexChanged(int)), this, SLOT(setAttenuationSweep(int)));
  QObject::connect( cbAttenuator1, SIGNAL( currentIndexChanged(int)), this, SLOT(setAttenuationControlsIndex(int)));

  int maxAtten = currentdata.attenuator.GetMaxAttenuation();
  labeldaempfung = new QLabel(tr("0db/-%1dB","").arg(maxAtten), gbAttenuator);
  
  sweepdata.bandbreite3db=FALSE;
  sweepdata.bandbreite6db=FALSE;

  if(checkboxk1->isChecked())sweepdata.bkanal1=TRUE;
  if(checkboxk2->isChecked())sweepdata.bkanal2=TRUE;
  if(checkbox3db->isChecked())sweepdata.bandbreite3db=TRUE;
  if(checkbox6db->isChecked())sweepdata.bandbreite6db=TRUE;

  buttonwobbeln = new QPushButton(tr("Sweep","Button"), nwt7sweep);
  buttonwobbeln->setEnabled(TRUE);
  QObject::connect( buttonwobbeln, SIGNAL( clicked()), this, SLOT(clickSweepContinuous()));
  QObject::connect( grafik, SIGNAL( SweepTimes()), this, SLOT(clickSweepContinuous()));
  
  buttoneinmal = new QPushButton(tr("Single","Button"), nwt7sweep);
  buttoneinmal->setEnabled(TRUE);
  QObject::connect( buttoneinmal, SIGNAL( clicked()), this, SLOT(clickSweepOnce()));
  QObject::connect( grafik, SIGNAL( SweepOnce()), this, SLOT(clickSweepOnce()));
  
  buttonstop = new QPushButton(tr("Stop","Button"), nwt7sweep);
  buttonstop->setEnabled(FALSE);
  QObject::connect( buttonstop, SIGNAL( clicked()), this, SLOT(clickSweepStop()));
  QObject::connect( grafik, SIGNAL( SweepStop()), this, SLOT(clickSweepStop()));
  
  gbZoom = new QGroupBox(tr("Frequency Zoom",""),nwt7sweep);
  labellupe = new QLabel(tr("2x Zoom +/-",""),gbZoom);
  labellupe->setEnabled(FALSE);
  buttonlupeplus = new QPushButton("+", gbZoom);
  buttonlupeplus->setEnabled(FALSE);
  QObject::connect( buttonlupeplus, SIGNAL( clicked()), this, SLOT(clicklMagnifyPlus()));
  QObject::connect( grafik, SIGNAL( MagnifyPlus()), this, SLOT(clicklMagnifyPlus()));
  buttonlupeminus = new QPushButton("-", gbZoom);
  buttonlupeminus->setEnabled(FALSE);
  QObject::connect( buttonlupeminus, SIGNAL( clicked()), this, SLOT(clicklMagnifyMinus()));
  buttonlupemitte = new QPushButton("^", gbZoom);
  buttonlupemitte->setEnabled(FALSE);
  QObject::connect( buttonlupemitte, SIGNAL( clicked()), this, SLOT(clicklMagnifyEqual()));
  QObject::connect( this, SIGNAL( cursormitte()), grafik, SLOT(setMouseCursorCenter()));
  
  QObject::connect( grafik, SIGNAL( MagnifyMinus()), this, SLOT(clicklMagnifyMinus()));
  QObject::connect( grafik, SIGNAL( MagnifyEqual()), this, SLOT(clicklMagnifyEqual()));
  
  //groupbar = new QGroupBox("NWT",nwt7wobbeln);
  progressbar = new QProgressBar(groupshift);
  progressbar->setTextVisible(FALSE);
  labelprogressbar = new QLabel(tr("Progress",""),groupshift);
  labelnwt = new QLabel("Offline",groupshift);

  mledit = new QTextEdit(nwt7sweep);
  //mledit->setReadOnly(TRUE);
  //Signal vom Widget Grafik Loeschen des Multieditfenster
  QObject::connect( grafik, SIGNAL( multiEditDelete()), this, SLOT(mleditloeschen()));
  //Signal vom Widget Grafik Einfuegen von Text ins Multieditfenster
  QObject::connect( grafik, SIGNAL( multiEditInsert(const QString &)), this, SLOT(mleditinsert(const QString &)));
  //mousezeiger bestimmt neue Anfangsfrequenz
  QObject::connect( grafik, SIGNAL( writeBegin(double)), this, SLOT(setFqStart(double)));
  //mousezeiger bestimmt neue Endfrequenz
  QObject::connect( grafik, SIGNAL( writeEnd(double)), this, SLOT(setFqEnd(double)));
  QObject::connect( grafik, SIGNAL( writeCalculation(double)), this, SLOT(readberechnung(double)));
  QObject::connect( grafik, SIGNAL( setCursorFrequency(double)), this, SLOT(setCursorFrequency(double)));
  
  vsweeptimer = new QTimer(this);
  //Signal by sweeptimer (reading the RS232)
  QObject::connect(vsweeptimer, SIGNAL(timeout()), this, SLOT(SweepTimerPerform() ));
  
  idletimer = new QTimer(this);
  //idle HW
  QObject::connect( idletimer, SIGNAL(timeout()), this, SLOT(IdleTimerPerform() ));
  
  vtimer = new QTimer(this);
  //(Reading the RS232) FW-Version
  QObject::connect(vtimer, SIGNAL(timeout()), this, SLOT(vTimerPerform() ));
  
  stimer = new QTimer(this);
  //(Reading the RS232) Status
  QObject::connect(stimer, SIGNAL(timeout()), this, SLOT(sTimerPerform() ));
  
///////////////////////////////////////////////////////////////////////////////
// WKMANAGER Widget
///////////////////////////////////////////////////////////////////////////////
  wkmanager = new MarkersDlg(this);
  tab_wkmanager=addTab(wkmanager, tr("Graph manager",""));
  //Verbindung vom Wobbelmanager zum Grafikfenster fuer die Wobbeldaten
  QObject::connect(wkmanager, SIGNAL(wkmsendwobbel(const TSweep &)), grafik, SLOT(setCurve(const TSweep &)));
  //verbindung zum holen der Wobbeldaten aus dem Hauptfenster zum Wobbelmanager
  QObject::connect(wkmanager, SIGNAL(loadkurve(int)), this, SLOT(wkmLoadCurve(int)));
  QObject::connect(wkmanager, SIGNAL(savekurve(const TSweep&)), this, SLOT(wkmSaveCurve(const TSweep&)));
  QObject::connect(wkmanager, SIGNAL(wkmgetwobbel(int)), this, SLOT(getwkm(int)));
//  connect(wkmanager, SIGNAL(setkurvendir(QString)), this, SLOT(getkdir(QString)));
//  connect(wkmanager, SIGNAL(warneichkorr()), this, SLOT(warneichkorr()));
  
///////////////////////////////////////////////////////////////////////////////
// VFO Widget
///////////////////////////////////////////////////////////////////////////////

  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  lcdauswahl = 1;
  nwt7vfo = new QWidget();
  tab_nwt7vfo = addTab(nwt7vfo, tr("VFO",""));
  
  QFont font("Helvetica", 16, QFont::Bold);
  
  vsp1hz = new QSpinBox(nwt7vfo);
  vsp1hz->setRange(-1, 10);
  vsp1hz->setFont(font);
  vsp1hz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp1hz, SIGNAL( valueChanged(int)), this, SLOT(change1hz(int)));
  vsp10hz = new QSpinBox(nwt7vfo);
  vsp10hz->setFont(font);  
  vsp10hz->setRange(-1, 10);
  vsp10hz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp10hz, SIGNAL( valueChanged(int)), this, SLOT(change10hz(int)));
  vsp100hz = new QSpinBox(nwt7vfo);
  vsp100hz->setRange(-1, 10);
  vsp100hz->setFont(font);
  vsp100hz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp100hz, SIGNAL( valueChanged(int)), this, SLOT(change100hz(int)));
  vsp1khz = new QSpinBox(nwt7vfo);
  vsp1khz->setRange(-1, 10);
  vsp1khz->setFont(font);
  vsp1khz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp1khz, SIGNAL( valueChanged(int)), this, SLOT(change1khz(int)));
  vsp10khz = new QSpinBox(nwt7vfo);
  vsp10khz->setRange(-1, 10);
  vsp10khz->setFont(font);
  vsp10khz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp10khz, SIGNAL( valueChanged(int)), this, SLOT(change10khz(int)));
  vsp100khz = new QSpinBox(nwt7vfo);
  vsp100khz->setRange(-1, 10);
  vsp100khz->setFont(font);  
  vsp100khz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp100khz, SIGNAL( valueChanged(int)), this, SLOT(change100khz(int)));
  vsp1mhz = new QSpinBox(nwt7vfo);
  vsp1mhz->setRange(-1, 10);
  vsp1mhz->setFont(font);
  vsp1mhz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp1mhz, SIGNAL( valueChanged(int)), this, SLOT(change1mhz(int)));
  vsp10mhz = new QSpinBox(nwt7vfo);
  vsp10mhz->setRange(-1, 10);
  vsp10mhz->setFont(font);
  vsp10mhz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp10mhz, SIGNAL( valueChanged(int)), this, SLOT(change10mhz(int)));
  vsp100mhz = new QSpinBox(nwt7vfo);
  vsp100mhz->setRange(-1, 10);
  vsp100mhz->setFont(font);
  vsp100mhz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp100mhz, SIGNAL( valueChanged(int)), this, SLOT(change100mhz(int)));
  vsp1ghz = new QSpinBox(nwt7vfo);
  vsp1ghz->setRange(-1, 10);
  vsp1ghz->setFont(font);
  vsp1ghz->findChild<QLineEdit*>()->setReadOnly(true);
  QObject::connect( vsp1ghz, SIGNAL( valueChanged(int)), this, SLOT(change1ghz(int)));
  
  editvfo = new QLineEdit(nwt7vfo);
  QObject::connect( editvfo, SIGNAL(returnPressed ()), this, SLOT(editVFO()));
  labelvfo = new QLabel(tr("in Hz"),nwt7vfo);

  lmhz = new QLabel("MHz",nwt7vfo);
  lmhz->setFont(font);  
  lkhz = new QLabel("kHz",nwt7vfo);
  lkhz->setFont(font);  
  lhz = new QLabel("Hz",nwt7vfo);
  lhz->setFont(font);  
  
  labelfrqfaktorv= new QLabel(tr("Frequency multiplication x 1",""), nwt7vfo); 

  //LCD Anzeige 10 stellen mit punkt
  LCD1 = new QLCDNumber(10, nwt7vfo);//, "LCD1");
  //Hintergrung gelb
  LCD1->setPalette(dp);
  LCD1->setAutoFillBackground(TRUE);
  LCD1->setBackgroundRole( QPalette::Background );
  
  //volle schwarze Zahlen
  LCD1->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD1->setSmallDecimalPoint(TRUE);
  rb1 = new QRadioButton(nwt7vfo);
  rb1->setChecked(TRUE);
  QObject::connect( rb1, SIGNAL(clicked()), this, SLOT(lcd1clicked() ));
  //LCD Anzeige 10 stellen mit punkt
  LCD2 = new QLCDNumber(10, nwt7vfo);//, "LCD2");
  //Hintergrung gelb
  LCD2->setPalette(dpd);
  LCD2->setAutoFillBackground(TRUE);
  LCD2->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD2->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD2->setSmallDecimalPoint(TRUE);
  rb2 = new QRadioButton(nwt7vfo);
  QObject::connect( rb2, SIGNAL(clicked()), this, SLOT(lcd2clicked() ));

  //LCD Anzeige 10 stellen mit punkt
  LCD3 = new QLCDNumber(10, nwt7vfo);//, "LCD3");
  //Hintergrung gelb
  LCD3->setPalette(dpd);
  LCD3->setAutoFillBackground(TRUE);
  LCD3->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD3->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD3->setSmallDecimalPoint(TRUE);
  rb3 = new QRadioButton(nwt7vfo);
  QObject::connect( rb3, SIGNAL(clicked()), this, SLOT(lcd3clicked() ));

  //LCD Anzeige 10 stellen mit punkt
  LCD4 = new QLCDNumber(10, nwt7vfo);//, "LCD4");
  //Hintergrung gelb
  LCD4->setPalette(dpd);
  LCD4->setAutoFillBackground(TRUE);
  LCD4->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD4->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD4->setSmallDecimalPoint(TRUE);
  rb4 = new QRadioButton(nwt7vfo);
  QObject::connect( rb4, SIGNAL(clicked()), this, SLOT(lcd4clicked() ));

  //LCD Anzeige 10 stellen mit punkt
  LCD5 = new QLCDNumber(10, nwt7vfo);//, "LCD5");
  //Hintergrung gelb
  LCD5->setPalette(dpd);
  LCD5->setAutoFillBackground(TRUE);
  LCD5->setBackgroundRole( QPalette::Background );
  //volle schwarze Zahlen
  LCD5->setSegmentStyle(QLCDNumber::Filled);
  //der Dezimalpunkt soll ganz wenig platz beanspruchen
  LCD5->setSmallDecimalPoint(TRUE);
  rb5 = new QRadioButton(nwt7vfo);
  QObject::connect( rb5, SIGNAL(clicked()), this, SLOT(lcd5clicked() ));

  editzf = new QLineEdit(nwt7vfo);
  QObject::connect( editzf, SIGNAL(returnPressed()), this, SLOT(setVfoToOutOnce() ));
  labelzf = new QLabel(nwt7vfo);
  labelzf->setText(tr("IF"));
  labelhz = new QLabel(nwt7vfo);
  labelhz->setText("Hz");
  checkboxzf = new QCheckBox(nwt7vfo);
  checkboxzf->setText(tr("Set  IF for Sweeping",""));
  checkboxzf->setChecked(FALSE);
  //the checkbox set IF only for sweep
  //QObject::connect( checkboxzf, SIGNAL(clicked()), this, SLOT(setIQVFO() ));
  checkboxiqvfo = new QCheckBox(nwt7vfo);
  checkboxiqvfo->setText(tr("VFO-Frequency x4 for I/Q Mixer",""));
  checkboxiqvfo->setChecked(FALSE);
  QObject::connect( checkboxiqvfo, SIGNAL(clicked()), this, SLOT(setIQVFO() ));
  
  cbAttenuator2 = new QComboBox(nwt7vfo);
  QObject::connect( cbAttenuator2, SIGNAL( currentIndexChanged(int)), this, SLOT(setAttenuationControlsIndex(int)));
  labeldaempfung1 = new QLabel(tr("Attenuator",""),nwt7vfo);

  vfotimer = new QTimer(this);
  //nach ablauf des vfotimers Daten an NWT7 senden (LCD daten)
  QObject::connect( vfotimer, SIGNAL(timeout()), this, SLOT(VfoTimerPerform() ));

  //erste Diplayeinstellung
  LCDaendern();

  ///////////////////////////////////////////////////////////////////////////////
  // Messure Power Widget
  //////////////////////////////////////////////////////////////////////////////

  nwt7measure = new QWidget(this);
  tab_nwt7measure=addTab(nwt7measure, tr("Wattmeter",""));

  progressbaPowerMeter1 = new QProgressBar(nwt7measure);
  progressbaPowerMeter1->setFormat("");
  mlabelk1 = new QLabel(nwt7measure);
  mlabelk1->setText("0");
  progressbaPowerMeter2 = new QProgressBar(nwt7measure);
  progressbaPowerMeter2->setFormat("");
  mlabelk2 = new QLabel(nwt7measure);
  mlabelk2->setText("0");

  ldampingk1 = new QComboBox(nwt7measure);
  ldampingk1->setEditable(TRUE);
  labelldaempfungk1 = new QLabel(tr("Attenuation (dB)",""),nwt7measure);

  ldaempfungk2 = new QComboBox(nwt7measure);
  ldaempfungk2->setEditable(TRUE);
  labelldaempfungk2 = new QLabel(tr("Attenuation (dB)",""),nwt7measure);

  vmesstimer = new QTimer(this);
  messlabel1 = new QLabel(nwt7measure);
  messlabel2 = new QLabel(nwt7measure);
  messlabel3 = new QLabel(nwt7measure);
  messlabel4 = new QLabel(nwt7measure);
  messlabel5 = new QLabel(nwt7measure);
  messlabel6 = new QLabel(nwt7measure);
  messedit = new QTextEdit(nwt7measure);
  //Schrifttyp einstellen
  messedit-> setFontFamily("courier");
  buttonmess = new QPushButton(tr("Write to Table",""), nwt7measure);
  QObject::connect( buttonmess, SIGNAL( clicked()), this, SLOT(writeMessureToTable()));
  buttonmesssave = new QPushButton(tr("Save data",""), nwt7measure);
  QObject::connect( buttonmesssave, SIGNAL( clicked()), this, SLOT(saveTableToFile()));
  //nach ablauf des messtimers Daten vom nwt7 holen
  QObject::connect( vmesstimer, SIGNAL(timeout()), this, SLOT(vMessTimerPerform() ));
  measureCounter = 0;
  measureValCh1 = 0.0;
  measureValCh2 = 0.0;
  measureCh1Peak = 0.0;
  measureCh2Peak = 0.0;
  boxwattoffset1 = new QComboBox(nwt7measure);
  boxwattoffset2 = new QComboBox(nwt7measure);
  QString snum;
  for(i=0;i<30;i++)
  {
    wattoffsetk1[i].str_offsetwatt = "NN";
    wattoffsetk1[i].offsetwatt = 0.0;
    snum.sprintf("%2i| ",i+1);
    boxwattoffset1->addItem(snum + wattoffsetk1[i].str_offsetwatt);
    wattoffsetk2[i].str_offsetwatt = "NN";
    wattoffsetk2[i].offsetwatt = 0.0;
    boxwattoffset2->addItem(snum + wattoffsetk2[i].str_offsetwatt);
  }
  woffset1 = 0.0;
  woffset2 = 0.0;
  
//  buttonvfo = new QPushButton(tr("VFO on/off",""), nwt7measure);
//  buttonwobbeln->setEnabled(TRUE);
//  QObject::connect( buttonvfo, SIGNAL( clicked()), this, SLOT(setMessureVfoMode(bool)));

  labellMessVfoState = new QLabel(tr("Output state","Output label Messure widget"),nwt7measure);

  rbVfoOn = new QRadioButton(tr("VFO","Output state Messure widget"), nwt7measure);
  rbMessVfoOn = new QRadioButton(tr("Wattmeter VFO","Output state Messure widget"), nwt7measure);
  rbVFOSOff = new QRadioButton(tr("VFO Off","Output state Messure widget"), nwt7measure);
  rbVFOSOff -> setChecked(true);
  QObject::connect( rbVfoOn, SIGNAL(clicked(bool)), this, SLOT(setMessureVfoMode(bool)));
  QObject::connect( rbMessVfoOn, SIGNAL(clicked(bool)), this, SLOT(setMessureVfoMode(bool)));
  QObject::connect( rbVFOSOff, SIGNAL(clicked(bool)), this, SLOT(setMessureVfoMode(bool)));

  labelfrqfaktorm= new QLabel(tr("Frequency multiplication x 1",""), nwt7measure);

  sp1hz = new QSpinBox(nwt7measure);
  sp1hz->setFont(font);  
  sp1hz->setRange(-1, 10);
  sp1hz->hide();
  QObject::connect( sp1hz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp10hz = new QSpinBox(nwt7measure);
  sp10hz->setFont(font);  
  sp10hz->setRange(-1, 10);
  sp10hz->hide();
  QObject::connect( sp10hz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp100hz = new QSpinBox(nwt7measure);
  sp100hz->setFont(font);  
  sp100hz->setRange(-1, 10);
  sp100hz->hide();
  QObject::connect( sp100hz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp1khz = new QSpinBox(nwt7measure);
  sp1khz->setFont(font);  
  sp1khz->setRange(-1, 10);
  sp1khz->hide();
  QObject::connect( sp1khz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp10khz = new QSpinBox(nwt7measure);
  sp10khz->setFont(font);  
  sp10khz->setRange(-1, 10);
  sp10khz->hide();
  QObject::connect( sp10khz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp100khz = new QSpinBox(nwt7measure);
  sp100khz->setFont(font);  
  sp100khz->setRange(-1, 10);
  sp100khz->hide();
  QObject::connect( sp100khz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp1mhz = new QSpinBox(nwt7measure);
  sp1mhz->setFont(font);  
  sp1mhz->setRange(-1, 10);
  sp1mhz->hide();
  QObject::connect( sp1mhz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp10mhz = new QSpinBox(nwt7measure);
  sp10mhz->setFont(font);  
  sp10mhz->setRange(-1, 10);
  sp10mhz->hide();
  QObject::connect( sp10mhz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp100mhz = new QSpinBox(nwt7measure);
  sp100mhz->setFont(font);  
  sp100mhz->setRange(-1, 10);
  sp100mhz->hide();
  QObject::connect( sp100mhz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  sp1ghz = new QSpinBox(nwt7measure);
  sp1ghz->setFont(font);  
  sp1ghz->setRange(-1, 9);
  sp1ghz->hide();
  QObject::connect( sp1ghz, SIGNAL( valueChanged(int)), this, SLOT(setMeasuredVfoFqValue()));
  labelsphz = new QLabel("Hz", nwt7measure);
  labelsphz->setFont(font);  
  labelspkhz = new QLabel("kHz", nwt7measure);
  labelspkhz->setFont(font);  
  labelspmhz = new QLabel("MHz", nwt7measure);
  labelspmhz->setFont(font);  
//  labelspghz = new QLabel("GHz", nwt7messen);
//  labelspghz->setFont(font);  
  labelsphz->hide();
  labelspkhz->hide();
  labelspmhz->hide();
//  labelspghz->hide();
  cbAttenuator3 = new QComboBox(nwt7measure);
  QObject::connect( cbAttenuator3, SIGNAL( currentIndexChanged(int)), this, SLOT(setAttenuationControlsIndex(int)));
  labeldaempfung3 = new QLabel(tr("Attenuation",""),nwt7measure);
  cbAttenuator3->hide();
  labeldaempfung3->hide();

  vfomesstimer = new QTimer(this);
  //Signal vom Wobbeltimer (Auslesen der RS232)
  //messvfotimerende Sobol A.E.
  QObject::connect(vfomesstimer, SIGNAL(timeout()), this, SLOT(VfoMessTimerPerform()));

  checkboxmesshang1 = new QCheckBox(nwt7measure);
  checkboxmesshang1->setText(tr("Slope",""));
  checkboxmesshang1->setChecked(TRUE);
  
  checkboxmesshang2 = new QCheckBox(nwt7measure);
  checkboxmesshang2->setText(tr("Slope",""));
  checkboxmesshang2->setChecked(TRUE);

  ///////////////////////////////////////////////////////////////////////////////
  // Calculations Widget
  ///////////////////////////////////////////////////////////////////////////////

  berechnung = new QWidget();
  tab_calculation=addTab(berechnung, tr("Calculations",""));
  groupschwingkreisc = new QGroupBox(tr("Capacitor for Resonant circuit calculation",""),berechnung);
  editf2 = new QLineEdit(groupschwingkreisc);
  QObject::connect( editf2, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisfc()));
  editl2 = new QLineEdit(groupschwingkreisc);
  QObject::connect( editl2, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisfc()));
  ergebnisc = new QLabel("", groupschwingkreisc);
  labell2 = new QLabel(tr("Inductance ( uH) ",""), groupschwingkreisc);
  labelf2 = new QLabel(tr("Frequency (MHz)",""), groupschwingkreisc);
  labelc2 = new QLabel(tr("Capacitor (pF)",""), groupschwingkreisc);
  ergebnisxc = new QLabel("", groupschwingkreisc);
  labelxc = new QLabel("Ohm XC", groupschwingkreisc);
  
  groupschwingkreisl = new QGroupBox(tr("Resonantcircuit / AL-value Calculation",""),berechnung);
  editf1 = new QLineEdit(groupschwingkreisl);
  QObject::connect( editf1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisfl()));
  editc1 = new QLineEdit(groupschwingkreisl);
  QObject::connect( editc1, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisfl()));
  editn = new QLineEdit(groupschwingkreisl);
  QObject::connect( editn, SIGNAL(textChanged(QString)), this, SLOT(schwingkreisfl()));
  ergebnisl = new QLabel("", groupschwingkreisl);
  ergebnisal = new QLabel("", groupschwingkreisl);
  labelf1 = new QLabel(tr("Frequency (MHz)",""), groupschwingkreisl);
  labell1 = new QLabel(tr("Inductance ( uH) ",""), groupschwingkreisl);
  labelc1 = new QLabel(tr("Capacitor (pF)",""), groupschwingkreisl);
  labeln = new QLabel(tr("Number of turns (N )",""), groupschwingkreisl);
  labelal = new QLabel(tr("AL nH/Nexp2",""), groupschwingkreisl);
  ergebnisxl = new QLabel("", groupschwingkreisl);
  labelxl = new QLabel("Ohm XL", groupschwingkreisl);
  
  groupwindungen = new QGroupBox(tr("Number of turns calculated from AL-value",""),berechnung);
  edital3 = new QLineEdit(groupwindungen);
  QObject::connect( edital3, SIGNAL(textChanged(QString)), this, SLOT(alwindungen()));
  labeledital3 = new QLabel(tr("AL nH/Nexp2",""), groupwindungen);
  editl3 = new QLineEdit(groupwindungen);
  QObject::connect( editl3, SIGNAL(textChanged(QString)), this, SLOT(alwindungen()));
  labeleditl3 = new QLabel(tr("Inductance ( uH) ",""), groupwindungen);
  ergebnisw = new QLabel("", groupwindungen);
  labelergebnisw = new QLabel(tr("Number of turns (N )",""), groupwindungen);

  ///////////////////////////////////////////////////////////////////////////////
  // Impedanz matching Widget
  //////////////////////////////////////////////////////////////////////////////

  wimpedance = new QWidget(this);
  tab_wimpedanz_match=addTab(wimpedance, tr("Impedance matching",""));
  
  gimp = new QGroupBox(tr("Matching type:",""),wimpedance);
  rbr = new QRadioButton(tr("R",""),gimp);
  QObject::connect( rbr, SIGNAL(clicked()), this, SLOT(setimp() ));
  rbr->setChecked(TRUE);
  rblc = new QRadioButton(tr("L/C",""),gimp);
  QObject::connect( rblc, SIGNAL(clicked()), this, SLOT(setimp() ));

  
  gzr = new QGroupBox(tr("Matching with R",""),wimpedance);
  editz1 = new QLineEdit(gzr);
  QObject::connect( editz1, SIGNAL(textChanged(QString)), this, SLOT(zrausrechnen()));
  labeleditz1 = new QLabel(tr("(Ohm) Z1",""), gzr);
  editz2 = new QLineEdit(gzr);
  QObject::connect( editz2, SIGNAL(textChanged(QString)), this, SLOT(zrausrechnen()));
  labeleditz2 = new QLabel(tr("(Ohm) Z2",""), gzr);
  editz3 = new QLineEdit(gzr);
  QObject::connect( editz3, SIGNAL(textChanged(QString)), this, SLOT(zrausrechnen()));
  labeleditz3 = new QLabel(tr("(Ohm) Z3",""), gzr);
  editz4 = new QLineEdit(gzr);
  QObject::connect( editz4, SIGNAL(textChanged(QString)), this, SLOT(zrausrechnen()));
  labeleditz4 = new QLabel(tr("(Ohm) Z4",""), gzr);
  lr1 = new QLabel(tr("xxx",""), gzr);
  lr1->setAlignment(Qt::AlignRight);
  lr2 = new QLabel(tr("xxx",""), gzr);
  lr2->setAlignment(Qt::AlignRight);
  lr3 = new QLabel(tr("xxx",""), gzr);
  lr3->setAlignment(Qt::AlignRight);
  lr4 = new QLabel(tr("xxx",""), gzr);
  lr4->setAlignment(Qt::AlignRight);
  ldaempfung = new QLabel(tr("xxx",""), gzr);
  ldaempfung->setAlignment(Qt::AlignRight);
  lbeschrr1 = new QLabel(tr("(Ohm) R1",""), gzr);
  lbeschrr2 = new QLabel(tr("(Ohm) R2",""), gzr);
  lbeschrr3 = new QLabel(tr("(Ohm) R3",""), gzr);
  lbeschrr4 = new QLabel(tr("(Ohm) R4",""), gzr);
  lbeschrdaempfung = new QLabel(tr("(dB) Insertion Loss",""), gzr);
  bild1 = new QLabel(wimpedance);
  bild1->setPixmap(QPixmap::fromImage(QImage(":/images/zmatch1.png")));
  
  gzlc = new QGroupBox(tr("Matching with LC",""),wimpedance);
  gzlc->hide();
  editzlc1 = new QLineEdit(gzlc);
  QObject::connect( editzlc1, SIGNAL(textChanged(QString)), this, SLOT(zlcausrechnen()));
  labeleditzlc1 = new QLabel(tr("(Ohm) Z1",""), gzlc);
  editzlc2 = new QLineEdit(gzlc);
  QObject::connect( editzlc2, SIGNAL(textChanged(QString)), this, SLOT(zlcausrechnen()));
  labeleditzlc2 = new QLabel(tr("(Ohm) Z2",""), gzlc);
  editzlc3 = new QLineEdit(gzlc);
  QObject::connect( editzlc3, SIGNAL(textChanged(QString)), this, SLOT(zlcausrechnen()));
  labeleditzlc3 = new QLabel(tr("(MHz) Frequency",""), gzlc);
  ll = new QLabel(tr("xxx",""), gzlc);
  ll->setAlignment(Qt::AlignRight);
  lc = new QLabel(tr("xxx",""), gzlc);
  lc->setAlignment(Qt::AlignRight);
  lbeschrl = new QLabel(tr("(uH) L",""), gzlc);
  lbeschrc = new QLabel(tr("(pF) C",""), gzlc);
  bild2 = new QLabel(wimpedance);
  bild2->setPixmap(QPixmap::fromImage(QImage(":/images/zmatch2.png")));
  bild2->hide();
  //  scaleFactor = 1.0;
  // Impedanz Widget Ende
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  // Widget Antennas Diagramm
  /////////////////////////////////////////////////////////////////////////////
  wdiagramm = new QWidget();

  //addTab(wdiagramm, tr("Antenna`s chart"));
  wdiagramm->hide();//On start it is not visible

  gdiagramm = new QGroupBox(tr("Antenna`s chart"), wdiagramm);
  bstart = new QPushButton(tr("Start"),gdiagramm);
  //QObject::connect( bstart, SIGNAL( clicked()), this, SLOT(diagrammsimulieren()));
  QObject::connect( bstart, SIGNAL( clicked()), this, SLOT(diagrammstart()));
  bstop = new QPushButton(tr("Stop"),gdiagramm);
  bstop->setEnabled(FALSE);
  QObject::connect( bstop, SIGNAL( clicked()), this, SLOT(diagrammstop()));
  bsave = new QPushButton(tr("Save CSV"),gdiagramm);
  QObject::connect( bsave, SIGNAL( clicked()), this, SLOT(diagrammspeichern()));

  bsim = new QPushButton(tr("Simmulation"),gdiagramm);
  QObject::connect( bsim, SIGNAL( clicked()), this, SLOT(diagrammsimulieren()));

  lanzeige = new QLabel("xxx", gdiagramm);
  tdiagramm = new QTimer(this);
  QObject::connect( tdiagramm, SIGNAL(timeout()), this, SLOT(tdiagrammbehandlung() ));
  tdiagramm->stop();
  gdiagrammdim = new QGroupBox(tr("Dimension"), wdiagramm);
  boxdbmax = new QComboBox(gdiagrammdim);
  boxdbmax->addItem("10 dB");
  boxdbmax->addItem("0 dB");
  boxdbmax->addItem("-10 dB");
  boxdbmax->addItem("-20 dB");
  boxdbmax->addItem("-30 dB");
  boxdbmax->addItem("-40 dB");
  boxdbmax->addItem("-50 dB");
  boxdbmax->addItem("-70 dB");
  boxdbmax->addItem("-60 dB");
  boxdbmax->addItem("-80 dB");
  boxdbmin = new QComboBox(gdiagrammdim);
  boxdbmin->addItem("10 dB");
  boxdbmin->addItem("0 dB");
  boxdbmin->addItem("-10 dB");
  boxdbmin->addItem("-20 dB");
  boxdbmin->addItem("-30 dB");
  boxdbmin->addItem("-40 dB");
  boxdbmin->addItem("-50 dB");
  boxdbmin->addItem("-70 dB");
  boxdbmin->addItem("-60 dB");
  boxdbmin->addItem("-80 dB");
  ldbmax = new QLabel(tr("max"), gdiagrammdim);
  ldbmin = new QLabel(tr("min"), gdiagrammdim);
  boxdbmax->setCurrentIndex(1);
  boxdbmin->setCurrentIndex(4);

  spinadbegin = new QDoubleSpinBox(gdiagrammdim);
  spinadbegin->setRange(0.0,2.0);
  spinadbegin->setSingleStep(0.01);
  spinadbegin->setValue(0.0);
  spinadend = new QDoubleSpinBox(gdiagrammdim);
  spinadend->setRange(3.0,5.0);
  spinadend->setSingleStep(0.01);
  spinadend->setValue(5.0);
  spingradbegin = new QSpinBox(gdiagrammdim);
  spingradbegin->setRange(0,180);
  spingradbegin->setValue(0);
  spingradend = new QSpinBox(gdiagrammdim);
  spingradend->setRange(180,360);
  spingradend->setValue(360);
  lbegin = new QLabel(tr("Beginning (V)"), gdiagrammdim);
  lbegin1 = new QLabel(tr("Degree"), gdiagrammdim);
  lend = new QLabel(tr("End (V)"), gdiagrammdim);
  lend1 = new QLabel(tr("Degree"), gdiagrammdim);

  QObject::connect( boxdbmax, SIGNAL( currentIndexChanged(int)), this, SLOT(diagrammdim(int)));
  QObject::connect( boxdbmin, SIGNAL( currentIndexChanged(int)), this, SLOT(diagrammdim(int)));
  QObject::connect(spinadbegin, SIGNAL( valueChanged(double)), this, SLOT(diagrammdimd(double)));
  QObject::connect(spinadend, SIGNAL( valueChanged(double)), this, SLOT(diagrammdimd(double)));
  QObject::connect(spingradbegin, SIGNAL( valueChanged(int)), this, SLOT(diagrammdim(int)));
  QObject::connect(spingradend, SIGNAL( valueChanged(int)), this, SLOT(diagrammdim(int)));

  // Antennas Diagramm Widget Ende
  ///////////////////////////////////////////////////////////////////////////////

  ///////////////////////////////////////////////////////////////////////////////
  // Grafischen elemente anordnen
  resizeWidgets();

  //eichkorrektur auf 0.0 setzen
  for(i=0;i<maxmesspunkte;i++)
  {
    sweepdata.kalibrierarray.arrayk1[i] = 0.0;
    sweepdata.kalibrierarray.arrayk2[i] = 0.0;
    sweepdata.swrkalibrierarray.arrayk1[i] = -10.0;
    sweepdata.swrkalibrierarray.arrayk2[i] = -10.0;
  }
  //Korrekturwerte vom SAV TP und BP auf 0,0dB setzen
  for(i=0;i< calmaxindex;i++)
  {
    sweepdata.savcalarray.p[i] = 0.0;
  }
  //for the zoom function
  bSweepIsRun = FALSE;
  //for the power meter display to the top
  counterPeak1 = 0;
  counterPeak2 = 0;
  emit sendsweepdata(sweepdata);
  //keine FW-Version
  fwversion = 0;
  //keine Zwischenzeit da FW noch nicht bekannt
  boxzwischenzeit->setCurrentIndex(0);
  //diese Box erst freischalten wenn richtige FW erkannt wird
  boxzwischenzeit->setEnabled(FALSE);
  labelzwischenzeit->setEnabled(FALSE);
  verstep = TRUE;
  //FW ist noch unbekannt
  currentdata.fwWrong = TRUE;
  //Hide tabs if there is no connection to the NWT
#ifndef DEBUG_INTERFACE
  setTabEnabled(tab_nwt7vfo, FALSE);
  setTabEnabled(tab_nwt7measure, FALSE);
  setTabEnabled(tab_calculation, TRUE);
#endif
  sabereich = bkein;
  sabereichalt = bkein;
  sabw = bwkein;
  //zur besseren Optik das Grafikfenster spaet sichtbar machen
  grafik->show();
  //Tab switch control
  QObject::connect( this, SIGNAL(currentChanged(int)), this, SLOT(tabAfterSwitch(int)));

  restartMessure = FALSE;
  filenamesonde1 = "";
  filenamesonde2 = "";
  sweepdata.hline = 0.0;
  currentdata.bfliessend = TRUE;
  if (currentIndex() != tab_nwt7sweep)
      setCurrentIndex(tab_nwt7sweep);
  else
      tabAfterSwitch(tab_nwt7sweep);
  setInitInterface(false); //disable controls
  idletimer->start(currentdata.idletime); //if conncted enable controls
}

Nwt7MainClass::~Nwt7MainClass()
{
  wkmanager->beenden();
  grafik->hide();
}

void Nwt7MainClass::editProbe1(){
  int r;
  TProbe sonde;
  QString snum;
  int pos;
  
  QString qs = filenamesonde1;
  if(!qs.contains(".")) qs = qs + ".hfm";
  
  sonde.dname = qs;
//  sonde.sname = checkboxk1->text();
  sonde.sname = qs;
  for(int i=0;i<30;i++){
    snum = wattoffsetk1[i].str_offsetwatt;
    pos = snum.indexOf("|");
    if(pos > -1){
      sonde.stroffset[i] = snum.right(pos+2);
    }else{
      sonde.stroffset[i] = snum;
    }
    sonde.offset[i] = wattoffsetk1[i].offsetwatt;
  }
  ProbeDlg *dlg = new ProbeDlg(this);

  dlg->setdaten(sonde);
  dlg->tip(btip);
  r = dlg->exec();
  if(r == QDialog::Accepted){
    sonde = dlg->getdaten();
    boxwattoffset1->clear(); // Box loeschen
    for(int i=0;i<30;i++){
      snum.sprintf("%2i| ",i+1);
      boxwattoffset1->addItem(snum + sonde.stroffset[i]);
      wattoffsetk1[i].str_offsetwatt = sonde.stroffset[i];
      wattoffsetk1[i].offsetwatt = sonde.offset[i];
    }
    sweepdata.beschreibung1 = sonde.sname;
    checkboxk1->setText(sonde.sname);
    qDebug()<< "Dialog OK";
    ConfigFile nwt7sondenfile;
    bersterstart = TRUE;
    messsondespeichern1(sonde.dname);
    bersterstart = FALSE;
  }
  delete dlg;
}

void Nwt7MainClass::editProbe2(){
  int r;
  TProbe sonde;
  QString snum;
  int pos;

  QString qs = filenamesonde2;
  if((qs.indexOf(".")== -1)) qs = qs + ".hfm";
  
  sonde.dname = qs;
  sonde.sname = qs;
  for(int i=0;i<30;i++){
    snum = wattoffsetk2[i].str_offsetwatt;
    pos = snum.indexOf("|");
    if(pos > -1){
      sonde.stroffset[i] = snum.right(pos+2);
    }else{
      sonde.stroffset[i] = snum;
    }
    sonde.offset[i] = wattoffsetk2[i].offsetwatt;
  }
  
  ProbeDlg *dlg = new ProbeDlg(this);
  dlg->setdaten(sonde);
  dlg->tip(btip);
  r = dlg->exec();
  if(r == QDialog::Accepted){
    sonde = dlg->getdaten();
    boxwattoffset2->clear();
    for(int i=0;i<30;i++){
      snum.sprintf("%2i| ",i+1);
      boxwattoffset2->addItem(snum + sonde.stroffset[i]);
      wattoffsetk2[i].str_offsetwatt = sonde.stroffset[i];
      wattoffsetk2[i].offsetwatt = sonde.offset[i];
    }
    sweepdata.beschreibung2 = sonde.sname;
    checkboxk2->setText(sonde.sname);
    ConfigFile nwt7sondenfile;
    bersterstart = TRUE;
    messsondespeichern2(sonde.dname);
    bersterstart = FALSE;
  }
  delete dlg;
}

void Nwt7MainClass::setAttItem(){
  cbAttenuator1->blockSignals(TRUE);
  cbAttenuator2->blockSignals(TRUE);
  cbAttenuator3->blockSignals(TRUE);
  cbAttenuator1->clear();
  cbAttenuator2->clear();
  cbAttenuator3->clear();

  const QVector<int> &attens = currentdata.attenuator.GetAttenuations();
  for (QVector<int>::const_iterator it = attens.begin(); it != attens.end(); ++it)
  {
      QString s(tr("%1dB", "").arg((int)*it));
//      QString s(tr("%1dB [%2]", "")
//                .arg(*it)
//                .arg(QString(QString::number(currentdata.attenuator.GetControlCode(*it), 2))));

      cbAttenuator1->addItem(s);
      cbAttenuator2->addItem(s);
      cbAttenuator3->addItem(s);
  }
  int maxAtten = currentdata.attenuator.GetMaxAttenuation();
  labeldaempfung->setText(tr("0dB/%1dB","").arg(maxAtten));
  labeldaempfung1->setText(tr("Attenuator 0-%1dB","").arg(maxAtten));
  labeldaempfung3->setText(tr("Attenuator 0-%1dB","").arg(maxAtten));

  fwportbalt = 0;
  cbAttenuator1->blockSignals(FALSE);
  cbAttenuator2->blockSignals(FALSE);
  cbAttenuator3->blockSignals(FALSE);
}

void Nwt7MainClass::resetCursor(int a){
  boxkursornr->setCurrentIndex(a);
}

void Nwt7MainClass::sweepInvalid(){
  int i;
  
  for(i=0; i<maxmesspunkte; i++){
    sweepdata.mess.k1[i]=0;
    sweepdata.mess.k2[i]=0;
  }
  sweepdata.mess.daten_enable = FALSE;
}

void Nwt7MainClass::setIQVFO(){
  setVfoToOutOnce();
}

void Nwt7MainClass::setShift(){
  QString sdbshift1, sdbshift2;
  bool ok;

  sdbshift1 = boxdbshift1->currentText();
  sdbshift2 = boxdbshift2->currentText();
  sweepdata.dbshift1 = sdbshift1.toDouble(&ok);
  sweepdata.dbshift2 = sdbshift2.toDouble(&ok);
  fsendsweepdata();
  #ifdef LDEBUG
    qDebug("setShift k1 %f k2 %f", sweepdata.dbshift1, sweepdata.dbshift2);
  #endif
}

void Nwt7MainClass::clicklMagnifyPlus(){
  double fa, fe, fm, fbereich;
  
  if(buttonlupeplus->isEnabled()){
    fa = sweepdata.anfangsfrequenz;
    fbereich = double(sweepdata.cntpoints) * sweepdata.schrittfrequenz;
    fm = KursorFrequenz;
    fa = fm - fbereich / 4.0;
    fe = fm + fbereich / 4.0;
    if(sweepdata.freq_faktor > 1){
      fa = fa / sweepdata.freq_faktor;
      fe = fe / sweepdata.freq_faktor;
    }
    if(fa < currentdata.calibrate_start_freq)fa = currentdata.calibrate_start_freq;
    if(fe > currentdata.calibrate_end_freq)fe = currentdata.calibrate_end_freq;
    if(sweepdata.freq_faktor > 1){
      fa = fa * sweepdata.freq_faktor;
      fe = fe * sweepdata.freq_faktor;
    }
    setFqStart(fa);
    setFqEnd(fe);
    labellupe->setEnabled(FALSE);
    buttonlupeplus->setEnabled(FALSE);
    buttonlupeminus->setEnabled(FALSE);
    buttonlupemitte->setEnabled(FALSE);
    if(!bSweepIsRun)clickSweepOnce();
  }
}

void Nwt7MainClass::clicklMagnifyEqual(){
  double fa, fe, fm, fbereich;
  
  fa = sweepdata.anfangsfrequenz;
  fbereich = double(sweepdata.cntpoints) * sweepdata.schrittfrequenz / 2.0;
  //  qDebug("%f",KursorFrequenz);
  fm = KursorFrequenz;
  if( fm > 0.0){
    fa = fm - fbereich;
    fe = fm + fbereich;
    if((sweepdata.eoperatemode == eSpectrumAnalyser) or
       (sweepdata.eoperatemode == eSpectrumDisplayShift)){
      if(sabereich1->isChecked()){
        if(fa < currentdata.calibrate_start_freq)fa = currentdata.frqa1;
        if(fe > currentdata.calibrate_end_freq)fe = currentdata.frqb1;
      }
      if(sabereich2->isChecked()){
        if(fa < currentdata.calibrate_start_freq)fa = currentdata.frqa2;
        if(fe > currentdata.calibrate_end_freq)fe = currentdata.frqb2;
      }
    }else{
      if(sweepdata.freq_faktor > 1){
        fa = fa / sweepdata.freq_faktor;
        fe = fe / sweepdata.freq_faktor;
      }
      if(fa < currentdata.calibrate_start_freq)fa = currentdata.calibrate_start_freq;
      if(fe > currentdata.calibrate_end_freq)fe = currentdata.calibrate_end_freq;
      if(sweepdata.freq_faktor > 1){
        fa = fa * sweepdata.freq_faktor;
        fe = fe * sweepdata.freq_faktor;
      }
    }
    setFqStart(fa);
    setFqEnd(fe);
  }
  labellupe->setEnabled(FALSE);
  buttonlupeplus->setEnabled(FALSE);
  buttonlupeminus->setEnabled(FALSE);
  buttonlupemitte->setEnabled(FALSE);
  if(!bSweepIsRun)clickSweepOnce();
  emit cursormitte();
}

void Nwt7MainClass::clicklMagnifyMinus(){
  double fa, fe, fm, fbereich;
  
  if(buttonlupeminus->isEnabled()){
    fa = sweepdata.anfangsfrequenz;
    fbereich = double(sweepdata.cntpoints) * sweepdata.schrittfrequenz;
    fm = KursorFrequenz;
    fa = fm - fbereich;
    fe = fm + fbereich;
    if(sweepdata.freq_faktor > 1){
      fa = fa / sweepdata.freq_faktor;
      fe = fe / sweepdata.freq_faktor;
    }
    if(fa < currentdata.calibrate_start_freq)fa = currentdata.calibrate_start_freq;
    if(fe > currentdata.calibrate_end_freq)fe = currentdata.calibrate_end_freq;
    if(sweepdata.freq_faktor > 1){
      fa = fa * sweepdata.freq_faktor;
      fe = fe * sweepdata.freq_faktor;
    }
    setFqStart(fa);
    setFqEnd(fe);
    labellupe->setEnabled(FALSE);
    buttonlupeplus->setEnabled(FALSE);
    buttonlupeminus->setEnabled(FALSE);
    buttonlupemitte->setEnabled(FALSE);
    if(!bSweepIsRun)clickSweepOnce();
  }
}

void Nwt7MainClass::setCursorFrequency(double afrq){
  KursorFrequenz = afrq;
  if(bSweepIsRun){
    labellupe->setEnabled(FALSE);
    buttonlupeplus->setEnabled(FALSE);
    buttonlupeminus->setEnabled(FALSE);
    buttonlupemitte->setEnabled(FALSE);
  }else{
    if(KursorFrequenz == 0.0){
      labellupe->setEnabled(FALSE);
      buttonlupeplus->setEnabled(FALSE);
      buttonlupeminus->setEnabled(FALSE);
      buttonlupemitte->setEnabled(FALSE);
    }else{
      labellupe->setEnabled(TRUE);
      buttonlupeplus->setEnabled(TRUE);
      buttonlupeminus->setEnabled(TRUE);
      buttonlupemitte->setEnabled(TRUE);
    }
  }
}

void Nwt7MainClass::menuFirsSet(){
  emit setmenu(emCalibrateK1, TRUE);
  if(sweepdata.numberchanel){
    emit setmenu(emCalibrateK2, FALSE);
    emit setmenu(emLoadCallibrateK2, FALSE);
    emit setmenu(emSaveCallibrateK2, FALSE);
    emit setmenu(emWattCallibrateK2, FALSE);
    emit setmenu(emWattEditSonde2, FALSE);
    checkboxk2->setChecked(FALSE);
    checkboxk2->setEnabled(FALSE);
    progressbaPowerMeter2->setValue(0);
    progressbaPowerMeter2->setEnabled(FALSE);
    ldaempfungk2->setEnabled(FALSE);
    boxwattoffset2->setEnabled(FALSE);
    messlabel2->setText("");
    messlabel2->setEnabled(FALSE);
    messlabel4->setText("");
    messlabel4->setEnabled(FALSE);
    messlabel6->setText("");
    messlabel6->setEnabled(FALSE);
    mlabelk2->setText("");
    mlabelk2->setEnabled(FALSE);
    labelldaempfungk2->setText("");
    checkboxmesshang2->setEnabled(FALSE);
  }else{
    emit setmenu(emCalibrateK2, TRUE);
    emit setmenu(emLoadCallibrateK2, TRUE);
    emit setmenu(emSaveCallibrateK2, TRUE);
    emit setmenu(emWattCallibrateK2, TRUE);
    emit setmenu(emWattEditSonde2, TRUE);
    checkboxk2->setEnabled(TRUE);
    progressbaPowerMeter2->setEnabled(TRUE);
    ldaempfungk2->setEnabled(TRUE);
    mlabelk2->setEnabled(TRUE);
    boxwattoffset2->setEnabled(TRUE);
    labelldaempfungk2->setText(tr("Attenuation (dB)",""));
    checkboxmesshang2->setEnabled(TRUE);
  }
}


void Nwt7MainClass::configLoad(const QString &filename){
  ConfigFile nwt7configfile;
  QString qs;
  int fontsize;
  int i,j,p;
  
  //qDebug("Nwt7linux::configurationladen(const QString &filename)");
  sw305 = FALSE;
  nwt7configfile.open(filename);
#ifdef Q_OS_WIN
  currentdata.str_tty = nwt7configfile.readString("serielle_schnittstelle", "COM1");
#else
  wgrunddaten.str_tty = nwt7configfile.readString("serielle_schnittstelle", "/dev/ttyS0");
#endif
  ttyOpen(0); //Sobol A.E.
  editanfang->setText(nwt7configfile.readString("editanfang", "1000000"));
  editende->setText(nwt7configfile.readString("editende", "150000000"));
  editschritte->setText(nwt7configfile.readString("editschritte", "401"));
  editdisplay->setText(nwt7configfile.readString("editdisplay", "0"));
  currentdata.grperwobbeln = nwt7configfile.readBool("grafikfocuswobbeln", FALSE);
  currentdata.grpereinmal = nwt7configfile.readBool("grafikfocuseinmal", FALSE);
  currentdata.grperstop = nwt7configfile.readBool("grafikfocusstop", FALSE);
  currentdata.bwarnsavbw = nwt7configfile.readBool("bwarnsavbw", TRUE);
  currentdata.bwarnsavbwmax = nwt7configfile.readBool("bwarnsavbwmax", TRUE);
  currentdata.bset0hz = nwt7configfile.readBool("bset0hz", TRUE);
  currentdata.bfliessend = nwt7configfile.readBool("bfliessend", TRUE);
  currentdata.bsendalways = nwt7configfile.readBool("bsendalways", TRUE);
  //ab FW 1.20 neue Parameter fuer das NF Wobbeln
  currentdata.audioztime = nwt7configfile.readInteger("audioztime", 10000);
  currentdata.audiosens = nwt7configfile.readInteger("audiosens", 2);
  currentdata.wmprezision = nwt7configfile.readInteger("wattmeter_prezision", 1);

  //////////////////////////////////////////////////////////
  bool bant = nwt7configfile.readBool("AntDiagramm", false);
  if(bant){
    wdiagramm->show();
    tab_wdiagramm=addTab(wdiagramm, tr("Antenna`s chart"));
  }
  //////////////////////////////////////////////////////////

  /////////////////////////////////////////////////
  //Paramatemeter fuer das Antennendiagram
  /////////////////////////////////////////////////
  sweepdata.antdiagramm.adbegin = nwt7configfile.readInteger("antdia_adbegin", 0);
  if(sweepdata.antdiagramm.adbegin < 0)sweepdata.antdiagramm.adbegin = 0;
  if(sweepdata.antdiagramm.adbegin > 1023)sweepdata.antdiagramm.adbegin = 0;
  sweepdata.antdiagramm.adend = nwt7configfile.readInteger("antdia_adend", 1023);
  if(sweepdata.antdiagramm.adend < 0)sweepdata.antdiagramm.adend = 1023;
  if(sweepdata.antdiagramm.adend > 1023)sweepdata.antdiagramm.adend = 1023;
  sweepdata.antdiagramm.dbmin = nwt7configfile.readInteger("antdia_dbmin", -40);
  sweepdata.antdiagramm.dbmax = nwt7configfile.readInteger("antdia_dbmax", 10);
  sweepdata.antdiagramm.diabegin = nwt7configfile.readInteger("antdia_diabegin", 10);
  sweepdata.antdiagramm.diaend = nwt7configfile.readInteger("antdia_diaend", 350);
  /////////////////////////////////////////////////
  //Paramatemeter fuer das Einblenden der Infotexte
  /////////////////////////////////////////////////
  nbildinfo.posx = nwt7configfile.readInteger("pngposx", 40);
  nbildinfo.posy = nwt7configfile.readInteger("pngposy", 120);
  nbildinfo.fontsize = nwt7configfile.readInteger("pngfontsize", 11);
  //////////////////////////////////////
  //Paramatemeter die nur gelesen werden
  //////////////////////////////////////
  currentdata.stime = nwt7configfile.readInteger("stime", 50);
  currentdata.vtime = nwt7configfile.readInteger("vtime", 60);
  currentdata.idletime = nwt7configfile.readInteger("idletime", 400);
  messtime = nwt7configfile.readInteger("messtime", 20);
  messtimeneu = nwt7configfile.readInteger("messtimeneu", 200);
  //begrenzen auf 10mSek
  if(messtime < 10)messtime=10;
  currentdata.pllmodeenable = nwt7configfile.readBool("pllmodeenable", FALSE);
  //qDebug("stime:%i",wgrunddaten.stime);
  //qDebug("vtime:%i",wgrunddaten.vtime);
  //qDebug("idletime:%i",wgrunddaten.idletime);
  //////////////////////////////////////
  //////////////////////////////////////
  //////////////////////////////////////
  //Profil ab 2.07
  //*******************************************************************
  aprofil[0].name = "default";
  aprofil[0].fqstart = editanfang->text();
  aprofil[0].fqend = editende->text();
  aprofil[0].cntpoints = editschritte->text();
  boxprofil->addItem(aprofil[0].name);
  p = nwt7configfile.readInteger("profile", 0);
  j=1;
  for(i=1; i < p; i++){
    qs.sprintf("profilname%02i",i);
    qs = nwt7configfile.readString(qs, "def");
    if(qs != "NN"){
      qs.sprintf("profilname%02i",i);
      aprofil[j].name = nwt7configfile.readString(qs, "0");
      qs.sprintf("profilanfang%02i",i);
      aprofil[j].fqstart = nwt7configfile.readString(qs, "0");
      qs.sprintf("profilende%02i",i);
      aprofil[j].fqend = nwt7configfile.readString(qs, "0");
      qs.sprintf("profilschritte%02i",i);
      aprofil[j].cntpoints = nwt7configfile.readString(qs, "1");
      boxprofil->addItem(aprofil[j].name);
      j++;
    } 
  }
  currentdata.pfsize = nwt7configfile.readInteger("programfontsize", 9);
  emit setFontSize(currentdata.pfsize);
  //*******************************************************************
  //gemerkte Wobbelfrequenzeinstellung laden
  //*******************************************************************
  currentdata.wanfang = nwt7configfile.readDouble("mwanfang", 100000.0);
  currentdata.wende = nwt7configfile.readDouble("mwende", 150000000.0);
  currentdata.wschritte = nwt7configfile.readInteger("mwschritte", 1001);
  //*******************************************************************
  //gemerkte SA-Einstelungen laden
  //*******************************************************************
  
  currentdata.sa1anfang = nwt7configfile.readDouble("msa1anfang", 1000000.0);
  currentdata.sa1ende = nwt7configfile.readDouble("msa1ende", 72000000.0);
  currentdata.sa1schritte = nwt7configfile.readInteger("msa1schritte", 1001);
  currentdata.sa2anfang = nwt7configfile.readDouble("msa2anfang", 135000000.0);
  currentdata.sa2ende = nwt7configfile.readDouble("msa2ende", 149000000.0);
  currentdata.sa2schritte = nwt7configfile.readInteger("msa2schritte", 1001);
  currentdata.sa3anfang = nwt7configfile.readDouble("msa3anfang", 410000000.0);
  currentdata.sa3ende = nwt7configfile.readDouble("msa3ende", 430000000.0);
  currentdata.sa3schritte = nwt7configfile.readInteger("msa3schritte", 1001);
  currentdata.frqa1 = nwt7configfile.readDouble("sa1anfang", 1000000.0);
  currentdata.frqb1 = nwt7configfile.readDouble("sa1ende", 72000000.0);
  currentdata.frqa2 = nwt7configfile.readDouble("sa2anfang", 135000000.0);
  currentdata.frqb2 = nwt7configfile.readDouble("sa2ende", 149000000.0);
  currentdata.frqzf1 = nwt7configfile.readDouble("sazf1", 85300000.0);
  currentdata.frqzf2 = nwt7configfile.readDouble("sazf2", -85300000.0);
  currentdata.frqshift = nwt7configfile.readDouble("safrqshift", -410000000.0);
  currentdata.sastatus = nwt7configfile.readBool("sastatus", FALSE);
  currentdata.bschrittkorr = nwt7configfile.readBool("saschrittkorr", TRUE);
  currentdata.psavabs1 = nwt7configfile.readDouble("pegelsavabs1", 0.0);
  currentdata.psavabs2 = nwt7configfile.readDouble("pegelsavabs2", 0.0);
  currentdata.psavabs3 = nwt7configfile.readDouble("pegelsavabs3", 0.0);
  sweepdata.psav300 = nwt7configfile.readDouble("pegelsav300hz", 0.0);
  sweepdata.psav7k = nwt7configfile.readDouble("pegelsav7khz", 0.0);
  sweepdata.psav30k = nwt7configfile.readDouble("pegelsav30khz", 0.0);
  sweepdata.bsavdbm = nwt7configfile.readBool("savdbmanz", TRUE);
  sweepdata.bsavuv = nwt7configfile.readBool("savuvanz", FALSE);
  sweepdata.bsavwatt = nwt7configfile.readBool("savwattanz", FALSE);
  sweepdata.bsavdbuv = nwt7configfile.readBool("savdbuvanz", FALSE);
  sweepdata.safehlermax = nwt7configfile.readInteger("safehlermax", -5);
  sweepdata.safehlermin = nwt7configfile.readInteger("safehlermin", -60);
  currentdata.saminschritte = nwt7configfile.readInteger("saminschritte", 401);
  currentdata.bw300_max = nwt7configfile.readDouble("bw300_max", 100.0);
  currentdata.bw300_min = nwt7configfile.readDouble("bw300_min", 50.0);
  currentdata.bw7kHz_max = nwt7configfile.readDouble("bw7kHz_max", 2500.0);
  currentdata.bw7kHz_min = nwt7configfile.readDouble("bw7kHz_min", 1250.0);
  currentdata.bw30kHz_max = nwt7configfile.readDouble("bw30kHz_max", 10000.0);
  currentdata.bw30kHz_min = nwt7configfile.readDouble("bw30kHz_min", 5000.0);
  
  //*******************************************************************
  //ab V3.04.03
  //*******************************************************************
  sweepdata.ydbmax = nwt7configfile.readInteger("ydbmax", 10);
  sweepdata.ydbmin = nwt7configfile.readInteger("ydbmin", -90);
  boxydbmax->setCurrentIndex((30-sweepdata.ydbmax)/10);
  boxydbmin->setCurrentIndex((-10-sweepdata.ydbmin)/10);
  //*******************************************************************
  //ab V3.04.05
  //*******************************************************************
  qs = nwt7configfile.readString("kurvendir", "");
  currentdata.kurvendir = qs;
  if(!qs.isEmpty()){
    kurvendir.setPath(qs);
  }else{
    kurvendir = homedir;
  } 
  //*******************************************************************
  qs = nwt7configfile.readString("messlabelfont", "");
  currentdata.calibrate_start_freq = nwt7configfile.readDouble("eichanfangsfrequenz", 100000.0);
  currentdata.calibrate_end_freq = nwt7configfile.readDouble("eichendfrequenz", 150000000.0);
  currentdata.kalibrierstep = nwt7configfile.readLong("eichschritte", maxmesspunkte);
  
  //VFO im Wattmeter ein/aus
  messPanelState = nwt7configfile.readInteger("messvfoaktiv", TRUE);
  vfoOnOffControls();
  //VFO im Wattmeter Frequenz einstellen
  double fr;
  fr = nwt7configfile.readDouble("messvfo", 10000000.0);
  int a;
  a = int(fr/1000000000.0);
  sp1ghz->setValue(a);
  fr = fr - (double(a * 1000000000.0));
  a = int(fr/100000000.0);
  sp100mhz->setValue(a);
  fr = fr - (double(a * 100000000.0));
  a = int(fr/10000000.0);
  sp10mhz->setValue(a);
  fr = fr - (double(a * 10000000.0));
  a = int(fr/1000000.0);
  sp1mhz->setValue(a);
  fr = fr - (double(a * 1000000.0));
  a = int(fr/100000.0);
  sp100khz->setValue(a);
  fr = fr - (double(a * 100000.0));
  a = int(fr/10000.0);
  sp10khz->setValue(a);
  fr = fr - (double(a * 10000.0));
  a = int(fr/1000.0);
  sp1khz->setValue(a);
  fr = fr - (double(a * 1000.0));
  a = int(fr/100.0);
  sp100hz->setValue(a);
  fr = fr - (double(a * 100.0));
  a = int(fr/10.0);
  sp10hz->setValue(a);
  fr = fr - (double(a * 10.0));
  sp1hz->setValue(int(fr));

  currentdata.dds_core_freq = nwt7configfile.readDouble("DDStakt", 400000000.0);
  currentdata.pll = nwt7configfile.readInteger("PLLMode", 1);

  // Load attenuator
  currentdata.attenuatorType = (AttenuatorType) nwt7configfile.readInteger("AttenuatorType", AttenuatorType_Standard);
  currentdata.attenuatorCustomConfig = nwt7configfile.readString("AttenuatorCustomConfig", AttenuatorLoader::GetEmptyCustomConfig());
  if (currentdata.attenuatorType < AttenuatorType_Standard || currentdata.attenuatorType > AttenuatorType_Custom)
  {
      currentdata.attenuatorType = AttenuatorType_Standard;
  }
  if (currentdata.attenuatorType == AttenuatorType_Custom && !AttenuatorLoader::IsCustomConfigValid(currentdata.attenuatorCustomConfig))
  {
      currentdata.attenuatorType = AttenuatorType_Standard;
      currentdata.attenuatorCustomConfig = AttenuatorLoader::GetEmptyCustomConfig();
  }
  AttenuatorLoader::Load(currentdata.attenuator, currentdata.attenuatorType, currentdata.attenuatorCustomConfig);
  setAttItem();

  vmesstimer->stop();
  fontsize = nwt7configfile.readInteger("messlabelfontsize", 20);
  messfont.setPointSize(fontsize);
  if(!qs.isNull()){
    messfont.setFamily(qs);
    messlabel1->setFont(messfont);
    messlabel2->setFont(messfont);
    messlabel3->setFont(messfont);
    messlabel4->setFont(messfont);
    messlabel5->setFont(messfont);
    messlabel6->setFont(messfont);
    lmhz->setFont(messfont);
    lkhz->setFont(messfont);
    lhz->setFont(messfont);
  }  
  qs = nwt7configfile.readString("infofont", "");
  if(!qs.isNull()){
    infofont.setFamily(qs);
    fontsize = nwt7configfile.readInteger("infofontsize", 8);
    infofont.setPointSize(fontsize);
    mledit->setFont(infofont);
  }
  grafik->setSweepData(sweepdata);
  lcdfrq1 = nwt7configfile.readDouble("LCD1", 1800000.0);
  lcdfrq2 = nwt7configfile.readDouble("LCD2", 3600000.0);
  lcdfrq3 = nwt7configfile.readDouble("LCD3", 7000000.0);
  lcdfrq4 = nwt7configfile.readDouble("LCD4", 14000000.0);
  lcdfrq5 = nwt7configfile.readDouble("LCD5", 28000000.0);
  LCD1->display(qs.sprintf("%2.6f", lcdfrq1/1000000.0));
  LCD2->display(qs.sprintf("%2.6f", lcdfrq2/1000000.0));
  LCD3->display(qs.sprintf("%2.6f", lcdfrq3/1000000.0));
  LCD4->display(qs.sprintf("%2.6f", lcdfrq4/1000000.0));
  LCD5->display(qs.sprintf("%2.6f", lcdfrq5/1000000.0));
  editzf->setText(nwt7configfile.readString("editzf", "0"));
  sweepdata.namesonde1 = nwt7configfile.readString("namesonde1", "def_probe1");
  currentdata.strsonde1lin = nwt7configfile.readString("namesonde1lin", "def_probe1lin");
  sweepdata.namesonde2 = nwt7configfile.readString("namesonde2", "def_probe2");
  sweepdata.numberchanel = nwt7configfile.readBool("Einkanalig", TRUE);
  currentdata.kalibrierk1 = nwt7configfile.readBool("eichkorrk1", FALSE);
  currentdata.kalibrierk2 = nwt7configfile.readBool("eichkorrk2", FALSE);
  kalibrier1->setChecked(currentdata.kalibrierk1);
  kalibrier2->setChecked(currentdata.kalibrierk2);
  sweepdata.bswriteration = nwt7configfile.readBool("SWRIteration", TRUE);
  sweepdata.bswrrelais = nwt7configfile.readBool("SWR_Relais", FALSE); //Relais fuer NWT500 Nachbau
  sweepdata.binvers = nwt7configfile.readBool("bandbreite_invers", TRUE); //Darstellung der inversen Bandbreite
  checkboxinvers->setChecked(sweepdata.binvers);
  int r,g,b;
  //default rot
  r = nwt7configfile.readInteger("color1r", 255);
  g = nwt7configfile.readInteger("color1g", 0);
  b = nwt7configfile.readInteger("color1b", 0);
  a = nwt7configfile.readInteger("color1a", 255);
  sweepdata.penwidth = nwt7configfile.readInteger("penwidth", 1);
  QColor pencolor1(r,g,b,a);
  sweepdata.penkanal1.setColor(pencolor1);
  sweepdata.penkanal1.setWidth(sweepdata.penwidth);
  
  //default gruen
  r = nwt7configfile.readInteger("color2r", 85);
  g = nwt7configfile.readInteger("color2g", 255);
  b = nwt7configfile.readInteger("color2b", 0);
  a = nwt7configfile.readInteger("color2a", 255);
  QColor pencolor2(r,g,b,a);
  sweepdata.penkanal2.setColor(pencolor2);
  sweepdata.penkanal2.setWidth(sweepdata.penwidth);
  //default weiss
  r = nwt7configfile.readInteger("colorhr", 255);
  g = nwt7configfile.readInteger("colorhg", 255);
  b = nwt7configfile.readInteger("colorhb", 255);
  a = nwt7configfile.readInteger("colorha", 255);
  QColor hcolor(r,g,b,a);
  sweepdata.colorhintergrund = hcolor;
  //fuer SAV
  //wobdaten.colorhintergrunddunkel = nwt7configfile.readDouble("colorhd", 0.95);
  
  currentdata.max_sweep_freq = nwt7configfile.readDouble("maxwobbel", 200000000.0);
  sweepdata.freq_faktor = nwt7configfile.readInteger("frequenzfaktor", 1);
  setFrqFaktorLabel();
  //Dial1mhz->setMaximum(int(weichen.maxvfo / 1000000.0)-1);
  checkboxk1->setChecked(nwt7configfile.readBool("k1checked", TRUE));
  checkboxk2->setChecked(nwt7configfile.readBool("k2checked", FALSE));
  sweepdata.bswrkanal2 = checkboxk2->isChecked();
  //ComboBox im Wattmeter neu aufbauen
  ldampingk1->clear();
  ldampingk1->addItem("0");
  ldampingk1->addItem("10");
  ldampingk1->addItem("20");
  ldampingk1->addItem("30");
  ldampingk1->addItem("40");
  ldaempfungk2->clear();
  ldaempfungk2->addItem("0");
  ldaempfungk2->addItem("10");
  ldaempfungk2->addItem("20");
  ldaempfungk2->addItem("30");
  ldaempfungk2->addItem("40");
  qs = nwt7configfile.readString("powerk15", "99");
  if(qs != "99")ldampingk1->addItem(qs);
  qs = nwt7configfile.readString("powerk16", "99");
  if(qs != "99")ldampingk1->addItem(qs);
  qs = nwt7configfile.readString("powerk17", "99");
  if(qs != "99")ldampingk1->addItem(qs);
  qs = nwt7configfile.readString("powerk18", "99");
  if(qs != "99")ldampingk1->addItem(qs);
  qs = nwt7configfile.readString("powerk19", "99");
  if(qs != "99")ldampingk1->addItem(qs);
  qs = nwt7configfile.readString("powerk25", "99");
  if(qs != "99")ldaempfungk2->addItem(qs);
  qs = nwt7configfile.readString("powerk26", "99");
  if(qs != "99")ldaempfungk2->addItem(qs);
  qs = nwt7configfile.readString("powerk27", "99");
  if(qs != "99")ldaempfungk2->addItem(qs);
  qs = nwt7configfile.readString("powerk28", "99");
  if(qs != "99")ldaempfungk2->addItem(qs);
  qs = nwt7configfile.readString("powerk29", "99");
  if(qs != "99")ldaempfungk2->addItem(qs);
  currentdata.numberchanel = sweepdata.numberchanel;
  frqmarken.f1160m = nwt7configfile.readLongLong("a160m", 1810000);
  frqmarken.f2160m = nwt7configfile.readLongLong("b160m", 2000000);
  frqmarken.f180m = nwt7configfile.readLongLong("a80m", 3500000);
  frqmarken.f280m = nwt7configfile.readLongLong("b80m", 3800000);
  frqmarken.f140m = nwt7configfile.readLongLong("a40m", 7000000);
  frqmarken.f240m = nwt7configfile.readLongLong("b40m", 7200000);
  frqmarken.f130m = nwt7configfile.readLongLong("a30m", 10100000);
  frqmarken.f230m = nwt7configfile.readLongLong("b30m", 10150000);
  frqmarken.f120m = nwt7configfile.readLongLong("a20m", 14000000);
  frqmarken.f220m = nwt7configfile.readLongLong("b20m", 14350000);
  frqmarken.f117m = nwt7configfile.readLongLong("a17m", 18068000);
  frqmarken.f217m = nwt7configfile.readLongLong("b17m", 18168000);
  frqmarken.f115m = nwt7configfile.readLongLong("a15m", 21000000);
  frqmarken.f215m = nwt7configfile.readLongLong("b15m", 21450000);
  frqmarken.f112m = nwt7configfile.readLongLong("a12m", 24890000);
  frqmarken.f212m = nwt7configfile.readLongLong("b12m", 24990000);
  frqmarken.f110m = nwt7configfile.readLongLong("a10m", 28000000);
  frqmarken.f210m = nwt7configfile.readLongLong("b10m", 29700000);
  frqmarken.f16m = nwt7configfile.readLongLong("a6m", 50000000);
  frqmarken.f26m = nwt7configfile.readLongLong("b6m", 54000000);
  frqmarken.f12m = nwt7configfile.readLongLong("a2m", 144000000);
  frqmarken.f22m = nwt7configfile.readLongLong("b2m", 146000000);
  frqmarken.f1u1 = nwt7configfile.readLongLong("au1", 0);
  frqmarken.f2u1 = nwt7configfile.readLongLong("bu1", 0);
  frqmarken.f1u2 = nwt7configfile.readLongLong("au2", 0);
  frqmarken.f2u2 = nwt7configfile.readLongLong("bu2", 0);
  frqmarken.f1u3 = nwt7configfile.readLongLong("au3", 0);
  frqmarken.f2u3 = nwt7configfile.readLongLong("bu3", 0);
  frqmarken.b160m = nwt7configfile.readBool("160m", FALSE);
  frqmarken.b80m = nwt7configfile.readBool("80m", FALSE);
  frqmarken.b40m = nwt7configfile.readBool("40m", FALSE);
  frqmarken.b30m = nwt7configfile.readBool("30m", FALSE);
  frqmarken.b20m = nwt7configfile.readBool("20m", FALSE);
  frqmarken.b17m = nwt7configfile.readBool("17m", FALSE);
  frqmarken.b15m = nwt7configfile.readBool("15m", FALSE);
  frqmarken.b12m = nwt7configfile.readBool("12m", FALSE);
  frqmarken.b10m = nwt7configfile.readBool("10m", FALSE);
  frqmarken.b6m = nwt7configfile.readBool("6m", FALSE);
  frqmarken.b2m = nwt7configfile.readBool("2m", FALSE);
  frqmarken.bu1 = nwt7configfile.readBool("zusatz1", FALSE);
  frqmarken.bu2 = nwt7configfile.readBool("zusatz2", FALSE);
  frqmarken.bu3 = nwt7configfile.readBool("zusatz3", FALSE);
  frqmarken.st160m = nwt7configfile.readString("s160m", "160m");
  frqmarken.st80m = nwt7configfile.readString("s80m", "80m");
  frqmarken.st40m = nwt7configfile.readString("s40m", "40m");
  frqmarken.st30m = nwt7configfile.readString("s30m", "30m");
  frqmarken.st20m = nwt7configfile.readString("s20m", "20m");
  frqmarken.st17m = nwt7configfile.readString("s17m", "17m");
  frqmarken.st15m = nwt7configfile.readString("s15m", "15m");
  frqmarken.st12m = nwt7configfile.readString("s12m", "12m");
  frqmarken.st10m = nwt7configfile.readString("s10m", "10m");
  frqmarken.st6m = nwt7configfile.readString("s6m", "6m");
  frqmarken.st2m = nwt7configfile.readString("s2m", "2m");
  frqmarken.stu1 = nwt7configfile.readString("szusatz1", "Zusatz 1");
  frqmarken.stu2 = nwt7configfile.readString("szusatz2", "Zusatz 2");
  frqmarken.stu3 = nwt7configfile.readString("szusatz3", "Zusatz 3");
  // Sobol A.E.
  rbMessVfoOn->setChecked(nwt7configfile.readBool("rbMessVfoOn", FALSE));
  rbVfoOn->setChecked(nwt7configfile.readBool("rbVfoOn", FALSE));
  rbVFOSOff->setChecked(nwt7configfile.readBool("rbVFOSOff", TRUE));
  //
  if(sweepdata.numberchanel){
    emit setmenu(emCalibrateK2, FALSE);
    emit setmenu(emLoadCallibrateK2, FALSE);
    emit setmenu(emSaveCallibrateK2, FALSE);
    emit setmenu(emWattCallibrateK2, FALSE);
    emit setmenu(emWattEditSonde2, FALSE);
    checkboxk2->setChecked(FALSE);
    checkboxk2->setEnabled(FALSE);
    progressbaPowerMeter2->setValue(0);
    progressbaPowerMeter2->setEnabled(FALSE);
    ldaempfungk2->setEnabled(FALSE);
    boxwattoffset2->setEnabled(FALSE);
    messlabel2->setText("");
    messlabel2->setEnabled(FALSE);
    messlabel4->setText("");
    messlabel4->setEnabled(FALSE);
    messlabel6->setText("");
    messlabel6->setEnabled(FALSE);
    mlabelk2->setText("");
    mlabelk2->setEnabled(FALSE);
    labelldaempfungk2->setText("");
    checkboxmesshang2->setEnabled(FALSE);
  }else{
    emit setmenu(emCalibrateK2, TRUE);
    emit setmenu(emLoadCallibrateK2, TRUE);
    emit setmenu(emSaveCallibrateK2, TRUE);
    emit setmenu(emWattCallibrateK2, TRUE);
    emit setmenu(emWattEditSonde2, TRUE);
    checkboxk2->setEnabled(TRUE);
    progressbaPowerMeter2->setEnabled(TRUE);
    ldaempfungk2->setEnabled(TRUE);
    labelldaempfungk2->setText(tr("Attenuation (dB)",""));
    checkboxmesshang2->setEnabled(TRUE);
    mlabelk2->setEnabled(TRUE);
    boxwattoffset2->setEnabled(TRUE);
  }
  menuFirsSet();
  sweepdata.eoperatemode = enumOperateMode(nwt7configfile.readInteger("betriebsart", eGainSweep));
  cbOperatedMode->setCurrentIndex(int(sweepdata.eoperatemode));
  setOperateMode(int(sweepdata.eoperatemode));
  setFrqBereich();
  nwt7configfile.close();
  emit sendefrqmarken(frqmarken);
  ConfigFile nwt7sondenfile;
  qs = sweepdata.namesonde1;
  if(!qs.contains(".")) qs = qs + ".hfm";
  messsondenfilek1laden(qs);
  qs = sweepdata.namesonde2;
  if(!qs.contains(".")) qs = qs + ".hfm";
  messsondenfilek2laden(qs);
  setFrqBereich();
  SweepInit();
  fsendsweepdata();
  if(!bersterstart){
    if(!sw305){
      QMessageBox::warning( this, tr("Calibration files old Version",""), 
                                  tr("<b>The Calibrationdata of a Calibrationfile is in old Format !</b><br><br>In each Calibration-file there is Calibrationdata of the used channel. <br><br>The Calibrationdata of a channel is, from Version 4.00 on, in a new Format.<br> The new Format was necessary after implementation of the Scaling<br> of the Y-axis.<br><br>The Calibrationdata is being re-calculated automatically,  it can be used<br> for measurements.<br><br>If you want to store the new Calibrationdata, you can do so<br> by<b> \"Store Channelcalibration\" in the menu \"Sweep\" .</b><br>",""));
    }
  }
}

void Nwt7MainClass::configSave(const QString &filename){
  QString qs;
  int i;

  //  boxprofil->setCurrentIndex(0);
  ConfigFile *nwt7configfile = new ConfigFile();
  nwt7configfile->open(filename);
  qs.sprintf("%01i.%02i:V%02i",fwversion/100,fwversion-100,fwvariante);
  nwt7configfile->writeInteger("programfontsize", currentdata.pfsize);
  nwt7configfile->writeString("firmware", qs);
  nwt7configfile->writeString("serielle_schnittstelle", currentdata.str_tty);
  nwt7configfile->writeString("editanfang", editanfang->text());
  nwt7configfile->writeString("editende", editende->text());
  nwt7configfile->writeString("editschritte", editschritte->text());
  nwt7configfile->writeString("editdisplay", editdisplay->text());
  nwt7configfile->writeDouble("eichanfangsfrequenz", currentdata.calibrate_start_freq);
  nwt7configfile->writeDouble("eichendfrequenz", currentdata.calibrate_end_freq);
  nwt7configfile->writeLong("eichschritte", currentdata.kalibrierstep);
  nwt7configfile->writeDouble("DDStakt", currentdata.dds_core_freq);
  nwt7configfile->writeBool("Einkanalig", currentdata.numberchanel);
  nwt7configfile->writeInteger("PLLMode", currentdata.pll);
  nwt7configfile->writeDouble("maxwobbel", currentdata.max_sweep_freq);
  nwt7configfile->writeInteger("frequenzfaktor", sweepdata.freq_faktor);

  nwt7configfile->writeInteger("AttenuatorType", currentdata.attenuatorType);
  nwt7configfile->writeString("AttenuatorCustomConfig", currentdata.attenuatorCustomConfig);

  if(sweepdata.eoperatemode == eSmithDiagramm)sweepdata.eoperatemode = eGainSweep;
  nwt7configfile->writeInteger("betriebsart", int(sweepdata.eoperatemode));
  nwt7configfile->writeBool("grafikfocuswobbeln", currentdata.grperwobbeln);
  nwt7configfile->writeBool("grafikfocuseinmal", currentdata.grpereinmal);
  nwt7configfile->writeBool("grafikfocusstop", currentdata.grperstop);
  nwt7configfile->writeBool("bwarnsavbw", currentdata.bwarnsavbw);
  nwt7configfile->writeBool("bwarnsavbwmax", currentdata.bwarnsavbwmax);
  nwt7configfile->writeBool("bset0hz", currentdata.bset0hz);
  nwt7configfile->writeBool("bfliessend", currentdata.bfliessend);
  nwt7configfile->writeBool("bsendalways", currentdata.bsendalways);
  //ab FW 1.20 neue Parameter fuer das NF Wobbeln
  nwt7configfile->writeInteger("audioztime", currentdata.audioztime);
  nwt7configfile->writeInteger("audiosens", currentdata.audiosens);
  nwt7configfile->writeInteger("wattmeter_prezision", currentdata.wmprezision);

  //*******************************************************************
  //ab V3.04.03 Y-Skala speichern
  //*******************************************************************
  nwt7configfile->writeInteger("ydbmax", sweepdata.ydbmax);
  nwt7configfile->writeInteger("ydbmin", sweepdata.ydbmin);
  //*******************************************************************
  //ab V3.04.05 Kurven Dir abspeichern
  //*******************************************************************
  nwt7configfile->writeString("kurvendir", kurvendir.absolutePath());
  //*********************************************************************
  //Wobbelfrequenzeinstellungen merken
  //*********************************************************************
  nwt7configfile->writeDouble("mwanfang", currentdata.wanfang);
  nwt7configfile->writeDouble("mwende", currentdata.wende);
  nwt7configfile->writeInteger("mwschritte", currentdata.wschritte);
  //*********************************************************************
  //SA-Einstellungen merken
  //*********************************************************************
  nwt7configfile->writeDouble("msa1anfang", currentdata.sa1anfang);
  nwt7configfile->writeDouble("msa1ende", currentdata.sa1ende);
  nwt7configfile->writeInteger("msa1schritte", currentdata.sa1schritte);
  nwt7configfile->writeDouble("msa2anfang", currentdata.sa2anfang);
  nwt7configfile->writeDouble("msa2ende", currentdata.sa2ende);
  nwt7configfile->writeInteger("msa2schritte", currentdata.sa2schritte);
  nwt7configfile->writeDouble("msa3anfang", currentdata.sa3anfang);
  nwt7configfile->writeDouble("msa3ende", currentdata.sa3ende);
  nwt7configfile->writeInteger("msa3schritte", currentdata.sa3schritte);
  nwt7configfile->writeDouble("sa1anfang", currentdata.frqa1);
  nwt7configfile->writeDouble("sa1ende", currentdata.frqb1);
  nwt7configfile->writeDouble("sa2anfang", currentdata.frqa2);
  nwt7configfile->writeDouble("sa2ende", currentdata.frqb2);
  nwt7configfile->writeDouble("sazf1", currentdata.frqzf1);
  nwt7configfile->writeDouble("sazf2", currentdata.frqzf2);
  nwt7configfile->writeDouble("safrqshift", currentdata.frqshift);
  nwt7configfile->writeBool("sastatus", currentdata.sastatus);
  nwt7configfile->writeBool("saschrittkorr", currentdata.bschrittkorr);
  nwt7configfile->writeDouble("pegelsavabs1", currentdata.psavabs1);
  nwt7configfile->writeDouble("pegelsavabs2", currentdata.psavabs2);
  nwt7configfile->writeDouble("pegelsavabs3", currentdata.psavabs3);
  nwt7configfile->writeDouble("pegelsav300hz", sweepdata.psav300);
  nwt7configfile->writeDouble("pegelsav7khz", sweepdata.psav7k);
  nwt7configfile->writeDouble("pegelsav30khz", sweepdata.psav30k);
  nwt7configfile->writeBool("savdbmanz", sweepdata.bsavdbm);
  nwt7configfile->writeBool("savuvanz", sweepdata.bsavuv);
  nwt7configfile->writeBool("savwattanz", sweepdata.bsavwatt);
  nwt7configfile->writeBool("savdbuvanz", sweepdata.bsavdbuv);
  nwt7configfile->writeInteger("safehlermax", sweepdata.safehlermax);
  nwt7configfile->writeInteger("safehlermin", sweepdata.safehlermin);
  nwt7configfile->writeInteger("saminschritte", currentdata.saminschritte);
  nwt7configfile->writeDouble("bw300_max", currentdata.bw300_max);
  nwt7configfile->writeDouble("bw300_min", currentdata.bw300_min);
  nwt7configfile->writeDouble("bw7kHz_max", currentdata.bw7kHz_max);
  nwt7configfile->writeDouble("bw7kHz_min", currentdata.bw7kHz_min);
  nwt7configfile->writeDouble("bw30kHz_max", currentdata.bw30kHz_max);
  nwt7configfile->writeDouble("bw30kHz_min", currentdata.bw30kHz_min);
  //*********************************************************************
  sweepdata = grafik->getSweepData();
  nwt7configfile->writeDouble("LCD1", lcdfrq1);
  nwt7configfile->writeDouble("LCD2", lcdfrq2);
  nwt7configfile->writeDouble("LCD3", lcdfrq3);
  nwt7configfile->writeDouble("LCD4", lcdfrq4);
  nwt7configfile->writeDouble("LCD5", lcdfrq5);
  nwt7configfile->writeString("editzf", editzf->text());
  // VFO im Wattbereich
  nwt7configfile->writeBool("messvfoaktiv", messPanelState);
  double fr = double(sp1hz->value() + 
              (sp10hz->value()   * 10) +
              (sp100hz->value()  * 100) +
              (sp1khz->value()   * 1000) +
              (sp10khz->value()  * 10000) +
              (sp100khz->value() * 100000) +
              (sp1mhz->value()   * 1000000) +
              (sp10mhz->value()  * 10000000) +
              (sp100mhz->value() * 100000000) +
              (sp1ghz->value()   * 1000000000));
  nwt7configfile->writeDouble("messvfo", fr);
  messfont = messlabel1->font();
  qs = messfont.family();
  if(qs.isEmpty())qs = messfont.defaultFamily();
  nwt7configfile->writeString("messlabelfont", qs);
  nwt7configfile->writeInteger("messlabelfontsize", messfont.pointSize());
  nwt7configfile->writeString("infofont", infofont.family());
  nwt7configfile->writeInteger("infofontsize", infofont.pointSize());
  nwt7configfile->writeString("namesonde1", sweepdata.namesonde1);
  nwt7configfile->writeString("namesonde1lin", currentdata.strsonde1lin);
  nwt7configfile->writeString("namesonde2", sweepdata.namesonde2);
  nwt7configfile->writeBool("k1checked", checkboxk1->isChecked());
  nwt7configfile->writeBool("k2checked", checkboxk2->isChecked());
  nwt7configfile->writeBool("eichkorrk1", currentdata.kalibrierk1);
  nwt7configfile->writeBool("eichkorrk2", currentdata.kalibrierk2);
  nwt7configfile->writeBool("SWRIteration", sweepdata.bswriteration);
  nwt7configfile->writeBool("bandbreite_invers", sweepdata.binvers); //Darstellung der inversen Bandbreite
  
  int r,g,b,a;
  QColor pencolor1 = sweepdata.penkanal1.color();
  pencolor1.getRgb(&r, &g, &b, &a);
  nwt7configfile->writeInteger("color1r", r);
  nwt7configfile->writeInteger("color1g", g);
  nwt7configfile->writeInteger("color1b", b);
  nwt7configfile->writeInteger("color1a", a);
  nwt7configfile->writeInteger("penwidth", sweepdata.penwidth);
  
  QColor pencolor2 = sweepdata.penkanal2.color();
  pencolor2.getRgb(&r, &g, &b, &a);
  nwt7configfile->writeInteger("color2r", r);
  nwt7configfile->writeInteger("color2g", g);
  nwt7configfile->writeInteger("color2b", b);
  nwt7configfile->writeInteger("color2a", a);
  
  QColor colorh = sweepdata.colorhintergrund;
  colorh.getRgb(&r, &g, &b, &a);
  nwt7configfile->writeInteger("colorhr", r);
  nwt7configfile->writeInteger("colorhg", g);
  nwt7configfile->writeInteger("colorhb", b);
  nwt7configfile->writeInteger("colorha", a);
  
  //nwt7configfile->writeDouble("colorhd", wobdaten.colorhintergrunddunkel);

  //Speichern der profile
  nwt7configfile->writeInteger("profile", boxprofil->count());
  for(i=0; i < boxprofil->count(); i++){
    qs.sprintf("profilname%02i",i);
    nwt7configfile->writeString(qs, aprofil[i].name);
    qs.sprintf("profilanfang%02i",i);
    nwt7configfile->writeString(qs, aprofil[i].fqstart);
    qs.sprintf("profilende%02i",i);
    nwt7configfile->writeString(qs, aprofil[i].fqend);
    qs.sprintf("profilschritte%02i",i);
    nwt7configfile->writeString(qs, aprofil[i].cntpoints);
  } 
  for(i=5; i<ldampingk1->count(); i++){
    qs.sprintf("powerk1%i",i);
    ldampingk1-> setCurrentIndex(i);
    nwt7configfile->writeString(qs, ldampingk1->currentText());
  } 
  ldampingk1-> setCurrentIndex(0);
  for(i=5; i<ldaempfungk2->count(); i++){
    qs.sprintf("powerk2%i",i);
    ldaempfungk2-> setCurrentIndex(i);
    nwt7configfile->writeString(qs, ldaempfungk2->currentText());
  } 
  ldaempfungk2-> setCurrentIndex(0);
  nwt7configfile->writeLongLong("a160m", frqmarken.f1160m );
  nwt7configfile->writeLongLong("b160m", frqmarken.f2160m);
  nwt7configfile->writeLongLong("a80m", frqmarken.f180m);
  nwt7configfile->writeLongLong("b80m", frqmarken.f280m);
  nwt7configfile->writeLongLong("a40m", frqmarken.f140m);
  nwt7configfile->writeLongLong("b40m", frqmarken.f240m);
  nwt7configfile->writeLongLong("a30m", frqmarken.f130m);
  nwt7configfile->writeLongLong("b30m", frqmarken.f230m);
  nwt7configfile->writeLongLong("a20m", frqmarken.f120m);
  nwt7configfile->writeLongLong("b20m", frqmarken.f220m);
  nwt7configfile->writeLongLong("a17m", frqmarken.f117m);
  nwt7configfile->writeLongLong("b17m", frqmarken.f217m);
  nwt7configfile->writeLongLong("a15m", frqmarken.f115m);
  nwt7configfile->writeLongLong("b15m", frqmarken.f215m);
  nwt7configfile->writeLongLong("a12m", frqmarken.f112m);
  nwt7configfile->writeLongLong("b12m", frqmarken.f212m);
  nwt7configfile->writeLongLong("a10m", frqmarken.f110m);
  nwt7configfile->writeLongLong("b10m", frqmarken.f210m);
  nwt7configfile->writeLongLong("a6m", frqmarken.f16m);
  nwt7configfile->writeLongLong("b6m", frqmarken.f26m);
  nwt7configfile->writeLongLong("a2m", frqmarken.f12m);
  nwt7configfile->writeLongLong("b2m", frqmarken.f22m);
  nwt7configfile->writeLongLong("au1", frqmarken.f1u1);
  nwt7configfile->writeLongLong("bu1", frqmarken.f2u1);
  nwt7configfile->writeLongLong("au2", frqmarken.f1u2);
  nwt7configfile->writeLongLong("bu2", frqmarken.f2u2);
  nwt7configfile->writeLongLong("au3", frqmarken.f1u3);
  nwt7configfile->writeLongLong("bu3", frqmarken.f2u3);
  nwt7configfile->writeBool("160m", frqmarken.b160m );
  nwt7configfile->writeBool("80m", frqmarken.b80m );
  nwt7configfile->writeBool("40m", frqmarken.b40m );
  nwt7configfile->writeBool("30m", frqmarken.b30m );
  nwt7configfile->writeBool("20m", frqmarken.b20m );
  nwt7configfile->writeBool("17m", frqmarken.b17m );
  nwt7configfile->writeBool("15m", frqmarken.b15m );
  nwt7configfile->writeBool("12m", frqmarken.b12m );
  nwt7configfile->writeBool("10m", frqmarken.b10m );
  nwt7configfile->writeBool("6m", frqmarken.b6m );
  nwt7configfile->writeBool("2m", frqmarken.b2m );
  nwt7configfile->writeBool("zusatz1", frqmarken.bu1 );
  nwt7configfile->writeBool("zusatz2", frqmarken.bu2 );
  nwt7configfile->writeBool("zusatz3", frqmarken.bu3 );
  nwt7configfile->writeString("s160m", frqmarken.st160m );
  nwt7configfile->writeString("s80m", frqmarken.st80m );
  nwt7configfile->writeString("s40m", frqmarken.st40m );
  nwt7configfile->writeString("s30m", frqmarken.st30m );
  nwt7configfile->writeString("s20m", frqmarken.st20m );
  nwt7configfile->writeString("s17m", frqmarken.st17m );
  nwt7configfile->writeString("s15m", frqmarken.st15m );
  nwt7configfile->writeString("s12m", frqmarken.st12m );
  nwt7configfile->writeString("s10m", frqmarken.st10m );
  nwt7configfile->writeString("s6m", frqmarken.st6m );
  nwt7configfile->writeString("s2m", frqmarken.st2m );
  nwt7configfile->writeString("szusatz1", frqmarken.stu1 );
  nwt7configfile->writeString("szusatz2", frqmarken.stu2 );
  nwt7configfile->writeString("szusatz3", frqmarken.stu3 );
  // Sobol A.E.
  nwt7configfile->writeBool("rbMessVfoOn", rbMessVfoOn->isChecked() );
  nwt7configfile->writeBool("rbVfoOn", rbVfoOn->isChecked() );
  nwt7configfile->writeBool("rbVFOSOff", rbVFOSOff->isChecked() );
  //

  nwt7configfile->close();
}

///////////////////////////////////////////////////////////////////////////////
// Menubereich

void Nwt7MainClass::beenden()
{
  configSave(homedir.filePath(configfilename));
  wkmanager->beenden();
  if (bttyOpen)
  {
    defaultClearTty(); //eventuelle Daten im UART loeschen
    picmodem->closetty();
  }
  close();
}

void Nwt7MainClass::printDiagramm(){
  bstrlist.clear();
  QSize size = grafik->size();
  grafik->resize(500,460);
  QStringList sl = mledit->toPlainText().split("\n", QString::KeepEmptyParts);
  bstrlist = sl;
  emit drucken(printfont, bstrlist);
  grafik->resize(size);
}

void Nwt7MainClass::printDiagrammPdf(){
  bstrlist.clear();
  QSize size = grafik->size();
  grafik->resize(500,460);
  QStringList sl = mledit->toPlainText().split("\n", QString::KeepEmptyParts);
  bstrlist = sl;
  emit druckenpdf(printfont, bstrlist);
  grafik->resize(size);
}

void Nwt7MainClass::saveDiagrammImage(){
  bstrlist.clear();
  QStringList sl = mledit->toPlainText().split("\n", QString::KeepEmptyParts);
  bstrlist = sl;
  emit bildspeichern(nbildinfo, bstrlist);
}

void Nwt7MainClass::setconffilename(const QString &fname)
{
  //qDebug("Nwt7linux::setconffilename(const QString &fname)");
  bkalibrierenswr = FALSE;
  bkalibrierenswrneu = FALSE;
  bkalibrieren40 = FALSE;
  bkalibrieren0 = FALSE;
  bmkalibrierenk1_0db = FALSE;
  bmkalibrierenk1_20db = FALSE;
  bmkalibrierenk2_0db = FALSE;
  bmkalibrierenk2_20db = FALSE;
  btabelleschreiben = FALSE;
  bcurveloaded = FALSE;
  //Grundkonfig erzeugen
  if(bersterstart){
    QMessageBox::information( this, tr("First  Program start",""), 
                                    tr("<B> The program is the first time started </B> <BR> is the directory \"WinNWT5\" generated <BR> the configuration file and the files <br> probe. The directory is in the \"home area\" of the <br > of the operating system created. <br> The default data provide the first measurements. <br> Better values are obtained by calibration!",""));

    if(configfilename.isEmpty())configfilename = "hfm9.hfc";
    //defaultwerte laden
    configLoad(homedir.filePath(configfilename));
    //defaultwerte speichern
    configSave(homedir.filePath(configfilename));
    //defaultwerte speichern
    messsondespeichern1("def");
    messsondespeichern2("def");
    ConfigFile nwt7sondenfile;
    //lineare Defsonde erzeugen
    nwt7sondenfile.open(homedir.filePath("def_probe1lin.hfm"));
    nwt7sondenfile.writeString("dateiversion", "#V3.05");
    nwt7sondenfile.writeString("Sonde", "AD8361intern");
    nwt7sondenfile.writeDouble("faktor", 0.193143);
    nwt7sondenfile.writeDouble("shift", -84.634597);
    nwt7sondenfile.writeDouble("messx", 0.193143);
    nwt7sondenfile.writeDouble("messy", -84.634597);
    nwt7sondenfile.writeBool("linear", TRUE);
    nwt7sondenfile.close();
    bersterstart = FALSE;
  }

  configfilename = fname;
  configLoad(homedir.filePath(configfilename));
  wkmanager->loadConfig();
}

QString Nwt7MainClass::getConfFileName()
{
  return configfilename;
}

QString Nwt7MainClass::delPath(const QDir &s)
{
  QDir qd(s); 
  QString qs, qs1;
  
  //qDebug("Nwt7linux::delPath");
  qs = qd.dirName();
  //  qs1 = qd.filePath();
  //  qs.remove(1, qs1.length());
  //qDebug(qs);
  return qs;
}

void Nwt7MainClass::loadConfigFileDlg()
{
  QString s = QFileDialog::getOpenFileName(
                    this,
                    tr("Load NWT Configuration",""),
                    homedir.filePath("*.hfc"),
                    tr("NWT Configuration file (*.hfc)",""),
                    0,0);
  if (!s.isNull())
  {
    configLoad(s);
    s = delPath(s);
    #ifdef LDEBUG
        qDebug() << s;
    #endif
    configfilename = s;
    setWindowTitle(currentdata.str_tty);
  }
}

void Nwt7MainClass::saveConfigFileDlg()
{
  QString s = QFileDialog::getSaveFileName(
                    this,
                    tr("Save NWT Configuration",""),
                    homedir.filePath("*.hfc"),
                    tr("NWT Configuration file (*.hfc)",""),
                    0,0);
  if (!s.isNull())
  {
    //Datei ueberpruefen ob Sufix vorhanden
    if((s.indexOf(".")== -1)) s += ".hfc";
    configSave(s);
    wkmanager->beenden();
    s = delPath(s);
    //qDebug(s);
    configfilename = s;
    setWindowTitle(currentdata.str_tty);
    configLoad(homedir.filePath(configfilename));
  }
}

void Nwt7MainClass::getwkm(int i){
  switch(i){
    case 1:
      sweepdata.kurvennr = 1;
    break;
    case 2:
      sweepdata.kurvennr = 2;
    break;
    case 3:
      sweepdata.kurvennr = 3;
    break;
    case 4:
      sweepdata.kurvennr = 4;
    break;
    case 5:
      sweepdata.kurvennr = 5;
    break;
    case 6:
      sweepdata.kurvennr = 6;
  break;

  }
//  warneichkorr();
  QString qs;
  if(infoueber.count() > 0){
    qs = "";
    if(infoueber.at(0).indexOf(";") == 0)qs = qs = infoueber.at(0);
    if(infoueber.at(0).indexOf(";no_label") == 0)qs = "";
  }
  wkmanager->suebergabe(sweepdata, qs);
}

void Nwt7MainClass::wkmLoadCurve(int i){
  
  enumkurvenversion ekv = ekvkeine;
  TSweep walt;
  
  walt = sweepdata;
  ekv = loadCurve();
  if(ekv == ekvabbruch)return;
  switch(i){
    case 1:
      sweepdata.kurvennr = 1;
    break;
    case 2:
      sweepdata.kurvennr = 2;
    break;
    case 3:
      sweepdata.kurvennr = 3;
    break;
    case 4:
      sweepdata.kurvennr = 4;
    break;
    case 5:
      sweepdata.kurvennr = 5;
    break;
    case 6:
      sweepdata.kurvennr = 6;
  break;

  }
  sweepdata.bhintergrund = TRUE;
  sweepdata.mousesperre = TRUE;
//  warneichkorr();
  wkmanager->suebergabe(sweepdata, displaystr);
  sweepdata = walt;
}

enumkurvenversion Nwt7MainClass::loadCurve()
{
  enumkurvenversion ekv = ekvkeine;
  int i,j;
  int ikversion = 0; //neue Versionserkennung
/*
  bool v305 = FALSE;
  bool v404 = FALSE;
  bool v405 = FALSE;
  bool v410 = FALSE;
*/
  QString zeile;
  QString qs;
  QString qs1 = "";
  double faktor1, faktor2;
  double shift1, shift2;
  bool ok;
  
  QFile f;
  QStringList filters;
  filters << "Image files (*.png *.xpm *.jpg)"
          << "Text files (*.txt)"
          << "Any files (*)";

  QString s = QFileDialog::getOpenFileName(this, tr("Load NWT Graph file",""),
                                                 kurvendir.absolutePath(),
                                                 tr("NWT Graphfile (*.hfd)","")
                                                 );
/*
  
  QString s = QFileDialog::getOpenFileName(
                    kurvendir.filePath("*.hfd"),
                    tr("NWT Graphfile (*.hfd)",""),
                    this, 0, tr("Load NWT Graph file",""));
*/  
  displaystr = "";
  //String ist nicht leer
  if (!s.isNull())
  {
    displaystr = s;
    kurvendir.setPath(s);
    qs = kurvendir.absolutePath();
    //qDebug(qs);
    //Filename setzen
    f.setFileName(s);
    //QFile f = new QFile(s);
    if(f.open(QIODevice::ReadOnly)){
      QTextStream ts(&f);
      //erste Zeile lesen
      ts >> zeile;
      ikversion = 100;
      if(zeile == "#V3.05")ikversion = 305;
      if(zeile == "#V4.04")ikversion = 404;
      if(zeile == "#V4.05")ikversion = 405;
      if(zeile == "#V4.10")ikversion = 410;
/*
      if(zeile == "#V3.05")v305 = TRUE;
      if(zeile == "#V4.04"){
        v404 = TRUE;
        v305 = TRUE;
      }
      if(zeile == "#V4.05"){
        v405 = TRUE;
        v404 = TRUE;
        v305 = TRUE;
      }
      if(zeile == "#V4.10"){
        v410 = TRUE;
        v405 = TRUE;
        v404 = TRUE;
        v305 = TRUE;
      }
*/
  //if(!v305){
    if(ikversion < 305){ //erste eingelesene Zeile ist keine Versionsbezeichnung
        sweepdata.anfangsfrequenz = zeile.toDouble(&ok);
      }else{
        //Es ist V4.10: diese Zeile entscheidet ob NF oder HF Wobblung
        if(ikversion == 410){
          ts >> i; // einlesen
          sweepdata.maudio2 = false;
          if(i == 1)sweepdata.maudio2 = true;
        }
        // Version >= 305 Anfangsfrequenz einlesen
        ts >> sweepdata.anfangsfrequenz;
      }
      //qDebug("a1 %li", wobdaten.anfangsfrequenz);
      ts >> sweepdata.schrittfrequenz;
      ts >> sweepdata.cntpoints;
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
        //if(!v305){
          if(ikversion < 305){
            //Version < 3.05 faktor + shift umrechnen
            sweepdata.faktor1 = faktor1 / 10.23;
            sweepdata.shift1 = ((((511.5 - shift1) / faktor1) * sweepdata.faktor1) + 40.0) * -1.0;
            sweepdata.faktor2 = faktor2 / 10.23;
            sweepdata.shift2 = ((((511.5 - shift2) / faktor2) * sweepdata.faktor2) + 40.0) * -1.0;
/*
            qDebug("f1 %f",faktor1);
            qDebug("s1 %f",shift1);
            qDebug("f2 %f",faktor2);
            qDebug("s2 %f",shift2);
            qDebug("f1 %f",wobdaten.faktor1);
            qDebug("s1 %f",wobdaten.shift1);
            qDebug("f2 %f",wobdaten.faktor2);
            qDebug("s2 %f",wobdaten.shift2);
*/
          }else{
            //Version >= 3.05
            sweepdata.faktor1 = faktor1;
            sweepdata.shift1 = shift1;
            sweepdata.faktor2 = faktor2;
            sweepdata.shift2 = shift2;
          }
        break;
        //lin
        case 2:
        //if(!v305){
          if(ikversion < 305){
            //Version < 3.05 faktor + shift umrechnen
            sweepdata.faktor1 = faktor1 / 9.3;
            sweepdata.shift1 = ((((465.0 - shift1) / faktor1) * sweepdata.faktor1) + 50.0) * -1.0;
            sweepdata.faktor2 = faktor2 / 9.3;
            sweepdata.shift2 = ((((465.0 - shift2) / faktor2) * sweepdata.faktor2) + 50.0) * -1.0;
/*
            qDebug("f1 %f",faktor1);
            qDebug("s1 %f",shift1);
            qDebug("f2 %f",faktor2);
            qDebug("s2 %f",shift2);
            qDebug("f1 %f",wobdaten.faktor1);
            qDebug("s1 %f",wobdaten.shift1);
            qDebug("f2 %f",wobdaten.faktor2);
            qDebug("s2 %f",wobdaten.shift2);
*/
          }else{
            //Version >= 3.05
            sweepdata.faktor1 = faktor1;
            sweepdata.shift1 = shift1;
            sweepdata.faktor2 = faktor2;
            sweepdata.shift2 = shift2;
          }
        break;
      }
      switch(i){
        case 0: //wobbeln dB
          sweepdata.eoperatemode = eGainSweep;
          cbOperatedMode->setCurrentIndex (0);
          sweepdata.linear1 = FALSE;
          sweepdata.linear2 = FALSE;
          setTabEnabled(tab_nwt7measure, TRUE);
        break;
        case 1: // wobbeln SWR
          sweepdata.eoperatemode = eSWR;
          cbOperatedMode->setCurrentIndex (1);
          setTabEnabled(tab_nwt7measure, TRUE);
        break;
        case 2: // wobbeln linear
          sweepdata.eoperatemode = eGainSweep;
          cbOperatedMode->setCurrentIndex (0);
          sweepdata.linear1 = TRUE;
          sweepdata.linear2 = TRUE;
          setTabEnabled(tab_nwt7measure, FALSE);
        break;
        case 3: // wobbeln SWR ANT
          sweepdata.eoperatemode = eSWRfeeder;
          cbOperatedMode->setCurrentIndex (2);
          setTabEnabled(tab_nwt7measure, TRUE);
        break;
        case 4: // wobbeln Z
          sweepdata.eoperatemode = eImpedance;
          cbOperatedMode->setCurrentIndex (3);
          setTabEnabled(tab_nwt7measure, FALSE);
        break;
        case 5: // wobbeln SAV
          sweepdata.eoperatemode = eSpectrumAnalyser;
          cbOperatedMode->setCurrentIndex (4);
          sweepdata.linear1 = FALSE;
          sweepdata.linear2 = FALSE;
          setTabEnabled(tab_nwt7measure, FALSE);
        //if(v404){
          if(ikversion > 305){
            // Version > 3.05
            //noch zusaetliche Daten fuer die dBm Darstellung einlesen
            ts >> sweepdata.psavabs;
            ts >> sweepdata.psav300;
            ts >> sweepdata.psav7k;
            ts >> sweepdata.psav30k;
            ts >> j;
            switch(j){
              case 0: // keine Bandbreitenerkennung
                sweepdata.ebbsav = ekeinebandbreite;
                break;
              case 1: // 300Hz Kanal1
                sweepdata.ebbsav = e300b1;
                break;
              case 2: // 7kHz Kanal1
                sweepdata.ebbsav = e7kb1;
                break;
              case 3: // 30kHz Kanal1
                sweepdata.ebbsav = e30kb1;
                break;
              case 4: // 300Hz Kanal2
                sweepdata.ebbsav = e300b2;
                break;
              case 5: // 7kHz Kanal2
                sweepdata.ebbsav = e7kb2;
                break;
              case 6: // 30kHz Kanal2
                sweepdata.ebbsav = e30kb2;
                break;
            }
            ts >> sweepdata.display_shift;
          }else{
            //Version < 4.04
            //alte Versionen holen die fehlenden Daten aus dem Programm
            sweepdata.psavabs = wobdatenmerk.psavabs;
            sweepdata.psav300 = wobdatenmerk.psav300;
            sweepdata.psav7k = wobdatenmerk.psav7k;
            sweepdata.psav30k = wobdatenmerk.psav30k;
            sweepdata.ebbsav = wobdatenmerk.ebbsav;
            sweepdata.display_shift = wobdatenmerk.display_shift;
          }
        break;
      }
      for(i=0; i<sweepdata.cntpoints; i++){
        ts >> sweepdata.mess.k1[i];
      }
      for(i=0; i<sweepdata.cntpoints; i++){
        ts >> sweepdata.mess.k2[i];
      }
      // Beschreibung noch einlesen
    //if(!v405){
      if(ikversion < 405){
        //Version < 4.05
        qs = "";
        qs1 = "";
        while(!ts.atEnd()){
          ts >> qs1;
          qs = qs + qs1;
        }
      }else{
        ts >> qs;
        //qDebug(qs);
        //qDebug(wobdaten.layoutlabel);
        //Kalibrierung lesen
        for(i=0; i < maxmesspunkte; i++){
          ts >> sweepdata.kalibrierarray.arrayk1[i];
        }
        for(i=0; i < maxmesspunkte; i++){
          ts >> sweepdata.kalibrierarray.arrayk2[i];
        }
        for(i=0; i < maxmesspunkte; i++){
          ts >> sweepdata.swrkalibrierarray.arrayk1[i];
        }
        for(i=0; i < maxmesspunkte; i++){
          ts >> sweepdata.swrkalibrierarray.arrayk2[i];
        }
        for(i=0; i < calmaxindex; i++){
          ts >> sweepdata.savcalarray.p[i];
        }
      }
      //Test ob Label vorhanden
      if(qs.length() == 0)qs = ";no_label";
      //Ab SW 4.09 wird beim Label ein Semikolon voran gestellt
      if(qs[0] != QChar(';'))qs = ";" + qs;
      //qDebug(qs);
      f.close();
      //Infoueberschrift setzen
      mledit->clear();
      infoueber.clear();
      if(qs.indexOf("@;")){
        qs1 = qs.section("@",0,0);
        displaystr += " - " + qs1;
        //qDebug(qs1);
        infoueber.append(qs1);
        mledit->append(qs1);
        qs1 = qs.section("@",1,1);
        //qDebug(qs1);
        infoueber.append(qs1);
        mledit->append(qs1);
        qs1 = qs.section("@",2,2);
        //qDebug(qs1);
        infoueber.append(qs1);
        mledit->append(qs1);
        qs1 = qs.section("@",3,3);
        //qDebug(qs1);
        infoueber.append(qs1);
        mledit->append(qs1);
      }else{
        infoueber.append(qs);
        mledit->append(qs);
        displaystr += " - " + qs;
      }
      //Werte die in der Kurve nicht enthalten sind
      //muessen definiert gesetzt werden
      //kein dB Shift
      sweepdata.dbshift1 = 0.0;
      sweepdata.dbshift2 = 0.0;
      //===================
      sweepdata.mess.daten_enable = TRUE;
/*
      if(v305)ekv = ekv305;
      if(v404)ekv = ekv404;
      if(v405)ekv = ekv405;
      if(v410)ekv = ekv410;
*/
    }
  }
  switch(ikversion){
  case 100: ekv = ekvabbruch; break;
  case 305: ekv = ekv305; break;
  case 404: ekv = ekv404; break;
  case 405: ekv = ekv405; break;
  case 410: ekv = ekv410; break;
  default: ekv = ekvabbruch; break;
  }

  return ekv;
}


void Nwt7MainClass::loadCurves()
{
  enumkurvenversion ekv;
  bool v305 = FALSE;
  //bool v404 = FALSE;
  //bool v405 = FALSE;
  //bool v410 = FALSE;
  QString s;
  
//  warneichkorr();
  if(bcurveloaded){
    sweepdata = wobdatenmerk;
    sweepdata.mess.daten_enable = TRUE;
    sweepdata.bhintergrund = FALSE;
    sweepdata.mousesperre = FALSE;
    mledit->clear();
    fsendsweepdata();
  }
  wobdatenmerk = sweepdata; //alte Einstellung sichern
  //Bandbreitenanzeige ausschalten
  checkbox3db->setChecked(FALSE);
  checkbox6db->setChecked(FALSE);
  checkboxgrafik->setChecked(FALSE);
  checkboxinvers->setChecked(FALSE);
  bcurveloaded = TRUE;
  ekv = loadCurve();
  switch(ekv){
    case ekvabbruch:
      return;
      break;
    case ekvkeine:
      v305 = FALSE;
      //v404 = FALSE;
      //v405 = FALSE;
      //v410 = FALSE;
      break;
    case ekv305:
      v305 = TRUE;
      //v404 = FALSE;
      //v405 = FALSE;
      //v410 = FALSE;
      s = " #V3.05";
      break;
    case ekv404:
      v305 = TRUE;
      //v404 = TRUE;
      //v405 = FALSE;
      //v410 = FALSE;
      s = " #V4.04";
      break;
    case ekv405:
      v305 = TRUE;
      //v404 = TRUE;
      //v405 = TRUE;
      //v410 = FALSE;
      s = " #V4.05";
      break;
    case ekv410:
      v305 = TRUE;
      //v404 = TRUE;
      //v405 = TRUE;
      //v410 = TRUE;
      s = " #V4.10";
      break;
  }
  //Werte die in der Kurve nicht enthalten sind
  //muessen definiert gesetzt werden
  //kein dB Shift
  sweepdata.dbshift1 = 0.0;
  sweepdata.dbshift2 = 0.0;
  //keine Eichkorrektur
  //wobdaten.beichkorrk1 = FALSE;
  //wobdaten.beichkorrk2 = FALSE;
  //===================
  sweepdata.mess.daten_enable = TRUE;
  sweepdata.bhintergrund = FALSE;
  sweepdata.mousesperre = FALSE;
  //qDebug("a %li", wobdaten.anfangsfrequenz);
  fsendsweepdata();
  grafik->setWindowTitle(tr("Graphical Display","") + " - " + displaystr + " - " + s);
  if(!v305){
    QMessageBox::warning( this, tr("Graph files old Version",""), 
                                tr("<center><b>The Calibrationdata in a Graphfile is in old Format !</center></b><br>In each Graph-file there is Calibrationdata of the used channel also. <br> The Calibrationdata of a channel is, from Version 4.00 on, in a new Format.<br> The new Format was necessary after implementation of the Scaling<br> of the Y-axis. <br>The Calibrationdata is being re-calculated automatically. <br>If you want to store this Graph in the new Format,<br> you have to save the Graph again from this program.",""));
  }
}

void Nwt7MainClass::SaveCurvesMenu(){
  TSweep walt;

  walt = sweepdata;
  sweepdata = grafik->getSweepData();
  SaveCurves();
  sweepdata = walt;
}

void Nwt7MainClass::wkmSaveCurve(const TSweep &awobbel){
  TSweep walt;
  
  walt = sweepdata;
  sweepdata = awobbel;
  SaveCurves();
  sweepdata = walt;
}

void Nwt7MainClass::SaveCurves()
{
  int i,j;
  QFile f;
  QString qs;

  fsendsweepdata();
  QString s = QFileDialog::getSaveFileName(this,
                                           tr("Save NWT Graph",""),
                                           kurvendir.absolutePath(),
                                           tr("NWT Graphfile (*.hfd)","")
                                           );
/*  
  QString s = QFileDialog::getSaveFileName(
                    kurvendir.filePath("*.hfd"),
                    tr("NWT Graphfile (*.hfd)",""),
                    this, 0, tr("Save NWT Graph",""));
*/
  if (!s.isNull())
  {
    kurvendir.setPath(s);
    qs = kurvendir.absolutePath();
    //qDebug(qs);
    //Datei ueberpruefen ob Sufix vorhanden
    if((s.indexOf(".")== -1)) s += ".hfd";
    f.setFileName(s);
    if(f.open( QIODevice::WriteOnly )){
      QTextStream ts(&f);
      ts << "#V4.10" << endl;
      if(sweepdata.maudio2){
        ts << 1 << endl;
      }else{
        ts << 0 << endl;
      }
      ts << sweepdata.anfangsfrequenz << endl;
      ts << sweepdata.schrittfrequenz << endl;
      //qDebug() << "Neu 2: " << wobdaten.freq_step;
      ts << sweepdata.cntpoints << endl;
      ts << sweepdata.faktor1 << endl;
      ts << sweepdata.shift1 << endl;
      ts << sweepdata.faktor2 << endl;
      ts << sweepdata.shift2 << endl;
      switch(sweepdata.eoperatemode){
       case eGainSweep:
         if(sweepdata.linear1 or sweepdata.linear2){
           //linear == 2
           ts << 2 << endl;
         }else{
           //log == 0
           ts << 0 << endl;
         }
         break;
       case eSWR:
         //swr == 1
         ts << 1 << endl;
         break;
       case eSWRfeeder:
         //swr == 3
         ts << 3 << endl;
         break;
       case eImpedance:
         //impedanz == 4
         ts << 4 << endl;
         break;
       case eSpectrumAnalyser:
       case eSpectrumDisplayShift:
         //speky == 5
         ts << 5 << endl;
         ts << sweepdata.psavabs << endl;
         ts << sweepdata.psav300 << endl;
         ts << sweepdata.psav7k << endl;
         ts << sweepdata.psav30k << endl;
         switch(sweepdata.ebbsav){
           case ekeinebandbreite:
             ts << 0 << endl;
             break;
           case e300b1:
             ts << 1 << endl;
             break;
           case e7kb1:
             ts << 2 << endl;
             break;
           case e30kb1:
             ts << 3 << endl;
             break;
           case e300b2:
             ts << 4 << endl;
             break;
           case e7kb2:
             ts << 5 << endl;
             break;
           case e30kb2:
             ts << 6 << endl;
             break;
         }
         ts << sweepdata.display_shift << endl;
         break;
       case eSmithDiagramm:
         break;
      }
      //Messung speichern
      for(i=0; i<sweepdata.cntpoints; i++){
        ts << sweepdata.mess.k1[i] << endl;
      }
      for(i=0; i<sweepdata.cntpoints; i++){
        ts << sweepdata.mess.k2[i] << endl;
      }
      //Beschreibungstext speichern
      qDebug() << infoueber.count();
      //keine Beschreibung?
      if(infoueber.count() == 0){
        infoueber.append(";no_label");
      }else{
        // max 4 Zeilen zusammenfassen
        j = infoueber.count();
        if(j > 4)j=4;
        qs = infoueber.at(0);
        for(i=1;i<j;i++){
          qs = qs + "@" + infoueber.at(i);
        }
        //die Leerzeichen muessen entfernt werden
        qDebug() << qs;
        while(qs.contains(' ')){
          qs = qs.replace(' ', '_');
        }
        qDebug() << qs;
      }
      ts << qs << endl;
      //Kalibrierung speichern
      for(i=0; i < maxmesspunkte; i++){
        ts << sweepdata.kalibrierarray.arrayk1[i] << endl;
      }
      for(i=0; i < maxmesspunkte; i++){
        ts << sweepdata.kalibrierarray.arrayk2[i] << endl;
      }
      for(i=0; i < maxmesspunkte; i++){
        ts << sweepdata.swrkalibrierarray.arrayk1[i] << endl;
      }
      for(i=0; i < maxmesspunkte; i++){
        ts << sweepdata.swrkalibrierarray.arrayk2[i] << endl;
      }
      for(i=0; i < calmaxindex; i++){
        ts << sweepdata.savcalarray.p[i] << endl;
      }
      f.close();
    }
    QString finfo = s;
    if((finfo.indexOf(".")== -1)) s += ".hfd";
    finfo.replace(QString(".hfd"), ".info");
    f.setFileName(finfo);
    if(f.open( QIODevice::WriteOnly )){
      QTextStream ts(&f);
      for(i=0;i < infoueber.count();i++){
        ts << infoueber.at(i);
        ts << "\r\n" << flush;
      }
      for(i=0;i < mleditlist.count();i++){
        ts << mleditlist.at(i);
        ts << "\r\n" << flush;
      }
      f.close();
    }
    if(sweepdata.eoperatemode == eGainSweep){
      if(QMessageBox::question(this, tr("Making CSV-file",""), 
                                     tr("<b>A CSV-file of the same name has been made also,</b><br>Save the File ?",""),
                                     tr("&Yes",""), tr("&No",""),
                                        QString::null, 1, 0) == 0){
        if((s.indexOf(".")== -1)) s += ".hfd";
        s.replace(QString(".hfd"), ".csv");
        f.setFileName(s);
        if(f.open( QIODevice::WriteOnly )){
          QTextStream ts(&f);
          double anfangsfrq;
          double freq_step_width;
          double faktor1;
          double faktor2;
          double shift1;
          double shift2;
          //int freq_step;
          int k1, k2;
          double f1, f2, db1, db2, frq;
          QString qs1;
          
          anfangsfrq = sweepdata.anfangsfrequenz;
          freq_step_width = sweepdata.schrittfrequenz;
          //freq_step = wobdaten.freq_step;
          faktor1 = sweepdata.faktor1;
          faktor2 = sweepdata.faktor2;
          shift1 = sweepdata.shift1;
          shift2 = sweepdata.shift2;
          if(sweepdata.linear1 or sweepdata.linear2){
            ts << "Frequenz(Hz)";
            if(sweepdata.bkanal1)
              ts << ";Kanal1;Kanal1(dB)";
            if(sweepdata.bkanal2)
              ts << ";Kanal2;Kanal2(dB)";
            ts << endl;
          }else{
            ts << "Frequenz(Hz)";
            if(sweepdata.bkanal1)
              ts << ";Kanal1(dB)";
            if(sweepdata.bkanal2)
              ts << ";Kanal2(dB)";
            ts << endl;
          }
          for(i=0; i<sweepdata.cntpoints; i++){
            k1 = sweepdata.mess.k1[i];
            k2 = sweepdata.mess.k2[i];
            db1 = double(k1) * faktor1 + shift1;
            db2 = double(k2) * faktor2 + shift2;
            frq = anfangsfrq + (double(i) * freq_step_width);
            if(sweepdata.linear1 or sweepdata.linear2){
              f1 = db1;
              f2 = db2;
              f1 = (100.0 + f1) / 100.0;
              db1 = 20.0 * log10(f1);
              f2 = (100.0 + f2) / 100.0;
              db2 = 20.0 * log10(f2);
              qs.sprintf("%9.0f", frq);
              if(sweepdata.bkanal1){
                qs1.sprintf(";%1.5f;%2.5f",f1,db1);
                qs = qs + qs1;
              }
              if(sweepdata.bkanal2){
                qs1.sprintf(";%1.5f;%2.5f",f2,db2);
                qs = qs + qs1;
              }
            }else{
              db1 = db1 - getkalibrierwertk1(frq) + sweepdata.dbshift1 + double(sweepdata.display_shift);
              db2 = db2 - getkalibrierwertk2(frq) + sweepdata.dbshift2 + double(sweepdata.display_shift);
              qs.sprintf("%9.0f", frq);
              if(sweepdata.bkanal1){
                qs1.sprintf(";%2.5f",db1);
                qs = qs + qs1;
              }
              if(sweepdata.bkanal2){
                qs1.sprintf(";%2.5f",db2);
                qs = qs + qs1;
              }
            }
            qs.replace('.',',');
            ts << qs << endl;
          }
          f.close();
        }
      }
    }
  }
}

double Nwt7MainClass::korrsavpegel(double p)
{
  #ifdef LDEBUG
    qDebug("Nwt7linux::korrsavpegel(double p)");
  #endif
  if(sweepdata.ebbsav == e300b1)p = p + sweepdata.psav300;
  if(sweepdata.ebbsav == e7kb1)p = p + sweepdata.psav7k;
  if(sweepdata.ebbsav == e30kb1)p = p + sweepdata.psav30k;
  if(sweepdata.ebbsav == e300b2)p = p + sweepdata.psav300;
  if(sweepdata.ebbsav == e7kb2)p = p + sweepdata.psav7k;
  if(sweepdata.ebbsav == e30kb2)p = p + sweepdata.psav30k;
  p = p + sweepdata.psavabs;
  return p;
}

double Nwt7MainClass::getkalibrierwertk1(double afrequenz)
{
  double j;
  int i;
  
  #ifdef LDEBUG
    qDebug("WidgetWobbeln::getkalibrierwertk1()");
  #endif
  if(currentdata.kalibrierk1 and sweepdata.eoperatemode == eGainSweep)
  {
    //qDebug("WidgetWobbeln::getkalibrierwertk1()");
    j = round(afrequenz / eteiler);
    i = int(j);
    if(i > (maxmesspunkte-1))i = maxmesspunkte-1;
    return sweepdata.kalibrierarray.arrayk1[i];
  }else{
    return 0.0;
  }
}

double Nwt7MainClass::getkalibrierwertk2(double afrequenz)
{
  double j;
  int i;
  
  #ifdef LDEBUG
     qDebug("WidgetWobbeln::getkalibrierwertk1()");
  #endif
  if(currentdata.kalibrierk2 and sweepdata.eoperatemode == eGainSweep)
  {
    //qDebug("WidgetWobbeln::getkalibrierwertk1()");
    j = round(afrequenz / eteiler);
    i = int(j);
    if(i > (maxmesspunkte-1))i = maxmesspunkte-1;
    return sweepdata.kalibrierarray.arrayk2[i];
  }else{
    return 0.0;
  }
}

double Nwt7MainClass::getsavcalwert(double afrequenz)
{
  int a,b;
  double pegela, pegelb, pegeldiff;
  double frequa, frequb, frequdiff;
  
  #ifdef LDEBUG
    qDebug("Nwt7linux::getsavcalwert()");
  #endif
  //aus der Frequenz den Index errechnen
  // Bereich 0 - 100 GHz
  a = int(savcalkonst * log10(afrequenz));
  b = a + 1;
  //arraygrenzen einhalten
  if(a<0)a=0;
  if(a > calmaxindex-1)a = calmaxindex-1;
  if(b > calmaxindex-1)b = calmaxindex-1;
  pegela = sweepdata.savcalarray.p[a];
  // Ist das Ende des Array erreicht?
  if(a != b){
    //Eine Iteration ist moeglich
    pegelb = sweepdata.savcalarray.p[b];
    pegeldiff = pegelb - pegela;
    frequa = pow(10.0,(double(a)/savcalkonst));
    frequb = pow(10.0,(double(b)/savcalkonst));
    frequdiff = frequb - frequa;
    //Iteration zwischen PegelA und PegelB
    pegela = pegela + (((afrequenz - frequa)/frequdiff) * pegeldiff);
    //qDebug("index %i Pegel %f",i, pegel);
  }
  return (pegela);
}

double Nwt7MainClass::getswrkalibrierwertk1(double afrequenz)
{
  double j;
  int i;
  double w1;
  
  #ifdef LDEBUG
    qDebug("Nwt7linux::getswrkalibrierwertek1()");
  #endif
  j = round(afrequenz / eteiler);
  i = int(j);
  if(i > (maxmesspunkte-3))i = maxmesspunkte-3;
//  return wswrkalibrierarray.arrayk1[i];
  w1 = (sweepdata.swrkalibrierarray.arrayk1[i-2] +
        sweepdata.swrkalibrierarray.arrayk1[i-1] +
        sweepdata.swrkalibrierarray.arrayk1[i] +
        sweepdata.swrkalibrierarray.arrayk1[i+1] +
    sweepdata.swrkalibrierarray.arrayk1[i+2]) / 5.0;
  return (w1);
}


///////////////////////////////////////////////////////////////////////////////
// Funktionen im Wobbelbereich
void Nwt7MainClass::setInfoFont(){
  bool ok;

  infofont = QFontDialog::getFont( &ok, infofont, this);
  if(ok){
    mledit->setFont(infofont);
  }
}

void Nwt7MainClass::messsondenfilek1laden(const QString &filename){
  int i;
  QFile f;
  QString qs;
  QString s;
  QString snum;
  QString version;
  double faktor;
  double shift;
  
  s = filename;
  if (!s.isNull()){
    if(bcurveloaded){
      sweepdata = wobdatenmerk;
      sweepdata.mess.daten_enable = TRUE;
      sweepdata.bhintergrund = FALSE;
      sweepdata.mousesperre = FALSE;
      bcurveloaded = FALSE;
      fsendsweepdata();
    }
//    betriebsart->setCurrentIndex (0);
    QFileInfo fi(s);
    filenamesonde1 = fi.baseName();
    ConfigFile nwt7sondenfile;
    nwt7sondenfile.open(homedir.filePath(s));
    qs = nwt7sondenfile.readString("Sonde", "AD8307intern");
    checkboxk1->setText(filenamesonde1+".hfm");
    sweepdata.beschreibung1 = qs;
    qs = nwt7sondenfile.readString("dateiversion", "#V3.05");
    if(qs == "#V3.05")sw305 = TRUE;
    faktor = nwt7sondenfile.readDouble("faktor", 0.191187);
    shift = nwt7sondenfile.readDouble("shift", -87.139634);
    sweepdata.linear1 = nwt7sondenfile.readBool("linear", FALSE);
    //Ab Firmware 1.20
    sweepdata.maudio1 = nwt7sondenfile.readBool("audio_nf", FALSE);
    //Umrechnung der Messsondendaten wenn nicht die richtige Version
    if(!sweepdata.linear1){
      //log
      if(sw305){
        sweepdata.faktor1 = faktor;
        sweepdata.shift1 = shift;
      }else{
        sweepdata.faktor1 = faktor / 10.23;
        sweepdata.shift1 = ((((511.5 - shift) / faktor) * sweepdata.faktor1) + 40.0) * -1.0;
      }
    }else{
      //lin
      if(sw305){
        sweepdata.faktor1 = faktor;
        sweepdata.shift1 = shift;
      }else{
        sweepdata.faktor1 = faktor / 9.3;
        sweepdata.shift1 = ((((465.0 - shift) / faktor) * sweepdata.faktor1) + 50.0) * -1.0;
      }
    }
    /*
    qDebug(qs);
    qDebug("f %f",wobdaten.faktor1);
    qDebug("s %f",wobdaten.shift1);
    */
    sweepdata.mfaktor1 = nwt7sondenfile.readDouble("messx", 0.193143);
    if(fwversion == 120){
      if(sweepdata.mfaktor1 > 0.01)sweepdata.mfaktor1 = sweepdata.mfaktor1 / 32.0;
    }
    sweepdata.mshift1 = nwt7sondenfile.readDouble("messy", -84.634597);
    //wobdaten.beichkorrk1 = nwt7sondenfile.readBool("eichkorrk1", FALSE);
    wattoffsetk1[0].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr0", "0 - 10 MHz +/-0,0dB");
    wattoffsetk1[1].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr1", "10 - 20 MHz  0,8dB");
    wattoffsetk1[2].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr2", "20 - 50 MHz  1,0dB");
    wattoffsetk1[3].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr3", "50 - 100 MHz  2,0dB");
    wattoffsetk1[4].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr4", "100 - 150 MHz  2,5dB");
    wattoffsetk1[5].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr5", "150 - 200 MHz  3,0dB");
    wattoffsetk1[6].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr6", "200 - 300 MHz  4,0dB");
    wattoffsetk1[7].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr7", "300 - 400 MHz  6,0dB");
    wattoffsetk1[8].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr8", "400 - 500 MHz  9,0dB");
    wattoffsetk1[9].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr9", "NN");
    wattoffsetk1[10].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr10", "NN");
    wattoffsetk1[11].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr11", "NN");
    wattoffsetk1[12].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr12", "NN");
    wattoffsetk1[13].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr13", "NN");
    wattoffsetk1[14].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr14", "NN");
    wattoffsetk1[15].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr15", "NN");
    wattoffsetk1[16].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr16", "NN");
    wattoffsetk1[17].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr17", "NN");
    wattoffsetk1[18].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr18", "NN");
    wattoffsetk1[19].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr19", "NN");
    wattoffsetk1[20].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr20", "NN");
    wattoffsetk1[21].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr21", "NN");
    wattoffsetk1[22].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr22", "NN");
    wattoffsetk1[23].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr23", "NN");
    wattoffsetk1[24].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr24", "NN");
    wattoffsetk1[25].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr25", "NN");
    wattoffsetk1[26].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr26", "NN");
    wattoffsetk1[27].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr27", "NN");
    wattoffsetk1[28].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr28", "NN");
    wattoffsetk1[29].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr29", "NN");
    boxwattoffset1->clear();
    for(i=0;i<30;i++){
      snum.sprintf("%2i| ",i+1);
      boxwattoffset1->addItem(snum + wattoffsetk1[i].str_offsetwatt);
    }
    wattoffsetk1[0].offsetwatt = nwt7sondenfile.readDouble("wattoffset0", 0.0);
    wattoffsetk1[1].offsetwatt = nwt7sondenfile.readDouble("wattoffset1", 0.8);
    wattoffsetk1[2].offsetwatt = nwt7sondenfile.readDouble("wattoffset2", 1.0);
    wattoffsetk1[3].offsetwatt = nwt7sondenfile.readDouble("wattoffset3", 2.0);
    wattoffsetk1[4].offsetwatt = nwt7sondenfile.readDouble("wattoffset4", 2.5);
    wattoffsetk1[5].offsetwatt = nwt7sondenfile.readDouble("wattoffset5", 3.0);
    wattoffsetk1[6].offsetwatt = nwt7sondenfile.readDouble("wattoffset6", 4.0);
    wattoffsetk1[7].offsetwatt = nwt7sondenfile.readDouble("wattoffset7", 6.0);
    wattoffsetk1[8].offsetwatt = nwt7sondenfile.readDouble("wattoffset8", 9.0);
    wattoffsetk1[9].offsetwatt = nwt7sondenfile.readDouble("wattoffset9", 0.0);
    wattoffsetk1[10].offsetwatt = nwt7sondenfile.readDouble("wattoffset10", 0.0);
    wattoffsetk1[11].offsetwatt = nwt7sondenfile.readDouble("wattoffset11", 0.0);
    wattoffsetk1[12].offsetwatt = nwt7sondenfile.readDouble("wattoffset12", 0.0);
    wattoffsetk1[13].offsetwatt = nwt7sondenfile.readDouble("wattoffset13", 0.0);
    wattoffsetk1[14].offsetwatt = nwt7sondenfile.readDouble("wattoffset14", 0.0);
    wattoffsetk1[15].offsetwatt = nwt7sondenfile.readDouble("wattoffset15", 0.0);
    wattoffsetk1[16].offsetwatt = nwt7sondenfile.readDouble("wattoffset16", 0.0);
    wattoffsetk1[17].offsetwatt = nwt7sondenfile.readDouble("wattoffset17", 0.0);
    wattoffsetk1[18].offsetwatt = nwt7sondenfile.readDouble("wattoffset18", 0.0);
    wattoffsetk1[19].offsetwatt = nwt7sondenfile.readDouble("wattoffset19", 0.0);
    wattoffsetk1[20].offsetwatt = nwt7sondenfile.readDouble("wattoffset20", 0.0);
    wattoffsetk1[21].offsetwatt = nwt7sondenfile.readDouble("wattoffset21", 0.0);
    wattoffsetk1[22].offsetwatt = nwt7sondenfile.readDouble("wattoffset22", 0.0);
    wattoffsetk1[23].offsetwatt = nwt7sondenfile.readDouble("wattoffset23", 0.0);
    wattoffsetk1[24].offsetwatt = nwt7sondenfile.readDouble("wattoffset24", 0.0);
    wattoffsetk1[25].offsetwatt = nwt7sondenfile.readDouble("wattoffset25", 0.0);
    wattoffsetk1[26].offsetwatt = nwt7sondenfile.readDouble("wattoffset26", 0.0);
    wattoffsetk1[27].offsetwatt = nwt7sondenfile.readDouble("wattoffset27", 0.0);
    wattoffsetk1[28].offsetwatt = nwt7sondenfile.readDouble("wattoffset28", 0.0);
    wattoffsetk1[29].offsetwatt = nwt7sondenfile.readDouble("wattoffset29", 0.0);
    nwt7sondenfile.close();
    sweepInvalid();
    testLogLin();
    s.replace(QString(".hfm"), "k1.hfe");
    //Filename setzen
    f.setFileName(homedir.filePath(s));
    if(f.open( QIODevice::ReadOnly )){
      QTextStream ts(&f);
      ts >> version;
      if(version != "#V2.02"){
        currentdata.kalibrierk1 = FALSE;
        QMessageBox::warning( this, tr("Mathemetical Correction Channel1",""),
                                    tr("<br ><b> Calibrationfile is invalid :</b> <br><br>The file has a lower version as v2.02. <br><br>The file is no longer usable, because the mathematical<br> functions have been re-programmed.<br><br><b>Please Calibrate Channel 1 again.</b><br>After a re-calibration this message shows no more.<br>","") );
        setColorBlackk1();
      }else{ 
        for(i=0; i<maxmesspunkte; i++){
          ts >> sweepdata.kalibrierarray.arrayk1[i];
        }
      }  
      f.close();
      fsendsweepdata();
    }
    if(currentdata.kalibrierk1 and !sweepdata.linear1){
      setColorRedk1();
    }else{
      setColorBlackk1();
    }
    s.replace(QString("k1.hfe"), "k1.hfs");
    //Filename setzen fuer SWR Eichdatei lesen
    f.setFileName(homedir.filePath(s));
    if(f.open( QIODevice::ReadOnly )){
      QTextStream ts(&f);
      for(i=0; i<maxmesspunkte; i++){
        ts >> sweepdata.swrkalibrierarray.arrayk1[i];
      }
      f.close();
      emit sendsweepdata(sweepdata);
    }else{
      sweepdata.bkalibrierswrk1 = FALSE;
    }
    f.setFileName(homedir.filePath("sav.cal"));
    bool bsavcal;
    bsavcal = f.open( QIODevice::ReadOnly);
    if(bsavcal){
      //qDebug("Laden SAV Kalibrierung");
      QTextStream ts(&f);
      ts >> qs;
      if(qs == "#V4.04"){
        bsavcal = TRUE;
      }else{
        bsavcal = FALSE;
      }
      for(i=0; i < calmaxindex; i++){
        ts >> sweepdata.savcalarray.p[i];
      }
      f.close();
    }
    if(!bsavcal){
      //qDebug("kein Laden SAV Kalibrierung");
      for(i=0; i < calmaxindex; i++){
        sweepdata.savcalarray.p[i] = 0.0;
      }
    }
    emit sendsweepdata(sweepdata);
    //Daten ins Grafikfenster uebermitteln und neu darstellen
    fsendsweepdata();
  }
};

void Nwt7MainClass::messsondenfilek2laden(const QString &filename){
  int i;
  QFile f;
  QString qs;
  QString s;
  QString snum;
  QString version;
  double faktor;
  double shift;
  
  s = filename;
  if (!s.isNull()){
    if(bcurveloaded){
      sweepdata = wobdatenmerk;
      sweepdata.mess.daten_enable = TRUE;
      sweepdata.bhintergrund = FALSE;
      sweepdata.mousesperre = FALSE;
      bcurveloaded = FALSE;
      fsendsweepdata();
    }
    //betriebsart->setCurrentIndex (0);
    QFileInfo fi(s);
    filenamesonde2 = fi.baseName();
    checkboxk2->setText(filenamesonde2+".hfm");
    ConfigFile nwt7sondenfile;
    nwt7sondenfile.open(homedir.filePath(s));
    qs = nwt7sondenfile.readString("Sonde", "AD8307externK2");
    //checkboxk2->setText(qs);
    sweepdata.beschreibung2 = qs;
    //Wenn es die Versionsangabe noch nicht gibt, alte Version einblenden
    qs = nwt7sondenfile.readString("dateiversion", "#V3.05");
    faktor = nwt7sondenfile.readDouble("faktor", 0.191187);
    shift = nwt7sondenfile.readDouble("shift", -87.139634);
    sweepdata.linear2 = nwt7sondenfile.readBool("linear", FALSE);
    //Ab Firmware 1.20
    sweepdata.maudio2 = nwt7sondenfile.readBool("audio_nf", FALSE);
    if(!sweepdata.linear2){
      if(qs == "#V3.05"){
        sweepdata.faktor2 = faktor;
        sweepdata.shift2 = shift;
      }else{
        sweepdata.faktor2 = faktor / 10.23;
        sweepdata.shift2 = ((((511.5 - shift) / faktor) * sweepdata.faktor2) + 40.0) * -1.0;
      }
    }else{
      if(qs == "#V3.05"){
        sweepdata.faktor2 = faktor;
        sweepdata.shift2 = shift;
      }else{
        sweepdata.faktor2 = faktor / 9.3;
        sweepdata.shift2 = ((((465.0 - shift) / faktor) * sweepdata.faktor2) + 50.0) * -1.0;
      }
    }
    sweepdata.mfaktor2 = nwt7sondenfile.readDouble("messx", 0.193143);
    if(fwversion == 120){
      if(sweepdata.mfaktor2 > 0.01)sweepdata.mfaktor2 = sweepdata.mfaktor2 /32.0;
    }
    sweepdata.mshift2 = nwt7sondenfile.readDouble("messy", -84.634597);
    //wobdaten.beichkorrk2 = nwt7sondenfile.readBool("eichkorrk2", FALSE);
    wattoffsetk2[0].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr0", "0 - 10 MHz +/-0,0dB");
    wattoffsetk2[1].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr1", "10 - 20 MHz  0,8dB");
    wattoffsetk2[2].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr2", "20 - 50 MHz  1,0dB");
    wattoffsetk2[3].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr3", "50 - 100 MHz  2,0dB");
    wattoffsetk2[4].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr4", "100 - 150 MHz  2,5dB");
    wattoffsetk2[5].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr5", "150 - 200 MHz  3,0dB");
    wattoffsetk2[6].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr6", "200 - 300 MHz  4,0dB");
    wattoffsetk2[7].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr7", "300 - 400 MHz  6,0dB");
    wattoffsetk2[8].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr8", "400 - 500 MHz  9,0dB");
    wattoffsetk2[9].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr9", "NN");
    wattoffsetk2[10].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr10", "NN");
    wattoffsetk2[11].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr11", "NN");
    wattoffsetk2[12].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr12", "NN");
    wattoffsetk2[13].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr13", "NN");
    wattoffsetk2[14].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr14", "NN");
    wattoffsetk2[15].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr15", "NN");
    wattoffsetk2[16].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr16", "NN");
    wattoffsetk2[17].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr17", "NN");
    wattoffsetk2[18].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr18", "NN");
    wattoffsetk2[19].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr19", "NN");
    wattoffsetk2[20].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr20", "NN");
    wattoffsetk2[21].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr21", "NN");
    wattoffsetk2[22].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr22", "NN");
    wattoffsetk2[23].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr23", "NN");
    wattoffsetk2[24].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr24", "NN");
    wattoffsetk2[25].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr25", "NN");
    wattoffsetk2[26].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr26", "NN");
    wattoffsetk2[27].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr27", "NN");
    wattoffsetk2[28].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr28", "NN");
    wattoffsetk2[29].str_offsetwatt = nwt7sondenfile.readString("wattoffsetstr29", "NN");
    boxwattoffset2->clear();
    for(i=0;i<30;i++){
      snum.sprintf("%2i| ",i+1);
      boxwattoffset2->addItem(snum + wattoffsetk2[i].str_offsetwatt);
    }
    wattoffsetk2[0].offsetwatt = nwt7sondenfile.readDouble("wattoffset0", 0.0);
    wattoffsetk2[1].offsetwatt = nwt7sondenfile.readDouble("wattoffset1", 0.8);
    wattoffsetk2[2].offsetwatt = nwt7sondenfile.readDouble("wattoffset2", 1.0);
    wattoffsetk2[3].offsetwatt = nwt7sondenfile.readDouble("wattoffset3", 2.0);
    wattoffsetk2[4].offsetwatt = nwt7sondenfile.readDouble("wattoffset4", 2.5);
    wattoffsetk2[5].offsetwatt = nwt7sondenfile.readDouble("wattoffset5", 3.0);
    wattoffsetk2[6].offsetwatt = nwt7sondenfile.readDouble("wattoffset6", 4.0);
    wattoffsetk2[7].offsetwatt = nwt7sondenfile.readDouble("wattoffset7", 6.0);
    wattoffsetk2[8].offsetwatt = nwt7sondenfile.readDouble("wattoffset8", 9.0);
    wattoffsetk2[9].offsetwatt = nwt7sondenfile.readDouble("wattoffset9", 0.0);
    wattoffsetk2[10].offsetwatt = nwt7sondenfile.readDouble("wattoffset10", 0.0);
    wattoffsetk2[11].offsetwatt = nwt7sondenfile.readDouble("wattoffset11", 0.0);
    wattoffsetk2[12].offsetwatt = nwt7sondenfile.readDouble("wattoffset12", 0.0);
    wattoffsetk2[13].offsetwatt = nwt7sondenfile.readDouble("wattoffset13", 0.0);
    wattoffsetk2[14].offsetwatt = nwt7sondenfile.readDouble("wattoffset14", 0.0);
    wattoffsetk2[15].offsetwatt = nwt7sondenfile.readDouble("wattoffset15", 0.0);
    wattoffsetk2[16].offsetwatt = nwt7sondenfile.readDouble("wattoffset16", 0.0);
    wattoffsetk2[17].offsetwatt = nwt7sondenfile.readDouble("wattoffset17", 0.0);
    wattoffsetk2[18].offsetwatt = nwt7sondenfile.readDouble("wattoffset18", 0.0);
    wattoffsetk2[19].offsetwatt = nwt7sondenfile.readDouble("wattoffset19", 0.0);
    wattoffsetk2[20].offsetwatt = nwt7sondenfile.readDouble("wattoffset20", 0.0);
    wattoffsetk2[21].offsetwatt = nwt7sondenfile.readDouble("wattoffset21", 0.0);
    wattoffsetk2[22].offsetwatt = nwt7sondenfile.readDouble("wattoffset22", 0.0);
    wattoffsetk2[23].offsetwatt = nwt7sondenfile.readDouble("wattoffset23", 0.0);
    wattoffsetk2[24].offsetwatt = nwt7sondenfile.readDouble("wattoffset24", 0.0);
    wattoffsetk2[25].offsetwatt = nwt7sondenfile.readDouble("wattoffset25", 0.0);
    wattoffsetk2[26].offsetwatt = nwt7sondenfile.readDouble("wattoffset26", 0.0);
    wattoffsetk2[27].offsetwatt = nwt7sondenfile.readDouble("wattoffset27", 0.0);
    wattoffsetk2[28].offsetwatt = nwt7sondenfile.readDouble("wattoffset28", 0.0);
    wattoffsetk2[29].offsetwatt = nwt7sondenfile.readDouble("wattoffset29", 0.0);
    //if(wobdaten.linear2)wobdaten.beichkorrk2 = FALSE;
    nwt7sondenfile.close();
    sweepInvalid();
    //Daten ins Grafikfenster uebermitteln und neu darstellen
    testLogLin();
    //Filename setzen
    s.replace(QString(".hfm"), "k2.hfe");
    f.setFileName(homedir.filePath(s));
    if(f.open( QIODevice::ReadOnly )){
      QTextStream ts(&f);
      ts >> version;
      if(version != "#V2.02"){
        currentdata.kalibrierk2 = FALSE;
        QMessageBox::warning( this, tr("Mathemetical Correction Channel 2",""),
                                    tr("<br ><b> Calibrationfile is invalid :</b> <br><br>The file has a lower version as v2.02. <br><br>The file is no longer usable, because the mathematical<br> functions have been re-programmed.<br><br><b>Please Calibrate Channel 1 again.</b><br>After a re-calibration this message shows no more.<br>","") );
        setColorBlackk2();
      }else{
        for(i=0; i<maxmesspunkte; i++){
          ts >> sweepdata.kalibrierarray.arrayk2[i];
        }
      }
      f.close();
      emit sendsweepdata(sweepdata);
    }
    if(currentdata.kalibrierk2 and !sweepdata.linear2){
      setColorRedk2();
    }else{
      setColorBlackk2();
    }
    //Filename setzen fuer SWR Eichdatei lesen
    s.replace(QString("k2.hfe"), "k2.hfs");
    f.setFileName(homedir.filePath(s));
    if(f.open( QIODevice::ReadOnly )){
      qDebug("k2 lesen");
      QTextStream ts(&f);
      for(i=0; i<maxmesspunkte; i++){
        ts >> sweepdata.swrkalibrierarray.arrayk2[i];
      }
      f.close();
      emit sendsweepdata(sweepdata);
    }else{
      sweepdata.bkalibrierswrk2 = FALSE;
    }
    //Daten ins Grafikfenster uebermitteln und neu darstellen
    fsendsweepdata();
  }
};

void Nwt7MainClass::messurechanel1(){
  QString s1;
  s1 = tr("NWT Calibrationfile (*.hfm)","");
  //qDebug(s1);
  QString s = QFileDialog::getOpenFileName(
              this,
              tr("Load NWT Channel 1 Calibrationfile",""),
              homedir.filePath("*.hfm"),
              s1, 0,0 );
  messsondenfilek1laden(s);
  QFileInfo fi(s);
  filenamesonde1 = fi.baseName();

}

void Nwt7MainClass::messurechanel2(){
  QString s1;
  s1 = tr("NWT Calibrationfile (*.hfm)","");
  QString s = QFileDialog::getOpenFileName(
              this,
              tr("Load NWT Channel 2  Calibrationfile",""),
              homedir.filePath("*.hfm"),
              s1,0,0 );
  messsondenfilek2laden(s);
  QFileInfo fi(s);
  filenamesonde2 = fi.baseName();
}

void Nwt7MainClass::messsondespeichern1(const QString &sa){
  QString s1 = sa;
  QString s2;
  QString beschr;
  QString sondenname;
  QString s, smerk;
  QFile f;
  int i;
  bool ok = FALSE;
  
  //kein erster Start normales abspeichern
  if(!bersterstart){
    beschr = tr("<b> Name of the probe on the check button </b> <br> Give the measuring probe a name or <br> the given name take over:","");
  }
  //default Dateiname verwenden
  if(s1 == "def"){
    if(sweepdata.eoperatemode == eGainSweep){
      s1 = sweepdata.namesonde1;
    }
  } 
  s2 = tr("NWT Calibrationfile (*.hfm)","");

  //Datei ueberpruefen ob Sufix vorhanden
  if(!s1.contains(".hfm")) s1 += ".hfm";
  if(!bersterstart){
    /*
    sondenname = QInputDialog::getText(tr("Messsondenname","InputDialog"), beschr,
                 QLineEdit::Normal,
                 checkboxk1->text(), &ok, this);
    */
    ok = true;
  }else{
    sondenname = checkboxk1->text();
    ok = TRUE;
  }
  if(ok){
    //kein erster Start normales abspeichern
    if(!bersterstart){
      s = QFileDialog::getSaveFileName(
                  this,
                  tr("Save NWT Calibrationfile",""),
                  homedir.filePath(s1),
                  s2, 0,0);
    }else{
      s = homedir.filePath(s1);
    }
    if (!s.isNull())
    {
      //Datei ueberpruefen ob Sufix vorhanden
      if((s.indexOf(".hfm")== -1)) s += ".hfm";
    }
    smerk = s; //Dateinamen merken
    QFileInfo fi(s);
    filenamesonde1 = fi.baseName();
    //qDebug()<< "messsondespeichern1:"<<s;
    ConfigFile nwt7sondenfile;
    nwt7sondenfile.open(s);
    if((sweepdata.eoperatemode == eGainSweep)
       or (sweepdata.eoperatemode == eSWR)
       ){
      //wobdaten = grafik->getWobbeldaten();
      nwt7sondenfile.writeString("dateiversion", "#V3.05");
      nwt7sondenfile.writeString("Sonde", sondenname);
      nwt7sondenfile.writeDouble("faktor", sweepdata.faktor1);
      nwt7sondenfile.writeDouble("shift", sweepdata.shift1);
      nwt7sondenfile.writeBool("linear", sweepdata.linear1);
      nwt7sondenfile.writeBool("audio_nf", sweepdata.maudio1);
      nwt7sondenfile.writeDouble("messx", sweepdata.mfaktor1);
      nwt7sondenfile.writeDouble("messy", sweepdata.mshift1);
      //wobdaten.beichkorrk1 = weichen.eichk1;
      //nwt7sondenfile.writeBool("eichkorrk1", wobdaten.beichkorrk1);
      nwt7sondenfile.writeString("wattoffsetstr0", wattoffsetk1[0].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr1", wattoffsetk1[1].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr2", wattoffsetk1[2].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr3", wattoffsetk1[3].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr4", wattoffsetk1[4].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr5", wattoffsetk1[5].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr6", wattoffsetk1[6].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr7", wattoffsetk1[7].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr8", wattoffsetk1[8].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr9", wattoffsetk1[9].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr10", wattoffsetk1[10].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr11", wattoffsetk1[11].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr12", wattoffsetk1[12].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr13", wattoffsetk1[13].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr14", wattoffsetk1[14].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr15", wattoffsetk1[15].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr16", wattoffsetk1[16].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr17", wattoffsetk1[17].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr18", wattoffsetk1[18].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr19", wattoffsetk1[19].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr20", wattoffsetk1[20].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr21", wattoffsetk1[21].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr22", wattoffsetk1[22].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr23", wattoffsetk1[23].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr24", wattoffsetk1[24].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr25", wattoffsetk1[25].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr26", wattoffsetk1[26].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr27", wattoffsetk1[27].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr28", wattoffsetk1[28].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr29", wattoffsetk1[29].str_offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset0", wattoffsetk1[0].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset1", wattoffsetk1[1].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset2", wattoffsetk1[2].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset3", wattoffsetk1[3].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset4", wattoffsetk1[4].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset5", wattoffsetk1[5].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset6", wattoffsetk1[6].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset7", wattoffsetk1[7].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset8", wattoffsetk1[8].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset9", wattoffsetk1[9].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset10", wattoffsetk1[10].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset11", wattoffsetk1[11].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset12", wattoffsetk1[12].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset13", wattoffsetk1[13].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset14", wattoffsetk1[14].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset15", wattoffsetk1[15].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset16", wattoffsetk1[16].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset17", wattoffsetk1[17].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset18", wattoffsetk1[18].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset19", wattoffsetk1[19].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset20", wattoffsetk1[20].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset21", wattoffsetk1[21].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset22", wattoffsetk1[22].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset23", wattoffsetk1[23].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset24", wattoffsetk1[24].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset25", wattoffsetk1[25].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset26", wattoffsetk1[26].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset27", wattoffsetk1[27].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset28", wattoffsetk1[28].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset29", wattoffsetk1[29].offsetwatt);
      nwt7sondenfile.close();
      checkboxk1->setText(filenamesonde1 + ".hfm");
      //Filename setzen
      //Eichkorrektur bei log. Messkopf
      if((currentdata.kalibrierk1) and (!sweepdata.linear1)){
        s.replace(QString(".hfm"), "k1.hfe");
        f.setFileName(homedir.filePath(s));
        if(f.open(QIODevice::WriteOnly)){
          QTextStream ts(&f);
          ts << "#V2.02" << endl;
          for(i=0; i<maxmesspunkte; i++){
            ts << sweepdata.kalibrierarray.arrayk1[i] << endl;
          }
          f.close();
        }
      }
    }
    if(sweepdata.eoperatemode == eSWR){
      s = smerk;
      //Filename setzen
      s.replace(QString(".hfm"), "k1.hfs");
      f.setFileName(homedir.filePath(s));
      if(f.open(QIODevice::WriteOnly)){
        #ifdef LDEBUG
          qDebug() << s;
        #endif
        QTextStream ts(&f);
        for(i=0; i<maxmesspunkte; i++){
          ts << sweepdata.swrkalibrierarray.arrayk1[i] << endl;
        }
        f.close();
      }
    }
  }
}

void Nwt7MainClass::messsondespeichern2(const QString &sa){
  QString s1 = sa;
  QString s2;
  QString beschr;
  QString sondenname;
  QString s, smerk;
  QFile f;
  int i;
  bool ok = FALSE;

  if(!bersterstart){
    beschr = tr("<b> Name of the probe on the check button </b> <br> Give the measuring probe a name or <br> the given name take over:","");
  }
  if(s1 == "def"){
    s1 = sweepdata.namesonde2;
  } 
  s2 = tr("NWT Calibrationfile (*.hfm)");
  //Datei ueberpruefen ob Sufix vorhanden
  if(!s1.contains(".hfm")) s1 += ".hfm";
  //wenn keine Erstinitialisierung Dialog oeffnen
  if(!bersterstart){
    /*
    sondenname = QInputDialog::getText(tr("Messsondenname","InputDialog"), beschr,
                               QLineEdit::Normal,
                               checkboxk2->text(), &ok, this);
    */
    ok = true;
  }else{
    sondenname = checkboxk2->text();
    ok = TRUE;
  }
  if(ok){
    //wenn keine Erstinitialisierung Dialog oeffnen
    if(!bersterstart){
      s = QFileDialog::getSaveFileName(
                  this,
                  tr("Save NWT Calibrationfile",""),
                  homedir.filePath(s1),
                  s2,0,0);
    }else{
      s = homedir.filePath(s1);
    }
    if (!s.isNull())
    {
      //Datei ueberpruefen ob Sufix vorhanden
        if(!s1.contains(".hfm")) s1 += ".hfm";
    }
    smerk = s;
    QFileInfo fi(s);
    filenamesonde2 = fi.baseName();
    //qDebug() << filenamesonde2;
    ConfigFile nwt7sondenfile;
    nwt7sondenfile.open(s);
    if(sweepdata.eoperatemode == eGainSweep){
      //wobdaten = grafik->getWobbeldaten();
      nwt7sondenfile.writeString("dateiversion", "#V3.05");
      nwt7sondenfile.writeString("Sonde", sondenname);
      nwt7sondenfile.writeDouble("faktor", sweepdata.faktor2);
      nwt7sondenfile.writeDouble("shift", sweepdata.shift2);
      nwt7sondenfile.writeBool("linear", sweepdata.linear2);
      nwt7sondenfile.writeBool("audio_nf", sweepdata.maudio2);
      nwt7sondenfile.writeDouble("messx", sweepdata.mfaktor2);
      nwt7sondenfile.writeDouble("messy", sweepdata.mshift2);
      //wobdaten.beichkorrk2 = weichen.eichk2;
      //nwt7sondenfile.writeBool("eichkorrk2", wobdaten.beichkorrk2);
      nwt7sondenfile.writeString("wattoffsetstr0", wattoffsetk2[0].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr1", wattoffsetk2[1].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr2", wattoffsetk2[2].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr3", wattoffsetk2[3].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr4", wattoffsetk2[4].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr5", wattoffsetk2[5].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr6", wattoffsetk2[6].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr7", wattoffsetk2[7].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr8", wattoffsetk2[8].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr9", wattoffsetk2[9].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr10", wattoffsetk2[10].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr11", wattoffsetk2[11].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr12", wattoffsetk2[12].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr13", wattoffsetk2[13].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr14", wattoffsetk2[14].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr15", wattoffsetk2[15].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr16", wattoffsetk2[16].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr17", wattoffsetk2[17].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr18", wattoffsetk2[18].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr19", wattoffsetk2[19].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr20", wattoffsetk2[20].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr21", wattoffsetk2[21].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr22", wattoffsetk2[22].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr23", wattoffsetk2[23].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr24", wattoffsetk2[24].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr25", wattoffsetk2[25].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr26", wattoffsetk2[26].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr27", wattoffsetk2[27].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr28", wattoffsetk2[28].str_offsetwatt);
      nwt7sondenfile.writeString("wattoffsetstr29", wattoffsetk2[29].str_offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset0", wattoffsetk2[0].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset1", wattoffsetk2[1].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset2", wattoffsetk2[2].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset3", wattoffsetk2[3].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset4", wattoffsetk2[4].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset5", wattoffsetk2[5].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset6", wattoffsetk2[6].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset7", wattoffsetk2[7].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset8", wattoffsetk2[8].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset9", wattoffsetk2[9].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset10", wattoffsetk2[10].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset11", wattoffsetk2[11].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset12", wattoffsetk2[12].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset13", wattoffsetk2[13].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset14", wattoffsetk2[14].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset15", wattoffsetk2[15].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset16", wattoffsetk2[16].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset17", wattoffsetk2[17].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset18", wattoffsetk2[18].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset19", wattoffsetk2[19].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset20", wattoffsetk2[20].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset21", wattoffsetk2[21].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset22", wattoffsetk2[22].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset23", wattoffsetk2[23].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset24", wattoffsetk2[24].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset25", wattoffsetk2[25].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset26", wattoffsetk2[26].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset27", wattoffsetk2[27].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset28", wattoffsetk2[28].offsetwatt);
      nwt7sondenfile.writeDouble("wattoffset29", wattoffsetk2[29].offsetwatt);
      nwt7sondenfile.close();
      checkboxk2->setText(filenamesonde2 + ".hfm");
      //Filename setzen
      if((currentdata.kalibrierk2) and (!sweepdata.linear2)){
        s.replace(QString(".hfm"), "k2.hfe");
        f.setFileName(homedir.filePath(s));
        if(f.open( QIODevice::WriteOnly )){
          QTextStream ts(&f);
          ts << "#V2.02" << endl;
          for(i=0; i<maxmesspunkte; i++){
            ts << sweepdata.kalibrierarray.arrayk2[i] << endl;
          }
          f.close();
        }
      }	
    }  
    if(sweepdata.eoperatemode == eSWR){
      //Filename setzen
      s = smerk;
      s.replace(QString(".hfm"), "k2.hfs");
      f.setFileName(homedir.filePath(s));
      if(f.open( QIODevice::WriteOnly )){
        QTextStream ts(&f);
        for(i=0; i<maxmesspunkte; i++){
          ts << sweepdata.swrkalibrierarray.arrayk2[i] << endl;
        }
        f.close();
      }
    }
  }  
}

void Nwt7MainClass::calibratechanel1(){
  QString s;
  int i;

  if(!ttyOpen(1)) return;

  if (bttyOpen)
  for(i=0;i<maxmesspunkte;i++)
  {
    //Corresponding Array delete
    if(sweepdata.eoperatemode==eGainSweep)sweepdata.kalibrierarray.arrayk1[i] = 0.0;
    if(sweepdata.eoperatemode==eSWR)sweepdata.swrkalibrierarray.arrayk1[i] = -10.0;
  }
  if(sweepdata.eoperatemode==eGainSweep)emit sendsweepdata(sweepdata);
  if(sweepdata.eoperatemode==eSWR)emit sendsweepdata(sweepdata);
  switch(sweepdata.eoperatemode){
    case eSmithDiagramm:
      break;
    case eSpectrumDisplayShift:
      break;
    case eSpectrumAnalyser:
      break;
    case eGainSweep:
      switch( QMessageBox::information( this, tr("Channel type",""),
            tr("<b>Select Channel :</b> <br> <br>Linear- or Logarithmic-Channel ?",""),
            "L&in.", "L&og.",
            0,
            1 ) ) {
        case 0:
            // Linear
            //qDebug("Linear");
            sweepdata.linear1 = TRUE;
            testLogLin();
            emit sendsweepdata(sweepdata);
            break;
        case 1:
            // Logarithmisch
            //qDebug("Log");
            sweepdata.linear1 = FALSE;
            sweepdata.linear2 = FALSE;
            testLogLin();
            emit sendsweepdata(sweepdata);
            break;
      }
      checkboxk1->setChecked(TRUE);
      checkboxk2->setChecked(FALSE);
      buttoneinmal->setEnabled(FALSE);
      buttonwobbeln->setEnabled(FALSE);
      buttonstop->setEnabled(FALSE);
      emit setmenu(emOnce, FALSE);
      emit setmenu(emSweep, FALSE);
      emit setmenu(emStop, FALSE);
      gbZoom->setEnabled(FALSE);
      gbSweep->setEnabled(FALSE);
      gbOperatedStart->setEnabled(FALSE);
      if(currentdata.fwWrong){
        boxzwischenzeit->setCurrentIndex(0);
        boxzwischenzeit->setEnabled(FALSE);
        labelzwischenzeit->setEnabled(FALSE);
//        editscantime->setEnabled(FALSE);
//        checkboxtime->setEnabled(FALSE);
      }
      bkalibrieren40 = TRUE;
      if(!sweepdata.linear1){
        //math. Korrektur aktiv ?
        if(currentdata.kalibrierk1){
          //beim Kalibrieren andere Frequenzbereiche zulassen
          //wenn der Faktor > 1 ist
          if(sweepdata.freq_faktor == 1){
            //Frequenzbereiche ins Wobbelfenster uebertragen
            setFqStart(currentdata.calibrate_start_freq);
            setFqEnd(currentdata.calibrate_end_freq);
          }
          setFqStep(double(currentdata.kalibrierstep));
          SweepInit();
        }

        // Set -40 dB
        if (currentdata.attenuator.CanAttenuateBy(40)) {
            setAttenuationControls_dB(40);
        } else {
            setAttenuationControls_dB(0);
            QMessageBox::warning( this, tr("Channel 1 Calibration","Menue"),
                                        tr("<b>Setting the -40dB line : </b> <br><br>Insert 40dB Attenuator.<br>",""));
        }

        nwtDelay(currentdata.attenuatorDelay_ms);
      }else{
          if (currentdata.attenuator.CanAttenuateBy(6)) {
              setAttenuationControls_dB(6);
          } else {
              setAttenuationControls_dB(0);
              QMessageBox::warning( this, tr("Channel 1 Calibration","Menue"),
                                          tr("<br><b> Setting the 0.5 line : </b> <br><br>Insert 6db Attenuator.<br>",""));
          }
          nwtDelay(currentdata.attenuatorDelay_ms);
      }
      break;
    //SWR kalibrieren
    case eSWR:
      s.sprintf("%li",currentdata.kalibrierstep);
      editschritte->setText(s);
      SweepInit();
      bkalibrierenswr = TRUE;
      bkalibrieren0 = TRUE;
      QMessageBox::warning( this, tr("SWR calibration",""),
                                  tr("<br><b> Setting SWR = Infinite </b> <br><br>Returnloss-bridge Open or Shorted.","") );
      break;
    case eSWRfeeder:
    case eImpedance:
      break;
  }
  wobbelstop = TRUE;
//  fsendewobbeldaten();
  sendSweepCmd();
}

void Nwt7MainClass::calibratechanel2(){
  QString s;
  int i;

  if(!ttyOpen(1)) return;

  if (bttyOpen)
  for(i=0;i<maxmesspunkte;i++)
  {
    //Entsprechendes Array loeschen
    if(sweepdata.eoperatemode==eGainSweep)sweepdata.kalibrierarray.arrayk2[i] = 0.0;
    if(sweepdata.eoperatemode==eSWR)sweepdata.swrkalibrierarray.arrayk2[i] = -10.0;
  }
  if(sweepdata.eoperatemode==eGainSweep)emit sendsweepdata(sweepdata);
  if(sweepdata.eoperatemode==eSWR)emit sendsweepdata(sweepdata);
  switch(sweepdata.eoperatemode){
    case eSmithDiagramm:
      break;
    case eSpectrumDisplayShift:
      break;
    case eSpectrumAnalyser:
      break;
    case eGainSweep:
      switch( QMessageBox::information( this, tr("Channel type",""),
                                        tr("<b>Select Channel :</b> <br> <br>Linear- or Logarithmic-Channel ?",""),
                                        "L&in.", "L&og.",
                                        0,
                                        1 ) ) {
       case 0:
        // Linear
        // fprintf(stderr, "Linear\n");
        sweepdata.linear2 = TRUE;
        testLogLin();
        emit sendsweepdata(sweepdata);
        break;
       case 1:
        // Logarithmisch
        // fprintf(stderr, "Log\n");
        sweepdata.linear1 = FALSE;
        sweepdata.linear2 = FALSE;
        testLogLin();
        emit sendsweepdata(sweepdata);
        break;
      }
      if(fwversion == 120){
        if(QMessageBox::question(this, tr("Probe channel 2",""),
                                 tr("<B> type of the measuring head </B> <BR> Is that an audio NF probe?",""),
                                 tr("&Yes",""), tr("&No",""),
                                 QString::null, 1, 0) == 0){
          sweepdata.maudio2 = true;
          //boxzwischenzeit->setCurrentIndex(12); // Zwischenzeit auf maximum
        }
      }
      checkboxk1->setChecked(FALSE);
      checkboxk2->setChecked(TRUE);
      buttoneinmal->setEnabled(FALSE);
      buttonwobbeln->setEnabled(FALSE);
      buttonstop->setEnabled(FALSE);
      emit setmenu(emOnce, FALSE);
      emit setmenu(emSweep, FALSE);
      emit setmenu(emStop, FALSE);
      gbZoom->setEnabled(FALSE);
      gbSweep->setEnabled(FALSE);
      gbOperatedStart->setEnabled(FALSE);
      if(currentdata.fwWrong){
        boxzwischenzeit->setCurrentIndex(0);
        boxzwischenzeit->setEnabled(FALSE);
        labelzwischenzeit->setEnabled(FALSE);
//        editscantime->setEnabled(FALSE);
//        checkboxtime->setEnabled(FALSE);
      }
      bkalibrieren40 = TRUE;
      if(!sweepdata.linear2){
        if(currentdata.kalibrierk2){
          //beim Kalibrieren andere Frequenzbereiche zulassen
          //wenn der Faktor > 1 ist
          if(sweepdata.freq_faktor == 1){
          //Frequenzbereiche ins Wobbelfenster uebertragen
            setFqStart(currentdata.calibrate_start_freq);
            setFqEnd(currentdata.calibrate_end_freq);
          }
          setFqStep(double(currentdata.kalibrierstep));
          SweepInit();
        }

        // Set -40 dB
        if (currentdata.attenuator.CanAttenuateBy(40)) {
            setAttenuationControls_dB(40);
        } else {
            setAttenuationControls_dB(0);
            QMessageBox::warning( this, tr("Channel 2 Calibration","Menu"),
                                        tr("<b>Setting the -40dB line : </b> <br><br>Insert 40dB Attenuator.<br>",""));
        }
        nwtDelay(currentdata.attenuatorDelay_ms);
      }else{
          if (currentdata.attenuator.CanAttenuateBy(6)) {
              setAttenuationControls_dB(6);
          } else {
              setAttenuationControls_dB(0);
              QMessageBox::warning( this, tr("Channel 2 Calibration","Menu"),
                                          tr("<br><b> Setting the 0.5 line : </b> <br><br>Insert 6db Attenuator.<br>",""));
          }
          nwtDelay(currentdata.attenuatorDelay_ms);
      }
      break;
    //SWR kalibrieren
    case eSWR:
      s.sprintf("%li",currentdata.kalibrierstep);
      editschritte->setText(s);
      SweepInit();
      bkalibrierenswr = TRUE;
      bkalibrieren0 = TRUE;
      QMessageBox::warning( this, tr("Calibration Channel 2",""),
                                  tr("<br><b> Setting SWR = Infinite </b> <br><br>Returnloss-bridge Open or Shorted.","") );
      break;
    case eSWRfeeder:
    case eImpedance:
      break;
  }
  wobbelstop = TRUE;
//  fsendewobbeldaten();
  sendSweepCmd();
}

void Nwt7MainClass::vTimerPerform(){
  unsigned char a;
#ifdef TIMERDEBUG
  qDebug(">>vtimerbeperform()");
#endif
  vtimer->stop();
  if (bttyOpen){
    if((picmodem->readttybuffer(&a, 1)) == 1){
        #ifdef CMDDEBUG
            qDebug() << "v <--- R232 OK";
        #endif
      fwversion = int (a);
      #ifdef VERDEBUG
        qDebug("vtimerbeperform() data exist");
      #endif
      verstep = FALSE;
    }else{
      #ifdef VERDEBUG
        qDebug("vtimerbeperform() data not exist");
      #endif
        cleareVersionVariante();
        fwversion = 0;
        verstep = TRUE;
      //defaultClearTty(); //delete any data in the UART
    };
  }
  verCounterError =0;
  curTimerPerform = 0; //this timer was performed
  #ifdef VERDEBUG
    qDebug("version %i",fwversion);
  #endif
  //if (fwversion)
    setWindowTitle(currentdata.str_tty);
  //else
  //  setWindowTitle(currentdata.str_tty+" - NODATA");
#ifdef TIMERDEBUG
  qDebug(">>vtimerbeperform()");
#endif
}

void Nwt7MainClass::sTimerPerform(){
  char abuffer[4];
  unsigned char c1, c2; 
#ifdef TIMERDEBUG
  qDebug(">>sTimerPerform()");
#endif
  stimer->stop();
  if (bttyOpen){
    if((picmodem->readttybuffer(&abuffer, 4)) == 4){
        #ifdef CMDDEBUG
            qDebug() << "s <--- R232 OK";
        #endif
      fwvariante = int(abuffer[0]);
      fwportb = int(abuffer[1]);
      c1 = abuffer[2];
      c2 = abuffer[3];
      fwsav = int(c1 + c2 * 256);
      verstep = TRUE;
    }else{
      cleareVersionVariante();
      verstep = TRUE;
    }
  }
  verCounterError =0;
  curTimerPerform = 0; //this timer was performed
  #ifdef VERDEBUG
    qDebug("variant %i",fwvariante);
    qDebug("portb %i",fwportb);
    qDebug("c1 %x",c1);
    qDebug("c2 %x",c2);
    qDebug("sav %i",fwsav); 
  #endif
#ifdef TIMERDEBUG
  qDebug("<<sTimerPerform()");
#endif
}

int Nwt7MainClass::ttyOpen(int showWarning){
    // Test connection
    if(!fwversion){
        picmodem->closetty();
        bttyOpen=false;
        bSweepIsRun = FALSE;
    }
    int res = 1; //if already open
    // Process open
    if(!bttyOpen){
      bttyOpen = picmodem->opentty(currentdata.str_tty);
      if(!bttyOpen){
        if (showWarning){
            QMessageBox::warning( this, tr("Serial interface"),
                    "\"" + currentdata.str_tty + tr("\"Can not be opened!"));
        }
        #ifdef TTYDEBUG
          qDebug("Nwt7linux::ttyopen()");
        #endif
        labelnwt->setText(tr("Offline",""));
        res = 0;
      } else {
        res = 1;
      }
      fwvariante = 0;
      fwportb = 0;
      fwsav = 0;
      fwversion=0;
      bSweepIsRun = FALSE;
    }
    return res;
}

void Nwt7MainClass::setInitInterface(bool connected)
{
#ifdef DEBUG_INTERFACE
    connected = TRUE;
#endif
    int idx;
    if(!connected){
        wobbelstop = TRUE;
        idx=this->currentIndex();
        if ((idx == tab_nwt7sweep) || (idx == tab_nwt7vfo) || (idx == tab_nwt7measure)){
            setCurrentIndex(tab_nwt7sweep);
            cbOperatedMode->setCurrentIndex(eGainSweep);
        }
    }
    setTabEnabled(tab_nwt7sweep, true);
    setTabEnabled(tab_nwt7vfo, connected);
    setTabEnabled(tab_nwt7measure, connected);
    setTabEnabled(tab_wkmanager, true);
    setTabEnabled(tab_calculation, true);
    setTabEnabled(tab_wimpedanz_match, true);
    buttoneinmal->setEnabled(connected);
    buttonwobbeln->setEnabled(connected);
    buttonstop->setEnabled(connected);
    emit setmenu(emOnce, connected);
    emit setmenu(emSweep, connected);
    emit setmenu(emStop, FALSE);
    switch(sweepdata.eoperatemode){
    case eSWRfeeder:
    case eImpedance:
    case eSWR:
    case eSmithDiagramm:
        gbAttenuator->setEnabled(false);
        cbAttenuator2->setEnabled(false);
        break;
    case eGainSweep:
    case eSpectrumAnalyser:
    case eSpectrumDisplayShift:
        gbAttenuator->setEnabled(connected);
        cbAttenuator2->setEnabled(connected);
        break;
    };
    gbZoom->setEnabled(connected);
    gbSweep->setEnabled(connected);
    gbOperatedStart->setEnabled(connected);
    if(!currentdata.fwWrong){
      boxzwischenzeit->setEnabled(TRUE);
      labelzwischenzeit->setEnabled(TRUE);
    }
    // K1 or K2 linear = Measurement window disable
    if(sweepdata.linear2 || sweepdata.linear1){
      setTabEnabled(tab_nwt7measure, false);
    }else{
      if(sweepdata.eoperatemode == eGainSweep){
#ifndef DEBUG_INTERFACE
        setTabEnabled(tab_nwt7measure, connected);
#endif
      }
    }
}

void Nwt7MainClass::IdleTimerStop(){
    idletimer->stop();
    stimer->stop();
    vtimer->stop();
    curTimerPerform = 0;
}

void Nwt7MainClass::IdleTimerStart(){
    idletimer->start(currentdata.idletime);
}

void Nwt7MainClass::IdleTimerPerform(){
  //Calibration values ??for the detection of the set values ??on the ATT
  int sa75_003  = 353;
  int sa75_05   = 446;
  int sa75_30   = 544;
  int sa148_003 = 674;
  int sa148_05  = 853;

#ifdef TIMERDEBUG
  qDebug(">>IdleTimerbePerform()");
#endif
  idletimer->stop();
  ttyOpen(0);
  setInitInterface(bttyOpen  && fwversion);
  versionControl(); // Start vtimer and stimer
  //If not swept 0 Hz output at the DDS
  setIdleDdsOff();
  //Preparing for color adjustment
  QPalette palette = labelnwt->palette();
  if(fwversion != 0){
    //Dunkel Gruene Schrift
    palette.setColor(QPalette::Active, QPalette::WindowText, Qt::darkGreen);
    labelnwt->setPalette(palette);
    labelnwt->setText(tr("Online",""));
    fwversioncounter = 3;
    //setTabEnabled(tab_nwt7vfo, TRUE);
    //setTabEnabled(berechnung, TRUE);
    if(sweepdata.linear2 or sweepdata.linear1){
      setTabEnabled(tab_nwt7measure, FALSE);
    }else{
      setTabEnabled(tab_nwt7measure, TRUE);
    };
    if((fwversion > 114) and (fwversion < 150)){
      if(sweepdata.maudio2){
        if(!checkboxk1->isChecked() and checkboxk2->isChecked()){
          kalibrier1->setEnabled(false);
          kalibrier2->setChecked(false);
          kalibrier2->setEnabled(false);
          boxzwischenzeit->setCurrentIndex(0);
          boxzwischenzeit->setEnabled(FALSE);
          labelzwischenzeit->setEnabled(FALSE);
        }else{
          boxzwischenzeit->setEnabled(TRUE);
          labelzwischenzeit->setEnabled(TRUE);
          kalibrier1->setEnabled(true);
          kalibrier2->setEnabled(true);
        }
      }else{
        boxzwischenzeit->setEnabled(TRUE);
        labelzwischenzeit->setEnabled(TRUE);
      }
      //checkboxtime->setEnabled(TRUE);
      //      editscantime->setEnabled(TRUE);
      currentdata.fwWrong = FALSE;
      //fwportb comes from the HW NWT01
      //pb = fwportb;
      //pb = pb >> 1; //PortB a BIT to the left
      //qDebug("portb2 %X %i", pb, pb);
      //Rueckwaertzausw ertung des Daempfungg liedes
      //no version status polling query but was out Trough
      if(!verstep){
        // I set ATT after 2 idle cycle with "s" command
        //qDebug("PORT_B values %i %i", fwportb, fwportbalt);
        if(fwportb != fwportbalt){
            if(fwportbcounter > 0)fwportbcounter--;
        }else{
            fwportbcounter = 2;
        }
        if(fwportbcounter == 0){
              fwportbalt = fwportb;
              setAttenuationControls_dB(currentdata.attenuator.GetAttenuationByCode(fwportbalt));
        }
        //SA Setting the correct status
        if(fwsav < 10){
          labelsafrqbereich->setText(tr("No Spectrumanalyser",""));
          labelsabandbreite->setText(tr("Connected",""));
          sabw = bwkein;
          if(!bcurveloaded)sweepdata.ebbsav = ekeinebandbreite;
        }
        if((fwsav >= 10) and (fwsav < sa75_003)){
          labelsafrqbereich->setText(tr("Range 1",""));
          labelsabandbreite->setText(tr("Bandwidth : 30 kHz",""));
          if(currentdata.sastatus){
            sabereich1->setChecked(TRUE);
            sabereich2->setChecked(FALSE);
            sabereich = bsa1;
            sabw = bw30kHz;
            if(!bcurveloaded)sweepdata.ebbsav = e30kb1;
            //qDebug("30k");
          }
        }
        if((fwsav >= sa75_003) and (fwsav < sa75_05)){
          labelsafrqbereich->setText(tr("Range 1",""));
          labelsabandbreite->setText(tr("Bandwidth : 7 kHz",""));
          if(currentdata.sastatus){
            sabereich1->setChecked(TRUE);
            sabereich2->setChecked(FALSE);
            sabereich = bsa1;
            sabw = bw7kHz;
            if(!bcurveloaded)sweepdata.ebbsav = e7kb1;
            //qDebug("7k");
          }
        }
        if((fwsav >= sa75_05) and (fwsav < sa75_30)){
          labelsafrqbereich->setText(tr("Range 1",""));
          labelsabandbreite->setText(tr("Bandwidth : 300 Hz",""));
          if(currentdata.sastatus){
            sabereich1->setChecked(TRUE);
            sabereich2->setChecked(FALSE);
            sabereich = bsa1;
            sabw = bw300Hz;
            if(!bcurveloaded)sweepdata.ebbsav = e300b1;
            //qDebug("300");
          }
        }
        if((fwsav >= sa75_30) and (fwsav < sa148_003)){
          labelsafrqbereich->setText(tr("Range 2",""));
          labelsabandbreite->setText(tr("Bandwidth : 30 kHz",""));
          if(currentdata.sastatus){
            sabereich1->setChecked(FALSE);
            sabereich2->setChecked(TRUE);
            sabereich = bsa2;
            sabw = bw30kHz;
            if(!bcurveloaded)sweepdata.ebbsav = e30kb2;
            //qDebug("30k");
          }
        }
        if((fwsav >= sa148_003) and (fwsav < sa148_05)){
          labelsafrqbereich->setText(tr("Range 2",""));
          labelsabandbreite->setText(tr("Bandwidth : 7 kHz",""));
          if(currentdata.sastatus){
            sabereich1->setChecked(FALSE);
            sabereich2->setChecked(TRUE);
            sabereich = bsa2;
            sabw = bw7kHz;
            if(!bcurveloaded)sweepdata.ebbsav = e7kb2;
            //qDebug("7k");
          }
        }
        if(fwsav >= sa148_05){
          labelsafrqbereich->setText(tr("Range 2",""));
          labelsabandbreite->setText(tr("Bandwidth : 300 Hz",""));
          if(currentdata.sastatus){
            sabereich1->setChecked(FALSE);
            sabereich2->setChecked(TRUE);
            sabereich = bsa2;
            sabw = bw300Hz;
            if(!bcurveloaded)sweepdata.ebbsav = e300b2;
            //qDebug("300");
          }
        }
      }
      if((sweepdata.eoperatemode == eSpectrumAnalyser/* or wobdaten.ebetriebsart == espekdisplayshift*/) and currentdata.sastatus){
        //nur Frequenz umspeichern wenn HW Rueckmeldung und Betriebsart SA
        setFrqBereich();
      }
    }
  }else{ // fwversion = 0
    if(fwversioncounter == 0){
      // Rote Schrift
      palette.setColor(QPalette::Active, QPalette::WindowText, Qt::red);
      labelnwt->setText(tr("Offline",""));
      labelnwt->setPalette(palette);
#ifndef DEBUG_INTERFACE
      boxzwischenzeit->setCurrentIndex(0);
      boxzwischenzeit->setEnabled(FALSE);
      labelzwischenzeit->setEnabled(FALSE);
#endif
      //not recognize the correct FW or no connection to the NWT
      currentdata.fwWrong = TRUE;
      //SAV Bandbreitenerkennung AUS
      if(!bcurveloaded)sweepdata.ebbsav = ekeinebandbreite;
    }
    fwversioncounter--;
    if(fwversioncounter < 0)fwversioncounter = 0;
  }
  // Main window set text
  setWindowTitle(currentdata.str_tty);
  // Timmer idleTimerAlways except tab_nwt7measure
  //if (currentIndex() == tab_nwt7measure){
  if(restartMessure){
       switch(messVfoModeState){
        case 0: //VFO
        case 1: //MessVFO
            startMessure();
            break;
        case 2:
            /*@TODO: NEED Stop DDS OUT*/
          IdleTimerStart();
      }
  } else  IdleTimerStart();
#ifdef TIMERDEBUG
  qDebug("<<IdleTimerbePerform()");
#endif
}

// Start one sweep
void Nwt7MainClass::clickSweepContinuous(){
  SweepInit(); //Calibration of all input data
  if(sweepdestroy)return;
  if(sweepdata.numberchanel){
    emit setmenu(emCalibrateK2, FALSE);
    emit setmenu(emLoadCallibrateK2, FALSE);
    emit setmenu(emSaveCallibrateK2, FALSE);
    emit setmenu(emWattCallibrateK2, FALSE);
    emit setmenu(emWattEditSonde2, FALSE);
    emit setmenu(emVersion, FALSE);
  }else{
    emit setmenu(emCalibrateK2, TRUE);
    emit setmenu(emLoadCallibrateK2, TRUE);
    emit setmenu(emSaveCallibrateK2, TRUE);
    emit setmenu(emWattCallibrateK2, TRUE);
    emit setmenu(emWattEditSonde2, TRUE);
  }
  //versionskontrolle();
  if(!ttyOpen(1)) return;

  grafik->setWindowTitle(tr("Graphical Display",""));
  if (bcurveloaded){
    setWindowTitle(currentdata.str_tty);
    bcurveloaded = FALSE; //mode off
    sweepdata = wobdatenmerk;//Old settings back again invite
    checkboxk1->setChecked(sweepdata.bkanal1);
    checkboxk2->setChecked(sweepdata.bkanal2);
    sweepInvalid();
    switch(sweepdata.eoperatemode){
      case eSmithDiagramm:
      break;
      case eGainSweep:
      cbOperatedMode->setCurrentIndex (0);
      break;
      case eSWR:
      cbOperatedMode->setCurrentIndex (1);
      break;
      case eSWRfeeder:
      cbOperatedMode->setCurrentIndex (2);
      break;
      case eImpedance:
      cbOperatedMode->setCurrentIndex (3);
      break;
      case eSpectrumAnalyser:
      cbOperatedMode->setCurrentIndex (4);
      break;
      case eSpectrumDisplayShift:
      cbOperatedMode->setCurrentIndex (5);
      break;
    }
    checkbox3db->setChecked(sweepdata.bandbreite3db);
    checkbox6db->setChecked(sweepdata.bandbreite6db);
    checkboxgrafik->setChecked(sweepdata.bandbreitegrafik);
    checkboxinvers->setChecked(sweepdata.binvers);
    mledit->clear();
  }
  if (bttyOpen)
  {
    bSweepIsRun = TRUE;
    labellupe->setEnabled(FALSE);
    buttonlupeplus->setEnabled(FALSE);
    buttonlupeminus->setEnabled(FALSE);
    buttonlupemitte->setEnabled(FALSE);
    buttoneinmal->setEnabled(FALSE);
    buttonwobbeln->setEnabled(FALSE);
    buttonstop->setEnabled(TRUE);
//    groupdaempfung->setEnabled(FALSE);
//    daempfung2->setEnabled(FALSE);

    emit setmenu(emOnce, FALSE);
    emit setmenu(emSweep, FALSE);
    emit setmenu(emStop, TRUE);
    emit setmenu(emVersion, FALSE);

    gbZoom->setEnabled(FALSE);
    gbSweep->setEnabled(FALSE);
    gbOperatedStart->setEnabled(FALSE);
    if(!currentdata.fwWrong){
      //keine Zwischenzeit bei ungeeigneter FW
      boxzwischenzeit->setEnabled(FALSE);
      labelzwischenzeit->setEnabled(FALSE);
    }
    wobbelstop = FALSE;
#ifndef DEBUG_INTERFACE
    setTabEnabled(tab_wkmanager, FALSE);
    setTabEnabled(tab_nwt7vfo, FALSE);
    setTabEnabled(tab_calculation, FALSE);
    setTabEnabled(tab_nwt7measure, FALSE);
#endif
    sendSweepCmd(); //command to tty
    sweepdata.mousesperre = TRUE; //Active fields cursor lock
    fsendsweepdata(); //data to signal sendsweepdata
  }
  //Counter for termination of traffic
  demolitionlevel=0;
  bdatareturn=FALSE;
  if(currentdata.grperwobbeln){
    grafik->setFocus();
    grafik->activateWindow ();
  }
  //emit setmenu(emmainhidde, TRUE);
}

void Nwt7MainClass::clickSweepOnce(){
  SweepInit(); //Abgleich aller Eingabedaten
  if(sweepdestroy)return;

  if(!ttyOpen(1)) return;

  grafik->setWindowTitle(tr("Graphical Display",""));
  if (bcurveloaded){
    setWindowTitle(currentdata.str_tty);
    bcurveloaded = FALSE; //Modus ausschalten
    sweepdata = wobdatenmerk;//Alte einstellungen wieder zurueckladen
    checkboxk1->setChecked(sweepdata.bkanal1);
    checkboxk2->setChecked(sweepdata.bkanal2);
    sweepInvalid();
    switch(sweepdata.eoperatemode){
      case eSmithDiagramm:
      break;
      case eGainSweep:
      cbOperatedMode->setCurrentIndex (0);
      break;
      case eSWR:
      cbOperatedMode->setCurrentIndex (1);
      break;
      case eSWRfeeder:
      cbOperatedMode->setCurrentIndex (2);
      break;
      case eImpedance:
      cbOperatedMode->setCurrentIndex (3);
      break;
      case eSpectrumAnalyser:
      cbOperatedMode->setCurrentIndex (4);
      break;
      case eSpectrumDisplayShift:
      cbOperatedMode->setCurrentIndex (5);
      break;
    }
    checkbox3db->setChecked(sweepdata.bandbreite3db);
    checkbox6db->setChecked(sweepdata.bandbreite6db);
    checkboxgrafik->setChecked(sweepdata.bandbreitegrafik);
    checkboxinvers->setChecked(sweepdata.binvers);
    mledit->clear();
  }
  if (bttyOpen)
  {
    buttoneinmal->setEnabled(FALSE);
    buttonwobbeln->setEnabled(FALSE);
    buttonstop->setEnabled(FALSE);
    gbAttenuator->setEnabled(FALSE);
    cbAttenuator2->setEnabled(FALSE);

    emit setmenu(emOnce, FALSE);
    emit setmenu(emSweep, FALSE);
    emit setmenu(emStop, FALSE);
    emit setmenu(emVersion, FALSE);

    gbZoom->setEnabled(FALSE);
    gbSweep->setEnabled(FALSE);
    gbOperatedStart->setEnabled(FALSE);
    if(!currentdata.fwWrong){
      //keine Zwischenzeit bei ungeeigneter FW
      boxzwischenzeit->setEnabled(FALSE);
      labelzwischenzeit->setEnabled(FALSE);
    }
    wobbelstop = TRUE;
    setTabEnabled(tab_wkmanager, FALSE);
    setTabEnabled(tab_nwt7vfo, FALSE);
    setTabEnabled(tab_calculation, FALSE);
    setTabEnabled(tab_nwt7measure, FALSE);
    sendSweepCmd(); //command to tty
    sweepdata.mousesperre = TRUE; //Active fields cursor lock
    fsendsweepdata(); //to graphik
  }
  demolitionlevel=0;
  bdatareturn=FALSE;
  if(currentdata.grpereinmal){
    grafik->setFocus();
    grafik->activateWindow ();
  }
}

void Nwt7MainClass::clickSweepStop(){
  if (bttyOpen)
  {
    wobbelstop = TRUE;
    buttonstop->setEnabled(FALSE);
    emit setmenu(emStop, FALSE);
  }
  if(currentdata.grperstop){
    grafik->setFocus();
    grafik->activateWindow ();
  }
}

void Nwt7MainClass::sendSweepCmd(){
  QString qs;
  double dfanfang, dfschrittfrequenz;
  long fanfang;
  long fschrittfrequenz;
  double zf;
  int freq_faktor;
  
  freq_faktor = sweepdata.freq_faktor;
  //qDebug("freq_faktor %i",freq_faktor);
  if(bSweepIsRun and bSetDamping){
    if(bttyOpen){
      bSetDamping = FALSE;
      picmodem->writeChar(0x8F);
      picmodem->writeChar('r');
      picmodem->writeChar(fwportb);
      #ifdef CMDDEBUG
        qs.sprintf("r%i",fwportb);
        qDebug() << qs;
      #endif    
    }
  }
  
  wcounter = 3;
  dfanfang =  sweepdata.anfangsfrequenz;
  dfschrittfrequenz = sweepdata.schrittfrequenz;
  switch(sweepdata.eoperatemode){
    case eSmithDiagramm:
      break;
    case eGainSweep:
    case eSWR:
    case eSWRfeeder:
    case eImpedance:
      if(freq_faktor > 1){
        dfanfang = round(dfanfang / double(freq_faktor));
        dfschrittfrequenz = round(dfschrittfrequenz / double(freq_faktor));
        //qDebug("dfanfang %f",dfanfang);
      }else{
        if(checkboxzf->isChecked()){
          qs = editzf->text();
          zf = linenormalisieren(qs, false);
          // nur wenn keine Frequenzvervielfachung
          dfanfang += zf;
        }
      }
    break;
    case eSpectrumAnalyser:
      if(sabereich1->isChecked()){
        dfanfang += currentdata.frqzf1;
        sweepdata.psavabs = currentdata.psavabs1;
      }
      //Bereich 2 Pegelabweichung addieren
      if(sabereich2->isChecked()){
        dfanfang += currentdata.frqzf2;
        sweepdata.psavabs = currentdata.psavabs2;
      }
    break;
    case eSpectrumDisplayShift:
      //SA-Konverter Frequenzverschiebung addieren
      dfanfang += currentdata.frqshift;
      if(sabereich1->isChecked()){
        dfanfang += currentdata.frqzf1;
        sweepdata.psavabs = currentdata.psavabs1;
      }
      //Bereich 2 Pegelabweichung addieren
      if(sabereich2->isChecked()){
        dfanfang += currentdata.frqzf2;
        sweepdata.psavabs = currentdata.psavabs2;
      }
      //SAshift Pegelabweichung addieren
      sweepdata.psavabs = sweepdata.psavabs + currentdata.psavabs3;
    break;
  }
  
  
/*  
  wcounter = 3;
  fanfang =  wobdaten.anfangsfrequenz;
  //SAshift Frequenzverschiebung addieren
  if(wobdaten.ebetriebsart == espekdisplayshift){
    fanfang += weichen.frqshift;
  }
  //SA ZF addieren
  if((wobdaten.ebetriebsart == espektrumanalyser) or 
     (wobdaten.ebetriebsart == espekdisplayshift)){
    //Bereich 1 Pegelabweichung addieren
    if(sabereich1->isChecked()){
      fanfang += weichen.frqzf1;
      wobdaten.psavabs = weichen.psavabs1;
    }
    //Bereich 2 Pegelabweichung addieren
    if(sabereich2->isChecked()){
      fanfang += weichen.frqzf2;
      wobdaten.psavabs = weichen.psavabs2;
    }
    //SAshift Pegelabweichung addieren
    if(wobdaten.ebetriebsart == espekdisplayshift){
      wobdaten.psavabs = wobdaten.psavabs + weichen.psavabs3;
    } 
  }else{
    // alle anderen Betriebsarten
    // eventuell ZF addieren
    if(checkboxzf->isChecked()){
      qs = editzf->text();
      zf = linenormalisieren(qs);
      // nur wenn keine Frequenzvervielfachung
      if(freq_faktor == 1.0)fanfang += long(zf);
    }
  }
  fschrittfrequenz = wobdaten.schrittfrequenz;
  // bei Frequenzvervielfachung Frequenzen umrechnen
  if(freq_faktor > 1.0 and wobdaten.ebetriebsart == ewobbeln){
    fanfang = fanfang / freq_faktor;
    fschrittfrequenz = wobdaten.schrittfrequenz / freq_faktor;
  }
*/
  //Daten in LONG umrechen
  fanfang = long(round(dfanfang));
  fschrittfrequenz = long(round(dfschrittfrequenz));
  //NF Wobbeln
  if((checkboxk2->isChecked() == true) and (checkboxk1->isChecked() == false) and
     (sweepdata.maudio2 == true) and (fwversion == 120)){
    int ztime = currentdata.audioztime / sweepdata.cntpoints;
    if(ztime > 999)ztime = 999;
    //qDebug() << "wgrunddaten.audioztime :"  << wgrunddaten.audioztime;
    //qDebug() << "ztime :"  << ztime;
    qs.sprintf("%09li%08li%04i%03i%02i", fanfang, fschrittfrequenz, sweepdata.cntpoints, ztime, currentdata.audiosens);
    if(sweepdata.linear1 or sweepdata.linear2){
      qs = "d" + qs; //linear einschalten
    }else{
      qs = "c" + qs; //log einschalten
    };
  }else{
    if(boxzwischenzeit->currentIndex() != 0){
      qs.sprintf("%09li%08li%04i%03i", fanfang,
                                      fschrittfrequenz,
                                      sweepdata.cntpoints,
                                      sweepdata.ztime);
      if(sweepdata.linear1 or sweepdata.linear2){
        qs = "b" + qs; //linear einschalten
      }else{
        qs = "a" + qs; //log einschalten
      }
    }else{
      qs.sprintf("%09li%08li%04i", fanfang,
                                   fschrittfrequenz,
                                   sweepdata.cntpoints);
      if(sweepdata.linear1 or sweepdata.linear2){
        qs = "w" + qs; //linear einschalten
      }else{
        qs = "x" + qs; //log einschalten
      }
    }
  }
  //qDebug(qs);
  ttyindex = 0; //index zum lesen der Daten vom NWT auf 0

  if(!ttyOpen(1)) return;

  if (bttyOpen)
  {
    defaultClearTty(); //eventuelle Daten im UART loeschen
    //zuerst 8fh senden Vektor fuer LinuxSW im PIC
    picmodem->writeChar(143);
    picmodem->writeLine(qs);
    #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(qs);
    #endif
    wcounter = 3;
    IdleTimerStop();
    vsweeptimer->start(10);
    progressbar->setMaximum(sweepdata.cntpoints * 4);
    buttoneinmal->setEnabled(FALSE);
    buttonwobbeln->setEnabled(FALSE);
//    groupdaempfung->setEnabled(FALSE);
//    daempfung2->setEnabled(FALSE);
    emit setmenu(emOnce, FALSE);
    emit setmenu(emSweep, FALSE);
    synclevel=0;
    bSweepIsRun = TRUE;
  }
}

void Nwt7MainClass::setIdleDdsOff()
{
  int index = currentIndex(); //tab index
  bool operate = (index != tab_nwt7vfo) or (index == tab_nwt7measure);
  if (!operate) operate = (messVfoModeState == 2); //if tabs with vfo mode
  if (operate){
      if(currentdata.bset0hz){
          setDdsOff();
      }
   }
}

void Nwt7MainClass::setDdsOff()
{
    if(bttyOpen){
        QString spark = QString().sprintf("f%09.0f",0.0);
        picmodem->writeChar(143);
        picmodem->writeLine(spark);
        #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(spark);
        #endif
    }
}

//End sweep
void Nwt7MainClass::SweepTimerPerform(){
  int  i, j;
  qint64 a;
  char abuffer[(maxmesspunkte * 4)];
  double yf=0.0, ys=0.0;
  unsigned char c, c1;
  //Set buffer lengths multiplier
  int bufferfaktor = 4;
  double afrequenz;
  double findex;
  QString s;
#ifdef TIMERDEBUG
  qDebug(">>SweepTimerPerform()");
#endif
  //Read data from the API
  if (bttyOpen){
    //a = picmodem->size();
    a = picmodem->readttybuffer(abuffer, ((sweepdata.cntpoints) * bufferfaktor));
    #ifdef SWEEPDEBUG
    qDebug(">>readttymain()");
    #endif
    if(a>0){
        #ifdef SWEEPDEBUG
            qDebug() << "from tty buff=" << a;
        #endif
        #ifdef CMDDEBUG
            qDebug("sweep <--- 4byte  RS232 OK");
        #endif
      bdatareturn = TRUE;
    }
    if(a == 0){
      synclevel++; //there were no data on
    }else{
      demolitionlevel = 0;
      synclevel = 0; //it lay on data
    }
    for(i=0, j=ttyindex; i<a; i++, j++){
      ttybuffer[j] = abuffer[i]; //The read data in the correct buffer copy
    }
    ttyindex = ttyindex + a; //and the index set to the correct location
    progressbar->setValue(ttyindex);
    for(i=0, j=0; i < (ttyindex / bufferfaktor); i++, j++){
      c = ttybuffer[4 * i];
      c1 = ttybuffer[(4 * i) + 1];
      sweepdata.mess.k1[i] = c + (c1 * 256);
      c = ttybuffer[(4 * i) + 2];
      c1 = ttybuffer[(4 * i) + 3];
      sweepdata.mess.k2[i] = c + (c1 * 256);
    }
    if(bdatareturn){
      sweepdata.mess.daten_enable = TRUE;
      wcounter--;
      if(wcounter==0){
        if(currentdata.bfliessend)emit sendsweepcurve(sweepdata.mess);
        wcounter=3;
      }
    }
  }
  if(((sweepdata.cntpoints) * bufferfaktor) == ttyindex){ //all is read
    //All data are from NWT7 read and can be split channel 1 2
    #ifdef SWEEPDEBUG
    qDebug("readttymain() - all data is read and can split chanel 1 and 2");
    #endif
    for(i=0, j=0; i < (ttyindex / bufferfaktor); i++, j++){
      c = ttybuffer[4 * i];
      c1 = ttybuffer[(4 * i) + 1];
      sweepdata.mess.k1[i] = c + (c1 * 256);
      c = ttybuffer[(4 * i) + 2];
      c1 = ttybuffer[(4 * i) + 3];
      sweepdata.mess.k2[i] = c + (c1 * 256);
    }
    //Sweep data are valid
    if(bdatareturn){
      sweepdata.mess.daten_enable = TRUE;
    }else{
      sweepdata.mess.daten_enable = FALSE;
    }
    //Sweep data copy the graphics window
    fsendsweepdata();
    if(wobbelstop){
      //DDS Set output to 0Hz
      setIdleDdsOff();
      //Menu button again to activate
      sweepdata.mousesperre = FALSE; //mouse cursor free again
      fsendsweepdata();
      bSweepIsRun = FALSE;
      setCursorFrequency(KursorFrequenz);
      buttoneinmal->setEnabled(TRUE);
      buttonwobbeln->setEnabled(TRUE);
      buttonstop->setEnabled(FALSE);
      vsweeptimer->stop();//Sobol A.E.
      idletimer->start(currentdata.idletime);
      if((sweepdata.eoperatemode == eSWR) or (sweepdata.eoperatemode == eSWRfeeder)){
        gbAttenuator->setEnabled(FALSE);
        cbAttenuator2->setEnabled(FALSE);
      }
      if((sweepdata.eoperatemode == eGainSweep) or
         (sweepdata.eoperatemode == eSpectrumAnalyser) or
         (sweepdata.eoperatemode == eSpectrumDisplayShift)){
        gbAttenuator->setEnabled(TRUE);
        cbAttenuator2->setEnabled(TRUE);
      }
      emit setmenu(emOnce, TRUE);
      emit setmenu(emSweep, TRUE);
      emit setmenu(emStop, FALSE);
      emit setmenu(emVersion, TRUE);

      gbZoom->setEnabled(TRUE);
      gbSweep->setEnabled(TRUE);
      gbOperatedStart->setEnabled(TRUE);
      if(!currentdata.fwWrong){
        //Mean while, with a suitable FW
        boxzwischenzeit->setEnabled(TRUE);
        labelzwischenzeit->setEnabled(TRUE);
      }
      setTabEnabled(tab_wkmanager, TRUE);
      setTabEnabled(tab_nwt7vfo, TRUE);
      setTabEnabled(tab_calculation, TRUE);
      // K1 or K2 = linear measurement window disable
      if(sweepdata.linear2 || sweepdata.linear1){
        setTabEnabled(tab_nwt7measure, FALSE);
      }else{
        setTabEnabled(tab_nwt7measure, TRUE);
      }
      if(bkalibrieren0){
        ym_0db = 0.0;
        //Kanal 1
        if(checkboxk1->isChecked()){
          //  Mittelwert bilden
          for(i=0; i<sweepdata.cntpoints; i++){
            ym_0db = ym_0db + double(sweepdata.mess.k1[i]);
          }
          ym_0db = ym_0db / double(sweepdata.cntpoints);
          //qDebug("ym_0db = %f",ym_0db); 
          //SWR eichen
          if(bkalibrierenswr){
            if(sweepdata.eoperatemode == eSWR){
              afrequenz = sweepdata.anfangsfrequenz;
              for(i=0; i<sweepdata.cntpoints; i++){
                yf = double(sweepdata.mess.k1[i]) * sweepdata.faktor1 + sweepdata.shift1;
                findex = (afrequenz + (sweepdata.schrittfrequenz * i)) / sweepdata.freq_faktor;
                j = int(findex / eteiler);
                sweepdata.swrkalibrierarray.arrayk1[j] = yf;
              }
              emit sendsweepdata(sweepdata);
              bkalibrierenswr = FALSE;
              //qDebug("faktor1 %f shift1 %f", wobdaten.faktor1, wobdaten.shift1);	      
            }
          //Wobbeln kalibrieren
          }else{
            // Lineare Sonde kalibrieren
            if(sweepdata.linear1){
              yf = 50.0 / (ym_0db - ym_40db);
              ys = ((ym_0db * yf) - maxkalibrier) * -1.0;
              //yf = (yaufloesung * 5.0)/(11.0 * (ym_0db - ym_40db));
              //ys = (yaufloesung * 5.0)/11.0 - yf * ym_40db;
              //logaritmische Sonde Eichen
            }else{
              //qDebug("ym_0dB = %f; ym_40dB = %f", ym_0db, ym_40db);
              //Neue Berechnung ab V3.05
              yf = (40.0 + maxkalibrier) / (ym_0db - ym_40db);
              //yf = (yaufloesung * (4.0 - maxkalibrier))/(ydisplayteilung_log * (ym_0db - ym_40db));
              //Neue Berechnung ab V3.05
              ys = ((ym_0db * yf) - maxkalibrier) * -1.0;
              //ys = (yaufloesung * 5.0)/ydisplayteilung_log - yf * ym_40db;
            }
            //qDebug("faktor = %f; shift = %f", yf, ys);
            sweepdata.faktor1 = yf;
            sweepdata.shift1 = ys;
            // Eichkorretur nur bei log. Sonde
            if(!sweepdata.linear1){
              afrequenz = sweepdata.anfangsfrequenz;
              for(i=0; i<sweepdata.cntpoints; i++){
                //aus der Frequenz den Index j errechnen
                //Neue Berechnung ab V3.05
                findex = (afrequenz + (sweepdata.schrittfrequenz * i)) / sweepdata.freq_faktor;
                j = int(findex / eteiler);
                sweepdata.kalibrierarray.arrayk1[j] = (double(sweepdata.mess.k1[i]) * sweepdata.faktor1 + sweepdata.shift1) - maxkalibrier;
                //qDebug(" nkalibrierarray.arrayk1[j] %f", nkalibrierarray.arrayk1[j]);
                //yf = double(wobdaten.mess.k1[i]) * wobdaten.faktor1 + wobdaten.shift1;
                //nkalibrierarray.arrayk1[j] = (yaufloesung * (0.9 - maxkalibrier/10.0)) - yf;
              }
              emit sendsweepdata(sweepdata);
            }
          }
        }
        if(checkboxk2->isChecked()){
          //average these
          for(i=0; i<sweepdata.cntpoints; i++){
            ym_0db = ym_0db + double(sweepdata.mess.k2[i]);
          }
          ym_0db = ym_0db / double(sweepdata.cntpoints);
          if(sweepdata.maudio2)ym_0db = ym_0db / 32.0;
          //qDebug("ym_0db = %f",ym_0db); 
          //SWR calibrate
          if(bkalibrierenswr){
            if(sweepdata.eoperatemode == eSWR){
              afrequenz = sweepdata.anfangsfrequenz;
              for(i=0; i<sweepdata.cntpoints; i++){
                yf = double(sweepdata.mess.k2[i]) * sweepdata.faktor2 + sweepdata.shift2;
                findex = (afrequenz + (sweepdata.schrittfrequenz * i)) / sweepdata.freq_faktor;
                j = int(findex / eteiler);
                sweepdata.swrkalibrierarray.arrayk2[j] = yf;
              }
              emit sendsweepdata(sweepdata);
              bkalibrierenswr = FALSE;
              //qDebug("faktor1 %f shift1 %f", wobdaten.faktor1, wobdaten.shift1);	      
            }
          //sweep calibrate
          }else{
            //Linear probe calibration
            if(sweepdata.linear2){
              yf = 50.0 / (ym_0db - ym_40db);
              ys = ((ym_0db * yf) - maxkalibrier) * -1.0;
              //yf = (yaufloesung * 5.0)/(11.0 * (ym_0db - ym_40db));
              //ys = (yaufloesung * 5.0)/11.0 - yf * ym_40db;
              //logaritmische Sonde Eichen
            }else{
              //qDebug("ym_0dB = %f; ym_40dB = %f", ym_0db, ym_40db);
              //Neue Berechnung ab V3.05
              yf = (40.0 + maxkalibrier) / (ym_0db - ym_40db);
              //yf = (yaufloesung * (4.0 - maxkalibrier))/(ydisplayteilung_log * (ym_0db - ym_40db));
              //Neue Berechnung ab V3.05
              ys = ((ym_0db * yf) - maxkalibrier) * -1.0;
              //ys = (yaufloesung * 5.0)/ydisplayteilung_log - yf * ym_40db;
            }
            //qDebug("faktor = %f; shift = %f", yf, ys);
            sweepdata.faktor2 = yf;
            sweepdata.shift2 = ys;
            // Eichkorretur nur bei log. Sonde
            if(!sweepdata.linear2){
              afrequenz = sweepdata.anfangsfrequenz;
              for(i=0; i<sweepdata.cntpoints; i++){
                //aus der Frequenz den Index j errechnen
                //Neue Berechnung ab V3.05
                findex = (afrequenz + (sweepdata.schrittfrequenz * i)) / sweepdata.freq_faktor;
                j = int(findex / eteiler);
                sweepdata.kalibrierarray.arrayk2[j] = (double(sweepdata.mess.k2[i]) * sweepdata.faktor2 + sweepdata.shift2) - maxkalibrier;
                //qDebug(" nkalibrierarray.arrayk1[j] %f", nkalibrierarray.arrayk1[j]);
                //yf = double(wobdaten.mess.k1[i]) * wobdaten.faktor1 + wobdaten.shift1;
                //nkalibrierarray.arrayk1[j] = (yaufloesung * (0.9 - maxkalibrier/10.0)) - yf;
              }
              emit sendsweepdata(sweepdata);
            }
          }
        }
        //Wobble data are valid
        if(bdatareturn){
          sweepdata.mess.daten_enable = TRUE;
        }else{
          sweepdata.mess.daten_enable = FALSE;
        }
        //Wobbeldaten ins Grafikfenster kopieren
        fsendsweepdata();
        //qDebug(wobdaten.namesonde1);
        if(QMessageBox::question(this, tr("Save Calibration ? -- NWT",""), 
                                       tr("<B> storing the values in the probe file </B> <BR> Save the values obtained immediately?",""),
                                       tr("&Yes",""), tr("&No",""),
                                 QString::null, 0, 1) == 0){
          if(sweepdata.eoperatemode == eGainSweep){
            if(checkboxk1->isChecked()){
              if(sweepdata.linear1){
                messsondespeichern1(currentdata.strsonde1lin);
              }else{
                messsondespeichern1(sweepdata.namesonde1);
              }
            }
            if(checkboxk2->isChecked()){
              messsondespeichern2(sweepdata.namesonde2);
	    }
          }
          if(sweepdata.eoperatemode == eSWR){
            if(checkboxk1->isChecked()){
              messsondespeichern1(filenamesonde1);
            }
            if(checkboxk2->isChecked()){
              messsondespeichern2(filenamesonde2);
	    }
          }
        }
        bkalibrieren0 = FALSE;
        bkalibrierenswr = FALSE;
      }
      if(bkalibrieren40){
        ym_40db = 0.0;
        if(checkboxk1->isChecked()){
          for(i=0; i<sweepdata.cntpoints; i++){
            ym_40db = ym_40db + double(sweepdata.mess.k1[i]);
          }
          ym_40db = ym_40db / double(sweepdata.cntpoints);
          setAttenuationControls_dB(0); // 0 dB
          nwtDelay(currentdata.attenuatorDelay_ms);
          maxkalibrier = 0.0;
          if(!sweepdata.linear1){
            QString beschr(tr("<br><b> Setting`second dB line Channel 1 </b> <br><br> Connect Output of the NWT to the  Input, or Insert a small <br> attenuaton. <br><br> Enter Attenuation, or set to  Default :",""));
            bool ok;
            double l = QInputDialog::getDouble(this, tr("Channel 1 Calibration","Menu"),
                                                    beschr, 0.0, -20.0, 0.0, 0, &ok); 
            if(ok){
              maxkalibrier = l;
              //maxkalibrier = l / -10.0;
            }
          }else{
          QMessageBox::warning( this, tr("Channel 1 Calibration","Menu"),
                                      tr("<br><b>Setting  0 dB line  :</b> <br><br> Connect Output of the NWT to the Input!",""));
          }
        }
        if(checkboxk2->isChecked()){
          for(i=0; i<sweepdata.cntpoints; i++){
            ym_40db = ym_40db + double(sweepdata.mess.k2[i]);
          }
          ym_40db = ym_40db / double(sweepdata.cntpoints);
          if(sweepdata.maudio2)ym_40db = ym_40db / 32.0;
          setAttenuationControls_dB(0); // 0 dB
          maxkalibrier = 0.0;
          if(!sweepdata.linear2){
            QString beschr(tr("<br><b>Setting second dB line Channel 2 : </b> <br> <br>Connect Output of the NWT to the Input, or Insert a small<br>  attenuaton. <br><br> Enter attenuation , or set to  Default :",""));
            bool ok;
            double l = QInputDialog::getDouble(this, tr("Channel 2 Calibration"),
                                                    beschr, 0.0, -20.0, 0.0, 0, &ok); 
            if(ok){
              maxkalibrier = l;
              //maxkalibrier = l / -10.0;
            }
          }else{
          QMessageBox::warning( this, tr("Channel 2 Calibration","Menu"), 
                                      tr("<br><b>Setting  0 dB line  :</b> <br><br> Connect Output of the NWT to the Input!",""));
          }
        }
        bkalibrieren40 = FALSE;
        bkalibrieren0 = TRUE;
        //again sweep
        sendSweepCmd();
        //Start timer for reading the data NWT7
        IdleTimerStop();
        vsweeptimer->start(10);
      }
    }else{
      //again sweep
      sendSweepCmd();
      //Start timer for reading the data NWT7
      IdleTimerStop();
      vsweeptimer->start(10);
    }  
  }else{
    //All data are not read start timer again
    if(synclevel >=20){
      demolitionlevel++;
      if(demolitionlevel > 5){
        //Termination of functions sweep
        QMessageBox::warning( this, tr("Serial interface","InformationsBox"), 
                                    tr("Can't Receive data from NWT !",""));
        #ifdef SWEEPDEBUG
            qDebug("Nwt7linux::readtty() - data are not read start timer again");
        #endif
        vsweeptimer->stop();//Sobol A.E.
        idletimer->start(currentdata.idletime);
        wobbelstop = TRUE;
        setInitInterface(bttyOpen && fwversion);
        return;
      }else{
        synclevel = 0;
        sendSweepCmd();
        idletimer->stop();
        stimer->stop();
        vtimer->stop();
        vsweeptimer->start(10);
        return;
      }
    }
    IdleTimerStop();
    vsweeptimer->start(10);
  }
#ifdef TIMERDEBUG
  qDebug("<<SweepTimerPerform()");
#endif
}

void Nwt7MainClass::checkboxtime_checked(int b){
  if(b == Qt::Unchecked){
//    editscantime->setText("0");
//    wobdaten.ztime = 0;
  }else{
//    editscantime->setText("100.0");
  }
  SweepInit();
}

void Nwt7MainClass::checkboxinvers_checked(bool b){
  //geaenderter Darstellung
  sweepdata.binvers = b;
  //Daten ins Grafikfenster uebermitteln und neu darstellen
  fsendsweepdata();
}

void Nwt7MainClass::checkboxgrafik_checked(bool b){
  //geaenderter Darstellung
  sweepdata.bandbreitegrafik = b;
  //Daten ins Grafikfenster uebermitteln und neu darstellen
  fsendsweepdata();
}

void Nwt7MainClass::checkbox6db_checked(bool b){
  //geaenderter Darstellung
  sweepdata.bandbreite6db = b;
  if(b)sweepdata.bandbreitegrafik = b;
  if(b)checkboxgrafik->setChecked(b);
  //Daten ins Grafikfenster uebermitteln und neu darstellen
  fsendsweepdata();
}

void Nwt7MainClass::checkbox3db_checked(bool b){
  //geaenderter Darstellung
  sweepdata.bandbreite3db = b;
  if(b)sweepdata.bandbreitegrafik = b;
  if(b)checkboxgrafik->setChecked(b);
  //Daten ins Grafikfenster uebermitteln und neu darstellen
  fsendsweepdata();
}

void Nwt7MainClass::checkboxk1_checked(bool b){
  //geaenderter Darstellung Kanal 1
  sweepdata.bkanal1 = b;
  switch(sweepdata.eoperatemode){
    case eSmithDiagramm:
      break;
    case eGainSweep:
    break;
    case eSWR:
    case eSWRfeeder:
    case eImpedance:
      if(b){
        sweepdata.bkanal2 = FALSE;
        checkboxk2->setChecked(FALSE);
        sweepdata.bswrkanal2 = FALSE;
      }else{
        sweepdata.bkanal2 = TRUE;
        checkboxk2->setChecked(TRUE);
        sweepdata.bswrkanal2 = TRUE;
      }
    break;
    case eSpectrumAnalyser:
    case eSpectrumDisplayShift:
    break;
  }
  //Daten ins Grafikfenster uebermitteln und neu darstellen
  fsendsweepdata();
}

void Nwt7MainClass::checkboxk2_checked(bool b){
  //geaenderter Darstellung Kanal 2
  sweepdata.bkanal2 = b;
  switch(sweepdata.eoperatemode){
    case eSmithDiagramm:
    break;
    case eGainSweep:
    break;
    case eSWR:
    case eSWRfeeder:
    case eImpedance:
      if(b){
        sweepdata.bkanal1 = FALSE;
        checkboxk1->setChecked(FALSE);
        sweepdata.bswrkanal2 = TRUE;
      }else{
        sweepdata.bkanal1 = TRUE;
        checkboxk1->setChecked(TRUE);
        sweepdata.bswrkanal2 = FALSE;
      }
    break;
    case eSpectrumAnalyser:
    case eSpectrumDisplayShift:
    break;
  }
  
  //Daten ins Grafikfenster uebermitteln und neu darstellen
  fsendsweepdata();
}

void Nwt7MainClass::setSaBereich1(){
  sabereich = bsa1;
  setFrqBereich();
}

void Nwt7MainClass::setSaBereich2(){
  sabereich = bsa2;
  setFrqBereich();
}

void Nwt7MainClass::setFrqBereich(){
  QString qs;
  
  //nur wenn sich Beriebsart oder SA-Bereich aendert
  //nicht beim Laden der Kurvendatei
  if(!bcurveloaded){
    if(sabereich != sabereichalt){
      if(sweepdata.eoperatemode == eSpectrumAnalyser){
        if(sabereich1->isChecked()){
          setFqStart(double(currentdata.sa1anfang));
          setFqEnd(double(currentdata.sa1ende));
          setFqStep(double(currentdata.sa1schritte));
          sabereich = bsa1;
        }
        if(sabereich2->isChecked()){
          setFqStart(double(currentdata.sa2anfang));
          setFqEnd(double(currentdata.sa2ende));
          setFqStep(double(currentdata.sa2schritte));
          sabereich = bsa2;
        }
      }else{
        if(sweepdata.eoperatemode == eSpectrumDisplayShift){
          if(sabereich1->isChecked()){
            setFqStart(double(currentdata.sa3anfang));
            setFqEnd(double(currentdata.sa3ende));
            setFqStep(double(currentdata.sa3schritte));
            sabereich = bsa3;
          }
          if(sabereich2->isChecked()){
            /*setanfang(double(wgrunddaten.sa2anfang- wgrunddaten.frqshift));
            setende(double(wgrunddaten.sa2ende- wgrunddaten.frqshift));
            setschritte(double(wgrunddaten.sa2schritte));
            sabereich = bsa2;*/
          }
        }else{
          setFqStart(double(currentdata.wanfang));
          setFqEnd(double(currentdata.wende));
          setFqStep(double(currentdata.wschritte));
          sabereich = bwobbeln;
        }
      }
      SweepInit();
      sabereichalt = sabereich;
    }
  }
}


void Nwt7MainClass::setOperateMode(int i){
  QString qs;
  if (!fwversion) return;
  switch(i){
    case 0: //Sweep
      qs = "o0"; //SWR relay off
      sweepdata.eoperatemode = eGainSweep;
      sabereich = bwobbeln;
      emit setmenu(emCalibrateK1, TRUE);
      if(!sweepdata.numberchanel){
        emit setmenu(emCalibrateK2, TRUE);
        checkboxk2->setEnabled(TRUE);
        emit setmenu(emSaveCallibrateK2, TRUE);
        emit setmenu(emLoadCallibrateK2, TRUE);
        kalibrier2->show();
      }	
      kalibrier1->show();
      checkboxk1->setEnabled(TRUE);
      checkboxgrafik->setEnabled(TRUE);
      checkbox3db->setEnabled(TRUE);
      checkbox6db->setEnabled(TRUE);
      checkboxinvers->setEnabled(TRUE);
      gbAttenuator->setEnabled(TRUE);
      cbAttenuator2->setEnabled(TRUE);
      cbResolutionSWR->hide();
      labelaufloesung->hide();
      //SA
      groupsa->hide();
      labelsafrqbereich->hide();
      labelsabandbreite->hide();
      sabereich1->hide();
      sabereich2->hide();
      //setTabEnabled(tab_nwt7messen, TRUE);
      labela_100->hide();
      labelkabellaenge->hide();
      edita_100->hide();
      editkabellaenge->hide();
      boxdbshift1->setEnabled(TRUE);
      boxdbshift2->setEnabled(TRUE);
      labelboxdbshift1->setEnabled(TRUE);
      labelboxdbshift2->setEnabled(TRUE);
      break;
    case 1:  //SWR
      qs = "o1"; //SWR Relais ein
      sweepdata.eoperatemode = eSWR;
      sabereich = bwobbeln;
      emit setmenu(emCalibrateK1, TRUE);
      emit setmenu(emCalibrateK2, TRUE);
      emit setmenu(emSaveCallibrateK2, TRUE);
      emit setmenu(emLoadCallibrateK2, TRUE);
      if(checkboxk1->isChecked() and checkboxk2->isChecked()){
        checkboxk1->setChecked(TRUE);
        checkboxk2->setChecked(FALSE);
        sweepdata.bswrkanal2 = FALSE;
      }
      if(!checkboxk1->isChecked() and !checkboxk2->isChecked()){
        checkboxk1->setChecked(TRUE);
        checkboxk2->setChecked(FALSE);
        sweepdata.bswrkanal2 = FALSE;
      }
//      checkboxk1->setChecked(TRUE);
//      checkboxk2->setChecked(FALSE);
//      checkboxk1->setEnabled(FALSE);
      if(!sweepdata.numberchanel){
        checkboxk2->setEnabled(TRUE);
      }
      kalibrier1->hide();
      kalibrier2->hide();
      checkboxgrafik->setEnabled(FALSE);
      checkbox3db->setEnabled(FALSE);
      checkbox6db->setEnabled(FALSE);
      checkboxinvers->setEnabled(FALSE);
      //Relais auf 0dB schalten
      //setAttenuationControls(0);
      gbAttenuator->setEnabled(FALSE);
      cbAttenuator2->setEnabled(FALSE);
      cbResolutionSWR->show();
      labelaufloesung->show();
      //SA
      groupsa->hide();
      labelsafrqbereich->hide();
      labelsabandbreite->hide();
      sabereich1->hide();
      sabereich2->hide();
      //setTabEnabled(tab_nwt7messen, FALSE);
      labela_100->hide();
      labelkabellaenge->hide();
      edita_100->hide();
      editkabellaenge->hide();
      boxdbshift1->setCurrentIndex(20);
      boxdbshift2->setCurrentIndex(20);
      boxdbshift1->setEnabled(FALSE);
      boxdbshift2->setEnabled(FALSE);
      labelboxdbshift1->setEnabled(FALSE);
      labelboxdbshift2->setEnabled(FALSE);
      break;
    case 2:  //SWRANT
      qs = "o1"; //SWR Relais ein
      sweepdata.eoperatemode = eSWRfeeder;
      sabereich = bwobbeln;
      emit setmenu(emCalibrateK1, FALSE);
      emit setmenu(emCalibrateK2, FALSE);
      emit setmenu(emSaveCallibrateK2, FALSE);
      emit setmenu(emLoadCallibrateK2, FALSE);
      if(checkboxk1->isChecked() and checkboxk2->isChecked()){
        checkboxk1->setChecked(TRUE);
        checkboxk2->setChecked(FALSE);
        sweepdata.bswrkanal2 = FALSE;
      }
      if(!checkboxk1->isChecked() and !checkboxk2->isChecked()){
        checkboxk1->setChecked(TRUE);
        checkboxk2->setChecked(FALSE);
        sweepdata.bswrkanal2 = FALSE;
      }
      kalibrier1->hide();
      kalibrier2->hide();
//     checkboxk1->setChecked(TRUE);
//     checkboxk2->setChecked(FALSE);
//     checkboxk1->setEnabled(FALSE);
      if(!sweepdata.numberchanel)checkboxk2->setEnabled(TRUE);
      checkboxgrafik->setEnabled(FALSE);
      checkbox3db->setEnabled(FALSE);
      checkbox6db->setEnabled(FALSE);
      checkboxinvers->setEnabled(FALSE);
      //Relais auf 0dB schalten
      setAttenuationControls_dB(0);
      gbAttenuator->setEnabled(FALSE);
      cbAttenuator2->setEnabled(FALSE);
      cbResolutionSWR->show();
      labelaufloesung->show();
      labela_100->show();
      labelkabellaenge->show();
      edita_100->show();
      editkabellaenge->show();
      boxdbshift1->setCurrentIndex(20);
      boxdbshift2->setCurrentIndex(20);
      boxdbshift1->setEnabled(FALSE);
      boxdbshift2->setEnabled(FALSE);
      labelboxdbshift1->setEnabled(FALSE);
      labelboxdbshift2->setEnabled(FALSE);
      //SA
      groupsa->hide();
      labelsafrqbereich->hide();
      labelsabandbreite->hide();
      sabereich1->hide();
      sabereich2->hide();
      //setTabEnabled(tab_nwt7messen, FALSE);
      break;
    case 3:  //Impedanz
      qs = "o1"; //SWR Relais ein
      sweepdata.eoperatemode = eImpedance;
      sabereich = bwobbeln;
      emit setmenu(emCalibrateK1, FALSE);
      emit setmenu(emCalibrateK2, FALSE);
      emit setmenu(emSaveCallibrateK2, FALSE);
      emit setmenu(emLoadCallibrateK2, FALSE);
      if(checkboxk1->isChecked() and checkboxk2->isChecked()){
        checkboxk1->setChecked(TRUE);
        checkboxk2->setChecked(FALSE);
        sweepdata.bswrkanal2 = FALSE;
      }
      if(!checkboxk1->isChecked() and !checkboxk2->isChecked()){
        checkboxk1->setChecked(TRUE);
        checkboxk2->setChecked(FALSE);
        sweepdata.bswrkanal2 = FALSE;
      }
      kalibrier1->hide();
      kalibrier2->hide();
//      checkboxk1->setChecked(TRUE);
//      checkboxk2->setChecked(FALSE);
//      checkboxk1->setEnabled(FALSE);
      if(!sweepdata.numberchanel)checkboxk2->setEnabled(TRUE);
      checkboxgrafik->setEnabled(FALSE);
      checkbox3db->setEnabled(FALSE);
      checkbox6db->setEnabled(FALSE);
      checkboxinvers->setEnabled(FALSE);
      //Relais auf 0dB schalten
      //setAttenuationControls(0);
      gbAttenuator->setEnabled(FALSE);
      cbAttenuator2->setEnabled(FALSE);
      cbResolutionSWR->hide();
      labelaufloesung->hide();
      labela_100->hide();
      labelkabellaenge->hide();
      edita_100->hide();
      editkabellaenge->hide();
      //SA
      groupsa->hide();
      labelsafrqbereich->hide();
      labelsabandbreite->hide();
      sabereich1->hide();
      sabereich2->hide();
      //setTabEnabled(tab_nwt7messen, FALSE);
      boxdbshift1->setCurrentIndex(20);
      boxdbshift2->setCurrentIndex(20);
      boxdbshift1->setEnabled(FALSE);
      boxdbshift2->setEnabled(FALSE);
      labelboxdbshift1->setEnabled(FALSE);
      labelboxdbshift2->setEnabled(FALSE);
      if(!bcurveloaded){
        QMessageBox::warning( this, tr("Impedance measurement with Series-resistor 50 Ohm",""), 
                                    tr("<br><b> Impedance measurement |Z|</b> <br><br>To measure the Impedance, a Series-resistor of 50 Ohms shall be connected to the Impedance to be measured.",""));
      }
      break;
    case 4: //spektrumanalyser
      qs = "o0"; //SWR Relais aus
      sweepdata.eoperatemode = eSpectrumAnalyser;
      emit setmenu(emCalibrateK1, FALSE);
      if(!sweepdata.numberchanel){
        emit setmenu(emCalibrateK2, FALSE);
        checkboxk2->setEnabled(TRUE);
        emit setmenu(emSaveCallibrateK2, TRUE);
        emit setmenu(emLoadCallibrateK2, TRUE);
      }	
      kalibrier1->hide();
      kalibrier2->hide();
      checkboxk1->setEnabled(TRUE);
      checkboxgrafik->setEnabled(TRUE);
      checkbox3db->setEnabled(TRUE);
      checkbox6db->setEnabled(TRUE);
      checkboxinvers->setEnabled(TRUE);
      gbAttenuator->setEnabled(TRUE);
      cbAttenuator2->setEnabled(TRUE);
      cbResolutionSWR->hide();
      labelaufloesung->hide();
      //SA
      if(currentdata.sastatus){
        groupsa->setTitle(tr("Status",""));
        groupsa->show();
        labelsafrqbereich->show();
        labelsabandbreite->show();
        sabereich1->hide();
        sabereich2->hide();
      }else{
        groupsa->setTitle(tr("Range",""));
        groupsa->show();
        labelsafrqbereich->hide();
        labelsabandbreite->hide();
        sabereich1->show();
        sabereich2->show();
        if(sabereich1->isChecked())sabereich = bsa1;
        if(sabereich2->isChecked())sabereich = bsa2;
      }
      labela_100->hide();
      labelkabellaenge->hide();
      edita_100->hide();
      editkabellaenge->hide();
      boxdbshift1->setEnabled(TRUE);
      boxdbshift2->setEnabled(TRUE);
      labelboxdbshift1->setEnabled(TRUE);
      labelboxdbshift2->setEnabled(TRUE);
      break;
    case 5: //spekdisplayshift
      qs = "o0"; //SWR Relais aus
      sweepdata.eoperatemode = eSpectrumDisplayShift;
      emit setmenu(emCalibrateK1, FALSE);
      if(!sweepdata.numberchanel){
        emit setmenu(emCalibrateK2, FALSE);
        checkboxk2->setEnabled(TRUE);
        emit setmenu(emSaveCallibrateK2, TRUE);
        emit setmenu(emLoadCallibrateK2, TRUE);
      }	
      kalibrier1->hide();
      kalibrier2->hide();
      checkboxk1->setEnabled(TRUE);
      checkboxgrafik->setEnabled(FALSE);
      checkbox3db->setEnabled(FALSE);
      checkbox6db->setEnabled(FALSE);
      checkboxinvers->setEnabled(FALSE);
      gbAttenuator->setEnabled(TRUE);
      cbAttenuator2->setEnabled(TRUE);
      cbResolutionSWR->hide();
      labelaufloesung->hide();
      //SA
      if(currentdata.sastatus){
        groupsa->setTitle(tr("Status+Freq.shift",""));
        groupsa->show();
        labelsafrqbereich->show();
        labelsabandbreite->show();
        sabereich1->hide();
        sabereich2->hide();
      }else{
        groupsa->setTitle(tr("Range+Freq.Shift",""));
        groupsa->show();
        labelsafrqbereich->hide();
        labelsabandbreite->hide();
        sabereich1->show();
        sabereich2->show();
      }
      labela_100->hide();
      labelkabellaenge->hide();
      edita_100->hide();
      editkabellaenge->hide();
      boxdbshift1->setEnabled(TRUE);
      boxdbshift2->setEnabled(TRUE);
      labelboxdbshift1->setEnabled(TRUE);
      labelboxdbshift2->setEnabled(TRUE);
      sabereich = bsa3;
      break;
  }
  setFrqBereich();
  if(!bcurveloaded)SweepInit();
  //only send a command to the PIC if command implemented in PIC
  if(sweepdata.bswrrelais){
    if(!ttyOpen(1)) return;
    if(bttyOpen)
    {
      picmodem->writeChar(143);
      picmodem->writeLine(qs);
    #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(qs);
    #endif
    }
  }
  //Data transmitted over the graphics window and restate
  fsendsweepdata();
}

void Nwt7MainClass::setFqStart(double a)
{
  QString s;
  s.sprintf("%1.0f",a);
  //qDebug(s);
  editanfang->setText(s);
}

void Nwt7MainClass::setFqEnd(double a)
{
  QString s;
  s.sprintf("%1.0f",a);
  editende->setText(s);
}

void Nwt7MainClass::setFqStep(double a)
{
  QString s;
  s.sprintf("%1.0f",a);
  editschritte->setText(s);
}

void Nwt7MainClass::readberechnung(double a)
{
  QString s;
  s.sprintf("%f",a/1000000.0);
//  qDebug(s);
  editf1->setText(s);
  editf2->setText(s);
}

void Nwt7MainClass::schwingkreisfc()
{
  QString sf;
  QString sl;
  QString sc;
  double f;
  double l;
  double c;
  double xc;
  bool ok;
  
  //qDebug("schwingkreisfc()");
  sf = editf2->text();
  //qDebug(sf);
  f = sf.toDouble(&ok);
  if(!ok)return;
  sl = editl2->text();
  //qDebug(sl);
  l = sl.toDouble(&ok);
  if(!ok)return;
  f = f * 1000000.0;
  //qDebug("f=%g",f);
  l = l / 1000000.0;
  //qDebug("c=%g",l);
  c = 1.0/(4.0*3.1415*3.1415*l*f*f);
  xc = 1.0/(2.0*3.1415*f*c);
  //qDebug("c=%g",c);
  //sl.sprintf("%03.3f",l);
  sc = QString("%1").arg(c*1000000000000.0);
  //qDebug(sc);
  ergebnisc->setText(sc);
  sc = QString("%1").arg(xc);
  //qDebug(sc);
  ergebnisxc->setText(sc);
}

void Nwt7MainClass::alwindungen()
{
  QString sal;
  QString sl;
  QString sw;
  double al;
  double l;
  double w;
  bool ok;
  
  sal = edital3->text();
  al = sal.toDouble(&ok);
  if(!ok)return;
  sl = editl3->text();
  l = sl.toDouble(&ok);
  if(!ok)return;
  l = l * 1000.0;
  //  qDebug("l=%g",l);
  //  qDebug("al=%g",al);
  w = sqrt(l/al);
  //  qDebug("al * l=%g",al * l);
  //  qDebug("w=%g",w);
  sw = QString("%1").arg(w);
  if((sal.length() > 0) and (sl.length() > 0))ergebnisw->setText(sw);
}

void Nwt7MainClass::schwingkreisfl()
{
  QString sf;
  QString sl;
  QString sc;
  QString sn;
  double f;
  double l;
  double c;
  double n;
  double al;
  double xl;
  bool ok;
  
  sf = editf1->text();
  f = sf.toDouble(&ok);
  if(!ok)return;
  sc = editc1->text();
  c = sc.toDouble(&ok);
  if(!ok)return;
  f = f * 1000000.0;
  c = c / 1000000000000.0;
  l = 1.0/(4.0*3.1415*3.1415*c*f*f);
  xl = 2.0*3.1415*f*l;
  sl = QString("%1").arg(l*1000000.0);
  ergebnisl->setText(sl);
  if(sl.length() > 0){
    sn = editn->text();
    n = sn.toDouble(&ok);
    sl = QString("%1").arg(xl);
    ergebnisxl->setText(sl);
    if(!ok)return;
    al = l / (n * n);
    sl = QString("%1").arg(al*1000000000.0);
    ergebnisal->setText(sl);
    //AL-value with copy into the next calculation window
    edital3->setText(sl);
  }
}

double Nwt7MainClass::linenormalisieren(const QString &line, bool positive_only)
{
  bool ok;
  double faktor=1.0;
  double ergebnis = 0.0;
  bool gefunden = FALSE;
  
  QString aline(line);
  
  aline.toLower();	//all lower case
  aline.remove(QChar(' ')); // Remove spaces
  if(aline.contains('g')){
    if(aline.contains(',') or aline.contains('.')){
      aline.replace('g',' ');
    }else{
      aline.replace('g','.');
    }
    faktor = 1000000000.0;
    gefunden = TRUE;
  }
  if(aline.contains('m')){
    if(aline.contains(',') or aline.contains('.')){
      aline.replace('m',' ');
    }else{
      aline.replace('m','.');
    }
    faktor = 1000000.0;
    gefunden = TRUE;
  }
  if(aline.contains('k')){
    if(aline.contains(',') or aline.contains('.')){
      aline.replace('k',' ');
    }else{
      aline.replace('k','.');
    }
    faktor = 1000.0;
    gefunden = TRUE;
  }
  ergebnis = aline.toDouble(&ok);
  if(!ok)ergebnis = 0.0;
  if(gefunden){
    ergebnis *= faktor;
  }
  if((ergebnis < 0.0) && positive_only) ergebnis = 0.0; // prevent negative number
  if(ergebnis >= 9999999999.0)ergebnis = 9999999999.0;  // greater than 9.999 .. GHz prevent
  return ergebnis;
}

void Nwt7MainClass::swrantaendern(){
  QString qs;
  double a_100 = 0.0;
  double laenge = 0.0;
  bool ok;

  qs = edita_100->text();
  a_100 = qs.toDouble(&ok);
  if(ok){
    sweepdata.a_100 = a_100;
  }else{
    sweepdata.a_100 = 0.0;
  }
  qs = editkabellaenge->text();
  laenge = qs.toDouble(&ok);
  if(ok){
    sweepdata.kabellaenge = laenge;
  }else{
    sweepdata.kabellaenge = 0.0;
  }
  fsendsweepdata();
}

void Nwt7MainClass::SweepInit(){
  double scantime,  //Zwischenzeit der Wobbelpunkte
         freq_start,  //Anfangsfrequenz
         freq_end,  //Endfrequenz
         freq_step, //Anzahl der Schritte
         freq_step_new, //Anzahl der Schritte neu
         freq_step_width, //Schrittweite
         display_shift, //dB Verschiebnung im Display
         bw=0.0,  //Spektrumanalyser Bandbreitenkontrolle
         freq_faktor, //Faktor fuer Frequenzvervielfachung
         new_step_width,
         new_step,
         step_max = double(maxmesspunkte);

  QString sscantime,
          sfreq_start,
          sfreq_end,
          sschritte,
          sschrittweite,
          sdbshift1,
          sdbshift2,
          sdisplayshift,
          sbw;

  bool ok,
       saschrittkorr,
       sakorrnachoben;

  int button;

  sweepdestroy = FALSE;
  //qDebug("wobnormalisieren()");
  /////////////////////////////////////////////////////////////////////////////  
  //Werte aus den Editfenster lesen
  //Zwischenzeit in uSek zwischen den Messungen
  /////////////////////////////////////////////////////////////////////////////  
  sscantime = boxzwischenzeit->currentText();
  scantime = sscantime.toDouble(&ok);
  if(ok){scantime = scantime / 10.0;}else{scantime=0.0;}
  sfreq_start = editanfang->text();
  sfreq_end = editende->text();
  freq_start = linenormalisieren(sfreq_start, true);
  freq_end = linenormalisieren(sfreq_end, true);
  freq_faktor = double(sweepdata.freq_faktor);
  //qDebug("freq_faktor: %f",freq_faktor);
  //qDebug("Ende  : %f",freq_end);
  sschritte = editschritte->text();
  sschritte.remove(QChar(' '));
  freq_step = sschritte.toDouble(&ok);
  if(!ok)freq_step = 1000.0;
  //freq_step_width = editschrittweite->text();
  //freq_step_width = sschrittweite.toDouble(&ok);
  freq_step_width = qRound((freq_end - freq_start)/(freq_step-1));
  if(sweepdata.maudio2){
      new_step = qRound(freq_end - freq_start + 1.0);
      /*
      if(new_step > wgrunddaten.audioschrittkorrgrenze){
        new_step = wgrunddaten.audioschrittkorrgrenze;
      }
      */
      new_step_width = qRound((freq_end - freq_start)/(new_step-1));
      if(new_step_width < 1.0)new_step_width = 1.0;
      if(new_step_width > 1.0){
        freq_step_width = qRound((freq_end - freq_start)/(new_step-1));
      }else{
        freq_step_width = 1.0;
      }
      freq_step = new_step;
  }
  if(freq_step_width == 0.0)freq_step_width=1.0;
  //qDebug() << "Schrittweite2:" << freq_step_width;
  //qDebug() << "Schritte2:" << freq_step;
  switch(sweepdata.eoperatemode){
    case eGainSweep:
    case eSWR:
    case eSWRfeeder:
    case eImpedance:
      if(freq_faktor > 1.0){
        //ganzzahliger Wert wird gebraucht
        freq_start = round(freq_start / freq_faktor) * freq_faktor;
        freq_step_width = qRound(freq_step_width / freq_faktor) * freq_faktor;
      }
    break;
    case eSmithDiagramm:
    case eSpectrumAnalyser:
    case eSpectrumDisplayShift:
    break;
  }
  //Anzahl der Schritte aendern wenn Frequenzabstand zu klein
  if(freq_step_width == 1){
    if(freq_end - freq_start < freq_step) freq_step = freq_end - freq_start + 1.0;
  }
  //freq_end noch einmal neu berechnen
  freq_end = freq_start + (freq_step - 1.0) * freq_step_width;
  //qDebug("freq_start %f",freq_start);
  //qDebug("freq_end %f",freq_end);
  //qDebug("freq_step %f",freq_step);
  //qDebug("freq_step_width %f",freq_step_width);
  // ============ Displayshift behandlung
  sdisplayshift = editdisplay->text();
  display_shift = sdisplayshift.toDouble(&ok);;
  if(!ok)display_shift = 0.0;
  display_shift = qRound(display_shift/10)*10;
  sdisplayshift.sprintf("%1.0f",display_shift);
  editdisplay->setText(sdisplayshift);
  sweepdata.display_shift = int(display_shift);
  // ============ Displayshift behandlung
  sdbshift1 = boxdbshift1->currentText();
  sdbshift2 = boxdbshift2->currentText();
  /////////////////////////////////////////////////////////////////////////////  
  //Alle wichtigen werte sind gefuellt ?
  /////////////////////////////////////////////////////////////////////////////  
  if((sfreq_start.length() > 0) & (sfreq_end.length() > 0) & (sschritte.length() > 0)){
    //qDebug("wobnormalisieren() 1");
    ///////////////////////////////////////////////////////////////////////////  
    //Verschiebung auf der Y-Achse einstellen
    ///////////////////////////////////////////////////////////////////////////  
    sweepdata.dbshift1 = sdbshift1.toDouble(&ok);
    if(!ok)sweepdata.dbshift1 = 0.0;
    sweepdata.dbshift2 = sdbshift2.toDouble(&ok);
    if(!ok)sweepdata.dbshift2 = 0.0;
    ///////////////////////////////////////////////////////////////////////////  
    //Korrektur der Schritte
    ///////////////////////////////////////////////////////////////////////////  
    if(freq_step < 2.0){
      freq_step = 2.0;
      sschritte.sprintf("%1.0f",freq_step);
      editschritte->setText(sschritte);
    }
    if(freq_step > step_max){
      freq_step = step_max;
      sschritte.sprintf("%1.0f",freq_step);
      editschritte->setText(sschritte);
    }
    ///////////////////////////////////////////////////////////////////////////  
    //Korrektur der Frequenzdaten Wobbeln, SA-Bereich 1 und SA-Bereich 2 wenn Ueberschreitung
    ///////////////////////////////////////////////////////////////////////////  
    switch(sweepdata.eoperatemode){
      case eSmithDiagramm:
        break;
      case eGainSweep:
      case eSWR:
      case eSWRfeeder:
      case eImpedance:
        if(freq_end > (currentdata.max_sweep_freq )){
          freq_end = currentdata.max_sweep_freq;
          if(freq_start > (currentdata.max_sweep_freq)){
            freq_start = freq_end - ((freq_step - 1.0) * freq_step_width);
          }
        }
        break;
      case eSpectrumAnalyser:
        if(sabereich1->isChecked()){
          sweepdata.psavabs = currentdata.psavabs1;
          if(freq_end > currentdata.frqb1){
            freq_end = currentdata.frqb1;
            if(freq_start >= freq_end)freq_start = currentdata.frqa1;
          }
        }
        if(sabereich2->isChecked()){
          sweepdata.psavabs = currentdata.psavabs2;
          if(freq_start < currentdata.frqa2){
            freq_start = currentdata.frqa2;
          }
          if(freq_end > currentdata.frqb2){
            freq_end = currentdata.frqb2;
            if(freq_start >= freq_end)freq_start = currentdata.frqa2;
          }
        }
        break;
      case eSpectrumDisplayShift:
        if(sabereich1->isChecked()){
          sweepdata.psavabs = currentdata.psavabs1 + currentdata.psavabs3;
          //qDebug("freq_start %f",freq_start);
          //qDebug("freq_end %f",freq_end);
          if((freq_start + currentdata.frqshift) < currentdata.frqa1){
            freq_start = currentdata.frqa1 - currentdata.frqshift;
            //qDebug("anfang1 %f",freq_start);
          }
          if((freq_end + currentdata.frqshift) >= currentdata.frqb1){
            freq_end = currentdata.frqb1 - currentdata.frqshift;
            //qDebug("ende1 %f",freq_end);
            if((freq_start + currentdata.frqshift) >= freq_end)
              freq_start = currentdata.frqa1 - currentdata.frqshift;
              //qDebug("anfang2 %f",freq_start);
          }
        }
        if(sabereich2->isChecked()){
          sweepdata.psavabs = currentdata.psavabs2  + currentdata.psavabs3;
          if((freq_start + currentdata.frqshift) < currentdata.frqa2){
            freq_start = currentdata.frqa2 - currentdata.frqshift;
          }
          if((freq_end + currentdata.frqshift) > currentdata.frqb2){
            freq_end = currentdata.frqb2 - currentdata.frqshift;
            if((freq_start + currentdata.frqshift) >= freq_end)
              freq_start = currentdata.frqa2 - currentdata.frqshift;
          }
        }
        break;
    }
    ///////////////////////////////////////////////////////////////////////////  
    //Korrektur von Anfang, Ende bei allen Betriebsarten
    ///////////////////////////////////////////////////////////////////////////  
    if(freq_start >= freq_end){
      freq_end = freq_start + 1.0; //min um 1  erhoehen
    }
    ///////////////////////////////////////////////////////////////////////////  
    //Ueberpruefung und Korrektur der Bandbreite und Schrittweite bei SA 
    //Nur wenn keine Kurvendatei geladen
    ///////////////////////////////////////////////////////////////////////////  
    if(sabw != bwkein and ((sweepdata.eoperatemode == eSpectrumAnalyser) or (sweepdata.eoperatemode == eSpectrumDisplayShift))){
      saschrittkorr = FALSE; //Schrittanzahl fuer den SA nicht OK
      sakorrnachoben = FALSE; //keine Schrittweite nach oben korrigieren
      // Test ob Schrittweite erhoeht werden muss
      if(sabw == bw30kHz and freq_step_width > currentdata.bw30kHz_max){
        saschrittkorr = TRUE; bw = currentdata.bw30kHz_max;
      }
      //if(saschrittkorr){qDebug("saschrittkorr TRUE");}else{qDebug("saschrittkorr FALSE");}
      if(sabw == bw7kHz and freq_step_width > currentdata.bw7kHz_max){
        saschrittkorr = TRUE; bw = currentdata.bw7kHz_max;
      }
      //if(saschrittkorr){qDebug("saschrittkorr TRUE");}else{qDebug("saschrittkorr FALSE");}
      if(sabw == bw300Hz and freq_step_width > currentdata.bw300_max){
        saschrittkorr = TRUE; bw = currentdata.bw300_max;
      }
      //if(saschrittkorr){qDebug("saschrittkorr TRUE");}else{qDebug("saschrittkorr FALSE");}
      if(!saschrittkorr){
        //Test ob Schrittanzahl verringert werden kann, BW festlegen
        //keine Korr nach oben
        if(sabw == bw30kHz and freq_step_width < currentdata.bw30kHz_min){
          saschrittkorr = TRUE; bw = currentdata.bw30kHz_min; sakorrnachoben = TRUE;
        }
        if(sabw == bw7kHz and freq_step_width < currentdata.bw7kHz_min){
          saschrittkorr = TRUE; bw = currentdata.bw7kHz_min; sakorrnachoben = TRUE;
        }
        if(sabw == bw300Hz and freq_step_width < currentdata.bw300_min){
          saschrittkorr = TRUE; bw = currentdata.bw300_min; sakorrnachoben = TRUE;
        }
      }
      //if(sakorrnachoben){qDebug("sakorrnachoben TRUE");}else{qDebug("sakorrnachoben FALSE");}
      //qDebug("bw %f",bw);
      if(saschrittkorr){
        if(bw < 1000.0){
          sbw.sprintf(" %1.0fHz !!!",bw);
        }else{
          sbw.sprintf(" %1.2fkHz !!!",bw/1000.0);
        }
        //Schrittanzahl mit neuer BW korrigieren
        freq_step_new = (freq_end - freq_start) / bw;
        int j = int(freq_step_new / 100.0) + 1;
        freq_step_new = 100.0 * j + 1.0;
        if(freq_step_new > step_max)freq_step_new = step_max;
        if((freq_step_new < currentdata.saminschritte) and currentdata.bschrittkorr)freq_step_new = currentdata.saminschritte;
        //Die neuen Werte schon mal anzeigen
        freq_step_width = (freq_end - freq_start) / (freq_step_new - 1.0);
        //sind die Schritte kleiner als 1.0
        if(freq_step_width < 1.0){
          freq_step_width = 1.0;
        }
        freq_end = freq_start + (freq_step_new - 1) * freq_step_width;
        setFqStart(freq_start);
        setFqEnd(freq_end);
        setFqStep(freq_step_new);
        sschrittweite.sprintf("%1.0f",freq_step_width);
        editschrittweite->setText(sschrittweite);
        //Warnung nur bei Obergrenze und Schrittanzahlerhoehung und Warnung ist erlaubt
        if((freq_step_new == step_max) and currentdata.bwarnsavbwmax and !sakorrnachoben){
          button = QMessageBox::Abort;
          button = QMessageBox::warning(this, tr("SA-Bandwidth Check",""), 
                                              tr("The stepsize is more than","")
                                                 + sbw + "<br><br> " +
                                              tr("The maximum sample number is reached end set<br><b>Reduce the stepsize or increase the Bandwith please !</b>",""),
                                                 QMessageBox::Ok, QMessageBox::Abort);
          if(button == QMessageBox::Abort)sweepdestroy = TRUE;
          freq_step = freq_step_new;
        }else{
          //Schrittmaximum noch nicht erreicht
          button = QMessageBox::No;
          if(currentdata.bschrittkorr){
            //automatische Schrittkorrektur
            freq_step = freq_step_new;
          }else{
            //keine automatische Schrittkorrektur
            if(currentdata.bwarnsavbw and !sakorrnachoben){
              //Warnung erlaubt und Schrittweite verringern und korr Schrittanzahl nach oben
              button = QMessageBox::question(this, tr("SA-Bandwidth Check",""), 
                                                   tr("The stepsize is more than","")
                                                   + sbw + "<br><br> " +
                                                   tr("The new values are set, do you want them used ?",""),
                                                   QMessageBox::Yes, QMessageBox::No);
            }
            //qDebug("button %x",button);
            if(button == QMessageBox::Yes){
              //Die Werte von der Vorberechnung nehmen und Schritte korrigieren
              freq_step = freq_step_new;
              //qDebug("button %x",button);
            }
          }
        }
      }
    }
    ///////////////////////////////////////////////////////////////////////////  
    //Correction of the increment in all modes
    ///////////////////////////////////////////////////////////////////////////  
    switch(sweepdata.eoperatemode){
      case eSmithDiagramm:
        break;
      case eGainSweep:
        if(freq_faktor > 1.0){
          freq_start = qRound(freq_start / freq_faktor) * freq_faktor;
        }
        freq_step_width = (freq_end - freq_start)/(freq_step - 1.0);
        if(freq_faktor > 1.0){
          freq_step_width = qRound(freq_step_width / freq_faktor) * freq_faktor;
        }
        break;
      case eSWR:
      case eSWRfeeder:
      case eImpedance:
      case eSpectrumAnalyser:
      case eSpectrumDisplayShift:
        freq_step_width = qRound((freq_end - freq_start) / (freq_step - 1.0));
        break;
    }
    //sind die Schritte kleiner als 1.0
    if(freq_step_width < 1.0){
      if(freq_faktor > 1){
        freq_step_width = double(freq_faktor);
      }else{
        freq_step_width = 1.0;
      }
    }
    ///////////////////////////////////////////////////////////////////////////  
    //Calculating the mean of each step was set when SCANTIME
    ///////////////////////////////////////////////////////////////////////////  
    sweepdata.ztime = int(scantime);
    ///////////////////////////////////////////////////////////////////////////  
    //Calculating the scan time for display without meantime
    ///////////////////////////////////////////////////////////////////////////  
    ///////////////////////////////////////////////////////////////////////////  
    //Set the end frequency for all calculations
    ///////////////////////////////////////////////////////////////////////////  
    freq_end = freq_start + ((freq_step-1.0) * freq_step_width);
    //Write back again all values
    setFqStart(freq_start);
    setFqEnd(freq_end);
    setFqStep(freq_step);
    sschrittweite.sprintf("%1.0f",freq_step_width);
    editschrittweite->setText(sschrittweite);
    //Data transmitted over the graphics window
    ok = TRUE;
    
    if(sweepdata.anfangsfrequenz != freq_start)ok = FALSE;
    sweepdata.anfangsfrequenz = freq_start;
    if(sweepdata.cntpoints != int(freq_step))ok = FALSE;
    sweepdata.cntpoints = int(freq_step);
    if(sweepdata.schrittfrequenz != freq_step_width)ok = FALSE;
    sweepdata.schrittfrequenz = freq_step_width;
    if(!ok)sweepInvalid();
    //Remember, depending on the mode frequency vertices
    switch(sweepdata.eoperatemode){
      case eSmithDiagramm:
        break;
      case eSpectrumAnalyser:
        if(sabereich1->isChecked()){
          currentdata.sa1anfang = freq_start;
          currentdata.sa1ende = freq_end;
          currentdata.sa1schritte = int(freq_step);
        }
        if(sabereich2->isChecked()){
          currentdata.sa2anfang = freq_start;
          currentdata.sa2ende = freq_end;
          currentdata.sa2schritte = int(freq_step);
        }
        break;
      case eSpectrumDisplayShift:
        if(sabereich1->isChecked()){
          currentdata.sa3anfang = freq_start;
          currentdata.sa3ende = freq_end;
          currentdata.sa3schritte = int(freq_step);
        }
        if(sabereich2->isChecked()){
          /*wgrunddaten.sa2anfang = freq_start;
          wgrunddaten.sa2ende = freq_end;
          wgrunddaten.sa2schritte = int(freq_step);*/
        }
        break;
      case eGainSweep:
      case eSWR:
      case eSWRfeeder:
      case eImpedance:
        currentdata.wanfang = freq_start;
        currentdata.wende = freq_end;
        currentdata.wschritte = int(freq_step);
        break;
    }
    fsendsweepdata(); //fraphik refresh
  }
}

void Nwt7MainClass::mleditloeschen(){
  QString qs;
  
  QStringList sl = mledit->toPlainText().split("\n", QString::KeepEmptyParts);
  mleditlist.clear();
  infoueber.clear();
  
  for(int i=0; i<sl.count(); i++){
    //qDebug(sl.at(i));
    //if((sl.at(i).indexOf("Kanal") == 0) or (sl.at(i).indexOf("Kursor") == 0) or (sl.at(i).indexOf(" ") == 0))break;
    if(sl.at(i).indexOf(";") == 0)infoueber.append(sl.at(i));
  }
  mleditlist.append(" ");
  if(infoueber.count() == 0){
    infoueber.append(";no_label");
  }
  mledit->clear();
}

void Nwt7MainClass::mleditinsert(const QString &s){
  int i;
  QString qs;
  
  mleditlist.append(s);
  mledit->clear();
  for(i=0;i < infoueber.count();i++){
    mledit->append(infoueber.at(i));
  }
  for(i=0;i < mleditlist.count();i++){
    mledit->append(mleditlist.at(i));
  }
}

///////////////////////////////////////////////////////////////////////////////
// Funktionen im Messfenster

void Nwt7MainClass::saveTableToFile(){
  QFile f;
  QString s = QFileDialog::getSaveFileName(
              this,
              tr("Saving NWT Measuringdata",""),
              kurvendir.filePath("messtab.txt"),
              "Textdatei (*.txt)", 0,0);
  if (!s.isNull())
  {
    //Datei ueberpruefen ob Sufix vorhanden
    if((s.indexOf(".")== -1)) s += ".txt";
    f.setFileName(s);
    if(f.open( QIODevice::WriteOnly )){
      QTextStream ts(&f);
      ts << messedit->toPlainText();
      //messedit->setModified( FALSE );
      f.close();
    }
  }
}

void Nwt7MainClass::writeMessureToTable(){
  btabelleschreiben = TRUE;
}

void Nwt7MainClass::messsetfont(){
  bool ok;
  
  messfont = QFontDialog::getFont( &ok, messfont, this);
  if(ok){
    messlabel1->setFont(messfont);
    messlabel2->setFont(messfont);
    messlabel3->setFont(messfont);
    messlabel4->setFont(messfont);
    messlabel5->setFont(messfont);
    messlabel6->setFont(messfont);
    lmhz->setFont(messfont);
    lkhz->setFont(messfont);
    lhz->setFont(messfont);
  }
}

//Command function for measured value output to the module send

void Nwt7MainClass::startMessure(){
  QString qs;

  if(bttyOpen)defaultClearTty();
  if(fwversion == 120){
    messtime = messtimeneu; //ab FW 1.20 die Zwischenzeit fuer
                            //das Wattmetermessen anders setzen
    qs="n";
  }else{
    qs= "m";
  }
  if(!ttyOpen(1)){
      vmesstimer->stop();
      idletimer->start(currentdata.idletime);
      return;
  }
  if (bttyOpen)
  {
    if(fwversion == 120){
      progressbaPowerMeter1->setMaximum(maxmessstep*32);
      progressbaPowerMeter2->setMaximum(maxmessstep*32);
    }else{
      progressbaPowerMeter1->setMaximum(maxmessstep);
      progressbaPowerMeter2->setMaximum(maxmessstep);
    }
    picmodem->writeChar(143); //send command

    picmodem->writeLine(qs);
    #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(qs);
    #endif
    vmesstimer->start(messtime); //Timer starten
  }  
}

//Get on timer expiration data

void Nwt7MainClass::vMessTimerPerform(){
  char abuffer[10];
  unsigned char c1=0, c2=0;
  int i1, i2, a;
  QString qs, qsline;
  double w, wzu;
  double uv;
  double p;
  bool ok;
  int maxcount; // count messuring on one value
  bool displayCalcValue;
#ifdef TIMERDEBUG
    qDebug(">>MessureTimerPerform()");
#endif
  vmesstimer->stop();
  //fwversion = 120;
  if(fwversion == 120){
    maxcount = 1;
  }else{
    maxcount = 100 / messtime;
  }
   //maxcount = 1;
  if(maxcount<1) maxcount=1;
  QWidget *wg = this->currentWidget();
  if(messsync > 100 || !bttyOpen){
    #ifdef TIMERDEBUG
        qDebug("TTY CLOSED <> MessureTimerPerform();");
    #endif
    idletimer->start(currentdata.idletime);
    return;
  }
  if(wg == nwt7measure){
    a = picmodem->readttybuffer(abuffer, 10);    
    //qDebug("a = %i",a);
    if(a < 4){
      messsync++;
        #ifdef TIMERDEBUG
            qDebug("Nwt7linux::MessureTimerPerform() ERROR readttybuffer < 4");
        #endif
      startMessure(); //and start the measurement
    }else{
    #ifdef CMDDEBUG
        qDebug("m,n <--- R232 OK");
    #endif
      messsync=0;
      c1 = abuffer[0];
      c2 = abuffer[1];
      i1 = int(c1 + c2 * 256);
      c1 = abuffer[2];
      c2 = abuffer[3];
      i2 = int(c1 + c2 * 256);
      measureCounter++;
      if(fwversion == 120){
        if(i1 < 32*1024)measureValCh1 += double(i1);
        if(i2 < 32*1024)measureValCh2 += double(i2);
      }else{
        if(i1 < 1024*16)measureValCh1 += double(i1);
        if(i2 < 1024*16)measureValCh2 += double(i2);
      }
      displayCalcValue = FALSE;
      //qDebug()<<messcounter;
      //qDebug()<<maxcount;
      //qDebug()<<messtime;
      if(measureCounter == (maxcount)){
        measureCounter=0;
        if(!checkboxmesshang1->isChecked()){
          displayCalcValue = TRUE;
        }
        if((measureValCh1/maxcount) > measureCh1Peak){
          displayCalcValue = TRUE;
          measureCh1Peak = measureValCh1/maxcount;
          counterPeak1 = 0;
        }else{
          counterPeak1 ++;
          if(counterPeak1 > 8){
            displayCalcValue = TRUE;
            counterPeak1 = 0;
            measureCh1Peak = 0.0;
          }
        }
        if(displayCalcValue){
        //Kanal 1
          qs.sprintf("%i",int(measureValCh1/maxcount));
          mlabelk1->setText(qs);
          if(fwversion == 120){
            if((measureValCh1/maxcount)>maxmessstep*32){
              progressbaPowerMeter1->setValue(maxmessstep*32);
            }else{
              progressbaPowerMeter1->setValue(int(measureValCh1/maxcount));
            }
          }else{
            if((measureValCh1/maxcount)>maxmessstep){
              progressbaPowerMeter1->setValue(maxmessstep);
            }else{
              progressbaPowerMeter1->setValue(int(measureValCh1/maxcount));
            }
          }
        }
        // from the calibration factors to calculate the correct value
        w = sweepdata.mfaktor1 * (measureValCh1/maxcount) + sweepdata.mshift1;
        qs = ldampingk1->currentText();
        wzu = qs.toDouble(&ok);
        if(ok)w += wzu;
        //offset ermitten 21.07.12
        setwattoffset1(boxwattoffset1->currentIndex());
        w = w + woffset1;
        qs.sprintf("%2.1f dBm", w);
        if(currentdata.wmprezision == 2)qs.sprintf("%2.2f dBm", w);
        qsline = qs;
        if(displayCalcValue)messlabel1->setText(qs);
        //aus dBm Volt errechnen
        uv = sqrt((pow(10.0 , w/10.0) * 0.001) * 50.0);
        p = uv * uv / 50.0;
        //Messbereche automatisch umschalten
        if (uv > 999){
          qs.sprintf("%3.3f kV", uv/1000);
        }
        if (uv < 1000){
          qs.sprintf("%3.1f V", uv);
        }
        if (uv < 1.0){
          uv = uv * 1000.0;
          qs.sprintf("%3.1f mV", uv);
        }
        if (uv < 1.0){
          uv = uv * 1000.0;
          qs.sprintf("%3.1f uV", uv);
        }
        if(displayCalcValue)messlabel3->setText(qs);
        qsline = qsline + "; " + qs;
        if (p > 999){
          qs.sprintf("%3.3f kW", p/1000);
        }
        if (p < 1000){
          qs.sprintf("%3.1f W", p);
        }
        if (p < 1.0){
          p = p * 1000.0;
          qs.sprintf("%3.1f mW", p);
        }
        if (p < 1.0){
          p = p * 1000.0;
          qs.sprintf("%3.1f uW", p);
        }
        if (p < 1.0){
          p = p * 1000.0;
          qs.sprintf("%3.1f nW", p);
        }
        if (p < 1.0){
          p = p * 1000.0;
          qs.sprintf("%3.1f pW", p);
        }
        if(displayCalcValue)messlabel5->setText(qs);
        qsline = qsline + "; " + qs;
        //Kanal 2 
        if(!sweepdata.numberchanel){
          if(!checkboxmesshang2->isChecked()){
            displayCalcValue = TRUE;
          }else{
            displayCalcValue = FALSE;
          }
          if((measureValCh2/maxcount) > measureCh2Peak){
            displayCalcValue = TRUE;
            measureCh2Peak = measureValCh2/maxcount;
            counterPeak2 = 0;
          }else{
            counterPeak2 ++;
            if(counterPeak2 > 8){
              displayCalcValue = TRUE;
              counterPeak2 = 0;
              measureCh2Peak = 0.0;
            }  
          }
          if(displayCalcValue){
            qs.sprintf("%i",int(measureValCh2/maxcount));
            mlabelk2->setText(qs);
            if(fwversion == 120){
              if((measureValCh2/maxcount)>maxmessstep*32){
                progressbaPowerMeter2->setValue(maxmessstep*32);
              }else{
                progressbaPowerMeter2->setValue(int(measureValCh2/maxcount));
              }
            }else{
              if((measureValCh2/maxcount)>maxmessstep){
                progressbaPowerMeter2->setValue(maxmessstep);
              }else{
                progressbaPowerMeter2->setValue(int(measureValCh2/maxcount));
              }
            }
          }
          //calculated from the calibration factors the right value
          w = sweepdata.mfaktor2 * (measureValCh2/maxcount) + sweepdata.mshift2;
          qs = ldaempfungk2->currentText();
          wzu = qs.toDouble(&ok);
          if(ok)w += wzu;
          //offset ermitten 21.07.12
          setwattoffset2(boxwattoffset2->currentIndex());
          w = w + woffset2;
          if(sweepdata.maudio2){
            qs.sprintf("%2.2f dBV", w);
          }else{
            qs.sprintf("%2.1f dBm", w);
            if(currentdata.wmprezision == 2)qs.sprintf("%2.2f dBm", w);
          }
          if(displayCalcValue)messlabel2->setText(qs);
          qsline = qsline + "\t" + qs;
          //aus dBm Volt errechnen
          if(sweepdata.maudio2){
            uv = pow(10.0, w/20);
          }else{
            uv = sqrt((pow(10.0 , w/10.0) * 0.001) * 50.0);
            p = uv * uv / 50.0;
          }
          //Messbereche automatisch umschalten
          if (uv > 999){
            qs.sprintf("%3.3f kV", uv/1000);
          }
          if (uv < 1000){
            qs.sprintf("%3.1f V", uv);
          }
          if (uv < 1.0){
            uv = uv * 1000.0;
            qs.sprintf("%3.2f mV", uv);
          }
          if (uv < 1.0){
            uv = uv * 1000.0;
            qs.sprintf("%3.2f uV", uv);
          }
          if(displayCalcValue)messlabel4->setText(qs);
          qsline = qsline + "; " + qs;
          if(sweepdata.maudio2){
            qs= "        ";
          }else{
            if (p > 999){
              qs.sprintf("%3.3f kW", p/1000);
            }
            if (p < 1000){
              qs.sprintf("%3.1f W", p);
            }
            if (p < 1.0){
              p = p * 1000.0;
              qs.sprintf("%3.1f mW", p);
            }
            if (p < 1.0){
              p = p * 1000.0;
              qs.sprintf("%3.1f uW", p);
            }
            if (p < 1.0){
              p = p * 1000.0;
              qs.sprintf("%3.1f nW", p);
            }
            if (p < 1.0){
              p = p * 1000.0;
              qs.sprintf("%3.1f pW", p);
            }
          }
          if(displayCalcValue)messlabel6->setText(qs);
          qsline = qsline + "; " + qs;
        }
        if (bmkalibrierenk1_20db && displayCalcValue){
          bmkalibrierenk1_20db = FALSE;
          ym_40db = measureValCh1/maxcount;

          //qDebug("ym_0db:%f",ym_0db);
          //qDebug("ym_40db:%f",ym_40db);
          //qDebug("ym_daempf:%f",ym_daempf);
          //qDebug("ym_gen:%f",ym_gen);
          //jetzt wobdaten.mfaktor1 und wobdaten.mshift1 ausrechnen
          sweepdata.mfaktor1 = ym_daempf/(ym_0db - ym_40db);
          sweepdata.mshift1 = ((ym_0db * sweepdata.mfaktor1) * -1.0) + ym_gen;
          //qDebug("wobdaten.mfaktor1:%f",wobdaten.mfaktor1);
          //qDebug("wobdaten.mshift1:%f",wobdaten.mshift1);
          //qDebug("Kanal 1: faktor=%f shift=%f", wobdaten.mfaktor1, wobdaten.mshift1);
          if(QMessageBox::question(this, tr("Save Calibration ? -- NWT",""), 
                                          tr("<B> storing the values in the probe file </B> <BR> Save the values obtained immediately?",""),
                                          tr("&Yes",""), tr("&No",""),
                                          QString::null, 0, 1) == 0){
              messsondespeichern1(filenamesonde1);
          }else{
            QMessageBox::warning( this, tr("Data-memory",""),
                                        tr("<br><b> Saving the data in the Calibrationfile :</b> <br><br> The data can be saved by clicking<br> \"Save Channel 1  calibration\" in the menu \"Sweep\"!","" ));
          }
          setAttenuationControls_dB(0); // 0 dB
        }
        if (bmkalibrierenk1_0db && displayCalcValue){
          bmkalibrierenk1_0db = FALSE;
          ym_0db = measureValCh1/maxcount;
          QString beschr(tr("<b>Calibration of channel 1  :</b> <br><br>Decrease level with an attenuator,<br> the attenuaton must be 20dB  min. <br><br>Attennuation (dB):",""));
          ym_daempf = QInputDialog::getDouble(this, tr("Channel 1 Calibration","Menue"),
                                                    beschr, 20.0, 20, 90, 2, &ok);
          //qDebug("ym_daempfung:%f",ym_daempf);
          if(ok){
            bmkalibrierenk1_20db = TRUE;

            // TODO: why do we ask the user to "decrease level with an attenuator", but then select 20dB attenuator automatically?
            if (currentdata.attenuator.CanAttenuateBy(20)) {
              setAttenuationControls_dB(20);
            } else {
              // Internal attenuator does not support -20dB, so an external attenuator should be connected
              setAttenuationControls_dB(0);
            }
            nwtDelay(currentdata.attenuatorDelay_ms);
          }
        }
        if (bmkalibrierenk2_20db && displayCalcValue){
          bmkalibrierenk2_20db = FALSE;
          ym_40db = measureValCh2/maxcount;
          //jetzt wobdaten.mfaktor2 und wobdaten.mshift2 ausrechnen
          sweepdata.mfaktor2 = ym_daempf/(ym_0db - ym_40db);
          sweepdata.mshift2 = ((ym_0db * sweepdata.mfaktor2) * -1.0) + ym_gen;
          //          qDebug("Kanal 2: faktor=%f shift=%f", wobdaten.mfaktor2, wobdaten.mshift2);
          if(QMessageBox::question(this, tr("Save Calibration ? -- NWT",""), 
                                          tr("<B> storing the values in the probe file </B> <BR> Save the values obtained immediately?",""),
                                    tr("&Yes",""), tr("&No",""),
                                    QString::null, 0, 1) == 0){
              messsondespeichern2(filenamesonde2);
          }else{
            QMessageBox::warning( this, tr("Data-memory",""),
                                        tr("<br><b> Saving the data in the Calibrationfile :</b> <br><br> The data can be saved by clicking<br>\"Save Channel 2  calibration\"  in the menu \"Sweep\"!","" ));
          }
        }
        if (bmkalibrierenk2_0db && displayCalcValue){
          bmkalibrierenk2_0db = FALSE;
          ym_0db = measureValCh2/maxcount;
          QString beschr(tr("<br><b> Calibration of channel 2  :</b> <br><br>Decrease level with an attenuator,<br>the attenuaton must be 20dB  min.<br><br>Attennuation (dB):",""));
          ym_daempf = QInputDialog::getDouble(this, tr("Channel 2 Calibration","InputBox"),
                                                    beschr, 20.0, 20, 90, 2, &ok);
          if(ok) {
              bmkalibrierenk2_20db = TRUE;
              nwtDelay(300);
          }
        }
        if(btabelleschreiben){
          btabelleschreiben = FALSE;
          messedit->insertHtml(qsline+"<br>");
        }
        measureValCh1 = 0.0;
        measureValCh2 = 0.0;
      }
      if(restartMessure){
        startMessure(); //und Messung neu starten
      } else{
        IdleTimerStart();
      }
    }
  }
#ifdef TIMERDEBUG
    qDebug("<<MessureTimerPerform()");
#endif
}

void Nwt7MainClass::mkalibrierenk1(){
  bool ok;
  ym_gen = 4.25;
  QString beschr(tr("<br><b>Calibration of channel 1 :</b> <br><br> 1) Connect a well-known RF-level to the Input of the NWT.<br> ...This can be the RF-Output of the NWT. <br>... When Using  NWT01, the output level at 1MHz is +4.25 dBm.<br>... This value is displayed as Default below. <br> <br> 2) In the course of the calibration  a well-known attenuation <br>... is inserted. This attenuation must be 20dB min.<br>... From these two measurements the  Function-constants<br>... are calculated and must be saved in the Calibrationfile.<br> <br> <br>Level of the RF-Generator (dBm) :",""));
  ym_gen = QInputDialog::getDouble(this, tr("Channel 1 Calibration","Menue"),
                                           beschr, ym_gen, -20, 13, 2, &ok); //max +13 dBm
  if(ok)bmkalibrierenk1_0db = TRUE;
  ym_0db = 0.0;
  ym_40db = 0.0;
  setAttenuationControls_dB(0); // 0 dB
  nwtDelay(currentdata.attenuatorDelay_ms);
}

void Nwt7MainClass::mkalibrierenk2(){
  bool ok;
  ym_gen = 4.25;
  QString beschr(tr("<br><b>Calibration of channel 2 :</b> <br><br> 1) Connect a well-known RF-level to the Input of the NWT.<br> ...This can be the RF-Output of the NWT. <br>... When Using  NWT01, the output level at 1MHz is +4.25 dBm.<br>... This value is displayed as Default below. <br> <br> 2) In the course of the calibration  a well-known attenuation <br>... is inserted. This attenuation must be 20dB min.<br>... From these two measurements the  Function-constants<br>... are calculated and must be saved in the Calibrationfile.<br> <br> <br>Level of the RF-Generator (dBm) :",""));
  ym_gen = QInputDialog::getDouble(this, tr("Channel 2 Calibration","InputBox"),
                                           beschr, ym_gen, -20, 13, 2, &ok); //max +13 dBm
  if(ok)bmkalibrierenk2_0db = TRUE;
  ym_0db = 0.0;
  ym_40db = 0.0;
  nwtDelay(300);
}


///////////////////////////////////////////////////////////////////////////////
// Functions in the main window

int Nwt7MainClass::defaultClearTty(){
  char abuffer[(maxmesspunkte * 4)];
  int a = 0;
  if (bttyOpen){
    a = picmodem->readttybuffer(abuffer, (maxmesspunkte * 4));
  }
    #ifdef TTYDEBUG
        qDebug() << ">>defaultcleartty() clear:" << a;
    #endif
  return a;
}

/*
void Nwt7linux::resizeEvent( QResizeEvent * ){
  //nach Groessenaenderung grafische Elemente anpassen
  qDebug("Nwt7linux::resizeEvent");
  qDebug("breite %i hoehe %i", width(), height());
  nwt7wobbeln->resize(width(), height());
  grafiksetzen();
}
*/

void Nwt7MainClass::tabAfterSwitch(int index ){
  if(index != this->indexOf(wdiagramm)){
    setOperateMode(cbOperatedMode->currentIndex());
  }
  if(index == tab_nwt7sweep){
    setTabEnabled(tab_nwt7sweep, true);
    diagrammstop();
    emit setmenu(emenuCurves, TRUE);
    emit setmenu(emenuSweep, TRUE);
    //emit setmenu(emenuvfo, FALSE);
    emit setmenu(emenuMeasure, FALSE);
    emit setmenu(emSavePNG, TRUE);
    emit setmenu(emPrint, TRUE);
    emit setmenu(emPrintPdf, TRUE);
    restartMessure = FALSE;
    vmesstimer->stop();
    vfotimer->stop();
    vfomesstimer->stop();
    //IdleTimerStop();
    IdleTimerStart();
  }
  if(index == tab_nwt7vfo){
    setTabEnabled(tab_nwt7vfo, true);
    diagrammstop();
    emit setmenu(emenuCurves, FALSE);
    emit setmenu(emenuSweep, FALSE);
    //emit setmenu(emmenuvfo, TRUE);
    emit setmenu(emenuMeasure, FALSE);
    emit setmenu(emSavePNG, FALSE);
    emit setmenu(emPrint, FALSE);
    emit setmenu(emPrintPdf, FALSE);
    restartMessure = FALSE;
    vsweeptimer->stop();
    vmesstimer->stop();
    //vfotimer->stop();
    vfomesstimer->stop();
    setVfoToOutOnce();
    //and the encoders convert
    setFqFromLcdPanelToSetPanell();
    IdleTimerStart();
  }
  if(index == tab_nwt7measure){
    setTabEnabled(tab_nwt7measure, true);
    diagrammstop();
    emit setmenu(emenuCurves, FALSE);
    emit setmenu(emenuSweep, FALSE);
    //emit setmenu(emmenuvfo, FALSE);
    emit setmenu(emenuMeasure, TRUE);
    emit setmenu(emSavePNG, FALSE);
    emit setmenu(emPrint, FALSE);
    emit setmenu(emPrintPdf, FALSE);
    //    defaultlesenrtty(); //eventuelle Daten im UART loeschen    
    vsweeptimer->stop();
    vfotimer->stop();
    vfomesstimer->stop();
    restartMessure = TRUE;
    setMessureVfoMode(messVfoModeState);
    messsync=0;
  }
  if(index == tab_wkmanager){
    restartMessure = FALSE;
    emit setmenu(emenuCurves, TRUE);
    emit setmenu(emenuSweep, FALSE);
    //emit setmenu(emmenuvfo, FALSE);
    emit setmenu(emenuMeasure, FALSE);
    emit setmenu(emSavePNG, TRUE);
    emit setmenu(emPrint, TRUE);
    emit setmenu(emPrintPdf, TRUE);
    diagrammstop();
    vsweeptimer->stop();
    vmesstimer->stop();
    vfotimer->stop();
    vfomesstimer->stop();
    IdleTimerStart();
  }
  if(index == tab_calculation){
    setTabEnabled(tab_calculation, true);
    restartMessure = FALSE;
    emit setmenu(emenuCurves, FALSE);
    emit setmenu(emenuSweep, FALSE);
    //emit setmenu(emmenuvfo, FALSE);
    emit setmenu(emenuMeasure, FALSE);
    emit setmenu(emSavePNG, FALSE);
    emit setmenu(emPrint, FALSE);
    emit setmenu(emPrintPdf, FALSE);
    diagrammstop();
    vsweeptimer->stop();
    vmesstimer->stop();
    vfotimer->stop();
    vfomesstimer->stop();
    IdleTimerStart();
  }
  if(index == this->indexOf(wimpedance)){
    restartMessure = FALSE;
    emit setmenu(emenuCurves, FALSE);
    emit setmenu(emenuSweep, FALSE);
    //emit setmenu(emmenuvfo, FALSE);
    emit setmenu(emenuMeasure, FALSE);
    emit setmenu(emSavePNG, FALSE);
    emit setmenu(emPrint, FALSE);
    emit setmenu(emPrintPdf, FALSE);
    diagrammstop();
    vsweeptimer->stop();
    vmesstimer->stop();
    vfotimer->stop();
    vfomesstimer->stop();
    IdleTimerStart();
  }
  if(index == tab_wdiagramm){
    setTabEnabled(tab_wdiagramm, true);
    restartMessure = FALSE;
    sweepdata.eoperatemode = eSmithDiagramm;
    vsweeptimer->stop();
    vmesstimer->stop();
    vfotimer->stop();
    vfomesstimer->stop();
    IdleTimerStart();
    //wobdaten.mess.daten_enable = FALSE;
    diagrammdim(0);
    emit sendsweepdata(sweepdata);
  }
}


void Nwt7MainClass::resizeWidgets(){

  ///////////////////////////////////////////////
  // zuerst den VFO bereich

  nwt7vfo->setGeometry(0,0,5000,5000);
  editvfo->setGeometry(30,10,150,30);
  labelvfo->setGeometry(190,10,90,30);
  LCD1->setGeometry(30,50,200,45);
  rb1->setGeometry(240,50,20,40);
  LCD2->setGeometry(30,100,200,45);
  rb2->setGeometry(240,100,20,40);
  LCD3->setGeometry(30,150,200,45);
  rb3->setGeometry(240,150,20,40);
  LCD4->setGeometry(30,200,200,45);
  rb4->setGeometry(240,200,20,40);
  LCD5->setGeometry(30,250,200,45);
  rb5->setGeometry(240,250,20,40);
  editzf->setGeometry(60,300,100,20);
  checkboxzf->setGeometry(500,15,250,20);
  checkboxiqvfo->setGeometry(500,35,250,20);
  labelzf->setGeometry(30,300,25,20);
  labelhz->setGeometry(170,300,25,20);
  cbAttenuator2->setGeometry(250,14,60,20);
  labeldaempfung1->setGeometry(320,14,170,20);
  
  int vpos1 = 320;
  int vpos2 = vpos1+20;
  int vpos3 = vpos1+40;
  int vab = 40;
  int hpos1 = 265;
  
  labelfrqfaktorv->setGeometry(vpos1+20,hpos1-80,190,30);

  lmhz->setGeometry(vpos1+20,hpos1-40,90,30);
  lkhz->setGeometry(vpos2+30+3*vab,hpos1-40,90,30);
  lhz->setGeometry(vpos3+30+6*vab,hpos1-40,90,30);
  
  
  vsp1ghz->setGeometry(vpos1-40,hpos1,40,30);
  
  vsp100mhz->setGeometry(vpos1,hpos1,40,30);
  vsp10mhz->setGeometry(vpos1+vab,hpos1,40,30);
  vsp1mhz->setGeometry(vpos1+2*vab,hpos1,40,30);
  
  vsp100khz->setGeometry(vpos2+3*vab,hpos1,40,30);
  vsp10khz->setGeometry(vpos2+4*vab,hpos1,40,30);
  vsp1khz->setGeometry(vpos2+5*vab,hpos1,40,30);
  
  vsp100hz->setGeometry(vpos3+6*vab,hpos1,40,30);
  vsp10hz->setGeometry(vpos3+7*vab,hpos1,40,30);
  vsp1hz->setGeometry(vpos3+8*vab,hpos1,40,30);

  //wobbelfenster
  // grafik hat jetzt ein eigenes Window
  //  grafik->setGeometry(0, 0, wgrafik->width(), wgrafik->height());

  int gbreite, ghoehe;
  int spalte1;
  int spalte2;
  int spalte3;
  int abstand = 21;
  int i = 0;
  
  gbreite = width();
  ghoehe = height() - 50;
  
  if(gbreite < 750){
    spalte1 = 500;
    spalte2 = 0;
    spalte3 = 255;
  }else{
    spalte1 = 10;
    spalte2 = 250;
    spalte3 = 505;
  }

  mledit->setGeometry(spalte1, 10, 250, ghoehe);

  //Frequency set group
  gbSweep->setGeometry(spalte2+20, 10, 220, 185);
  editanfang->setGeometry(110, 15 + i*abstand,100,20);
  labelanfang->setGeometry(5, 17 + i*abstand,100,20);
  labelanfang->setAlignment(Qt::AlignRight);
  i++;
  editende->setGeometry(110, 15 + i*abstand,100,20);
  labelende->setGeometry(5, 17 + i*abstand, 100, 20);
  labelende->setAlignment(Qt::AlignRight);
  i++;
  editschrittweite->setGeometry(130, 15 + i*abstand,80,20);
  labelschrittweite->setGeometry(5, 17 + i*abstand, 120, 20);
  labelschrittweite->setAlignment(Qt::AlignRight);
  i++;
  editschritte->setGeometry(130, 15 + i*abstand,80,20);
  labelschritte->setGeometry(5, 17 + i*abstand, 120, 20);
  labelschritte->setAlignment(Qt::AlignRight);
  i++;
  boxzwischenzeit->setGeometry(130, 15 + i*abstand,80,20);
  labelzwischenzeit->setGeometry(5, 17 + i*abstand, 120, 20);
  labelzwischenzeit->setAlignment(Qt::AlignRight);
//  checkboxtime->setGeometry(5, 15 + i*abstand, 150, 20);
  i++;
  editdisplay->setGeometry(130, 15 + i*abstand,80,20);
  labeldisplayshift->setGeometry(5, 17 + i*abstand, 120, 20);
  labeldisplayshift->setAlignment(Qt::AlignRight);
  i++;
  boxprofil->setGeometry(80, 15 + i*abstand,130,20);
  labelprofil->setGeometry(5, 17 + i*abstand, 70, 20);
  labelprofil->setAlignment(Qt::AlignRight);
  i++;
  labelfrqfaktor->setGeometry(5, 17 + i*abstand, 190, 20);
  labelfrqfaktor->setAlignment(Qt::AlignRight);

  //ATT group
  gbAttenuator->setGeometry(spalte2+20, 200, 220, 40);
  cbAttenuator1->setGeometry(130, 15,80,20);
  labeldaempfung->setGeometry(5, 17, 110, 20);
  labeldaempfung->setAlignment(Qt::AlignRight);

  //Operate mode group
  gbOperatedStart->setGeometry(spalte2+20, 245, 220, 105);
  i=0;
  cbOperatedMode->setGeometry(80, 15 + i*abstand, 130, 20);
  labelbetriebsart->setGeometry(5, 18 + i*abstand,70, 20);
  labelbetriebsart->setAlignment(Qt::AlignRight);
  i++;
  kalibrier1->setGeometry(5, 18 + i*abstand, 200, 20);
  kalibrier1->setLayoutDirection(Qt::RightToLeft);
  cbResolutionSWR->setGeometry(100, 15 + i*abstand, 110, 20);
  labelaufloesung->setGeometry(5, 18 + i*abstand, 90, 20);
  labelaufloesung->setAlignment(Qt::AlignRight);

  groupsa->setGeometry(30, 15 + i*abstand, 170, 60);
  labelsafrqbereich->setGeometry(10, 15, 160, 20);
  sabereich1->setGeometry(10, 15, 160, 20);
  labelsabandbreite->setGeometry(10, 35, 160, 20);
  sabereich2->setGeometry(10, 35, 160, 20);
  i++;
  kalibrier2->setGeometry(5, 18 + i*abstand, 200, 20);
  kalibrier2->setLayoutDirection(Qt::RightToLeft);
  labela_100->setGeometry(5, 18 + i*abstand, 110, 20);
  edita_100->setGeometry(120, 15 + i*abstand, 90, 20);
  labela_100->setAlignment(Qt::AlignRight);
  i++;
  labelkabellaenge->setGeometry(5, 18 + i*abstand, 110, 20);
  editkabellaenge->setGeometry(120, 15 + i*abstand, 90, 20);
  labelkabellaenge->setAlignment(Qt::AlignRight);

  //  labelprogressbar->setAlignment(Qt::AlignRight);

  //Sweep buttons group
  buttonwobbeln->setGeometry(spalte3+150, 10, 80, 30);
  buttoneinmal->setGeometry(spalte3+150, 45, 80, 30);
  buttonstop->setGeometry(spalte3+150, 80, 80, 30);

  //Markers group
  i = 0;
  groupbandbreite->setGeometry(spalte3, 10, 140, 100);
  checkbox3db->setGeometry(5, 15, 130, 20);
  checkbox3db->setLayoutDirection(Qt::RightToLeft);
  checkbox6db->setGeometry(5, 35, 130, 20);
  checkbox6db->setLayoutDirection(Qt::RightToLeft);
  checkboxgrafik->setGeometry(5, 55, 130, 20);
  checkboxgrafik->setLayoutDirection(Qt::RightToLeft);
  checkboxinvers->setGeometry(5, 75, 130, 20);
  checkboxinvers->setLayoutDirection(Qt::RightToLeft);

  //Zoom display group
  gbZoom->setGeometry(spalte3, 111,230,48);
  labellupe->setGeometry(5, 24, 95, 20);
  labellupe->setAlignment(Qt::AlignRight);
  buttonlupeplus->setGeometry(110, 15, 30, 30);
  buttonlupeminus->setGeometry(140, 15, 30, 30);
  buttonlupemitte->setGeometry(170, 15, 30, 30);

  //Probe Profile select
  groupkanal->setGeometry(spalte3, 160, 230, 58);
  checkboxk1->setGeometry(5, 15,210,20);
  checkboxk1->setLayoutDirection(Qt::RightToLeft);
  checkboxk2->setGeometry(5, 35,210,20);
  checkboxk2->setLayoutDirection(Qt::RightToLeft);

  //Progress Bar Box
  groupshift->setGeometry(spalte3, 218, 230, 132);
  labelboxydbmax->setGeometry(5, 33, 60, 20);
  labelboxydbmax->setAlignment(Qt::AlignRight);
  labelboxydbmin->setGeometry(5, 58, 60, 20);
  labelboxydbmin->setAlignment(Qt::AlignRight);
  boxydbmax->setGeometry(70, 30, 50, 20);
  boxydbmin->setGeometry(70, 55, 50, 20);
  labelboxdbshift1->setGeometry(123, 33, 45, 20);
  labelboxdbshift1->setAlignment(Qt::AlignRight);
  labelboxdbshift2->setGeometry(123, 58, 45, 20);
  labelboxdbshift2->setAlignment(Qt::AlignRight);
  boxdbshift1->setGeometry(175, 30, 45, 20);
  boxdbshift2->setGeometry(175, 55, 45, 20);

  //  groupkursor->setGeometry(spalte3, 276, 220, 38);
  labelkursornr->setGeometry(15, 83, 150, 20);
  labelkursornr->setAlignment(Qt::AlignRight);
  boxkursornr->setGeometry(175, 80, 45, 20);

  //  groupbar->setGeometry(spalte3, 314, 220, 35);
  progressbar->setGeometry(165, 109, 56, 10);
  labelnwt->setGeometry(5, 106, 40, 20);
  labelnwt->setAlignment(Qt::AlignRight);
  labelprogressbar->setGeometry(50, 106, 100, 20);
  labelprogressbar->setAlignment(Qt::AlignRight);

  //WIDGET Messure
  progressbaPowerMeter1->setGeometry(10,60,600,20);
  progressbaPowerMeter2->setGeometry(10,160,600,20);

  boxwattoffset1->setGeometry(10,81,180,20);
  labelldaempfungk1->setGeometry(220,81,110,20);
  ldampingk1->setGeometry(330,81,100,20);
  checkboxmesshang1->setGeometry(450,81,100,20);

  boxwattoffset2->setGeometry(10,181,180,20);
  labelldaempfungk2->setGeometry(220,181,110,20);
  ldaempfungk2->setGeometry(330,181,100,20);
  checkboxmesshang2->setGeometry(450,181,100,20);

  mlabelk1->setGeometry(610,60,40,20);
  mlabelk2->setGeometry(610,160,40,20);
  messlabel1->setGeometry(10,10,185,50);
  messlabel2->setGeometry(10,110,185,50);
  messlabel3->setGeometry(200,10,185,50);
  messlabel4->setGeometry(200,110,185,50);
  messlabel5->setGeometry(390,10,185,50);
  messlabel6->setGeometry(390,110,185,50);
  messedit->setGeometry(10,220,560,ghoehe-210);

  buttonmesssave->setGeometry(590,220,110,25);
  buttonmess->setGeometry(590,250,110,25);
  //buttonvfo->setGeometry(590,280,110,30);
  labellMessVfoState->setGeometry(590,280,110,20);
  rbVfoOn ->setGeometry(590,300,110,20);
  rbMessVfoOn ->setGeometry(590,320,110,20);
  rbVFOSOff ->setGeometry(590,340,110,20);

//  labelspghz->setGeometry(30,240,90,30);
  sp1ghz->setGeometry(10,270,40,30);

  labelfrqfaktorm->setGeometry(70,210,190,30);

  labelspmhz->setGeometry(70,240,90,30);
  sp100mhz->setGeometry(50,270,40,30);
  sp10mhz->setGeometry(90,270,40,30);
  sp1mhz->setGeometry(130,270,40,30);

  labelspkhz->setGeometry(220,240,90,30);
  sp100khz->setGeometry(190,270,40,30);
  sp10khz->setGeometry(230,270,40,30);
  sp1khz->setGeometry(270,270,40,30);

  labelsphz->setGeometry(370,240,90,30);
  sp100hz->setGeometry(330,270,40,30);
  sp10hz->setGeometry(370,270,40,30);
  sp1hz->setGeometry(410,270,40,30);

  cbAttenuator3->setGeometry(460,270,60,20);
  labeldaempfung3->setGeometry(460,240,170,20);

  //WIDGET CALCULATE berechnungsfenster
  groupschwingkreisc->setGeometry(20,20,250,110);
  editf2->setGeometry(20, 20, 100, 20);
  labelf2->setGeometry(130, 20, 110, 20);
  editl2->setGeometry(20, 40, 100, 20);
  labell2->setGeometry(130, 40, 110, 20);
  ergebnisc->setGeometry(25, 60, 95, 20);
  labelc2->setGeometry(130, 60, 110, 20);
  ergebnisxc->setGeometry(25, 80, 95, 20);
  labelxc->setGeometry(130, 80, 110, 20);

  groupschwingkreisl->setGeometry(20,160,250,150);
  editf1->setGeometry(20, 20, 100, 20);
  labelf1->setGeometry(130, 20, 110, 20);
  editc1->setGeometry(20, 40, 100, 20);
  labelc1->setGeometry(130, 40, 110, 20);
  ergebnisl->setGeometry(25, 60, 95, 20);
  labell1->setGeometry(130, 60, 110, 20);
  editn->setGeometry(20,100, 100, 20);
  labeln->setGeometry(130, 100, 110, 20);
  ergebnisal->setGeometry(25, 120, 95, 20);
  labelal->setGeometry(130, 120, 110, 20);
  ergebnisxl->setGeometry(25, 80, 95, 20);
  labelxl->setGeometry(130, 80, 110, 20);

  groupwindungen->setGeometry(290,20,250,110);
  edital3->setGeometry(20, 20, 100, 20);
  labeledital3->setGeometry(130, 20, 110, 20);
  editl3->setGeometry(20, 40, 100, 20);
  labeleditl3->setGeometry(130, 40, 110, 20);
  ergebnisw->setGeometry(25, 60, 95, 20);
  labelergebnisw->setGeometry(130, 60, 110, 20);

  gimp->setGeometry(20,20,250,50);
  rbr->setGeometry(20,20,100,20);
  rblc->setGeometry(120,20,100,20);

  bild1->setGeometry(280,110,444,156);
  gzr->setGeometry(20,100,250,220);
  editz1->setGeometry(20, 20, 50, 20);
  labeleditz1->setGeometry(90, 20, 90, 20);
  editz2->setGeometry(20, 40, 50, 20);
  labeleditz2->setGeometry(90, 40, 90, 20);
  editz3->setGeometry(20, 60, 50, 20);
  labeleditz3->setGeometry(90, 60, 90, 20);
  editz4->setGeometry(20, 80, 50, 20);
  labeleditz4->setGeometry(90, 80, 90, 20);
  lr1->setGeometry(20, 100, 50, 20);
  lbeschrr1->setGeometry(90, 100, 90, 20);
  lr2->setGeometry(20, 120, 50, 20);
  lbeschrr2->setGeometry(90, 120, 90, 20);
  lr3->setGeometry(20, 140, 50, 20);
  lbeschrr3->setGeometry(90, 140, 90, 20);
  lr4->setGeometry(20, 160, 50, 20);
  lbeschrr4->setGeometry(90, 160, 90, 20);
  ldaempfung->setGeometry(20, 180, 50, 20);
  lbeschrdaempfung->setGeometry(90, 180, 160, 20);

  bild2->setGeometry(280,110,444,209);
  gzlc->setGeometry(20,100,250,220);
  editzlc1->setGeometry(20, 20, 50, 20);
  labeleditzlc1->setGeometry(90, 20, 90, 20);
  editzlc2->setGeometry(20, 40, 50, 20);
  labeleditzlc2->setGeometry(90, 40, 90, 20);
  editzlc3->setGeometry(20, 60, 50, 20);
  labeleditzlc3->setGeometry(90, 60, 90, 20);
  ll->setGeometry(20, 80, 50, 20);
  lbeschrl->setGeometry(90, 80, 90, 20);
  lc->setGeometry(20, 100, 50, 20);
  lbeschrc->setGeometry(90, 100, 90, 20);

  gdiagramm->setGeometry(10, 10, 160, 200);
  bstart->setGeometry(10, 20, 100, 20);
  bstop->setGeometry(10, 50, 100, 20);
  bsave->setGeometry(10, 80, 100, 20);
  lanzeige->setGeometry(10, 110, 200, 20);
  bsim->setGeometry(10, 170, 100, 20);

  gdiagrammdim->setGeometry(180, 10, 230, 150);
  ldbmax->setGeometry(10, 20, 50, 20);
  boxdbmax->setGeometry(80, 20, 80, 20);
  ldbmin->setGeometry(10, 40, 50, 20);
  boxdbmin->setGeometry(80, 40, 80, 20);
  lbegin->setGeometry(10, 70, 60, 20);
  spinadbegin->setGeometry(80, 70, 50, 20);
  spingradbegin->setGeometry(130, 70, 50, 20);
  lbegin1->setGeometry(180, 70, 50, 20);
  lend->setGeometry(10, 100, 60, 20);
  spinadend->setGeometry(80, 100, 50, 20);
  spingradend->setGeometry(130, 100, 50, 20);
  lend1->setGeometry(180, 100, 50, 20);
}

///////////////////////////////////////////////////////////////////////////////
// Widget VFO
void Nwt7MainClass::editVFO(){
  double l;
  QString s;
  QString s1;
  
  s = editvfo->text();
  l = linenormalisieren(s, true);
  //Frequenzen zwischen 10 Hz und 200 MHz
  if(l > 10.0 && l < 9999999999.0){
    //Drehgeber auslesen
    s1.sprintf("%4.6f", l / 1000000.0);
    //auf Display darstellen
    switch(lcdauswahl)
    {
      case 1:
        LCD1->display(s1);
        lcdfrq1 = l;
        break;
      case 2:
        LCD2->display(s1);
        lcdfrq2 = l;
        break;
      case 3:
        LCD3->display(s1);
        lcdfrq3 = l;
        break;
      case 4:
        LCD4->display(s1);
        lcdfrq4 = l;
        break;
      case 5:
        LCD5->display(s1);
        lcdfrq5 = l;
        break;
    }
    setFqFromLcdPanelToSetPanell();
  }
}

void Nwt7MainClass::LCDaendern(){
  double l;
  QString s;

  //Drehgeber auslesen
  //Werte zusammenfuegen
  l = getdrehgeber();
  // komma einfuegen nach Mhz
  s.sprintf("%4.6f", l / 1000000.0);
  //auf Display darstellen
  switch(lcdauswahl)
  {
    case 1:
      LCD1->display(s);
      lcdfrq1 = l;
      break;
    case 2:
      LCD2->display(s);
      lcdfrq2 = l;
      break;
    case 3:
      LCD3->display(s);
      lcdfrq3 = l;
      break;
    case 4:
      LCD4->display(s);
      lcdfrq4 = l;
      break;
    case 5:
      LCD5->display(s);
      lcdfrq5 = l;
      break;
  }
  s.sprintf("%3.0f", l);
  editvfo->setText(s);
}

void Nwt7MainClass::setVfoToOutOnce(){
  //timer start after 10 mSec
  IdleTimerStop();
  vfotimer->start(10);
}

void Nwt7MainClass::VfoTimerPerform(){
  if(!bersterstart){
        QString qs;
        QString s;
        double fr = 1000000.0;
        double zf = 0.0;
#ifdef TIMERDEBUG
    qDebug(">>VfoTimerPerform()");
#endif
        //after the end of the vfoTimers frequency is sent to the NWT
        if(!bttyOpen){
            #ifdef LDEBUG
            qDebug("TTY CLOSED <> VfoTimerPerform()");
            #endif
            vfotimer->stop();
            idletimer->start(currentdata.idletime);
            return;
        }
        switch(lcdauswahl)
        {
        case 1:fr = lcdfrq1;break;
        case 2:fr = lcdfrq2;break;
        case 3:fr = lcdfrq3;break;
        case 4:fr = lcdfrq4;break;
        case 5:fr = lcdfrq5;break;
        }
        //Point representation again wegrechnen
        //ZF read with abbreviations
        qs = editzf->text();
        zf = linenormalisieren(qs, false);
        qs.sprintf("%1.0f", zf);
        editzf->setText(qs);
        //in Hz umrechnen
        //fr *= 1000000.0;
        //Frequency factor einrechnen
        fr = qRound(fr / double(sweepdata.freq_faktor));
        //ZF addieren
        fr += zf;
        // I/Q Frequenz * 4
        if(checkboxiqvfo->isChecked())fr = fr * 4.0;
        if((fr < 1000000000.0) and fr >= 0.0){
            s=QString().sprintf("f%09.0f",fr);
            //qDebug(s);
            defaultClearTty(); //delete any data in the UART
            picmodem->writeChar(143);
            picmodem->writeLine(s);
            #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(s);
            #endif
            }
   }
  //timer stop because task is completed
  vfotimer->stop();
  if(restartMessure){
       switch(messVfoModeState){
        case 0: //VFO
            startMessure();
            break;
        case 1: //MessVFO
            startMessure();
            break;
        case 2:
            /*@TODO: NEED Stop DDS OUT*/
          IdleTimerStart();
      }
  } else
          IdleTimerStart();
}


double Nwt7MainClass::getdrehgeber()
{
  double wert = double(vsp1hz->value()) + 
                double(vsp10hz->value())   * 10.0 +
                double(vsp100hz->value())  * 100.0 +
                double(vsp1khz->value())   * 1000.0 +
                double(vsp10khz->value())  * 10000.0 +
                double(vsp100khz->value()) * 100000.0 +
                double(vsp1mhz->value())   * 1000000.0 +
                double(vsp10mhz->value())  * 10000000.0 +
                double(vsp100mhz->value()) * 100000000.0 +
                double(vsp1ghz->value())   * 1000000000.0;
  return wert;  
}

void Nwt7MainClass::setFqFromLcdPanelToSetPanell()
{
  double fr = 10000000.0;

  switch(lcdauswahl)
  {
    case 1:
      fr = lcdfrq1;
      break;
    case 2:
      fr = lcdfrq2;
      break;
    case 3:
      fr = lcdfrq3;
      break;
    case 4:
      fr = lcdfrq4;
      break;
    case 5:
      fr = lcdfrq5;
      break;
  }
  setFqToSetPanel(fr);
}

void Nwt7MainClass::change1hz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp1hz->setValue(0);
    int b = vsp10hz->value();
    b++;
    vsp10hz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(vsp100mhz->value() != 0)isnull = FALSE;
    if(vsp10mhz->value() != 0)isnull = FALSE;
    if(vsp1mhz->value() != 0)isnull = FALSE;
    if(vsp100khz->value() != 0)isnull = FALSE;
    if(vsp10khz->value() != 0)isnull = FALSE;
    if(vsp1khz->value() != 0)isnull = FALSE;
    if(vsp100hz->value() != 0)isnull = FALSE;
    if(vsp10hz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp1hz->setValue(0);
    }else{
      vsp1hz->setValue(9);
      int b = vsp10hz->value();
      b--;
      vsp10hz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change10hz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp10hz->setValue(0);
    int b = vsp100hz->value();
    b++;
    vsp100hz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(vsp100mhz->value() != 0)isnull = FALSE;
    if(vsp10mhz->value() != 0)isnull = FALSE;
    if(vsp1mhz->value() != 0)isnull = FALSE;
    if(vsp100khz->value() != 0)isnull = FALSE;
    if(vsp10khz->value() != 0)isnull = FALSE;
    if(vsp1khz->value() != 0)isnull = FALSE;
    if(vsp100hz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp10hz->setValue(0);
    }else{
      vsp10hz->setValue(9);
      int b = vsp100hz->value();
      b--;
      vsp100hz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change100hz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp100hz->setValue(0);
    int b = vsp1khz->value();
    b++;
    vsp1khz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(vsp100mhz->value() != 0)isnull = FALSE;
    if(vsp10mhz->value() != 0)isnull = FALSE;
    if(vsp1mhz->value() != 0)isnull = FALSE;
    if(vsp100khz->value() != 0)isnull = FALSE;
    if(vsp10khz->value() != 0)isnull = FALSE;
    if(vsp1khz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp100hz->setValue(0);
    }else{
      vsp100hz->setValue(9);
      int b = vsp1khz->value();
      b--;
      vsp1khz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change1khz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp1khz->setValue(0);
    int b = vsp10khz->value();
    b++;
    vsp10khz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(vsp100mhz->value() != 0)isnull = FALSE;
    if(vsp10mhz->value() != 0)isnull = FALSE;
    if(vsp1mhz->value() != 0)isnull = FALSE;
    if(vsp100khz->value() != 0)isnull = FALSE;
    if(vsp10khz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp1khz->setValue(0);
    }else{
      vsp1khz->setValue(9);
      int b = vsp10khz->value();
      b--;
      vsp10khz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change10khz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp10khz->setValue(0);
    int b = vsp100khz->value();
    b++;
    vsp100khz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(vsp100mhz->value() != 0)isnull = FALSE;
    if(vsp10mhz->value() != 0)isnull = FALSE;
    if(vsp1mhz->value() != 0)isnull = FALSE;
    if(vsp100khz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp10khz->setValue(0);
    }else{
      vsp10khz->setValue(9);
      int b = vsp100khz->value();
      b--;
      vsp100khz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change100khz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp100khz->setValue(0);
    int b = vsp1mhz->value();
    b++;
    vsp1mhz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(vsp100mhz->value() != 0)isnull = FALSE;
    if(vsp10mhz->value() != 0)isnull = FALSE;
    if(vsp1mhz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp100khz->setValue(0);
    }else{
      vsp100khz->setValue(9);
      int b = vsp1mhz->value();
      b--;
      vsp1mhz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change1mhz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp1mhz->setValue(0);
    int b = vsp10mhz->value();
    b++;
    vsp10mhz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(vsp100mhz->value() != 0)isnull = FALSE;
    if(vsp10mhz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp1mhz->setValue(0);
    }else{
      vsp1mhz->setValue(9);
      int b = vsp10mhz->value();
      b--;
      vsp10mhz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change10mhz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp10mhz->setValue(0);
    int b = vsp100mhz->value();
    b++;
    vsp100mhz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(vsp100mhz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp10mhz->setValue(0);
    }else{
      vsp10mhz->setValue(9);
      int b = vsp100mhz->value();
      b--;
      vsp100mhz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change100mhz(int a)
{
  bool isnull = TRUE;
  
  if(a==10){
    vsp100mhz->setValue(0);
    int b = vsp1ghz->value();
    b++;
    vsp1ghz->setValue(b);
  }
  if(a==-1){
    if(vsp1ghz->value() != 0)isnull = FALSE;
    if(isnull){
      vsp100mhz->setValue(0);
    }else{
      vsp100mhz->setValue(9);
      int b = vsp1ghz->value();
      b--;
      vsp1ghz->setValue(b);
    }
  }
  LCDaendern();
  setVfoToOutOnce();
}

void Nwt7MainClass::change1ghz(int a)
{
  if(a==10){
    vsp1ghz->setValue(9);
  }
  if(a==-1){
    vsp1ghz->setValue(0);
  }
  LCDaendern();
  setVfoToOutOnce();
}

double Nwt7MainClass::checkVFOfrequency(double freq)
{
    QString s_if_freq;
    double if_freq;
    //@TODO: How check max VFOfrequency ???

    //Representation IF frequency
    s_if_freq = editzf->text();
    if_freq = linenormalisieren(s_if_freq, false);
    s_if_freq.sprintf("%1.0f", if_freq);
    editzf->setText(s_if_freq);
    //in Hz umrechnen
    //fr *= 1000000.0;
    //Frequency factor einrechnen
    freq = qRound(freq / double(sweepdata.freq_faktor));
    //IF
    freq += if_freq;
    // I/Q Frequenz * 4
    if(checkboxiqvfo->isChecked()) freq = freq * 4.0;
    return freq;
}

void Nwt7MainClass::setFqToSetPanel(double inFq)
{
  double fr;
  int a;
  
  fr = inFq;
  a = int(fr/1000000000.0);
  vsp1ghz->setValue(a);
  fr = fr - double(a) * 1000000000.0;
  a = int(fr/100000000.0);
  vsp100mhz->setValue(a);
  fr = fr - double(a) * 100000000.0;
  a = int(fr/10000000.0);
  vsp10mhz->setValue(a);
  fr = fr - double(a) * 10000000.0;
  a = int(fr/1000000.0);
  vsp1mhz->setValue(a);
  fr = fr - double(a) * 1000000.0;
  a = int(fr/100000.0);
  vsp100khz->setValue(a);
  fr = fr - double(a) * 100000.0;
  a = int(fr/10000.0);
  vsp10khz->setValue(a);
  fr = fr - double(a) * 10000.0;
  a = int(fr/1000.0);
  vsp1khz->setValue(a);
  fr = fr - double(a) * 1000.0;
  a = int(fr/100.0);
  vsp100hz->setValue(a);
  fr = fr - double(a) * 100.0;
  a = int(fr/10.0);
  vsp10hz->setValue(a);
  fr = fr - double(a) * 10.0;
  vsp1hz->setValue(int(fr));
}

void Nwt7MainClass::lcd1clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(TRUE);
  rb2->setChecked(FALSE);
  rb3->setChecked(FALSE);
  rb4->setChecked(FALSE);
  rb5->setChecked(FALSE);
  
  LCD1->setPalette(dp);
  LCD2->setPalette(dpd);
  LCD3->setPalette(dpd);
  LCD4->setPalette(dpd);
  LCD5->setPalette(dpd);

  //  Dial1hz->setFocus();
  lcdauswahl = 1;
  setFqFromLcdPanelToSetPanell();
}

void Nwt7MainClass::lcd2clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(FALSE);
  rb2->setChecked(TRUE);
  rb3->setChecked(FALSE);
  rb4->setChecked(FALSE);
  rb5->setChecked(FALSE);
  
  LCD1->setPalette(dpd);
  LCD2->setPalette(dp);
  LCD3->setPalette(dpd);
  LCD4->setPalette(dpd);
  LCD5->setPalette(dpd);

//  Dial1hz->setFocus();
  lcdauswahl = 2;
  setFqFromLcdPanelToSetPanell();
}

void Nwt7MainClass::lcd3clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(FALSE);
  rb2->setChecked(FALSE);
  rb3->setChecked(TRUE);
  rb4->setChecked(FALSE);
  rb5->setChecked(FALSE);
  
  LCD1->setPalette(dpd);
  LCD2->setPalette(dpd);
  LCD3->setPalette(dp);
  LCD4->setPalette(dpd);
  LCD5->setPalette(dpd);

  //  Dial1hz->setFocus();
  lcdauswahl = 3;
  setFqFromLcdPanelToSetPanell();
}

void Nwt7MainClass::lcd4clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(FALSE);
  rb2->setChecked(FALSE);
  rb3->setChecked(FALSE);
  rb4->setChecked(TRUE);
  rb5->setChecked(FALSE);
  
  LCD1->setPalette(dpd);
  LCD2->setPalette(dpd);
  LCD3->setPalette(dpd);
  LCD4->setPalette(dp);
  LCD5->setPalette(dpd);

//  Dial1hz->setFocus();
  lcdauswahl = 4;
  setFqFromLcdPanelToSetPanell();
}

void Nwt7MainClass::lcd5clicked(){
  QPalette dp;
  dp.setColor(QPalette::Background, Qt::yellow);
  QPalette dpd;
  dpd.setColor(QPalette::Background, Qt::darkYellow);

  rb1->setChecked(FALSE);
  rb2->setChecked(FALSE);
  rb3->setChecked(FALSE);
  rb4->setChecked(FALSE);
  rb5->setChecked(TRUE);
  
  LCD1->setPalette(dpd);
  LCD2->setPalette(dpd);
  LCD3->setPalette(dpd);
  LCD4->setPalette(dpd);
  LCD5->setPalette(dp);

  //  Dial1hz->setFocus();
  lcdauswahl = 5;
  setFqFromLcdPanelToSetPanell();
}
/*
void Nwt7linux::changedrehgeber(){
  double fr = 1000000.0, fr1;

  switch(lcdauswahl)
  {
    case 1:fr = LCD1->value();break;
    case 2:fr = LCD2->value();break;
    case 3:fr = LCD3->value();break;
    case 4:fr = LCD4->value();break;
    case 5:fr = LCD5->value();break;
  }
  //Kommadarstellung wieder wegrechnen
  fr = fr * 1000000.0; //Darstellung in Herz
  fr1 = floor(fr / 1000000.0);
  Dial1mhz->setValue(int(fr1));
  fr = fr - fr1 * 1000000.0;
  fr1 = floor(fr / 1000.0);
  Dial1khz->setValue(int(fr1));
  fr = fr - fr1 * 1000.0;
  Dial1hz->setValue(int(fr));
}
*/
void Nwt7MainClass::setFrequenzmarken(){
  int r;

  FreqMarkerDlg *dlg = new FreqMarkerDlg(this);
  //Frequenzmarken in den Dialog
  dlg->setdaten(frqmarken);
  QFont font("Helvetica", currentdata.pfsize);
  dlg->setFont(font);
  r = dlg->exec();
  if(r == QDialog::Accepted){
    //    qDebug("ok");
    //Frequenzmarken aus den Dialog holen
    frqmarken = dlg->getdaten();
    //Frequenzmarken ins Wobbelfenster 
    emit sendefrqmarken(frqmarken);
  }
  delete dlg;
}

void Nwt7MainClass::setHLine(){
  bool ok;
  sweepdata.hline = QInputDialog::getDouble(this, "dB-line", "Input in dB", sweepdata.hline, -120.0, 50.0, 2, &ok);
  emit sendsweepdata(sweepdata);
}

void Nwt7MainClass::editProfile(){
  int i, j, r;
  TProfile cprofil;
  
  ProfileDlg *dlg = new ProfileDlg(this);
  //Frequenzmarken in den Dialog
  QFont font("Helvetica", currentdata.pfsize);
  dlg->setFont(font);
  dlg->tip(btip);
  j = boxprofil->count();
  if(j>1){
    for(i=0;i<j-1;i++){
      cprofil.fqstart = aprofil[i+1].fqstart;
      cprofil.fqend = aprofil[i+1].fqend;
      cprofil.cntpoints = aprofil[i+1].cntpoints;
      cprofil.beschr = aprofil[i+1].name;
      //qDebug("S " + cprofil.beschr);
      dlg->setprofil(cprofil,i);
    }
  }
  r = dlg->exec();
  if(r == QDialog::Accepted){
    // alle Profile ausser default loeschen
    for(i=1; i<profilanz; i++){
      //qDebug(aprofil[i].name);
      aprofil[i].name = "NN";
    }
    for(i=0;i<profilanz;i++){
      cprofil = dlg->getprofil(i);
      aprofil[i+1].fqstart = cprofil.fqstart;
      aprofil[i+1].fqend = cprofil.fqend;
      aprofil[i+1].name = cprofil.beschr;
      aprofil[i+1].cntpoints = cprofil.cntpoints;
      //qDebug("G " + cprofil.beschr);
    }
    boxprofil->clear();
    TFProfile bprofil[profilanz+1];
    j=0;
    for(i=0; i<=profilanz; i++){
      bprofil[i].name="NN";
    }
    for(i=0; i<=profilanz; i++){
      if(aprofil[i].name != "NN"){
        bprofil[j]=aprofil[i];
        j++;
      }
    }
    for(i=0; i<=profilanz; i++){
      aprofil[i]=bprofil[i];
    }
    for(i=0; i<=profilanz; i++){
      //qDebug(aprofil[i].name);
      if(aprofil[i].name != "NN")boxprofil->addItem(aprofil[i].name);
    }
  }
  delete dlg;
}

void Nwt7MainClass::option(){
  int r;
  double adds_core_freq;
  int apll;
  QString stty;
  double dkonst;
  long double ddskonst;
  qulonglong test;
  QString qs;
  QString qpll;
  
  adds_core_freq = currentdata.dds_core_freq;
  apll = currentdata.pll;
  OptionDlg *dlg = new OptionDlg(this);
  dlg->psetfontsize(currentdata.pfsize);
  //wgrunddaten.kalibrierk1 = wobdaten.bkalibrierkorrk1;
  //wgrunddaten.kalibrierk2 = wobdaten.bkalibrierkorrk2;
  currentdata.numberchanel = sweepdata.numberchanel;
  currentdata.strsonde1 = sweepdata.namesonde1;
  currentdata.strsonde2 = sweepdata.namesonde2;
  currentdata.bswriteration = sweepdata.bswriteration;
  currentdata.penkanal1 = sweepdata.penkanal1;
  currentdata.penkanal2 = sweepdata.penkanal2;
  currentdata.colorhintergrund = sweepdata.colorhintergrund;
  currentdata.psav300 = sweepdata.psav300;
  currentdata.psav7k = sweepdata.psav7k;
  currentdata.psav30k = sweepdata.psav30k;
  currentdata.bsavdbm = sweepdata.bsavdbm;
  currentdata.bsavuv = sweepdata.bsavuv;
  currentdata.bsavwatt = sweepdata.bsavwatt;
  currentdata.bsavdbuv = sweepdata.bsavdbuv;
  currentdata.safehlermax = sweepdata.safehlermax;
  currentdata.safehlermin = sweepdata.safehlermin;
  currentdata.freq_faktor = sweepdata.freq_faktor;
  currentdata.penwidth = sweepdata.penwidth;
  stty = currentdata.str_tty;
  dlg->setdaten(currentdata);
  dlg->setsavcal(sweepdata.savcalarray);
  dlg->setkalibrierarray(sweepdata.kalibrierarray);
  dlg->tip(btip);
  r = dlg->exec();
  bool dlgRes;
  dlgRes = r == QDialog::Accepted;
  if(dlgRes) {
        vmesstimer->stop();
        IdleTimerStop();
        currentdata = dlg->getdaten();
        setAttItem(); //Daempfungsglied einstellen
        if(currentdata.calibrate_end_freq > currentdata.max_sweep_freq){
            QMessageBox::warning( this, tr("Frequency limits",""),
                                        tr("<br>  The stopfrequency of the calibration is higher than the max. sweepfrequency ! <br> <center><b> The stopfrequency will be corrected.</b></center><br>",""));
          currentdata.calibrate_end_freq = currentdata.max_sweep_freq;
        }
        if(sweepdata.eoperatemode == eSpectrumAnalyser){
          setOperateMode(4);
        }
        if(sweepdata.eoperatemode == eSpectrumDisplayShift){
          setOperateMode(5);
        }
        //wobdaten.bkalibrierkorrk1 = wgrunddaten.kalibrierk1;
        //wobdaten.bkalibrierkorrk2 = wgrunddaten.kalibrierk2;
        sweepdata.numberchanel = currentdata.numberchanel;
        sweepdata.namesonde1 = currentdata.strsonde1;
        sweepdata.namesonde2 = currentdata.strsonde2;
        sweepdata.bswriteration = currentdata.bswriteration;
        sweepdata.penkanal1 = currentdata.penkanal1;
        sweepdata.penkanal2 = currentdata.penkanal2;
        sweepdata.penwidth = currentdata.penwidth;
        sweepdata.penkanal1.setWidth(sweepdata.penwidth);
        sweepdata.penkanal2.setWidth(sweepdata.penwidth);
        sweepdata.colorhintergrund = currentdata.colorhintergrund;
        sweepdata.psav300 = currentdata.psav300;
        sweepdata.psav7k = currentdata.psav7k;
        sweepdata.psav30k = currentdata.psav30k;
        sweepdata.bsavdbm = currentdata.bsavdbm;
        sweepdata.bsavuv = currentdata.bsavuv;
        sweepdata.bsavwatt = currentdata.bsavwatt;
        sweepdata.bsavdbuv = currentdata.bsavdbuv;
        sweepdata.safehlermax = currentdata.safehlermax;
        sweepdata.safehlermin = currentdata.safehlermin;
        sweepdata.freq_faktor = currentdata.freq_faktor;

        setFrqFaktorLabel();
        sweepdata.savcalarray = dlg->getsavcal();
        emit sendsweepdata(sweepdata);
        qs = "sav.cal";
        QFile f;
        int i;
        f.setFileName(homedir.filePath(qs));
        if(f.open(QIODevice::WriteOnly)){
            #ifdef LDEBUG
            qDebug() << qs;
            #endif
            QTextStream ts(&f);
            //zuerst die Versionsnummer der SAV-Kalibrierung
            ts << "#V4.04" << endl;
            for(i=0; i< calmaxindex; i++){
                ts << sweepdata.savcalarray.p[i] << endl;
            }
            f.close();
        }
        //    Dial1mhz->setMaximum(int(wgrunddaten.maxvfo / 1000000.0)-1);
        if(sweepdata.numberchanel){
            if(currentdata.kalibrierk1 and !sweepdata.linear1){
            setColorRedk1();
                currentdata.kalibrierstep = maxmesspunkte;
            }else{
                setColorBlackk1();
            }
            checkboxk2->setChecked(FALSE);
            checkboxk2->setEnabled(FALSE);
            progressbaPowerMeter2->setValue(0);
            progressbaPowerMeter2->setEnabled(FALSE);
            ldaempfungk2->setEnabled(FALSE);
            boxwattoffset2->setEnabled(FALSE);
            messlabel2->setText("");
            messlabel2->setEnabled(FALSE);
            messlabel4->setText("");
            messlabel4->setEnabled(FALSE);
            messlabel6->setText("");
            messlabel6->setEnabled(FALSE);
            mlabelk2->setText("");
            mlabelk2->setEnabled(FALSE);
            labelldaempfungk2->setText("");
            checkboxmesshang2->setEnabled(FALSE);
            //      checkboxM10dbK2->setEnabled(FALSE);
            //      checkboxM20dbK2->setEnabled(FALSE);
            emit setmenu(emCalibrateK2, FALSE);
            emit setmenu(emLoadCallibrateK2, FALSE);
            emit setmenu(emSaveCallibrateK2, FALSE);
            emit setmenu(emWattCallibrateK2, FALSE);
            emit setmenu(emWattEditSonde2, FALSE);
        }else{
            if(currentdata.kalibrierk1 and !sweepdata.linear1){
                setColorRedk1();
                currentdata.kalibrierstep = maxmesspunkte;
            }else{
                setColorBlackk1();
            }
            if(currentdata.kalibrierk2 and !sweepdata.linear2){
                setColorRedk2();
                currentdata.kalibrierstep = maxmesspunkte;
            }else{
                setColorBlackk2();
            }
            //Aenderung nur erlauben wenn Betriebsart Wobbeln, sonst wird Eichen K2 frei obwohl SWR eingestellt ist
            if(sweepdata.eoperatemode == eGainSweep){
                checkboxk2->setEnabled(TRUE);
                progressbaPowerMeter2->setEnabled(TRUE);
                ldaempfungk2->setEnabled(TRUE);
                labelldaempfungk2->setText(tr("Attenuation (dB)",""));
                checkboxmesshang2->setEnabled(TRUE);
                boxwattoffset2->setEnabled(TRUE);
                messlabel2->setEnabled(TRUE);
                messlabel4->setEnabled(TRUE);
                messlabel6->setEnabled(TRUE);
                mlabelk2->setEnabled(TRUE);
                //      checkboxM10dbK2->setEnabled(TRUE);
                //      checkboxM20dbK2->setEnabled(TRUE);
                emit setmenu(emCalibrateK2, TRUE);
                emit setmenu(emLoadCallibrateK2, TRUE);
                emit setmenu(emSaveCallibrateK2, TRUE);
                emit setmenu(emWattCallibrateK2, TRUE);
                emit setmenu(emWattEditSonde2, TRUE);
            }
        }
        //    QFont font("Helvetica", weichen.pfsize);
        emit setFontSize(currentdata.pfsize);
        fsendsweepdata();
        if ((stty != currentdata.str_tty) && dlgRes){
            picmodem->closetty();
            bttyOpen=false;
            bSweepIsRun = FALSE;
            ttyOpen(1);
        }
        if(currentdata.bsendalways or (adds_core_freq != currentdata.dds_core_freq) or (apll != currentdata.pll)){
          if (!bttyOpen || (fwversion ==0)){
                QMessageBox::warning( this, tr("Warning"),
                                    tr("Can't save DDS settings! Port is closed or the HW is not found"));
                currentdata.dds_core_freq = adds_core_freq;
                currentdata.pll = apll;
          } else {
                switch(currentdata.pll){
                  case 1: qpll="00"; break;
                  case 2: qpll="17"; break;
                  case 3: qpll="1F"; break;
                  case 4: qpll="27"; break;
                  case 5: qpll="2F"; break;
                  case 6: qpll="37"; break;
                  case 7: qpll="3F"; break;
                  case 8: qpll="47"; break;
                  case 9: qpll="4F"; break;
                  case 10: qpll="57"; break;
                  case 11: qpll="5F"; break;
                  case 12: qpll="67"; break;
                  case 13: qpll="6F"; break;
                  case 14: qpll="77"; break;
                  case 15: qpll="7F"; break;
                  case 16: qpll="87"; break;
                  case 17: qpll="8F"; break;
                  case 18: qpll="97"; break;
                  case 19: qpll="9F"; break;
                  case 20: qpll="A7"; break;
                }
                dkonst = 4294967296.0;  // 2 ^ 32
                ddskonst = dkonst * dkonst / currentdata.dds_core_freq ;
                test = qulonglong(ddskonst);
                qs = QString("%1").arg(test,0,16);
                if(qs.length() < 10)qs="0"+qs;
                #ifdef LDEBUG
                    qDebug() << QString("%1f").arg(qulonglong(currentdata.dds_core_freq));
                    qDebug() <<  QString("%1i").arg(test);
                    qDebug() << qs;
                #endif
                qs = qs.toUpper();
                qs = "e" + qs + qpll;

                if (bttyOpen)
                {
                    defaultClearTty(); //delete any data in the UART
                    picmodem->writeChar(143);
                    picmodem->writeLine(qs);
                    #ifdef CMDDEBUG
                        qDebug() << QString("%1 --> rs232").arg(qs);
                    #endif
                }
            }
        }
  }
  delete dlg;
  if (dlgRes){ //if Acept click
      if(this->currentIndex() == tab_nwt7vfo){ // VFO Tab activ
          this->tabAfterSwitch(tab_nwt7vfo);
      } else if(this->currentIndex() == tab_nwt7measure){ // Wattmeter VFO activ
          this->tabAfterSwitch(tab_nwt7measure);
      } else
      IdleTimerStart();
  }
}

void Nwt7MainClass::setWindowTitle(QString stty){
  QString qs;
  if(fwversion > 0){
    qs.sprintf("FW:%01i.%02i:V%02i",fwversion/100,fwversion-100,fwvariante);
  }else{
    qs = "FW:0.00";
  }
  qs += " - " + configfilename;
  qs += " - " + stty;
  if (bttyOpen) qs += ":OPENED"; else qs += ":CLOSED";
#ifdef TTYDEBUG
  qDebug() << qs;
#endif
  emit setAppText(qs);
}


void Nwt7MainClass::cleareVersionVariante()
{
    fwvariante = 0;
    fwportb = 0;
    fwsav = 0;
    verstep = TRUE;
}

void Nwt7MainClass::versionControl(){
  QString qs;
  #ifdef VERDEBUG
    qDebug(">>versioncontrol()");
  #endif
  if (!((fwversion > 113) and (fwversion < 150))) verstep = TRUE;
  if(verstep){
    if (bttyOpen)
    {
      #ifdef VERDEBUG
          qDebug("verstep = TRUE");
      #endif
      verCounterError++;
      if (verCounterError >10) {
          cleareVersionVariante();
          fwversion = 0;
          curTimerPerform = 0;
          verCounterError =0;
      }
      if (curTimerPerform != vtimer){ //some timer already was started
          stimer->stop();
          vtimer->stop();
          defaultClearTty(); //delete any data in the UART
          picmodem->writeChar(143);
          qs = "v";
          picmodem->writeLine(qs);
          vtimer->start(currentdata.vtime);
          curTimerPerform = vtimer;
          #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(qs);
          #endif            
      }
    }
  }else{  
    if (bttyOpen and (fwversion > 113) and (fwversion < 150))
    {
      #ifdef VERDEBUG
          qDebug("verstep = FALSE");
      #endif
      verCounterError++;
      if (verCounterError >10) {
          cleareVersionVariante();
          fwversion = 0;
          curTimerPerform = 0;
          verCounterError =0;
          verstep = TRUE;
      }
      if (curTimerPerform != stimer){ //some timer already was started
          defaultClearTty(); //delete any data in the UART
          picmodem->writeChar(143);
          qs = "s";
          vtimer->stop();
          stimer->stop();
          picmodem->writeLine(qs);
          stimer->start(currentdata.stime);
          curTimerPerform = stimer;
          #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(qs);
          #endif
      }
    } else verstep = TRUE;
  }
}

void Nwt7MainClass::version(){
  int a=0, i=0;
  QString qs;

  if (bttyOpen)
  {
    defaultClearTty(); //eventuelle Daten im UART loeschen
    picmodem->writeChar(143);
    QString s="v";
    picmodem->writeLine(s);
    #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(s);
    #endif
    while((picmodem->readttybuffer(&a, 1)) != 1){
      i++;
      if(i==20000){
        a=0;
      break;
      }
    };
    #ifdef VERDEBUG
        qDebug("version = %i",a);
    #endif    
    fwversion = a;
    qs = tr("<b>Checking the Firmware in the MCU :</b><br>The result must be more than 100<br><br>Version : ","");
    QString as;
    as.sprintf("%03i",a);
    QMessageBox::about(this, "Firmware", qs+as);
  }
}

void Nwt7MainClass::firmupdate(){
  char c;
  QFile f;
  int i;
  QString qs, qs1, zeile;
  QString beschr(tr("<br><b>Firmware Update procedure : </b> <br><br> 1)  Networktester OFF .<br>2) Place Jumper over GND and RB0. <br><br><B>Network Tester remains  OFF </b> <br><br> 3) Load the .HEX file for the PIC <br><br>  <B>More Info to follow !!!</b> <br>",""));
  QString beschr1(tr("<br><b>Firmware Update Procedure Continue :</b> <br><br> 1)  <b>After</b> pressing the OK - Button, switch the Network Tester ON. <br>2)  After pointing to- and entering the appropriate .HEX file,<br>... the update starts.  If not, switch Off the NWT and start<br> ...from scratch <br> <br><b>  After placing the jumper and Power-ON,  it is  necessary  to load new Firmware to the PIC ! </ b><br>",""));
  FirmwareDlg firmw(this);
 
  QMessageBox::about(this, tr("Firmware Update",""), beschr);
  QString s = QFileDialog::getOpenFileName(
              this,
               "Firmware Updaten",
              homedir.filePath("*.hex"),
              "Firmware Hexdatei (*.hex)",
              0,0);
  if (!s.isNull())
  {
    firmw.show();
    QMessageBox::about(this, tr("Firmware Update"), beschr1); 
    firmw.setText("HEX-Datei "+s);
    #ifdef LDEBUG
        qDebug() << s;
    #endif    
    f.setFileName(s);
    if(f.open( QIODevice::ReadOnly )){
      IdleTimerStop();
      QTextStream ts(&f);
      qs = "";
      while((qs.indexOf("HFM9") == -1)and(qs.indexOf("NWT7") == -1)){
        if(picmodem->readttybuffer(&c, 1) == 1){
          firmw.setText("Kennung RX"); 
          qs = qs + c;
          //qDebug(qs);
        }
      }
      qs = "";
      i = 0;
      while(qs.indexOf("OK") == -1){
        ts >> zeile;
        picmodem->writeLine(zeile);
        #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(zeile);
        #endif
        c = ' ';
        while(picmodem->readttybuffer(&c, 1) != 1) ;
        i++;
        qs1 = QString(tr("Line: %i")).arg(i);
        firmw.setText(qs1);
        if(c == 'O'){
          qs = qs + c;
          while(picmodem->readttybuffer(&c, 1) != 1) ;
        }
        qs = qs + c;
        #ifdef LDEBUG
            qDebug() << qs;
        #endif	
      }
      idletimer->start(currentdata.idletime);
    }
    QMessageBox::about(this, tr("Firmware Update",""), 
                             tr("<CENTER> <b> Update successful .</b> </CENTER> <BR> Remove the jumper !",""));
  }
}

void Nwt7MainClass::setAttenuationControlsIndex(int index)
{
  //qDebug("Nwt7linux::setDaempfungWob");
  cbAttenuator1->blockSignals(TRUE);
  cbAttenuator2->blockSignals(TRUE);
  cbAttenuator3->blockSignals(TRUE);
  cbAttenuator1->setCurrentIndex(index);
  cbAttenuator2->setCurrentIndex(index);
  cbAttenuator3->setCurrentIndex(index);

  setAttenuation(currentdata.attenuator.GetControlCodeByIndex(index));

  cbAttenuator1->blockSignals(FALSE);
  cbAttenuator2->blockSignals(FALSE);
  cbAttenuator3->blockSignals(FALSE);
}

void Nwt7MainClass::setAttenuationControls_dB(int attenuation_db)
{
    setAttenuationControlsIndex(currentdata.attenuator.GetIndex(attenuation_db));
}

//void Nwt7MainClass::setAttenuationSweep(int index)
//{
//  //qDebug("Nwt7linux::setDaempfungWob");
//  cbAttenuator2->blockSignals(TRUE);
//  cbAttenuator3->blockSignals(TRUE);
//  cbAttenuator3->setCurrentIndex(index);
//  cbAttenuator2->setCurrentIndex(index);
//  setAttenuation(index);
//  cbAttenuator2->blockSignals(FALSE);
//  cbAttenuator3->blockSignals(FALSE);
//}

//void Nwt7MainClass::setDaempfungVfo(int index)
//{
//  //qDebug("Nwt7linux::setDaempfungVfo");
//  cbAttenuator1->blockSignals(TRUE);
//  cbAttenuator3->blockSignals(TRUE);
//  cbAttenuator1->setCurrentIndex(index);
//  cbAttenuator3->setCurrentIndex(index);
//  setAttenuation(index);
//  cbAttenuator1->blockSignals(FALSE);
//  cbAttenuator3->blockSignals(FALSE);
//}

//void Nwt7MainClass::setDaempfungWatt(int index)
//{
//  //qDebug("Nwt7linux::setDaempfungWatt");
//    cbAttenuator1->blockSignals(TRUE);
//    cbAttenuator2->blockSignals(TRUE);
//    cbAttenuator1->setCurrentIndex(index);
//    cbAttenuator2->setCurrentIndex(index);
//    setAttenuation(index);
//    cbAttenuator1->blockSignals(FALSE);
//    cbAttenuator2->blockSignals(FALSE);
//}

void Nwt7MainClass::setAttenuation(int controlCode)
{
  bool bAttIdleTimer = FALSE;
  bool bAttStimer = FALSE;
  bool bAttVtimer = FALSE;
  bool bAttMessTimer = FALSE;

  //qDebug("Nwt7linux::setAttenuation");

  if(idletimer->timerId() != -1){
    idletimer->stop();
    bAttIdleTimer = TRUE;
  }
  if(stimer->timerId() != -1){
    stimer->stop();
    bAttStimer = TRUE;
  }
  if(vtimer->timerId() != -1){
    vtimer->stop();
    bAttVtimer = TRUE;
  }
  if(vmesstimer->timerId() != -1){
    vmesstimer->stop();
    bAttMessTimer = TRUE;
  }

  //qDebug("index  : %i", index);
  //qDebug("portb  : %x hex", portb);
  //qDebug("fwportb: %x hex", fwportb >> 1);
  //Sofort Systemwert richtig einstellen
  fwportb = controlCode;
  //it is not swept
  if(!bSweepIsRun){
    if(bttyOpen){
      picmodem->writeChar(0x8F);
      picmodem->writeChar('r');
      picmodem->writeChar(controlCode);
      #ifdef CMDDEBUG
        QString qs;
        qDebug() << qs.sprintf("r%i --> rs232", controlCode);
      #endif
    }
  }else{
    bSetDamping = TRUE;
  }
  //nwtDelay(100);
  if(bAttIdleTimer)idletimer->start(currentdata.idletime+250);
  if(bAttStimer)stimer->start(currentdata.stime);
  if(bAttVtimer)vtimer->start(currentdata.vtime);
  if(bAttMessTimer and restartMessure)
        vmesstimer->start(messtime);
}

void Nwt7MainClass::setwattoffset1(int index)
{
  //  qDebug("%i",index);
  switch(index){
  case 0: woffset1 = wattoffsetk1[0].offsetwatt; break;
  case 1: woffset1 = wattoffsetk1[1].offsetwatt; break;
  case 2: woffset1 = wattoffsetk1[2].offsetwatt; break;
  case 3: woffset1 = wattoffsetk1[3].offsetwatt; break;
  case 4: woffset1 = wattoffsetk1[4].offsetwatt; break;
  case 5: woffset1 = wattoffsetk1[5].offsetwatt; break;
  case 6: woffset1 = wattoffsetk1[6].offsetwatt; break;
  case 7: woffset1 = wattoffsetk1[7].offsetwatt; break;
  case 8: woffset1 = wattoffsetk1[8].offsetwatt; break;
  case 9: woffset1 = wattoffsetk1[9].offsetwatt; break;
  case 10: woffset1 = wattoffsetk1[10].offsetwatt; break;
  case 11: woffset1 = wattoffsetk1[11].offsetwatt; break;
  case 12: woffset1 = wattoffsetk1[12].offsetwatt; break;
  case 13: woffset1 = wattoffsetk1[13].offsetwatt; break;
  case 14: woffset1 = wattoffsetk1[14].offsetwatt; break;
  case 15: woffset1 = wattoffsetk1[15].offsetwatt; break;
  case 16: woffset1 = wattoffsetk1[16].offsetwatt; break;
  case 17: woffset1 = wattoffsetk1[17].offsetwatt; break;
  case 18: woffset1 = wattoffsetk1[18].offsetwatt; break;
  case 19: woffset1 = wattoffsetk1[19].offsetwatt; break;
  case 20: woffset1 = wattoffsetk1[20].offsetwatt; break;
  case 21: woffset1 = wattoffsetk1[21].offsetwatt; break;
  case 22: woffset1 = wattoffsetk1[22].offsetwatt; break;
  case 23: woffset1 = wattoffsetk1[23].offsetwatt; break;
  case 24: woffset1 = wattoffsetk1[24].offsetwatt; break;
  case 25: woffset1 = wattoffsetk1[25].offsetwatt; break;
  case 26: woffset1 = wattoffsetk1[26].offsetwatt; break;
  case 27: woffset1 = wattoffsetk1[27].offsetwatt; break;
  case 28: woffset1 = wattoffsetk1[28].offsetwatt; break;
  case 29: woffset1 = wattoffsetk1[29].offsetwatt; break;
  }
}

void Nwt7MainClass::setwattoffset2(int index)
{
  switch(index){
  case 0: woffset2 = wattoffsetk2[0].offsetwatt; break;
  case 1: woffset2 = wattoffsetk2[1].offsetwatt; break;
  case 2: woffset2 = wattoffsetk2[2].offsetwatt; break;
  case 3: woffset2 = wattoffsetk2[3].offsetwatt; break;
  case 4: woffset2 = wattoffsetk2[4].offsetwatt; break;
  case 5: woffset2 = wattoffsetk2[5].offsetwatt; break;
  case 6: woffset2 = wattoffsetk2[6].offsetwatt; break;
  case 7: woffset2 = wattoffsetk2[7].offsetwatt; break;
  case 8: woffset2 = wattoffsetk2[8].offsetwatt; break;
  case 9: woffset2 = wattoffsetk2[9].offsetwatt; break;
  case 10: woffset2 = wattoffsetk2[10].offsetwatt; break;
  case 11: woffset2 = wattoffsetk2[11].offsetwatt; break;
  case 12: woffset2 = wattoffsetk2[12].offsetwatt; break;
  case 13: woffset2 = wattoffsetk2[13].offsetwatt; break;
  case 14: woffset2 = wattoffsetk2[14].offsetwatt; break;
  case 15: woffset2 = wattoffsetk2[15].offsetwatt; break;
  case 16: woffset2 = wattoffsetk2[16].offsetwatt; break;
  case 17: woffset2 = wattoffsetk2[17].offsetwatt; break;
  case 18: woffset2 = wattoffsetk2[18].offsetwatt; break;
  case 19: woffset2 = wattoffsetk2[19].offsetwatt; break;
  case 20: woffset2 = wattoffsetk2[20].offsetwatt; break;
  case 21: woffset2 = wattoffsetk2[21].offsetwatt; break;
  case 22: woffset2 = wattoffsetk2[22].offsetwatt; break;
  case 23: woffset2 = wattoffsetk2[23].offsetwatt; break;
  case 24: woffset2 = wattoffsetk2[24].offsetwatt; break;
  case 25: woffset2 = wattoffsetk2[25].offsetwatt; break;
  case 26: woffset2 = wattoffsetk2[26].offsetwatt; break;
  case 27: woffset2 = wattoffsetk2[27].offsetwatt; break;
  case 28: woffset2 = wattoffsetk2[28].offsetwatt; break;
  case 29: woffset2 = wattoffsetk2[29].offsetwatt; break;
  }
}

void Nwt7MainClass::setProfil(int index)
{
  if(boxprofil->count() != 0){
    editanfang->setText(aprofil[index].fqstart);
    editende->setText(aprofil[index].fqend);
    editschritte->setText(aprofil[index].cntpoints);
  }
  SweepInit();
}

void Nwt7MainClass::setAufloesung(int index)
{
  int aufloesung=4;
  
  switch(index){
    case 0:
        aufloesung = 4;
        break;
    case 1:
        aufloesung = 1;
        break;
    case 2:
        aufloesung = 2;
        break;
    case 3:
        aufloesung = 3;
        break;
    case 4:
        aufloesung = 4;
        break;
    case 5:
        aufloesung = 5;
        break;
    case 6:
        aufloesung = 6;
        break;
    case 7:
        aufloesung = 7;
        break;
    case 8:
        aufloesung = 8;
        break;
    case 9:
        aufloesung = 9;
        break;
  }
  sweepdata.swraufloesung = aufloesung;
  fsendsweepdata();
}

void Nwt7MainClass::tip(bool atip)
{
QString tip_kalibrierkorrk1 = tr("<br><b>Activation of the Mathemetical calibration-correction<br> (Log Channel only) :</b><br><br>Calibration-correction means a Mathemetical correction of the Graph.<br>At calibration of the 0 dB line the Deviation pro Frequency is stored<br> in a File.<br>When displaying the Graph the trace is corrected by means of the Calibrationfile.<br><br><b> Beware this works with the Log. Channel only !!</b><br>","");
QString tip_grafik = Nwt7MainClass::tr("<br><b>Left mouse button:</b>.......Cursor setting.<br><b>Right mouse button:</b>.....Menu (only after the left mouse button) <br><br><b>Operation with the keyboard:</b> <br><br><em><b>Return</b></em>.....Cursor setting. <br><em><b>Space</b></em> ......SingleSweep with cursor being set, the active cursor<br>  ............... is the middle of theSweepWindow.<br><br><em><b>Up</b></em>......... Cursor in the middle and Magnifiying Glass +  Single Sweep <br><em><b>Down</b></em>..... Cursor in the middle and Magnifying Glass - Single Sweep <br><em><b>Left</b></em>........Cursor 5 Pixel left <br><em><b>Right</b></em>..... Cursor 5 Pixel right <br><br><em><b>1</b></em>..... Switch to Cursor 1, with Return  Set the Cursor<br><em><b>2</b></em>..... Switch to Cursor 2, with Return Set the Cursor<br><em><b>3</b></em>..... Switch to Cursor 3, with Return Set the cursor <br><em><b>4</b></em>..... Switch to Cursor 4, with Return Set the cursor <br><em><b>5</b></em>..... Switch to Cursor 5, with Return Set the Cursor <br><em><b>0</b></em>..... Delete all Cursors <br><br><em><b>E</b></em>..... Single-Sweep  (Like Button) <br><em><b>W</b></em>.....Continuous Sweep (like Button) <br><em><b>S</b></em> .....Stop (Like Button) <br>","");
QString tip_edit = Nwt7MainClass::tr("<b> Input: </b> <br> 3m5 or 3.5m or 3.5m = 3 500 000 Hz <br> <b> Input: </b> <br> 350k5 or 350,5k or 350.5k = 350 500 Hz","");
QString tip_k2 = Nwt7MainClass::tr("<br><b> External Probe : </b> <br><br>Connected to PIN7/RA5 of PIC .<br><br><b>Activation :</b> <br> in  \"Options\" :  \" Disable One Chann.\"<br>","");
QString tip_relais = Nwt7MainClass::tr("<br><b>Relaycontrol for attenuator : </b> <br><br>Relay-connections for an Attenuator  to Port B of the PIC. <br><br> <b> See : </b> Documentation","");
QString tip_zf = Nwt7MainClass::tr("<br><b>IF setting </b> <br> 0 = no IF <br> 4915000 = VFO + IF <br> -4915000 = VFO - IF<br>","");
QString tip_dial = Nwt7MainClass::tr(
    "<BR><b> Frequency Control :</b> <br> By the Arrow-keys or the Mouse- wheel.<br><br><b>"
    "The VFO  now can be set by the Keyboard also :</b><br><br>"
    "|__Q___|__W__|__E___|__R__|__T__|__Y___|__U___|__I___|<br>"
    "|_+10 __|__+1 _|_+100 _|_+10_ |__+1_|_+100_|__+10_|__+1__|<br>"
    "|_____ MHz___|________ kHz______|_________ Hz _______|<br><br>"
    "|__A___|__ S__|__D___|__F__|__G__|__H___|__J___|__K___|<br>"
    "|_ -10 __|__ -1 _|_ -100 _|_ -10_ |__ -1_|_ -100_|__ -10_|__ -1__|<br>"
    "|_____ MHz___|________ kHz______|_________ Hz _______|<br>","");
QString tip_checkmess = Nwt7MainClass::tr("<br><b>Power-attenuation :</b> <br> <br>This is used for a PowerLoad  with a measuring-output<br> of -10 to -40dB.<br>When Intermediate Values are used,  simply add value. <br> <br> All inputs can be deleted by the Menue \"Measurement\" <br> (Set Attenuation to Default).<br>","");
QString tip_mledit = Nwt7MainClass::tr("<b> Info window </b> <br> Is used for representation of different <br> text output for the <em> sweep. </em> <br> The first line serves as a description <br> when saving the curve. The first line <br> must begin with a semicolon <br> example. <br> <em> \"Bandpass filter 80m\" </em> <br> The DEFAULT label is always: <br> <em> \"no_label\" </em>","");
QString tip_betriebsart = Nwt7MainClass::tr("<b> Mode switch </b> <br> <u> Sweeping </u> Normal sweep <br> <u> SWR </u> SWR display channel 1 or 2 | <u> SWR_ant </u> determine old- SWR directly at the antenna <br> <u> Impedance </u> Display the impedance Z. Right ad with a series resistor 50 ohm <br> <u> Spectrum analyzer </u> Spectrum analyzer from the FA <u> Spec.FRQ - shift </u> SA. from the FA + Converter","");
QString tip_3db = Nwt7MainClass::tr("<br><b> Determination of the -3 dB points in a  Graph :</b><br><br>First the maximum is searched for.<br>Next  the -3 dB points related to that maximum are determined.<br>The results are  written to the Infoscreen.<br><br>From version 2.05 on, there is also an <b>inverse</b> Graph possible,<br> (Combobox checked). Used for Seriesresonant circuits. <br> The goal is the calculation of the Q (Quality) out of the -3 dB values. <br>","");
QString tip_6db = Nwt7MainClass::tr("<br><b> Determination of  the -6dB, -60dB points and Shapefactor of a Graph :</b> <br><br> First the maximum is searched for and next the -6dB and -60dB points related to that maximum are determined.<br> The results are written to the Infoscreen.<br><br>Only when the search was successful the Shapefactor is calculated.","");
QString tip_dbgrafik = Nwt7MainClass::tr("<b> Graphical display of the dB limits of a transmission curve </B> <BR> appear vertical dashed lines in the sweep curve. Upon activation of 3dB, 6dB / 60dB <br> the graphical representation is automatically active.","");
QString tip_schritte = Nwt7MainClass::tr("<b> Steps: </b> <br> The maximum number of measurement points is 9999. In order to come to a round <br> increment it might be a good step to enter a more <br> Enter (steps: 201). <br> The initial frequency is also already a measurement point. Thus, 200 steps result.","");
QString tip_pbar = Nwt7MainClass::tr("<br><b>Progress :</b> <br><br>Showing the Progress while sweeping .","");
QString tip_messtext = Nwt7MainClass::tr("<br><b>Textwindow for showing the  Measuringresults :</b><br><br>The Textwindow can  be fully edited and<br>can be stored as a Textfile.<br>","");
QString tip_messoffset = Nwt7MainClass::tr("<br><b>Editing the Correction per Frequency :</b> <br> <br>When the Measuringfrequency is known, here the<br> error of the Channel can be corrected.<br>In the NWT01 it  is built around the AD8307.<br><br>10 different corrections can be entered.<br>Therefore \"Edit Channelcalibration\"  is in the Menue<br> \"Measurement\".<br>","");
QString tip_swra_100 = Nwt7MainClass::tr("<br><b>Enter Cable-Attenuation :<br></b><br> Enter the attenuation in dB/100m, number only<br>","");
QString tip_swrlaenge = Nwt7MainClass::tr("<br><b>Enter the Cablelength :</b><br><br>Enter the Cablelength in Meters, numbers only<br>","");
QString tip_lupeplus = Nwt7MainClass::tr("<br><b> Zoom-in the Frequencyrange :</b> <br><br> Starting point is the cursor.<br>At the same time the approximate Center of the<br> Sweep-display is set.<br>The Frequencyrange will be halved and then a<br>  Single Sweep is started.<br>The  Frequency limits are  set in the Options.","");
QString tip_lupeminus = Nwt7MainClass::tr("<br><b> Zoom-out the Frequencyrange :</b><br><br> Starting point is the cursor.<br>At the same time the approximate center of<br>the Graphical Display is set.<br>The frequencyrange will be doubled and then<br>a Single Sweep  is started.<br>The Frequency limits are set in the Options.","");
QString tip_lupemitte = Nwt7MainClass::tr("<b> Increase frequency range </b> <BR> Starting point is the cursor. <br> Frequency range will increase and then <b> one </b> continuous sweep started. <br> Narrow frequency responses form the calibration frequency specified in the option.","");
QString tip_offset = Nwt7MainClass::tr("<br><b>Graphshift in Y-zone :</b><br><br>The graph can be shifted up and down by mousewheel<br>or by  selection.<br>All measure-results are adapted.<br> Y-text changes to color red.","");
QString tip_kursor = Nwt7MainClass::tr("<br><b>Selecting the Cursor number :</b><br><br>The Number of the cursor can be changed by the Mousewheel  or by Selection, Range 1-5.<br>Only the Selected Number is active.<br>","");
QString tip_zfwobbel = Nwt7MainClass::tr("<br><b>Sweeping with IF : </b> <br><br> This activates Sweeping with<br>Frequencyshift. <br><br><b> 4915000 = VFO + IF <br> -4915000 = VFO - IF</b><br>","");
QString tip_vfo_x4 = Nwt7MainClass::tr("<br><b> Feeding  I / Q mixers :</b><br><br>This activates VFO-frequency x 4 .<br>This way it is possible to feed SDR-radio's<br> like \"Softrock\".<br>The \"Maximum frequency\"  setting<br> changes accordingly. <br>","");
QString tip_messabs = Nwt7MainClass::tr("<br><b>Numeric Value A/D Converter :</b><br><br>This value is the average of several A/D<br> conversions.<br>","");
QString tip_displayshift = Nwt7MainClass::tr("<br><b>Y-axis text : </b><br><br> The Networktester- input can handle aprox.+10 dBm max.<br> So,  when  the output of a transmittert is measured,<br>an Attenuator must be inserted after the output.<br>(eg. a Powerload with -40 dB Measuring output).<br><br> When eg. 40 is entered, the 0dB-line shifts down and the<br> text is displayed correctly, 40 dB is added to all values.<br>The range upto included +50 dB is visible in this example.<br>","");
QString tip_scantime = Nwt7MainClass::tr("<br><b>Interrupt :</b><br><br>Selection of the Time between the samples.<br> The unit is uS.<br><br><b> This function can be used with appropriate<br> Firmware only.</b><br>","");
QString tip_profil = Nwt7MainClass::tr("<br><b>Profile :</b><br><br>With the Combobox pre-settings for Startfrequency, Stopfrequency and Stepnumber can be selected.<br>For Editing the profile there is a Menu on top .<br>","");
QString tip_invers = Nwt7MainClass::tr("<br><b>Determination of Inverse bandwidth :<br></b> <br> This switch can Switch On/Off the Inverse -3 dB bandwidth/ Q determination.<br><br>The inverse bandwidth is used eg. for Calculation<br> of the Q (Quality) of a Series-resonant circuit.<br>","");
QString tip_sastatus = Nwt7MainClass::tr("<br><b> Range or Status of  the spectrumanalyser :</b> <br><br> There are 2 options :<br> <br>1) Setting the Frequencyrange of the spectrumanalyser. <br><br>2) With the appropriate Firmware the Feedback from the spectrumanalyser<br>... can be activated  (in \"Options\").<br>...The Software reads the settings of the Spectrumanalyser and displays<br>... it here.<br>","");
QString tip_bfrq = Nwt7MainClass::tr("<br><b> Frequency Input/Copy :</b> <br><br> The Frequency can be selected in the Cursormenue <br> or entered directly . The Unit is MHz. <br>","");
QString tip_bl = Nwt7MainClass::tr("<br><b> Enter the Inductance :</b> <br><br> Enter the Inductance  in uH here .<br>The Capacitance in pF is calculated. <br>","");
QString tip_bc = Nwt7MainClass::tr("<br><b> Enter the Capacitance :</b> <br><br>Enter the Capacitance in pF here.<br>The Inductivity in uH is calculated. <br>","");
QString tip_bn = Nwt7MainClass::tr("<br><b>Enter the Number of turns :</b> <br><br> Enter te number of turns of the testcoil here.<br> The value AL=nH /( Turns square)  is calculated .<br>","");
QString tip_mhang = Nwt7MainClass::tr("<br><b> HANG-function on / off :</b> <br><br>  A \"Hang\" function  is implemented In the Wattmeter.<br>Only higher values are  displayed immediately.<br> When the value drops, 8 Measurements are checked.<br>If the value after 8 measurements still is lower then<br> the  value being displayed ,  the New value is displayed. <br>","");
QString tip_alw = Nwt7MainClass::tr("<br><b>Number of turns calculated from AL-value :</b><br><br>The AL value is automatically filled by the AL-calculation, or can be entered manually.<br> The Inductance can be entered manually only.<br>","");
QString tip_ymax = Nwt7MainClass::tr("<br><b>Scaling of the Y-axis<br>(Ymax upper Value in dB) :</b><br><br>In this Combobox the upper Value<br>of the Y-axis is set.<br>","");
QString tip_ymin = Nwt7MainClass::tr("<br><b>Scaling of the Y-axis <br>(Ymin lower Value in dB)</b><br><br>In this Combobox the lower Value<br>of the Y-axis is set.<br>","");
  
  btip = atip;
  
  wkmanager->tip(btip);
  if(btip){
    grafik->setToolTip(tip_grafik);
    editanfang->setToolTip(tip_edit);
    editende->setToolTip(tip_edit);
    editdisplay->setToolTip(tip_displayshift);
    checkboxk2->setToolTip(tip_k2);
    cbAttenuator1->setToolTip(tip_relais);
    cbAttenuator2->setToolTip(tip_relais);
    cbAttenuator3->setToolTip(tip_relais);
    editzf->setToolTip(tip_zf);
    editvfo->setToolTip(tip_edit);
    vsp1hz->setToolTip(tip_dial);
    vsp10hz->setToolTip(tip_dial);
    vsp100hz->setToolTip(tip_dial);
    vsp1khz->setToolTip(tip_dial);
    vsp10khz->setToolTip(tip_dial);
    vsp100khz->setToolTip(tip_dial);
    vsp1mhz->setToolTip(tip_dial);
    vsp10mhz->setToolTip(tip_dial);
    vsp100mhz->setToolTip(tip_dial);
    vsp1ghz->setToolTip(tip_dial);
    ldampingk1->setToolTip(tip_checkmess);
    ldaempfungk2->setToolTip(tip_checkmess);
    progressbaPowerMeter2->setToolTip(tip_k2);
    mledit->setToolTip(tip_mledit);
    cbOperatedMode->setToolTip(tip_betriebsart);
    checkbox3db->setToolTip(tip_3db);
    checkbox6db->setToolTip(tip_6db);
    checkboxgrafik->setToolTip(tip_dbgrafik);
    editschritte->setToolTip(tip_schritte);
    progressbar->setToolTip(tip_pbar);
    messedit->setToolTip(tip_messtext);
    edita_100->setToolTip(tip_swra_100);
    labela_100->setToolTip(tip_swra_100);
    editkabellaenge->setToolTip(tip_swrlaenge);
    labelkabellaenge->setToolTip(tip_swrlaenge);
    buttonlupeplus->setToolTip(tip_lupeplus);
    buttonlupeminus->setToolTip(tip_lupeminus);
    buttonlupemitte->setToolTip(tip_lupemitte);
    boxdbshift1->setToolTip(tip_offset);
    boxdbshift2->setToolTip(tip_offset);
    boxkursornr->setToolTip(tip_kursor);
    boxwattoffset1->setToolTip(tip_messoffset);
    boxwattoffset2->setToolTip(tip_messoffset);
    checkboxzf->setToolTip(tip_zfwobbel);
    checkboxiqvfo->setToolTip(tip_vfo_x4);
    mlabelk1->setToolTip(tip_messabs);
    mlabelk2->setToolTip(tip_messabs);
    boxzwischenzeit->setToolTip(tip_scantime);
    boxprofil->setToolTip(tip_profil);
    checkboxinvers->setToolTip(tip_invers);
    groupsa->setToolTip(tip_sastatus);
    editf1->setToolTip(tip_bfrq);
    editf2->setToolTip(tip_bfrq);
    editc1->setToolTip(tip_bc);
    editl2->setToolTip(tip_bl);
    editn->setToolTip(tip_bn);
    groupwindungen->setToolTip(tip_alw);
    checkboxmesshang1->setToolTip(tip_mhang); 
    checkboxmesshang2->setToolTip(tip_mhang);
    boxydbmax->setToolTip(tip_ymax); 
    boxydbmin->setToolTip(tip_ymin);
    kalibrier1->setToolTip(tip_kalibrierkorrk1); 
    kalibrier2->setToolTip(tip_kalibrierkorrk1); 
  }else{
    grafik->setToolTip("");
    editanfang->setToolTip("");
    editende->setToolTip("");
    editdisplay->setToolTip("");
    checkboxk2->setToolTip("");
    cbAttenuator1->setToolTip("");
    cbAttenuator2->setToolTip("");
    cbAttenuator3->setToolTip("");
    editzf->setToolTip("");
    editvfo->setToolTip("");
    vsp1hz->setToolTip("");
    vsp10hz->setToolTip("");
    vsp100hz->setToolTip("");
    vsp1khz->setToolTip("");
    vsp10khz->setToolTip("");
    vsp100khz->setToolTip("");
    vsp1mhz->setToolTip("");
    vsp10mhz->setToolTip("");
    vsp100mhz->setToolTip("");
    vsp1ghz->setToolTip("");
    ldampingk1->setToolTip("");
    ldaempfungk2->setToolTip("");
    progressbaPowerMeter2->setToolTip("");
    mledit->setToolTip("");
    cbOperatedMode->setToolTip("");
    checkbox3db->setToolTip("");
    checkbox6db->setToolTip("");
    checkboxgrafik->setToolTip("");
    editschritte->setToolTip("");
    progressbar->setToolTip("");
    messedit->setToolTip("");
    edita_100->setToolTip("");
    labela_100->setToolTip("");
    editkabellaenge->setToolTip("");
    labelkabellaenge->setToolTip("");
    buttonlupeplus->setToolTip("");
    buttonlupeminus->setToolTip("");
    buttonlupemitte->setToolTip("");
    boxdbshift1->setToolTip("");
    boxdbshift2->setToolTip("");
    boxkursornr->setToolTip("");
    boxwattoffset1->setToolTip("");
    boxwattoffset2->setToolTip("");
    checkboxzf->setToolTip("");
    checkboxiqvfo->setToolTip("");
    mlabelk1->setToolTip("");
    mlabelk2->setToolTip("");
    boxzwischenzeit->setToolTip("");
    boxprofil->setToolTip("");
    checkboxinvers->setToolTip("");
    groupsa->setToolTip("");
    editf1->setToolTip("");
    editf2->setToolTip("");
    editc1->setToolTip("");
    editl2->setToolTip("");
    editn->setToolTip("");
    groupwindungen->setToolTip("");
    checkboxmesshang1->setToolTip(""); 
    checkboxmesshang2->setToolTip(""); 
    boxydbmax->setToolTip(""); 
    boxydbmin->setToolTip(""); 
    kalibrier1->setToolTip(""); 
    kalibrier2->setToolTip(""); 
  }
}

void Nwt7MainClass::setColorRedk1(){
  QPalette pred;
  pred.setColor(QPalette::WindowText, Qt::red);
  checkboxk1->setPalette(pred);
}

void Nwt7MainClass::setColorBlackk1(){
  QPalette pbl;
  pbl.setColor(QPalette::WindowText, Qt::black);
  checkboxk1->setPalette(pbl);
}

void Nwt7MainClass::setColorRedk2(){
  QPalette pred;
  pred.setColor(QPalette::WindowText, Qt::red);
  checkboxk2->setPalette(pred);
}

void Nwt7MainClass::setColorBlackk2(){
  QPalette pbl;
  pbl.setColor(QPalette::WindowText, Qt::black);
  checkboxk2->setPalette(pbl);
}

void Nwt7MainClass::setDaempfungdefault(){
  ConfigFile nwt7configfile;
  QString qs;
  
  nwt7configfile.open(homedir.filePath(configfilename));
  while(nwt7configfile.clearItem("powerk")) ;
  nwt7configfile.close();
  while(ldampingk1->count() > 5)ldampingk1->removeItem(5) ;
  while(ldaempfungk2->count() > 5)ldaempfungk2->removeItem(5) ;
}

void Nwt7MainClass::fzaehlen(){
  QString qs;
  //unsigned long zeit, ergebnis;
  char abuffer[4];
  int a=0, i, j;

  //zeit = 624998;
  int zeit = 62499;

  if(!ttyOpen(1)) return;

  if (bttyOpen)
  {
    qs = QString("%l").arg(zeit,0,16);
    while(qs.length() < 8)qs = '0' + qs;
    //return;
    qs = "z" + qs;
    //qDebug(qs);
    defaultClearTty(); //eventuelle Daten im UART loeschen
    picmodem->writeChar(143);
    picmodem->writeLine(qs);
    #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(qs);
    #endif
    ttyindex = 0;
    do{
      a = picmodem->readttybuffer(abuffer, 4);
      //if(a>0)qDebug("%i",a);
      for(i=0, j=ttyindex; i<a; i++, j++){
        ttybuffer[j] = abuffer[i]; // Die gelesenen Daten in den richtigen Buffer kopieren
      }
      ttyindex = ttyindex + a; // und den Index auf die richtige Stelle setzen
    }while(ttyindex != 4);
    #ifdef LDEBUG
        long ergebnis = ttybuffer[0] + ttybuffer[1]*256 + ttybuffer[2]*256*256 + ttybuffer[3]*256*256*256;
        qDebug() << QString().sprintf("ergebnis= %li", ergebnis);
    #endif
  }
}

void Nwt7MainClass::keyPressEvent(QKeyEvent *event){
  if (this->currentIndex() == tab_nwt7vfo){
      double wert = getdrehgeber();
      switch(event->key()){
        case Qt::Key_Q: wert+=10000000.0; break; // +10 MHz
        case Qt::Key_A: wert-=10000000.0; break; // -10 MHz
        case Qt::Key_W: wert+=1000000.0; break; // +1 MHz
        case Qt::Key_S: wert-=1000000.0; break; // -1 MHz
        case Qt::Key_E: wert+=100000.0; break; // 100 kHz
        case Qt::Key_D: wert-=100000.0; break; // -100 kHz
        case Qt::Key_R: wert+=10000.0; break; // 10 kHz
        case Qt::Key_F: wert-=10000.0; break; // -10 kHz
        case Qt::Key_T: wert+=1000.0; break; // +1kHz
        case Qt::Key_G: wert-=1000.0; break; // -1kHz
        case Qt::Key_Y: wert+=100.0; break; // +100Hz
        case Qt::Key_H: wert-=100.0; break; //-100Hz
        case Qt::Key_U: wert+=10.0; break; // +10Hz
        case Qt::Key_J: wert-=10.0; break; //-10Hz
        case Qt::Key_I: wert+=1.0; break; //+1Hz
        case Qt::Key_K: wert-=1.0; break; //-1Hz
        default:QWidget::keyPressEvent(event);
      }
      if(wert<0.0)wert=0.0;
      setFqToSetPanel(wert);
  } else {
       QWidget::keyPressEvent(event);
  }

}

void Nwt7MainClass::openCurveManager(){
  #ifdef LDEBUG
  qDebug("Nwt7linux::openKurvenManager()");
  #endif    
  wkmanager->show();
}



void Nwt7MainClass::fsendsweepdata(){
  //Set the sweep limits, respect for the background curves
  sweepdata.absanfang = sweepdata.anfangsfrequenz;
  sweepdata.absende = sweepdata.anfangsfrequenz
                   + sweepdata.schrittfrequenz
                   * ((double)sweepdata.cntpoints -1);
  if(sweepdata.linear1 or sweepdata.linear2){
    sweepdata.bkalibrierkorrk1 = FALSE;
  }else{
    sweepdata.bkalibrierkorrk1 = currentdata.kalibrierk1;
  }
  //send signal
  emit sendsweepdata(sweepdata);
}

void Nwt7MainClass::readSettings()
{
  //Position vom Grafikfenster nachladen
  QSettings settings("AFU", "NWT");
  QPoint pos = settings.value("wpos", QPoint(0,0)).toPoint();
  QSize size = settings.value("wsize", QSize(700, 400)).toSize();
  grafik->resize(size);
  grafik->move(pos);
}

void Nwt7MainClass::writeSettings()
{
  //Position vom Grafikfenster abspeichern
  QSettings settings("AFU", "NWT");
  settings.setValue("wpos", grafik->pos());
  settings.setValue("wsize", grafik->size());
}

void Nwt7MainClass::setMessureVfoMode(bool messVfoMode){
    messVfoModeState = messVfoMode;
    IdleTimerStop();
    vmesstimer->stop();
    vfomesstimer->stop();
    vmesstimer->stop();
    if (rbVfoOn->isChecked()){
        messPanelState = FALSE;
        setVfoToOutOnce();
    } else if (rbMessVfoOn->isChecked()){
        //If enabled LCD Panel set VfoMess frequency
        messPanelState = TRUE;
        setMeasuredVfoFqValue();
    } else if (rbVFOSOff->isChecked()){
        messPanelState = FALSE;
        setDdsOff();
        vmesstimer->start(messtime);
    }
    vfoOnOffControls();
}

void Nwt7MainClass::MessPanelToggle()
{
  if(messPanelState){
    messPanelState = FALSE;
  }else{
    messPanelState = TRUE;
  }
  vfoOnOffControls();
}

void Nwt7MainClass::vfoOnOffControls()
{
  if(messPanelState){
    labelsphz->show();
    labelspkhz->show();
    labelspmhz->show();
    sp1hz->show();
    sp10hz->show();
    sp100hz->show();
    sp1khz->show();
    sp10khz->show();
    sp100khz->show();
    sp1mhz->show();
    sp10mhz->show();
    sp100mhz->show();
    sp1ghz->show();
    cbAttenuator3->show();
    labeldaempfung3->show();
    messedit->hide();
    buttonmess->hide();
    buttonmesssave->hide();
    
  }else{
    labelsphz->hide();
    labelspkhz->hide();
    labelspmhz->hide();
    sp1hz->hide();
    sp10hz->hide();
    sp100hz->hide();
    sp1khz->hide();
    sp10khz->hide();
    sp100khz->hide();
    sp1mhz->hide();
    sp10mhz->hide();
    sp100mhz->hide();
    sp1ghz->hide();
    cbAttenuator3->hide();
    labeldaempfung3->hide();
    messedit->show();
    buttonmess->show();
    buttonmesssave->show();
  }
}

void Nwt7MainClass::setMeasuredVfoFqValue()
{
  int i;
  double fr;
    
  //qDebug("setmessvfo()");
  if(messedit->isHidden()){
    if(sp1hz->value()==10){
      sp1hz->setValue(0);
      i = sp10hz->value();
      i++;
      sp10hz->setValue(i);
    }
    if(sp10hz->value()==10){
      sp10hz->setValue(0);
      i = sp100hz->value();
      i++;
      sp100hz->setValue(i);
    }
    if(sp100hz->value()==10){
      sp100hz->setValue(0);
      i = sp1khz->value();
      i++;
      sp1khz->setValue(i);
    }
    if(sp1khz->value()==10){
      sp1khz->setValue(0);
      i = sp10khz->value();
      i++;
      sp10khz->setValue(i);
    }
    if(sp10khz->value()==10){
      sp10khz->setValue(0);
      i = sp100khz->value();
      i++;
      sp100khz->setValue(i);
    }
    if(sp100khz->value()==10){
      sp100khz->setValue(0);
      i = sp1mhz->value();
      i++;
      sp1mhz->setValue(i);
    }
    if(sp1mhz->value()==10){
      sp1mhz->setValue(0);
      i = sp10mhz->value();
      i++;
      sp10mhz->setValue(i);
    }
    if(sp10mhz->value()==10){
      sp10mhz->setValue(0);
      i = sp100mhz->value();
      i++;
      sp100mhz->setValue(i);
    }
    if(sp100mhz->value()==10){
      sp100mhz->setValue(0);
      i = sp1ghz->value();
      i++;
      sp1ghz->setValue(i);
    }
    fr = double(sp1hz->value()) + 
         double(sp10hz->value())   * 10.0 +
         double(sp100hz->value())  * 100.0 +
         double(sp1khz->value())   * 1000.0 +
         double(sp10khz->value())  * 10000.0 +
         double(sp100khz->value()) * 100000.0 +
         double(sp1mhz->value())   * 1000000.0 +
         double(sp10mhz->value())  * 10000000.0 +
         double(sp100mhz->value()) * 100000000.0 +
         double(sp1ghz->value())   * 1000000000.0;
    if(fr < 0.0){
      if(sp1hz->value()==-1)sp1hz->setValue(0);
      if(sp10hz->value()==-1)sp10hz->setValue(0);
      if(sp100hz->value()==-1)sp100hz->setValue(0);
      if(sp1khz->value()==-1)sp1khz->setValue(0);
      if(sp10khz->value()==-1)sp10khz->setValue(0);
      if(sp100khz->value()==-1)sp100khz->setValue(0);
      if(sp1mhz->value()==-1)sp1mhz->setValue(0);
      if(sp10mhz->value()==-1)sp10mhz->setValue(0);
      if(sp100mhz->value()==-1)sp100mhz->setValue(0);
      if(sp1ghz->value()==-1)sp1ghz->setValue(0);
    }
    if(sp1hz->value()==-1){
      sp1hz->setValue(9);
      i = sp10hz->value();
      i--;
      sp10hz->setValue(i);
    }
    if(sp10hz->value()==-1){
      sp10hz->setValue(9);
      i = sp100hz->value();
      i--;
      sp100hz->setValue(i);
    }
    if(sp100hz->value()==-1){
      sp100hz->setValue(9);
      i = sp1khz->value();
      i--;
      sp1khz->setValue(i);
    }
    if(sp1khz->value()==-1){
      sp1khz->setValue(9);
      i = sp10khz->value();
      i--;
      sp10khz->setValue(i);
    }
    if(sp10khz->value()==-1){
      sp10khz->setValue(9);
      i = sp100khz->value();
      i--;
      sp100khz->setValue(i);
    }
    if(sp100khz->value()==-1){
      sp100khz->setValue(9);
      i = sp1mhz->value();
      i--;
      sp1mhz->setValue(i);
    }
    if(sp1mhz->value()==-1){
      sp1mhz->setValue(9);
      i = sp10mhz->value();
      i--;
      sp10mhz->setValue(i);
    }
    if(sp10mhz->value()==-1){
      sp10mhz->setValue(9);
      i = sp100mhz->value();
      i--;
      sp100mhz->setValue(i);
    }
    if(sp100mhz->value()==-1){
      sp100mhz->setValue(9);
      i = sp1ghz->value();
      i--;
      sp1ghz->setValue(i);
    }
    if(sp1ghz->value()==-1){
      sp1ghz->setValue(0);
    }
  }
  //If enabled LCD Panel set VfoMess frequency
  if (!appstart){
      IdleTimerStop();
      vfomesstimer->start(10);
  }
}

void Nwt7MainClass::VfoMessTimerPerform()
{
#ifdef TIMERDEBUG
    qDebug("Nwt7linux::VfoMessTimerPerform()");
#endif
  if(!bersterstart){
    QString qs;
    QString s;
    double fr;
    //after expiration of the timer frequency is sent to the NWT
    if(!bttyOpen){
        vmesstimer->stop();//Sobol A.E.
        idletimer->start(currentdata.idletime);
        setInitInterface(bttyOpen);
        #ifdef TIMERDEBUG
        qDebug("Nwt7linux::VfoMessTimerPerform() tty off");
        #endif
        return;
    }
    if (bttyOpen)
    {
      fr = double(sp1hz->value()) + 
           double(sp10hz->value()) *   10.0 +
           double(sp100hz->value()) *  100.0 +
           double(sp1khz->value()) *   1000.0 +
           double(sp10khz->value()) *  10000.0 +
           double(sp100khz->value()) * 100000.0 +
           double(sp1mhz->value()) *   1000000.0 +
           double(sp10mhz->value()) *  10000000.0 +
           double(sp100mhz->value()) * 100000000.0 +
           double(sp1ghz->value()) *   1000000000.0;
      //Frequency factor apply
      fr = qRound(fr / double(sweepdata.freq_faktor));
      if((fr < 1000000000.0) and fr >= 0.0){
        s = QString().sprintf("f%09.0f",fr);
        //qDebug(s);
        picmodem->writeChar(143);
        picmodem->writeLine(s);
        #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(s);
        #endif
      }
    }
  }
  //timer stop because task is completed
  vfomesstimer->stop();
  vmesstimer->start(messtime);
}

void Nwt7MainClass::setDisplayYmax(const QString &sa)
{
  bool ok;
  int ydbmax;
  QString s = sa;
  //  qDebug(s);
  ydbmax = s.toInt(&ok);
  if(ok){
    sweepdata.ydbmax = ydbmax;
    fsendsweepdata();
//    emit sendewobbeldaten(wobdaten);
  }
}

void Nwt7MainClass::setDisplayYmin(const QString &sa)
{
  bool ok;
  int ydbmin;
  QString s = sa;
  //  qDebug(s);
  ydbmin = s.toInt(&ok);
  if(ok){
    sweepdata.ydbmin = ydbmin;
    fsendsweepdata();
//    emit sendewobbeldaten(wobdaten);
  }
}

/*
void Nwt7linux::setkdir()
{
  wkmanager->setkdir(kurvendir.absolutePath());
}

void Nwt7linux::getkdir(QString s)
{
  kurvendir.setPath(s);
}

void Nwt7linux::warnkalibrierkorr()
{
  if(wobdaten.bkalibrierkorrk1 or wobdaten.bkalibrierkorrk2){
      
    if(wgrunddaten.bwarnekorr){
      //keine Warnung bei SA
      if((wobdaten.ebetriebsart != espektrumanalyser) and (wobdaten.ebetriebsart != espekdisplayshift)){
        if( QMessageBox::warning( this, 
            tr("Grafische Darstellung","InformationsBox"),
            tr("<B>Kurvendatei Darstellung</B><BR>"
                "Durch die aktiverte mathematische Kalibrierkorrektur "
                "weicht die Darstellung der Kurvendatei etwas ab. Auf "
                "der Y-Achse entstehen Differenzen zwischen der "
                "realen Wobbelkurven und den Kurvendateinen.<br><br>"
                "<B>Zur genauen Darstellung ist es notwendig "
                "die mathematische Kalibrierkorrektur zu deaktivieren!</B>" ,"WarnungsBox"),
                tr("Weiter", "WarnungsBox"),
                tr("Deaktivieren", "WarnungsBox")
                ,0,1) == 1)
        {
          wobdaten.bkalibrierkorrk1 = FALSE;
          wobdaten.bkalibrierkorrk2 = FALSE;
          emit sendewobbeldaten(wobdaten);
        }
      }
    }
      //qDebug(Nwt7linux::warnkalibrierkorr()");
    if(wobdaten.bkalibrierkorrk1){
      setColorRedk1();
    }else{
      setColorBlackk1();
    }
    if(wobdaten.bkalibrierkorrk2){
      setColorRedk2();
    }else{
      setColorBlackk2();
    }
  }
}
*/

void Nwt7MainClass::kalibrier1_checked(bool b){
  currentdata.kalibrierk1 = b;
  sweepdata.bkalibrierkorrk1 = b;
  if(currentdata.kalibrierk1 and !sweepdata.linear1){
    setColorRedk1();
  }else{
    setColorBlackk1();
  }
  if(currentdata.kalibrierk2 and !sweepdata.linear2){
    setColorRedk2();
  }else{
    setColorBlackk2();
  }
  emit sendsweepdata(sweepdata);
}

void Nwt7MainClass::kalibrier2_checked(bool b){
  currentdata.kalibrierk2 = b;
  sweepdata.bkalibrierkorrk2 = b;
  if(currentdata.kalibrierk1 and !sweepdata.linear1){
    setColorRedk1();
  }else{
    setColorBlackk1();
  }
  if(currentdata.kalibrierk2 and !sweepdata.linear2){
    setColorRedk2();
  }else{
    setColorBlackk2();
  }
  emit sendsweepdata(sweepdata);
}

void Nwt7MainClass::setProgramPath(const QString &ap){
  //qDebug("Nwt7linux::setProgramPath(QString s)");
  programpath = ap;
  //qDebug(programpath);
  homedir.setPath(programpath);
  //der Programname ist noch mit im Pfad: deshalb cdUp
  homedir.cdUp();
  //QString s = homedir.absolutePath();
  //qDebug(s);
  bersterstart = FALSE;
  //Test ob im Progrannverzeichnis das Verzeichnis nwt befindet
  if(!homedir.cd("nwt")){
    //KEIN nwt Verzeichnis: HOME ist das neue Verzeichnis
    homedir = QDir::home();
    if(!homedir.cd("WinNWT5")){
      //Befindet sich im HOME kein WinNWT5-Verzeichnis
      //WinNWT5 erzeugen
      homedir.mkdir("WinNWT5");
      //in das WinNWT5 gehen
      homedir.cd("WinNWT5");
      //das ist der erste Start des Programmes
      bersterstart = TRUE;
    }
  }
  QDir dir=homedir;
  dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Size | QDir::Reversed);

  /*
  QFileInfoList list = dir.entryInfoList();
  std::cout << "     Bytes Filename" << std::endl;
  for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo = list.at(i);
      std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
                                              .arg(fileInfo.fileName()));
      std::cout << std::endl;
  }
  */
  //s = homedir.absolutePath();
  //qDebug(s);
}

void Nwt7MainClass::setFrqFaktorLabel(){
  QString qs, s;
  QPalette pred;
  QPalette pbl;
  pred.setColor(QPalette::WindowText, Qt::red);
  pbl.setColor(QPalette::WindowText, Qt::black);
  
  s = tr("Frequency multiplication");
  qs.sprintf(" x %i", sweepdata.freq_faktor);
  qs = s + qs;
  labelfrqfaktor->setText(qs);
  labelfrqfaktorv->setText(qs);
  labelfrqfaktorm->setText(qs);
  if(sweepdata.freq_faktor > 1){
    labelfrqfaktor->setPalette(pred);
    labelfrqfaktorv->setPalette(pred);
    labelfrqfaktorm->setPalette(pred);
    labelfrqfaktor->show();
    labelfrqfaktorv->show();
    labelfrqfaktorm->show();
  }else{
    labelfrqfaktor->setPalette(pbl);
    labelfrqfaktorv->setPalette(pbl);
    labelfrqfaktorm->setPalette(pbl);
    labelfrqfaktor->hide();
    labelfrqfaktorv->hide();
    labelfrqfaktorm->hide();
  }
}

void Nwt7MainClass::nkurve_loeschen(){
  sweepInvalid();
  fsendsweepdata();
}

void Nwt7MainClass::zrausrechnen(){
  double r1;
  double r2;
  double r3;
  double r4;
  double daempf;
  double z1;
  double z2;
  double z3;
  double z4;
  double d1;
  double d2;
  QString qs;
  bool ok;
  
  qs = editz1->text();
  z1 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editz2->text();
  z2 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editz3->text();
  z3 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editz4->text();
  z4 = qs.toDouble(&ok);
  if(!ok)return;
  r1 = z1/(sqrt(1.0 - (z1/z2)));
  r2 = z2*sqrt(1.0 - (z1/z2));
  r3 = z3*sqrt(1.0 - (z4/z3));
  r4 = z4/(sqrt(1.0 - (z4/z3)));
  
  qs.sprintf("%3.2f",r1);
  lr1->setText(qs);
  qs.sprintf("%3.2f",r2);
  lr2->setText(qs);
  qs.sprintf("%3.2f",r3);
  lr3->setText(qs);
  qs.sprintf("%3.2f",r4);
  lr4->setText(qs);
  
  d1 = 10.0 * log10(((r1*z1)/(r1+z1))/(2.0*r2+(r1*z1/(r1+z1))));
  //qDebug("%f",d1);
  d2 = 10.0 * log10(((r4*z4)/(r4+z4))/(2.0*r3+(r4*z4/(r4+z4))));
  //qDebug("%f",d2);
  
  daempf = d1+d2;
  qs.sprintf("%3.2f",daempf);
  ldaempfung->setText(qs);
}

void Nwt7MainClass::zlcausrechnen(){
  double xl;
  double xc;
  double l;
  double c;
  double f;
  double z1;
  double z2;
  QString qs;
  bool ok;
  
  qs = editzlc1->text();
  z1 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editzlc2->text();
  z2 = qs.toDouble(&ok);
  if(!ok)return;
  qs = editzlc3->text();
  f = qs.toDouble(&ok);
  if(!ok)return;
  f = f*1000000.0;
  
  xl = sqrt(z1*z2-z1*z1);
  xc = (z1*z2)/xl;
  
  //qDebug("xl %f",xl);
  //qDebug("xc %f",xc);
  
  l = (xl/(2.0*3.1415*f))*1000000.0;
  c = (1.0/(2.0*3.1415*f*xc))*1000000000000.0;
  
  //qDebug("l %f",l);
  //qDebug("c %f",c);

  qs.sprintf("%3.2f",l);
  ll->setText(qs);
  qs.sprintf("%3.2f",c);
  lc->setText(qs);
}

void Nwt7MainClass::setimp(){
  if(rbr->isChecked()){
    gzr->show();
    bild1->show();
    gzlc->hide();
    bild2->hide();
  }else{
    gzr->hide();
    bild1->hide();
    gzlc->show();
    bild2->show();
  }
} 

void Nwt7MainClass::testLogLin(){
  if(sweepdata.linear2 or sweepdata.linear1){
    setColorBlackk1();
    setColorBlackk2();
    checkboxgrafik->setEnabled(TRUE);
    checkbox6db->setEnabled(TRUE);
    checkbox3db->setEnabled(TRUE);
    checkboxinvers->setEnabled(TRUE);
    cbOperatedMode->setEnabled(FALSE);
    labelbetriebsart->setEnabled(FALSE);
    labelboxdbshift1->setEnabled(FALSE);
    labelboxdbshift2->setEnabled(FALSE);
    boxdbshift1->setEnabled(FALSE);
    boxdbshift2->setEnabled(FALSE);
    kalibrier1->setEnabled(FALSE);
    kalibrier2->setEnabled(FALSE);
    boxydbmax->setEnabled(FALSE);
    boxydbmin->setEnabled(FALSE);
    labelboxydbmax->setEnabled(FALSE);
    labelboxydbmin->setEnabled(FALSE);
    editdisplay->setEnabled(FALSE);
    labeldisplayshift->setEnabled(FALSE);
  }else{
    if(kalibrier1->isChecked())setColorRedk1();
    if(kalibrier2->isChecked())setColorRedk2();
    checkboxgrafik->setEnabled(TRUE);
    checkbox6db->setEnabled(TRUE);
    checkbox3db->setEnabled(TRUE);
    checkboxinvers->setEnabled(TRUE);
    cbOperatedMode->setEnabled(TRUE);
    labelbetriebsart->setEnabled(TRUE);
    labelboxdbshift1->setEnabled(TRUE);
    labelboxdbshift2->setEnabled(TRUE);
    boxdbshift1->setEnabled(TRUE);
    boxdbshift2->setEnabled(TRUE);
    kalibrier1->setEnabled(TRUE);
    kalibrier2->setEnabled(TRUE);
    boxydbmax->setEnabled(TRUE);
    boxydbmin->setEnabled(TRUE);
    labelboxydbmax->setEnabled(TRUE);
    labelboxydbmin->setEnabled(TRUE);
    editdisplay->setEnabled(TRUE);
    labeldisplayshift->setEnabled(TRUE);
  }
}

void Nwt7MainClass::diagrammstart(){
  bstop->setEnabled(true);
  bstart->setEnabled(false);
  bsave->setEnabled(false);
  IdleTimerStop();
  for(int i=0; i<1024;i++){
    sweepdata.mess.k1[i]=0;
  }
  defaultClearTty();
  sweepdata.mess.daten_enable = TRUE;
  emit sendsweepdata(sweepdata);
  picmodem->writeChar(143);
  QString s = "m";
  picmodem->writeLine(s);
    #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(s);
    #endif
  tdiagramm->start(10);
  dummycounter = 50;
  counterkurve = 0;
}

void Nwt7MainClass::diagrammstop(){
  bstart->setEnabled(true);
  bsave->setEnabled(true);
  bstop->setEnabled(false);
  tdiagramm->stop();
  idletimer->start(currentdata.idletime);
  sweepdata.mess.daten_enable = TRUE;
  emit sendsweepcurve(sweepdata.mess);
}

void Nwt7MainClass::diagrammspeichern(){
  QString qs;
  QFile f;
  double w;

  QString s = QFileDialog::getSaveFileName(this,
                                           tr("Save NWT Graph",""),
                                           kurvendir.filePath(""),
                                           tr("CSV file (* .csv)","")
                                           );
  if (!s.isNull())
  {
    kurvendir.setPath(s);
    qs = kurvendir.absolutePath();
    //qDebug(qs);
    //Datei ueberpruefen ob Sufix vorhanden
    if((s.indexOf(".")== -1)) s += ".csv";
    f.setFileName(s);
    if(f.open( QIODevice::WriteOnly )){
      QTextStream ts(&f);
      ts << "index; Volt; dBm" << "\r\n" << flush;
      for(int i=0; i<1024; i++){
        if(sweepdata.mess.k1[i] != 0){
          w = sweepdata.mfaktor1 * sweepdata.mess.k1[i] + sweepdata.mshift1;
          qs.sprintf("%i; %f; %f\r\n", i, 5.0 / 1024 * (double) i, w);
          qs.replace('.',',');
          ts << qs;
        }
      }
    }
  }
}

void Nwt7MainClass::tdiagrammbehandlung(){
  unsigned char abuffer[10];
  unsigned char c1=0, c2=0;
  int i1, i2, a;
  QString qs, qsline;
  double w;


  if (bttyOpen){
    a = picmodem->readttybuffer(abuffer, 10);
    //qDebug("a = %i",a);
    if(a != 4){
      messsync++;
      picmodem->writeChar(143);
      QString s="m";
      picmodem->writeLine(s);
#ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(s);
#endif
      tdiagramm->start(10);
      if(messsync > 100){
        diagrammstop();
        QMessageBox::warning( this, tr("Antenna`s chart","InformationsBox"),
                                    tr("Can't Receive data from NWT !",""));
        setCurrentIndex(0);
        return;
      }
    }else{
      messsync=0;
      c1 = abuffer[0];
      c2 = abuffer[1];
      i1 = int(c1 + c2 * 256);
      c1 = abuffer[2];
      c2 = abuffer[3];
      i2 = int(c1 + c2 * 256);
      if(i2 < 1024){
        if(dummycounter == 0){
          sweepdata.mess.k1[i2]=i1;
          w = sweepdata.mfaktor1 * i1 + sweepdata.mshift1;
          qs.sprintf("idx:%i, %0.2fV, %0.2f dBm", i2, 5.0/1024 * (double)i2, w);
        }else{
          dummycounter--;
          qs = "Warten!";
        }
      }
      lanzeige->setText(qs);
      picmodem->writeChar(143);
      QString s="m";
      picmodem->writeLine(s);
    #ifdef CMDDEBUG
            qDebug() << QString("%1 --> rs232").arg(s);
    #endif
      tdiagramm->start(10);
      counterkurve++;
      if(counterkurve==50){
        emit sendsweepcurve(sweepdata.mess);
        counterkurve = 0;
      }
    }
  }
}

void Nwt7MainClass::diagrammsimulieren(){
  //initialisieren
  int w;
  w = 250;
  for(int i=sweepdata.antdiagramm.adbegin; i<=sweepdata.antdiagramm.adend; i++){
    if(i % 4 == 0){
      if(i<512){w++;}else{w--;}
    }
    sweepdata.mess.k1[i] = w;
  }
  sweepdata.mess.daten_enable = TRUE;
  emit sendsweepcurve(sweepdata.mess);
}

void Nwt7MainClass::diagrammdim(int){
  int idmin = boxdbmin->currentIndex();
  int idmax = boxdbmax->currentIndex();
  switch(idmin){
  case 0: sweepdata.antdiagramm.dbmin = 10; break;
  case 1: sweepdata.antdiagramm.dbmin = 0; break;
  case 2: sweepdata.antdiagramm.dbmin = -10; break;
  case 3: sweepdata.antdiagramm.dbmin = -20; break;
  case 4: sweepdata.antdiagramm.dbmin = -30; break;
  case 5: sweepdata.antdiagramm.dbmin = -40; break;
  case 6: sweepdata.antdiagramm.dbmin = -50; break;
  case 7: sweepdata.antdiagramm.dbmin = -60; break;
  case 8: sweepdata.antdiagramm.dbmin = -70; break;
  case 9: sweepdata.antdiagramm.dbmin = -80; break;
  }
  switch(idmax){
  case 0: sweepdata.antdiagramm.dbmax = 10; break;
  case 1: sweepdata.antdiagramm.dbmax = 0; break;
  case 2: sweepdata.antdiagramm.dbmax = -10; break;
  case 3: sweepdata.antdiagramm.dbmax = -20; break;
  case 4: sweepdata.antdiagramm.dbmax = -30; break;
  case 5: sweepdata.antdiagramm.dbmax = -40; break;
  case 6: sweepdata.antdiagramm.dbmax = -50; break;
  case 7: sweepdata.antdiagramm.dbmax = -60; break;
  case 8: sweepdata.antdiagramm.dbmax = -70; break;
  case 9: sweepdata.antdiagramm.dbmax = -80; break;
  }
  sweepdata.antdiagramm.adbegin = int(qRound(spinadbegin->value()*1023.0/5.0));
  //qDebug()<<"qRound(spinadbegin->value()*1023.0/5.0: "<<qRound(spinadbegin->value()*1023.0/5.0);
  //qDebug()<<wobdaten.antdiagramm.adbegin;
  sweepdata.antdiagramm.adend = int(qRound(spinadend->value()*1023.0/5.0));
  //qDebug()<<"qRound(spinadend->value()*1023.0/5.0: "<<qRound(spinadend->value()*1023.0/5.0);
  //qDebug()<<wobdaten.antdiagramm.adend;
  sweepdata.antdiagramm.diabegin = spingradbegin->value();
  //qDebug()<<wobdaten.antdiagramm.diabegin;
  sweepdata.antdiagramm.diaend = spingradend->value();
  //qDebug()<<wobdaten.antdiagramm.diaend;

  emit sendsweepdata(sweepdata);
}

void Nwt7MainClass::diagrammdimd(double){
  diagrammdim(0);
}

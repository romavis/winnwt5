/***************************************************************************

    MainWindow.cpp  -  description
    ----------------------------------------------------
    begin                : 2007
    copyright            : (C) 2007 by Andreas Lindenau
    email                : DL4JAL@darc.de

    MainWindow.cpp  -  description
    ----------------------------------------------------
    begin                : 2015
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

/*#include <QtGui>*/
#include <QObject>
#include <QLocale>
#include <QMenuBar>
#include <QToolBar>
#include <QStatusBar>
#include <QMessageBox>

#include "mainwindow.h"
#include "build.h"

MainWindow::MainWindow()
{
  QLocale::setDefault(QLocale::German);
///////////////////////////////////////////////////////////////////////////////
// Info Version usw. 

  infoversion = "5.1.0";
  infobuildnum = QString::number(BUILDNO);
  QString infobuildate = QDate::currentDate().toString("dd/MM/yyyy");
  infotext = "<h2>WinNWT5 and LinNWT5</h2>"
             "<p><B>Version:"+infoversion+"</B>    build:"+infobuildnum+"</p>"
             "Developed under Windows QT5.3.2</p>"
             "<p>WinNWT4 (c) Andreas Lindenau DL4JAL (2002-2013)<br>"
             "DL4JAL@darc.de<br>"
             "<a href=\"http://www.dl4jal.eu\">www.dl4jal.eu</a></p>"
             "<p>WinNWT5 (c) Andrey Sobol UB3TAF (2015-2017)<br>"
             "andrey.sobol.nn@gmail.com<br>"
             "<a href=\"http://www.asobol.ru/software/winnwt5\">www.asobol.ru</a></p>";
  nwtMainObj = new Nwt7MainClass(this);
  setCentralWidget(nwtMainObj);

  connect(nwtMainObj, SIGNAL(setAppText(QString)), this, SLOT(setText(QString)));
  //Font size of the program set
  connect(nwtMainObj, SIGNAL(setFontSize(int)), this, SLOT(setFontSize(int)));

  createActions();
  createToolBars();
  createStatusBar();
  createMenus();
#ifndef DEBUG_INTERFACE
  connect( nwtMainObj, SIGNAL(setmenu(emenuenable,bool)), this,SLOT(mSetMenu(emenuenable,bool)));
#endif
  setWindowIcon(QIcon(":/images/appicon.png"));
  
  setMaximumSize (2000, 2000);
//  setMinimumSize (750, 480);
  setMinimumSize (500, 480);
  menuCurves->setEnabled ( true );
  menuSweep->setEnabled ( true );
  mStop->setEnabled ( false );
//  menuvfo->setEnabled ( false );
  menuMeasure->setEnabled ( true );
  mPrint->setEnabled ( true );
  readSettings();
  createToolTip(mTooltip->isChecked());
  nwtMainObj->resizeWidgets();
//  nwtMainObj->menuFirsSet();
}

void MainWindow::setFontSize(int s)
{
  QFont font("Helvetica", s);
  this->setFont(font);
  nwtMainObj->setFont(font);
}

void MainWindow::closeEvent(QCloseEvent *event)
{
  writeSettings();
  nwtMainObj->beenden();
  delete nwtMainObj;
  event->accept();
}


void MainWindow::createMenus()
{
  menuFile = menuBar()->addMenu(tr("&File","Menue"));
  menuFile->addAction(mPrint);
  menuFile->addAction(mPrintPdf);
  menuFile->addAction(mSavePng);
  menuFile->addSeparator();
  menuFile->addAction(mEnd);

  menuView = menuBar()->addMenu(tr("&View","Menue"));
  //menuView->addSeparator();
  menuView->addAction(mFontInfo);
  menuView->addAction(mWidth);


  menuSetting = menuBar()->addMenu(tr("&Settings","Menue"));
  menuSetting->addAction(mLoadNewConfig);
  menuSetting->addAction(mSaveConfToFile);
  menuSetting->addSeparator();
  menuSetting->addAction(mOption);
  menuSetting->addSeparator();
  menuSetting->addAction(mFirmware);
  
  menuCurves = menuBar()->addMenu(tr("&Graph","Menue"));
  menuCurves->addAction(mCurveLoad);
  menuCurves->addAction(mCurveSave);
  menuCurves->addSeparator();
  menuCurves->addAction(mFrequencyMark);
  menuCurves->addAction(mHLine);

  menuSweep = menuBar()->addMenu(tr("&Sweep","Menue"));
  menuSweep->addAction(mSweepContinous);
  menuSweep->addAction(mSweepOnce);
  menuSweep->addAction(mStop);
  menuSweep->addSeparator();
  menuSweep->addAction(mCalibrateK1);
  menuSweep->addAction(mCalibrateK2);
  menuSweep->addAction(mLoadCallibrateK1);
  menuSweep->addAction(mLoadCallibrateK2);
  menuSweep->addAction(mSaveCallibrateK1);
  menuSweep->addAction(mSaveCallibrateK2);
  menuSweep->addSeparator();
  menuSweep->addAction(mProfile);

  menuMeasure = menuBar()->addMenu(tr("&Measurement","Menue"));
  menuMeasure->addAction(mWattFont);
  menuMeasure->addAction(mWattCallibrateK1);
  menuMeasure->addAction(mWattCallibrateK2);
  menuMeasure->addSeparator();
  menuMeasure->addAction(mWattSave);
  menuMeasure->addAction(mTableAsFileSave);
  menuMeasure->addSeparator();
  menuMeasure->addAction(mSaveToDefault);
  menuMeasure->addSeparator();
  menuMeasure->addAction(mWattEditprobe1);
  menuMeasure->addAction(mWattEditprobe2);

  menuHelp = menuBar()->addMenu(tr("&Help","Menue"));
  menuHelp->addAction(mTooltip);
  menuHelp->addSeparator();
  menuHelp->addAction(mInfo);
  menuHelp->addAction(mVersion);
}

void MainWindow::createActions()
{
  mPrint = new QAction(QIcon(":/images/print.png"), tr("&Print","Menue"), this);
  mPrint->setStatusTip(tr("Print Graphical Display","Statustip"));
  connect(mPrint, SIGNAL(triggered()), this, SLOT(printDiagramm()));

  mPrintPdf = new QAction(QIcon(":/images/printpdf.png"), tr("Save as PDF","Menue"), this);
  mPrintPdf->setStatusTip(tr("Save  Graphical Display as PDF-file","Statustip"));
  connect(mPrintPdf, SIGNAL(triggered()), this, SLOT(printDiagrammPdf()));

  mSavePng = new QAction(QIcon(":/images/saveimage.png"), tr("Save as image",""), this);
  mSavePng->setStatusTip(tr("Save the sweep window as a picture file",""));
  connect(mSavePng, SIGNAL(triggered()), this, SLOT(bildSave()));

  mEnd = new QAction(QIcon(":/images/exit.png"), tr("&Exit","Menue"), this);
  mEnd->setShortcut(Qt::CTRL + Qt::Key_Q);
  mEnd->setStatusTip(tr("Close program and save the settings","Statustip"));
  connect(mEnd, SIGNAL(triggered()), this, SLOT(beenden()));

  mLoadNewConfig = new QAction(tr("&Load","menue"), this);
  mLoadNewConfig->setStatusTip(tr("Load new Programm settings","Statustip"));
  connect(mLoadNewConfig, SIGNAL(triggered()), this, SLOT(saveSetting()));

  mSaveConfToFile = new QAction(tr("&Save","Menue"), this);
  mSaveConfToFile->setStatusTip(tr("Store the Program settings in a file","Statustip"));
  connect(mSaveConfToFile, SIGNAL(triggered()), this, SLOT(backupSetting()));

  mGeraetNeu = new QAction(tr("&New Devices",""), this);
  mGeraetNeu->setStatusTip(tr("Create a new devices path",""));
  connect(mGeraetNeu, SIGNAL(triggered()), this, SLOT(geraet_neu()));
  
  mCurveLoad = new QAction(QIcon(":/images/open.png"), tr("Load &Graph","Menue"), this);
  mCurveLoad->setStatusTip(tr("Load a Graph file","Statustip"));
  connect(mCurveLoad, SIGNAL(triggered()), this, SLOT(mcLoadCurves()));

  mCurveSave = new QAction(QIcon(":/images/save.png"),tr("Save &Graph","Menue"), this);
  mCurveSave->setStatusTip(tr("Save Graph to file","Statustip"));
  connect(mCurveSave, SIGNAL(triggered()), this, SLOT(mcSaveCurves()));
  
  mInfo = new QAction(QIcon(":/images/info.png"), tr("&About","Menue"), this);
  mInfo->setStatusTip(tr("About Program version","Statustip"));
  connect(mInfo, SIGNAL(triggered()), this, SLOT(showInfoDialog()));

  mVersion = new QAction(tr("&Firmware Version","Menue"), this);
  mVersion->setStatusTip(tr(" Firmware info","Statustip"));
  connect(mVersion, SIGNAL(triggered()), this, SLOT(version()));

  mFirmware = new QAction(tr("&Firmware Update","Menue"), this);
  mFirmware->setStatusTip(tr("Load new Firmware to NWT","Statustip"));
  connect(mFirmware, SIGNAL(triggered()), this, SLOT(updateFirmware()));

  mOption = new QAction(QIcon(":/images/option.png"), tr("&Options","Menue"), this);
  mOption->setStatusTip(tr("Set program Options","Statustip"));
  connect(mOption, SIGNAL(triggered()), this, SLOT(openOptionDialog()));

  mSweepContinous = new QAction(QIcon(":/images/appicon.png"),tr("Sweep","Menue"), this);
  mSweepContinous->setStatusTip(tr("Continuous Sweep","Statustip"));
  connect(mSweepContinous, SIGNAL(triggered()), this, SLOT(clickSweepStartOnce()));

  mSweepOnce = new QAction(tr("Single","Menue"), this);
  mSweepOnce->setStatusTip(tr("Single sweep","Statustip"));
  connect(mSweepOnce, SIGNAL(triggered()), this, SLOT(clickSweepStartTimes()));

  mStop = new QAction(tr("Stop","Menue"), this);
  mStop->setStatusTip(tr("Stop sweeping","Statustip"));
  connect(mStop, SIGNAL(triggered()), this, SLOT(clickSweepStop()));

  mCalibrateK1 = new QAction(tr("Channel 1 Calibration","Menue"), this);
  mCalibrateK1->setStatusTip(tr("Channel 1 Calibration (Collect Channel data)","Statustip"));
  connect(mCalibrateK1, SIGNAL(triggered()), this, SLOT(callibrateChanel1()));

  mCalibrateK2 = new QAction(tr("Channel 2 Calibration","Menu"), this);
  mCalibrateK2->setStatusTip(tr("Channel 2 Calibration (Collect Channel data)","Statustip"));
  connect(mCalibrateK2, SIGNAL(triggered()), this, SLOT(callibrateChanel2()));

  mLoadCallibrateK1 = new QAction(QIcon(":/images/open_ch1.png"), tr(" Select Channel 1","Menue"), this);
  mLoadCallibrateK1->setStatusTip(tr("Load  Channel 1 calibration ","Statustip"));
  connect(mLoadCallibrateK1, SIGNAL(triggered()), this, SLOT(messsondeladen1()));

  mLoadCallibrateK2 = new QAction(QIcon(":/images/open_ch2.png"), tr("Select Channel  2","Menue"), this);
  mLoadCallibrateK2->setStatusTip(tr("Load Channel 2 calibration","Statustip"));
  connect(mLoadCallibrateK2, SIGNAL(triggered()), this, SLOT(messsondeladen2()));

  mSaveCallibrateK1 = new QAction(tr("Store  Channel 1 calibration","Menue"), this);
  mSaveCallibrateK1->setStatusTip(tr("Storing Channel 1 calibration to a File","Statustip"));
  connect(mSaveCallibrateK1, SIGNAL(triggered()), this, SLOT(messsondespeichern1()));

  mSaveCallibrateK2 = new QAction(tr("Store Channel  2  calibration","Menue"), this);
  mSaveCallibrateK2->setStatusTip(tr("Storing Channel 2 calibration to a File","Statustip"));
  connect(mSaveCallibrateK2, SIGNAL(triggered()), this, SLOT(messsondespeichern2()));

  mFontInfo = new QAction(tr("Set Font of Infoscreen","Menue"), this);
  mFontInfo->setStatusTip(tr("Setting the Font of Infoscreen","Statustip"));
  connect(mFontInfo, SIGNAL(triggered()), this, SLOT(setInfoFont()));

  mFrequencyMark = new QAction(QIcon(":/images/fmarker.png"), tr("Frequency markers","Menue"), this);
  mFrequencyMark->setStatusTip(tr("Show frequency markers in Graphical Display","Statustip"));
  connect(mFrequencyMark, SIGNAL(triggered()), this, SLOT(setFrequenzmarken()));
  
  mHLine = new QAction(QIcon(":/images/hline.png"), tr("dB-Line",""), this);
  mHLine->setStatusTip(tr("Show an additional dB in-line Sweep window",""));
  connect(mHLine, SIGNAL(triggered()), this, SLOT(setHLine()));
  
  mProfile = new QAction(QIcon(":/images/profile.png"), tr("Profile","Menue"), this);
  mProfile->setStatusTip(tr("Edit profile","Statustip"));
  connect(mProfile, SIGNAL(triggered()), this, SLOT(editProfile()));
  
  mWidth = new QAction(QIcon(":/images/width.png"), tr("Change Windowsize","Menue"), this);
  mWidth->setStatusTip(tr("Change width of  Mainwindow","Statustip"));
  connect(mWidth, SIGNAL(triggered()), this, SLOT(setWidth()));

  mWattFont = new QAction(tr("Set Font","Menue"), this);
  mWattFont->setStatusTip(tr("Font setting","Statustip"));
  connect(mWattFont, SIGNAL(triggered()), this, SLOT(messsetfont()));

  mWattCallibrateK1 = new QAction(tr("Channel 1 Calibration","Menue"), this);
  mWattCallibrateK1->setStatusTip(tr("Wattmeter Calibration","Statustip"));
  connect(mWattCallibrateK1, SIGNAL(triggered()), this, SLOT(mkalibrierenk1()));

  mWattCallibrateK2 = new QAction(tr("Channel 2 Calibration","Menu"), this);
  mWattCallibrateK2->setStatusTip(tr("Wattmeter Calibration","Statustip"));
  connect(mWattCallibrateK2, SIGNAL(triggered()), this, SLOT(mkalibrierenk2()));

  mWattSave = new QAction(tr("Write to Table","Menue"), this);
  mWattSave->setStatusTip(tr("Writing Measuringdata to Table","Statustip"));
  connect(mWattSave, SIGNAL(triggered()), this, SLOT(tabelleschreiben()));

  mWattEditprobe1 = new QAction(tr("Edit Channel 1 calibration","Menue"), this);
  mWattEditprobe1->setStatusTip(tr("Edit Channel data","Statustip"));
  connect(mWattEditprobe1, SIGNAL(triggered()), this, SLOT(editProbe1()));
  
  mWattEditprobe2 = new QAction(tr("Edit Channel 2","Menue"), this);
  mWattEditprobe2->setStatusTip(tr("Edit Channel data","Statustip"));
  connect(mWattEditprobe2, SIGNAL(triggered()), this, SLOT(editProbe2()));
  
  mTableAsFileSave = new QAction(tr("Save data","Menue"), this);
  mTableAsFileSave->setStatusTip(tr("Store data as a Text-file ","Statustip"));
  connect(mTableAsFileSave, SIGNAL(triggered()), this, SLOT(tabellespeichern()));

  mSaveToDefault = new QAction(tr("Set Attenuation to default ","Menue"), this);
  mSaveToDefault->setStatusTip(tr("Clear ComboBox","Statustip"));
  connect(mSaveToDefault, SIGNAL(triggered()), this, SLOT(setDaempfungdefault()));
  
  mTooltip = new QAction(tr("Tip ?","Menue"), this);
  mTooltip->setStatusTip(tr("Mouse Help on/off","Statustip"));
  mTooltip->setCheckable(true);
  connect(mTooltip, SIGNAL(triggered()), this, SLOT(tooltip()));

}

void MainWindow::createToolBars()
{
  tbFile = addToolBar("ToolBarsDatei");
  tbFile->addAction(mEnd);
  tbFile->addAction(mPrint);
  tbFile->addAction(mPrintPdf);
  tbFile->addAction(mSavePng);

  tbSetting = addToolBar("ToolBarsEinstellung");
  tbSetting->addAction(mOption);
  tbSetting->addAction(mFrequencyMark);
  tbSetting->addAction(mHLine);
  tbSetting->addAction(mProfile);
  //tbSetting->addAction(mInfo);

  tbProbes = addToolBar("ToolBarsSonden");
  tbProbes->addAction(mLoadCallibrateK1);
  tbProbes->addAction(mLoadCallibrateK2);
  
  tbCurves = addToolBar("ToolBarsKurven");
  tbCurves->addAction(mCurveLoad);
  tbCurves->addAction(mCurveSave);
  tbCurves->addAction(mWidth);
}

void MainWindow::createStatusBar()
{
  statusBar()->showMessage("Ready");
}

void MainWindow::createToolTip(bool btip){
  nwtMainObj->tip(btip);
}

void MainWindow::tooltip(){
  writeSettings();
  createToolTip(mTooltip->isChecked());
}

void MainWindow::loadConfig(char *name)
{
  QString s = name;  
  //qDebug("MainWindow::loadConfig(char *name)");
  nwtMainObj->setconffilename(s);
}

void MainWindow::loadGeraet(char *name)
{
  QString s = name;  
  //qDebug("MainWindow::loadGeraet(char *name)");
  
/*  
  QDir dir=homedir;
  dir.setFilter(QDir::Dirs | QDir::Hidden | QDir::NoSymLinks);
  dir.setSorting(QDir::Size | QDir::Reversed);

  QFileInfoList list = dir.entryInfoList();
  std::cout << "     Bytes Filename" << std::endl;
  for (int i = 0; i < list.size(); ++i) {
      QFileInfo fileInfo = list.at(i);
      std::cout << qPrintable(QString("%1 %2").arg(fileInfo.size(), 10)
                                              .arg(fileInfo.fileName()));
      std::cout << std::endl;
  }
*/  
  nwtMainObj->setconffilename(s);
}

void MainWindow::geraet_neu(){
}

void MainWindow::readSettings()
{
  QSettings settings("AFU", "NWT");
  QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
  QSize size = settings.value("size", QSize(750, 480)).toSize();
  resize(size);
  move(pos);
  mTooltip->setChecked(settings.value("tooltip", true).toBool());
  nwtMainObj->readSettings();
  if(size.rwidth() < 750){
    bbreite = true;
  }else{
    bbreite = false;
  }
}

void MainWindow::setWidth()
{
  int breite = this->width();
  int hoehe = this->height();
  if(bbreite){
    bbreite = false;
    breite = 750;
  }else{
    bbreite = true;
    breite = 500;
  }
  resize(breite, hoehe);
}

void MainWindow::writeSettings()
{
  QSettings settings("AFU", "NWT");
  settings.setValue("pos", pos());
  settings.setValue("size", size());
  settings.setValue("tooltip", mTooltip->isChecked());
  nwtMainObj->writeSettings();
}

void MainWindow::printDiagramm(){
  QSize size = this->size();
//  resize(750,600);
  nwtMainObj->printDiagramm();
  resize(size);
}

void MainWindow::printDiagrammPdf(){
  QSize size = this->size();
//  resize(750,600);
  nwtMainObj->printDiagrammPdf();
  resize(size);
}

void MainWindow::bildSave(){
  nwtMainObj->saveDiagrammImage();
}

void MainWindow::Count(){
  nwtMainObj->fzaehlen();
}

void MainWindow::saveSetting(){
  nwtMainObj->loadConfigFileDlg();
}

void MainWindow::backupSetting(){
  nwtMainObj->saveConfigFileDlg();
}

void MainWindow::mcLoadCurves(){
  nwtMainObj->loadCurves();
}

void MainWindow::mcSaveCurves(){
  nwtMainObj->SaveCurvesMenu();
}

void MainWindow::showInfoDialog()
{
#ifdef Q_OS_WIN
  QMessageBox::about(this, "WinNWT5", infotext);
#else
  QMessageBox::about(this, "LinNWT5", infotext);
#endif
}

void MainWindow::openOptionDialog(){
  nwtMainObj->option();
}

void MainWindow::clickSweepStartOnce(){
  nwtMainObj->clickSweepContinuous();
}

void MainWindow::clickSweepStartTimes(){
  nwtMainObj->clickSweepOnce();
}

void MainWindow::clickSweepStop(){
  nwtMainObj->clickSweepStop();
}

void MainWindow::callibrateChanel1(){
  nwtMainObj->calibratechanel1();
}

void MainWindow::callibrateChanel2(){
  nwtMainObj->calibratechanel2();
}

void MainWindow::messsondeladen1(){
  nwtMainObj->messurechanel1();
}

void MainWindow::messsondeladen2(){
  nwtMainObj->messurechanel2();
}

void MainWindow::messsondespeichern1(){
  nwtMainObj->messsondespeichern1("def");
}

void MainWindow::messsondespeichern2(){
  nwtMainObj->messsondespeichern2("def");
}

void MainWindow::setInfoFont(){
  nwtMainObj->setInfoFont();
}

void MainWindow::setFrequenzmarken(){
  nwtMainObj->setFrequenzmarken();
}

void MainWindow::setHLine(){
  nwtMainObj->setHLine();
}

void MainWindow::editProfile(){
  nwtMainObj->editProfile();
}

void MainWindow::messsetfont(){
  nwtMainObj->messsetfont();
}

void MainWindow::mkalibrierenk1(){
  nwtMainObj->mkalibrierenk1();
}

void MainWindow::mkalibrierenk2(){
  nwtMainObj->mkalibrierenk2();
}

void MainWindow::editProbe1(){
  nwtMainObj->editProbe1();
}

void MainWindow::editProbe2(){
  nwtMainObj->editProbe2();
}

void MainWindow::version(){
  nwtMainObj->version();
}

void MainWindow::tabelleschreiben(){
  nwtMainObj->writeMessureToTable();
}

void MainWindow::tabellespeichern(){
  nwtMainObj->saveTableToFile();
}

void MainWindow::setDaempfungdefault(){
  nwtMainObj->setDaempfungdefault();
}

void MainWindow::beenden()
{
  nwtMainObj->beenden();
  close();
}

void MainWindow::setText(QString stty){
  QString qs;
  #ifdef Q_OS_WIN
  qs = "WinNWT5";
  #else
  qs = "LinNWT5";
  #endif
  QString info = infoversion+"."+infobuildnum;
  setWindowTitle(tr("%1 - V.%2 - %3","Don't translate").arg(qs).arg(info).arg(stty));
}

void MainWindow::updateFirmware(){
  nwtMainObj->firmupdate();
}

void MainWindow::mSetMenu(emenuenable menuenable, bool einaus){
  switch(menuenable){
    case emPrint: mPrint->setEnabled(einaus);break;
    case emPrintPdf: mPrintPdf->setEnabled(einaus);break;
    case emSavePNG: mSavePng->setEnabled(einaus);break;
    case emSpeichernpng: mSavePng->setEnabled(einaus);break;
    case emEnd: mEnd->setEnabled(einaus);break;
    case emLoadNewConfig: mLoadNewConfig->setEnabled(einaus);break;
    case emSaveConfToFile: mSaveConfToFile->setEnabled(einaus);break;
    case emInfo: mInfo->setEnabled(einaus);break;
    case emOption: mOption->setEnabled(einaus);break;
    case emFirmware: mFirmware->setEnabled(einaus);break;
    case emCurveLoad: mCurveLoad->setEnabled(einaus);break;
    case emCurveSave: mCurveSave->setEnabled(einaus);break;
    case emSweep: mSweepContinous->setEnabled(einaus);break;
    case emOnce: mSweepOnce->setEnabled(einaus);break;
    case emStop: mStop->setEnabled(einaus);break;
    case emCalibrateK1: mCalibrateK1->setEnabled(einaus);break;
    case emCalibrateK2: mCalibrateK2->setEnabled(einaus);break;
    case emLoadCallibrateK1: mLoadCallibrateK1->setEnabled(einaus);break;
    case emLoadCallibrateK2: mLoadCallibrateK2->setEnabled(einaus);break;
    case emSaveCallibrateK1: mSaveCallibrateK1->setEnabled(einaus);break;
    case emSaveCallibrateK2: mSaveCallibrateK2->setEnabled(einaus);break;
    case emFontInfo: mFontInfo->setEnabled(einaus);break;
    case emFrequencyMark: mFrequencyMark->setEnabled(einaus);break;
    case emWattFont: mWattFont->setEnabled(einaus);break;
    case emWattCallibrateK1: mWattCallibrateK1->setEnabled(einaus);break;
    case emWattCallibrateK2: mWattCallibrateK2->setEnabled(einaus);break;
    case emWattEditSonde1: mWattEditprobe1->setEnabled(einaus);break;
    case emWattEditSonde2: mWattEditprobe2->setEnabled(einaus);break;
    case emWattSave: mWattSave->setEnabled(einaus);break;
    case emTableAsFileSave: mTableAsFileSave->setEnabled(einaus);break;
    case emSaveToDefault: mSaveToDefault->setEnabled(einaus);break;
    case emVersion: mVersion->setEnabled(einaus);break;
    case emTooltip: mTooltip->setEnabled(einaus);break;
    case emenuFile: menuFile->setEnabled(einaus);break;
    case emenuSetting: menuSetting->setEnabled(einaus);break;
    case emenuCurves: menuCurves->setEnabled(einaus);break;
    case emenuSweep: menuSweep->setEnabled(einaus);break;
//    case emmenuvfo: menuvfo->setEnabled(einaus);break;
    case emenuMeasure: menuMeasure->setEnabled(einaus);break;
    case eshowMinimized: showMinimized();break;
    case eshowNormal: showNormal();break;
  }
}  


void MainWindow::paintEvent(QPaintEvent*){
#ifdef PAINTDEBUG
  qDebug("MainWindow::paintEvent");
#endif
  nwtMainObj->resizeWidgets();
}

void MainWindow::resizeEvent( QResizeEvent * ){
  //nach Groessenaenderung grafische Elemente anpassen
#ifdef PAINTDEBUG
  qDebug("MainWindow::resizeEvent");
  qDebug("breite %i hoehe %i", width(), height());
#endif
  resize(this->rect().size());
  nwtMainObj->resizeWidgets();
}

void MainWindow::setProgramPath(QString s){
  //qDebug("MainWindow::setProgramPath(QString s)");
  programpath = s;
  nwtMainObj->setProgramPath(programpath);
}

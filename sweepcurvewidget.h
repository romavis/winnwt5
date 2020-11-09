/***************************************************************************
    sweepcurvewidget.cpp

    widgetwobbeln.cpp  -  description
    ----------------------------------------------------
    begin                : 2003
    copyright            : (C) 2003 by Andreas Lindenau
    email                : DL4JAL@darc.de

    widgetwobbeln.cpp  have been refactored to sweepcurvewidget.cpp QT5
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

#ifndef WIDGETWOBBWELN_H
#define WIDGETWOBBWELN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QWidget>
#include <QPainter>
#include <QPixmap>
#include <QString>
#include <QLineEdit>
#include <QObject>
#include <QSpinBox>
#include <QCheckBox>
#include <QPushButton>
#include <QDialog>
//#include <QtPrintSupport>
#include <QPrinter>

#include <QLabel>
#include <QGroupBox>
#include <QVector>

/*#include <Q3PaintDeviceMetrics>*/
//#include <Q3PopupMenu>
#include <QMouseEvent>
#include <QtGui>

#include "constdef.h"
  
class SaveCurveDlg : public QDialog
{
  Q_OBJECT

public:
  SaveCurveDlg(QWidget* parent=0, Qt::WindowFlags f=0);
  ~SaveCurveDlg();
  

public slots:
  void setBildinfo(TBildinfo);

private slots:
  void changefont(int);
  void changexpos(int);
  void changeypos(int);
  void changesetboxinfo();
  
private:
  TBildinfo bbildinfo;

  QGroupBox *ginfo;
  QSpinBox *spfontsize;
  QSpinBox *spxpos;
  QSpinBox *spypos;
  
  QLabel *labelfontsize; 
  QLabel *labelxpos; 
  QLabel *labelypos; 

  QPushButton *buttonok;
  QPushButton *buttoncancel;
  QPushButton *fontcolor;
  
  QCheckBox *checkboxinfo;
  
  signals:
  void  pixauffrischen(const TBildinfo &);
};

class SweepCurveWidget : public QWidget
{
  Q_OBJECT

private:
  QVector<QPoint> kurve;
  QVector<QPoint> kurveb;


  public:
    /** construtor */
  SweepCurveWidget(QWidget* parent=0);
    /** destructor */
  ~SweepCurveWidget();

  void setSweepData(TSweep);    
  TSweep getSweepData();
  double getCallibrateWorthK1(double afrequenz);
  double getCallibrateWorthK2(double afrequenz);
  double getSWRCallibrateWorthK1(double afrequenz);
  double getSWRCallibrateWorthK2(double afrequenz);
  
  public slots:
  void setWCursorNr(int);
  void setMouseCursorCenter();
  void newBuildWidthInfo(const TBildinfo &);

  protected:
  void closeEvent(QCloseEvent *event);
  virtual void paintEvent(QPaintEvent*);
  virtual void resizeEvent(QResizeEvent*);
  virtual void mouseMoveEvent(QMouseEvent *e);
  virtual void mousePressEvent(QMouseEvent *e);
  virtual void keyPressEvent( QKeyEvent *event );

  private:
  QAction *akursor_clr;
  QAction *akursor_clr_all;
  QAction *akurve_clr;
  QAction *afrequenzanfang;
  QAction *afrequenzende;
  QAction *akabellaenge;
  QAction *akabeldaten;
  QAction *aberechnen;
  
//  Q3PopupMenu *menmouse;
  QMenu *popupmenu;
  
  QPixmap pix, pix1;
  TSweep sweepData;
  TSweep kurve1;
  TSweep kurve2;
  TSweep kurve3;
  TSweep kurve4;
  TSweep kurve5;
  TSweep kurve6;

  double dbreitealt;
  double dbreite;
  int upperBorder;
  int widthBorder;
  int mouseposx[5];
  int mouseposx1[5];
  int mouseposy[5];
  int kxm1[5];
  int kxm2[5];
  int cursorix;
  bool bnomove;
  bool banfang;
  bool bende;
  bool bberechnung;
  bool blupeplus;
  bool blupeminus;
  double mousefrequenz;
  TFrqMarkers wfmarken;
  int hoehe;  //Hoehe der Wobbeldarstellung
  int breite; //Breite der wobbeldarstellung
  int durchmesser; //Durchmesser Antennendiagramm
  int yswrteilung; //SWR Anzeige Aufloesung
  //double yswraufloesung; //ADC Aufloesung
  double xSchritt; //Schrittweite Y 
  double ySchritt; //Schrittweite X
  double xAnfang; //absoluter Anfang normal bei 0.0
  double xEnde; //absolutes Ende normal Wobbelfensterbreite
  bool bresize;
  //obere Anzeige der LOG_Y-Achse
  double ymax;//dB
  //obere Anzeige der LOG_Y-Achse
  double ymin;//dB
  //obere Anzeige der LOG_Y-Achse
  double ydisplayteilung_log;

  QStringList qusl;
  QString fname;
  QString squdaten;
  bool speichern;
  int qunr;
  
  QStringList infolist;
  TBildinfo wbildinfo;
  
  QString kursorsavstr(double);
  double getSavCalWorth(double afrequenz);
  void resetMouseCursor();
  void drawGrid(QPainter *p);
  void drawCurve(QPainter *p);
  void refreshPixmap();
  void refreshCurve();
  void setParameterPixmap();
  double korrsavpegel(double);
  void CreateQuarzFile();
  void WriteToQuarzDatei(const QString &);
  void CloseQuarzFile();

  private slots:
  void setCurve(TSweep);
  void deleteCursor();
  void deleteCurve();
  void wSetFrqBegin();
  void wSetFrqEnd();
  void setCalculation();
  void CableLen();
  void CableData();
  void deleteAllCursor();
  void SweepDataReceipt(const TSweep&);
  void SweepDataCurve(const TMessure&);
  void printDiagramm(const QFont &, const QStringList &);
  void printDiagramPdf(const QFont &, const QStringList &);
  void bildspeichern(const TBildinfo &, const QStringList &);
  void empfangfrqmarken(const TFrqMarkers &);
  
  signals:
  void multiEditDelete();
  void multiEditInsert(const QString &);
  void writeYWorth(int);
  void writeBegin(double);
  void writeEnd(double);
  void writeCalculation(double);
  void setCursorFrequency(double);
  void resetCursor(int);
  void MagnifyPlus();
  void MagnifyMinus();
  void MagnifyEqual();
  void SweepOnce();
  void SweepTimes();
  void SweepStop();
  void wCurveDelete();

};

#endif

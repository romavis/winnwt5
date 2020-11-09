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

#include <math.h>
#include <stdlib.h>
#include <QDateTime>
#include <QCursor>
#include <QFileDialog>
#include <QLocale>
#include <QtGlobal>
//#include <QPaintDeviceMetrics>
#include <QPaintDevice>
#include <QMessageBox>
#include <QInputDialog>
#include <QStylePainter>
#include <QTextStream>
#include <QMapIterator>
#include <QVector>
#include <QMenu>


#include "sweepcurvewidget.h"

//#define LDEBUG
//#define PAINTGRAF_DEBUG
#ifdef LDEBUG
    #include <QtDebug>
#endif

/*
class winsinfo : public QWidget
{
  Q_OBJECT

public:
  winsinfo(QWidget * parent = 0, Qt::WindowFlags f = 0);
  ~winsinfo();

public slots:
private slots:
  void changefont();
  void changexpos();
  void changeypos();
  void clickinfo();
  
private:

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
  void  pixauffrischen();
};
*/

SaveCurveDlg::SaveCurveDlg(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
  this->resize(270, 260);
  this->setMinimumSize(270, 260);
  
  checkboxinfo = new QCheckBox(tr("Show info"), this);
  checkboxinfo->setGeometry(10,10,200,30);
  QObject::connect( checkboxinfo, SIGNAL(stateChanged (int)), this, SLOT(changesetboxinfo()));
  
  ginfo = new QGroupBox(tr("Place Info"), this);
  ginfo->setGeometry(10,50,250,150);
  ginfo->setEnabled(FALSE);

  spfontsize = new QSpinBox(ginfo);
  spfontsize->setGeometry(150,20,60,30);
  spfontsize->setRange(5,30);
  spfontsize->setSingleStep(1);
  spfontsize->setValue(10);
  QObject::connect( spfontsize, SIGNAL(valueChanged(int)), this, SLOT(changefont(int)));
  
  labelfontsize = new QLabel(tr("Font-size"), ginfo);
  labelfontsize->setGeometry(10,20,80,30);
   
  QLabel *labelxpos; 
  QLabel *labelypos; 
  
  spxpos = new QSpinBox(ginfo);
  spxpos->setGeometry(150,60,60,30);
  spxpos->setRange(0,1000);
  spxpos->setSingleStep(10);
  QObject::connect( spxpos, SIGNAL(valueChanged(int)), this, SLOT(changexpos(int)));
  
  labelxpos = new QLabel(tr("X-Position"), ginfo);
  labelxpos->setGeometry(10,60,80,30);
  
  spypos = new QSpinBox(ginfo);
  spypos->setGeometry(150,100,60,30);
  spypos->setRange(0,1000);
  spypos->setSingleStep(10);
  QObject::connect( spypos, SIGNAL(valueChanged(int)), this, SLOT(changeypos(int)));
  
  labelypos = new QLabel(tr("Y-Position"), ginfo);
  labelypos->setGeometry(10,100,80,30);
  
  buttonok = new QPushButton(tr("Save Image",""), this);
  buttonok->setGeometry(130,210,120,30);
  connect(buttonok, SIGNAL(clicked()), SLOT(accept()));
  
  buttoncancel = new QPushButton(tr("Exit",""), this);
  buttoncancel->setGeometry(20,210,100,30);
  connect(buttoncancel, SIGNAL(clicked()), SLOT(reject()));
}

SaveCurveDlg::~SaveCurveDlg()
{
}

void SaveCurveDlg::setBildinfo(TBildinfo binfo)
{
  bbildinfo = binfo;
  spxpos->setValue(bbildinfo.posx);
  spypos->setValue(bbildinfo.posy);
  spfontsize->setValue(bbildinfo.fontsize);
}

void SaveCurveDlg::changefont(int a)
{
  bbildinfo.fontsize = a;
  emit pixauffrischen(bbildinfo);
}

void SaveCurveDlg::changexpos(int x)
{
  bbildinfo.posx = x;
  emit pixauffrischen(bbildinfo);
}

void SaveCurveDlg::changeypos(int y)
{
  bbildinfo.posy = y;
  emit pixauffrischen(bbildinfo);
}

void SaveCurveDlg::changesetboxinfo()
{
  if(checkboxinfo->checkState() == Qt::Checked){
    ginfo->setEnabled(TRUE);
    bbildinfo.infoanzeigen = TRUE;
  }else{
    ginfo->setEnabled(FALSE);
    bbildinfo.infoanzeigen = FALSE;
  }
  emit pixauffrischen(bbildinfo);
}

SweepCurveWidget::SweepCurveWidget(QWidget *parent) : QWidget(parent)
{
  this->resize(800,480);
  this->setMinimumSize(400,400);
  this->setWindowTitle(tr("Graphical Display",""));
  this->setWindowIcon( QIcon(":/images/appicon2.png"));
  this->setWindowFlags(Qt::WindowTitleHint | Qt::WindowMinimizeButtonHint |
                              Qt::WindowMaximizeButtonHint|Qt::WindowCloseButtonHint);

  
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::WidgetWobbeln()");
  #endif
  //  setBackgroundMode( NoBackground );
  pix = QPixmap(this->size());
  pix1 = QPixmap(this->size());
#ifdef PAINTGRAF_DEBUG
  qDebug("WidgetWobbeln::WidgetWobbeln() pix :w:%i h:%i",pix.rect().size().width(),pix.rect().size().height());
  qDebug("WidgetWobbeln::WidgetWobbeln() pix1 :w:%i h:%i",pix1.rect().size().width(),pix1.rect().size().height());
#endif
  upperBorder= 35;
  widthBorder= 35;
  cursorix = 0;
  
  int i;
  for(i=0; i<=5; i++){
    mouseposx[cursorix] = 0;
    mouseposx1[cursorix] = 0;
    mouseposy[cursorix] = 0;
  }
  
  banfang = FALSE;
  bende = FALSE;
  blupeplus = FALSE;
  blupeminus = FALSE;
  mousefrequenz = 0.0;
 /*
  menmouse = new Q3PopupMenu(this);
  menmouse->insertItem(tr(" Delete Cursor#",""), this, SLOT(kursor_loeschen()));
  menmouse->insertItem(tr("Delete all Cursors",""), this, SLOT(alle_kursor_loeschen()));
  menmouse->insertItem(tr("Delete curve",""), this, SLOT(kurve_loeschen()));
  menmouse->insertSeparator();
  menmouse->insertItem(tr("Startfrequency ",""), this, SLOT(wsetfrqanfang()));
  menmouse->insertItem(tr("Stopfrequency",""), this, SLOT(wsetfrqende()));
  menmouse->insertSeparator();
  menmouse->insertItem(tr("Measuring the Geometric Cablelength",""), this, SLOT(kabellaenge()));
  menmouse->insertItem(tr("Measuring the Electrical parameters of the Cable",""), this, SLOT(kabeldaten()));
  menmouse->insertSeparator();
  menmouse->insertItem(tr("Cursor-frequency to Calculate",""), this, SLOT(setberechnung()));
*/  
  popupmenu = new QMenu();
  akursor_clr = new QAction(tr(" Delete Cursor#",""), popupmenu);
  connect(akursor_clr, SIGNAL(triggered()), this, SLOT(deleteCursor()));
  popupmenu->addAction(akursor_clr);
  akursor_clr_all = new QAction(tr("Delete all Cursors",""), popupmenu);
  connect(akursor_clr_all, SIGNAL(triggered()), this, SLOT(deleteAllCursor()));
  popupmenu->addAction(akursor_clr_all);
  akurve_clr = new QAction(tr("Delete curve",""), popupmenu);
  connect(akurve_clr, SIGNAL(triggered()), this, SLOT(deleteCurve()));
  popupmenu->addAction(akurve_clr);
  popupmenu->addSeparator();
  afrequenzanfang = new QAction(tr("Startfrequency ",""), popupmenu);
  connect(afrequenzanfang, SIGNAL(triggered()), this, SLOT(wSetFrqBegin()));
  popupmenu->addAction(afrequenzanfang);
  afrequenzende = new QAction(tr("Stopfrequency",""), popupmenu);
  connect(afrequenzende, SIGNAL(triggered()), this, SLOT(wSetFrqEnd()));
  popupmenu->addAction(afrequenzende);
  popupmenu->addSeparator();
  akabellaenge = new QAction(tr("Measuring the Geometric Cablelength",""), popupmenu);
  connect(akabellaenge, SIGNAL(triggered()), this, SLOT(CableLen()));
  popupmenu->addAction(akabellaenge);
  akabeldaten = new QAction(tr("Measuring the Electrical parameters of the Cable",""), popupmenu);
  connect(akabeldaten, SIGNAL(triggered()), this, SLOT(CableData()));
  popupmenu->addAction(akabeldaten);
  popupmenu->addSeparator();
  aberechnen = new QAction(tr("Cursor-frequency to Calculate",""), popupmenu);
  connect(aberechnen, SIGNAL(triggered()), this, SLOT(setCalculation()));
  popupmenu->addAction(aberechnen);
  akursor_clr->setEnabled(FALSE);
  akursor_clr_all->setEnabled(FALSE);
  afrequenzanfang->setEnabled(FALSE);
  afrequenzende->setEnabled(FALSE);
  akabellaenge->setEnabled(FALSE);
  akabeldaten->setEnabled(FALSE);
  aberechnen->setEnabled(FALSE);
  
  setBackgroundRole(QPalette::Dark);
  setAutoFillBackground(TRUE);
  setCursor(Qt::CrossCursor);
  kurve1.mess.daten_enable = FALSE;
  kurve2.mess.daten_enable = FALSE;
  kurve3.mess.daten_enable = FALSE;
  kurve4.mess.daten_enable = FALSE;
  sweepData.bhintergrund = FALSE;
  bresize = TRUE;
  //obere Anzeige der LOG_Y-Achse
  ymax=10.0;//dB
  //obere Anzeige der LOG_Y-Achse
  ymin=-90.0;//dB
  //obere Anzeige der LOG_Y-Achse
  ydisplayteilung_log= (ymax - ymin) / 10.0;
  //fuer Stringliste Quarzdaten
  speichern = FALSE; 
}

SweepCurveWidget::~SweepCurveWidget()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::~WidgetWobbeln()");
  #endif
  /*
  if(kurve1 != NULL) delete kurve1;
  if(kurve2 != NULL) delete kurve2;
  if(kurve3 != NULL) delete kurve3;
  if(kurve4 != NULL) delete kurve4;
  */
}

void SweepCurveWidget::setCurve(TSweep akurve){
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::setCurve()");
  #endif
  switch(akurve.kurvennr){
    case 1:
      kurve1 = akurve;
      kurve1.absanfang = sweepData.absanfang;
      kurve1.absende = sweepData.absende;
    break;
    case 2:
      kurve2 = akurve;
      kurve2.absanfang = sweepData.absanfang;
      kurve2.absende = sweepData.absende;
    break;
    case 3:
      kurve3 = akurve;
      kurve3.absanfang = sweepData.absanfang;
      kurve3.absende = sweepData.absende;
    break;
    case 4:
      kurve4 = akurve;
      kurve4.absanfang = sweepData.absanfang;
      kurve4.absende = sweepData.absende;
    break;
    case 5:
      kurve5 = akurve;
      kurve5.absanfang = sweepData.absanfang;
      kurve5.absende = sweepData.absende;
    break;
    case 6:
      kurve6 = akurve;
      kurve6.absanfang = sweepData.absanfang;
      kurve6.absende = sweepData.absende;
    break;

  }
  refreshPixmap();
  repaint();
}

//Die Kursornummer kommt vom Hauptfenster
void SweepCurveWidget::setWCursorNr(int a){
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::setWCursorNr()");
  #endif
  cursorix = 4 -a;
  refreshCurve();
}

void SweepCurveWidget::CableData()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::CableData()");
  #endif
  QString beschr(tr("<br><b> Determination of the Cabledata :</b> <br><br> 1)  Let the Cable-end open and insert a Series-Resistor<br>... of 50 ohms  between Returnloss-bridge and inner conductor<br>... of the cable to be measured.<br><br>2) Put the cursor is at the lowest resonance-frequency.<BR><BR> Measured Length of the Cable (m) :",""));
  QString qs, qv, qe;
  double v, e;
  bool ok;
  double l = QInputDialog::getDouble(this, tr("Cablelength",""),
                                           beschr, 0.0, 0, 10000, 3, &ok); 
  if(ok){
    v = 0.0133 * l * (mousefrequenz / 1000000.0);
    e = 1 / (v * v);
    qv.sprintf("%1.3f",v);
    qe.sprintf("%1.3f",e);
    qs = tr("<b> Calculated Cabledata :</b> <br><center>Velocityfactor: ","") +
            qv +
            "<BR>" +
            tr("Dielectric-constant: ","") +
            qe;
    QMessageBox::warning( this, tr("Cabledata",""), qs);
  }
}


void SweepCurveWidget::CableLen()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::CableLen()");
  #endif
  QString beschr(tr("<br><b> Determination of the cablelength :</b> <br><br> There are 2 ways:<br> <br> 1) Leave the Cable-end open and insert a seriesresistor of 50 ohms <br>... between Returnloss-bridge and the cable to be measured.<br> <br> 2) Short the Cable-end and insert a 50 ohm Resistor in parallel to the Input of the cable.<br> <BR> After selecting the wiring,  measure the SWR  from 0.1 to 160 MHz<br> and  set the cursor to the lowest  resonancefrequency.<br> For larger cable lengths , it is necessary to reduce the Stopfrequency,<br> to get more accurate results. <br><br> VelocitfFactor of the Cable (default = PE cable) :",""));
  QString qs, qer;
  double er;
  bool ok;
  double v = QInputDialog::getDouble(this, tr("Velocityfactor",""),
                                           beschr, 0.67, -10000, 10000, 3, &ok); 
  if(ok){
    er = v / (mousefrequenz / 1000000.0) * 75.0;
    qer.sprintf("%1.3f",er);
    qs = tr("<b> Calculated Cablelength :</b> <br><center> Geometric Length of the Cable: ","") +
            qer +
            " m";
    QMessageBox::warning( this, tr("Geometric Length",""), qs);
  }
}

void SweepCurveWidget::deleteCursor()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::deleteCursor()");
  #endif
  resetMouseCursor();
//  menmouse->hide();
}

void SweepCurveWidget::deleteCurve()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::deleteCurve()");
  #endif
  emit wCurveDelete();
}

void SweepCurveWidget::deleteAllCursor()
{
  int k;
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::deleteAllCursor()");
  #endif
  for(k=0;k<5;k++){
    cursorix = k;
    resetMouseCursor();
  }  
//  menmouse->hide();
  setWCursorNr(4);
  emit resetCursor(4);
}

void SweepCurveWidget::wSetFrqBegin()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::wSetFrqBegin()");
  #endif
  banfang = TRUE;
  refreshCurve();
//  menmouse->hide();
}

void SweepCurveWidget::wSetFrqEnd()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::wSetFrqEnd()");
  #endif
  bende = TRUE;
  refreshCurve();
//  menmouse->hide();
}

void SweepCurveWidget::setCalculation()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::setCalculation()");
  #endif
  bberechnung = TRUE;
  refreshCurve();
//  menmouse->hide();
}

void SweepCurveWidget::mouseMoveEvent(QMouseEvent * e)
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::mouseMoveEvent()");
  #endif
  if(sweepData.mess.daten_enable){
    mouseposx[cursorix] = e->x();
    mouseposy[cursorix] = e->y();
    if(!sweepData.mousesperre){
      refreshPixmap();
      repaint();
    }
  }
}

void SweepCurveWidget::mousePressEvent(QMouseEvent * e)
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::mousePressEvent()");
  #endif
  bnomove = FALSE;
  if(sweepData.mess.daten_enable){
    if(e->button() == Qt::LeftButton){
      mouseposx[cursorix] = e->x();
      mouseposy[cursorix] = e->y();
      bnomove = TRUE;
    }
    if(e->button() == Qt::RightButton){
      if(mouseposx[cursorix] != 0){
        akursor_clr->setEnabled(TRUE);
//        akursor_clr_all->setEnabled(TRUE);
        afrequenzanfang->setEnabled(TRUE);
        afrequenzende->setEnabled(TRUE);
        akabellaenge->setEnabled(TRUE);
        akabeldaten->setEnabled(TRUE);
        aberechnen->setEnabled(TRUE);
      }else{
        akursor_clr->setEnabled(FALSE);
//        akursor_clr_all->setEnabled(FALSE);
        afrequenzanfang->setEnabled(FALSE);
        afrequenzende->setEnabled(FALSE);
        akabellaenge->setEnabled(FALSE);
        akabeldaten->setEnabled(FALSE);
        aberechnen->setEnabled(FALSE);
      }
      popupmenu->popup(QCursor::pos());
      akursor_clr_all->setEnabled(FALSE);
      // Ist noch ein Kursor aktiv ?
      for(int i=0; i<5; i++){
        if(mouseposx[i] != 0){
          akursor_clr_all->setEnabled(TRUE);
          break;
        }
      }
    }
    if(!sweepData.mousesperre){
      refreshPixmap();
      repaint();
    }
  }
}

void SweepCurveWidget::setMouseCursorCenter()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::setMouseCursorCenter()");
  #endif
  QSize pixsize = this->rect().size();
  int i = pixsize.width();
  mouseposx[cursorix] = i/2;
  if(!sweepData.mousesperre){
    refreshPixmap();
    repaint();
  }
}

void SweepCurveWidget::resetMouseCursor()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::resetMouseCursor()");
  #endif
  mouseposx[cursorix] = 0;
  mouseposy[cursorix] = 0;
  if(!sweepData.mousesperre){
    refreshPixmap();
    repaint();
  }
}

double SweepCurveWidget::getCallibrateWorthK1(double afrequenz)
{
  double findex;
  int i;
  
  #ifdef PAINTGRAF_DEBUG
//    qDebug("WidgetWobbeln::getCallibrateWorthK1()");
  #endif
  if(sweepData.bkalibrierkorrk1 and sweepData.eoperatemode == eGainSweep)
  {
    //qDebug("WidgetWobbeln::getkalibrierwertk1()");
    //Stuetzfrequenzen beziehen sich auf die Grundfrequenz
    //deshalb wieder durch den Frqfaktor teilen
    findex = afrequenz / sweepData.freq_faktor;
    findex = round(findex / eteiler);
    i = int(findex);
    if(i > (maxmesspunkte-1))i = maxmesspunkte-1;
    return sweepData.kalibrierarray.arrayk1[i];
  }else{
    return 0.0;
  }
}

double SweepCurveWidget::getCallibrateWorthK2(double afrequenz)
{
  double findex;
  int i;
  
  #ifdef PAINTGRAF_DEBUG
//    qDebug("WidgetWobbeln::getCallibrateWorthK2()");
  #endif
  if(sweepData.bkalibrierkorrk2 and sweepData.eoperatemode == eGainSweep)
  {
    //Stuetzfrequenzen beziehen sich auf die Grundfrequenz
    //deshalb wieder durch den Frqfaktor teilen
    findex = afrequenz / sweepData.freq_faktor;
    findex = round(findex / eteiler);
    i = int(findex);
    if(i > (maxmesspunkte-1))i = maxmesspunkte-1;
    return sweepData.kalibrierarray.arrayk2[i];
  }else
  {
    return 0.0;
  }
}

double SweepCurveWidget::getSavCalWorth(double afrequenz)
{
  int a,b;
  double pegela, pegelb, pegeldiff;
  double frequa, frequb, frequdiff;
  
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::getSavCalWorth()");
  #endif
  //aus der Frequenz den Index errechnen
  // Bereich 0 - 100 GHz
  a = int(savcalkonst * log10(afrequenz));
  b = a + 1;
  //arraygrenzen einhalten
  if(a<0)a=0;
  if(a > calmaxindex-1)a = calmaxindex-1;
  if(b > calmaxindex-1)b = calmaxindex-1;
  pegela = sweepData.savcalarray.p[a];
  // Ist das Ende des Array erreicht?
  if(a != b){
    //Eine Iteration ist moeglich
    pegelb = sweepData.savcalarray.p[b];
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

double SweepCurveWidget::getSWRCallibrateWorthK1(double afrequenz)
{
  double findex;
  int i;
  double w1;
  
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::getSWRCallibrateWorthK1()");
  #endif
  //Stuetzfrequenzen beziehen sich auf die Grundfrequenz
  //deshalb wieder durch den Frqfaktor teilen
  findex = afrequenz / sweepData.freq_faktor;
  findex = round(findex / eteiler);
  i = int(findex);
  if(i > (maxmesspunkte-3))i = maxmesspunkte-3;
//  return wswrkalibrierarray.arrayk1[i];
  w1 = (sweepData.swrkalibrierarray.arrayk1[i-2] +
        sweepData.swrkalibrierarray.arrayk1[i-1] +
        sweepData.swrkalibrierarray.arrayk1[i] +
        sweepData.swrkalibrierarray.arrayk1[i+1] +
    sweepData.swrkalibrierarray.arrayk1[i+2]) / 5.0;
  return (w1);
}

double SweepCurveWidget::getSWRCallibrateWorthK2(double afrequenz)
{
  double findex;
  int i;
  
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::getSWRCallibrateWorthK2()");
  #endif
  //Stuetzfrequenzen beziehen sich auf die Grundfrequenz
  //deshalb wieder durch den Frqfaktor teilen
  findex = afrequenz / sweepData.freq_faktor;
  findex = round(findex / eteiler);
  i = int(findex);
  if(i > (maxmesspunkte-1))i = maxmesspunkte-1;
  return sweepData.swrkalibrierarray.arrayk2[i];
}

void SweepCurveWidget::setParameterPixmap()
{
  QSize pixsize;
  bool drawenable = FALSE;
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::setParameterPixmap()");
  #endif
  pixsize = this->rect().size();
  drawenable = (pixsize.width() > 0) and (pixsize.height() > 0);
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::setParameterPixmap():w:%i h:%i",pixsize.width(), pixsize.height());
  #endif
  //delete the Pixmap
  if(drawenable){
    //pix.resize(pixsize);
    //pix = 0;
    pix = QPixmap(pixsize);
    QPainter p;
    p.begin(&pix);
    #ifdef PAINTGRAF_DEBUG
        qDebug("pix->size: w:%i h:%i",pix.size().width(), pix.size().height());
    #endif
    if(sweepData.linear1 or sweepData.linear2){
      ymax = 10.0;
      ymin = -100.0;
    }else{
      ymax = double(sweepData.ydbmax);
      ymin = double(sweepData.ydbmin);
    }
    #ifdef PAINTGRAF_DEBUG
      qDebug("ymax: %f", ymax);
      qDebug("ymin: %f", ymin);
    #endif
    //Background color of the Sweep window
    if((sweepData.eoperatemode == eSpectrumAnalyser) or
       (sweepData.eoperatemode == eSpectrumDisplayShift)){
      //SAV background with gray Areas
      if(sweepData.linear1 or sweepData.linear2){
        //Background without gray Areas
          #ifdef PAINTGRAF_DEBUG
            qDebug("1 WidgetWobbeln::setParameterPixmap() PIX :w:%i h:%i",pix.rect().size().width(),pix.rect().size().height());
          #endif
        p.fillRect( this->rect(), sweepData.colorhintergrund);
      }else{
         qDebug("2 WidgetWobbeln::setParameterPixmap()");
        int x, x1, x2;
        int y;
        int w;
        int h1, h2, h3, h4, h5;
        double hd = 0.9;
        double h5db;
        int r,g,b,a;
        bool boben = TRUE, bunten = TRUE;
  
        //Get original size
        QRect rect = this->rect();
        //und zuerst einmal mit dem gueltigen Kintergrund fuellen
        p.fillRect( this->rect(), sweepData.colorhintergrund);
        //auslesen des ganzen Rechteckes
        x = rect.x();
        y = rect.y();
        #ifdef PAINTGRAF_DEBUG
           qDebug("WidgetWobbeln::setParameterPixmap() RECT() :w:%i h:%i",x,y);
        #endif
        w = rect.width();
        h5 = rect.height();
        //erste Spalte nach Beschriftung links
        x1 = widthBorder;
        //zweite Spalte vor Beschriftung rechts
        x2 = w - widthBorder;
        //dB Y-Achse aus den Daten lesen
        //db Teilung auf der Y-Achse berechnen
        ydisplayteilung_log = (ymax - ymin) / 10.0;
        //Pixel pro 5dB Einheit errechnen
        h5db = (double(h5) - (double(upperBorder) * 2.0)) / (double(ydisplayteilung_log) * 2.0);
        //Zeilen festlegen
        //h1 Zeile nach der Beschriftung oben
        h1 = upperBorder;
        //h2 Zeile bei + 5dB
        h2 = h1 + int(round(((ymax - sweepData.safehlermax) / 5.0) * h5db));
        //h3 Zeile bei -60 dB
        h3 = h1 + int(round(((ymax - sweepData.safehlermin) / 5.0) * h5db));
        //h4 Zeile vor der Beschriftung unten
        h4 = h5 - upperBorder;
        //Test ob obere Abdunklung groesser 5 dB eingeblendet werden soll
        if(h2 <= h1)boben = FALSE;
        //Test ob untere Abdunklung ab -60 dB eingeblendet werden soll
        if(h3 >= h4)bunten = FALSE;
        //Abdunklung einstellen
        QColor chd = sweepData.colorhintergrund;
        chd.getRgb(&r,&g,&b,&a);
        chd.setRgb(int(r*hd), int(g*hd), int(b*hd), int(a*hd));
        QRect rect1(x, y, x1, h5);//rechts
        QRect rect2(x2, y, w-x2, h5);//links
        QRect rect3(x1, y, w-x1-x1, h1);//von Oben bis Zeile h1
        QRect rect4(x1, y + h4, w-x1-x1, h5 - h4);//von Zeile h4 bis h5
        QRect rect5(x1, y + h2, w-x1-x1, h3 - h2);//von Zeile h2 (5dB) bis h3 (-60dB)
        QRect rect6(x1, y + h1, w-x1-x1, h2 - h1);//von Zeile h1 bis h2
        QRect rect7(x1, y + h3, w-x1-x1, h4 - h3);//von Zeile h3 bis h4
        p.fillRect( rect1, sweepData.colorhintergrund);
        p.fillRect( rect2, sweepData.colorhintergrund);
        p.fillRect( rect3, sweepData.colorhintergrund);
        p.fillRect( rect4, sweepData.colorhintergrund);
        p.fillRect( rect5, sweepData.colorhintergrund);
        if(boben)p.fillRect(rect6, chd);
        if(bunten)p.fillRect(rect7, chd);
      }
    }else{
      //No SAV background without gray Areas
      #ifdef PAINTGRAF_DEBUG
            qDebug("3 WidgetWobbeln::setParameterPixmap()");
      #endif
      p.fillRect(this->rect(), sweepData.colorhintergrund);
    }
    p.end();
  }
}

void SweepCurveWidget::refreshCurve()
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::refreshCurve()");
  #endif
  QPainter p;
  //Labeled get pixmap
  pix = pix1;
  p.begin(&pix);
  //and curve draw
  //wobbeldaten.bhintergrund = FALSE;
  drawCurve(&p);
  p.end();
  //und anzeigen
  update(); //paintEvent produce
}

void SweepCurveWidget::refreshPixmap()
{
  TSweep awobbeldaten;
  #ifdef PAINTGRAF_DEBUG
    qDebug(">>WidgetWobbeln::refreshPixmap()");
  #endif
  //Customize size and delete pixmap
  setParameterPixmap();
  QPainter p;
  p.begin(&pix);
  //label produce
  drawGrid(&p);
  //Curves show in the background when it's all OK
  awobbeldaten = sweepData;
  sweepData = kurve1;
  drawCurve(&p);
  sweepData = kurve2;
  drawCurve(&p);
  sweepData = kurve3;
  drawCurve(&p);
  sweepData = kurve4;
  drawCurve(&p);
  sweepData = kurve5;
  drawCurve(&p);
  sweepData = kurve6;
  drawCurve(&p);

  sweepData = awobbeldaten;
  //Additionally labeled pixmap save
  //before the curve is drawn
  pix1 = pix;
  //curve sign
  drawCurve(&p);
  p.end();
  //and Display
  update();
}

void SweepCurveWidget::paintEvent(QPaintEvent*)
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::paintEvent()");
  #endif
  QPainter painter(this);
  //Labeling and curve copy the Window
  //has the character size is changed
  painter.drawPixmap(0, 0, pix);
}

void SweepCurveWidget::resizeEvent(QResizeEvent*)
{
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::resizeEvent()");
  #endif
  //at PaintEventHandler sign again
  bresize = TRUE;
  //Curve sign new
  refreshPixmap();
}

void SweepCurveWidget::drawGrid(QPainter *p)
{
  int i=0,j=0;
  int xmodulo=2;
  int xshift=10;
  int x=0;
  int y=0;
  int x1=0;
  int y1=0;

  int display_shift;
  double px1; 
  double px2; 
  double py1; 
  double bx;
  double wxanfang; 
  double wxende; 
  double wxbereich; 
  double xRaster=1.0; 
  double me=1.0; 
  double yzeichenfaktor_log = 0.0;
  double yzeichenfaktor_lin = 0.0;
  QString xRasterStr, s, qstr;
  bool zeilenwechsel;
  bool boolrot = FALSE;
  efrqbereich frqbereich;
  QPen penblacksolid = QPen( Qt::black, 0, Qt::SolidLine); 
  QPen pendarkgreensolid = QPen( Qt::darkGreen, 0, Qt::SolidLine);
  QPen penredsolid = QPen( Qt::red, 0, Qt::SolidLine); 
  QPen penblackdot = QPen( Qt::black, 0, Qt::DotLine); 
  QPen penlightgraydot = QPen( Qt::lightGray, 0, Qt::DotLine);
  QPen pencyansolid = QPen( Qt::cyan, 0, Qt::SolidLine);
  QPen penreddot = QPen( Qt::red, 0, Qt::DotLine);
  QPen penhline = QPen( Qt::lightGray, 0, Qt::DashLine);
  
  #ifdef PAINTGRAF_DEBUG
    qDebug("WidgetWobbeln::drawGrid()");
  #endif
  
  
  ydisplayteilung_log = ymax - ymin;
  display_shift = sweepData.display_shift / -10;
  if(display_shift != 0)boolrot = TRUE;
  yswrteilung = sweepData.swraufloesung;//Y-Teilung bei SWR
  breite = this->rect().width() - widthBorder * 2;
//  qDebug("breite %i", breite);
  hoehe = this->rect().height() - upperBorder * 2;
  double zw1 = double(breite) / (sweepData.absende - sweepData.absanfang);
  xSchritt = zw1 * sweepData.schrittfrequenz;
  xAnfang = zw1 * (sweepData.anfangsfrequenz - sweepData.absanfang);
  ySchritt = double(hoehe) / yaufloesung;

  //###################################################################################
  // ANFANG Beschriftung des Y-Bereiches
  //###################################################################################
    p->setPen( penblacksolid );
  // Schrift setzen
  p->setFont( QFont("Sans", 9));
  px1 = widthBorder;
  px2 = breite + widthBorder;
  switch(sweepData.eoperatemode){
    case eSmithDiagramm:
      // Kreise mit db
      if(hoehe >= breite)durchmesser = breite;
      if(hoehe < breite)durchmesser = hoehe;
      p->setPen(penblacksolid);
      int idbmax;
      idbmax = sweepData.antdiagramm.dbmax;
      int idbmin;
      idbmin = sweepData.antdiagramm.dbmin;
      double db_teilung;
      db_teilung = double(idbmax - idbmin) / double(antdia_db_teilung);
      //qDebug()<<db_teilung;
      double w10db;
      w10db = double(durchmesser)/2.0/db_teilung;
      p->setPen(penblacksolid);
      p->drawEllipse ( widthBorder, upperBorder, durchmesser, durchmesser);
      for(i=idbmax, j=0; i>=idbmin; i-=antdia_db_teilung, j++){
        p->setPen(penhline);
        p->drawEllipse ( widthBorder + j*w10db, upperBorder+j*w10db, durchmesser-j*w10db*2, durchmesser-j*w10db*2);
        s.sprintf("%idB",i);
        p->setPen(penblacksolid);
        p->drawText( widthBorder+durchmesser/2-15, upperBorder + j*w10db-3, s);
        //in der Mitte nur einmal Beschriften
        if(i != idbmin) p->drawText( widthBorder+durchmesser/2-15, upperBorder + durchmesser - j*w10db + 13, s);
      }
      // Gradlinien
      p->setPen(penhline);
      for(i=0; i<=360; i+=10){
        x = widthBorder + durchmesser/2 + int(round(sin(double(i)*M_PI/180.0)*durchmesser/2));
        y = upperBorder + durchmesser/2 - int(round(cos(double(i)*M_PI/180.0)*durchmesser/2));
        x1 = widthBorder + durchmesser/2 + int(round(sin(double(i)*M_PI/180.0)*durchmesser/20));
        y1 = upperBorder + durchmesser/2 - int(round(cos(double(i)*M_PI/180.0)*durchmesser/20));
        p->drawLine(x, y, x1, y1);
        s.sprintf("%i",i);
        if(i<=90){
          if(i!=0)p->drawText( x+10, y-10, s);
          if(i==0)p->drawText( x+30, y-10, s);
        }
        if(i>90 and i<=180){
          if(i>90 and i<180)p->drawText( x+10, y+10, s);
          if(i==180){
            p->drawText( x+30, y+20, s);
            p->drawText( x-40, y+20, s);
          }
        }
        if(i>180 and i<=270){
          if(i>180 and i<270)p->drawText( x-30, y+10, s);
          if(i==270){
            p->drawText( x-30, y+10, s);
          }
        }
        if(i>270 and i<=360){
          if(i>270 and i<360)p->drawText( x-30, y-10, s);
          if(i==360){
            if(i>270 and i<360)p->drawText( x-30, y-10, s);
            if(i==360)p->drawText( x-40, y-10, s);
          }
        }
      }
      break;
    case eSpectrumAnalyser:
    case eSpectrumDisplayShift:
    case eGainSweep:
      if(!sweepData.linear1 && !sweepData.linear2){ // logaritmische Beschriftung
        //py1 ist die Pixelanzahl pro 10dB
        py1 = double(hoehe) / (ydisplayteilung_log / 10.0);
        //Farbe einstellen abhaengig von SHIFT-Einstellung
        if((sweepData.dbshift1 != 0.0) or (sweepData.dbshift2 != 0.0))boolrot = TRUE;
        if(boolrot){
          p->setPen( penredsolid );
        }else{
          p->setPen( penblacksolid );
        }
        //bei log. Messkopf Messeinheit anzeigen  linker Rand rechtbuendig
        if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
          s = "dBm";
        }else{
          s = "dB";
        }
        p->drawText(1, int(round(py1*0.5))+ upperBorder, s);
        p->drawText(int(round(px2))+5, int(round(py1*0.5))+ upperBorder, s);
        //Linen und Beschriftung beginnend bei ymax(obere dB-Zahl) bis ymin(untere dB-Zahl)
        for(i=int(ymax/10.0), j=0; i>=int(ymin/10.0); i--, j++)
        {
          //Farbe einstellen abhaengig von SHIFT-Einstellung
          if(boolrot){
            p->setPen(penredsolid);
          }else{
            p->setPen(penblacksolid);
          }
          //dB Zahlen generieren rechtsbuendig
          s.sprintf("%3i", (i-display_shift)*10);
          p->drawText(1, int(round(py1*j))+ upperBorder + 3,s);
          // nur bei log-Sonde beschriften
          //an den rechten Rand schreiben linksbuendig
          p->drawText(int(round(px2))+3, int(round(py1*j))+ upperBorder + 3,s);
          //Null-Linie als anderen Strich darstellen
          if((i-display_shift) == 0){
            p->setPen(penblackdot);
          }else{
            p->setPen(penlightgraydot);
          }
          //Abschliessend noch die Linie zeichnen
          //qDebug("%3i %i", i*10, int(round(py1*j))+ ObererRand);
          p->drawLine(int(round(px1)), int(round(py1*j))+ upperBorder,
                      int(round(px2)), int(round(py1*j))+ upperBorder);
        }
        p->setPen(penhline);
        yzeichenfaktor_log = hoehe / (ymin - ymax);
        if(sweepData.hline != 0.0){
          py1 = (sweepData.hline - ymax - sweepData.display_shift) * yzeichenfaktor_log + upperBorder;
          if(py1 < (hoehe + upperBorder))p->drawLine(int(round(px1)), int(py1), int(round(px2)), int(py1));
          s.sprintf("%0.2fdB", sweepData.hline);
          p->drawText(int(round(px1)), int(py1), s);
          p->drawText(int(round(px2))-60, int(py1)+10, s);
        }
      }else{ //Linear Beschriftung
        py1 = hoehe / ydisplayteilung_lin;
        for(i=0; i<(ydisplayteilung_lin + 1); i++)
        {
          //lineare Beschriftung
          p->setPen(pendarkgreensolid);
          p->drawText(1, int(round(py1*0.5)+ upperBorder + 3.0)," dB");
          bx = double((11 - i) * 0.1);
          //qDebug("%f",bx);
          bx = 20.0 * log10(bx);
          //qDebug("%f",bx);
          if(bx > -10.0){
            s.sprintf("%1.2f", bx);
          }else{
            s.sprintf("%1.1f", bx);
          }
          if(bx < -99.9){
            s = tr("Inf","");
          }  
          //bx = bx / 20.0;
          //bx = pow(10.0 , bx);
          //qDebug("pow: %f",bx);
          //an den linken Rand schreiben
          p->drawText(1, int(round(py1*i))+ upperBorder + 3,s);
          //lineare Beschriftung
          s.sprintf("%1.1f", double((11 - i) * 0.1));
          //an den rechten Rand schreiben
          p->setPen(penblacksolid);
          p->drawText(int(round(px2))+3, int(round(py1*i))+ upperBorder + 3,s);
          p->setPen(penlightgraydot);
          //Nulllinie fest an der 1.0 Stelle
          if(i == 1){
            p->setPen(penblackdot);
          }else{
            p->setPen(penlightgraydot);
          }
          p->drawLine(int(round(px1)), int(round(py1*i))+ upperBorder,
                     int(round(px2)), int(round(py1*i))+ upperBorder);
        }
        p->setPen(penhline);
        yzeichenfaktor_lin = hoehe / (-10.0 - 1.0);
        if(sweepData.hline != 0.0){
          bx = sweepData.hline / 20.0;
          bx = pow(10.0, bx);
          //qDebug("pow: %f",bx);
          py1 = (bx - 1.0) * 10.0 * yzeichenfaktor_lin + upperBorder - yzeichenfaktor_lin;
          //qDebug("py1: %f",py1);
          //qDebug("hoehe: %i",hoehe);
          //qDebug("yzeichenfaktor_lin: %f",yzeichenfaktor_lin);
          if(py1 < (hoehe + upperBorder))p->drawLine(int(round(px1)), int(py1), int(round(px2)), int(py1));
          s.sprintf("%0.2fdB", sweepData.hline);
          p->drawText(int(round(px1)), int(py1), s);
          p->drawText(int(round(px2))-60, int(py1)+10, s);
        }
      }
      break;
    case eSWR:
    case eSWRfeeder:
      //2 kanalige SWR-Anzeige
      py1 = hoehe / double(yswrteilung*10);       //Einteilung Y
      p->setPen( penblacksolid);
      p->drawText(1, hoehe + upperBorder + 20,"SWR");
      //Messeinheit anzeigen  rechter Rand
      p->drawText(int(round(px2))+5, hoehe + upperBorder + 20,"SWR");
      if(yswrteilung > 6){
        xmodulo = 4;
      }else{
        xmodulo = 2;
      }
      if(yswrteilung > 8){
        xmodulo = 5;
      }
      for(i=0; i< (yswrteilung*10+1); i++)
      {
        //modulo 2
//        qDebug(((i+1) % xmodulo));
        if((i % xmodulo) == 0){
          //SWR Beschriftung aller 2 Teilungen Y-Achse
          s.sprintf("%1.1f", double(((yswrteilung*10 - i) * 0.1)) + 1.0);
          //an den linken Rand schreiben
          p->setPen(penblacksolid);
          p->drawText(1, int(round(py1*i))+ upperBorder + 3,s);
          //SWR Beschriftung
          s.sprintf("%1.1f", double(((yswrteilung*10 - i) * 0.1)) + 1.0);
          p->setPen(penblacksolid);
          p->drawText(int(round(px2))+3, int(round(py1*i))+ upperBorder + 3,s);
          p->setPen( penlightgraydot);
          p->drawLine(int(round(px1)), int(round(py1*i))+ upperBorder,
                     int(round(px2)), int(round(py1*i))+ upperBorder);
        }
      }
      break;
    case eImpedance:
      //2 kanalige SWR-Anzeige
      py1 = hoehe / 200.0;       //Einteilung Y
      p->setPen(penblacksolid);
      p->drawText(1, hoehe + upperBorder + 20,"Z(Ohm)");
      //Messeinheit anzeigen  rechter Rand
      p->drawText(int(round(px2))-15, hoehe + upperBorder + 20,"Z(Ohm)");
      for(i=0; i <= 200; i++)
      {
        //modulo 2
//        qDebug(((i+1) % xmodulo));
        if((i % 10) == 0){
          //Impedanz Beschriftung aller 10 Ohm an der Y-Achse
          s.sprintf("%i",  200 - i);
          //an den linken Rand schreiben
          p->setPen(penblacksolid);
          p->drawText(1, int(round(py1*i))+ upperBorder + 3,s);
          p->drawText(int(round(px2))+3, int(round(py1*i))+ upperBorder + 3,s);
          if(i == 150){
            p->setPen(penblackdot);
          }else{
            p->setPen(penlightgraydot);
          }
          p->drawLine(int(round(px1)), int(round(py1*i))+ upperBorder,
                     int(round(px2)), int(round(py1*i))+ upperBorder);
        }
      }
      break;
  }
  
  //###################################################################################
  // ENDE Beschriftung des Y-Bereiches
  //###################################################################################
  //###################################################################################
  // Anfang Beschriftung des X-Bereiches
  //###################################################################################
  if(sweepData.eoperatemode != eSmithDiagramm){
  //Festlegung der X-Achseneinteilung
  wxanfang = sweepData.anfangsfrequenz;
  wxende = wxanfang + (double(sweepData.cntpoints) - 1.0) * sweepData.schrittfrequenz;
  wxbereich = (wxende - wxanfang) / 5.0;
  if((wxbereich <=           2.5) & (wxbereich >           1.0))xRaster =           1.0;
  if((wxbereich <=           5.0) & (wxbereich >           2.5))xRaster =           2.5;
  if((wxbereich <=          10.0) & (wxbereich >           5.0))xRaster =           5.0;
  if((wxbereich <=          25.0) & (wxbereich >          10.0))xRaster =          10.0;
  if((wxbereich <=          50.0) & (wxbereich >          25.0))xRaster =          25.0;
  if((wxbereich <=         100.0) & (wxbereich >          50.0))xRaster =          50.0;
  if((wxbereich <=         250.0) & (wxbereich >         100.0))xRaster =         100.0;
  if((wxbereich <=         500.0) & (wxbereich >         250.0))xRaster =         250.0;
  if((wxbereich <=        1000.0) & (wxbereich >         500.0))xRaster =         500.0;
  if((wxbereich <=        2500.0) & (wxbereich >        1000.0))xRaster =        1000.0;
  if((wxbereich <=        5000.0) & (wxbereich >        2500.0))xRaster =        2500.0;
  if((wxbereich <=       10000.0) & (wxbereich >        5000.0))xRaster =        5000.0;
  if((wxbereich <=       25000.0) & (wxbereich >       10000.0))xRaster =       10000.0;
  if((wxbereich <=       50000.0) & (wxbereich >       25000.0))xRaster =       25000.0;
  if((wxbereich <=      100000.0) & (wxbereich >       50000.0))xRaster =       50000.0;
  if((wxbereich <=      250000.0) & (wxbereich >      100000.0))xRaster =      100000.0;
  if((wxbereich <=      500000.0) & (wxbereich >      250000.0))xRaster =      250000.0;
  if((wxbereich <=     1000000.0) & (wxbereich >      500000.0))xRaster =      500000.0;
  if((wxbereich <=     2500000.0) & (wxbereich >     1000000.0))xRaster =     1000000.0;
  if((wxbereich <=     5000000.0) & (wxbereich >     2500000.0))xRaster =     2500000.0;
  if((wxbereich <=    10000000.0) & (wxbereich >     5000000.0))xRaster =     5000000.0;
  if((wxbereich <=    25000000.0) & (wxbereich >    10000000.0))xRaster =    10000000.0;
  if((wxbereich <=    50000000.0) & (wxbereich >    25000000.0))xRaster =    25000000.0;
  if((wxbereich <=   100000000.0) & (wxbereich >    50000000.0))xRaster =    50000000.0;
  if((wxbereich <=   250000000.0) & (wxbereich >   100000000.0))xRaster =   100000000.0;
  if((wxbereich <=   500000000.0) & (wxbereich >   250000000.0))xRaster =   250000000.0;
  if((wxbereich <=  1000000000.0) & (wxbereich >   500000000.0))xRaster =   500000000.0;
  if((wxbereich <=  2500000000.0) & (wxbereich >  1000000000.0))xRaster =  1000000000.0;
  if((wxbereich <=  5000000000.0) & (wxbereich >  2500000000.0))xRaster =  2500000000.0;
  if((wxbereich <= 10000000000.0) & (wxbereich >  5000000000.0))xRaster =  5000000000.0;
  if((wxbereich <= 25000000000.0) & (wxbereich > 10000000000.0))xRaster = 10000000000.0;
  xRasterStr = "Hz";
  me = 1.0;
  frqbereich = hz;
  if(wxende >= 1000.0){
    xRasterStr = "kHz";
    me = 1000.0;
    frqbereich = khz;
  }  
  if(wxende >= 1000000.0){
    xRasterStr = "MHz";
    me = 1000000.0;
    frqbereich = mhz;
  } 
  if(wxende >= 1000000000.0){
    xRasterStr = "GHz";
    me = 1000000000.0;
    frqbereich = ghz;
  } 
  //ganz linke Linie ziehen
  p->setPen(penlightgraydot);
  p->drawLine(widthBorder, upperBorder, widthBorder, hoehe + upperBorder);
  //ganz rechts Linie ziehen
  p->drawLine(breite + widthBorder, upperBorder, breite + widthBorder, hoehe + upperBorder);
  if(boolrot){
    p->setPen(penredsolid);
  }else{
    p->setPen(penblacksolid);
  }
  p->drawText(2, 13, xRasterStr);
  i = 1;
  if(xRaster < wxanfang)i = int(round( wxanfang / xRaster)) + 1;
  //Beschriftung x-Achse + Linien
  //erste Beschriftung oben
  zeilenwechsel = TRUE;
  while( (xRaster * i) < wxende){
    x = int(round((((xRaster * i) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    //je nach laenge des Strings den Text etwas nach links schieben
    QLocale::setDefault(QLocale(QLocale::German, QLocale::Germany));
    //Grundeinstellung
    switch(frqbereich){
      case hz:
        xshift = 10;
        s.sprintf("%3.0f", xRaster*i/me);
        break;
      case khz:
        xshift = 10;
        s.sprintf("%3.1f", xRaster*i/me);
        if((xRaster) < 1000.0){
          // Bereich kleiner 1MHz
          xshift = 15;//Zahl 30px weiter links
          s.sprintf("%3.2f", xRaster*i/me);
        }  
        if((xRaster) < 100.0){
          // Bereich kleiner 100kHz
          xshift = 20;//Zahl 30px weiter links
          s.sprintf("%3.3f", xRaster*i/me);
        }  
        if((xRaster) < 10.0){
          // Bereich kleiner 10kHz
          xshift = 25;//Zahl 30px weiter links
          s.sprintf("%3.4f", xRaster*i/me);
        }  
        break;
      case mhz:
        xshift = 10;
        s.sprintf("%3.1f", xRaster*i/me);
        if((xRaster) < 1000000.0){
          // Bereich kleiner 1MHz
          xshift = 15;//Zahl 30px weiter links
          s.sprintf("%3.2f", xRaster*i/me);
        }  
        if((xRaster) < 100000.0){
          // Bereich kleiner 100kHz
          xshift = 20;//Zahl 30px weiter links
          s.sprintf("%3.3f", xRaster*i/me);
        }  
        if((xRaster) < 10000.0){
          // Bereich kleiner 10kHz
          xshift = 25;//Zahl 30px weiter links
          s.sprintf("%3.4f", xRaster*i/me);
        }  
        if((xRaster) < 1000.0){
          // Bereich kleiner 1kHz
          xshift = 30;//Zahl 30px weiter links
          s.sprintf("%3.5f", xRaster*i/me);
        }  
        if((xRaster) < 100.0){
          // Bereich kleiner 100Hz
          xshift = 35;//Zahl 30px weiter links
          s.sprintf("%3.6f", xRaster*i/me);
        }
        break;
      case ghz:
        s.sprintf("%3.1f", xRaster*i/me);
        xshift = 10;//Zahl 30px weiter links
        if((xRaster) < 10000000000.0){
          // Bereich kleiner 1GHz
          xshift = 15;//Zahl 30px weiter links
          s.sprintf("%3.2f", xRaster*i/me);
        }  
        if((xRaster) < 1000000000.0){
          // Bereich kleiner 100MHz
          xshift = 20;//Zahl 30px weiter links
          s.sprintf("%3.3f", xRaster*i/me);
        }  
        if((xRaster) < 100000000.0){
          // Bereich kleiner 10MHz
          xshift = 25;//Zahl 30px weiter links
          s.sprintf("%3.4f", xRaster*i/me);
        }  
        if((xRaster) < 10000000.0){
          // Bereich kleiner 1MHz
          xshift = 30;//Zahl 30px weiter links
          s.sprintf("%3.5f", xRaster*i/me);
        }  
        if((xRaster) < 100000.0){
          // Bereich kleiner 100kHz
          xshift = 35;//Zahl 30px weiter links
          s.sprintf("%3.6f", xRaster*i/me);
        }  
        if((xRaster) < 10000.0){
          // Bereich kleiner 10kHz
          xshift = 40;//Zahl 30px weiter links
          s.sprintf("%3.7f", xRaster*i/me);
        }  
        if((xRaster) < 1000.0){
          // Bereich kleiner 1kHz
          xshift = 45;//Zahl 30px weiter links
          s.sprintf("%3.8f", xRaster*i/me);
        }  
        if((xRaster) < 100.0){
          // Bereich kleiner 100Hz
          xshift = 50;//Zahl 30px weiter links
          s.sprintf("%3.9f", xRaster*i/me);
        }
        break;
    }
    if(boolrot){
      p->setPen(penredsolid);
    }else{
      p->setPen(penblacksolid);
    }
    if(zeilenwechsel){
      p->drawText(x-xshift, 28, s);
      zeilenwechsel = FALSE;
    }else{  
      p->drawText(x-xshift, 13, s);
      zeilenwechsel = TRUE;
    }
    p->setPen(penlightgraydot);
    p->drawLine(x, upperBorder, x, hoehe + upperBorder);
    i++;
  }
  if(boolrot){
    p->setPen(penredsolid);
  }else{
    p->setPen(penblacksolid);
  }
  xRasterStr = "Hz";
  if(xRaster >= 1000.0){
    xRaster /= 1000.0;
    xRasterStr = "kHz";
  }
  if(xRaster >= 1000.0){
    xRaster /= 1000.0;
    xRasterStr = "MHz";
  }
  qstr = tr("X-axis:","");
  s.sprintf(" %1.1f ", xRaster);
  s = qstr + s + xRasterStr;
  if(boolrot){
    p->setPen(penredsolid);
  }else{
    p->setPen(penblacksolid);
  }
  p->drawText(int(px2/2.0)-50, hoehe + upperBorder + 20,s);
  if(boolrot){
    p->setPen(penredsolid);
    s = tr("Shift active","");
    p->drawText(widthBorder + 20, hoehe + upperBorder + 20,s);
    p->drawText(breite - 80, hoehe + upperBorder + 20,s);
  }
  //###################################################################################
  // ENDE Beschriftung des X-Bereiches
  //###################################################################################
  //###################################################################################
  // Frequenzmarken 
  //###################################################################################
  p->setPen(pencyansolid);
  if(wfmarken.b160m){
    x = int(round((((wfmarken.f1160m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f2160m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st160m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b80m){
    x = int(round((((wfmarken.f180m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f280m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st80m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b40m){
    x = int(round((((wfmarken.f140m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f240m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st40m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b30m){
    x = int(round((((wfmarken.f130m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f230m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st30m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b20m){
    x = int(round((((wfmarken.f120m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f220m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st20m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b17m){
    x = int(round((((wfmarken.f117m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f217m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st17m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b15m){
    x = int(round((((wfmarken.f115m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f215m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st15m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b12m){
    x = int(round((((wfmarken.f112m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f212m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st12m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b10m){
    x = int(round((((wfmarken.f110m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f210m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st10m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b6m){
    x = int(round((((wfmarken.f16m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f26m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st6m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.b2m){
    x = int(round((((wfmarken.f12m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f22m) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.st2m);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.bu1){
    x = int(round((((wfmarken.f1u1) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f2u1) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.stu1);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.bu2){
    x = int(round((((wfmarken.f1u2) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f2u2) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.stu2);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  if(wfmarken.bu3){
    x = int(round((((wfmarken.f1u3) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    x1 = int(round((((wfmarken.f2u3) - wxanfang) / (wxende - wxanfang))* breite) - 1 + widthBorder);
    if((x > widthBorder)and(x < (breite+widthBorder)))
      p->drawLine(x, upperBorder, x, hoehe + upperBorder);
      if((x1 - x) > 50){
        p->drawText(x-20+(x1-x)/2, (hoehe + upperBorder)/2 , wfmarken.stu3);
      }	
    if((x1 > widthBorder)and(x1 < (breite+widthBorder)))
      p->drawLine(x1, upperBorder, x1, hoehe + upperBorder);
  }  
  //###################################################################################
  // Ende Frequenzmarken 
  //###################################################################################
}else{

}
}


void SweepCurveWidget::drawCurve(QPainter *p)
{
//Variablen

  int i,
  k,
  j1,
  j2,
  j3,
  x, 
  x1=0, 
  y=0, 
  y1=0,
  y3=0,
  y4=0,
  mx1[5]={0,0,0,0,0},
  mx2[5]={0,0,0,0,0},
  my1[5]={0,0,0,0,0},
  my2[5]={0,0,0,0,0},
  xm1[5]={0,0,0,0,0},
  xm2[5]={0,0,0,0,0},
  kx [5]={0,0,0,0,0},
  kx1[5]={0,0,0,0,0},
  kx2[5]={0,0,0,0,0},
  kx3[5]={0,0,0,0,0},
  ky1[5]={0,0,0,0,0},
  ky2[5]={0,0,0,0,0},
  kw1[5]={0,0,0,0,0},
  kw2[5]={0,0,0,0,0},
  kw3[5]={0,0,0,0,0},
  db3x1=0,
  db3x2=0,
  maxk1,
  maxk2,
  mink1,
  mink2,
  maxk1f=0,
  maxk2f=0,
  mink1f=0,
  mink2f=0;
  double db3xa, 
  db3xb,
  db3ya,
  db3yb,
  bandbreite3db=0.0,
  bandbreite6db=0.0,
  px1=0.0,
  py1,
  py2, 
  y1faktor,
  y2faktor,
  y1shift,
  y2shift,
  display_shift,
  minswr, //fuer SWR neu
  maxswr, //fuer SWR neu
  afrequenz=1000000.0,
  uricht=0.0,
  w1,
  w2,
  w3,
  swv=1.0,
  maxdbk1=-100.0,
  mindbk1=20.0,
  maxdbk2=-100.0,
  mindbk2=20.0;

  QString xRasterStr, s, qstr;

  double s_ant;
  double a;
  double r_tx;
  double r_ant;
  //Kabellaengendaten
  double a_100 = 1.0;
  double laenge = 10.0;
  //Kabellaengendaten
  double yzeichenfaktor_log = 0.0;
  double yzeichenshift_log = 0.0;
  double yzeichenfaktor_lin = 0.0;
  double yzeichenshift_lin = 0.0;
  double bx;
  bool bkurvezeichnen;
  bool bmouse = TRUE;
  
  double db3xai = 0.0;
  double db3xbi = 0.0;
  double db3yai = 0.0;
  double db3ybi = 0.0;
  int db3x1i = 0;
  int db3x2i = 0;
  double xfrequenz;
  double grad1;
  double ateiler = 32.0;

  QPen penreddot = QPen( Qt::red, 0, Qt::DotLine);
  QPen penreddash = QPen( Qt::red, 0, Qt::DashLine);
  QPen penbluedash = QPen( Qt::blue, 0, Qt::DashLine);
  QPen pendarkgreendot = QPen( Qt::darkGreen, 0, Qt::DotLine);
  QPen pendarkgreendash = QPen( Qt::darkGreen, 0, Qt::DashLine);
  QPen pendarkbluesolid = QPen( Qt::darkBlue, 0, Qt::SolidLine);
  QPen pendarkredsolid = QPen( Qt::darkRed, 0, Qt::SolidLine);
  QPen penbluesolid = QPen( Qt::blue, 0, Qt::SolidLine);
  
  #ifdef PAINTGRAF_DEBUG
  //qDebug("WidgetWobbeln::drawKurve()");
  #endif
  a_100 = sweepData.a_100;
  laenge = sweepData.kabellaenge;
  double zw1 = double(breite) / (sweepData.absende - sweepData.absanfang);
  xSchritt = zw1 * sweepData.schrittfrequenz;
  xAnfang = zw1 * (sweepData.anfangsfrequenz - sweepData.absanfang);
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
//*************************************************************************************
  //###################################################################################
  // ANFANG ermitteln der max- und min-Werte
  //###################################################################################
  y1faktor = sweepData.faktor1;
  y1shift = sweepData.shift1;
  y2faktor = sweepData.faktor2;
  y2shift = sweepData.shift2;
  display_shift = sweepData.display_shift;
  afrequenz = sweepData.anfangsfrequenz;
  maxk1 = 0;
  maxk2 = 0;
  minswr = 99.9;
  maxswr = 1.0;
  //Messdaten mit 10bit laenge
  mink1 = 1023;
  mink2 = 1023;
  maxdbk1 = -100.0;
  mindbk1 = 20.0;
  maxdbk2 = -100.0;
  mindbk2 = 20.0;
  //Neue Berechnung ab V3.05
  yzeichenfaktor_log = hoehe / (ymin - ymax);
  yzeichenshift_log = (ymax * yzeichenfaktor_log) * -1.0;
  yzeichenfaktor_lin = hoehe / (-100.0 - 10.0);
  yzeichenshift_lin = (10.0 * yzeichenfaktor_lin) * -1.0;
  //qDebug("y1faktor %f",y1faktor);
  //qDebug("y1shift  %f",y1shift);
  
  //###################################################################################
  // ANFANG Kurven darstellen
  //###################################################################################
  if(sweepData.mess.daten_enable){
    //##########################################################
    // Alle Gemeinsamkeiten erledigen
    //##########################################################
    // alle Listen loeschen aber nur wenn es die Onlinekurve ist und kein
    // Hintergrund
    if(!sweepData.bhintergrund)emit multiEditDelete();
    switch (sweepData.eoperatemode){
      case eSmithDiagramm:
        //Stiftfarbe Kanal1 setzen
        p->setPen( sweepData.penkanal1 );
        //Daten aus Struct umspeichern
        int adbegin;
        adbegin = sweepData.antdiagramm.adbegin;
        int adend;
        adend = sweepData.antdiagramm.adend;
        int idbmin;
        idbmin = sweepData.antdiagramm.dbmin;
        int idbmax;
        idbmax = sweepData.antdiagramm.dbmax;
        int diabegin;//Gradzahl Diagramm Beginn
        diabegin = sweepData.antdiagramm.diabegin;
        int diaend;//Gradzahl Diagramm Ende
        diaend = sweepData.antdiagramm.diaend;
        //aus (gradende - gradbegin) / (adwertgradende - adwertgradbegin)
        //grad1 ist der gradwert pro adschritt
        grad1 = double(diaend - diabegin) / double(adend - adbegin);
        kurve.clear();
        double db_bereich;
        db_bereich = double(idbmax-idbmin);//dB Bereich errechnen
        double grad2bogen;
        grad2bogen = M_PI/180.0;//Umrechnung von Grad auf Bogenmass
        double calgradcorr;//Winkelkorrektur durch adbegin und diabegin
        calgradcorr = (diabegin-adbegin*360/1023)*grad2bogen;
        for(i=adbegin; i<=adend; i++){//von Spannung als AD-Wert fuer unterstes Gradmass
                                          //bis Spannung als AD-Wert fuer oberstes Gradmass
          if(sweepData.mess.k1[i]!=0){  //alle ungueltigen Messwerte nichtberuecksichtigen
            //dBm Wert berechnen aus der Wattmeterkalibrierung
            py1 = sweepData.mess.k1[i] * sweepData.mfaktor1 + sweepData.mshift1;
            //Ueberschreitung des maximalen dB Diagrammwertes
            if(int(py1) > idbmax){
              py1 = double(idbmax);
            }
            //Unterschreitung des minimalen dB Diagrammwertes
            if(int(py1) < idbmin){
              py1 = double(idbmin);
            }
            //Zeigerlaenge des dBm Wertes mit dem Grafikradius berechnen
            py1 = (py1 - double(idbmin))/db_bereich * double(durchmesser/2);
            //Zeiger in x und y Punkte berechnen
            //Zeigeranfang ist Kreismittelpunkt; Zeigerende ist x,y Punkt
            x = widthBorder + durchmesser/2 +
                int(round(sin(calgradcorr+grad1*double(i)*grad2bogen)*py1));
            y = upperBorder + durchmesser/2 -
                int(round(cos(calgradcorr+grad1*double(i)*grad2bogen)*py1));
            //errechnete Werte in einem Polygon abspeichern
            kurve.append(QPoint(x,y));
          }
        }
        p->drawPolyline(kurve);
        break;
      case eSpectrumAnalyser:
      case eSpectrumDisplayShift:
      case eGainSweep:
      //##########################################################
      // Betriebsart Wobbeln
      //##########################################################
      //##########################################################
      // Zuerst Kanal 1 
      //##########################################################
        if(sweepData.bkanal1){
          // Pen setzen
          p->setPen( sweepData.penkanal1 );
          kurve.clear();
          for(i=0; i < sweepData.cntpoints; i++){
            bkurvezeichnen = TRUE;
            x = int(round(i * xSchritt) + widthBorder) + (int)xAnfang;
            if( x < widthBorder)bkurvezeichnen = FALSE;
            if( x > (widthBorder + breite))bkurvezeichnen = FALSE;
            //x1 = int(round((i+1) * xSchritt) + LinkerRand) + (int)xAnfang;
            //if( x1 < LinkerRand)bkurvezeichnen = FALSE;
            //if( x1 > (LinkerRand + breite))bkurvezeichnen = FALSE;
            //Neue Berechnung ab V3.05
            if(sweepData.linear1){
              //dB ausrechnen LINEAR
              py1 = sweepData.mess.k1[i] * y1faktor + y1shift;
              //py2 = wobbeldaten.mess.k1[i+1] * y1faktor + y1shift;
              py1 = py1 * yzeichenfaktor_lin + yzeichenshift_lin;
              //py2 = py2 * yzeichenfaktor_lin + yzeichenshift_lin;
            }else{
              //dB ausrechnen LOG
              py1 = (sweepData.mess.k1[i] * y1faktor + y1shift) -
                    getCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * i);
              //py2 = (wobbeldaten.mess.k1[i+1] * y1faktor + y1shift) -
                //    getkalibrierwertk1(afrequenz + wobbeldaten.schrittfrequenz * (i+1));
              //Zuerst dB-Shift hinzu
              py1 = py1 + sweepData.dbshift1;
              //py2 = py2 + wobbeldaten.dbshift1;
              //qDebug("py1+dbshift1 %f",py1);
              //Pegelkorrektur vom SAV
              if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                py1 = korrsavpegel(py1) - getSavCalWorth(afrequenz + sweepData.schrittfrequenz * i);
                //py2 = korrsavpegel(py2) - getsavcalwert(afrequenz + wobbeldaten.schrittfrequenz * (i+1));
              }
              //Y-Position im Windows errechen
              py1 = py1 * yzeichenfaktor_log + yzeichenshift_log;
              //py2 = py2 * yzeichenfaktor_log + yzeichenshift_log;
              //qDebug("py1 %f",py1);
            }
            // Oberen Rand hinzu addieren
            y = int(round(py1) + upperBorder);
            //y1 = int(round(py2) + ObererRand);
            //qDebug("y %i",y);
            //auf extremwerte reagieren Linie soll Anzeige nicht nach oben oder unten verlassen
            if(y > (hoehe + upperBorder))y = hoehe + upperBorder;
            if(y < upperBorder)y = upperBorder;
            //if(y1 > (hoehe + ObererRand))y1 = hoehe + ObererRand;
            //if(y1 < ObererRand)y1 = ObererRand;
            //Linie zeichnen
            //if(bkurvezeichnen)p->drawLine(x, y, x1, y1);
            if(bkurvezeichnen)kurve.append(QPoint(x,y));
            //Max Min Punkt 1
            if(sweepData.linear1){
              py1 = sweepData.mess.k1[i] * y1faktor + y1shift;
            }else{
              py1 = (sweepData.mess.k1[i] * y1faktor + y1shift) -
                    getCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * i);
              if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                py1 = korrsavpegel(py1) - getSavCalWorth(afrequenz + sweepData.schrittfrequenz * i);
              }
            }
            if(py1 > maxdbk1){
              maxdbk1 = py1;
              maxk1f = i;//passende Frequenz dazu
            }
            //minimum Kanal1 ermitteln
            if(py1 < mindbk1){
              mindbk1 = py1;
              mink1f = i;//passende Frequenz dazu
            }
            //Max Min Punkt 2
            //if(wobbeldaten.linear1){
            //  py1 = wobbeldaten.mess.k1[i+1] * y1faktor + y1shift;
            //}else{
            //  py1 = (wobbeldaten.mess.k1[i+1] * y1faktor + y1shift) -
            //        getkalibrierwertk1(afrequenz + wobbeldaten.schrittfrequenz * i+1);
            //  if((wobbeldaten.ebetriebsart == espektrumanalyser) or (wobbeldaten.ebetriebsart == espekdisplayshift)){
            //    py1 = korrsavpegel(py1) - getsavcalwert(afrequenz + wobbeldaten.schrittfrequenz * (i+1));
            //  }
            //}
            //maximum Kanal1 ermitteln
            //if(py1 > maxdbk1){
            //  maxdbk1 = py1;
            //  maxk1f = i+1;//passende Frequenz dazu
            //}
            //minimum Kanal1 ermitteln
            //if(py1 < mindbk1){
            //  mindbk1 = py1;
            //  mink1f = i+1;//passende Frequenz dazu
            //}
            //Anfangswerte setzen
            for(k=0;k<5;k++){
              if(mouseposx1[k] != mouseposx[k]){
                if(i == 0){
                  xm1[k] = widthBorder;
                  mx1[k] = x;
                  my1[k] = y;
                  kx[k] = i;
                  ky1[k] = sweepData.mess.k1[i];
                  kxm1[k] = kx[k];
                }
                //neue Werte setzen
                xm2[k] = abs(x - mouseposx[k]);
                if(xm2[k] < xm1[k]){
                  xm1[k] = xm2[k];
                  mx1[k] = x;
                  my1[k] = y;
                  kx[k] = i;
                  ky1[k] = sweepData.mess.k1[kx[k]];
                  kxm1[k] = kx[k];
                }
                /*
                xm2[k] = abs(x1 - mouseposx[k]);
                if(xm2[k] < xm1[k]){
                  xm1[k] = xm2[k];
                  mx1[k] = x1;
                  my1[k] = y1;
                  kx[k] = i + 1;
                  ky1[k] = wobbeldaten.mess.k1[kx[k]];
                  kxm1[k] = kx[k];
                }
                */
              }else{
                mx1[k] = int(round(kxm1[k] * xSchritt) + widthBorder);
                py1 = (sweepData.mess.k1[kxm1[k]] * y1faktor + y1shift) -
                      getCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * kxm1[k]);
                py1 = py1 + sweepData.dbshift1;
                if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                  py1 = korrsavpegel(py1) - getSavCalWorth(afrequenz + sweepData.schrittfrequenz * kxm1[k]);
                }
                py1 = py1 * yzeichenfaktor_log + yzeichenshift_log;
                my1[k] = int(round(py1) + upperBorder);
                if(my1[k] > (hoehe + upperBorder))my1[k] = hoehe + upperBorder;
                if(my1[k] < upperBorder)my1[k] = upperBorder;
              }
            }
          }
          p->drawPolyline(kurve);
        }
        //##########################################################
        // Dann Kanal 2 
        //##########################################################
        if(sweepData.bkanal2){
          p->setPen( sweepData.penkanal2);
          kurve.clear();
          //for(i=0; i < wobbeldaten.freq_step - 1; i++){
          for(i=0; i < sweepData.cntpoints; i++){
            //ab V4.11 wobbeldaten.maudio2
            double messk2 = sweepData.mess.k2[i];
            if(sweepData.maudio2)messk2 = messk2 / ateiler;
            //Neue Berechnung ab V3.05
            if(sweepData.linear1){
              //dB ausrechnen LINEAR
              py1 = messk2 * y2faktor + y2shift;
              //py2 = wobbeldaten.mess.k2[i+1] * y2faktor + y2shift;
              py1 = py1 * yzeichenfaktor_lin + yzeichenshift_lin;
              //py2 = py2 * yzeichenfaktor_lin + yzeichenshift_lin;
            }else{
              //dB ausrechnen LOG
              py1 = messk2 * y2faktor + y2shift -
                    getCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * i);
              //py2 = wobbeldaten.mess.k2[i+1] * y2faktor + y2shift -
              //      getkalibrierwertk2(afrequenz + wobbeldaten.schrittfrequenz * (i+1));
              if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                py1 = korrsavpegel(py1) - getSavCalWorth(afrequenz + sweepData.schrittfrequenz * i);
                //py2 = korrsavpegel(py2) - getsavcalwert(afrequenz + wobbeldaten.schrittfrequenz * (i+1));
              }
              //eventuell dbshift addieren
              py1 = py1 + sweepData.dbshift2;
              //py2 = py2 + wobbeldaten.dbshift2;
              //Y-Position im Windows errechen
              py1 = py1 * yzeichenfaktor_log + yzeichenshift_log;
              //py2 = py2 * yzeichenfaktor_log + yzeichenshift_log;
            }
            y = int(round(py1) + upperBorder);
            //y1 = int(round(py2) + ObererRand);
            if(y > (hoehe + upperBorder))y = hoehe + upperBorder;
            if(y < upperBorder)y = upperBorder;
            //if(y1 > (hoehe + ObererRand))y1 = hoehe + ObererRand;
            //if(y1 < ObererRand)y1 = ObererRand;
            bkurvezeichnen = TRUE;
            x = int(round(i * xSchritt) + widthBorder) + (int)xAnfang;
            if( x < widthBorder)bkurvezeichnen = FALSE;
            if( x > (widthBorder + breite))bkurvezeichnen = FALSE;
            //x1 = int(round((i+1) * xSchritt) + LinkerRand) + (int)xAnfang;
            //if( x1 < LinkerRand)bkurvezeichnen = FALSE;
            //if( x1 > (LinkerRand + breite))bkurvezeichnen = FALSE;
            if(bkurvezeichnen)kurve.append(QPoint(x,y));
            if(sweepData.linear2){
              py1 = messk2 * y2faktor + y2shift;
            } else {
              py1 = messk2 * y2faktor + y2shift -
                    getCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * i);
              if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                py1 = korrsavpegel(py1) - getSavCalWorth(afrequenz + sweepData.schrittfrequenz * i);
              }
            }
            //maximum Kanal2 ermitteln
            if(py1 > maxdbk2){
              maxdbk2 = py1;
              maxk2f = i;//passende Frequenz dazu
            }
            //minimum Kanal2 ermitteln
            if(py1 < mindbk2){
              mindbk2 = py1;
              mink2f = i;//passende Frequenz dazu
            }
            /*
            if(wobbeldaten.linear2){
              py1 = wobbeldaten.mess.k2[i+1] * y2faktor + y2shift;
            } else {
              py1 = wobbeldaten.mess.k2[i+1] * y2faktor + y2shift - 
                    getkalibrierwertk2(afrequenz + wobbeldaten.schrittfrequenz * (i+1)); 
              if((wobbeldaten.ebetriebsart == espektrumanalyser) or (wobbeldaten.ebetriebsart == espekdisplayshift)){
                py1 = korrsavpegel(py1) - getsavcalwert(afrequenz + wobbeldaten.schrittfrequenz * (i+1));
              }
            }
            //maximum Kanal2 ermitteln
            if(py1 > maxdbk2){
              maxdbk2 = py1;
              maxk2f = i+1;//passende Frequenz dazu
            }
            //minimum Kanal2 ermitteln
            if(py1 < mindbk2){
              mindbk2 = py1;
              mink2f = i+1;//passende Frequenz dazu
            }
            */
            for(k=0;k<5;k++){
              if(mouseposx1[k] != mouseposx[k]){
                if(i == 0){
                  xm1[k] = widthBorder;
                  mx2[k] = x;
                  my2[k] = y;
                  kx[k] = i;
                  ky2[k] = messk2;
                  kxm2[k] = kx[k];
                }
                xm2[k] = abs(x - mouseposx[k]);
                if(xm2[k] < xm1[k]){
                  xm1[k] = xm2[k];
                  mx2[k] = x;
                  my2[k] = y;
                  kx[k] = i;
                  double messk21 = sweepData.mess.k2[kx[k]];
                  if(sweepData.maudio2)messk21 = messk21 / ateiler;
                  ky2[k] = messk21;
                  kxm2[k] = kx[k];
                }
                /*
                xm2[k] = abs(x1 - mouseposx[k]);
                if(xm2[k] < xm1[k]){
                  xm1[k] = xm2[k];
                  mx2[k] = x1;
                  my2[k] = y1;
                  kx[k] = i + 1;
                  ky2[k] = wobbeldaten.mess.k2[kx[k]];
                  kxm2[k] = kx[k];
                }
                */
              }else{
                mx2[k] = int(round(kxm2[k] * xSchritt) + widthBorder);
                double messk21 = sweepData.mess.k2[kxm2[k]];
                if(sweepData.maudio2)messk21 = messk21 / ateiler;
                py1 = (messk21 * y2faktor + y2shift) -
                      getCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * kxm2[k]);
                py1 = py1 + sweepData.dbshift2;
                if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                  py1 = korrsavpegel(py1) - getSavCalWorth(afrequenz + sweepData.schrittfrequenz * kxm2[k]);
                }
                py1 = py1 * yzeichenfaktor_log + yzeichenshift_log;
                my2[k] = int(round(py1) + upperBorder);
                if(my2[k] > (hoehe + upperBorder))my2[k] = hoehe + upperBorder;
                if(my2[k] < upperBorder)my2[k] = upperBorder;
              }
            }
          }
          p->drawPolyline(kurve);
        }
        if(!sweepData.bhintergrund){
          //##########################################################
          // Kursor Berechnungen 
          //##########################################################
          //Aus aktiven Kursor die Frequenz ermitteln
          if((mouseposx[cursorix] != 0) && (mouseposy[cursorix] != 0)){ //Cursorindex aktiv
            if(sweepData.bkanal1){
              px1 = (kxm1[cursorix] * sweepData.schrittfrequenz ) + afrequenz;
            }  
            if(sweepData.bkanal2){
              px1 = (kxm2[cursorix] * sweepData.schrittfrequenz ) + afrequenz;
            }  
            if(banfang){ //Uebergabe aus dem Mausmenue
              //qDebug("banfang = FALSE;");
              banfang = FALSE;
              //neue Anfangsfrequenz an nwtlinux.cpp uebermittlen
              emit writeBegin(px1);
            }
            if(bende){ //Uebergabe aus dem Mausmenue
              bende = FALSE;
              //neue Endfrequenz an nwtlinux.cpp uebermittlen
              emit writeEnd(px1);
            }
            if(bberechnung){ //Uebergabe aus dem Mausmenue
              bberechnung = FALSE;
              //Berechnungsfrequenz an nwtlinux.cpp uebermittlen
              emit writeCalculation(px1);
            }
            mousefrequenz = px1;
          }
          for(k=0;k<5;k++){
            bmouse = TRUE;
            if(k!=0)bmouse = !sweepData.mousesperre;
            if((mouseposx[k] != 0) && (mouseposy[k] != 0) && bmouse ){ //Cursorkreuz aktiv
              qstr = tr("Cursor","");
              s.sprintf(" %i:", k+1);
              s = qstr + s;
              emit multiEditInsert(s);
              if(sweepData.bkanal1){
                px1 = (kxm1[k] * sweepData.schrittfrequenz ) + afrequenz;
              }  
              if(sweepData.bkanal2){
                px1 = (kxm2[k] * sweepData.schrittfrequenz ) + afrequenz;
              }
              s.sprintf("%2.6f MHz", px1 / 1000000.0);
              emit multiEditInsert(s);
              //##########################################################
              // Kursortext Kanal 1 
              //##########################################################
              if(sweepData.bkanal1){
                ky1[k] = sweepData.mess.k1[kxm1[k]];
                if(sweepData.linear1){
                  py1 = ky1[k] * y1faktor + y1shift;
                }else{
                  py1 = ky1[k] * y1faktor + y1shift - getCallibrateWorthK1(px1);
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    py1 = korrsavpegel(py1) - getSavCalWorth(px1);
                  }
                }
                qstr = tr("Channel 1:","");
                if(sweepData.linear1){
                  py1 = (100.0 + py1) / 100.0;
                  bx = 20.0 * log10(py1);
                  s.sprintf(" %1.2f, %1.2fdB", py1, bx);
                  s = qstr + s;
                } else {
                  py1 = py1 + sweepData.dbshift1 + display_shift;
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    s = kursorsavstr(py1);
                  }else{
                    s.sprintf(" %2.2fdB", py1);
                    s = qstr + s;
                  }
                }
                emit multiEditInsert(s);
              }
              //##########################################################
              // Kursortext Kanal 2 
              //##########################################################
              if(sweepData.bkanal2){
                double messk21 = sweepData.mess.k2[kxm2[k]];
                if(sweepData.maudio2)messk21 = messk21 / ateiler;
                ky2[k] = messk21;
                if(sweepData.linear2){
                  py1 = ky2[k] * y2faktor + y2shift;
                }else{
                  py1 = ky2[k] * y2faktor + y2shift - getCallibrateWorthK2(px1);
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    py1 = korrsavpegel(py1) - getSavCalWorth(px1);
                  }
                }
                qstr = tr("Channe l2:","");
                if(sweepData.linear2){
                  py1 = (100.0 + py1) / 100.0;
                  bx = 20.0 * log10(py1);
                  s.sprintf(" %1.2f, %1.2fdB", py1, bx);
                } else {
                  py1 = py1 + sweepData.dbshift2 + display_shift;
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    s = kursorsavstr(py1);
                  }else{
                    s.sprintf(" %2.2fdB", py1);
                    s = qstr + s;
                  }
                }
                emit multiEditInsert(s);
              }
              s = "---------------------";
              emit multiEditInsert(s);
            }
          }
          //##########################################################
          // max min Text Kanal 1
          //##########################################################
          if(sweepData.bkanal1){
            s = tr("Channel 1","");
            emit multiEditInsert(s);
            px1 = (maxk1f * sweepData.schrittfrequenz ) + afrequenz;
            px1 = px1 / 1000000.0;
            qstr = tr("max :","");
            if(sweepData.linear1){
              py1 = (100.0 + maxdbk1) / 100.0;
              bx = 20.0 * log10(py1);
              s.sprintf("%1.2f, %1.2fdB %2.6fMHz", py1, bx, px1);
              squdaten.sprintf("%6.0f;%1.2fdB",px1 * 1000000.0,bx); 
            } else {
              if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                s.sprintf("%2.2fdBm %2.6fMHz", maxdbk1 + sweepData.dbshift1 + display_shift, px1);
              }else{
                s.sprintf("%2.2fdB %2.6fMHz", maxdbk1 + sweepData.dbshift1 + display_shift, px1);
              }
            }
            s = qstr + s;
            emit multiEditInsert(s);
            px1 = (mink1f * sweepData.schrittfrequenz ) + afrequenz;
            px1 = px1 / 1000000.0;
            qstr = tr("min :","");
            if(sweepData.linear1){
              py1 = (100.0 + mindbk1) / 100.0;
              bx = 20.0 * log10(py1);
              s.sprintf("%1.2f, %1.2fdB %2.6fMHz", py1, bx, px1);
            } else {
              if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                s.sprintf("%2.2fdBm %2.6fMHz", mindbk1 + sweepData.dbshift1 + display_shift, px1);
              }else{
                s.sprintf("%2.2fdB %2.6fMHz", mindbk1 + sweepData.dbshift1 + display_shift, px1);
              }
            }
            s = qstr + s;
            emit multiEditInsert(s);
            //#####################################################
            // Bandbreite 3dB
            //#####################################################
            if(sweepData.bandbreite3db){
              db3xa = 0.0;
              db3xb = 0.0;
              //ermitteln der 1. Frequenz in Richtung tiefer Frequenz
              for(i=maxk1f; i>=0; i--){
                if(!sweepData.linear1){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift - getCallibrateWorthK1(xfrequenz);
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    py1 = korrsavpegel(py1) - getSavCalWorth(xfrequenz);
                  }
                  // 3db Unterschritten
                  if((maxdbk1 - py1) > 3.0){
                    db3ya = py1;
                    db3xa = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1 = i;
                    break;// Schleife verlassen
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  //maxdb
                  py2 = (100.0 + maxdbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((bx - py1) > 3.0){
                    db3ya = py1;
                    db3xa = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1 = i;
                    break;
                  }
                }
              }
              //ermitteln der 2. Frequenz
              for(i=maxk1f; i < sweepData.cntpoints; i++){
                if(!sweepData.linear1){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift - getCallibrateWorthK1(xfrequenz);
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    py1 = korrsavpegel(py1) - getSavCalWorth(xfrequenz);
                  }
                  if((maxdbk1 - py1) > 3.0){
                    db3yb = py1;
                    db3xb = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2 = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + maxdbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((bx - py1) > 3.0){
                    db3yb = py1;
                    db3xb = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2 = i;
                    break;
                  }
                }
              }
              //invers noch ermitteln
              //ermitteln der 1. Frequenz
              db3xai = 0.0;
              db3xbi = 0.0;
              for(i=mink1f; i>=0; i--){
                if(!sweepData.linear1){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift - getCallibrateWorthK1(xfrequenz);
                  if((py1 - mindbk1) > 3.0){
                    db3yai = py1;
                    db3xai = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1i = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + mindbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((py1 - bx) > 3.0){
                    db3yai = py1;
                    db3xai = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1i = i;
                    break;
                  }
                }
              }
              //ermitteln der 2. Frequenz
              for(i=mink1f; i < sweepData.cntpoints; i++){
                if(!sweepData.linear1){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift - getCallibrateWorthK1(xfrequenz);
                  if((py1 - mindbk1) > 3.0){
                    db3ybi = py1;
                    db3xbi = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2i = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + mindbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((py1 - bx) > 3.0){
                    db3ybi = py1;
                    db3xbi = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2i = i;
                    break;
                  }
                }
              }
              // es hat geklappt, es ist eine Durchlasskurve voerhanden
              if((db3xa != 0.0) and (db3xb != 0.0)){
                bandbreite3db = db3xb - db3xa;
                s.sprintf(";%1.3f", bandbreite3db);
                squdaten = squdaten + s;
                qstr = tr("B3dB :","");
                s.sprintf(" %1.3f Hz", bandbreite3db);
                if(bandbreite3db > 1000.0){
                  s.sprintf(" %1.3f kHz", bandbreite3db/1000.0);
                }
                if(bandbreite3db > 1000000.0){
                  s.sprintf(" %1.3f MHz", bandbreite3db/1000000.0);
                }
                s = qstr + s;
                emit multiEditInsert(s);
                s.sprintf("Q: %1.2f", ((db3xa + db3xb)/2.0)/bandbreite3db);
                emit multiEditInsert(s);
                if(sweepData.bandbreitegrafik){
                  p->setPen(penreddot);
                  x = int(round(db3x1 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                  x = int(round(db3x2 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                }
                //untere Frequenz
                s.sprintf("f1: %1.6f MHz", db3xa/1000000.0);
                emit multiEditInsert(s);
                //mitten Frequenz
                s.sprintf("fm: %1.6f MHz", (db3xa + ((db3xb - db3xa) / 2))/1000000.0);
                emit multiEditInsert(s);
                //obere Frequenz
                s.sprintf("f2: %1.6f MHz", db3xb/1000000.0);
                emit multiEditInsert(s);
              } else {
                //es hat nicht geklappt nichts gefunden
                s = tr("B3dB : None","");
                emit multiEditInsert(s);
              }
              //inverser 3dB Buckel
              if(sweepData.binvers){
                if((db3xai != 0.0) and (db3xbi != 0.0)){
                  bandbreite3db = db3xbi - db3xai;
                  qstr = tr("B3dB-Inv. :","");
                  s.sprintf(" %1.3f Hz", bandbreite3db);
                  if(bandbreite3db > 1000.0){
                    s.sprintf(" %1.3f kHz", bandbreite3db/1000.0);
                  }
                  if(bandbreite3db > 1000000.0){
                    s.sprintf(" %1.3f MHz", bandbreite3db/1000000.0);
                  }
                  s = qstr + s;
                  emit multiEditInsert(s);
                  qstr = tr("Q-inv. :","");

                  s.sprintf(" %1.2f", ((db3xai + db3xbi)/2.0)/bandbreite3db);
                  s = qstr + s;
                  emit multiEditInsert(s);
                  if(sweepData.bandbreitegrafik){
                    p->setPen(penreddot);
                    x = int(round(db3x1i * xSchritt) + widthBorder);
                    p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                    x = int(round(db3x2i * xSchritt) + widthBorder);
                    p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                  }
                  s.sprintf("f1: %1.6f MHz", db3xai/1000000.0);
                  emit multiEditInsert(s);
                  s.sprintf("f2: %1.6f MHz", db3xbi/1000000.0);
                  emit multiEditInsert(s);
                } else {
                  s = tr("B3dB-Inv. : None","");
                  emit multiEditInsert(s);
                }
              }
            }
            // Bandbreite 6dB
            if(sweepData.bandbreite6db){
              db3xa = 0.0;
              db3xb = 0.0;
              //ermitteln der 1. Frequenz
              for(i=maxk1f; i>=0; i--){
                if(!sweepData.linear1){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift - getCallibrateWorthK1(xfrequenz);
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    py1 = korrsavpegel(py1) - getSavCalWorth(xfrequenz);
                  }
                  if((maxdbk1 - py1) > 6.0){
                    db3ya = py1;
                    db3xa = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1 = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + maxdbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((bx - py1) > 6.0){
                    db3ya = py1;
                    db3xa = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1 = i;
                    break;
                  }
                }
              }
              //ermitteln der 2. Frequenz
              for(i=maxk1f; i < sweepData.cntpoints; i++){
                if(!sweepData.linear1){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift - getCallibrateWorthK1(xfrequenz);
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    py1 = korrsavpegel(py1) - getSavCalWorth(xfrequenz);
                  }
                  if((maxdbk1 - py1) > 6.0){
                    db3yb = py1;
                    db3xb = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2 = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + maxdbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((bx - py1) > 6.0){
                    db3yb = py1;
                    db3xb = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2 = i;
                    break;
                  }
                }
              }
              //es hat geklappt Durchlasskurve vorhanden
              if((db3xa != 0.0) and (db3xb != 0.0)){
                bandbreite6db = db3xb - db3xa;
                qstr = tr("B6dB :","");
                s.sprintf(" %1.3f Hz", bandbreite6db);
                if(bandbreite3db > 1000.0){
                  s.sprintf(" %1.3f kHz", bandbreite6db/1000.0);
                }
                if(bandbreite3db > 1000000.0){
                  s.sprintf(" %1.3f MHz", bandbreite6db/1000000.0);
                }
                s = qstr + s;
                emit multiEditInsert(s);
                if(sweepData.bandbreitegrafik){
                  p->setPen(penreddash);
                  x = int(round(db3x1 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                  x = int(round(db3x2 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                }
                s.sprintf("f1: %1.6f MHz", db3xa/1000000.0);
                emit multiEditInsert(s);
                s.sprintf("f2: %1.6f MHz", db3xb/1000000.0);
                emit multiEditInsert(s);
              } else {
                s = tr("B6dB : None","");
                emit multiEditInsert(s);
              }
            }
            //noch die 60dB Bandbreite ermitteln wenn vorhanden
            if(sweepData.bandbreite6db){
              db3xa = 0.0;
              db3xb = 0.0;
              //ermitteln der 1. Frequenz
              for(i=maxk1f; i>=0; i--){
                if(!sweepData.linear1){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift - getCallibrateWorthK1(xfrequenz);
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    py1 = korrsavpegel(py1) - getSavCalWorth(xfrequenz);
                  }
                  if((maxdbk1 - py1) > 60.0){
                    db3ya = py1;
                    db3xa = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1 = i;
                    break;
                  }
                }
              }
              //ermitteln der 2. Frequenz
              for(i=maxk1f; i < sweepData.cntpoints; i++){
                if(!sweepData.linear1){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = sweepData.mess.k1[i] * y1faktor + y1shift - getCallibrateWorthK1(xfrequenz);
                  if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                    py1 = korrsavpegel(py1) - getSavCalWorth(xfrequenz);
                  }
                  if((maxdbk1 - py1) > 60.0){
                    db3yb = py1;
                    db3xb = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2 = i;
                    break;
                  }
                }
              }
             //es hat geklappt Durchlasskurve vorhanden
              if((db3xa != 0.0) and (db3xb != 0.0)){
                bandbreite3db = db3xb - db3xa;
                qstr = tr("B60dB :","");
                s.sprintf(" %1.3f Hz", bandbreite3db);
                if(bandbreite3db > 1000.0){
                  s.sprintf(" %1.3f kHz", bandbreite3db/1000.0);
                }
                if(bandbreite3db > 1000000.0){
                  s.sprintf(" %1.3f MHz", bandbreite3db/1000000.0);
                }
                s = qstr + s;
                emit multiEditInsert(s);
                if(sweepData.bandbreitegrafik){
                  p->setPen(penbluedash);
                  x = int(round(db3x1 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                  x = int(round(db3x2 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                }
                s.sprintf("f1: %1.6f MHz", db3xa/1000000.0);
                emit multiEditInsert(s);
                s.sprintf("f2: %1.6f MHz", db3xb/1000000.0);
                emit multiEditInsert(s);
                s.sprintf("Shape-Faktor: %1.6f", bandbreite3db / bandbreite6db);
                emit multiEditInsert(s);
              } else {
                s = "B60db: keine";
                emit multiEditInsert(s);
              }
            }
            s = "---------------------";
            emit multiEditInsert(s);
          }
          //##########################################################
          // max min Text Kanal 2
          //##########################################################
          if(sweepData.bkanal2){
            s = tr("Channel 2","");
            emit multiEditInsert(s);
            px1 = (maxk2f * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
            px1 = px1 / 1000000.0;
            qstr = tr("max :","");
            if(sweepData.linear2){
              py1 = (100.0 + maxdbk2) / 100.0;
              bx = 20.0 * log10(py1);
              s.sprintf("%1.2f, %1.2fdB %2.6fMHz", py1, bx, px1);
            } else {
              if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                s.sprintf("%2.2fdBm %2.6fMHz", maxdbk2 + sweepData.dbshift2 + display_shift, px1);
              }else{
                s.sprintf("%2.2fdB %2.6fMHz", maxdbk2 + sweepData.dbshift2 + display_shift, px1);
              }
            }
            s = qstr + s;
            emit multiEditInsert(s);
            px1 = (mink2f * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
            px1 = px1 / 1000000.0;
            qstr = tr("min :","");
            if(sweepData.linear2){
              py1 = (100.0 + mindbk2) / 100.0;
              bx = 20.0 * log10(py1);
              s.sprintf("%1.2f, %1.2fdB %2.6fMHz", py1, bx, px1);
            } else {
              if((sweepData.eoperatemode == eSpectrumAnalyser) or (sweepData.eoperatemode == eSpectrumDisplayShift)){
                s.sprintf("%2.2fdBm %2.6fMHz", mindbk2 + sweepData.dbshift2 + display_shift, px1);
              }else{
                s.sprintf("%2.2fdB %2.6fMHz", mindbk2 + sweepData.dbshift2 + display_shift, px1);
              }
            }
            s = qstr + s;
            emit multiEditInsert(s);
            //3dB Bandbreite + Q fuer Kanal 2
            if(sweepData.bandbreite3db){
              db3xa = 0.0;
              db3xb = 0.0;
              //ermitteln der unteren Frequenz
              for(i=maxk2f; i>=0; i--){
                double messk2 = sweepData.mess.k2[i];
                if(sweepData.maudio2)messk2 = messk2 / ateiler;
                if(!sweepData.linear2){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift - getCallibrateWorthK2(xfrequenz);
                  if((maxdbk2 - py1) > 3.0){
                    db3ya = py1;
                    db3xa = xfrequenz;
                    db3x1 = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + mindbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((py1 - bx) > 3.0){
                    db3ya = py1;
                    db3xa = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1 = i;
                    break;
                  }
                }
              }
              //ermitteln der oberen Frequenz
              for(i=maxk2f; i < sweepData.cntpoints; i++){
                double messk2 = sweepData.mess.k2[i];
                if(sweepData.maudio2)messk2 = messk2 / ateiler;
                if(!sweepData.linear2){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift - getCallibrateWorthK2(xfrequenz);
                  if((maxdbk2 - py1) > 3.0){
                    db3yb = py1;
                    db3xb = xfrequenz;
                    db3x2 = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + mindbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((py1 - bx) > 3.0){
                    db3yb = py1;
                    db3xb = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2 = i;
                    break;
                  }
                }
              }
              //invers
              db3xai = 0.0;
              db3xbi = 0.0;
              //ermitteln der unteren Frequenz
              for(i=mink2f; i>=0; i--){
                double messk2 = sweepData.mess.k2[i];
                if(sweepData.maudio2)messk2 = messk2 / ateiler;
                if(!sweepData.linear2){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift - getCallibrateWorthK2(xfrequenz);
                  if((py1 - mindbk2) > 3.0){
                    db3yai = py1;
                    db3xai = xfrequenz;
                    db3x1i = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + mindbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((py1 - bx) > 3.0){
                    db3yai = py1;
                    db3xai = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1i = i;
                    break;
                  }
                }
              }
              //ermitteln der oberen Frequenz
              for(i=mink2f; i < sweepData.cntpoints; i++){
                double messk2 = sweepData.mess.k2[i];
                if(sweepData.maudio2)messk2 = messk2 / ateiler;
                if(!sweepData.linear2){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift - getCallibrateWorthK2(xfrequenz);
                  if((py1 - mindbk2) > 3.0){
                    db3ybi = py1;
                    db3xbi = xfrequenz;
                    db3x2i = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + mindbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((py1 - bx) > 3.0){
                    db3ybi = py1;
                    db3xbi = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2i = i;
                    break;
                  }
                }
              }
              //Wurden 3dB Werte gefunden
              if((db3xa != 0.0) and (db3xb != 0.0)){
                bandbreite3db = db3xb - db3xa;
                qstr = tr("B3dB :","");
                s.sprintf(" %1.3f Hz", bandbreite3db);
                if(bandbreite3db > 1000.0)s.sprintf(" %1.3f kHz", bandbreite3db/1000.0);
                if(bandbreite3db > 1000000.0)s.sprintf(" %1.3f MHz", bandbreite3db/1000000.0);
                s = qstr + s;
                emit multiEditInsert(s);
                s.sprintf("Q: %1.2f", ((db3xa + db3xb)/2.0)/bandbreite3db);
                emit multiEditInsert(s);
                //Bandbreitengrafik aktiv
                if(sweepData.bandbreitegrafik){
                  p->setPen(pendarkgreendot);
                  x = int(round(db3x1 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                  x = int(round(db3x2 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                }
                s.sprintf("f1: %1.6f MHz", db3xa/1000000.0);
                emit multiEditInsert(s);
                s.sprintf("f2: %1.6f MHz", db3xb/1000000.0);
                emit multiEditInsert(s);
              } else {
                s = tr("B3dB : None","");
                emit multiEditInsert(s);
              }
              //inverse Resonanz ermitteln nur 3dB Kurve
              if(sweepData.binvers){
                if((db3xai != 0.0) and (db3xbi != 0.0)){
                  bandbreite3db = db3xbi - db3xai;
                  qstr = tr("B3dB-Inv. :","");
                  s.sprintf(" %1.3f Hz", bandbreite3db);
                  if(bandbreite3db > 1000.0)s.sprintf(" %1.3f kHz", bandbreite3db/1000.0);
                  if(bandbreite3db > 1000000.0)s.sprintf(" %1.3f MHz", bandbreite3db/1000000.0);
                  s = qstr + s;
                  emit multiEditInsert(s);
                  qstr = tr("Q-inv. :","");
                  s.sprintf(" %1.2f", ((db3xai + db3xbi)/2.0)/bandbreite3db);
                  s = qstr + s;
                  emit multiEditInsert(s);
                  if(sweepData.bandbreitegrafik){
                    p->setPen(penreddot);
                    x = int(round(db3x1i * xSchritt) + widthBorder);
                    p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                    x = int(round(db3x2i * xSchritt) + widthBorder);
                    p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                  }
                  s.sprintf("f1: %1.6f MHz", db3xai/1000000.0);
                  emit multiEditInsert(s);
                  s.sprintf("f2: %1.6f MHz", db3xbi/1000000.0);
                  emit multiEditInsert(s);
                } else {
                  s = tr("B3dB-Inv. : None","");
                  emit multiEditInsert(s);
                }
              }
            }
            //Bandbreite 6dB ermitteln
            if(sweepData.bandbreite6db){
              db3xa = 0.0;
              db3xb = 0.0;
              //ermitteln der 1. Frequenz in Richtung tiefer Frequenz
              for(i=maxk2f; i>=0; i--){
                double messk2 = sweepData.mess.k2[i];
                if(sweepData.maudio2)messk2 = messk2 / ateiler;
                if(!sweepData.linear2){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift - getCallibrateWorthK2(xfrequenz);
                  if((maxdbk2 - py1) > 6.0){
                    db3ya = py1;
                    db3xa = xfrequenz;
                    db3x1 = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + mindbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((py1 - bx) > 6.0){
                    db3ya = py1;
                    db3xa = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x1 = i;
                    break;
                  }
                }
              }
              //ermitteln der 2. Frequenz in Richtung hoher Frequenz
              for(i=maxk2f; i < sweepData.cntpoints; i++){
                double messk2 = sweepData.mess.k2[i];
                if(sweepData.maudio2)messk2 = messk2 / ateiler;
                if(!sweepData.linear2){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift - getCallibrateWorthK2(xfrequenz);
                  if((maxdbk2 - py1) > 6.0){
                    db3yb = py1;
                    db3xb = xfrequenz;
                    db3x2 = i;
                    break;
                  }
                }else{
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift;
                  py1 = (100.0 + py1) / 100.0;
                  py1 = 20.0 * log10(py1);
                  py2 = (100.0 + mindbk1) / 100.0;
                  bx = 20.0 * log10(py2);
                  if((py1 - bx) > 6.0){
                    db3yb = py1;
                    db3xb = (i * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
                    db3x2 = i;
                    break;
                  }
                }
              }
              if((db3xa != 0.0) and (db3xb != 0.0)){
                bandbreite6db = db3xb - db3xa;
                qstr = tr("B6dB :","");
                s.sprintf(" %1.3f Hz", bandbreite6db);
                if(bandbreite3db > 1000.0)s.sprintf(" %1.3f kHz", bandbreite6db/1000.0);
                if(bandbreite3db > 1000000.0)s.sprintf(" %1.3f MHz", bandbreite6db/1000000.0);
                s = qstr + s;
                emit multiEditInsert(s);
                if(sweepData.bandbreitegrafik){
                  p->setPen(pendarkgreendash);
                  x = int(round(db3x1 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                  x = int(round(db3x2 * xSchritt) + widthBorder);
                  p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                }
                s.sprintf("f1: %1.6f MHz", db3xa/1000000.0);
                emit multiEditInsert(s);
                s.sprintf("f2: %1.6f MHz", db3xb/1000000.0);
                emit multiEditInsert(s);
              } else {
                s = tr("B6dB : None","");
                emit multiEditInsert(s);
              }
            }
            //Ermitteln der Bandbreite 60dB
            if(sweepData.bandbreite6db){
              db3xa = 0.0;
              db3xb = 0.0;
              //ermitteln der 1. Frequenz
              for(i=maxk2f; i>=0; i--){
                double messk2 = sweepData.mess.k2[i];
                if(sweepData.maudio2)messk2 = messk2 / ateiler;
                if(!sweepData.linear2){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift - getCallibrateWorthK2(xfrequenz);
                  if((maxdbk2 - py1) > 60.0){
                    db3ya = py1;
                    db3xa = xfrequenz;
                    db3x1 = i;
                    break;
                  }
                }
              }
              //ermitteln der 2. Frequenz
              for(i=maxk2f; i < sweepData.cntpoints; i++){
                double messk2 = sweepData.mess.k2[i];
                if(sweepData.maudio2)messk2 = messk2 / ateiler;
                if(!sweepData.linear2){
                  xfrequenz = afrequenz + sweepData.schrittfrequenz * i;
                  py1 = messk2 * y2faktor + y2shift - getCallibrateWorthK2(xfrequenz);
                  if((maxdbk2 - py1) > 60.0){
                    db3yb = py1;
                    db3xb = xfrequenz;
                    db3x2 = i;
                    break;
                  }
                }
              }
              //es hat geklappt Durchlasskurve vorhanden
              if((!sweepData.linear2)){
                if((db3xa != 0.0) and (db3xb != 0.0)){
                  bandbreite3db = db3xb - db3xa;
                  qstr = tr("B60dB :","");
                  s.sprintf(" %1.3f Hz", bandbreite3db);
                  if(bandbreite3db > 1000.0)s.sprintf(" %1.3f kHz", bandbreite3db/1000.0);
                  if(bandbreite3db > 1000000.0)s.sprintf(" %1.3f MHz", bandbreite3db/1000000.0);
                  s = qstr + s;
                  emit multiEditInsert(s);
                  if(sweepData.bandbreitegrafik){
                    p->setPen(penbluedash);
                    x = int(round(db3x1 * xSchritt) + widthBorder);
                    p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                    x = int(round(db3x2 * xSchritt) + widthBorder);
                    p->drawLine(x, upperBorder, x, hoehe + upperBorder);
                  }
                  s.sprintf("f1: %1.6f MHz", db3xa/1000000.0);
                  emit multiEditInsert(s);
                  s.sprintf("f2: %1.6f MHz", db3xb/1000000.0);
                  emit multiEditInsert(s);
                  qstr = tr("Shapefactor:","");
                  s.sprintf(" %1.6f", bandbreite3db / bandbreite6db);
                  s =qstr + s;
                  emit multiEditInsert(s);
                } else {
                  s = tr("B60dB : None","");
                  emit multiEditInsert(s);
                }
              }
            }
            s = "---------------------";
            emit multiEditInsert(s);
          }
        }
        break;
      case eSWR:
      case eSWRfeeder:
      case eImpedance:
      //##########################################################
      // Betriebsart SWRneu 
      //##########################################################
      afrequenz = sweepData.anfangsfrequenz;
      kurve.clear();
      kurveb.clear();
      //for(i=0; i < wobbeldaten.freq_step - 1; i++){
      for(i=0; i < sweepData.cntpoints; i++){
        if(sweepData.bswrkanal2){
          p->setPen( sweepData.penkanal2 );
        }else{
          p->setPen( sweepData.penkanal1 );
        }
        bkurvezeichnen = TRUE;
        x = int(round(i * xSchritt) + widthBorder) + (int)xAnfang;
        if( x < widthBorder)bkurvezeichnen = FALSE;
        if( x > (widthBorder + breite))bkurvezeichnen = FALSE;
        /*
        x1 = int(round((i+1) * xSchritt) + LinkerRand) + (int)xAnfang;
        if( x1 < LinkerRand)bkurvezeichnen = FALSE;
        if( x1 > (LinkerRand + breite))bkurvezeichnen = FALSE;
        */
        j1 = i - 1;
        j2 = i;
        j3 = i + 1;
        if(j1<0)j1=0;
        if(j3>(sweepData.cntpoints-1))j3 = sweepData.cntpoints-1;
        //Return loss berechnen
        if(sweepData.bswrkanal2){
          w1 = double(sweepData.mess.k2[j1]);
          w2 = double(sweepData.mess.k2[j2]);
          w3 = double(sweepData.mess.k2[j3]);
          py1 = w2 * y2faktor + y2shift;
          py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(j2));
        }else{
          w1 = double(sweepData.mess.k1[j1]);
          w2 = double(sweepData.mess.k1[j2]);
          w3 = double(sweepData.mess.k1[j3]);
          py1 = w2 * y1faktor + y1shift;
          py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(j2));
        }
        if(py1 > -1.0)py1 = -1.0;
        uricht = pow(10.0 , (py1/20.0));
        swv = (1.0 + uricht)/(1.0 - uricht);
        if(sweepData.bswriteration){
          if(sweepData.bswrkanal2){
            py1 = w1 * y2faktor + y2shift;
            py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(j1));
          }else{
            py1 = w1 * y1faktor + y1shift;
            py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(j1));
          }
          if(py1 > -1.0)py1 = -1.0;
          uricht = pow(10.0 , (py1/20.0)); //Reflektionsfaktor ausrechnen
          swv = swv + (1.0 + uricht)/(1.0 - uricht); //swv ausrechnen
          if(sweepData.bswrkanal2){
            py1 = w3 * y2faktor + y2shift;
            py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(j3));
          }else{
            py1 = w3 * y1faktor + y1shift;
            py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(j3));
          }
          if(py1 > -1.0)py1 = -1.0;
          uricht = pow(10.0 , (py1/20.0));
          swv = swv + (1.0 + uricht)/(1.0 - uricht);
          swv = swv / 3.0; // da Iteration durch 3 Teilen
        }
        if((sweepData.eoperatemode == eSWR)or(sweepData.eoperatemode == eSWRfeeder)){
          y = int(round(hoehe - round((swv - 1.0) * (double(hoehe)/yswrteilung)) - 1 + upperBorder));
        } 
        if(sweepData.eoperatemode == eImpedance){
          swv = (50.0 * swv) - 50.0;
          y = int(round(hoehe - round((swv - 1.0) * (double(hoehe)/200.0)) - 2 + upperBorder));
        } 
        if(sweepData.eoperatemode == eSWRfeeder){
          a = a_100 * laenge / 100.0;
          r_tx = (swv - 1.0) / (swv + 1.0);
          r_ant = r_tx * pow(10.0 , a/10.0);
          if(r_ant >= 1.0)r_ant = 0.99;
          s_ant = (1.0 + r_ant) / (1.0 - r_ant);
          y3 = int(round(hoehe - round((s_ant - 1.0) * (double(hoehe)/yswrteilung)) - 1 + upperBorder));
        }
        j1 = i ;
        j2 = i + 1;
        j3 = i + 2;
        if(j2>(sweepData.cntpoints-1))j2 = sweepData.cntpoints-1;
        if(j3>(sweepData.cntpoints-1))j3 = sweepData.cntpoints-1;
        if(sweepData.bswrkanal2){
          w1 = double(sweepData.mess.k2[j1]);
          w2 = double(sweepData.mess.k2[j2]);
          w3 = double(sweepData.mess.k2[j3]);
          py1 = w2 * y2faktor + y2shift;
          py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(j2));
        }else{
          w1 = double(sweepData.mess.k1[j1]);
          w2 = double(sweepData.mess.k1[j2]);
          w3 = double(sweepData.mess.k1[j3]);
          py1 = w2 * y1faktor + y1shift;
          py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(j2));
        }
        if(py1 > -1.0)py1 = -1.0;
        uricht = pow(10.0 , (py1/20.0));
        swv = (1.0 + uricht)/(1.0 - uricht);
        if(sweepData.bswriteration){
          if(sweepData.bswrkanal2){
            py1 = w1 * y2faktor + y2shift;
            py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(j1));
          }else{
            py1 = w1 * y1faktor + y1shift;
            py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(j1));
          }
          if(py1 > -1.0)py1 = -1.0;
          uricht = pow(10.0 , (py1/20.0));
          swv = swv + (1.0 + uricht)/(1.0 - uricht);
          if(sweepData.bswrkanal2){
            py1 = w3 * y2faktor + y2shift;
            py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(j3));
          }else{
            py1 = w3 * y1faktor + y1shift;
            py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(j3));
          }
          if(py1 > -1.0)py1 = -1.0;
          uricht = pow(10.0 , (py1/20.0));
          swv = swv + (1.0 + uricht)/(1.0 - uricht);
          swv = swv / 3.0;
         }
	//ermitteln des minimalen SWR in ganzen Bereich
         if(minswr > swv){
           minswr = swv;
           mink1 = j2;
         }  
	//ermitteln des maximalen SWR in ganzen Bereich
         if(maxswr < swv){
           maxswr = swv;
           maxk1 = j2;
        }  
        if((sweepData.eoperatemode == eSWR)or(sweepData.eoperatemode == eSWRfeeder)){
          y1 = int(round(hoehe - round((swv - 1.0) * (double(hoehe)/yswrteilung)) - 1 + upperBorder));
        }
        if(sweepData.eoperatemode == eImpedance){
          swv = (50.0 * swv) - 50.0;
          y1 = int(round(hoehe - round((swv - 1.0) * (double(hoehe)/200.0)) - 2 + upperBorder));
        }
        if(sweepData.eoperatemode == eSWRfeeder){
          a = a_100 * laenge / 100.0;
          r_tx = (swv - 1.0) / (swv + 1.0);
          r_ant = r_tx * pow(10.0 , a/10.0);
          if(r_ant >= 1.0)r_ant = 0.99;
          s_ant = (1.0 + r_ant) / (1.0 - r_ant);
          y4 = int(round(hoehe - round((s_ant - 1.0) * (double(hoehe)/yswrteilung)) - 1 + upperBorder));
        }
        //auf extremwerte reagieren Linie soll Anzeige nicht nach oben verlassen
        if(y > (hoehe + upperBorder))y = hoehe + upperBorder;
        if(y < upperBorder)y = upperBorder;
        if(sweepData.eoperatemode == eSWRfeeder){
          if(y3 > (hoehe + upperBorder))y3 = hoehe + upperBorder;
          if(y3 < upperBorder)y3 = upperBorder;
        }  
        //Linie zeichnen
        if(bkurvezeichnen){
          kurve.append(QPoint(x, y)); // neuen Messpunkt anfuegen
          if(sweepData.eoperatemode == eSWRfeeder){
            kurveb.append(QPoint(x, y3)); // neuen Ant-Messpunkt anfuegen
          }
        }
        // keine Hintergrundkurven aus den WKM Marker-Berechnung
        if(!sweepData.bhintergrund){
          //Anfangswerte setzen
          for(k=0;k<5;k++){
            if(mouseposx1[k] != mouseposx[k]){
              if(i == 0){
                xm1[k] = widthBorder;
                mx1[k] = x; //pos im Wobbelfenster
                my1[k] = y; //pos im Wobbelfenster
                kx[k] = i; //nr des Schrittes
                kx1[k] = i - 1;
                kx2[k] = i ;
                if(kx1[k]<0)kx1[k]=0;
                kx3[k] = i + 1;
                if(kx3[k]>(sweepData.cntpoints-1))kx3[k] = sweepData.cntpoints-1;
                if(sweepData.bswrkanal2){
                  kw1[k] = sweepData.mess.k2[kx1[k]];
                  kw2[k] = sweepData.mess.k2[kx2[k]];
                  kw3[k] = sweepData.mess.k2[kx3[k]];
                }else{
                  kw1[k] = sweepData.mess.k1[kx1[k]];
                  kw2[k] = sweepData.mess.k1[kx2[k]];
                  kw3[k] = sweepData.mess.k1[kx3[k]];
                }
                kxm1[k] = kx[k];
              }
              //neue Werte setzen
              xm2[k] = abs(x - mouseposx[k]);
              if(xm2[k] < xm1[k]){
                xm1[k] = xm2[k];
                mx1[k] = x; //pos im Wobbelfenster
                my1[k] = y; //pos im Wobbelfenster
                kx[k] = i;  //nr des Schrittes
                kx1[k] = kx[k] - 1;  //nr des Schrittes
                kx2[k] = kx[k];      //nr des Schrittes
                kx3[k] = kx[k] + 1;  //nr des Schrittes
                if(kx1[k]<0)kx1[k]=0;
                if(kx3[k]>(sweepData.cntpoints-1))kx3[k] = sweepData.cntpoints-1;
                if(sweepData.bswrkanal2){
                  kw1[k] = sweepData.mess.k2[kx1[k]];
                  kw2[k] = sweepData.mess.k2[kx2[k]];
                  kw3[k] = sweepData.mess.k2[kx3[k]];
                }else{
                  kw1[k] = sweepData.mess.k1[kx1[k]];
                  kw2[k] = sweepData.mess.k1[kx2[k]];
                  kw3[k] = sweepData.mess.k1[kx3[k]];
                }
                if(sweepData.eoperatemode == eSWRfeeder){
                  my2[k] = y3;
                  mx2[k] = mx1[k];
                }
                kxm1[k] = kx[k];
              }
              xm2[k] = abs(x1 - mouseposx[k]);
              if(xm2[k] < xm1[k]){
                xm1[k] = xm2[k];
                mx1[k] = x1; //pos im Wobbelfenster
                my1[k] = y1; //pos im Wobbelfenster
                kx[k] = i + 1;  //nr des Schrittes
                kx1[k] = kx[k] - 1;  //nr des Schrittes
                kx2[k] = kx[k];      //nr des Schrittes
                kx3[k] = kx[k] + 1;  //nr des Schrittes
                if(kx1[k]<0)kx1[k]=0;
                if(kx3[k]>(sweepData.cntpoints-1))kx3[k] = sweepData.cntpoints-1;
                if(sweepData.bswrkanal2){
                  kw1[k] = sweepData.mess.k2[kx1[k]];
                  kw2[k] = sweepData.mess.k2[kx2[k]];
                  kw3[k] = sweepData.mess.k2[kx3[k]];
                }else{
                  kw1[k] = sweepData.mess.k1[kx1[k]];
                  kw2[k] = sweepData.mess.k1[kx2[k]];
                  kw3[k] = sweepData.mess.k1[kx3[k]];
                }
                if(sweepData.eoperatemode == eSWRfeeder){
                  my2[k] = y4;
                  mx2[k] = mx1[k];
                }
                kxm1[k] = kx[k];
              }
            }else{
              kx1[k] = kxm1[k] - 1;  //nr des Schrittes
              kx2[k] = kxm1[k];      //nr des Schrittes
              kx3[k] = kxm1[k] + 1;  //nr des Schrittes
              if(kx1[k]<0)kx1[k]=0;
              if(kx3[k]>(sweepData.cntpoints-1))kx3[k] = sweepData.cntpoints-1;
              if(sweepData.bswrkanal2){
                kw1[k] = sweepData.mess.k2[kx1[k]];
                kw2[k] = sweepData.mess.k2[kx2[k]];
                kw3[k] = sweepData.mess.k2[kx3[k]];
              }else{
                kw1[k] = sweepData.mess.k1[kx1[k]];
                kw2[k] = sweepData.mess.k1[kx2[k]];
                kw3[k] = sweepData.mess.k1[kx3[k]];
              }
              mx1[k] = int(round(kxm1[k] * xSchritt) + widthBorder);
              if(sweepData.eoperatemode == eSWRfeeder){
                mx2[k] = mx1[k];
              }
              if(sweepData.bswrkanal2){
                w1 = double(sweepData.mess.k2[kx1[k]]);
                w2 = double(sweepData.mess.k2[kx2[k]]);
                w3 = double(sweepData.mess.k2[kx3[k]]);
                py1 = w2 * y2faktor + y2shift;
                py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(kx2[k]));
              }else{
                w1 = double(sweepData.mess.k1[kx1[k]]);
                w2 = double(sweepData.mess.k1[kx2[k]]);
                w3 = double(sweepData.mess.k1[kx3[k]]);
                py1 = w2 * y1faktor + y1shift;
                py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(kx2[k]));
              }
              if(py1 > -1.0)py1 = -1.0;
              uricht = pow(10.0 , (py1/20.0));
              swv = (1.0 + uricht)/(1.0 - uricht);
              if(sweepData.bswriteration){
                if(sweepData.bswrkanal2){
                  py1 = w1 * y2faktor + y2shift;
                  py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(kx1[k]));
                }else{
                  py1 = w1 * y1faktor + y1shift;
                  py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(kx1[k]));
                }
                if(py1 > -1.0)py1 = -1.0;
                uricht = pow(10.0 , (py1/20.0));
                swv = swv + (1.0 + uricht)/(1.0 - uricht);
                if(sweepData.bswrkanal2){
                  py1 = w3 * y2faktor + y2shift;
                  py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(kx3[k]));
                }else{
                  py1 = w3 * y1faktor + y1shift;
                  py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(kx3[k]));
                }
                if(py1 > -1.0)py1 = -1.0;
                uricht = pow(10.0 , (py1/20.0));
                swv = swv + (1.0 + uricht)/(1.0 - uricht);
                swv = swv / 3.0;
              }
              if((sweepData.eoperatemode == eSWR)or(sweepData.eoperatemode == eSWRfeeder)){
                my1[k] = int(round(hoehe - round((swv - 1.0) * (double(hoehe)/yswrteilung)) - 1 + upperBorder));
              }
              if(sweepData.eoperatemode == eImpedance){
                swv = (50.0 * swv) - 50.0;
    //            swv = 10.0;
                my1[k] = int(round(hoehe - round((swv - 1.0) * (double(hoehe)/200.0)) - 2 + upperBorder));
              } 
              if(sweepData.eoperatemode == eSWRfeeder){
                a = a_100 * laenge / 100.0;
                r_tx = (swv - 1.0) / (swv + 1.0);
                r_ant = r_tx * pow(10.0 , a/10.0);
                if(r_ant >= 1.0)r_ant = 0.99;
                s_ant = (1.0 + r_ant) / (1.0 - r_ant);
                my2[k] = int(round(hoehe - round((s_ant - 1.0) * (double(hoehe)/yswrteilung)) - 1 + upperBorder));
                if(my2[k] > (hoehe + upperBorder))my2[k] = hoehe + upperBorder;
                if(my2[k] < upperBorder)my2[k] = upperBorder;
              }
              if(my1[k] > (hoehe + upperBorder))my1[k] = hoehe + upperBorder;
              if(my1[k] < upperBorder)my1[k] = upperBorder;
            }
          }
        }
        #ifdef PAINTGRAF_DEBUG
          qDebug("WidgetWobbeln::drawKurve():kx[kursorix] %i mx1[kursorix] %i mouseposx[kursorix] %i", kx[cursorix], mx1[cursorix], mouseposx[cursorix]);
        #endif
      }
      p->drawPolyline(kurve); // Kurve zeichnen
      // Ant Kurve zeichnen in blau
      if(sweepData.eoperatemode == eSWRfeeder){
        p->setPen(penbluesolid);
        p->drawPolyline(kurveb);
      }
      if(!sweepData.bhintergrund){
        //Aus aktiven Kursor die Frequenz ermitteln
        if((mouseposx[cursorix] != 0) && (mouseposy[cursorix] != 0)){ //Cursorindex aktiv
          mousefrequenz = (kxm1[cursorix] * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
        }
        for(k=0;k<5;k++){
          bmouse = TRUE;
          if(k!=0)bmouse = !sweepData.mousesperre;
          if((mouseposx[k] != 0) && (mouseposy[k] != 0) && bmouse ){ //Cursorkreuz aktiv
            qstr = tr("Cursor","");
            s.sprintf(" %i:",k+1);
            s = qstr + s;
            if(bnomove)emit multiEditInsert(s);
            px1 = (kxm1[k] * sweepData.schrittfrequenz ) + sweepData.anfangsfrequenz;
            if(banfang){ //Uebergabe aus dem Mausmenue
              //qDebug("banfang = FALSE;");
              banfang = FALSE;
              //neue Anfangsfrequenz an nwtlinux.cpp uebermittlen
              emit writeBegin(px1);
            }
            if(bende){ //Uebergabe aus dem Mausmenue
              bende = FALSE;
              //neue Endfrequenz an nwtlinux.cpp uebermittlen
              emit writeEnd(px1);
            }
            if(bberechnung){ //Uebergabe aus dem Mausmenue
              bberechnung = FALSE;
              //neue Endfrequenz an nwtlinux.cpp uebermittlen
              emit writeCalculation(px1);
            }
            s.sprintf("%2.6f MHz", px1 / 1000000.0);
            emit multiEditInsert(s);
            if(sweepData.bswrkanal2){
              py1 = kw2[k] * y2faktor + y2shift;
              py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(kx2[k]));
            }else{
              py1 = kw2[k] * y1faktor + y1shift;
              py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(kx2[k]));
            }
            if(py1 > -1.0)py1 = -1.0;
            uricht = pow(10.0 , (py1/20.0));
            swv = (1.0 + uricht)/(1.0 - uricht);
            if(sweepData.bswriteration){
              if(sweepData.bswrkanal2){
                py1 = kw1[k] * y2faktor + y2shift;
                py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(kx1[k]));
              }else{
                py1 = kw1[k] * y1faktor + y1shift;
                py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(kx1[k]));
              }
              if(py1 > -1.0)py1 = -1.0;
              uricht = pow(10.0 , (py1/20.0));
              swv = swv + (1.0 + uricht)/(1.0 - uricht);
              if(sweepData.bswrkanal2){
                py1 = kw3[k] * y2faktor + y2shift;
                py1 = py1 - getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(kx3[k]));
              }else{
                py1 = kw3[k] * y1faktor + y1shift;
                py1 = py1 - getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(kx3[k]));
              }
              if(py1 > -1.0)py1 = -1.0;
              uricht = pow(10.0 , (py1/20.0));
              swv = swv + (1.0 + uricht)/(1.0 - uricht);
              swv = swv / 3.0;
            }
            if((sweepData.eoperatemode == eSWR)or(sweepData.eoperatemode == eSWRfeeder)){
              qstr = tr("SWR :","");
              s.sprintf(" %4.4f", swv);
            }
            if(sweepData.eoperatemode == eImpedance){
              if(sweepData.bswrkanal2){
                py1=getSWRCallibrateWorthK2(afrequenz + sweepData.schrittfrequenz * double(kx1[k]));
              }else{
                py1=getSWRCallibrateWorthK1(afrequenz + sweepData.schrittfrequenz * double(kx1[k]));
              }
//              s.sprintf("Eich: %4.4f dB", py1);
//              emit multieditinsert(s);
              swv = (50.0 * swv) - 50.0;
              qstr = tr("|Z| :","");
              s.sprintf(" %4.4f Ohm", swv);
            }
            s = qstr + s;
            emit multiEditInsert(s);
            // SWR Messung ueber Kabel
            if(sweepData.eoperatemode == eSWRfeeder){
              a = a_100 * laenge / 100.0;
              r_tx = (swv - 1.0) / (swv + 1.0);
              r_ant = r_tx * pow(10.0 , a/10.0);
              if(r_ant >= 1.0)r_ant = 0.99;
              s_ant = (1.0 + r_ant) / (1.0 - r_ant);
              qstr = tr("SWR-Ant:","");
              s.sprintf(" %4.4f", s_ant);
              s = qstr + s;
              emit multiEditInsert(s);
            }
            s = "---------------------";
            emit multiEditInsert(s);
          }
        }
        if((sweepData.eoperatemode == eSWR) or (sweepData.eoperatemode == eSWRfeeder)){
          // SWR minimum anzeigen
          px1 = (mink1 * sweepData.schrittfrequenz ) + afrequenz;
          px1 = px1 / 1000000.0;
          qstr = tr("SWR min :","");
          s.sprintf(" %2.2f  %2.6f MHz", minswr, px1);
          s = qstr + s;
          emit multiEditInsert(s);
          // SWR maximum anzeigen
          px1 = (maxk1 * sweepData.schrittfrequenz ) + afrequenz;
          px1 = px1 / 1000000.0;
          qstr = tr("SWR max :","");
          s.sprintf(" %2.2f  %2.6f MHz", maxswr, px1);
          s = qstr + s;
          emit multiEditInsert(s);
          s = "---------------------";
          emit multiEditInsert(s);
        }
        if(sweepData.eoperatemode == eImpedance){
          // Impedanz minimum anzeigen
          px1 = (mink1 * sweepData.schrittfrequenz ) + afrequenz;
          px1 = px1 / 1000000.0;
          swv = (50.0 * minswr) - 50.0;
          qstr = tr("|Z| min. :","");
          s.sprintf(" %2.2f  %2.6f MHz", swv, px1);
          s = qstr + s;
          emit multiEditInsert(s);
          // SWR maximum anzeigen
          px1 = (maxk1 * sweepData.schrittfrequenz ) + afrequenz;
          px1 = px1 / 1000000.0;
          swv = (50.0 * maxswr) - 50.0;
          qstr = tr("|Z| max. :","");
          s.sprintf(" %2.2f  %2.6f MHz", swv, px1);
          s = qstr + s;
          emit multiEditInsert(s);
          s = "---------------------";
          emit multiEditInsert(s);
        }
      }
      break;
    }  
    //#####################################################################
    // Switch zuende
    //#####################################################################
    if(!sweepData.bhintergrund){
      if((mouseposx[cursorix] != 0) && (mouseposy[cursorix] != 0)){ //Cursorindex aktiv
      //Kursorfrequenz an nwt7linux.cpp uebermitteln
        emit setCursorFrequency(mousefrequenz);
        //qDebug("KursorFrequenz %f",mousefrequenz);
      }else{
        //0.0 an nwt7linux.cpp uebermitteln
        emit setCursorFrequency(0.0);
      }
      for(k=0;k<5;k++){
        //Mauskreuz zeigen aber nur wenn pos aktiv und keine Frequenzermittlung
        if((mouseposx[k] != 0) && (mouseposy[k] != 0) && !banfang && !bende){
          double dbz=8.0; //Mauszeigergroesse
          int bz = (int)((double)breite/430.0 * dbz); //Kursorgroesse automatisch vergroessern
          p->setPen(pendarkredsolid);
          // Kursor als Dreieck
          p->drawLine(mx1[k], my1[k], mx1[k]-bz/2, my1[k]-bz);
          p->drawLine(mx1[k], my1[k], mx1[k]+bz/2, my1[k]-bz);
          p->drawLine(mx1[k]+bz/2, my1[k]-bz, mx1[k]-bz/2, my1[k]-bz);
          p->drawLine(mx2[k], my2[k], mx2[k]-bz/2, my2[k]-bz);
          p->drawLine(mx2[k], my2[k], mx2[k]+bz/2, my2[k]-bz);
          p->drawLine(mx2[k]+bz/2, my2[k]-bz, mx2[k]-bz/2, my2[k]-bz);
          s.sprintf("%i", k+1);
          p->drawText(mx1[k]-2, my1[k]-bz-1, s);
          p->drawText(mx2[k]-2, my2[k]-bz-1, s);
        }
      }
    //*************************************************************************************
    //*************************************************************************************
    //*************************************************************************************
    //*************************************************************************************
    //  bitBlt(this, 0,0, pix);
      for(k=0;k<5;k++){
        mouseposx1[k] = mouseposx[k];
      }
    }
    // p->end();
  }
}

QString SweepCurveWidget::kursorsavstr(double pegel)
{
  double uvolt, power;
  QString s1, s2;
  
  if(sweepData.bsavdbm){
    s2.sprintf("%2.2fdBm", pegel);
    s1 = s1 + s2 + "; ";
  }
  if(sweepData.bsavdbuv){
    s2.sprintf("%2.2fdBuV", pegel + 106.9897);
    s1 = s1 + s2 + "; ";
  }
  if(sweepData.bsavuv){
    //aus dBm Volt errechnen
    uvolt = sqrt((pow(10.0 , pegel/10.0) * 0.001) * 50.0);
    //Messbereche automatisch umschalten
    if (uvolt > 0.999){
      s2.sprintf("%3.1f V", uvolt);
    }
    if (uvolt < 1.0){
      uvolt = uvolt * 1000.0;
      s2.sprintf("%3.1f mV", uvolt);
    }
    if (uvolt < 1.0){
      uvolt = uvolt * 1000.0;
      s2.sprintf("%3.1f uV", uvolt);
    }
    s1 = s1 + s2 + "; ";
  }
  if(sweepData.bsavwatt){
    uvolt = sqrt((pow(10.0 , pegel/10.0) * 0.001) * 50.0);
    power = uvolt * uvolt / 50.0;
    if (power > 0.999){
      s2.sprintf("%3.1f W", power);
      if (power > 999.999){
        s2.sprintf("%3.2f kW", power/1000.0);
      }
    }
    if (power < 1.0){
      power = power * 1000.0;
      s2.sprintf("%3.1f mW", power);
    }
    if (power < 1.0){
      power = power * 1000.0;
      s2.sprintf("%3.1f uW", power);
    }
    if (power < 1.0){
      power = power * 1000.0;
      s2.sprintf("%3.1f nW", power);
    }
    if (power < 1.0){
      power = power * 1000.0;
      s2.sprintf("%3.1f pW", power);
    }
    s1 = s1 + s2 + "; ";
  }
  s2 = s1;
  return s2;
}

double SweepCurveWidget::korrsavpegel(double p)
{
  #ifdef PAINTGRAF_DEBUG
  qDebug("WidgetWobbeln::korrsavpegel(double p)");
  #endif
  if(sweepData.ebbsav == e300b1)p = p + sweepData.psav300;
  if(sweepData.ebbsav == e7kb1)p = p + sweepData.psav7k;
  if(sweepData.ebbsav == e30kb1)p = p + sweepData.psav30k;
  if(sweepData.ebbsav == e300b2)p = p + sweepData.psav300;
  if(sweepData.ebbsav == e7kb2)p = p + sweepData.psav7k;
  if(sweepData.ebbsav == e30kb2)p = p + sweepData.psav30k;
  p = p + sweepData.psavabs;
  return p;
}

void SweepCurveWidget::setSweepData(TSweep wob)
{
  #ifdef PAINTGRAF_DEBUG
  qDebug("WidgetWobbeln::setSweepData()");
  #endif
  //Uebernahme der Wobbeldaten
  sweepData = wob;
  //Befehl zum Neuzeichnen ausloesen
  repaint();
}

void SweepCurveWidget::SweepDataReceipt(const TSweep& wob)
{
  //qDebug("WidgetWobbeln::wobbeldatenempfang()");
  //Uebernahme der Wobbeldaten
  sweepData = wob;
  kurve1.absanfang = sweepData.absanfang;
  kurve1.absende = sweepData.absende;
  kurve2.absanfang = sweepData.absanfang;
  kurve2.absende = sweepData.absende;
  kurve3.absanfang = sweepData.absanfang;
  kurve3.absende = sweepData.absende;
  kurve4.absanfang = sweepData.absanfang;
  kurve4.absende = sweepData.absende;
  refreshPixmap();
  //Befehl zum Neuzeichnen ausloesen
  //repaint();
}

void SweepCurveWidget::SweepDataCurve(const TMessure &amess)
{
  #ifdef PAINTGRAF_DEBUG
  qDebug("WidgetWobbeln::wobbeldatenkurve()");
  #endif
  sweepData.mess = amess;
  refreshCurve();
  repaint();
}

void SweepCurveWidget::empfangfrqmarken(const TFrqMarkers &afmarken)
{
  #ifdef PAINTGRAF_DEBUG
  qDebug("WidgetWobbeln::empfangfrqmarken()");
  #endif
  //Frequenzmarken uebergeben
  wfmarken = afmarken;
  refreshPixmap();
  repaint();
}

TSweep SweepCurveWidget::getSweepData(){
  #ifdef PAINTGRAF_DEBUG
  qDebug("WidgetWobbeln::getWobbeldaten()");
  #endif
  return sweepData;
}

void SweepCurveWidget::printDiagramm(const QFont &pfont, const QStringList &astrlist)
{
  QPrinter *print = new QPrinter;
  int xp = 40;
  int yp = 0;
  int ypmerk;
  int linespace = 20;
  unsigned int i;
  double mx, my/*, mxx*/;
  QString qs, qs1, qa, qe, qw, qm, qz;
  QFont afont;
  
  #ifdef PAINTGRAF_DEBUG
  qDebug("WidgetWobbeln::printdiagramm()");
  #endif
  QDateTime dt = QDateTime::currentDateTime();
  qs1 = dt.toString("dd MMMM yyyy, hh:mm");
  afont = pfont;
  refreshPixmap();
  repaint();
  if(print->isValid()){
    refreshPixmap();
    repaint();
    QPainter p(print);
    //Q3PaintDeviceMetrics metrics(p.device());
    //Print width surface determine
    mx = print->pageRect().width();
    //mxx = mx;
    my = print->pageRect().height();
    //Print margin left deduct
    //mx = mx - xp;
    //Width of the display window of the graphic
    double px = this->rect().width();
    
    mx = mx/px * 0.8;
    p.setFont(pfont);
    qs = "NWT 4 Linux & Windows  " + qs1;
    yp += linespace;
    yp += linespace;
    p.drawText(xp, yp, qs);
    yp += linespace;
    p.drawLine(xp, yp, mx - xp, yp);
    yp += linespace;
    
    qa.sprintf("%3.6f", double(sweepData.anfangsfrequenz)/1000000.0);
    qe.sprintf("%3.6f", double(((sweepData.cntpoints - 1) * sweepData.schrittfrequenz)+
                                       sweepData.anfangsfrequenz)/1000000.0);
    qw.sprintf("%4.3f", double(sweepData.schrittfrequenz)/1000.0);
    qm.sprintf("%4i", sweepData.cntpoints);
    qz.sprintf("%4i", sweepData.ztime * 10);
    
    qs = tr("Startfreq: ","") + 
         qa +
         tr(" MHz","") +
         "; " +
         tr("Stopfreq: ","") +
         qe +
         tr(" MHz","") +
         "; " +
         tr("<@>Schrittweite: ","") +
         qw +
         tr(" kHz","");
    
    p.drawText(xp, yp, qs);
    yp += linespace;
    
    qs = tr("Samples: ","") + 
         qm +
         "; " +
         tr("Interrupt: ","") +
         qz +
         tr(" uS","");
    
    p.drawText(xp, yp, qs);
    yp += linespace;
    //Linie ueber der Grafik
    p.drawLine(xp, yp, mx - xp, yp);
    yp += linespace;
    //Auf richtige Groesse bringen
//    p.scale(mx, mx * 0.9);
    p.drawPixmap(int(xp), int(yp), pix);
    yp += int(this->rect().height() * mx);
    //Groesse wieder restaurieren
//    p.scale(1/mx, 1/(mx * 0.9));
    //Linie unter dem Bild
    p.drawLine(xp, yp, mx - xp, yp);
    ypmerk = yp;
    if(astrlist.count() > 0){
      yp += linespace;
//      qDebug("yp=%i ",yp);
      qs = astrlist.first();
      p.drawText(xp, yp, qs);
      for(i=1; i < uint(astrlist.count()); i++){
        yp += linespace;
//        qDebug("yp=%i xp=%i", yp, xp);
        if((yp + linespace) > (int)my){
          yp = ypmerk;
          xp = xp + int(mx / 3.0);
          yp += linespace;
        }
        qs = astrlist.at(i);
        p.drawText(xp,yp, qs);
      }
      yp += linespace;
      //Abschlusslinie
  //    p.drawLine(xp, yp, print->width()-xp, yp);
    }
  }  
}

void SweepCurveWidget::printDiagramPdf(const QFont &pfont, const QStringList &astrlist)
{
//  QPrinter *print = new QPrinter(QPrinter::PrinterResolution);
  QPrinter *print = new QPrinter();
  int xp = 40;
  int yp = 0;
  int linespace = 20;
  int ypmerk;
  unsigned int i;
  QString qs, qs1, qa, qe, qw, qm, qz;
  QFont afont;
  double mx;
  int my;
  
  #ifdef PAINTGRAF_DEBUG
  qDebug("WidgetWobbeln::printdiagrammpdf()");
  #endif
  print->setOutputFormat(QPrinter::PdfFormat);
  print->setPageSize(QPrinter::A4);
  print->setOrientation(QPrinter::Portrait);
  QString s = QFileDialog::getSaveFileName(this, tr(" Save PDF",""),
                                                    "out.pdf","PDF (*.pdf *.PDF)");
  //Datei ueberpruefen ob Sufix vorhanden
  if (s.length() ==0) return;
  if((s.indexOf(".")== -1)) s += ".pdf";

  print->setOutputFileName(s);
  QDateTime dt = QDateTime::currentDateTime();
  qs1 = dt.toString("dd MMMM yyyy, hh:mm");
  afont = pfont;
  refreshPixmap();
  repaint();
  QPainter p(print);
//  QPaintDeviceMetrics metrics(p);
  //Druckblattbreite ermitteln
  mx = print->width();
  //Druckblatthoehe ermitteln
  my = print->height();
//  qDebug("breite=%f hoehe=%i", mx, my);
//  qDebug("breite=%i hoehe=%i",print->widthMM(), print->heightMM());
  //linken Druckrand abziehen
  mx = mx - xp;
  //breite des Anzeigefensters der Grafik
  double px = this->rect().width();
  
  mx = mx/px * 0.8;
  p.setFont(pfont);
  qs = "NWT 4 Linux & Windows  " + qs1;
  yp += linespace;
  yp += linespace;
  p.drawText(xp, yp, qs);
  yp += linespace;
  p.drawLine(xp, yp, print->width()-xp, yp);
  yp += linespace;
  
  qa.sprintf("%3.6f", double(sweepData.anfangsfrequenz)/1000000.0);
  qe.sprintf("%3.6f", double(((sweepData.cntpoints - 1) * sweepData.schrittfrequenz)+
                                      sweepData.anfangsfrequenz)/1000000.0);
  qw.sprintf("%4.3f", double(sweepData.schrittfrequenz)/1000.0);
  qm.sprintf("%4i", sweepData.cntpoints);
  qz.sprintf("%4i", sweepData.ztime * 10);
  
  qs = tr("Startfreq: ","") + qa +
       tr(" MHz","") + "; " +
       tr("Stopfreq: ","") + qe +
       tr(" MHz","") + "; " +
       tr("<@>Schrittweite: ","") + qw +
       tr(" kHz","");
  
  p.drawText(xp, yp, qs);
  yp += linespace;
  
  qs = tr("Samples: ","") + qm + "; " +
        tr("Interrupt: ","") + qz +
        tr(" uS","");
  
  p.drawText(xp, yp, qs);
  yp += linespace;
  //Linie ueber der Grafik
  p.drawLine(xp, yp, print->width()-xp, yp);
  yp += linespace;
  //Auf richtige Groesse bringen
//  p.scale(mx, mx * 0.9);
  p.drawPixmap(int(xp), int(yp), pix);
//  yp += int(this->rect().height() * mx)-50;
  yp += int(this->rect().height());
  //Groesse wieder restaurieren
//  p.scale(1/mx, 1/(mx * 0.9));
  //Linie unter dem Bild
  p.drawLine(xp, yp, print->width()-xp, yp);
  yp += linespace;
  ypmerk = yp;
  if(astrlist.count() > 0){
    yp += linespace;
//    qDebug("yp=%i ",yp);
    qs = astrlist.first();
    p.drawText(xp, yp, qs);
    for(i=1; i < uint(astrlist.count()); i++){
      yp += linespace;
//      qDebug("yp=%i xp=%i", yp, xp);
      if((yp + linespace) > (int)my){
        yp = ypmerk;
        xp = xp + (print->width() / 3);
        yp += linespace;
      }
      qs = astrlist.at(i);
      p.drawText(xp,yp, qs);
    }
    yp += linespace;
    //Abschlusslinie
//    p.drawLine(xp, yp, print->width()-xp, yp);
  }
}

void SweepCurveWidget::newBuildWidthInfo(const TBildinfo &abildinfo)
{
  int xp = 40;
  int yp = 60;
  int linespace = 20;
  QString qs;
  
  refreshPixmap();
  
  wbildinfo = abildinfo;
  xp = wbildinfo.posx;
  yp = wbildinfo.posy;
  linespace = int(1.2 * double(wbildinfo.fontsize));
  
  if(wbildinfo.infoanzeigen){
    QPainter p;
    //Beschriftetes Pixmap holen
    p.begin(&pix);
    QFont ifont;
    ifont.setPixelSize (wbildinfo.fontsize);
    p.setFont(ifont);
    //und Kurve einzeichnen
    for(uint i=0; i < uint(infolist.count()); i++){
      qs = infolist.at(i);
      //qDebug(qs);
      p.drawText(xp,yp, qs);
      yp += linespace;
    }
    p.end();
    //und anzeigen
    update(); //paintEvent erzeugen
  }
}

void SweepCurveWidget::bildspeichern(const TBildinfo &bildinfo, const QStringList &astrlist)
{
  //unsigned int i;
  QString qs, qs1;
  QFont afont;
  QString s;


  wbildinfo = bildinfo;
  infolist = astrlist;
  
  SaveCurveDlg *insinfo = new SaveCurveDlg(this);
  insinfo->setBildinfo(bildinfo);
  QObject::connect( insinfo, SIGNAL(pixauffrischen(const TBildinfo &)), this, SLOT(newBuildWidthInfo(const TBildinfo &)));

  bool ok = insinfo->exec();

  if(ok){
    QSettings settings("AFU", "NWT");
    QDir imgpath;
    imgpath.setPath(settings.value("imgpath", QDir::homePath()).toString());
    s = QFileDialog::getSaveFileName(this, tr("Save image",""),
        imgpath.path(),"PNG (*.png *.PNG);;BMP (*.bmp *.BMP);;JPG (*.jpg *.JPG)", &qs1);

    if(!s.isEmpty()){
      qDebug("save");
      //Datei ueberpruefen ob Sufix vorhanden
      if(!s.contains(".")){
        if(qs1.contains("PNG"))s += ".png";
        if(qs1.contains("BMP"))s += ".bmp";
        if(qs1.contains("JPG"))s += ".jpg";
      }
      QImage img(size(), QImage::Format_RGB32);
      render(&img);
      QImageWriter iw(s);
      iw.write(img);
      imgpath.setPath(s);
      settings.setValue("imgpath",imgpath.absolutePath());
    }
  }
  refreshPixmap();
  //qDebug(imgpath.absolutePath());
}

void SweepCurveWidget::closeEvent(QCloseEvent *event)
{
  event->ignore();
}

void SweepCurveWidget::keyPressEvent(QKeyEvent *event){
  int k;
  
  switch(event->key()){
    case Qt::Key_Up: if(sweepData.mess.daten_enable){ mouseposx[cursorix] = (this->rect().width()) / 2;} emit MagnifyPlus(); break;
    case Qt::Key_Down: if(sweepData.mess.daten_enable){ mouseposx[cursorix] = (this->rect().width()) / 2;} emit MagnifyMinus(); break;
    case Qt::Key_Left: 
      if(sweepData.mess.daten_enable){
        mouseposx[cursorix] = mouseposx[cursorix] - 5;
        if(!sweepData.mousesperre){
          refreshPixmap();
          repaint();
        }
      }
    break;
    case Qt::Key_Right:
      if(sweepData.mess.daten_enable){
        mouseposx[cursorix] = mouseposx[cursorix] + 5;
        if(!sweepData.mousesperre){
          refreshPixmap();
          repaint();
        }
      }
    break;
    case Qt::Key_Space: if(sweepData.mess.daten_enable){ mouseposx[cursorix] = (this->rect().width()) / 2;} emit MagnifyEqual(); break;
    case Qt::Key_Return:
      if(sweepData.mess.daten_enable){
        bnomove = FALSE;
        if(sweepData.mess.daten_enable){
          mouseposx[cursorix] = (this->rect().width()) / 2;
          mouseposy[cursorix] = (this->rect().height()) / 2;
          bnomove = TRUE;
          if(!sweepData.mousesperre){
            refreshPixmap();
            repaint();
          }
        }
      }
    break;
    case Qt::Key_1: setWCursorNr(4); emit resetCursor(4); break;
    case Qt::Key_2: setWCursorNr(3); emit resetCursor(3); break;
    case Qt::Key_3: setWCursorNr(2); emit resetCursor(2); break;
    case Qt::Key_4: setWCursorNr(1); emit resetCursor(1); break;
    case Qt::Key_5: setWCursorNr(0); emit resetCursor(0); break;
    case Qt::Key_0:
      for(k=0;k<5;k++){
        cursorix = k;
        resetMouseCursor();
      }  
//      menmouse->hide();
      setWCursorNr(4);
      emit resetCursor(4);
    break;
    case Qt::Key_W: emit SweepTimes(); break;
    case Qt::Key_E: emit SweepOnce(); break;
    case Qt::Key_S: emit SweepStop(); break;
    case Qt::Key_F: CreateQuarzFile(); break;
    case Qt::Key_D: WriteToQuarzDatei(squdaten); break;
    case Qt::Key_C: CloseQuarzFile(); break;
    default:QWidget::keyPressEvent(event);
  }
}

void SweepCurveWidget::CreateQuarzFile()
{
  qusl.clear();
  qunr = 1;
  QString qs = "Nr;Frequ;Loss(dB);B3dB(Hz)";
  qusl.append(qs);
  QString s = QFileDialog::getSaveFileName(this, "Crystal-File",
                    "crystallist.csv","CSV (*.csv *.CSV)");
  //Datei ueberpruefen ob Sufix vorhanden
  if(!s.isNull()){
    if((s.indexOf(".")== -1)) s += ".csv";
    fname = s;
    speichern = TRUE;
  }
}

void SweepCurveWidget::WriteToQuarzDatei(const QString &qs)
{
  QString s, as;
  
  if(speichern){
    s.sprintf("%i;",qunr);
    as = s + qs;
    qunr++;
    qusl.append(as);
    //qDebug(as);
  }
}

void SweepCurveWidget::CloseQuarzFile()
{
  QString qs;
  QStringList::Iterator it;
  QFile *f1;
  
  if(speichern){
    f1 = new QFile(fname);
    if(f1->open(QIODevice::WriteOnly)){
      QTextStream ts(f1);
      for(it = qusl.begin() ; it != qusl.end(); it++){
        qs = (*it).toLatin1();
        ts << qs << endl;
      }
      f1->close();
      delete(f1);
    }
  }
}






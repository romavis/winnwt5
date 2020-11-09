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

//#define LDEBUG

#ifndef NWT7MAIN_H
#define NWT7MAIN_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QTabWidget>
#include <QDial>
#include <QLCDNumber>
#include <QRadioButton>
#include <QLineEdit>
#include <QSpinBox>
#include <QFileDialog>
#include <QPainter>
#include <QPixmap>
#include <QCheckBox>
#include <QObject>
#include <QProgressBar>
#include <QDialog>
/*#include <Q3MultiLineEdit>*/
#include <QTextEdit>
#include <QFile>
#include <QFontDialog>
#include <QString>
#include <QDir>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>
#include <QDoubleSpinBox>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <qglobal.h>

#ifdef Q_OS_WIN
     #include "modem_win.h"
#else
     #include "modem_lin.h"
#endif


#include "sweepcurvewidget.h"
#include "optiondlg.h"
#include "fmarkedlg.h"
#include "firmwaredlg.h"
#include "markersdlg.h"

#include "constdef.h"

class QLabel;
class QMenuBar;

//void function nwtDelay(int ms);

/** NwtMain is the base class of the project */
class Nwt7MainClass : public QTabWidget
{
  Q_OBJECT 
  public:
    /** construtor */
    Nwt7MainClass(QWidget* parent=0);
    /** destructor */
    ~Nwt7MainClass();

    Modem *picmodem;
    void *curTimerPerform; //pointer to current timer started
    int verCounterError;
    void cleareVersionVariante();
//////////////////////////////////////////////////////////////////////////////
//Objects for measuring
    QWidget *nwt7measure;
    QWidget *wgrafik;
    QProgressBar *progressbaPowerMeter1;
    QProgressBar *progressbaPowerMeter2;
    QTimer *vmesstimer;
    QLabel *messlabel1;
    QLabel *messlabel2;
    QLabel *messlabel3;
    QLabel *messlabel4;
    QLabel *messlabel5;
    QLabel *messlabel6;
    QTextEdit  *messedit;
    QPushButton *buttonmess;
    QPushButton *buttonmesssave;
    QComboBox *ldampingk1;
    QComboBox *ldaempfungk2;
    QLabel *labelldaempfungk1;
    QLabel *labelldaempfungk2;
    
    QPushButton *buttonvfo;
    //QComboBox *cboxMessVfoMode;
    //QGroupBox *cboxMessVfoMode;
    QRadioButton *rbVfoOn;
    QRadioButton *rbMessVfoOn;
    QRadioButton *rbVFOSOff;
    QLabel *labellMessVfoState;
    QSpinBox *sp1hz;
    QSpinBox *sp10hz;
    QSpinBox *sp100hz;
    QSpinBox *sp1khz;
    QSpinBox *sp10khz;
    QSpinBox *sp100khz;
    QSpinBox *sp1mhz;
    QSpinBox *sp10mhz;
    QSpinBox *sp100mhz;
    QSpinBox *sp1ghz;
    QLabel *labelsphz;
    QLabel *labelspkhz;
    QLabel *labelspmhz;
//    QLabel *labelspghz;
    
//////////////////////////////////////////////////////////////////////////////
//Objects for sweep
    MarkersDlg *wkmanager;
    QWidget *nwt7sweep;
    SweepCurveWidget *grafik;
    
    QGroupBox *groupkanal,
              *groupbandbreite,
              *groupshift,
              *gbOperatedStart,
              *gbSweep,
              *gbAttenuator,
              *gbZoom,
              *groupkursor,
              *groupbar,
              *groupsa;
    
    QLineEdit *editanfang,
              *editende,
              *editschrittweite,
              *editschritte,
              *editdisplay,
              *editvfo;

    QLabel *labelanfang,
           *labelende,
           *labelschrittweite,
           *labelschritte,
           *labeldisplayshift,
           *labellupe,
           *labelprogressbar,
           *labelnwt,
           *labelvfo,
           *labelsafrqbereich,
           *labelsabandbreite,
           *labelfrqfaktor,
           *labelfrqfaktorv,
           *labelfrqfaktorm;

    QRadioButton *sabereich1;
    QRadioButton *sabereich2;

    QCheckBox *checkboxk1,
              *checkboxk2,
              *checkbox3db,
              *checkbox6db,
              *checkboxgrafik,
              *checkboxinvers,
              //*checkboxtime,
              *checkboxmesshang1,
              *checkboxmesshang2;
    
    QCheckBox *kalibrier1;
    QCheckBox *kalibrier2;

    QPushButton *buttonwobbeln,
                *buttoneinmal,
                *buttonstop,
                *buttonlupeminus,
                *buttonlupeplus,
                *buttonlupemitte;

    QTimer *vsweeptimer;  // do end sweep sweeptimer->timeout() -> readyttymain()
    QTimer *idletimer;
    QTimer *vtimer;
    QTimer *stimer;
    QTimer *vfomesstimer;
    
    
    QProgressBar *progressbar;
    
    QTextEdit *mledit;
    
    QStringList mleditlist;
    QComboBox *cbAttenuator1,
              *cbAttenuator2,
              *cbAttenuator3,
              *cbOperatedMode,
              *cbResolutionSWR,
              *boxdbshift1,
              *boxdbshift2,
              *boxkursornr,
              *boxwattoffset1,
              *boxwattoffset2,
              *boxprofil,
              *boxydbmax,
              *boxydbmin,
              *boxzwischenzeit;
    
    QLabel *labelboxydbmax;
    QLabel *labelboxydbmin;
    QLabel *labelboxdbshift1;
    QLabel *labelboxdbshift2;
    QLabel *labelkursornr;
    QLabel *labeldaempfung;
    QLabel *labeldaempfung1;
    QLabel *labeldaempfung3;
    QLabel *labelbetriebsart;
    QLabel *labelaufloesung;
    QLabel *labelprofil;
    QLabel *labelzwischenzeit;
    enumOperateMode ebetriebsart; //Betriebart wobbeln,swr,swrneu ....

    QLabel *labela_100;
    QLineEdit *edita_100;
    QLabel *labelkabellaenge;
    QLineEdit *editkabellaenge;

//////////////////////////////////////////////////////////////////////////////
//Objecte fuer VFO
    QWidget *nwt7vfo;
    QTimer *vfotimer;
    
    QSpinBox *vsp1hz;
    QSpinBox *vsp10hz;
    QSpinBox *vsp100hz;
    QSpinBox *vsp1khz;
    QSpinBox *vsp10khz;
    QSpinBox *vsp100khz;
    QSpinBox *vsp1mhz;
    QSpinBox *vsp10mhz;
    QSpinBox *vsp100mhz;
    QSpinBox *vsp1ghz;
    
    QLCDNumber *LCD1;
    QLCDNumber *LCD2;
    QLCDNumber *LCD3;
    QLCDNumber *LCD4;
    QLCDNumber *LCD5;
    QRadioButton *rb1;
    QRadioButton *rb2;
    QRadioButton *rb3;
    QRadioButton *rb4;
    QRadioButton *rb5;
    QLineEdit *editzf;
    QCheckBox *checkboxzf;
    QCheckBox *checkboxiqvfo;
    QLabel *labelzf;
    QLabel *labelhz;
    QLabel *lmhz;
    QLabel *lkhz;
    QLabel *lhz;
    QLabel *mlabelk1;
    QLabel *mlabelk2;
    QFileDialog *IniFileDialog;
//    optiondlg *dlg;
//////////////////////////////////////////////////////////////////////////////
//Objects for calculations
    QWidget *berechnung;
    
    QGroupBox *groupschwingkreisl;
    QLineEdit *editf1;
    QLineEdit *editc1;
    QLabel *ergebnisl;
    QLabel *labell1;
    QLabel *labelf1;
    QLabel *labelc1;
    QLineEdit *editn;
    QLabel *labeln;
    QLabel *ergebnisal;
    QLabel *labelal;
    
    QGroupBox *groupschwingkreisc;
    QLineEdit *editf2;
    QLineEdit *editl2;
    QLabel *ergebnisc;
    QLabel *labell2;
    QLabel *labelf2;
    QLabel *labelc2;
    QLabel *labelxc;
    QLabel *ergebnisxc;
    QLabel *labelxl;
    QLabel *ergebnisxl;
    
    QGroupBox *groupwindungen;
    QLineEdit *edital3;
    QLabel *labeledital3;
    QLineEdit *editl3;
    QLabel *labeleditl3;
    QLabel *ergebnisw;
    QLabel *labelergebnisw;

//Objects for calculations
//////////////////////////////////////////////////////////////////////////////
    
//////////////////////////////////////////////////////////////////////////////
//Objects for impedance
    QWidget *wimpedance;
    
    QGroupBox *gimp;
    QRadioButton *rbr;
    QRadioButton *rblc;
    
    QGroupBox *gzr;
    QLabel *labeleditz1;
    QLineEdit *editz1;
    QLabel *labeleditz2;
    QLineEdit *editz2;
    QLabel *labeleditz3;
    QLineEdit *editz3;
    QLabel *labeleditz4;
    QLineEdit *editz4;
    QLabel *lr1;
    QLabel *lbeschrr1;
    QLabel *lr2;
    QLabel *lbeschrr2;
    QLabel *lr3;
    QLabel *lbeschrr3;
    QLabel *lr4;
    QLabel *lbeschrr4;
    QLabel *ldaempfung;
    QLabel *lbeschrdaempfung;
    QLabel *bild1;
    
    QGroupBox *gzlc;
    QLabel *labeleditzlc1;
    QLineEdit *editzlc1;
    QLabel *labeleditzlc2;
    QLineEdit *editzlc2;
    QLabel *labeleditzlc3;
    QLineEdit *editzlc3;
    QLabel *ll;
    QLabel *lbeschrl;
    QLabel *lc;
    QLabel *lbeschrc;
    QLabel *bild2;
  //Objecte fuer Impedanz Ende
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  //Objects for antenna pattern
    QWidget *wdiagramm;
    QGroupBox *gdiagramm;
    QPushButton *bstart;
    QPushButton *bstop;
    QPushButton *bsave;
    QPushButton *bsim;
    QTimer *tdiagramm;
    QLabel *lanzeige;

    QGroupBox *gdiagrammdim;
    QComboBox *boxdbmax;
    QComboBox *boxdbmin;
    QLabel *ldbmax;
    QLabel *ldbmin;
    QDoubleSpinBox *spinadbegin;
    QDoubleSpinBox *spinadend;
    QSpinBox *spingradbegin;
    QSpinBox *spingradend;
    QLabel *lbegin;
    QLabel *lbegin1;
    QLabel *lend;
    QLabel *lend1;
  //Objects for antenna pattern end
  //////////////////////////////////////////////////////////////////////////////

  //////////////////////////////////////////////////////////////////////////////
  //Internal function to calculate and set the positions
    int ttyOpen(int showWarning);
    void resizeWidgets();
    void tip(bool);
    QString getConfFileName();
    void setconffilename(const QString &fname);
    TWattOffset wattoffsetk1[30];
    TWattOffset wattoffsetk2[30];
    TFProfile aprofil[100];
    int profilindex;
    void readSettings();
    void writeSettings();
    void setIdleDdsOff();
    void setDdsOff();
    void setProgramPath(const QString &);
    void setFrqFaktorLabel();
  
  public slots:
    void setimp();
    void zlcausrechnen();
    void zrausrechnen();
    void nkurve_loeschen();
    void wkmLoadCurve(int);
    void wkmSaveCurve(const TSweep &);
    void menuFirsSet();
    void loadConfigFileDlg();
    void saveConfigFileDlg();
    void loadCurves();
    void SaveCurves();
    void SaveCurvesMenu();
    //void drucklabel();
    void printDiagramm();
    void printDiagrammPdf();
    void saveDiagrammImage();
    void option();
    void firmupdate();
    void beenden();
    void version();
    void versionControl();
    void setWindowTitle(QString);
    void setShift();
    void setIQVFO();

    //for sweep
    void clickSweepContinuous(); //Sweep start
    void clickSweepOnce(); //A continuous sweep
    void clickSweepStop(); //Sweep stop
    //void kalibrierenswr();
    void calibratechanel1();
    void calibratechanel2();
    void messurechanel1();
    void messurechanel2();
    void messsondespeichern1(const QString&);
    void messsondespeichern2(const QString&);
    //
    void setInfoFont();
    void setFrequenzmarken();
    void editProfile();
    void setHLine();
    void openCurveManager();
    void getwkm(int);
    void resetCursor(int);
    
    void messsetfont();
    void mkalibrierenk1();
    void mkalibrierenk2();
    void editProbe1();
    void editProbe2();
    void writeMessureToTable();
    void saveTableToFile();
    void setDaempfungdefault();
    void mleditloeschen();
    void mleditinsert(const QString &);

//Functions for VFO
    void setAttItem();

//functions calculations

    void fzaehlen();

  private:
    QString filenamesonde1;
    QString filenamesonde2;
    eSaBandWith sabw;
    esabereich sabereich, sabereichalt;
    int fwversion;
    int fwversioncounter;
    int fwportb;
    int fwportbalt;
    int fwportbcounter;
    int fwsav;
    int fwvariante;
    bool verstep;
    int pfontsize;
    long anfangalt;
    long endealt;
    int schrittealt;
    bool sw305;
    bool sw410;
    bool bSetDamping;
    bool sweepdestroy;
    bool binfokeine;
    //bool bfliessend;
    int tab_nwt7vfo;
    int tab_nwt7measure;
    int tab_calculation;
    int tab_wimpedanz_match;
    int tab_wdiagramm;
    int tab_wkmanager;
    int tab_nwt7sweep;
    
    double woffset1, woffset2;
    bool btip;
    TFrqMarkers frqmarken;
    QString configfilename;
    TSweep wobdatenmerk;
    bool bcurveloaded;
    bool bSweepIsRun;
    QDir homedir;
    QDir kurvendir;

    QString infotext;
    QString displaystr;
    unsigned char linienr;
    bool bkalibrierenswr;
    bool bkalibrierenswrneu;
    bool bkalibrieren40;
    bool bkalibrieren0;
    double ym_40db;
    double ym_0db;
    double ym_gen;
    double ym_daempf;
    double KursorFrequenz;
    bool wobbelstop;
    bool bttyOpen;
    int lcdauswahl;
    QMenuBar *menu;
    TSweep sweepdata;
    char ttybuffer[maxmesspunkte * 4];
    int ttyindex;
    bool b10bit;
    int synclevel;
    int demolitionlevel;
    bool bersterstart;
    bool restartMessure; //always restart watt messure
    bool bmkalibrierenk1_0db;
    bool bmkalibrierenk1_20db;
    bool bmkalibrierenk2_0db;
    bool bmkalibrierenk2_20db;
    QFont messfont;
    QFont infofont;
    QFont printfont;
    bool btabelleschreiben;
    QStringList bstrlist;
    QStringList infoueber;
    TBaseData currentdata;
    double measureValCh1, measureValCh2;
    double measureCh1Peak, measureCh2Peak;
    int measureCounter;
    int messsync;
    bool bdatareturn;
    int messtime;
    int messtimeneu; //ab FW 120
    double maxkalibrier;
    int wcounter;
    double messvfofrequenz;
    bool messPanelState;
    int messVfoModeState;
    int counterPeak1;
    int counterPeak2;
    double lcdfrq1;
    double lcdfrq2;
    double lcdfrq3;
    double lcdfrq4;
    double lcdfrq5;
    TBildinfo nbildinfo;
    TDiagramArray diagrammarray;
    int dummycounter;
    int counterkurve;

    
    void testLogLin();
    void sweepInvalid();
    QString delPath(const QDir &s);
    void configLoad(const QString &filename);
    void configSave(const QString &filename);
    void messsondenfilek1laden(const QString &filename);
    void messsondenfilek2laden(const QString &filename);
    enumkurvenversion loadCurve();
    void startMessure();
    double getdrehgeber();
    void setFqToSetPanel(double inFq);
    void setFqFromLcdPanelToSetPanell();
    int defaultClearTty();
    double linenormalisieren(const QString &, bool positive_only);
    void sendSweepCmd();   //wobbelstring an NWT7 senden
    void setFrqBereich();
    void setColorRedk1();
    void setColorBlackk1();
    void setColorRedk2();
    void setColorBlackk2();
    void LCDaendern();
    void fsendsweepdata();
    void vfoOnOffControls();
    double getkalibrierwertk1(double afrequenz);
    double getkalibrierwertk2(double afrequenz);
    double getsavcalwert(double afrequenz);
    double getswrkalibrierwertk1(double afrequenz);
    double korrsavpegel(double p);
    QString programpath;

  private slots:
    void schwingkreisfl();
    void schwingkreisfc();
    void alwindungen();
    void setDisplayYmax(const QString &);
    void setDisplayYmin(const QString &);


    void IdleTimerStop();
    void IdleTimerStart();
    void IdleTimerPerform();
    void vTimerPerform();
    void sTimerPerform();

    void setAttenuationControlsIndex(int index);
    void setAttenuationControls_dB(int attenuation_db);

    //void setAttenuationSweep(int index);
    //void setDaempfungVfo(int index);
    //void setDaempfungWatt(int index);
    
    void setAttenuation(int controlCode);
    void setOperateMode(int index);
    void setSaBereich1();
    void setSaBereich2();
    void setAufloesung(int index);
    void setProfil(int index);
//    void setkdir();
//    void getkdir(QString);
/////////////////////////////////////////////////////////////////////////////
//Functions for all
    void tabAfterSwitch(int index);
    void setInitInterface(bool connected);
/////////////////////////////////////////////////////////////////////////////
//Functions for sweep
    void checkboxk1_checked(bool);
    void checkboxk2_checked(bool);
    void kalibrier1_checked(bool);
    void kalibrier2_checked(bool);
//    void checkboxswr_checked(bool);
    
    void SweepInit(); //Alle Eingaben pruefen
    void checkbox3db_checked(bool);
    void checkbox6db_checked(bool);
    void checkboxgrafik_checked(bool);
    void checkboxinvers_checked(bool);
    void checkboxtime_checked(int);
    void setFqStart(double);
    void setFqEnd(double);
    void setFqStep(double);
    void readberechnung(double);
    void setCursorFrequency(double);
    void swrantaendern();
    void clicklMagnifyPlus();
    void clicklMagnifyMinus();
    void clicklMagnifyEqual();
    
/////////////////////////////////////////////////////////////////////////////

    void editVFO();
    void setVfoToOutOnce();
    void VfoTimerPerform();
    void SweepTimerPerform();
    void lcd1clicked();
    void lcd2clicked();
    void lcd3clicked();
    void lcd4clicked();
    void lcd5clicked();
//    void warneichkorr();
    void change1hz(int);
    void change10hz(int);
    void change100hz(int);
    void change1khz(int);
    void change10khz(int);
    void change100khz(int);
    void change1mhz(int);
    void change10mhz(int);
    void change100mhz(int);
    void change1ghz(int);
    double checkVFOfrequency(double freq);

/////////////////////////////////////////////////////////////////////////////
//Functions for measuring
    void vMessTimerPerform();
    void setwattoffset1(int);
    void setwattoffset2(int);
    void MessPanelToggle();
    void setMessureVfoMode(bool messVfoMode);
    void setMeasuredVfoFqValue();
    void VfoMessTimerPerform();
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
//Functions for antenna pattern
    void diagrammstart();
    void diagrammstop();
    void diagrammsimulieren();
    void diagrammspeichern();
    void tdiagrammbehandlung();
    void diagrammdim(int);
    void diagrammdimd(double);
//
/////////////////////////////////////////////////////////////////////////////

protected:
   //virtual function must be redefined. Called in
   //sizes change the Windows main window
   virtual void keyPressEvent( QKeyEvent *event );
//   virtual void paintEvent( QPaintEvent * );
  signals:
    void sendsweepdata(const TSweep &);
    void sendsweepcurve(const TMessure &);
    void sendefrqmarken(const TFrqMarkers &);
    void drucken(const QFont &, const QStringList &);
    void druckenpdf(const QFont &, const QStringList &);
    void bildspeichern(const TBildinfo &, const QStringList &);
    void setmenu(emenuenable, bool);
    void setAppText(const QString &);
    void setWKursorNr(int);
    void setFontSize(int);
    void cursormitte();
};

#endif

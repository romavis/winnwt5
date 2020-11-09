/****************************************************************************
**
** Copyright (C) 2004-2006 Trolltech ASA. All rights reserved.
**
** This file is part of the example classes of the Qt Toolkit.
**
** This file may be used under the terms of the GNU General Public
** License version 2.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of
** this file.  Please review the following information to ensure GNU
** General Public Licensing requirements will be met:
** http://www.trolltech.com/products/qt/opensource.html
**
** If you are unsure which license is appropriate for your use, please
** review the following information:
** http://www.trolltech.com/products/qt/licensing.html or contact the
** sales department at sales@trolltech.com.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
****************************************************************************/

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QAction>
#include <QSettings>

#include "constdef.h"
#include "nwt7mainclass.h"

//class QTextEdit;
class Nwt7MainClass;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();
    void setProgramPath(QString);

protected:
    void closeEvent(QCloseEvent *event);
    virtual void paintEvent(QPaintEvent*);
    virtual void resizeEvent( QResizeEvent *);

public slots:
    void setText(QString);
    void readSettings();
    void loadConfig(char *name);
    void loadGeraet(char *name);
    void setFontSize(int s);

private slots:
    void mSetMenu(emenuenable,bool);

    void printDiagramm();
    void printDiagrammPdf();
    void bildSave();

    void saveSetting();
    void backupSetting();
    
    void geraet_neu();

    void mcLoadCurves();
    void mcSaveCurves();

    void showInfoDialog();
    void openOptionDialog();
    void updateFirmware();

    void clickSweepStartOnce(); //Wobbeln starten
    void clickSweepStartTimes(); //Einen Durchlauf wobbeln
    void clickSweepStop(); //Wobbeln stoppen
    void callibrateChanel1();
    void callibrateChanel2();
    void messsondeladen1();
    void messsondeladen2();
    void messsondespeichern1();
    void messsondespeichern2();
    void setInfoFont();
    void setWidth();
    
    void messsetfont();
    void mkalibrierenk1();
    void mkalibrierenk2();
    void editProbe1();
    void editProbe2();
    void tabelleschreiben();
    void tabellespeichern();
    void setDaempfungdefault();
    void beenden();
    void version();
    void Count();
    void tooltip();
    void setFrequenzmarken();
    void editProfile();
    void setHLine();

private:
    void createActions();
    void createMenus();
    void createToolBars();
    void createStatusBar();
    void createToolTip(bool);
    void writeSettings();

    bool bbreite;


//    QTextEdit *textEdit;
    Nwt7MainClass *nwtMainObj;
    QString curFile;
    QString programpath;
    QString infotext;
    QString infoversion;

    QMenu *fileMenu;
    QMenu *menuView;
    QMenu *editMenu;
    QMenu *helpMenu;
    QMenu *menuFile;
    QMenu *menuSetting;
    QMenu *menuGeraet;
    QMenu *menuCurves;
    QMenu *menuSweep;
//    QMenu *menuvfo;
    QMenu *menuMeasure;
#ifdef fzaehler
    QMenu *menuzaehlen;
#endif    
    QMenu *menuHelp;

    QToolBar *tbFile;
    QToolBar *tbSetting;
    QToolBar *tbProbes;
    QToolBar *tbCurves;
    QAction *newAct;
    QAction *openAct;
    QAction *saveAct;
    QAction *saveAsAct;
    QAction *exitAct;
    QAction *cutAct;
    QAction *copyAct;
    QAction *pasteAct;
    QAction *aboutAct;
    QAction *aboutQtAct;

    QAction *mPrint;
    QAction *mPrintPdf;
    QAction *mSavePng;
    QAction *mEnd;
    
    QAction *mLoadNewConfig;
    QAction *mSaveConfToFile;
    
    QAction *mGeraetNeu;
    
    QAction *mInfo;
    QAction *mOption;
    QAction *mFirmware;
    QAction *mCurveLoad;
    QAction *mCurveSave;
    QAction *mSweepContinous;
    QAction *mSweepOnce;
    QAction *mStop;
    QAction *mCalibrateK1;
    QAction *mCalibrateK2;
    QAction *mLoadCallibrateK1;
    QAction *mLoadCallibrateK2;
    QAction *mSaveCallibrateK1;
    QAction *mSaveCallibrateK2;
    QAction *mFontInfo;
    QAction *mFrequencyMark;
    QAction *mProfile;
    QAction *mVFODreh1;
    QAction *mVFODreh2;
    QAction *mVFODreh3;
    QAction *mWattFont;
    QAction *mWattCallibrateK1;
    QAction *mWattCallibrateK2;
    QAction *mWattSave;
    QAction *mWattEditprobe1;
    QAction *mWattEditprobe2;
    QAction *mTableAsFileSave;
    QAction *mSaveToDefault;
    QAction *mVersion;
    QAction *mTooltip;
    QAction *mKurvenManager;
    QAction *mWidth;
    QAction *mHLine;
    
#ifdef fzaehler
    QAction *mZaehlen;
#endif    

    bool bstart;
};

#endif

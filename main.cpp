/***************************************************************************

    main.cpp  -  description
    ----------------------------------------------------
    begin                : 2003
    copyright            : (C) 2007 by Andreas Lindenau
    email                : DL4JAL@darc.de

    main.cpp  -  description
    ----------------------------------------------------
    begin                : 2013
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

#include <QApplication>
#include <QtGlobal>
#include <QLocale>
#include <QTranslator>
#include <QString>
#include <QtGui>
#include <QDir>

#include "mainwindow.h"

// FIXME: 1. Прыгает окно с графиком по ширине, когда давишь открытие диалога сохранения PDF.

int appstart;

int main(int argc, char *argv[])
{
  int indexhfc = 0, indexqm = 0;
  QString s;
  char c[]="hfm9.hfc";
  QApplication app(argc, argv);
  QTranslator appTranslator;
  
  QDir dir(argv[0]);
  QString programpath(dir.absolutePath());
  //qDebug(programpath);
  appstart = 1;

  //Load language and config files
  if (argc > 1)
  {
    s = argv[1];
    if(s.contains(".qm"))indexqm = 1;
    if(s.contains(".hfc"))indexhfc = 1;
  }
  if (argc > 2)
  {
    s = argv[2];
    if(s.contains(".qm"))indexqm = 2;
    if(s.contains(".hfc"))indexhfc = 2;
  }

  if(indexqm){
    //qDebug("QM Loaded");
    s = argv[indexqm];
    //qDebug(s);
    if(appTranslator.load(s))qDebug("OK");
    app.installTranslator(&appTranslator); 
  }
  //Show main window
  app.setQuitOnLastWindowClosed(true);
  MainWindow *mainWin = new MainWindow();
  mainWin->setWindowIcon(QIcon(":images/appicon.png"));
  mainWin->show();
  //The path for configuration files set
  mainWin->setProgramPath(programpath);
  if(indexhfc){
    mainWin->loadConfig(argv[indexhfc]);
  }else{
    mainWin->loadConfig(c);
  }
  appstart = 0;
  return app.exec();
}

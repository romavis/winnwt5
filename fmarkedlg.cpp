/***************************************************************************

    fmarkedlg.cpp  -  description
    ----------------------------------------------------
    begin                : 2005
    copyright            : (C) 2005 by Andreas Lindenau
    email                : DL4JAL@darc.de

    fmarkedlg.cpp have been refactored to fmarkedlg.cpp QT5
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
#include <QString>

#include "fmarkedlg.h"
#include "modem.hpp"


//#####################################################################################
// Class fuer Option
//#####################################################################################
FreqMarkerDlg::FreqMarkerDlg(QWidget* parent, Qt::WindowFlags f): QDialog(parent, f)
{
  int a = 32;

  this->resize(330,10+a*16);
  this->setMinimumSize(250,250);
  this->setWindowTitle(tr("Set Frequency markers",""));
  ok = new QPushButton(tr("OK",""), this);
  ok->setGeometry(50,10+a*15,100,30);
  connect(ok, SIGNAL(clicked()), SLOT(accept()));
  cancel = new QPushButton(tr("Cancel",""), this);
  cancel->setGeometry(180,10+a*15,100,30);
  connect(cancel, SIGNAL(clicked()), SLOT(reject()));
  

  check160m = new QCheckBox(this);
  check160m->setGeometry(10,10,20,30);
  check80m = new QCheckBox(this);
  check80m->setGeometry(10,10+a,20,30);
  check40m = new QCheckBox(this);
  check40m->setGeometry(10,10+a*2,20,30);
  check30m = new QCheckBox(this);
  check30m->setGeometry(10,10+a*3,20,30);
  check20m = new QCheckBox(this);
  check20m->setGeometry(10,10+a*4,20,30);
  check17m = new QCheckBox(this);
  check17m->setGeometry(10,10+a*5,20,30);
  check15m = new QCheckBox(this);
  check15m->setGeometry(10,10+a*6,20,30);
  check12m = new QCheckBox(this);
  check12m->setGeometry(10,10+a*7,20,30);
  check10m = new QCheckBox(this);
  check10m->setGeometry(10,10+a*8,20,30);
  check6m = new QCheckBox(this);
  check6m->setGeometry(10,10+a*9,20,30);
  check2m = new QCheckBox(this);
  check2m->setGeometry(10,10+a*10,20,30);
  checku1 = new QCheckBox(this);
  checku1->setGeometry(10,10+a*11,20,30);
  checku2 = new QCheckBox(this);
  checku2->setGeometry(10,10+a*12,20,30);
  checku3 = new QCheckBox(this);
  checku3->setGeometry(10,10+a*13,20,30);
  s160m = new QLineEdit(this);
  s160m->setGeometry(30,10,60,30);
  s80m = new QLineEdit(this);
  s80m->setGeometry(30,10+a,60,30);
  s40m = new QLineEdit(this);
  s40m->setGeometry(30,10+a*2,60,30);
  s30m = new QLineEdit(this);
  s30m->setGeometry(30,10+a*3,60,30);
  s20m = new QLineEdit(this);
  s20m->setGeometry(30,10+a*4,60,30);
  s17m = new QLineEdit(this);
  s17m->setGeometry(30,10+a*5,60,30);
  s15m = new QLineEdit(this);
  s15m->setGeometry(30,10+a*6,60,30);
  s12m = new QLineEdit(this);
  s12m->setGeometry(30,10+a*7,60,30);
  s10m = new QLineEdit(this);
  s10m->setGeometry(30,10+a*8,60,30);
  s6m = new QLineEdit(this);
  s6m->setGeometry(30,10+a*9,60,30);
  s2m = new QLineEdit(this);
  s2m->setGeometry(30,10+a*10,60,30);
  su1 = new QLineEdit(this);
  su1->setGeometry(30,10+a*11,60,30);
  su2 = new QLineEdit(this);
  su2->setGeometry(30,10+a*12,60,30);
  su3 = new QLineEdit(this);
  su3->setGeometry(30,10+a*13,60,30);
  l160m1 = new QLineEdit(this);
  l160m1->setGeometry(110,10,100,30);
  l80m1 = new QLineEdit(this);
  l80m1->setGeometry(110,10+a,100,30);
  l40m1 = new QLineEdit(this);
  l40m1->setGeometry(110,10+a*2,100,30);
  l30m1 = new QLineEdit(this);
  l30m1->setGeometry(110,10+a*3,100,30);
  l20m1 = new QLineEdit(this);
  l20m1->setGeometry(110,10+a*4,100,30);
  l17m1 = new QLineEdit(this);
  l17m1->setGeometry(110,10+a*5,100,30);
  l15m1 = new QLineEdit(this);
  l15m1->setGeometry(110,10+a*6,100,30);
  l12m1 = new QLineEdit(this);
  l12m1->setGeometry(110,10+a*7,100,30);
  l10m1 = new QLineEdit(this);
  l10m1->setGeometry(110,10+a*8,100,30);
  l6m1 = new QLineEdit(this);
  l6m1->setGeometry(110,10+a*9,100,30);
  l2m1 = new QLineEdit(this);
  l2m1->setGeometry(110,10+a*10,100,30);
  lu11 = new QLineEdit(this);
  lu11->setGeometry(110,10+a*11,100,30);
  lu21 = new QLineEdit(this);
  lu21->setGeometry(110,10+a*12,100,30);
  lu31 = new QLineEdit(this);
  lu31->setGeometry(110,10+a*13,100,30);
  l160m2 = new QLineEdit(this);
  l160m2->setGeometry(220,10,100,30);
  l80m2 = new QLineEdit(this);
  l80m2->setGeometry(220,10+a,100,30);
  l40m2 = new QLineEdit(this);
  l40m2->setGeometry(220,10+a*2,100,30);
  l30m2 = new QLineEdit(this);
  l30m2->setGeometry(220,10+a*3,100,30);
  l20m2 = new QLineEdit(this);
  l20m2->setGeometry(220,10+a*4,100,30);
  l17m2 = new QLineEdit(this);
  l17m2->setGeometry(220,10+a*5,100,30);
  l15m2 = new QLineEdit(this);
  l15m2->setGeometry(220,10+a*6,100,30);
  l12m2 = new QLineEdit(this);
  l12m2->setGeometry(220,10+a*7,100,30);
  l10m2 = new QLineEdit(this);
  l10m2->setGeometry(220,10+a*8,100,30);
  l6m2 = new QLineEdit(this);
  l6m2->setGeometry(220,10+a*9,100,30);
  l2m2 = new QLineEdit(this);
  l2m2->setGeometry(220,10+a*10,100,30);
  lu12 = new QLineEdit(this);
  lu12->setGeometry(220,10+a*11,100,30);
  lu22 = new QLineEdit(this);
  lu22->setGeometry(220,10+a*12,100,30);
  lu32 = new QLineEdit(this);
  lu32->setGeometry(220,10+a*13,100,30);
};

void FreqMarkerDlg::setdaten(TFrqMarkers afmarken){
  QString qs;

  fmarken = afmarken;
  qs.setNum(fmarken.f1160m);
  l160m1->setText(qs);
  qs.setNum(fmarken.f2160m);
  l160m2->setText(qs);
  check160m->setChecked(fmarken.b160m);
  qs.setNum(fmarken.f180m);
  l80m1->setText(qs);
  qs.setNum(fmarken.f280m);
  l80m2->setText(qs);
  check80m->setChecked(fmarken.b80m);
  qs.setNum(fmarken.f140m);
  l40m1->setText(qs);
  qs.setNum(fmarken.f240m);
  l40m2->setText(qs);
  check40m->setChecked(fmarken.b40m);
  qs.setNum(fmarken.f130m);
  l30m1->setText(qs);
  qs.setNum(fmarken.f230m);
  l30m2->setText(qs);
  check30m->setChecked(fmarken.b30m);
  qs.setNum(fmarken.f120m);
  l20m1->setText(qs);
  qs.setNum(fmarken.f220m);
  l20m2->setText(qs);
  check20m->setChecked(fmarken.b20m);
  qs.setNum(fmarken.f117m);
  l17m1->setText(qs);
  qs.setNum(fmarken.f217m);
  l17m2->setText(qs);
  check17m->setChecked(fmarken.b17m);
  qs.setNum(fmarken.f115m);
  l15m1->setText(qs);
  qs.setNum(fmarken.f215m);
  l15m2->setText(qs);
  check15m->setChecked(fmarken.b15m);
  qs.setNum(fmarken.f112m);
  l12m1->setText(qs);
  qs.setNum(fmarken.f212m);
  l12m2->setText(qs);
  check12m->setChecked(fmarken.b12m);
  qs.setNum(fmarken.f110m);
  l10m1->setText(qs);
  qs.setNum(fmarken.f210m);
  l10m2->setText(qs);
  check10m->setChecked(fmarken.b10m);
  qs.setNum(fmarken.f16m);
  l6m1->setText(qs);
  qs.setNum(fmarken.f26m);
  l6m2->setText(qs);
  check6m->setChecked(fmarken.b6m);
  qs.setNum(fmarken.f12m);
  l2m1->setText(qs);
  qs.setNum(fmarken.f22m);
  l2m2->setText(qs);
  check2m->setChecked(fmarken.b2m);
  qs.setNum(fmarken.f1u1);
  lu11->setText(qs);
  qs.setNum(fmarken.f2u1);
  lu12->setText(qs);
  checku1->setChecked(fmarken.bu1);
  qs.setNum(fmarken.f1u2);
  lu21->setText(qs);
  qs.setNum(fmarken.f2u2);
  lu22->setText(qs);
  checku2->setChecked(fmarken.bu2);
  qs.setNum(fmarken.f1u3);
  lu31->setText(qs);
  qs.setNum(fmarken.f2u3);
  lu32->setText(qs);
  checku3->setChecked(fmarken.bu3);
  s160m->setText(fmarken.st160m);
  s80m->setText(fmarken.st80m);
  s40m->setText(fmarken.st40m);
  s30m->setText(fmarken.st30m);
  s20m->setText(fmarken.st20m);
  s17m->setText(fmarken.st17m);
  s15m->setText(fmarken.st15m);
  s12m->setText(fmarken.st12m);
  s10m->setText(fmarken.st10m);
  s6m->setText(fmarken.st6m);
  s2m->setText(fmarken.st2m);
  su1->setText(fmarken.stu1);
  su2->setText(fmarken.stu2);
  su3->setText(fmarken.stu3);
};

TFrqMarkers FreqMarkerDlg::getdaten(){
  QString qs;
  qs = l160m1->text();
  fmarken.f1160m = qs.toLongLong();
  qs = l160m2->text();
  fmarken.f2160m = qs.toLongLong();
  fmarken.b160m = check160m->isChecked();
  qs = l80m1->text();
  fmarken.f180m = qs.toLongLong();
  qs = l80m2->text();
  fmarken.f280m = qs.toLongLong();
  fmarken.b80m = check80m->isChecked();
  qs = l40m1->text();
  fmarken.f140m = qs.toLongLong();
  qs = l40m2->text();
  fmarken.f240m = qs.toLongLong();
  fmarken.b40m = check40m->isChecked();
  qs = l30m1->text();
  fmarken.f130m = qs.toLongLong();
  qs = l30m2->text();
  fmarken.f230m = qs.toLongLong();
  fmarken.b30m = check30m->isChecked();
  qs = l20m1->text();
  fmarken.f120m = qs.toLongLong();
  qs = l20m2->text();
  fmarken.f220m = qs.toLongLong();
  fmarken.b20m = check20m->isChecked();
  qs = l17m1->text();
  fmarken.f117m = qs.toLongLong();
  qs = l17m2->text();
  fmarken.f217m = qs.toLongLong();
  fmarken.b17m = check17m->isChecked();
  qs = l15m1->text();
  fmarken.f115m = qs.toLongLong();
  qs = l15m2->text();
  fmarken.f215m = qs.toLongLong();
  fmarken.b15m = check15m->isChecked();
  qs = l12m1->text();
  fmarken.f112m = qs.toLongLong();
  qs = l12m2->text();
  fmarken.f212m = qs.toLongLong();
  fmarken.b12m = check12m->isChecked();
  qs = l10m1->text();
  fmarken.f110m = qs.toLongLong();
  qs = l10m2->text();
  fmarken.f210m = qs.toLongLong();
  fmarken.b10m = check10m->isChecked();
  qs = l6m1->text();
  fmarken.f16m = qs.toLongLong();
  qs = l6m2->text();
  fmarken.f26m = qs.toLongLong();
  fmarken.b6m = check6m->isChecked();
  qs = l2m1->text();
  fmarken.f12m = qs.toLongLong();
  qs = l2m2->text();
  fmarken.f22m = qs.toLongLong();
  fmarken.b2m = check2m->isChecked();
  qs = lu11->text();
  fmarken.f1u1 = qs.toLongLong();
  qs = lu12->text();
  fmarken.f2u1 = qs.toLongLong();
  fmarken.bu1 = checku1->isChecked();
  qs = lu21->text();
  fmarken.f1u2 = qs.toLongLong();
  qs = lu22->text();
  fmarken.f2u2 = qs.toLongLong();
  fmarken.bu2 = checku2->isChecked();
  qs = lu31->text();
  fmarken.f1u3 = qs.toLongLong();
  qs = lu32->text();
  fmarken.f2u3 = qs.toLongLong();
  fmarken.bu3 = checku3->isChecked();
  fmarken.st160m.left(8) = s160m->text();
  fmarken.st80m = s80m->text();
  fmarken.st40m = s40m->text();
  fmarken.st30m = s30m->text();
  fmarken.st20m = s20m->text();
  fmarken.st17m = s17m->text();
  fmarken.st15m = s15m->text();
  fmarken.st12m = s12m->text();
  fmarken.st10m = s10m->text();
  fmarken.st6m = s6m->text();
  fmarken.st2m = s2m->text();
  fmarken.stu1 = su1->text();
  fmarken.stu2 = su2->text();
  fmarken.stu3 = su3->text();
  return fmarken;
};

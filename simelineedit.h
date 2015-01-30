/***************************************************************************
                               simelineedit.h  -
                             -------------------
    begin                : Sat May 12 23:25:00 CET 2001
    copyright            : (C) 2000-2001 by Simon Edwards
    email                : simon@simonzone.com
 ***************************************************************************/
/***************************************************************************
    Converted to a pure Qt5 application
    begin                : December 2014
    by                   : Antonio Martins
    email                : digiplan.pt@gmail.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SIMELINEEDIT_H
#define SIMELINEEDIT_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QLineEdit>

class SimeLineEdit : public QLineEdit {
public:
    SimeLineEdit(QWidget *parent);
    ~SimeLineEdit();
protected:
    virtual void keyPressEvent(QKeyEvent *e);
    virtual void focusOutEvent();
};

#endif

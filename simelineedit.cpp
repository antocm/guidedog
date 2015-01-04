/***************************************************************************
                               simelineedit.cpp  -
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

#include "simelineedit.h"

#include <QValidator>
#include <QKeyEvent>

///////////////////////////////////////////////////////////////////////////
SimeLineEdit::SimeLineEdit(QWidget *parent) : QLineEdit(parent) {
}

///////////////////////////////////////////////////////////////////////////
SimeLineEdit::~SimeLineEdit() {
}

///////////////////////////////////////////////////////////////////////////
// This is just a small modification for when the user hits the return key.
// We just give the focus back to the parent widget.
void SimeLineEdit::keyPressEvent(QKeyEvent *e) {
    QString t;
    
    t = text();
    QLineEdit::keyPressEvent(e);
    if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {
        e->accept();
        if (t != text()) {
            emit textChanged(text());   // This is bullshit. If keyPressEvent() changed the text
                // then I expect a textChanged() signal to be emitted. I should not have to DIY.
        }
        if (parentWidget() != 0) {
            parentWidget()->setFocus();
        }
    }
}

///////////////////////////////////////////////////////////////////////////
void SimeLineEdit::focusOutEvent() {
    const QValidator *v;
    QString t;
    int pos;

    v = validator();
    if (v != 0) {
        t = text();
        pos = 0;
        if (v->validate(t, pos) != QValidator::Acceptable) {
            v->fixup(t);
            setText(t);
        }
    }
}

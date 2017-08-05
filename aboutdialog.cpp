/***************************************************************************
                          aboutdialog.cpp  -  description
                             -------------------
    begin                : Dec 2014
    copyright            : (C) 2001 by Simon Edwards
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

#include "guidedog.h"
#include "aboutdialog.h"
#include "ui_aboutdialog.h"


/*!
 * \brief AboutDialog::AboutDialog
 * \param parent
 */
AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AboutDialog)
{
    ui->setupUi(this);
    ui->guidedogLabel->setText(QString("Guidedog ").append(GUIDEDOG_VERSION));
}


/*!
 * \brief AboutDialog::~AboutDialog
 */
AboutDialog::~AboutDialog()
{
    delete ui;
}


/*!
 * \brief AboutDialog::on_closeButton_clicked
 */
void AboutDialog::on_closeButton_clicked()
{
    close();
}

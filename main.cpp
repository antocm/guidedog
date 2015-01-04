/***************************************************************************
                          main.cpp  -  description
                             -------------------
    begin                : Thu Sep 27 22:23:16 CEST 2001
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

#include <QApplication>
#include <QMessageBox>
// #include <QCommandLineParser>
#include <unistd.h>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    // QCommandLineParser parser;
    // parser.addHelpOption();
    // parser.addVersionOption();
    // parser.addPositionalArgument("file", QObject::tr("main", "The file to open."));
    // parser.process(app);
    GuideDogApp w;
    if (w.initialize(getuid() == 0)) {
        w.show();
        if (getuid() != 0) {
            QMessageBox::information(0, QObject::tr("Information - Guidedog"),
                QObject::tr(
                "Since you do not have superuser privileges, Guidedog is\n"
                "running with reduced functionality. Scripts may be \n"
                "Imported/Exported, but  the system's networking settings\n"
                "may not be changed.\n"));
        }

        return app.exec();
    }

}

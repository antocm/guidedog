/***************************************************************************
                          commandrunner.h  -  description
                             -------------------
    begin                : Sat Jul 14 2001
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

#ifndef COMMANDRUNNER_H
#define COMMANDRUNNER_H

#include <QDialog>
#include <QProcess>
#include <QLabel>
#include <QTextEdit>


/*!
 * \brief The CommandRunner class
 */
class CommandRunner : public QDialog  {
    Q_OBJECT
public:
    CommandRunner(QWidget *parent = 0);
	~CommandRunner();
    void run(QString cmd);
    void pkexecRun(QString cmd);
    void setHeading(const QString &heading);

private slots:
    void slotKidExited(int exitCode, QProcess::ExitStatus exitStatus);
    void slotReceivedStdout();
    void slotReceivedStderr();
    void slotOkClicked();

private:
    QPushButton *okbutton;
    QLabel *headinglabel;
    QLabel *labelIcon;
    QTextEdit *outputview;

    QProcess *kid;
    QString command;
    QString output;
    bool running;
};

#endif

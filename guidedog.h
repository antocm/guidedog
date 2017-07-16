/***************************************************************************
                          guidedog.h  -  description
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

#ifndef GUIDEDOG_H
#define GUIDEDOG_H

#include "simelineedit.h"
#include "guidedogdoc.h"

#include <QDialog>
#include <QListWidget>
#include <QCheckBox>
#include <QValidator>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#define SYSTEM_RC_GUIDEDOG "/etc/rc.guidedog"

class AddressValidator : public QValidator {
    Q_OBJECT
public:
    explicit AddressValidator(QWidget *parent = 0);
    ~AddressValidator();

    virtual State validate(QString &input, int & pos) const;
    virtual void fixup(QString &input) const;
};

class IPValidator : public QValidator {
    Q_OBJECT
public:
    explicit IPValidator(QWidget *parent = 0);
    ~IPValidator();

    virtual State validate(QString &input, int & pos) const;
    virtual void fixup(QString &input) const;
};

namespace Ui {
class GuideDogApp;
}

/** Guidedog is the base class of the project */
class GuideDogApp : public QDialog {
    Q_OBJECT
public:
    explicit GuideDogApp(QWidget *parent = 0);
    ~GuideDogApp();

    // bool initialize(bool god);
    bool initialize();
    void openDefault();
    bool applyScript(bool warnfirst);
    bool resetSystemConfiguration();

protected:
    void saveOptions();
    void readOptions();

public slots:
    // Dialog main buttons
    void slotOk();
    void slotApply();
    void slotCancel();
    void slotAbout();
    void slotHelp();

    // Routing
    void slotRouting(bool on);
    void slotMasquerade(bool on);
    void slotMasqueradeFTP(bool on);
    void slotMasqueradeIRC(bool on);

    void slotNoMasqueradeListBox(QListWidgetItem *item);
    void slotNewAddressButton();
    void slotDeleteAddressButton();
    void slotAddressLineEdit(const QString &s);
    void slotAddressLineEditReturn();

    // Forwarding
    void slotForwardListBox(QListWidgetItem *item);
    void slotNewForwardButton();
    void slotDeleteForwardButton();
    // Original destination
    void slotOriginalPortSpinBox(int x);
    void slotOriginalMachineRadio();
    void slotOriginalSpecifyRadio();
    void slotOriginalSpecifyLineEdit(const QString &);
    void slotPortProtocolComboBox(int x);
    // New destination
    void slotNewMachineRadio();
    void slotNewSpecifyRadio();
    void slotNewSpecifyLineEdit(const QString &s);
    void slotNewPortSpinBox(int x);
    // Comment
    void slotCommentLineEdit(const QString &);
                 
    // Import/Export
    void slotDisableGuidedog(bool on);
    void slotImportButton();
    void slotExportButton();
    void slotDescriptionChanged();

private:
    // bool superusermode;
    bool systemconfigmodified;
	bool waspreviousconfiguration;

    GuidedogDoc *doc;
    bool updatinggui;

    void syncGUIFromDoc();

    void enabledGUIStuff();
    void setForwardRule(const GuidedogPortForwardRule *rule);

    Ui::GuideDogApp *ui;
    // The widgets that were previously here have been replaced by the ones in the ui
    AddressValidator *addressvalidator;
    IPValidator *originalspecifyaddressvalidator;
    IPValidator *newspecifyaddressvalidator;
    QSize commandrunnersize;
};

#endif // GUIDEDOG_H

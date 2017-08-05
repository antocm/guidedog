/***************************************************************************
                          guidedog.cpp  -  description
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
#include "ui_guidedogwindow.h"
#include "commandrunner.h"
#include "guidedogdoc.h"
#include "aboutdialog.h"

#include <unistd.h>
#include <QGroupBox>
#include <QMessageBox>
#include <QFileDialog>
#include <QTemporaryFile>
#include <QSettings>
#include <QDesktopServices>


///////////////////////////////////////////////////////////////////////////
/// - Class AddressValidator
///////////////////////////////////////////////////////////////////////////


/*!
 * \brief AddressValidator::AddressValidator Constructor
 * \param parent
 */
AddressValidator::AddressValidator(QWidget *parent) : QValidator(parent) {
}


/*!
 * \brief AddressValidator::~AddressValidator Destructor
 */
AddressValidator::~AddressValidator() {
}


/*!
 * \brief AddressValidator::validate
 * \param input
 * \param pos
 * \return
 */
QValidator::State AddressValidator::validate(QString &input, int & pos) const {
    Q_UNUSED(pos);
    QRegExp sanity("^[0-9a-zA-Z./-]*$");
    QRegExp domainnametest("^([a-zA-Z][a-zA-Z0-9-]*\\.)+[a-zA-Z][a-zA-Z0-9-]*$");
    QRegExp iptest("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");
    QRegExp ipmaskedtest("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)/([0-9]+)$");
    QRegExp ipmasked2test("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)/([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");
    long ipbyte;
    bool ok;

    if (input.isNull()) {
        return Intermediate;
    }

    // Smoke text
    if (!sanity.exactMatch(input)) {
        return Invalid;
    }
    
    if (input.length() == 0) {
        return Intermediate;
    }

    // Test against the domainname regexp.
    if (domainnametest.exactMatch(input)) {
        return Acceptable;
    }

    // Ok, now lets try the IP address regexp.
    if (iptest.exactMatch(input)) {
        ipbyte = iptest.cap(1).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = iptest.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = iptest.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = iptest.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        return Acceptable;
    }

    // Ok, now lets try the IP address regexp.
    if (ipmaskedtest.exactMatch(input)) {
        ipbyte = ipmaskedtest.cap(1).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmaskedtest.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmaskedtest.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmaskedtest.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmaskedtest.cap(5).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 32) {
            return Intermediate;
        }
        return Acceptable;
    }
    if (ipmasked2test.exactMatch(input)) {
        ipbyte = ipmasked2test.cap(1).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(5).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(6).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(7).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(8).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        return Acceptable;
    }
    return Intermediate;
}


/*!
 * \brief AddressValidator::fixup
 * \param input
 */
void AddressValidator::fixup(QString &input) const {
    QString clean, tmp, mask;
    int i, slashcount;
    QChar c;
    long ipbyte;
    QRegExp snarfnumber("^([0-9]+)");
    int l, pos;
    bool ok;

    // This is real DWIM (Do What I Mean) code.
    // Somehow it is meant to take what the user entered and work out
    // what they meant and then correct the entered string.
    // It's just a bunch of guesses, hunches and heristics.

    if (input.isNull()) {    // Just in case.
        input = "0.0.0.0";
        return;
    }

    // Filter out any bad characters.
    clean = "";
    slashcount = 0;
    for (i = 0; i < input.length(); i++) {
        c = input.at(i);
        if (c == '/') {
            if (slashcount == 0) {
                clean.append('/');
                slashcount++;
            }
        } else if (( c>= '0' && c<= '9') || c == '.' || c == '-' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z')) {
            clean.append(c);
        }
    }

    clean.replace(QRegExp("^\\.*"), QString(""));  // No dots at the start please.
    clean.replace(QRegExp("\\.*$"), QString(""));  // No dots at the end please.

    // Remove double dots.
    do {
        l = clean.length();
        clean.replace(QRegExp("\\.\\."), QString("."));
    } while (l != clean.length());

    // Do we still have a string?
    if (clean.length() == 0) {
        input = "0.0.0.0";  // This should not match much.
        return;
    }

    // Look at the first character and take a guess as to
    // what kind of value the user attempted to enter.
    if (clean.at(0).isDigit()) {
        // Ok, we expect some kind of IP address maybe with a netmask.
        clean.replace(QRegExp("[A-Za-z-]"), QString(""));   // Kill any funny chars.

        clean.replace(QRegExp("^\\.*"), QString(""));  // No dots at the start please.
        clean.replace(QRegExp("\\.*$"), QString(""));  // No dots at the end please.

        // Remove double dots.
        do {
            l = clean.length();
            clean.replace(QRegExp("\\.\\."), QString("."));
        } while (l != clean.length());

        pos = clean.indexOf('/');
        if (pos != -1) {
            mask = clean.right(clean.length() - pos - 1);
            clean = clean.left(pos);
        }

        i = 0;
        tmp = "";
        while (snarfnumber.exactMatch(clean) && i != 4) {
            ipbyte = snarfnumber.cap(1).toLong(&ok);
            if (ipbyte > 255) {
                ipbyte = 255;
            }
            i++;
            tmp.append(QString::number(ipbyte));
            tmp.append(".");
            clean = clean.right(clean.length() - snarfnumber.cap(1).length());
            clean.replace(QRegExp("^[^0-9]*"), QString(""));
        }
        for (;i < 4; i++) {
            tmp.append("0.");
        }
        tmp.replace(QRegExp("\\.$"), QString(""));

        if (mask.length() != 0) { // We still have not consumed all the input.
                                // There must be some kind of netmask left.
            if (mask.contains('.') == 0) {    // It must be a single number netmask.
                tmp.append("/");
                ipbyte = mask.toLong();
                if (ipbyte > 32) {
                    ipbyte = 32;
                }
                tmp.append(QString::number(ipbyte));
            } else {
                // Expecting a dotted quad netmask.
                tmp.append("/");
                i = 0;
                while (snarfnumber.exactMatch(mask) && i != 4) {
                    ipbyte = snarfnumber.cap(1).toLong(&ok);
                    if (ipbyte > 255) {
                        ipbyte = 255;
                    }
                    i++;
                    tmp.append(QString::number(ipbyte));
                    tmp.append(".");
                    mask = mask.right(mask.length() - snarfnumber.cap(1).length());
                    mask.replace(QRegExp("^[^0-9]*"), QString(""));
                }
                for (;i < 4; i++) {
                    tmp.append("0.");
                }
                tmp.replace(QRegExp("\\.$"), QString(""));
            }
        }
        clean = tmp;

    }

    pos = 0;
    if (validate(clean, pos) != Acceptable) {
        input ="0.0.0.0";
    } else {
        input = clean;
    }
    return;
}


///////////////////////////////////////////////////////////////////////////
/// - Class IPValidator
///////////////////////////////////////////////////////////////////////////


/*!
 * \brief IPValidator::IPValidator
 * \param parent
 */
IPValidator::IPValidator(QWidget *parent) : QValidator(parent) {
}


/*!
 * \brief IPValidator::~IPValidator
 */
IPValidator::~IPValidator() {
}


/*!
 * \brief IPValidator::validate
 * \param input
 * \param pos
 * \return
 */
QValidator::State IPValidator::validate(QString &input, int & pos) const {
    Q_UNUSED(pos);
    QRegExp sanity("^[0-9./]*$");
    QRegExp iptest("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");
    QRegExp ipmaskedtest("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)/([0-9]+)$");
    QRegExp ipmasked2test("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)/([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");
    long ipbyte;
    bool ok;

    if (input.isNull()) {
        return Intermediate;
    }

    // Smoke text
    if (!sanity.exactMatch(input)) {
        return Invalid;
    }

    if (input.isEmpty()) {
        return Intermediate;
    }

    // Ok, now lets try the IP address regexp.
    if (iptest.exactMatch(input)) {
        ipbyte = iptest.cap(1).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = iptest.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = iptest.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = iptest.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        return Acceptable;
    }

    // Ok, now lets try the IP address regexp.
    if (ipmaskedtest.exactMatch(input)) {
        ipbyte = ipmaskedtest.cap(1).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmaskedtest.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmaskedtest.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmaskedtest.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmaskedtest.cap(5).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 32) {
            return Intermediate;
        }
        return Acceptable;
    }
    if (ipmasked2test.exactMatch(input)) {
        ipbyte = ipmasked2test.cap(1).toLong(&ok);;
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(5).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(6).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(7).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        ipbyte = ipmasked2test.cap(8).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return Intermediate;
        }
        return Acceptable;
    }
    return Intermediate;
}


/*!
 * \brief IPValidator::fixup
 * \param input
 */
void IPValidator::fixup(QString &input) const {
    QString clean;
    QString tmp;
    QString mask;
    int i;
    int slashcount;
    QChar c;
    long ipbyte;
    QRegExp snarfnumber("^([0-9]+)");
    int l;
    int pos;
    bool ok;

    // This is real DWIM (Do What I Mean) code.
    // Somehow it is meant to take what the user entered and work out
    // what they meant and then correct the entered string.
    // It's just a bunch of guesses, hunches and heristics.

    if (input.isNull()) {    // Just in case.
        input = "0.0.0.0";
        return;
    }

    // Filter out any bad characters.
    clean = "";
    slashcount = 0;
    for (i = 0; i < input.length(); i++) {
        c = input.at(i);
        if (c == '/') {
            if (slashcount == 0) {
                clean.append('/');
                slashcount++;
            }
        } else if ((c >= '0' && c <= '9') || c == '.') {
            clean.append(c);
        }
    }

    clean.replace(QRegExp("^\\.*"), QString(""));  // No dots at the start please.
    clean.replace(QRegExp("\\.*$"), QString(""));  // No dots at the end please.

    // Remove double dots.
    do {
        l = clean.length();
        clean.replace(QRegExp("\\.\\."), QString("."));
    } while (l != clean.length());

    // Do we still have a string?
    if (clean.length() == 0) {
        input = "0.0.0.0";  // This should not match much.
        return;
    }

    // Look at the first character and take a guess as to
    // what kind of value the user attempted to enter.
    if (clean.at(0).isDigit()) {
        // Ok, we expect some kind of IP address maybe with a netmask.
        clean.replace(QRegExp("^\\.*"), QString(""));  // No dots at the start please.
        clean.replace(QRegExp("\\.*$"), QString(""));  // No dots at the end please.

        // Remove double dots.
        do {
            l = clean.length();
            clean.replace(QRegExp("\\.\\."), QString("."));
        } while (l != clean.length());

        pos = clean.indexOf('/');
        if (pos != -1) {
            mask = clean.right(clean.length() - pos - 1);
            clean = clean.left(pos);
        }

        i = 0;
        tmp = "";
        while (snarfnumber.exactMatch(clean) && i != 4) {
            ipbyte = snarfnumber.cap(1).toLong(&ok);
            if (ipbyte > 255) {
                ipbyte = 255;
            }
            i++;
            tmp.append(QString::number(ipbyte));
            tmp.append(".");
            clean = clean.right(clean.length() - snarfnumber.cap(1).length());
            clean.replace(QRegExp("^[^0-9]*"), QString(""));
        }
        for (;i < 4; i++) {
            tmp.append("0.");
        }
        tmp.replace(QRegExp("\\.$"), QString(""));

        if (mask.length() != 0) { // We still have not consumed all the input.
            // There must be some kind of netmask left.
            if (mask.contains('.') == 0) {    // It must be a single number netmask.
                tmp.append("/");
                ipbyte = mask.toLong();
                if (ipbyte > 32) {
                    ipbyte = 32;
                }
                tmp.append(QString::number(ipbyte));
            } else {
                // Expecting a dotted quad netmask.
                tmp.append("/");
                i = 0;
                while (snarfnumber.exactMatch(mask) && i != 4) {
                    ipbyte = snarfnumber.cap(1).toLong(&ok);
                    if (ipbyte > 255) {
                        ipbyte = 255;
                    }
                    i++;
                    tmp.append(QString::number(ipbyte));
                    tmp.append(".");
                    mask = mask.right(mask.length() - snarfnumber.cap(1).length());
                    mask.replace(QRegExp("^[^0-9]*"), QString(""));
                }
                for (;i < 4; i++) {
                    tmp.append("0.");
                }
                tmp.replace(QRegExp("\\.$"), QString(""));
            }
        }
        clean = tmp;

    }

    pos = 0;
    if (validate(clean, pos) != Acceptable) {
        input ="0.0.0.0";
    } else {
        input = clean;
    }
    return;
}


///////////////////////////////////////////////////////////////////////////
/// - Class GuideDogApp
///////////////////////////////////////////////////////////////////////////


/*!
 * \brief GuideDogApp::GuideDogApp Constructor
 * \param parent
 */
GuideDogApp::GuideDogApp(QWidget *parent) : QDialog(parent), ui(new Ui::GuideDogApp) {
    setWindowTitle("Guidedog");
    doc = 0;
    updatinggui = false;
    ui->setupUi(this);
}


/*!
 * \brief GuideDogApp::~GuideDogApp Desctructor
 */
GuideDogApp::~GuideDogApp() {
    delete doc;
    delete ui;
}


/*!
 * \brief GuideDogApp::initialize
 * \return
 */
bool GuideDogApp::initialize() {
    waspreviousconfiguration = false;
    systemconfigmodified = false;

    // If not running as root, only import export features available
    // superusermode = god;
    // if (!superusermode) {
    //     ui->saveApplyButton->setEnabled(false);
    //     ui->applyButton->setEnabled(false);
    // }

    doc = new GuidedogDoc();

    // All the widgets are in the ui class

    // Routing page
    connect(ui->routingcheckbox, SIGNAL(toggled(bool)), this, SLOT(slotRouting(bool)));
    connect(ui->masqueradecheckbox, SIGNAL(toggled(bool)), this, SLOT(slotMasquerade(bool)));
    connect(ui->masqueradeftpcheckbox, SIGNAL(toggled(bool)), this, SLOT(slotMasqueradeFTP(bool)));
    connect(ui->masqueradeirccheckbox, SIGNAL(toggled(bool)), this, SLOT(slotMasqueradeIRC(bool)));
    connect(ui->nomasqlistbox, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotNoMasqueradeListBox(QListWidgetItem *)));
    connect(ui->newnomasqbutton, SIGNAL(clicked()), this, SLOT(slotNewAddressButton()));
    connect(ui->deleteenomasqbutton, SIGNAL(clicked()), this, SLOT(slotDeleteAddressButton()));

    addressvalidator = new AddressValidator(ui->nomasqlineedit);
    ui->nomasqlineedit->setValidator(addressvalidator);

    connect(ui->nomasqlineedit, SIGNAL(textChanged(const QString &)), this, SLOT(slotAddressLineEdit(const QString &)));
    connect(ui->nomasqlineedit, SIGNAL(returnPressed()), this, SLOT(slotAddressLineEditReturn()));

    // Forwarding page
    connect(ui->forwardlistbox, SIGNAL(itemClicked(QListWidgetItem *)), this, SLOT(slotForwardListBox(QListWidgetItem *)));
    connect(ui->newforwardbutton, SIGNAL(clicked()), this, SLOT(slotNewForwardButton()));
    connect(ui->deleteforwardbutton, SIGNAL(clicked()), this, SLOT(slotDeleteForwardButton()));
    //     Original Destination box
    connect(ui->portprotocolcombobox, SIGNAL(activated(int)), this, SLOT(slotPortProtocolComboBox(int)));
    connect(ui->originalportspinbox, SIGNAL(valueChanged(int)), this, SLOT(slotOriginalPortSpinBox(int)));
    //     ( ) This machine
    connect(ui->originalthismachineradio, SIGNAL(toggled(bool)), this, SLOT(slotOriginalMachineRadio()));
    //     ( ) Specify:
    connect(ui->originalspecifyradio, SIGNAL(toggled(bool)), this, SLOT(slotOriginalSpecifyRadio()));
    connect(ui->originalspecifylineedit, SIGNAL(textChanged(const QString &)), this, SLOT(slotOriginalSpecifyLineEdit(const QString &)));
    originalspecifyaddressvalidator = new IPValidator(ui->originalspecifylineedit);
    ui->originalspecifylineedit->setValidator(originalspecifyaddressvalidator);
    //     New Destination
    connect(ui->newportspinbox, SIGNAL(valueChanged(int)), this, SLOT(slotNewPortSpinBox(int)));
    //     ( ) This machine
    connect(ui->newthismachineradio, SIGNAL(toggled(bool)), this, SLOT(slotNewMachineRadio()));
    //     ( ) Specify:
    connect(ui->newspecifyradio, SIGNAL(toggled(bool)), this, SLOT(slotNewSpecifyRadio()));
    connect(ui->newspecifylineedit, SIGNAL(textChanged(const QString &)), this, SLOT(slotNewSpecifyLineEdit(const QString &)));
    newspecifyaddressvalidator = new IPValidator(ui->newspecifylineedit);
    ui->newspecifylineedit->setValidator(newspecifyaddressvalidator);
    //     Comment line edit
    connect(ui->commentlineedit, SIGNAL(textChanged(const QString &)), this, SLOT(slotCommentLineEdit(const QString &)));

    // Advanced Page
    connect(ui->disablecheckbox, SIGNAL(toggled(bool)), this, SLOT(slotDisableGuidedog(bool)));
    //     Import/Export
    connect(ui->descriptionedit, SIGNAL(textChanged()), this, SLOT(slotDescriptionChanged()));
    connect(ui->importbutton, SIGNAL(clicked()), this, SLOT(slotImportButton()));
    connect(ui->exportbutton, SIGNAL(clicked()), this, SLOT(slotExportButton()));

    // Main buttons (Help, About, OK, Apply, Close)
    connect(ui->helpButton, SIGNAL(clicked()), this, SLOT(slotHelp()));
    connect(ui->aboutButton, SIGNAL(clicked()), this, SLOT(slotAbout()));
    connect(ui->applyButton, SIGNAL(clicked()), this, SLOT(slotApply()));
    ui->applyButton->setToolTip("Apply iptables rules");
    connect(ui->saveApplyButton, SIGNAL(clicked()), this, SLOT(slotApplySave()));
    ui->saveApplyButton->setToolTip("Save and apply iptables rules and exit");
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(slotClose()));
    ui->closeButton->setToolTip("Just exit");
	
    readOptions();
	openDefault();
    updatinggui = true;
    syncGUIFromDoc();
    updatinggui = false;

    return true;
}


/*!
 * \brief GuideDogApp::saveOptions
 * Saves the guidedog application options
 */
void GuideDogApp::saveAppOptions() {
    QSettings config("Guidedog", "Guidedog");

    config.setValue("General/Geometry", size());
    config.setValue("General/CommandrunnerGeometry", commandrunnersize);
    config.sync();
}


/*!
 * \brief GuideDogApp::readOptions
 * Reads the guidedog application options
 */
void GuideDogApp::readOptions() {
    QSettings config("Guidedog", "Guidedog");

    QSize size = config.value("General/Geometry").value<QSize>();
    if (!size.isEmpty()) {
        resize(size);
    }

    commandrunnersize = config.value("General/CommandrunnerGeometry").value<QSize>();
    if (commandrunnersize.isEmpty()) {
        commandrunnersize.setWidth(400);
        commandrunnersize.setHeight(300);
    }
}


/*!
 * \brief GuideDogApp::syncGUIFromDoc
 * Synchronizes the GUI based on object states from the document
 */
void GuideDogApp::syncGUIFromDoc() {
    int i;

    ui->routingcheckbox->setChecked(doc->isRouting());
    ui->masqueradecheckbox->setChecked(doc->isMasquerade());
    ui->masqueradeftpcheckbox->setChecked(doc->isMasqueradeFTP());
    ui->masqueradeirccheckbox->setChecked(doc->isMasqueradeIRC());
    ui->descriptionedit->setText(doc->description);
    ui->disablecheckbox->setChecked(doc->isDisabled());

    // Populate the nomasq list box.
    ui->nomasqlistbox->clear();
    for (i = 0; i < doc->nomasqueradelist.count(); i++) {
        ui->nomasqlistbox->addItem(doc->nomasqueradelist.at(i).getAddress());
    }
    // Select the first item.
    if (doc->nomasqueradelist.count() != 0) {
        ui->nomasqlistbox->setCurrentRow(0);
        ui->nomasqlineedit->setText(doc->nomasqueradelist.at(ui->nomasqlistbox->currentRow()).getAddress());
    }

    // Populate the forward rule box.
    ui->forwardlistbox->clear();
    for (i = 0; i < doc->forwardrulelist.count(); i++) {
        ui->forwardlistbox->addItem(doc->forwardrulelist.at(i).getSummary());
    }
    // Select the first item.
    if (doc->forwardrulelist.count() != 0) {
        ui->forwardlistbox->setCurrentRow(0);
        
        if (ui->forwardlistbox->currentRow() != -1) {
            setForwardRule(&(doc->forwardrulelist[ui->forwardlistbox->currentRow()]));
        } else {
            setForwardRule(0);
        }
    }
    enabledGUIStuff();
}


/*!
 * \brief GuideDogApp::show
 * Shown the dialog window, then check root privileges
 */
void GuideDogApp::show() {
    QDialog::show();
    checkRootPrivileges();
}


/*!
 * \brief GuideDogApp::setForwardRule
 * \param rule
 */
void GuideDogApp::setForwardRule(const GuidedogPortForwardRule *rule) {

    if (!rule) {
        ui->originalportspinbox->setValue(0);
        ui->portprotocolcombobox->setCurrentIndex(0);
        ui->originalthismachineradio->setChecked(true);
        ui->originalspecifyradio->setChecked(false);
        ui->originalspecifylineedit->setText("");
        ui->newportspinbox->setValue(0);
        ui->newthismachineradio->setChecked(true);
        ui->newspecifyradio->setChecked(true);
        ui->newspecifylineedit->setText("");
        ui->commentlineedit->setText("");
    } else {
        ui->originalportspinbox->setValue(rule->originalport);
        ui->portprotocolcombobox->setCurrentIndex(rule->iptype == IPPROTO_TCP ? 0 : 1 );
        ui->originalthismachineradio->setChecked(rule->specifyoriginal != true);
        ui->originalspecifyradio->setChecked(rule->specifyoriginal);
        ui->originalspecifylineedit->setText(rule->originaladdress.getAddress());
        ui->newportspinbox->setValue(rule->newport);
        ui->newthismachineradio->setChecked(rule->specifynewaddress != true );
        ui->newspecifyradio->setChecked(rule->specifynewaddress);
        ui->newspecifylineedit->setText(rule->newaddress.getAddress());
        ui->commentlineedit->setText(rule->comment);
    }
}


/*!
 * \brief GuideDogApp::enabledGUIStuff
 */
void GuideDogApp::enabledGUIStuff() {
    bool active, gotaddys, gotrules;
    GuidedogPortForwardRule rule;
    
    active = !doc->isDisabled();
    ui->routingcheckbox->setEnabled(active);
    ui->masqueradecheckbox->setEnabled(doc->isRouting() && active);
    ui->masqueradeftpcheckbox->setEnabled(doc->isRouting() && doc->isMasquerade() && active);
    ui->masqueradeirccheckbox->setEnabled(doc->isRouting() && doc->isMasquerade() && active);
    gotaddys = doc->nomasqueradelist.count() != 0;
    ui->nomasqlistbox->setEnabled(doc->isRouting() && doc->isMasquerade() && active && gotaddys);
    ui->nomasqlineedit->setEnabled(doc->isRouting() && doc->isMasquerade() && active && gotaddys);
    ui->newnomasqbutton->setEnabled(doc->isRouting() && doc->isMasquerade() && active);
    ui->deleteenomasqbutton->setEnabled(doc->isRouting() && doc->isMasquerade() && active && gotaddys);

    gotrules = doc->forwardrulelist.count() != 0;
    ui->forwardlistbox->setEnabled(active && gotrules);
    ui->newforwardbutton->setEnabled(active);
    ui->deleteforwardbutton->setEnabled(active && gotrules);

    ui->originalportspinbox->setEnabled(active && gotrules);
    ui->portprotocolcombobox->setEnabled(active && gotrules);
    ui->originalthismachineradio->setEnabled(active && gotrules);
    ui->originalspecifyradio->setEnabled(active && gotrules);
    ui->originalspecifylineedit->setEnabled(active && gotrules);

    ui->newportspinbox->setEnabled(active && gotrules);
    ui->newthismachineradio->setEnabled(active && gotrules);
    ui->newspecifyradio->setEnabled(active && gotrules);
    ui->newspecifylineedit->setEnabled(active && gotrules);

    ui->commentlineedit->setEnabled(active && gotrules);
    ui->arrow->setEnabled(active && gotrules);

    if (gotrules) {
        qDebug("Current row is now: %d", ui->forwardlistbox->currentRow());
        rule = doc->forwardrulelist[ui->forwardlistbox->currentRow()];
        ui->originalspecifylineedit->setEnabled(active && rule.specifyoriginal);
        ui->newspecifylineedit->setEnabled(active && rule.specifynewaddress);
    }
    
}


/*!
 * \brief GuideDogApp::checkRootPrivileges
 * Checks if guidedog ir running as root
 */
void GuideDogApp::checkRootPrivileges() {
    if (getuid() != 0) {
        isSuperUser = false;
        QMessageBox::information(0, QObject::tr("Information - Guidedog"),
            QObject::tr(
            "Since you do not have superuser privileges, Guidedog will\n"
            "ask for root credentials when needed.\n"));
    } else {
        // Is current user is root, flag it
        isSuperUser = true;
    }
}


/*!
 * \brief GuideDogApp::slotApplySave writes and applies script and exit application
 * Writes the guidedog script to the defined system file and
 * Runs the guidedog script (applies the iptables rules to the running system)
 * Exits.
 */
void GuideDogApp::slotApplySave() {
    saveScript();                   // Copies to system location
    if (applyScript(true)) {        // Applies the script to running system
        saveAppOptions();           // Save application options
        accept();                   // Exits
	}
}


/*!
 * \brief GuideDogApp::slotClose
 * Just exits. If script has been applied but never saved,
 * the user may return to the original settings before guidedog started.
 */
void GuideDogApp::slotClose() {
    QString errorstring;
    QMessageBox::StandardButton reply;

    if (waspreviousconfiguration && systemconfigmodified) {
        // This is where things become complex.
        // Should we try to restore things to how they were before this program started?
        reply = QMessageBox::warning(this, tr("Warning - Guidedog"),
            tr("The system's routing configuration has been modified.\n\n"
            "Shall I restore it to the previous configuration?\n\n"
            "These changes may disrupt current network connections."),
            QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        // "Yes, revert to the previous settings."
        switch(reply) {
            case QMessageBox::Yes:
                openDefault();
        		if (applyScript(false)) {
                    saveAppOptions();
                    accept();
                }
                break;

            // "Just leave the settings alone and piss off!!"
            case QMessageBox::No:
                saveAppOptions();
                accept();
                break;

            // "Forget I ever pressed the Quit button."
            default:
                break;
        }
    } else {
        // Simple Quit.
        saveAppOptions();
        accept();
    }	
}


/*!
 * \brief GuideDogApp::slotApply
 * Runs the guidedog script (applies the iptables rules to the running system)
 */
void GuideDogApp::slotApply() {
    applyScript(true);
}


/*!
 * \brief GuideDogApp::slotAbout
 * Shows the about dialog
 */
void GuideDogApp::slotAbout() {
    AboutDialog *aboutDialog = new AboutDialog(this);
    aboutDialog->show();
}


/*!
 * \brief GuideDogApp::slotHelp
 */
void GuideDogApp::slotHelp() {
    QDesktopServices::openUrl(QUrl("http://www.simonzone.com/software/guidedog/manual/", QUrl::TolerantMode));
}


/*!
 * \brief GuideDogApp::slotRouting
 * \param on
 */
void GuideDogApp::slotRouting(bool on) {
    if (updatinggui) return;
    updatinggui = true;

    doc->setRouting(on);

    enabledGUIStuff();
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotMasquerade
 * \param on
 */
void GuideDogApp::slotMasquerade(bool on) {
    if (updatinggui) return;
    updatinggui = true;

    doc->setMasquerade(on);

    enabledGUIStuff();
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotMasqueradeFTP
 * \param on
 */
void GuideDogApp::slotMasqueradeFTP(bool on) {
    if (updatinggui) return;
    updatinggui = true;
    doc->setMasqueradeFTP(on);
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotMasqueradeIRC
 * \param on
 */
void GuideDogApp::slotMasqueradeIRC(bool on) {
    if (updatinggui) return;
    updatinggui = true;
    doc->setMasqueradeIRC(on);
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotForwardListBox
 * \param item
 */
void GuideDogApp::slotForwardListBox(QListWidgetItem *item) {
    GuidedogPortForwardRule rule;
    
    if (item == 0) {
        return;
    }
    if (updatinggui) return;
    updatinggui = true;

    rule = doc->forwardrulelist.at(ui->forwardlistbox->currentRow());
    setForwardRule(&rule);
    enabledGUIStuff();

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotDisableGuidedog
 * \param on
 */
void GuideDogApp::slotDisableGuidedog(bool on) {

    if (updatinggui) return;
    updatinggui = true;

    doc->setDisabled(on);
    syncGUIFromDoc();

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotImportButton
 */
void GuideDogApp::slotImportButton() {
    QString filename, errorstring;
    GuidedogDoc *tmpdoc;

    if (updatinggui) return;
    updatinggui = true;

    filename = QFileDialog::getOpenFileName(this, tr("Import Configuration"));
    if (filename.isEmpty()) {
        updatinggui = false;
        return;
    }

    tmpdoc = new GuidedogDoc();
    if (tmpdoc->openScript(filename, errorstring) == false) {
        // Stick up a good ol' error message.
        QMessageBox::critical(this,
            tr("Error - Guidedog"),
            QString(tr("Guidedog was unable to read the file at %1 as being a Guidedog script.\n"
            "This probably means that this file in not actually a Guidedog script.\n\n"
            "(Detailed message \"%2\")")).arg(filename).arg(errorstring));
        delete tmpdoc;
        updatinggui = false;
        return;
    }

    // That loaded ok. Re-configure the GUI.

    delete doc;     //Switcherroo
    doc = tmpdoc;
    syncGUIFromDoc();
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotExportButton
 */
void GuideDogApp::slotExportButton() {
    QString filename, errorstring;

    if (updatinggui) return;
    updatinggui = true;

    filename = QFileDialog::getSaveFileName(this, tr("Export Configuration"));
    if (filename.isEmpty()) {
        updatinggui = false;
        return;
    }

    if (!doc->saveScript(filename, errorstring)) {
        QMessageBox::critical(this, tr("Error - Guidedog"),
            QString(tr("An error occurred while writing the script to %1.\n\n"
            "(Detailed message: \"%2\")")).arg(filename).arg(errorstring));
    }
    updatinggui = false;

}


/*!
 * \brief GuideDogApp::slotDescriptionChanged
 */
void GuideDogApp::slotDescriptionChanged() {
    if (updatinggui) return;
    updatinggui = true;

    doc->description = ui->descriptionedit->toPlainText();

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotNoMasqueradeListBox
 * \param item
 */
void GuideDogApp::slotNoMasqueradeListBox(QListWidgetItem *item) {
    QString address;

    if (updatinggui) return;
    updatinggui = true;

    if (item == 0) return;

    address = doc->nomasqueradelist.at(ui->nomasqlistbox->currentRow()).getAddress();
    ui->nomasqlineedit->setText(address);
    
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotNewAddressButton
 */
void GuideDogApp::slotNewAddressButton() {
    if (updatinggui) return;
    updatinggui = true;
    IPRange *new_address = new IPRange("new.address");

    doc->nomasqueradelist.append(*new_address);
    ui->nomasqlistbox->addItem((doc->nomasqueradelist.last()).getAddress());
    ui->nomasqlineedit->setText((doc->nomasqueradelist.last()).getAddress());
    ui->nomasqlistbox->setCurrentRow(ui->nomasqlistbox->count() - 1);
    enabledGUIStuff();
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotDeleteAddressButton
 */
void GuideDogApp::slotDeleteAddressButton() {
    int rowIndex;
        
    if (updatinggui) return;
    updatinggui = true;

    rowIndex = ui->nomasqlistbox->currentRow();
    if (rowIndex >= 0) {
        ui->nomasqlistbox->takeItem(rowIndex);
        doc->nomasqueradelist.removeAt(rowIndex);
        if (doc->nomasqueradelist.count()) {
            if (rowIndex == 0) {
                ui->nomasqlineedit->setText(doc->nomasqueradelist.at(0).getAddress());
                ui->nomasqlistbox->setCurrentItem(ui->nomasqlistbox->item(0));
            } else {
                ui->nomasqlineedit->setText(doc->nomasqueradelist.at(rowIndex - 1).getAddress());
                ui->nomasqlistbox->setCurrentItem(ui->nomasqlistbox->item(rowIndex - 1));
            }
        } else {
            ui->nomasqlineedit->setText("");
            ui->nomasqlistbox->setDisabled(true);
            ui->deleteenomasqbutton->setDisabled(true);
            ui->nomasqlineedit->setDisabled(true);
        }
        if (ui->nomasqlistbox->currentRow() == -1) {
            ui->nomasqlistbox->setCurrentItem(ui->nomasqlistbox->item(0));
        }
    }
    enabledGUIStuff();
        
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotAddressLineEdit
 * \param s
 */
void GuideDogApp::slotAddressLineEdit(const QString &s) {
    int i;

    if (updatinggui) return;
    updatinggui = true;

    i = ui->nomasqlistbox->currentRow();
    doc->nomasqueradelist[i].setAddress(s);
    ui->nomasqlistbox->item(i)->setText(s);

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotAddressLineEditReturn
 */
void GuideDogApp::slotAddressLineEditReturn() {
}


/*!
 * \brief GuideDogApp::slotNewForwardButton
 */
void GuideDogApp::slotNewForwardButton() {
    GuidedogPortForwardRule newrule;

    if (updatinggui) return;
    updatinggui = true;

    //newrule = new GuidedogPortForwardRule();
    doc->forwardrulelist.append(newrule);
    ui->forwardlistbox->addItem((doc->forwardrulelist.last()).getSummary());
    setForwardRule(&newrule);
    ui->forwardlistbox->setCurrentRow(ui->forwardlistbox->count() - 1);
    enabledGUIStuff();

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotDeleteForwardButton
 */
void GuideDogApp::slotDeleteForwardButton() {
    GuidedogPortForwardRule rule;
    int rowIndex;

    if (updatinggui) return;
    updatinggui = true;

    rowIndex = ui->forwardlistbox->currentRow();
    qDebug("Deleting row: %d", rowIndex);
    rule = doc->forwardrulelist.at(rowIndex);
    if (rowIndex >= 0) {
        ui->forwardlistbox->takeItem(rowIndex);
        doc->forwardrulelist.removeAt(rowIndex);
        if (rowIndex == 0) {
            ui->forwardlistbox->setCurrentRow(0);
        } else {
            ui->forwardlistbox->setCurrentRow(rowIndex - 1);
        }
        if (ui->forwardlistbox->currentRow() == -1) {
            setForwardRule(0);
        } else {
            setForwardRule(&doc->forwardrulelist[ui->forwardlistbox->currentRow()]);
        }
        enabledGUIStuff();
    }
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotOriginalMachineRadio
 */
void GuideDogApp::slotOriginalMachineRadio() {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    ui->originalthismachineradio->setChecked(true);
    ui->originalspecifyradio->setChecked(false);

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->specifyoriginal = false;
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }
    enabledGUIStuff();

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotOriginalSpecifyRadio
 */
void GuideDogApp::slotOriginalSpecifyRadio() {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    ui->originalspecifyradio->setChecked(true);
    ui->originalthismachineradio->setChecked(false);

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->specifyoriginal = true;
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }
    enabledGUIStuff();

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotNewMachineRadio
 */
void GuideDogApp::slotNewMachineRadio() {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    ui->newthismachineradio->setChecked(true);
    ui->newspecifyradio->setChecked(false);

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->specifynewaddress = false;
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }
    enabledGUIStuff();

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotNewSpecifyRadio
 */
void GuideDogApp::slotNewSpecifyRadio() {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    ui->newspecifyradio->setChecked(true);
    ui->newthismachineradio->setChecked(false);

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->specifynewaddress = true;
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }
    enabledGUIStuff();

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotOriginalPortSpinBox
 * \param x
 */
void GuideDogApp::slotOriginalPortSpinBox(int x) {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->originalport = x;
        // Update the forward listbox
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }
    
    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotNewPortSpinBox
 * \param x
 */
void GuideDogApp::slotNewPortSpinBox(int x) {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->newport = x;
        // Update the forward listbox
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotPortProtocolComboBox
 * \param x
 */
void GuideDogApp::slotPortProtocolComboBox(int x) {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->iptype = x == 0 ? IPPROTO_TCP : IPPROTO_UDP;
        // Update the forward listbox
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotOriginalSpecifyLineEdit
 * \param s
 */
void GuideDogApp::slotOriginalSpecifyLineEdit(const QString &s) {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->originaladdress.setAddress(s);
        // Update the forward listbox
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotNewSpecifyLineEdit
 * \param s
 */
void GuideDogApp::slotNewSpecifyLineEdit(const QString &s) {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->newaddress.setAddress(s);
        // Update the forward listbox
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::slotCommentLineEdit
 * \param s
 */
void GuideDogApp::slotCommentLineEdit(const QString &s) {
    int i;
    GuidedogPortForwardRule *rule;

    if (updatinggui) return;
    updatinggui = true;

    i = ui->forwardlistbox->currentRow();
    rule = &(doc->forwardrulelist[i]);
    if (rule != 0) {
        rule->comment = s;
        // Update the forward listbox
        ui->forwardlistbox->item(i)->setText(rule->getSummary());
    }

    updatinggui = false;
}


/*!
 * \brief GuideDogApp::applyScript
 * \param warnfirst If a warning dialog box should appear
 * \return
 * Executes the iptables rules as a BASH script
 */
bool GuideDogApp::applyScript(bool warnfirst) {
    QString finalRules;
    CommandRunner cr(this);

    if (!doc->isDisabled()) {
          // Normal apply
        if (warnfirst) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Warning - Guidedog"));
            msgBox.setText(tr("You are about to modify the system's routing configuration.\n"
                "These changes may disrupt current network connections.\n\n"
                "Do you wish to continue?"));
            QAbstractButton *myContinueButton = msgBox.addButton(tr("Continue"), QMessageBox::AcceptRole);
            QAbstractButton *myCancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
            msgBox.setDefaultButton((QPushButton *)myContinueButton);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            if(msgBox.clickedButton() == myCancelButton) {
                return false;
            }
        }
        QTextStream mFwRules(&finalRules);
        if (doc->writeScript(mFwRules) == false) {
            return false;
        }

        // Now we run the tmp script in our super friendly window.
        if (!commandrunnersize.isEmpty()) {
            cr.resize(commandrunnersize);
        }
        cr.setWindowTitle(tr("Modify Routing Configuration"));
        cr.setHeading(tr("Configuring...\n\nOutput:"));
        // Consider this line instead of exporting the variable in the shell
        // Proposed by Felix Geyer <debfx-pkg@fobos.de>  Sat, 08 Jan 2011 15:59:51 +0100
        // putenv("GUIDEDOG_VERBOSE=1");
        cr.pkexecRun(QString("export GUIDEDOG_VERBOSE=1\n" + finalRules + "\n"));
        systemconfigmodified = true;
        commandrunnersize = cr.size();
        return true;
    } else {
          // If we are to actually disable the firewall because the user has set
          // the Disable checkbox in the advanced section, then we use a different
          // warning question and use a different method to reset the network stack.
        if (warnfirst) {
            QMessageBox msgBox;
            msgBox.setWindowTitle(tr("Warning - Guidedog"));
            msgBox.setText(tr("You are about to reset the system's routing configuration.\n"
                              "These changes may also disrupt current network connections.\n\n"
                              "Do you wish to continue?"));
            QAbstractButton *myContinueButton = msgBox.addButton(tr("Continue"), QMessageBox::AcceptRole);
            QAbstractButton *myCancelButton = msgBox.addButton(tr("Cancel"), QMessageBox::RejectRole);
            msgBox.setDefaultButton((QPushButton *)myContinueButton);
            msgBox.setIcon(QMessageBox::Warning);
            msgBox.exec();
            if(msgBox.clickedButton() == myCancelButton) {
                return false;
            }
        }
        if (resetSystemConfiguration()) {
            return false;
        } else {
            systemconfigmodified = true;
            return true;
        }
    }
    return false;
}


/*!
 * \brief GuideDogApp::saveScript
 * \return
 */
bool GuideDogApp::saveScript() {
    QString errorstring;

    if (isSuperUser) {
        QString filename(SYSTEM_RC_GUIDEDOG);
        // Now the script gets written into filesystem
        if (doc->saveScript(filename, errorstring) == false) {
            QMessageBox::critical(this, tr("Error - Guidedog"),
                QString(tr("An error occurred while writing the script to disk.\n\n"
                "(Detailed message: \"%1\")")).arg(errorstring));
            return false;
        }
    } else {
        // Not as root
        // Create a temporary file
        QTemporaryFile tmpfile;

        if (!tmpfile.open()) {
            errorstring = QObject::tr("An error occurred while writing '%1'. The operating system has this to report about the error: %2")
                .arg(tmpfile.fileName()).arg(strerror(tmpfile.error()));
            QMessageBox::critical(this, tr("Error - Guidedog"),
                QString(tr("An error occurred while writing temporary file to disk.\n\n"
                "(Detailed message: \"%1\")")).arg(errorstring));
            return false;
        }
        CommandRunner cr(this);
        QTextStream out(&tmpfile);
        if (doc->writeScript(out) == false) {
            QMessageBox::critical(this, tr("Error - Guidedog"),
                tr("An error occurred while writing the script to disk.\n"));
             return false;
        }
        tmpfile.close();
        tmpfile.setPermissions(tmpfile.fileName(), QFileDevice::ReadOwner | QFileDevice::WriteOwner | QFileDevice::ExeOwner
            | QFileDevice::ReadGroup | QFileDevice::ExeGroup | QFileDevice::ReadOther | QFileDevice::ExeOther);

        // Now we run the tmp script in our super friendly window.
        if (!commandrunnersize.isEmpty()) {
             cr.resize(commandrunnersize);
        }
        cr.setWindowTitle(tr("Save guidedog script"));
        cr.setHeading(tr("Saving %1...\n\nOutput:").arg(SYSTEM_RC_GUIDEDOG));
        // Use command shell with pkexec to move it to system location
        cr.pkexecRun(QString("cp -v " + tmpfile.fileName() + " " + SYSTEM_RC_GUIDEDOG + "\n"));
        commandrunnersize = cr.size();
    }
    return true;
}


/*!
 * \brief GuideDogApp::resetSystemConfiguration
 * \return
 * Resets system iptables as a BASH script
 */
bool GuideDogApp::resetSystemConfiguration() {
    CommandRunner cr(this);

    if (!commandrunnersize.isEmpty()) {
        cr.resize(commandrunnersize);
    }

    cr.setWindowTitle(tr("Resetting system configuration"));
    cr.setHeading(tr("Resetting system configuration...\n\nOutput:"));
    cr.pkexecRun(QString(
        "echo \"Using iptables.\"\n"
        "echo \"Resetting nat table rules.\"\n"
        "PATH=/bin:/sbin:/usr/bin:/usr/sbin\n"
        "/sbin/sysctl -w net.ipv4.ip_forward=0\n"
        "iptables -t nat -F\n"
        "iptables -t nat -X\n"
        "iptables -t nat -P PREROUTING ACCEPT\n"
        "iptables -t nat -P POSTROUTING ACCEPT\n"
        "iptables -t nat -P OUTPUT ACCEPT\n"
        "echo \"Finished.\"\n"));
    commandrunnersize = cr.size();
    return true;
}


/*!
 * \brief GuideDogApp::openDefault
 * Reads the current guidedog script from the defined system file.
 * If the file is not present, warn the user.
 */
void GuideDogApp::openDefault() {
    QString filename(SYSTEM_RC_GUIDEDOG);
    QString errorstring;
    QFileInfo fileinfo(SYSTEM_RC_GUIDEDOG);

    // if (!superusermode) {
    //    return; // Sorry, if you are not root then you get no default firewall.
    // }

    if (!fileinfo.exists()) {
        // There doesn't appear to be a previous Guidedog firewall script.
        // Just warn the user about the ramifications.
        QMessageBox::information(this, tr("Information - Guidedog"),
            QString(tr("Guidedog was unable to find a Guidedog script at %1.\n"
            "This is probably ok, it just means that this is the first time Guidedog has been run on this system.\n"
            "But please be aware that the settings shown may not represent the system's current routing configuration.\n"
            "Your Guidedog settings will take effect once you use the 'Apply' button or exit Guidedog using 'Ok'.").arg(filename)));
    } else {
        if (doc->openScript(filename, errorstring) == false) {
	        doc->factoryDefaults();
            // We were unable to open the guidedog firewall.
            QMessageBox::critical(this, tr("Error - Guidedog"),
                QString(tr("Guidedog was unable to read the file at %1 as being a Guidedog script.\n"
                "Please be aware that the settings shown may not represent the system's current routing configuration.\n\n"
                "(Detailed message \"%2\")")).arg(filename).arg(errorstring));
        } else {
            waspreviousconfiguration = true;
        }
    }
}

/***************************************************************************
                          guidedogdoc.h  -  description
                             -------------------
    begin                : Thu Sep 27 2001
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

#ifndef GUIDEDOGDOC_H
#define GUIDEDOGDOC_H

#include "iprange.h"

#include <QTextStream>
#include <netinet/in.h>
#include <netinet/tcp.h>


/*!
 * \brief The GuidedogPortForwardRule class
 */
class GuidedogPortForwardRule {
public:
    GuidedogPortForwardRule();

    bool specifyoriginal;
    IPRange originaladdress;
    bool specifynewaddress;
    IPRange newaddress;
    uchar iptype;
    uint originalport;
    uint newport;
    QString comment;
    QString getSummary() const;
};
  

/*!
 * \brief The GuidedogDoc class
 */
class GuidedogDoc {
public: 
    GuidedogDoc();
    ~GuidedogDoc();

    void setDisabled(bool on);
    bool isDisabled();
    void setRouting(bool on);
    bool isRouting();
    void setMasquerade(bool on);
    bool isMasquerade();
    void setMasqueradeFTP(bool on);
    bool isMasqueradeFTP();
    void setMasqueradeIRC(bool on);
    bool isMasqueradeIRC();

    bool writeScript(QTextStream &stream);
    bool readScript(QTextStream &stream, QString &errorstring);
    bool openScript(const QString &filename, QString &errorstring);
    bool saveScript(const QString &filename, QString &errorstring);

    QString description;

    void factoryDefaults();

    // FIXME: Should use function interfaces here, and not direct.
    QList<GuidedogPortForwardRule> forwardrulelist;
    QList<IPRange> nomasqueradelist;
    
private:
    bool disabled;
    bool routing;
    bool masquerade;
    bool masqueradeftp;
    bool masqueradeirc;
};

#endif

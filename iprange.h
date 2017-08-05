/***************************************************************************
                          iprange.h  -  description
                             -------------------
    begin                : Thu May 10 08:08:00 EST 2001
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
#ifndef IPRANGE_H
#define IPRANGE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <QString>

/*!
 * \brief The IPRangeType enum
 */
enum IPRangeType {invalid, localmachine, domainname, ip, iprange};


/*!
 * \brief The IPRange class
 */
class IPRange {
public:
    IPRange();
    IPRange(const QString &a);
    ~IPRange();
    void setAddress(const QString &a);
    QString getAddress() const;
    IPRangeType getType();

private:
    QString address;
    bool gottype;
    IPRangeType type;

    IPRangeType guessType();
};

#endif


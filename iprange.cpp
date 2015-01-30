/***************************************************************************
                          iprange.cpp  -  description
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

#include "iprange.h"

#include <QRegExp>

///////////////////////////////////////////////////////////////////////////
IPRange::IPRange() {
    gottype = false;
}

///////////////////////////////////////////////////////////////////////////
IPRange::IPRange(const QString &a) {
    setAddress(a);
}
        
///////////////////////////////////////////////////////////////////////////
IPRange::~IPRange() {
}

///////////////////////////////////////////////////////////////////////////
void IPRange::setAddress(const QString &a) {
    address = a;
    gottype = false;
}

///////////////////////////////////////////////////////////////////////////
QString IPRange::getAddress() const {
    return address;
}

///////////////////////////////////////////////////////////////////////////
IPRangeType IPRange::getType() {
    if (!gottype) {
        type = guessType();
        gottype = true;
    }
    return type;
}

///////////////////////////////////////////////////////////////////////////
IPRangeType IPRange::guessType() {
    QRegExp sanity("^[0-9a-zA-Z./-]*$");
    QRegExp domainnametest("^([a-zA-Z0-9-]+\\.)+[a-zA-Z0-9-]+$");
    QRegExp iptest("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");
    QRegExp ipmaskedtest("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)/([0-9]+)$");
    QRegExp ipmasked2test("^([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)/([0-9]+)\\.([0-9]+)\\.([0-9]+)\\.([0-9]+)$");

    bool ok;
    long ipbyte;
        
        // Smoke text
    if (!sanity.exactMatch(address)) {
        return invalid;
    }

    if (address.length() == 0) {
        return invalid;
    }

        // Test against the domainname regexp.
    if (domainnametest.exactMatch(address)) {
        return domainname;
    }
    
        // Ok, now lets try the IP address regexp.
    if (iptest.exactMatch(address)) {
        ipbyte = iptest.cap(1).toLong(&ok);    // Yep, it returns char *.
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = iptest.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = iptest.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = iptest.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        return ip;
    }

        // Ok, now lets try the IP address regexp.
    if (ipmaskedtest.exactMatch(address)) {
        ipbyte = ipmaskedtest.cap(1).toLong(&ok);    // Yep, it returns char *.
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmaskedtest.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmaskedtest.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmaskedtest.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmaskedtest.cap(5).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 32) {
            return invalid;
        }
        return iprange;
    }
    
    if (ipmasked2test.exactMatch(address)) {
        ipbyte = ipmasked2test.cap(1).toLong(&ok);    // Yep, it returns char *.
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmasked2test.cap(2).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmasked2test.cap(3).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmasked2test.cap(4).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmasked2test.cap(5).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmasked2test.cap(6).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmasked2test.cap(7).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        ipbyte = ipmasked2test.cap(8).toLong(&ok);
        if (ipbyte < 0 || ipbyte > 255) {
            return invalid;
        }
        return iprange;
    }
    return invalid;
}

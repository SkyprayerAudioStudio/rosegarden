// -*- c-basic-offset: 4 -*-
/*
    Rosegarden
    A sequencer and musical notation editor.
    Copyright 2000-2008 the Rosegarden development team.
    See the AUTHORS file for more details.

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of the
    License, or (at your option) any later version.  See the file
    COPYING included with this distribution for more information.
*/

#ifndef _ROSE_STRINGS_H_
#define _ROSE_STRINGS_H_

#include <string>
#include <qstring.h>
#include "PropertyName.h"
#include "Exception.h"

extern QString strtoqstr(const std::string &);
extern QString strtoqstr(const Rosegarden::PropertyName &);
extern std::string qstrtostr(const QString &);
extern double strtodouble(const std::string &);
extern double qstrtodouble(const QString &);

class QTextCodec;
extern std::string convertFromCodec(std::string, QTextCodec *);

#endif

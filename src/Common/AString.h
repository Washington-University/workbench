/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */
/*LICENSE_END*/

#ifndef ASTRING_H
#define ASTRING_H

#include <QString>

//namespace caret {
class AString : public QString
{
public:
    AString() : QString() {}
    AString(const QChar *unicode, int size) : QString(unicode, size) {}
    explicit AString(const QChar *unicode) : QString(unicode) {} // Qt5: merge with the above
    AString(QChar c) : QString(c) {}
    AString(int size, QChar c) : QString(size, c) {}
    inline AString(const QLatin1String &latin1) : QString(latin1) {}
    inline AString(const AString &string) : QString(string) {}
    inline AString(const QString &string) : QString(string) {}

    inline AString(const char *ch) : QString(ch){}
    inline AString(const QByteArray &a) : QString(a) {}
    inline AString(const Null &t) : QString(t) {}
    //inline AString &operator=(const Null &t) { QString::operator=(t); return *this; }
    //AString(int size, Qt::Initialization) :  QString(size,Qt::Initialization) {}

    //using QString::operator=;
    //AString &operator=(QChar c) { QString::operator=(c); return *this;}
    //AString &operator=(const QString &string) { QString::operator=(string); return *this;}
    //inline AString &operator=(const QLatin1String &latin1) { QString::operator=(latin1); return *this;}
    //inline  AString &operator=(const char *ch) { QString::operator=(ch); return *this;}
    //inline  AString &operator=(const QByteArray &a) { QString::operator=(a); return *this;}
    //inline  AString &operator=(char c) { QString::operator=(c); return *this;}

};
   
//}

#endif // ASTRING_H

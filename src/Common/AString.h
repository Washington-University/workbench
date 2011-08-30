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
    AString(const QLatin1String &latin1) : QString(latin1) {}
    AString(const AString &string) : QString(string) {}
    AString(const QString &string) : QString(string) {}

    AString(const char *ch) : QString(ch){}
    AString(const QByteArray &a) : QString(a) {}
    AString(const Null &t) : QString(t) {}
    //AString &operator=(const Null &t) { QString::operator=(t); return *this; }
    //AString(int size, Qt::Initialization) :  QString(size,Qt::Initialization) {}

    //using QString::operator=;
    //AString &operator=(QChar c) { QString::operator=(c); return *this;}
    //AString &operator=(const QString &string) { QString::operator=(string); return *this;}
    //AString &operator=(const QLatin1String &latin1) { QString::operator=(latin1); return *this;}
    //AString &operator=(const char *ch) { QString::operator=(ch); return *this;}
    //AString &operator=(const QByteArray &a) { QString::operator=(a); return *this;}
    //AString &operator=(char c) { QString::operator=(c); return *this;}

};
//std::string compatibility
const std::string &operator= (std::string &lhs,AString &rhs) {lhs = AString::toStdString(rhs);}


//char * compatibility
const char* &operator= (char * &lhs,AString &rhs) {lhs = AString::toAscii(rhs); }

//double compatiblity
const double &operator= (double &lhs,AString &rhs) {lhs = AString::toDouble(rhs); }

//float compatiblity
const float &operator= (float &lhs,AString &rhs) {lhs = AString::toFloat(rhs); }

//int compatiblity
const int &operator= (int &lhs,AString &rhs) {lhs = AString::toInt(rhs); }

//long compatiblity
const long &operator= (long &lhs,AString &rhs) {lhs = AString::toLong(rhs); }

//long long compatiblity
const long long &operator= (long long &lhs,AString &rhs) {lhs = AString::toLongLong(rhs); }

//unsigned int compatiblity
const unsigned int &operator= (unsigned int&lhs,AString &rhs) {lhs = AString::toUInt(rhs); }

//unsigned long compatiblity
const unsigned long &operator= (unsigned long &lhs,AString &rhs) {lhs = AString::toULong(rhs); }

//unsigned long long compatiblity
const unsigned long long &operator= (unsigned long long &lhs,AString &rhs) {lhs = AString::toULongLong(rhs); }





//}

#endif // ASTRING_H

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
/*LICENSE_END*/

#ifndef ASTRING_H
#define ASTRING_H

#include <QString>
#include <vector>
#include <stdint.h>

namespace caret {
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
        AString &operator=(const Null &t) { QString::operator=(t); return *this; }
        //AString(int size, Qt::Initialization) :  QString(size,Qt::Initialization) {}
        
        //using QString::operator=;
        AString &operator=(QChar c) { QString::operator=(c); return *this;}
        AString &operator=(const QString &string) { QString::operator=(string); return *this;}
        AString &operator=(const QLatin1String &latin1) { QString::operator=(latin1); return *this;}
        AString &operator=(const char *ch) { QString::operator=(ch); return *this;}
        AString &operator=(const QByteArray &a) { QString::operator=(a); return *this;}
        AString &operator=(char c) { QString::operator=(c); return *this;}
        
        //std::string compatibility
        operator std::string () {return this->toStdString(); }
        
        //char * compatibility
        operator const char* () {return this->toAscii(); }
        
        //double compatiblity
        operator double () {return this->toDouble(); }
        
        //float compatiblity
        operator float () { return this->toFloat(); }
        
        //int compatiblity
        operator int () { return this->toInt(); }
        
        //long compatiblity
        operator long () { return this->toLong(); }
        
        //long long compatiblity
        operator long long () { return this->toLongLong(); }
        
        //unsigned int compatiblity
        operator unsigned int () { return this->toUInt(); }
        
        //unsigned long compatiblity
        operator unsigned long () { return this->toULong(); }
        
        //unsigned long long compatiblity
        operator unsigned long long () { return this->toULongLong(); }
        
        /// convert to a const char* (the operator() does not work in C++ library I/O functions)
        //const char* c_str() const { return qPrintable(*this); }
        
        char* toCharArray() const;
        
        AString convertURLsToHyperlinks() const;
        
        AString convertToHtmlPage() const;
        
        AString convertToHtmlPageWithFontSize(const int fontSize) const;
        
        AString convertToHtmlPageWithCssFontHeight(const int fontHeight) const;
        
        int32_t indexOfAnyChar(const AString& str,
                               int from = 0) const;
        
        int32_t indexNotOf(const QChar& ch) const;
        
        void appendWithNewLine(const AString& str);
        
        int64_t countMatchingCharactersFromEnd(const AString& rhs) const;
        
        static void toNumbers(const AString& s,
                              std::vector<float>& numbersOut);
        static void toNumbers(const AString& s,
                              std::vector<int32_t>& numbersOut);
        
        bool toBool() const;
                
        //I may move these outside the class since they don't require access to the class's internals
        static AString fromNumbers(const std::vector<uint8_t>& v, const AString& separator);
        static AString fromNumbers(const std::vector<int8_t>& v, const AString& separator);
        static AString fromNumbers(const std::vector<int32_t>& v, const AString& separator);
        static AString fromNumbers(const std::vector<uint32_t>& v, const AString& separator);
        static AString fromNumbers(const std::vector<int64_t>& v, const AString& separator);
        static AString fromNumbers(const std::vector<uint64_t>& v, const AString& separator);
        static AString fromNumbers(const std::vector<float>& v, const AString& separator);
        static AString fromNumbers(const std::vector<double>& v, const AString& separator);
        static AString fromNumbers(const float* array, const int64_t numberOfElements, const AString& separator);
        static AString fromNumbers(const uint8_t* array,const int64_t numberOfElements, const AString& separator);
        static AString fromNumbers(const int8_t* array,const int64_t numberOfElements, const AString& separator);
        static AString fromNumbers(const int32_t* array,const int64_t numberOfElements, const AString& separator);
        static AString fromNumbers(const int64_t* array,const int64_t numberOfElements, const AString& separator);
        static AString fromNumbers(const double* array, const int64_t numberOfElements, const AString& separator);
        static AString fromBool(const bool b);
        
        AString replaceHtmlSpecialCharactersWithEscapeCharacters() const;
        
        AString fixUnicodeHyphens(bool* hyphenReplaced = NULL, bool* hadOtherNonAscii = NULL) const;
    };
}

#include <string>
#include <iostream>
std::ostream& operator << (std::ostream &lhs, const caret::AString &rhs);

#endif // ASTRING_H

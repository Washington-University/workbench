#ifndef __HTMLSTRINGBUILDER_H__
#define __HTMLSTRINGBUILDER_H__

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


#include "CaretObject.h"


#include <stdint.h>

class StringBuilder;

namespace caret {

    class HtmlStringBuilder : public CaretObject {
        
    public:
        HtmlStringBuilder();
        
        virtual ~HtmlStringBuilder();
        
    private:
        HtmlStringBuilder(const HtmlStringBuilder& o);
        
        HtmlStringBuilder& operator=(const HtmlStringBuilder& o);
        
        
    public:
        void append(const AString& text);
        
        void clear();
        
        void add(const AString& text);
        
        void addLine(const AString& text);
        
        void add(const int32_t num);
        
        void add(const float num);
        
        void addBold(const AString& text);
        
        void addBold(const float num);
        
        void addBold(const int32_t num);
        
        void addHyperlink(
                          const AString& urlText,
                          const AString& linkText);
        
        void addParagraph();
        
        void addLineBreak();
        
        void addLineBreaks(const int32_t numLineBreaks);
        
        void addSpace();
        
        void addSpaces(const int32_t numSpaces);
        
        int32_t length();
        
        AString toString() const;
        
        AString toStringWithHtmlBody();
        
        AString toStringWithHtmlBodyForToolTip();
        
    private:
        AString toStringWithHtmlBodyPrivate(const bool toolTipFlag);
        
        AString stringBuilder;
        
    };

} // namespace

#endif // __HTMLSTRINGBUILDER_H__

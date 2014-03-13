#ifndef __IDENTIFICATIONSTRINGBUILDER_H__
#define __IDENTIFICATIONSTRINGBUILDER_H__

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


#include "HtmlStringBuilder.h"


#include <stdint.h>
#include <vector>

namespace caret {

    class IdentificationStringBuilder : public HtmlStringBuilder {
        
    public:
        IdentificationStringBuilder();
        
        virtual ~IdentificationStringBuilder();
        
    private:
        IdentificationStringBuilder(const IdentificationStringBuilder& o);
        
        IdentificationStringBuilder& operator=(const IdentificationStringBuilder& o);
        
    public:
        void addLine(
                     const bool indentFlag,
                     const AString& boldText,
                     const AString& normalText);
        
        void addLine(
                     const bool indentFlag,
                     const AString& normalText);
        
        void addLine(
                     const bool indentFlag,
                     const AString& boldText,
                     const int32_t number,
                     const bool displayOnlyIfNumberIsNonZeroFlag);
        
        void addLine(
                     const bool indentFlag,
                     const AString& boldText,
                     const float number,
                     const bool displayOnlyIfNumberIsNonZeroFlag);
        
        void addLine(const bool indentFlag,
                     const AString& boldText,
                     const float floatArray[],
                     const int floatArrayNumberOfElements,
                     const bool displayOnlyIfNonZeroElementInArrayFlag);
        
        void addLine(
                     const bool indentFlag,
                     const AString& boldText,
                     const std::vector<const CaretObject*>& objectList,
                     const bool displayOnlyIfNonNullElementInArrayFlag);
        
        void addIndent();
        
    };

} // namespace

#endif // __IDENTIFICATIONSTRINGBUILDER_H__

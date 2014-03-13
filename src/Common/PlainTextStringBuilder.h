#ifndef __PLAIN_TEXT_STRING_BUILDER_H__
#define __PLAIN_TEXT_STRING_BUILDER_H__

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



namespace caret {

    class PlainTextStringBuilder : public CaretObject {
        
    public:
        PlainTextStringBuilder();
        
        PlainTextStringBuilder(const int32_t indentationNumberOfSpaces);
        
        virtual ~PlainTextStringBuilder();
        
        void clear();
        
        void pushIndentation();
        
        void popIndentation();
        
        /** Add the text preceded by indentation and followed by a newline */
        void addLine(const AString& text);
        
        /** Get the text */
        AString getText() const;
        
    private:
        PlainTextStringBuilder(const PlainTextStringBuilder&);

        PlainTextStringBuilder& operator=(const PlainTextStringBuilder&);
        
        AString m_text;
        
        int32_t m_indentationNumberOfSpaces;
        
        int32_t m_currentIndentationCount;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PLAIN_TEXT_STRING_BUILDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PLAIN_TEXT_STRING_BUILDER_DECLARE__

} // namespace
#endif  //__PLAIN_TEXT_STRING_BUILDER_H__

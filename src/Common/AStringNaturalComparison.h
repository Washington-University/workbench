#ifndef __A_STRING_NATURAL_COMPARISON_H__
#define __A_STRING_NATURAL_COMPARISON_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "AString.h"



namespace caret {

    class AStringNaturalComparison {
        
    public:
        AStringNaturalComparison();
        
        AStringNaturalComparison(const AStringNaturalComparison&) { }
        
        virtual ~AStringNaturalComparison();
        
        bool operator() (const AString& s1,
                         const AString& s2) const;
        
        static int32_t compare(const AString& string1,
                               const AString& string2);

        // ADD_NEW_METHODS_HERE

    private:
        AStringNaturalComparison& operator=(const AStringNaturalComparison&);
        
        // ADD_NEW_MEMBERS_HERE

        class StringParser {
        public:
            StringParser(const AString& s);
            
            int64_t nextChar(bool& isNumberOut) const;
            
            inline bool hasMore() const {
                return (m_pos < m_len);
            }
            
        private:
            const AString& m_s;
            
            mutable int32_t m_pos;
            
            int32_t m_len;
        };
    };
    
#ifdef __A_STRING_NATURAL_COMPARISON_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __A_STRING_NATURAL_COMPARISON_DECLARE__

} // namespace
#endif  //__A_STRING_NATURAL_COMPARISON_H__

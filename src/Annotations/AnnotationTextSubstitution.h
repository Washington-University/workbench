#ifndef __ANNOTATION_TEXT_SUBSTITUTION_H__
#define __ANNOTATION_TEXT_SUBSTITUTION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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



#include <memory>

#include "CaretObject.h"

namespace caret {

    class AnnotationTextSubstitution : public CaretObject {
        
    public:
        AnnotationTextSubstitution(const AString& groupID,
                                   const AString& columnID,
                                   const int32_t startIndex,
                                   const int32_t length);
        
        virtual ~AnnotationTextSubstitution();
        
        AnnotationTextSubstitution(const AnnotationTextSubstitution& obj);

        AnnotationTextSubstitution& operator=(const AnnotationTextSubstitution& obj);
        
        bool operator==(const AnnotationTextSubstitution& obj) const;

        /** @return The substitution group ID */
        inline const AString& getGroupID() const { return m_groupID; }

        /** @return The substitution column ID */
        inline const AString& getColumnID() const { return m_columnID; }
        
        /** @return The start index of the substitution in the text annotation  */
        inline int32_t getStartIndex() const { return m_startIndex; }
        
        /** @return The length of the substitution in the text annotation */
        inline int32_t getLength() const { return m_length; }

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperAnnotationTextSubstitution(const AnnotationTextSubstitution& obj);

        AString m_groupID;
        
        AString m_columnID;
        
        int32_t m_startIndex;
        
        int32_t m_length;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TEXT_SUBSTITUTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_SUBSTITUTION_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_SUBSTITUTION_H__

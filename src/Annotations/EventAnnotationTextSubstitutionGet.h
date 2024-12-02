#ifndef __EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_H__
#define __EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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


#include <map>
#include <memory>

#include "AnnotationTextSubstitution.h"
#include "Event.h"

namespace caret {

    class EventAnnotationTextSubstitutionGet : public Event {
        
    public:
        EventAnnotationTextSubstitutionGet();
        
        virtual ~EventAnnotationTextSubstitutionGet();
        
        void addSubstitutionID(const AnnotationTextSubstitution& substitutionID);
        
        int32_t getNumberOfSubstitutionIDs() const;
        
        const AnnotationTextSubstitution& getSubstitutionID(const int32_t index) const;
        
        void setSubstitutionTextValue(const int32_t index,
                                      const AString textValue);
        
        AString getSubstitutionTextValue(const int32_t index) const;
        
        EventAnnotationTextSubstitutionGet(const EventAnnotationTextSubstitutionGet&) = delete;

        EventAnnotationTextSubstitutionGet& operator=(const EventAnnotationTextSubstitutionGet&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        std::vector<AnnotationTextSubstitution> m_substitutionIDs;
        
        std::vector<AString> m_substitutionTexts;
        
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET_H__

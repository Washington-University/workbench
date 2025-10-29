#ifndef __EVENT_IDENTIFICATION_HIGHLIGHT_STEREOTAXIC_LOCATIONS_IN_TABS_H__
#define __EVENT_IDENTIFICATION_HIGHLIGHT_STEREOTAXIC_LOCATIONS_IN_TABS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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
#include <vector>

#include "Event.h"
#include "Vector3D.h"


namespace caret {

    class EventIdentificationHighlightStereotaxicLocationsInTabs : public Event {
        
    public:
        enum class Mode {
            LABELS_IN_TABS
        };
        
        EventIdentificationHighlightStereotaxicLocationsInTabs(const AString& labelHierarchText);
        
        virtual ~EventIdentificationHighlightStereotaxicLocationsInTabs();
        
        EventIdentificationHighlightStereotaxicLocationsInTabs(const EventIdentificationHighlightStereotaxicLocationsInTabs&) = delete;

        EventIdentificationHighlightStereotaxicLocationsInTabs& operator=(const EventIdentificationHighlightStereotaxicLocationsInTabs&) = delete;
        
        void addLabelAndStereotaxicXYZ(const AString& labelName,
                                       const Vector3D& xyz);
        
        Mode getMode() const;
        
        const AString& getLabelHierarchyText() const;
        
        int32_t getNumberOfLabels() const;
        
        const AString& getLabelName(const int32_t index) const;

        const Vector3D& getLabelStereotaxicXYZ(const int32_t index) const;
        
        void addIdentificationText(const AString& text);
        
        AString getIdentificationText() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        class LabelAndStereotaxicXyz {
        public:
            LabelAndStereotaxicXyz(const AString& labelName,
                                   const Vector3D& xyz)
            : m_labelName(labelName)
            , m_xyz(xyz) { }
            
            AString m_labelName;
            
            Vector3D m_xyz;
        };

        const Mode m_mode;
        
        const AString m_labelHierarchyText;
        
        std::vector<LabelAndStereotaxicXyz> m_labels;
        
        std::vector<AString> m_identificationTextLines;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_IDENTIFICATION_HIGHLIGHT_STEREOTAXIC_LOCATIONS_IN_TABS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_IDENTIFICATION_HIGHLIGHT_STEREOTAXIC_LOCATIONS_IN_TABS_DECLARE__

} // namespace
#endif  //__EVENT_IDENTIFICATION_HIGHLIGHT_STEREOTAXIC_LOCATIONS_IN_TABS_H__

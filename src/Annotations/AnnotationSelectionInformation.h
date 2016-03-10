#ifndef __ANNOTATION_SELECTION_INFORMATION_H__
#define __ANNOTATION_SELECTION_INFORMATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include "AnnotationGroupingModeEnum.h"
#include "CaretObject.h"



namespace caret {

    class Annotation;
    class AnnotationGroup;
    
    class AnnotationSelectionInformation : public CaretObject {
        
    public:
        AnnotationSelectionInformation(const int32_t windowIndex);
        
        virtual ~AnnotationSelectionInformation();

        void clear();
        
        void update(const std::vector<Annotation*>& selectedAnnotations);
        
        int32_t getWindowIndex() const;

        std::vector<const AnnotationGroup*> getSelectedAnnotationGroups() const;
        
        int32_t getNumberOfSelectedAnnotations() const;
        
        bool isAnyAnnotationSelected() const;
        
        std::vector<Annotation*> getSelectedAnnotations() const;
        
        void getSelectedAnnotations(std::vector<Annotation*>& annotationsOut) const;
        
        bool isGroupingModeValid(const AnnotationGroupingModeEnum::Enum groupingMode) const;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        AnnotationSelectionInformation(const AnnotationSelectionInformation& obj);
        
        AnnotationSelectionInformation& operator=(const AnnotationSelectionInformation& obj);
        
        void copyHelperAnnotationSelectionInformation(const AnnotationSelectionInformation& obj);

        const int32_t m_windowIndex;
        
        std::vector<const AnnotationGroup*> m_annotationGroups;
        
        std::vector<Annotation*> m_annotations;
        
        bool m_groupingValid;
        
        bool m_ungroupValid;
        
        bool m_regroupValid;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_SELECTION_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_SELECTION_INFORMATION_DECLARE__

} // namespace
#endif  //__ANNOTATION_SELECTION_INFORMATION_H__

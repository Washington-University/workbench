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

#include "AnnotationGroupKey.h"
#include "AnnotationGroupingModeEnum.h"
#include "CaretObject.h"



namespace caret {

    class Annotation;
    
    class AnnotationEditingSelectionInformation : public CaretObject {
        
    public:
        AnnotationEditingSelectionInformation(const int32_t windowIndex);
        
        virtual ~AnnotationEditingSelectionInformation();

        void clear();
        
        void update(const std::vector<Annotation*>& selectedAnnotations);
        
        int32_t getWindowIndex() const;

        std::vector<AnnotationGroupKey> getSelectedAnnotationGroupKeys() const;
        
        int32_t getNumberOfSelectedAnnotations() const;
        
        bool isAnyAnnotationSelected() const;
        
        std::vector<Annotation*> getAnnotationsSelectedForEditing() const;
        
        void getAnnotationsSelectedForEditing(std::vector<Annotation*>& annotationsOut) const;
        
        bool isGroupingModeValid(const AnnotationGroupingModeEnum::Enum groupingMode) const;
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        AnnotationEditingSelectionInformation(const AnnotationEditingSelectionInformation& obj);
        
        AnnotationEditingSelectionInformation& operator=(const AnnotationEditingSelectionInformation& obj);
        
        void copyHelperAnnotationSelectionInformation(const AnnotationEditingSelectionInformation& obj);

        const int32_t m_windowIndex;
        
        std::vector<AnnotationGroupKey> m_annotationGroupKeys;
        
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

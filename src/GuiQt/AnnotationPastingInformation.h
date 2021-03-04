#ifndef __ANNOTATION_PASTING_INFORMATION_H__
#define __ANNOTATION_PASTING_INFORMATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include "AnnotationCoordinateSpaceEnum.h"
#include "CaretObject.h"



namespace caret {

    class AnnotationClipboard;
    class AnnotationCoordinateInformation;
    class MouseEvent;
    
    class AnnotationPastingInformation : public CaretObject {
        
    public:
        AnnotationPastingInformation(const MouseEvent& mouseEvent,
                                     const AnnotationClipboard* clipboard);
        
        virtual ~AnnotationPastingInformation();
        
        AnnotationPastingInformation(const AnnotationPastingInformation&) = delete;

        AnnotationPastingInformation& operator=(const AnnotationPastingInformation&) = delete;

        const MouseEvent& getMouseEvent() const;
        
        const std::vector<std::unique_ptr<AnnotationCoordinateInformation>>& getAllCoordinatesInformation() const;;
        
        const std::unique_ptr<AnnotationCoordinateInformation>& getMouseCoordinateInformation() const;

        std::vector<AnnotationCoordinateSpaceEnum::Enum> getPasteableSpaces() const;
        
        AString getInvalidDescription() const;
        
        bool isValid() const;
        
        bool isPasteableInSpace(const AnnotationCoordinateSpaceEnum::Enum space) const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        
        const MouseEvent& m_mouseEvent;
        
        /* Annotation coordinates in all spaces for each annotation coordinate at its pasted location */
        std::vector<std::unique_ptr<AnnotationCoordinateInformation>> m_allCoordinatesInformation;
        
        /* Spaces that are valid for all coordinates */
        std::unique_ptr<AnnotationCoordinateInformation> m_validForPastingInformation;
        
        /* Annotation coordinates at mouse */
        std::unique_ptr<AnnotationCoordinateInformation> m_mouseCoordinateInformation;
        
        std::vector<AnnotationCoordinateSpaceEnum::Enum> m_pasteableSpaces;
        
        AString m_invalidDescription;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_PASTING_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_PASTING_INFORMATION_DECLARE__

} // namespace
#endif  //__ANNOTATION_PASTING_INFORMATION_H__

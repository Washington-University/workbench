#ifndef __ANNOTATION_ARRANGER_INPUTS_H__
#define __ANNOTATION_ARRANGER_INPUTS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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


#include "AnnotationAlignmentEnum.h"
#include "CaretObject.h"

namespace caret {

    class BrainOpenGLTextRenderInterface;
    
    class AnnotationArrangerInputs : public CaretObject {
        
    public:
        AnnotationArrangerInputs(BrainOpenGLTextRenderInterface* textRenderInterface,
                                        const AnnotationAlignmentEnum::Enum m_alignment,
                                        const int32_t windowIndex);
        
        virtual ~AnnotationArrangerInputs();

        BrainOpenGLTextRenderInterface* getTextRender() const;
        
        AnnotationAlignmentEnum::Enum getAlignment() const;
        
        int32_t getWindowIndex() const;
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        AnnotationArrangerInputs(const AnnotationArrangerInputs&);

        AnnotationArrangerInputs& operator=(const AnnotationArrangerInputs&);
        
        BrainOpenGLTextRenderInterface* m_textRenderInterface;
        
        const AnnotationAlignmentEnum::Enum m_alignment;
        
        const int32_t m_windowIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef ____ANNOTATION_ARRANGER_INPUTS__DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // ____ANNOTATION_ARRANGER_INPUTS__DECLARE__

} // namespace
#endif  //__ANNOTATION_ARRANGER_INPUTS_H__

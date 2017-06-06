#ifndef __ANNOTATION_GRAPHICS_LABEL_H__
#define __ANNOTATION_GRAPHICS_LABEL_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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


#include "AnnotationPercentSizeText.h"



namespace caret {

    class AnnotationGraphicsLabel : public AnnotationPercentSizeText {
        
    public:
        AnnotationGraphicsLabel(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType,
                                    const AnnotationTextFontSizeTypeEnum::Enum fontSizeType);
        
        virtual ~AnnotationGraphicsLabel();
        
        AnnotationGraphicsLabel(const AnnotationGraphicsLabel& obj);

        AnnotationGraphicsLabel& operator=(const AnnotationGraphicsLabel& obj);
        
        virtual bool isDeletable() const;
        
        virtual bool isFontColorGuiEditable() const;
        
//        void setAxisViewport(const int32_t axisViewport[4]);
//        
//        void getAxisViewport(int32_t axisViewportOut[4]) const;

        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperAnnotationGraphicsLabel(const AnnotationGraphicsLabel& obj);

        // ADD_NEW_MEMBERS_HERE

//        int32_t m_axisViewport[4] = { 0, 0, 0, 0 };
    };
    
#ifdef __ANNOTATION_GRAPHICS_LABEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_GRAPHICS_LABEL_DECLARE__

} // namespace
#endif  //__ANNOTATION_GRAPHICS_LABEL_H__

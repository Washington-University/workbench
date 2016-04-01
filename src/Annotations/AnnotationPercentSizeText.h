#ifndef __ANNOTATION_PERCENT_SIZE_TEXT_H__
#define __ANNOTATION_PERCENT_SIZE_TEXT_H__

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


#include "AnnotationText.h"



namespace caret {

    class AnnotationPercentSizeText : public AnnotationText {
        
    public:
        AnnotationPercentSizeText(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationPercentSizeText();
        
        AnnotationPercentSizeText(const AnnotationPercentSizeText& obj);

        AnnotationPercentSizeText& operator=(const AnnotationPercentSizeText& obj);
        
        virtual float getFontPercentViewportSize() const;
        
        virtual void setFontPercentViewportSize(const float fontPercentViewportHeight);
        
        static bool isSurfaceSpaceMontageTabSizingEnabled();
        
        // ADD_NEW_METHODS_HERE

    private:
        void copyHelperAnnotationPercentSizeText(const AnnotationPercentSizeText& obj);

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_PERCENT_SIZE_TEXT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_PERCENT_SIZE_TEXT_DECLARE__

} // namespace
#endif  //__ANNOTATION_PERCENT_SIZE_TEXT_H__

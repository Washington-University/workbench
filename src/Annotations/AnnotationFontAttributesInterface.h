#ifndef __ANNOTATION_FONT_STYLE_INTERFACE_H__
#define __ANNOTATION_FONT_STYLE_INTERFACE_H__

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




#include "AnnotationTextFontNameEnum.h"
#include "CaretColorEnum.h"

namespace caret {

    class AnnotationFontAttributesInterface {
        
    public:
        AnnotationFontAttributesInterface() { }
        
        virtual ~AnnotationFontAttributesInterface() { }
                
        virtual AnnotationTextFontNameEnum::Enum getFont() const = 0;
        
        virtual void setFont(const AnnotationTextFontNameEnum::Enum font) = 0;
        
        virtual float getFontPercentViewportSize() const = 0;
        
        virtual void setFontPercentViewportSize(const float fontPercentViewportHeight) = 0;

        virtual CaretColorEnum::Enum getTextColor() const = 0;
        
        virtual void setTextColor(const CaretColorEnum::Enum color) = 0;
        
        virtual void getTextColorRGBA(float rgbaOut[4]) const = 0;
        
        virtual void getTextColorRGBA(uint8_t rgbaOut[4]) const = 0;
        
        virtual void getCustomTextColor(float rgbaOut[4]) const = 0;
        
        virtual void getCustomTextColor(uint8_t rgbaOut[4]) const = 0;
        
        virtual void setCustomTextColor(const float rgba[4]) = 0;
        
        virtual void setCustomTextColor(const uint8_t rgba[4]) = 0;
        
        virtual bool isStylesSupported() const = 0;
        
        virtual bool isBoldStyleEnabled() const = 0;
        
        virtual void setBoldStyleEnabled(const bool enabled) = 0;
        
        virtual bool isItalicStyleEnabled() const = 0;
        
        virtual void setItalicStyleEnabled(const bool enabled) = 0;
        
        virtual bool isUnderlineStyleEnabled() const = 0;
        
        virtual void setUnderlineStyleEnabled(const bool enabled) = 0;
        
        // ADD_NEW_METHODS_HERE

    private:
        AnnotationFontAttributesInterface(const AnnotationFontAttributesInterface&);

        AnnotationFontAttributesInterface& operator=(const AnnotationFontAttributesInterface&);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FONT_STYLE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_FONT_STYLE_INTERFACE_DECLARE__

} // namespace
#endif  //__ANNOTATION_FONT_STYLE_INTERFACE_H__

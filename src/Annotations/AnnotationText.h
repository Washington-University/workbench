#ifndef __ANNOTATION_TEXT_H__
#define __ANNOTATION_TEXT_H__

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


#include "AnnotationFontNameEnum.h"
#include "AnnotationFontSizeEnum.h"
#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTextAlignVerticalEnum.h"
#include "AnnotationTextOrientationEnum.h"
#include "AnnotationTwoDimensionalShape.h"
#include "CaretPointer.h"

namespace caret {

    class AnnotationText : public AnnotationTwoDimensionalShape {
        
    public:
        AnnotationText();
        
        virtual ~AnnotationText();
        
        AnnotationText(const AnnotationText& obj);

        AnnotationText& operator=(const AnnotationText& obj);
        
        AString getFontRenderingEncodedName() const;
        
        AString getText() const;
        
        void setText(const AString& text);

        AnnotationTextAlignHorizontalEnum::Enum getHorizontalAlignment() const;
        
        void setHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment);
        
        AnnotationTextAlignVerticalEnum::Enum getVerticalAlignment() const;
        
        void setVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment);
        
        AnnotationFontNameEnum::Enum getFont() const;
        
        void setFont(const AnnotationFontNameEnum::Enum font);
        
        AnnotationTextOrientationEnum::Enum getOrientation() const;
        
        void setOrientation(const AnnotationTextOrientationEnum::Enum orientation);
        
        AnnotationFontSizeEnum::Enum getFontSize() const;
        
        void setFontSize(const AnnotationFontSizeEnum::Enum fontSize);
        
        bool isBoldEnabled() const;
        
        void setBoldEnabled(const bool enabled);
        
        bool isItalicEnabled() const;
        
        void setItalicEnabled(const bool enabled);
        
        bool isUnderlineEnabled() const;
        
        void setUnderlineEnabled(const bool enabled);
        
        virtual bool isForegroundLineWidthSupported() const;
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationText(const AnnotationText& obj);

        void initializeAnnotationTextMembers();
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        AString m_text;
        
        AnnotationTextAlignHorizontalEnum::Enum  m_alignmentHorizontal;
        
        AnnotationTextAlignVerticalEnum::Enum  m_alignmentVertical;
        
        AnnotationFontNameEnum::Enum m_font;
        
        AnnotationTextOrientationEnum::Enum m_orientation;
        
        AnnotationFontSizeEnum::Enum m_fontSize;
        
        bool m_boldEnabled;
        
        bool m_italicEnabled;
        
        bool m_underlineEnabled;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TEXT_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_H__

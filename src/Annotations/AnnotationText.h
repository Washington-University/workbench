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


#include "AnnotationTextFontPointSizeEnum.h"
#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTextAlignVerticalEnum.h"
#include "AnnotationTextFontNameEnum.h"
#include "AnnotationTextFontSizeTypeEnum.h"
#include "AnnotationTextConnectTypeEnum.h"
#include "AnnotationTextOrientationEnum.h"
#include "AnnotationTwoDimensionalShape.h"
#include "CaretPointer.h"

namespace caret {

    class AnnotationText : public AnnotationTwoDimensionalShape {
        
    public:
        virtual ~AnnotationText();
        
        AnnotationText(const AnnotationText& obj);

        AnnotationText& operator=(const AnnotationText& obj);
        
        AString getFontRenderingEncodedName(const float drawingViewportHeight) const;
        
        AString getText() const;
        
        void setText(const AString& text);

        AnnotationTextAlignHorizontalEnum::Enum getHorizontalAlignment() const;
        
        void setHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment);
        
        AnnotationTextAlignVerticalEnum::Enum getVerticalAlignment() const;
        
        void setVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment);
        
        AnnotationTextFontNameEnum::Enum getFont() const;
        
        void setFont(const AnnotationTextFontNameEnum::Enum font);
        
        AnnotationTextOrientationEnum::Enum getOrientation() const;
        
        void setOrientation(const AnnotationTextOrientationEnum::Enum orientation);
        
        int32_t getFontSizeForDrawing(const int32_t drawingViewportHeight) const;
        
        AnnotationTextFontSizeTypeEnum::Enum getFontSizeType() const;
        
        bool isBoldEnabled() const;
        
        void setBoldEnabled(const bool enabled);
        
        bool isItalicEnabled() const;
        
        void setItalicEnabled(const bool enabled);
        
        bool isUnderlineEnabled() const;
        
        void setUnderlineEnabled(const bool enabled);
        
        AnnotationTextConnectTypeEnum::Enum getConnectToBrainordinate() const;
        
        void setConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum connectToBrainordinate);
        
        bool isConnectToBrainordinateValid() const;
        
        virtual bool isForegroundLineWidthSupported() const;
        
        virtual void applySpatialModification(const AnnotationSpatialModification& spatialModification);
        
        virtual void applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation);


        static void setUserDefaultHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment);
        
        static void setUserDefaultVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment);
        
        static void setUserDefaultFont(const AnnotationTextFontNameEnum::Enum font);
        
        static void setUserDefaultOrientation(const AnnotationTextOrientationEnum::Enum orientation);
        
        static void setUserDefaultFontPointSize(const AnnotationTextFontPointSizeEnum::Enum fontPointSize);
        
        static void setUserDefaultFontPercentViewportSize(const float fontPercentViewportHeight);
        
        static void setUserDefaultBoldEnabled(const bool enabled);
        
        static void setUserDefaultItalicEnabled(const bool enabled);
        
        static void setUserDefaultUnderlineEnabled(const bool enabled);
        
        static void setUserDefaultConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum connectToBrainordinate);
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        AnnotationText(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType,
                       const AnnotationTextFontSizeTypeEnum::Enum fontSizeType);
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

        AnnotationTextFontPointSizeEnum::Enum getFontPointSizeProtected() const;
        
        void setFontPointSizeProtected(const AnnotationTextFontPointSizeEnum::Enum fontPointSize);
        
        float getFontPercentViewportSizeProtected() const;
        
        void setFontPercentViewportSizeProtected(const float fontPercentViewportHeight);
        
    private:
        /* Not implemented */
        AnnotationText();
        
        void copyHelperAnnotationText(const AnnotationText& obj);

        void initializeAnnotationTextMembers();
        
        /* Not saved to scene since it is set by sub-class constructor. */
        const AnnotationTextFontSizeTypeEnum::Enum m_fontSizeType;
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;

        AString m_text;
        
        AnnotationTextAlignHorizontalEnum::Enum  m_alignmentHorizontal;
        
        AnnotationTextAlignVerticalEnum::Enum  m_alignmentVertical;
        
        AnnotationTextFontNameEnum::Enum m_font;
        
        AnnotationTextOrientationEnum::Enum m_orientation;
        
        AnnotationTextFontPointSizeEnum::Enum m_fontPointSize;
        
        AnnotationTextConnectTypeEnum::Enum m_connectToBrainordinate;
        
        float m_fontPercentViewportSize;
        
        bool m_boldEnabled;
        
        bool m_italicEnabled;
        
        bool m_underlineEnabled;
        
        // Defaults
        static AnnotationTextAlignHorizontalEnum::Enum  s_userDefaultAlignmentHorizontal;
        
        static AnnotationTextAlignVerticalEnum::Enum  s_userDefaultAlignmentVertical;
        
        static AnnotationTextFontNameEnum::Enum s_userDefaultFont;
        
        static AnnotationTextOrientationEnum::Enum s_userDefaultOrientation;
        
        static AnnotationTextFontPointSizeEnum::Enum s_userDefaultPointSize;
        
        static AnnotationTextConnectTypeEnum::Enum s_userDefaultConnectToBrainordinate;
        
        static float s_userDefaultFontPercentViewportSize;
        
        static bool s_userDefaultBoldEnabled;
        
        static bool s_userDefaultItalicEnabled;
        
        static bool s_userDefaultUnderlineEnabled;
        
        // ADD_NEW_MEMBERS_HERE

        friend class AnnotationFileXmlReader;
        friend class AnnotationFileXmlWriter;
    };
    
#ifdef __ANNOTATION_TEXT_DECLARE__
    AnnotationTextAlignHorizontalEnum::Enum  AnnotationText::s_userDefaultAlignmentHorizontal = AnnotationTextAlignHorizontalEnum::LEFT;
    
    AnnotationTextAlignVerticalEnum::Enum  AnnotationText::s_userDefaultAlignmentVertical = AnnotationTextAlignVerticalEnum::TOP;
    
    AnnotationTextFontNameEnum::Enum AnnotationText::s_userDefaultFont = AnnotationTextFontNameEnum::VERA;
    
    AnnotationTextOrientationEnum::Enum AnnotationText::s_userDefaultOrientation = AnnotationTextOrientationEnum::HORIZONTAL;
    
    AnnotationTextFontPointSizeEnum::Enum AnnotationText::s_userDefaultPointSize = AnnotationTextFontPointSizeEnum::SIZE14;
    
    AnnotationTextConnectTypeEnum::Enum AnnotationText::s_userDefaultConnectToBrainordinate = AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_NONE;
    
    float AnnotationText::s_userDefaultFontPercentViewportSize = 5.0;
    
    bool AnnotationText::s_userDefaultBoldEnabled = false;
    
    bool AnnotationText::s_userDefaultItalicEnabled = false;
    
    bool AnnotationText::s_userDefaultUnderlineEnabled = false;
    
#endif // __ANNOTATION_TEXT_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_H__

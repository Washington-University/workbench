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
        
        virtual void applyMoveOrResizeFromGUI(const AnnotationSizingHandleTypeEnum::Enum handleSelected,
                                              const float viewportWidth,
                                              const float viewportHeight,
                                              const float mouseX,
                                              const float mouseY,
                                              const float mouseDX,
                                              const float mouseDY);

        virtual void applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation);


        static void setDefaultHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment);
        
        static void setDefaultVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment);
        
        static void setDefaultFont(const AnnotationTextFontNameEnum::Enum font);
        
        static void setDefaultOrientation(const AnnotationTextOrientationEnum::Enum orientation);
        
        static void setDefaultFontPointSize(const AnnotationTextFontPointSizeEnum::Enum fontPointSize);
        
        static void setDefaultFontPercentViewportSize(const float fontPercentViewportHeight);
        
        static void setDefaultBoldEnabled(const bool enabled);
        
        static void setDefaultItalicEnabled(const bool enabled);
        
        static void setDefaultUnderlineEnabled(const bool enabled);
        
        static void setDefaultConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum connectToBrainordinate);
        
        // ADD_NEW_METHODS_HERE

          
          
          
          
          
    protected: 
        AnnotationText(const AnnotationTextFontSizeTypeEnum::Enum fontSizeType);
        
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
        static AnnotationTextAlignHorizontalEnum::Enum  s_defaultAlignmentHorizontal;
        
        static AnnotationTextAlignVerticalEnum::Enum  s_defaultAlignmentVertical;
        
        static AnnotationTextFontNameEnum::Enum s_defaultFont;
        
        static AnnotationTextOrientationEnum::Enum s_defaultOrientation;
        
        static AnnotationTextFontPointSizeEnum::Enum s_defaultPointSize;
        
        static AnnotationTextConnectTypeEnum::Enum s_defaultConnectToBrainordinate;
        
        static float s_defaultFontPercentViewportSize;
        
        static bool s_defaultBoldEnabled;
        
        static bool s_defaultItalicEnabled;
        
        static bool s_defaultUnderlineEnabled;
        
        // ADD_NEW_MEMBERS_HERE

        friend class AnnotationFileXmlReader;
        friend class AnnotationFileXmlWriter;
    };
    
#ifdef __ANNOTATION_TEXT_DECLARE__
    AnnotationTextAlignHorizontalEnum::Enum  AnnotationText::s_defaultAlignmentHorizontal = AnnotationTextAlignHorizontalEnum::CENTER;
    
    AnnotationTextAlignVerticalEnum::Enum  AnnotationText::s_defaultAlignmentVertical = AnnotationTextAlignVerticalEnum::MIDDLE;
    
    AnnotationTextFontNameEnum::Enum AnnotationText::s_defaultFont = AnnotationTextFontNameEnum::VERA;
    
    AnnotationTextOrientationEnum::Enum AnnotationText::s_defaultOrientation = AnnotationTextOrientationEnum::HORIZONTAL;
    
    AnnotationTextFontPointSizeEnum::Enum AnnotationText::s_defaultPointSize = AnnotationTextFontPointSizeEnum::SIZE14;
    
    AnnotationTextConnectTypeEnum::Enum AnnotationText::s_defaultConnectToBrainordinate = AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_NONE;
    
    float AnnotationText::s_defaultFontPercentViewportSize = 0.05;
    
    bool AnnotationText::s_defaultBoldEnabled = false;
    
    bool AnnotationText::s_defaultItalicEnabled = false;
    
    bool AnnotationText::s_defaultUnderlineEnabled = false;
    
#endif // __ANNOTATION_TEXT_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_H__

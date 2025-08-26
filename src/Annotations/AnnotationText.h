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

#include <set>

#include "AnnotationFontAttributesInterface.h"
#include "AnnotationTextFontPointSizeEnum.h"
#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTextAlignVerticalEnum.h"
#include "AnnotationTextFontSizeTypeEnum.h"
#include "AnnotationTextConnectTypeEnum.h"
#include "AnnotationTextOrientationEnum.h"
#include "AnnotationOneCoordinateShape.h"
#include "CaretPointer.h"

namespace caret {

    class AnnotationTextSubstitution;
    
    class AnnotationText : public AnnotationOneCoordinateShape, public AnnotationFontAttributesInterface {
        
    public:
        /**
         * @return The "too small" text size.
         *
         * When the text is smaller than this font height, either all of the
         * text string may not be drawn or just particular characters
         */
        static uint32_t getTooSmallTextHeight() { return 8; }
        
        virtual ~AnnotationText();
        
        AnnotationText(const AnnotationText& obj);

        AnnotationText& operator=(const AnnotationText& obj);
        
        virtual const AnnotationText* castToTextAnnotation() const override;
        
        std::set<AString> getTextSubstitutionGroupIDs() const;

        AString getFontRenderingEncodedName(const float drawingViewportWidth,
                                            const float drawingViewportHeight) const;
        
        AString getText() const;
        
        void setText(const AString& text);

        AString getTextWithSubstitutionsApplied() const;
        
        AnnotationTextAlignHorizontalEnum::Enum getHorizontalAlignment() const;
        
        void setHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment);
        
        AnnotationTextAlignVerticalEnum::Enum getVerticalAlignment() const;
        
        void setVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment);
        
        virtual AnnotationTextFontNameEnum::Enum getFont() const;
        
        virtual void setFont(const AnnotationTextFontNameEnum::Enum font);
        
        virtual float getFontPercentViewportSize() const = 0;
        
        virtual void setFontPercentViewportSize(const float fontPercentViewportHeight) = 0;
        
        AnnotationTextOrientationEnum::Enum getOrientation() const;
        
        void setOrientation(const AnnotationTextOrientationEnum::Enum orientation);
        
        int32_t getFontSizeForDrawing(const int32_t drawingViewportWidth,
                                      const int32_t drawingViewportHeight) const;

        bool isFontTooSmallWhenLastDrawn() const override;
        
        void setFontTooSmallWhenLastDrawn(const bool tooSmallFontFlag) const override;
        
        AnnotationTextFontSizeTypeEnum::Enum getFontSizeType() const;
        
        virtual CaretColorEnum::Enum getTextColor() const;
        
        virtual void setTextColor(const CaretColorEnum::Enum color);
        
        virtual void getTextColorRGBA(float rgbaOut[4]) const;
        
        virtual void getTextColorRGBA(uint8_t rgbaOut[4]) const;
        
        virtual void getCustomTextColor(float rgbaOut[4]) const;
        
        virtual void getCustomTextColor(uint8_t rgbaOut[4]) const;
        
        virtual void setCustomTextColor(const float rgba[4]);
        
        virtual void setCustomTextColor(const uint8_t rgba[4]);
        
        virtual CaretColorEnum::Enum getTextBackgroundColor() const;
        
        virtual void setTextBackgroundColor(const CaretColorEnum::Enum color);
        
        virtual void getTextBackgroundColorRGBA(float rgbaOut[4]) const;
        
        virtual void getTextBackgroundColorRGBA(uint8_t rgbaOut[4]) const;
        
        virtual void getCustomTextBackgroundColor(float rgbaOut[4]) const;
        
        virtual void getCustomTextBackgroundColor(uint8_t rgbaOut[4]) const;
        
        virtual void setCustomTextBackgroundColor(const float rgba[4]);
        
        virtual void setCustomTextBackgroundColor(const uint8_t rgba[4]);
        
        virtual bool isBoldStyleEnabled() const;
        
        virtual void setBoldStyleEnabled(const bool enabled);
        
        virtual bool isItalicStyleEnabled() const;
        
        virtual void setItalicStyleEnabled(const bool enabled);
        
        virtual bool isUnderlineStyleEnabled() const;
        
        virtual void setUnderlineStyleEnabled(const bool enabled);
        
        AnnotationTextConnectTypeEnum::Enum getConnectToBrainordinate() const;
        
        void setConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum connectToBrainordinate);
        
        bool isConnectToBrainordinateValid() const;
        
        virtual bool applySpatialModification(const AnnotationSpatialModification& spatialModification);
        
        virtual void applyCoordinatesSizeAndRotationFromOther(const Annotation* otherAnnotation);

        virtual void getItemIconColorsRGBA(float backgroundRgbaOut[4],
                                           float outlineRgbaOut[4],
                                           float textRgbaOut[4]) const;
        
        float getSmallSurfaceTextScaling() const;
        
        void setSmallSurfaceTextScaling(const float smallSurfaceTextScaling);
        
        static void setUserDefaultHorizontalAlignment(const AnnotationTextAlignHorizontalEnum::Enum alignment);
        
        static void setUserDefaultVerticalAlignment(const AnnotationTextAlignVerticalEnum::Enum alignment);
        
        static void setUserDefaultFont(const AnnotationTextFontNameEnum::Enum font);
        
        static void setUserDefaultOrientation(const AnnotationTextOrientationEnum::Enum orientation);
        
        static void setUserDefaultFontPointSize(const AnnotationTextFontPointSizeEnum::Enum fontPointSize);
        
        static void setUserDefaultFontPercentViewportSize(const float fontPercentViewportHeight);
        
        static void setUserDefaultTextColor(const CaretColorEnum::Enum color);
        
        static void setUserDefaultCustomTextColor(const float rgba[4]);
        
        static void setUserDefaultTextBackgroundColor(const CaretColorEnum::Enum color);
        
        static void setUserDefaultCustomTextBackgroundColor(const float rgba[4]);
        
        static void setUserDefaultBoldEnabled(const bool enabled);
        
        static void setUserDefaultItalicEnabled(const bool enabled);
        
        static void setUserDefaultUnderlineEnabled(const bool enabled);
        
        static void setUserDefaultOutlineEnabled(const bool enabled);
        
        static void setUserDefaultConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum connectToBrainordinate);
        
        // ADD_NEW_METHODS_HERE

    protected:
        AnnotationText(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType,
                       const AnnotationTextFontSizeTypeEnum::Enum fontSizeType);
        
        AnnotationText(const AnnotationTypeEnum::Enum type,
                       const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType,
                       const AnnotationTextFontSizeTypeEnum::Enum fontSizeType);
        
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

        AnnotationTextFontPointSizeEnum::Enum getFontPointSizeProtected() const;
        
        void setFontPointSizeProtected(const AnnotationTextFontPointSizeEnum::Enum fontPointSize);
        
        float getFontPercentViewportSizeProtected() const;
        
        void setFontPercentViewportSizeProtected(const float fontPercentViewportHeight);
        
        virtual void invalidateTextSubstitution() override;

    private:
        /* Not implemented */
        AnnotationText();
        
        void copyHelperAnnotationText(const AnnotationText& obj);

        void initializeAnnotationTextMembers();
        
        std::vector<std::unique_ptr<AnnotationTextSubstitution>> findSubstitutions() const;
        
        /* Not saved to scene since it is set by sub-class constructor. */
        const AnnotationTextFontSizeTypeEnum::Enum m_fontSizeType;
        
        CaretPointer<SceneClassAssistant> m_attributesAssistant;

        AString m_text;
        
        /* Not saved to scenes */
        mutable AString m_textWithSubstitutions;
        
        /* Not saved to scenes */
        mutable std::set<AString> m_textSubstitutionGroupIDs;
        
        AnnotationTextAlignHorizontalEnum::Enum  m_alignmentHorizontal;
        
        AnnotationTextAlignVerticalEnum::Enum  m_alignmentVertical;
        
        AnnotationTextFontNameEnum::Enum m_font;
        
        AnnotationTextOrientationEnum::Enum m_orientation;
        
        AnnotationTextFontPointSizeEnum::Enum m_fontPointSize;
        
        AnnotationTextConnectTypeEnum::Enum m_connectToBrainordinate;
        
        float m_fontPercentViewportSize;
        
        CaretColorEnum::Enum m_colorText;
        
        float m_customColorText[4];
        
        CaretColorEnum::Enum m_colorTextBackground;
        
        float m_customColorTextBackground[4];
        
        /** Transient when drawing, not saved to annotation file */
        float m_smallSurfaceTextScaling = 1.0;
        
        bool m_boldEnabled;
        
        bool m_italicEnabled;
        
        bool m_underlineEnabled;
        
        mutable bool m_fontTooSmallWhenLastDrawnFlag = false;
        
        // Defaults
        static AnnotationTextAlignHorizontalEnum::Enum  s_userDefaultAlignmentHorizontal;
        
        static AnnotationTextAlignVerticalEnum::Enum  s_userDefaultAlignmentVertical;
        
        static AnnotationTextFontNameEnum::Enum s_userDefaultFont;
        
        static AnnotationTextOrientationEnum::Enum s_userDefaultOrientation;
        
        static AnnotationTextFontPointSizeEnum::Enum s_userDefaultPointSize;
        
        static AnnotationTextConnectTypeEnum::Enum s_userDefaultConnectToBrainordinate;
        
        static CaretColorEnum::Enum s_userDefaultColorText;
        
        static float s_userDefaultCustomColorTextBackground[4];
        
        static CaretColorEnum::Enum s_userDefaultColorTextBackground;
        
        static float s_userDefaultCustomColorText[4];
        
        static float s_userDefaultFontPercentViewportSize;
        
        static bool s_userDefaultBoldEnabled;
        
        static bool s_userDefaultItalicEnabled;
        
        static bool s_userDefaultUnderlineEnabled;
        
        // ADD_NEW_MEMBERS_HERE

        friend class AnnotationFileXmlReader;
        friend class AnnotationFileXmlWriter;
        friend class AnnotationFontAttributes;
    };
    
#ifdef __ANNOTATION_TEXT_DECLARE__
    AnnotationTextAlignHorizontalEnum::Enum  AnnotationText::s_userDefaultAlignmentHorizontal = AnnotationTextAlignHorizontalEnum::CENTER;
    
    AnnotationTextAlignVerticalEnum::Enum  AnnotationText::s_userDefaultAlignmentVertical = AnnotationTextAlignVerticalEnum::MIDDLE;
    
    AnnotationTextFontNameEnum::Enum AnnotationText::s_userDefaultFont = AnnotationTextFontNameEnum::VERA;
    
    AnnotationTextOrientationEnum::Enum AnnotationText::s_userDefaultOrientation = AnnotationTextOrientationEnum::HORIZONTAL;
    
    AnnotationTextFontPointSizeEnum::Enum AnnotationText::s_userDefaultPointSize = AnnotationTextFontPointSizeEnum::SIZE14;
    
    AnnotationTextConnectTypeEnum::Enum AnnotationText::s_userDefaultConnectToBrainordinate = AnnotationTextConnectTypeEnum::ANNOTATION_TEXT_CONNECT_NONE;
    
    CaretColorEnum::Enum AnnotationText::s_userDefaultColorText = CaretColorEnum::WHITE;
    
    float AnnotationText::s_userDefaultCustomColorText[4] = { 1.0, 1.0, 1.0, 1.0 };
    
    CaretColorEnum::Enum AnnotationText::s_userDefaultColorTextBackground = CaretColorEnum::NONE;
    
    float AnnotationText::s_userDefaultCustomColorTextBackground[4] = { 0.0, 0.0, 0.0, 1.0 };
    
    float AnnotationText::s_userDefaultFontPercentViewportSize = 5.0;
    
    bool AnnotationText::s_userDefaultBoldEnabled = false;
    
    bool AnnotationText::s_userDefaultItalicEnabled = false;
    
    bool AnnotationText::s_userDefaultUnderlineEnabled = false;
    
#endif // __ANNOTATION_TEXT_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_H__

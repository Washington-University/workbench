#ifndef __ANNOTATION_COLOR_BAR_H__
#define __ANNOTATION_COLOR_BAR_H__

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

#include "AnnotationColorBarPositionModeEnum.h"
#include "AnnotationFontAttributesInterface.h"
#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTwoDimensionalShape.h"

namespace caret {

    class AnnotationColorBarSection;
    class AnnotationColorBarNumericText;
    
    class AnnotationColorBar : public AnnotationTwoDimensionalShape, public AnnotationFontAttributesInterface {
        
    public:
        AnnotationColorBar(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationColorBar();
        
        AnnotationColorBar(const AnnotationColorBar& obj);

        AnnotationColorBar& operator=(const AnnotationColorBar& obj);
        
        void reset();
        
        void resetSizeAttributes();
        
        virtual AnnotationTextFontNameEnum::Enum getFont() const;
        
        virtual void setFont(const AnnotationTextFontNameEnum::Enum font);
        
        virtual float getFontPercentViewportSize() const;
        
        virtual void setFontPercentViewportSize(const float fontPercentViewportHeight);
        
        AnnotationColorBarPositionModeEnum::Enum getPositionMode() const;
        
        void setPositionMode(const AnnotationColorBarPositionModeEnum::Enum positionMode);
        
        virtual CaretColorEnum::Enum getTextColor() const;
        
        virtual void setTextColor(const CaretColorEnum::Enum color);
        
        virtual void getTextColorRGBA(float rgbaOut[4]) const;
        
        virtual void getTextColorRGBA(uint8_t rgbaOut[4]) const;
        
        virtual void getCustomTextColor(float rgbaOut[4]) const;
        
        virtual void getCustomTextColor(uint8_t rgbaOut[4]) const;
        
        virtual void setCustomTextColor(const float rgba[4]);
        
        virtual void setCustomTextColor(const uint8_t rgba[4]);
        
        virtual bool isFontColorGuiEditable() const;
        
        virtual bool isStylesSupported() const;
        
        virtual bool isBoldStyleEnabled() const;
        
        virtual void setBoldStyleEnabled(const bool enabled);
        
        virtual bool isItalicStyleEnabled() const;
        
        virtual void setItalicStyleEnabled(const bool enabled);
        
        virtual bool isUnderlineStyleEnabled() const;
        
        virtual void setUnderlineStyleEnabled(const bool enabled);
        
        virtual bool isOutlineStyleEnabled() const;
        
        virtual void setOutlineStyleEnabled(const bool enabled);
        
        virtual bool isLineWidthSupported() const;
        
        bool isDisplayed() const;
        
        void setDisplayed(const bool displayed);

        void addSection(const float startScalar,
                        const float endScalar,
                        const float startRGBA[4],
                        const float endRGBA[4]);
        
        void clearSections();
        
        int32_t getNumberOfSections() const;
        
        const AnnotationColorBarSection* getSection(const int32_t index) const;
        
        void addNumericText(const float scalar,
                            const AString& numericText,
                            const AnnotationTextAlignHorizontalEnum::Enum horizontalAlignment,
                            const bool drawTickMarkAtScalar);
        
        void clearNumericText();
        
        int32_t getNumberOfNumericText() const;
        
        const AnnotationColorBarNumericText* getNumericText(const int32_t index) const;
        
        void getScalarMinimumAndMaximumValues(float& minimumScalarOut,
                                              float& maximumScalarOut) const;

        bool isShowTickMarksSelected() const;
        
        void setShowTickMarksSelected(const bool selected);
        
        virtual bool isDeletable() const;
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);
        
    private:
        void copyHelperAnnotationColorBar(const AnnotationColorBar& obj);

        CaretPointer<SceneClassAssistant> m_sceneAssistant;
        
        AnnotationTextFontNameEnum::Enum m_fontName;
        
        float m_fontPercentViewportHeight;
        
        AnnotationColorBarPositionModeEnum::Enum m_positionMode;
        
        CaretColorEnum::Enum m_colorText;
        
        float m_customColorText[4];
        
        bool m_displayedFlag;
        
        /** color bar sections NOT SAVED TO SCENE */
        std::vector<const AnnotationColorBarSection*> m_sections;
        
        /** color bar numeric text NOT SAVED TO SCENE */
        std::vector<const AnnotationColorBarNumericText*> m_numericText;
        
        bool m_showTickMarksSelected;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_COLOR_BAR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COLOR_BAR_DECLARE__

} // namespace
#endif  //__ANNOTATION_COLOR_BAR_H__

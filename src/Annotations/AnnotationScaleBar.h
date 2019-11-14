#ifndef __ANNOTATION_SCALE_BAR_H__
#define __ANNOTATION_SCALE_BAR_H__

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

#include <array>
#include <memory>

#include "AnnotationColorBarPositionModeEnum.h"
#include "AnnotationScaleBarUnitsTypeEnum.h"
#include "AnnotationFontAttributesInterface.h"
#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTwoDimensionalShape.h"

namespace caret {

    class AnnotationPercentSizeText;
    
    class AnnotationScaleBar : public AnnotationTwoDimensionalShape, public AnnotationFontAttributesInterface {
        
    public:
        /**
         * Contains information for drawing scale bar, its ticks, and text
         */
        class DrawingInfo {
        public:
            /**
             * Constructor
             */
            DrawingInfo() {
                reset();
            }
            
            /**
             * Reset to invalid
             */
            void reset() {
                m_backgroundBounds.fill(0.0);
                m_barBounds.fill(0.0);
                m_textStartXYZ.fill(0.0);
                m_validFlag = false;
            }
            
            /** Returns true if drawing info is valid */
            bool isValid() const { return m_validFlag; }
            
            /** Set the validity status @param status new status */
            void setValid(const bool status) { m_validFlag = status; }
            
            /** bounds are bottomLeft, bottomRight, topRight, topLeft */
            std::array<float, 12> m_backgroundBounds;
            
            /** bounds are bottomLeft, bottomRight, topRight, topLeft */
            std::array<float, 12> m_barBounds;

            /** starting XYZ for text containing length characters */
            std::array<float, 3> m_textStartXYZ;
            
            /** bounds are bottomLeft, bottomRight, topRight, topLeft  for each tick mark */
            std::vector<std::array<float, 12>> m_ticksBounds;
            
        private:
            bool m_validFlag = false;
        };
        
        AnnotationScaleBar(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationScaleBar();
        
        AnnotationScaleBar(const AnnotationScaleBar& obj);

        AnnotationScaleBar& operator=(const AnnotationScaleBar& obj);
        
        /**
         * @return this annotation cast to AnnotationScaleBar (NULL if not a scale bar)
         * Intended for overriding by the annotation type
         */
        virtual AnnotationScaleBar* castToScaleBar() override { return this; }
        
        /**
         * @return this annotation cast to AnnotationScaleBar (NULL if not a scale bar) const method
         * Intended for overriding by the annotation type
         */
        virtual const AnnotationScaleBar* castToScaleBar() const override { return this; }
        
        void reset();
        
        void resetSizeAttributes();

        float getLength() const;

        void setLength(const float length);
        
        bool isShowLengthText() const;
        
        void setShowLengthText(const bool status);
        
        bool isShowLengthUnitsText() const;
        
        void setShowLengthUnitsText(const bool status);
        
        bool isShowTickMarks() const;
        
        void setShowTickMarks(const bool status);
        
        int32_t getTickMarksSubdivsions() const;
        
        void setTickMarksSubdivisions(const int32_t subdivisions);
        
        float getTickMarksHeight() const;
        
        AnnotationScaleBarUnitsTypeEnum::Enum getLengthUnits() const;
        
        void setLengthUnits(const AnnotationScaleBarUnitsTypeEnum::Enum lengthUnits);
        
        void setDrawingOrthographicWidth(const float drawingOrthographicWidth);
        
        float getDrawingOrthographicWidth() const;
        
        void setDrawingViewportWidth(const float drawingViewportWidth);
        
        float getDrawingViewportWidth() const;
        
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
        
        virtual bool isBoldStyleEnabled() const;
        
        virtual void setBoldStyleEnabled(const bool enabled);
        
        virtual bool isItalicStyleEnabled() const;
        
        virtual void setItalicStyleEnabled(const bool enabled);
        
        virtual bool isUnderlineStyleEnabled() const;
        
        virtual void setUnderlineStyleEnabled(const bool enabled);
        
        virtual bool isOutlineStyleEnabled() const;
        
        virtual void setOutlineStyleEnabled(const bool enabled);
        
        bool isDisplayed() const;
        
        void setDisplayed(const bool displayed);

        bool isFontTooSmallWhenLastDrawn() const override;
        
        void setFontTooSmallWhenLastDrawn(const bool tooSmallFontFlag) const override;
        
        void getScaleBarDrawingInfo(const float viewportWidth,
                                     const float viewportHeight,
                                     const std::array<float, 3>& viewportXYZ,
                                     DrawingInfo& drawingInfoOut) const;
        
        const AnnotationPercentSizeText* getLengthTextAnnotation() const;
        
                // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);
        
        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);
        
    private:
        void copyHelperAnnotationScaleBar(const AnnotationScaleBar& obj);

        void initializeScaleBarInstance();
        
        CaretPointer<SceneClassAssistant> m_sceneAssistant;
        
        float m_length = 10.0;
        
        bool m_showLengthTextFlag = true;
        
        bool m_showLengthUnitsTextFlag = true;
        
        bool m_showTickMarksFlag = false;
        
        int32_t m_tickMarksSubdivisions = 2;
        
        AnnotationScaleBarUnitsTypeEnum::Enum m_lengthUnits = AnnotationScaleBarUnitsTypeEnum::MILLIMETERS;
        
        float m_drawingOrthographicWidth = 0.0;
        
        float m_drawingViewportWidth = 0.0;

        AnnotationTextFontNameEnum::Enum m_fontName;
        
        float m_fontPercentViewportHeight;
        
        AnnotationColorBarPositionModeEnum::Enum m_positionMode;
        
        CaretColorEnum::Enum m_colorText;
        
        float m_customColorText[4];
        
        bool m_displayedFlag;
        
        mutable bool m_fontTooSmallWhenLastDrawnFlag = false;
        
        mutable std::unique_ptr<AnnotationPercentSizeText> m_lengthTextAnnotation;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_SCALE_BAR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_SCALE_BAR_DECLARE__

} // namespace
#endif  //__ANNOTATION_SCALE_BAR_H__

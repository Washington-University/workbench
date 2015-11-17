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
#include "AnnotationTextAlignHorizontalEnum.h"
#include "AnnotationTextFontNameEnum.h"
#include "AnnotationTwoDimensionalShape.h"

namespace caret {

    class AnnotationColorBarSection;
    class AnnotationColorBarNumericText;
    
    class AnnotationColorBar : public AnnotationTwoDimensionalShape {
        
    public:
        AnnotationColorBar(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationColorBar();
        
        AnnotationColorBar(const AnnotationColorBar& obj);

        AnnotationColorBar& operator=(const AnnotationColorBar& obj);
        
        void reset();
        
        AnnotationTextFontNameEnum::Enum getFont() const;
        
        void setFont(const AnnotationTextFontNameEnum::Enum font);
        
        float getFontPercentViewportSize() const;
        
        void setFontPercentViewportSize(const float fontPercentViewportHeight);
        
        AnnotationColorBarPositionModeEnum::Enum getPositionMode() const;
        
        void setPositionMode(const AnnotationColorBarPositionModeEnum::Enum positionMode);
        
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

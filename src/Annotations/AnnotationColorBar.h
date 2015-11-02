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
#include "AnnotationTextFontNameEnum.h"
#include "AnnotationTwoDimensionalShape.h"

namespace caret {

    class AnnotationColorBar : public AnnotationTwoDimensionalShape {
        
    public:
        AnnotationColorBar(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationColorBar();
        
        AnnotationColorBar(const AnnotationColorBar& obj);

        AnnotationColorBar& operator=(const AnnotationColorBar& obj);
        
        AnnotationTextFontNameEnum::Enum getFont() const;
        
        void setFont(const AnnotationTextFontNameEnum::Enum font);
        
        float getFontPercentViewportSize() const;
        
        void setFontPercentViewportSize(const float fontPercentViewportHeight);
        
        AnnotationColorBarPositionModeEnum::Enum getPositionMode() const;
        
        void setPositionMode(const AnnotationColorBarPositionModeEnum::Enum positionMode);
        
        bool isDisplayed() const;
        
        void setDisplayed(const bool displayed);

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
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_COLOR_BAR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COLOR_BAR_DECLARE__

} // namespace
#endif  //__ANNOTATION_COLOR_BAR_H__

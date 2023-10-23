#ifndef __ANNOTATION_FONT_ATTRIBUTES_H__
#define __ANNOTATION_FONT_ATTRIBUTES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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



#include <memory>

#include "CaretObjectTracksModification.h"
#include "AnnotationAttributesDefaultTypeEnum.h"
#include "AnnotationFontAttributesInterface.h"
#include "SceneableInterface.h"

namespace caret {
    class SceneClassAssistant;

    class AnnotationFontAttributes : public CaretObjectTracksModification, public AnnotationFontAttributesInterface, public SceneableInterface {
        
    public:
        AnnotationFontAttributes(const AnnotationAttributesDefaultTypeEnum::Enum attributeDefaultType);
        
        virtual ~AnnotationFontAttributes();
        
        AnnotationFontAttributes(const AnnotationFontAttributes& obj);

        AnnotationFontAttributes& operator=(const AnnotationFontAttributes& obj);
        
        virtual AnnotationTextFontNameEnum::Enum getFont() const override;
        
        virtual void setFont(const AnnotationTextFontNameEnum::Enum font) override;
        
        virtual float getFontPercentViewportSize() const override;
        
        virtual void setFontPercentViewportSize(const float fontPercentViewportHeight) override;
        
        virtual CaretColorEnum::Enum getTextColor() const override;
        
        virtual void setTextColor(const CaretColorEnum::Enum color) override;
        
        virtual void getTextColorRGBA(float rgbaOut[4]) const override;
        
        virtual void getTextColorRGBA(uint8_t rgbaOut[4]) const override;
        
        virtual void getCustomTextColor(float rgbaOut[4]) const override;
        
        virtual void getCustomTextColor(uint8_t rgbaOut[4]) const override;
        
        virtual void setCustomTextColor(const float rgba[4]) override;
        
        virtual void setCustomTextColor(const uint8_t rgba[4]) override;
        
        virtual bool isBoldStyleEnabled() const override;
        
        virtual void setBoldStyleEnabled(const bool enabled) override;
        
        virtual bool isItalicStyleEnabled() const override;
        
        virtual void setItalicStyleEnabled(const bool enabled) override;
        
        virtual bool isUnderlineStyleEnabled() const override;
        
        virtual void setUnderlineStyleEnabled(const bool enabled) override;
        
        bool isFontTooSmallWhenLastDrawn() const override;
        
        void setFontTooSmallWhenLastDrawn(const bool tooSmallFontFlag) const override;
        
        // ADD_NEW_METHODS_HERE

        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
          
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        void copyHelperAnnotationFontAttributes(const AnnotationFontAttributes& obj);

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        AnnotationTextFontNameEnum::Enum m_font = AnnotationTextFontNameEnum::VERA;
        
        float m_fontPercentViewportSize = 5.0;
        
        CaretColorEnum::Enum m_colorText = CaretColorEnum::WHITE;
        
        float m_customColorText[4] = { 1.0, 1.0, 1.0, 1.0 };
        
        bool m_boldEnabled = false;
        
        bool m_italicEnabled = false;
        
        bool m_underlineEnabled = false;
        
        mutable bool m_fontTooSmallWhenLastDrawnFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FONT_ATTRIBUTES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_FONT_ATTRIBUTES_DECLARE__

} // namespace
#endif  //__ANNOTATION_FONT_ATTRIBUTES_H__

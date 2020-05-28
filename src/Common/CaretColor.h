#ifndef __CARET_COLOR_H__
#define __CARET_COLOR_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#include "CaretColorEnum.h"
#include "CaretObject.h"

namespace caret {
    class CaretColor : public CaretObject {
        
    public:
        CaretColor();
        
        virtual ~CaretColor();
        
        CaretColor(const CaretColor& obj);

        CaretColor& operator=(const CaretColor& obj);
        
        bool operator==(const CaretColor& obj) const;

        bool operator!=(const CaretColor& obj) const;
        
        CaretColorEnum::Enum getCaretColorEnum() const;
        
        void setCaretColorEnum(const CaretColorEnum::Enum color);
        
        std::array<uint8_t, 4> getCustomColorRGBA() const;
        
        std::array<float, 4> getCustomColorFloatRGBA() const;
        
        void setCustomColorRGBA(const std::array<uint8_t, 4>& customRGBA);
        
        std::array<uint8_t, 4> getRGBA() const;
        
        std::array<float, 4> getFloatRGBA() const;
        
        AString encodeInJson() const;
        
        bool decodeFromJson(const AString& xml,
                            AString& errorMessageOut);
        
        AString encodeInSceneXML() const;
        
        bool decodeFromSceneXML(const AString& xml,
                                AString& errorMessageOut);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
                  
    private:
        void copyHelperCaretColor(const CaretColor& obj);

        CaretColorEnum::Enum m_color = CaretColorEnum::BLACK;
        
        std::array<uint8_t, 4> m_customRGBA;
        
        static const QString VERSION_ONE;
        
        static const QString XML_TAG_CARET_COLOR;
        
        static const QString XML_ATTRIBUTE_VERSION;
        
        static const QString XML_ATTRIBUTE_COLOR_NAME;
        
        static const QString XML_ATTRIBUTE_CUSTOM_COLOR_RED;
        
        static const QString XML_ATTRIBUTE_CUSTOM_COLOR_GREEN;
        
        static const QString XML_ATTRIBUTE_CUSTOM_COLOR_BLUE;
        
        static const QString XML_ATTRIBUTE_CUSTOM_COLOR_ALPHA;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_COLOR_DECLARE__
    const QString CaretColor::VERSION_ONE                      = "1";
    const QString CaretColor::XML_TAG_CARET_COLOR              = "CaretColor";
    const QString CaretColor::XML_ATTRIBUTE_VERSION            = "Version";
    const QString CaretColor::XML_ATTRIBUTE_COLOR_NAME         = "Name";
    const QString CaretColor::XML_ATTRIBUTE_CUSTOM_COLOR_RED   = "R";
    const QString CaretColor::XML_ATTRIBUTE_CUSTOM_COLOR_GREEN = "G";
    const QString CaretColor::XML_ATTRIBUTE_CUSTOM_COLOR_BLUE  = "B";
    const QString CaretColor::XML_ATTRIBUTE_CUSTOM_COLOR_ALPHA = "A";

#endif // __CARET_COLOR_DECLARE__

} // namespace
#endif  //__CARET_COLOR_H__

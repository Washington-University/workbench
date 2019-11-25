#ifndef __CARET_COLOR_ENUM_COMBOBOX__H_
#define __CARET_COLOR_ENUM_COMBOBOX__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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
#include "WuQWidget.h"

class QComboBox;

namespace caret {

    class CaretColorEnumComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        /**
         * Custom color mode
         */
        enum class CustomColorModeEnum {
            /**
             * No custom color
             */
            DISABLED,
            /**
             * Custom's color editable by user (pop-up when selected)
             */
            EDITABLE,
            /**
             * Custom's color is fixed and set throught function call
             */
            FIXED
        };
        
        /**
         * None color mode
         */
        enum class NoneColorModeEnum {
            /**
             * No None color
             */
            DISABLED,
            /**
             * Custom's color editable by user (pop-up when selected)
             */
            ENABLED
        };
        
        CaretColorEnumComboBox(QObject* parent);
        
        CaretColorEnumComboBox(const CustomColorModeEnum customColorMode,
                               const NoneColorModeEnum noneColorMode,
                               QObject* parent);
                
        virtual ~CaretColorEnumComboBox();
        
        CaretColorEnum::Enum getSelectedColor();
        
        void setSelectedColor(const CaretColorEnum::Enum color);
        
        void getCustomColor(std::array<uint8_t, 3>& rgbOut) const;
        
        void getCustomColor(std::array<uint8_t, 4>& rgbaOut) const;
        
        void setCustomColor(const std::array<uint8_t, 3>& rgb);
        
        void setCustomColor(const std::array<uint8_t, 4>& rgba);
        
        void setCustomColorName(const AString& customColorName);
        
        void setCustomColorIcon(const QIcon& customColorIcon);
        
        QWidget* getWidget();
        
        QComboBox* getComboBox();
        
    signals:
        void colorSelected(const CaretColorEnum::Enum);
        
    private slots:
        void colorComboBoxIndexChanged(int);
        
    private:
        CaretColorEnumComboBox(const CaretColorEnumComboBox&);

        CaretColorEnumComboBox& operator=(const CaretColorEnumComboBox&);
        
    private:
        void setIconColor(const int32_t index,
                          const uint8_t rgba[4]);
        
        const CustomColorModeEnum m_customColorMode;
        
        const NoneColorModeEnum m_noneColorMode;
        
        QComboBox* colorComboBox;

        int32_t m_customColorIndex = -1;
        
        int32_t m_noneColorIndex = -1;
        
        std::array<uint8_t, 3> m_customColorRGB = { 255, 255, 255 };
    };
    
#ifdef __CARET_COLOR_ENUM_COMBOBOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_COLOR_ENUM_COMBOBOX_DECLARE__

} // namespace
#endif  //__CARET_COLOR_ENUM_COMBOBOX__H_

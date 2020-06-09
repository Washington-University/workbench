#ifndef __CARET_COLOR_TOOLBUTTON__H_
#define __CARET_COLOR_TOOLBUTTON__H_

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

#include <QToolButton>

#include "CaretColor.h"
#include "WuQWidget.h"

namespace caret {

    class CaretColor;
    class CaretColorEnumMenu;
    
    class CaretColorToolButton : public QToolButton {
        
        Q_OBJECT

    public:
        /**
         * Custom color mode
         */
        enum class CustomColorMode {
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
        enum class NoneColorMode {
            /**
             * No None color
             */
            DISABLED,
            /**
             * Custom's color editable by user (pop-up when selected)
             */
            ENABLED
        };
        
        CaretColorToolButton(QWidget* parent = 0);
        
        CaretColorToolButton(const CustomColorMode customColorMode,
                             const NoneColorMode noneColorMode,
                             QWidget* parent = 0);

        virtual ~CaretColorToolButton();
        
        CaretColor getSelectedColor();
        
        void setSelectedColor(const CaretColor& color);
        
    signals:
        void colorSelected(const CaretColor& caretColor);
        
    private slots:
        void caretColorMenuSelected(const CaretColorEnum::Enum colorEnum);
        
        void toolButtonClicked();
        
    private:
        CaretColorToolButton(const CaretColorToolButton&);

        CaretColorToolButton& operator=(const CaretColorToolButton&);
        
    private:
        void updateIconColor();
        
        const CustomColorMode m_customColorMode;
        
        const NoneColorMode m_noneColorMode;
        
        CaretColorEnumMenu* m_caretColorEnumMenu = NULL;
        
        CaretColor m_caretColor;
    };
    
#ifdef __CARET_COLOR_TOOLBUTTON_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_COLOR_TOOLBUTTON_DECLARE__

} // namespace
#endif  //__CARET_COLOR_TOOLBUTTON__H_

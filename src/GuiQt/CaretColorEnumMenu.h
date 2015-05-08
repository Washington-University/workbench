#ifndef __CARET_COLOR_ENUM_MENU_H__
#define __CARET_COLOR_ENUM_MENU_H__

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


#include <QMenu>

#include "CaretColorEnum.h"


namespace caret {

    class CaretColorEnumMenu : public QMenu {
        
        Q_OBJECT

    public:
        CaretColorEnumMenu();
        
        CaretColorEnumMenu(const int64_t caretColorOptions);
        
        virtual ~CaretColorEnumMenu();
        
        CaretColorEnum::Enum getSelectedColor();
        
        void setSelectedColor(const CaretColorEnum::Enum color);
        
        void setCustomIconColor(const float rgba[4]);

        // ADD_NEW_METHODS_HERE

    signals:
        void colorSelected(const CaretColorEnum::Enum);
        
    private slots:
        void colorActionSelected(QAction* action);
        
    private:
        CaretColorEnumMenu(const CaretColorEnumMenu&);

        CaretColorEnumMenu& operator=(const CaretColorEnumMenu&);
        
        void initializeCaretColorEnumMenu(const int64_t caretColorOptions);
        
        QAction* m_customColorAction;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_COLOR_ENUM_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARET_COLOR_ENUM_MENU_DECLARE__

} // namespace
#endif  //__CARET_COLOR_ENUM_MENU_H__

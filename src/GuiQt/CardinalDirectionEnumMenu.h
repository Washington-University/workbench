#ifndef __CARDINAL_DIRECTION_ENUM_MENU_H__
#define __CARDINAL_DIRECTION_ENUM_MENU_H__

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

#include "CardinalDirectionEnum.h"

namespace caret {

    class CardinalDirectionEnumMenu : public QMenu {
        
        Q_OBJECT

    public:
        CardinalDirectionEnumMenu();
        
        CardinalDirectionEnumMenu(const std::set<CardinalDirectionEnum::Options>& options);
        
        virtual ~CardinalDirectionEnumMenu();
        
        CardinalDirectionEnum::Enum getSelectedCardinalDirection();
        
        void setSelectedCardinalDirection(const CardinalDirectionEnum::Enum cardinalDirection);

        // ADD_NEW_METHODS_HERE

    signals:
        void cardinalDirectionSelected(const CardinalDirectionEnum::Enum);
        
    private slots:
        void actionSelected(QAction* action);
        
    private:
        CardinalDirectionEnumMenu(const CardinalDirectionEnumMenu&);

        CardinalDirectionEnumMenu& operator=(const CardinalDirectionEnumMenu&);
        
        void initializeCardinalDirectionEnumMenu(const std::set<CardinalDirectionEnum::Options>& options);
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARDINAL_DIRECTION_ENUM_MENU_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CARDINAL_DIRECTION_ENUM_MENU_DECLARE__

} // namespace
#endif  //__CARDINAL_DIRECTION_ENUM_MENU_H__

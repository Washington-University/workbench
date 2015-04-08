
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

#define __CARET_COLOR_ENUM_MENU_DECLARE__
#include "CaretColorEnumMenu.h"
#undef __CARET_COLOR_ENUM_MENU_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;
    
/**
 * \class caret::CaretColorEnumMenu 
 * \brief Menu for selection of the standard caret colors
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
CaretColorEnumMenu::CaretColorEnumMenu()
: QMenu()
{
    std::vector<CaretColorEnum::Enum> colors;
    CaretColorEnum::getAllEnums(colors);
    
    const int32_t numColors = static_cast<int32_t>(colors.size());
    for (int32_t i = 0; i < numColors; i++) {
        const CaretColorEnum::Enum colorEnum = colors[i];
        const int colorIntegerCode = CaretColorEnum::toIntegerCode(colorEnum);
        const AString name = CaretColorEnum::toGuiName(colorEnum);
        
        /*
         * Create an icon with the color.
         */
        const float* rgb = CaretColorEnum::toRGB(colorEnum);
        QPixmap pm(10, 10);
        pm.fill(QColor::fromRgbF(rgb[0],
                                 rgb[1],
                                 rgb[2]));
        QIcon icon(pm);
        
        /*
         * Add color action to menu and make it checkable
         */
        QAction* action = addAction(name);
        action->setData(colorIntegerCode);
        action->setIconText(name);
        action->setIcon(icon);
        action->setCheckable(true);
        
    }
    
    QObject::connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(colorActionSelected(QAction*)));
    
//    setSelectedColor(CaretColorEnum::BLACK);
//    QObject::connect(this->colorComboBox, SIGNAL(currentIndexChanged(int)),
//                     this, SLOT(colorComboBoxIndexChanged(int)));
}

/**
 * Destructor.
 */
CaretColorEnumMenu::~CaretColorEnumMenu()
{
}

/**
 * Set the selected color.
 *
 * @param color
 *      New selected color.
 */
void
CaretColorEnumMenu::setSelectedColor(const CaretColorEnum::Enum color)
{
    blockSignals(true);
    
    const int colorIntegerCode = CaretColorEnum::toIntegerCode(color);
    
    /*
     * Set checkbox next to selected color.
     */
    QList<QAction*> actionList = actions();
    QListIterator<QAction*> actionIter(actionList);
    while (actionIter.hasNext()) {
        QAction* action = actionIter.next();
        CaretAssert(action);
        const int actionIntegerCode = action->data().toInt();
        if (actionIntegerCode == colorIntegerCode) {
            action->setChecked(true);
        }
        else {
            action->setChecked(false);
        }
    }
    
    blockSignals(false);
}

/**
 * Gets called when a color is selected.
 *
 * @param action
 *     Action that is selected.
 */
void
CaretColorEnumMenu::colorActionSelected(QAction* action)
{
    const int integerCode = action->data().toInt();
    bool valid = false;
    const CaretColorEnum::Enum color = CaretColorEnum::fromIntegerCode(integerCode,
                                                                       &valid);
    if (valid) {
        /*
         * Update checkboxes
         */
        setSelectedColor(color);
        
        /*
         * Emit color changed signal.
         */
        emit colorSelected(color);
    }
    else {
        CaretLogSevere("Invalid CaretColorEnum integer code="
                       + AString::number(integerCode));
    }
}


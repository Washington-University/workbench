
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
#include "WuQtUtilities.h"

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
    initializeCaretColorEnumMenu(CaretColorEnum::OPTION_NO_OPTIONS);
}

/**
 * Constructor for menu that may provide optional caret color enums.
 *
 * @param caretColorOptions
 *     Options for caret color enums.
 */
CaretColorEnumMenu::CaretColorEnumMenu(const int64_t caretColorOptions)
: QMenu()
{
    initializeCaretColorEnumMenu(caretColorOptions);
}

/**
 * Destructor.
 */
CaretColorEnumMenu::~CaretColorEnumMenu()
{
}

/**
 * Initialize instance that may have optional caret color enums.
 *
 * @param caretColorOptions
 *     Options for caret color enums.
 */
void
CaretColorEnumMenu::initializeCaretColorEnumMenu(const int64_t caretColorOptions)
{
    m_customColorAction = NULL;
    
    std::vector<CaretColorEnum::Enum> colors;
    CaretColorEnum::getColorAndOptionalEnums(colors,
                                             caretColorOptions);
    
    const int32_t numColors = static_cast<int32_t>(colors.size());
    for (int32_t i = 0; i < numColors; i++) {
        const CaretColorEnum::Enum colorEnum = colors[i];
        const int colorIntegerCode = CaretColorEnum::toIntegerCode(colorEnum);
        const AString name = CaretColorEnum::toGuiName(colorEnum);
        
        /*
         * Create an icon with the color.
         */
        float rgba[4];
        CaretColorEnum::toRGBFloat(colorEnum, rgba);
        if (colorEnum == CaretColorEnum::NONE) {
            rgba[3] = 0.0;
        }
        else if (colorEnum == CaretColorEnum::CUSTOM) {
            /*
             * No pixmap for CUSTOM
             */
            rgba[3] = 0.0;
        }
        else {
            rgba[3] = 1.0;
        }
        
        /*
         * Add color action to menu and make it checkable
         */
        QAction* action = addAction(name);
        action->setData(colorIntegerCode);
        action->setIconText(name);
        QPixmap pm = WuQtUtilities::createCaretColorEnumPixmap(this, 10, 10, colorEnum, rgba, false);
        action->setIcon(QIcon(pm));
        
        action->setCheckable(true);
        
        if (colorEnum == CaretColorEnum::CUSTOM) {
            m_customColorAction = action;
        }
    }
    
    QObject::connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(colorActionSelected(QAction*)));
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

/**
 * @return The selected color.
 */
CaretColorEnum::Enum
CaretColorEnumMenu::getSelectedColor()
{
    QList<QAction*> actionList = actions();
    QListIterator<QAction*> actionIter(actionList);
    while (actionIter.hasNext()) {
        QAction* action = actionIter.next();
        CaretAssert(action);
        
        if (action->isChecked()) {
            const int actionIntegerCode = action->data().toInt();
            bool valid = false;
            CaretColorEnum::Enum colorSelected = CaretColorEnum::fromIntegerCode(actionIntegerCode, &valid);
            if (valid) {
                return colorSelected;
            }
        }
    }
    
    CaretAssertMessage(0,
                       "Did not find selected color.");
    
    return CaretColorEnum::WHITE;
}


/**
 * Set the color for the custom color's icon.
 *
 * @param rgb
 *     Red/Green/Blue/Alpha color components [0.0, 1.0]
 */
void
CaretColorEnumMenu::setCustomIconColor(const float rgba[4])
{
    if (m_customColorAction != NULL) {
        QPixmap pm = WuQtUtilities::createCaretColorEnumPixmap(this,
                                                               10, 10,
                                                               CaretColorEnum::CUSTOM,
                                                               rgba,
                                                               false);
        m_customColorAction->setIcon(QIcon(pm));
    }
}



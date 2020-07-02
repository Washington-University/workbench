
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

#define __CARDINAL_DIRECTION_ENUM_MENU_DECLARE__
#include "CardinalDirectionEnumMenu.h"
#undef __CARDINAL_DIRECTION_ENUM_MENU_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "WuQtUtilities.h"

using namespace caret;
    
/**
 * \class caret::CardinalDirectionEnumMenu
 * \brief Menu for selection of the standard cardinal directions
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
CardinalDirectionEnumMenu::CardinalDirectionEnumMenu()
: QMenu()
{
    const std::set<CardinalDirectionEnum::Options> options;
    initializeCardinalDirectionEnumMenu(options);
}

/**
 * Constructor for menu that may provide optional cardinal direction enums.
 *
 * @param options
 *     Options for cardinal direction enums.
 */
CardinalDirectionEnumMenu::CardinalDirectionEnumMenu(const std::set<CardinalDirectionEnum::Options>& options)
: QMenu()
{
    initializeCardinalDirectionEnumMenu(options);
}

/**
 * Destructor.
 */
CardinalDirectionEnumMenu::~CardinalDirectionEnumMenu()
{
}

/**
 * Initialize instance that may have optional cardinal direction enums.
 *
 * @param options
 *     Options for cardinal direction enums.
 */
void
CardinalDirectionEnumMenu::initializeCardinalDirectionEnumMenu(const std::set<CardinalDirectionEnum::Options>& options)
{
    std::vector<CardinalDirectionEnum::Enum> cardinalDirections;
    CardinalDirectionEnum::getAllEnums(cardinalDirections,
                                       options);
    
    for (const auto cd : cardinalDirections) {
        const AString name = CardinalDirectionEnum::toGuiName(cd);
        
        /*
         * Add cardinal direction action to menu and make it checkable
         */
        QAction* action = addAction(name);
        action->setCheckable(true);
        action->setData(CardinalDirectionEnum::toIntegerCode(cd));
        QObject::connect(action, &QAction::triggered,
                         [=] { actionSelected(action); });
        
        switch (cd) {
            case CardinalDirectionEnum::AUTO:
                addSeparator();
                break;
            case CardinalDirectionEnum::EAST:
                break;
            case CardinalDirectionEnum::NORTH:
                break;
            case CardinalDirectionEnum::NORTHEAST:
                break;
            case CardinalDirectionEnum::NORTHWEST:
                break;
            case CardinalDirectionEnum::SOUTH:
                break;
            case CardinalDirectionEnum::SOUTHEAST:
                break;
            case CardinalDirectionEnum::SOUTHWEST:
                break;
            case CardinalDirectionEnum::WEST:
                break;
        }
    }
}

/**
 * Set the selected cardinal direction.
 *
 * @param cardinalDirection
 *      New selected cardinal direction.
 */
void
CardinalDirectionEnumMenu::setSelectedCardinalDirection(const CardinalDirectionEnum::Enum cardinalDirection)
{
    blockSignals(true);
    
    const int integerCode = CardinalDirectionEnum::toIntegerCode(cardinalDirection);
    
    /*
     * Set checkbox next to selected cardinal direction.
     */
    QList<QAction*> actionList = actions();
    QListIterator<QAction*> actionIter(actionList);
    while (actionIter.hasNext()) {
        QAction* action = actionIter.next();
        CaretAssert(action);
        const int actionIntegerCode = action->data().toInt();
        if (actionIntegerCode == integerCode) {
            action->setChecked(true);
        }
        else {
            action->setChecked(false);
        }
    }
    
    blockSignals(false);
}

/**
 * Gets called when an action is selected.
 *
 * @param action
 *     Action that is selected.
 */
void
CardinalDirectionEnumMenu::actionSelected(QAction* action)
{
    const int integerCode = action->data().toInt();
    bool valid = false;
    const CardinalDirectionEnum::Enum cardinalDirection = CardinalDirectionEnum::fromIntegerCode(integerCode,
                                                                                     &valid);
    if (valid) {
        /*
         * Update checkboxes
         */
        setSelectedCardinalDirection(cardinalDirection);
        
        /*
         * Emit cardinal direction changed signal.
         */
        emit cardinalDirectionSelected(cardinalDirection);
    }
    else {
        CaretLogSevere("Invalid CardinalDirectionEnum integer code="
                       + AString::number(integerCode));
    }
}

/**
 * @return The selected cardinal direction.
 */
CardinalDirectionEnum::Enum
CardinalDirectionEnumMenu::getSelectedCardinalDirection()
{
    QList<QAction*> actionList = actions();
    QListIterator<QAction*> actionIter(actionList);
    while (actionIter.hasNext()) {
        QAction* action = actionIter.next();
        CaretAssert(action);
        
        if (action->isChecked()) {
            const int actionIntegerCode = action->data().toInt();
            bool valid = false;
            CardinalDirectionEnum::Enum enumSelected = CardinalDirectionEnum::fromIntegerCode(actionIntegerCode, &valid);
            if (valid) {
                return enumSelected;
            }
        }
    }
    
    CaretAssertMessage(0,
                       "Did not find selected cardinal direction.");
    
    return CardinalDirectionEnum::AUTO;
}



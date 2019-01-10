
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

#include <QComboBox>

#define __DISPLAY_GROUP_ENUM_COMBO_BOX_DECLARE__
#include "DisplayGroupEnumComboBox.h"
#undef __DISPLAY_GROUP_ENUM_COMBO_BOX_DECLARE__

#include "WuQMacroManager.h"

using namespace caret;


    
/**
 * \class caret::DisplayGroupEnumComboBox 
 * \brief Combo box for selection of a display group.
 * \ingroup GuiQt
 *
 * Encapsulates a QComboBox for the selection of a 
 * DisplayGroupEnum value.  QComboBox is not extended
 * to prevent access to its methods that could cause 
 * the selection to get messed up.
 */

/**
 * Constructor.
 * @param Parent
 *    Parent object.
 */
DisplayGroupEnumComboBox::DisplayGroupEnumComboBox(QObject* parent)
: DisplayGroupEnumComboBox(parent,
                           "",
                           "")
{
    
}
//: WuQWidget(parent)
//{
//    std::vector<DisplayGroupEnum::Enum> allDisplayGroups;
//    DisplayGroupEnum::getAllEnums(allDisplayGroups);
//    const int32_t numStructures = static_cast<int32_t>(allDisplayGroups.size());
//    
//    this->displayGroupComboBox = new QComboBox();
//    for (int32_t i = 0; i < numStructures; i++) {
//        this->displayGroupComboBox->addItem(DisplayGroupEnum::toGuiName(allDisplayGroups[i]));
//        this->displayGroupComboBox->setItemData(i, DisplayGroupEnum::toIntegerCode(allDisplayGroups[i]));
//    }
//    
//    QObject::connect(this->displayGroupComboBox, SIGNAL(activated(int)),
//                     this, SLOT(displayGroupComboBoxSelection(int)));
//}

/**
 * Constructor.
 * @param Parent
 *    Parent object.
 * @param objectNameForMacros
 *    Name of object for macros
 * @param descriptiveNameForMacros
 *    Descriptive name for macros
 */
DisplayGroupEnumComboBox::DisplayGroupEnumComboBox(QObject* parent,
                                                   const QString& objectNameForMacros,
                                                   const QString& descriptiveNameForMacros)
: WuQWidget(parent)
{
    std::vector<DisplayGroupEnum::Enum> allDisplayGroups;
    DisplayGroupEnum::getAllEnums(allDisplayGroups);
    const int32_t numStructures = static_cast<int32_t>(allDisplayGroups.size());
    
    this->displayGroupComboBox = new QComboBox();
    for (int32_t i = 0; i < numStructures; i++) {
        this->displayGroupComboBox->addItem(DisplayGroupEnum::toGuiName(allDisplayGroups[i]));
        this->displayGroupComboBox->setItemData(i, DisplayGroupEnum::toIntegerCode(allDisplayGroups[i]));
    }
    
    QObject::connect(this->displayGroupComboBox, SIGNAL(activated(int)),
                     this, SLOT(displayGroupComboBoxSelection(int)));
    
    if ( ! objectNameForMacros.isEmpty()) {
        this->displayGroupComboBox->setToolTip("Select Display Group");
        this->displayGroupComboBox->setObjectName(objectNameForMacros);
        WuQMacroManager::instance()->addMacroSupportToObject(this->displayGroupComboBox,
                                                             "Select display group for " + descriptiveNameForMacros);
    }
}

/**
 * Destructor.
 */
DisplayGroupEnumComboBox::~DisplayGroupEnumComboBox()
{
    
}

/**
 * @return The selected display group.
 */
DisplayGroupEnum::Enum 
DisplayGroupEnumComboBox::getSelectedDisplayGroup() const
{
    DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::getDefaultValue();
    const int32_t indx = this->displayGroupComboBox->currentIndex();
    if (indx >= 0) {
        const int32_t integerCode = this->displayGroupComboBox->itemData(indx).toInt();
        displayGroup = DisplayGroupEnum::fromIntegerCode(integerCode, NULL);
    }
    return displayGroup;
}

/**
 * @return The widget (combo box) for adding to layout.
 */
QWidget* 
DisplayGroupEnumComboBox::getWidget()
{
    return this->displayGroupComboBox;
}

/**
 * Set the display group.
 * @param displayGroup
 *    New value for display group.
 */
void 
DisplayGroupEnumComboBox::setSelectedDisplayGroup(const DisplayGroupEnum::Enum displayGroup)
{
    const int32_t displayGroupIntegerCode = DisplayGroupEnum::toIntegerCode(displayGroup);
    
    const int numStructures = this->displayGroupComboBox->count();
    for (int32_t i = 0; i < numStructures; i++) {
        if (displayGroupIntegerCode == this->displayGroupComboBox->itemData(i).toInt()) {
            if (this->signalsBlocked()) {
                this->displayGroupComboBox->blockSignals(true);
            }
            
            this->displayGroupComboBox->setCurrentIndex(i);
            
            if (this->signalsBlocked()) {
                this->displayGroupComboBox->blockSignals(false);
            }
            break;
        }
    }
}

/**
 * Called when a display group is selected (receives signal)
 * @param indx
 *    Index of selected item.
 */
void 
DisplayGroupEnumComboBox::displayGroupComboBoxSelection(int indx)
{
    if (this->signalsBlocked() == false) {
        const int32_t integerCode = this->displayGroupComboBox->itemData(indx).toInt();
        DisplayGroupEnum::Enum displayGroup = DisplayGroupEnum::fromIntegerCode(integerCode, NULL);
        emit displayGroupSelected(displayGroup);
    }
}

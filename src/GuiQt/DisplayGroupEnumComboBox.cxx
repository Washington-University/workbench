
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <QComboBox>

#define __DISPLAY_GROUP_ENUM_COMBO_BOX_DECLARE__
#include "DisplayGroupEnumComboBox.h"
#undef __DISPLAY_GROUP_ENUM_COMBO_BOX_DECLARE__

using namespace caret;


    
/**
 * \class caret::DisplayGroupEnumComboBox 
 * \brief Combo box for selection of a display group.
 * \addtogroup GuiQt
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

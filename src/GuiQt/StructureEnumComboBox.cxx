
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#define __STRUCTURE_ENUM_COMBOBOX_DECLARE__
#include "StructureEnumComboBox.h"
#undef __STRUCTURE_ENUM_COMBOBOX_DECLARE__

using namespace caret;


    
/**
 * \class caret::StructureEnumComboBox 
 * \brief Control for selection of a structure.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *   The parent.
 */
StructureEnumComboBox::StructureEnumComboBox(QObject* parent)
: WuQWidget(parent)
{
    std::vector<StructureEnum::Enum> allStructures;
    StructureEnum::getAllEnums(allStructures);
    const int32_t numStructures = static_cast<int32_t>(allStructures.size());
    
    this->structureComboBox = new QComboBox();
    for (int32_t i = 0; i < numStructures; i++) {
        this->structureComboBox->addItem(StructureEnum::toGuiName(allStructures[i]));
        this->structureComboBox->setItemData(i, StructureEnum::toIntegerCode(allStructures[i]));
    }
    
    QObject::connect(this->structureComboBox, SIGNAL(activated(int)),
                     this, SLOT(structureComboBoxSelection(int)));
    
}

/**
 * Destructor.
 */
StructureEnumComboBox::~StructureEnumComboBox()
{
    
}

/**
 * Called to set the selected structure.
 * @param structure
 *    New value for structure.
 */
void 
StructureEnumComboBox::setSelectedStructure(const StructureEnum::Enum structure)
{
    const int32_t structureIntegerCode = StructureEnum::toIntegerCode(structure);
    
    const int numStructures = this->structureComboBox->count();
    for (int32_t i = 0; i < numStructures; i++) {
        if (structureIntegerCode == this->structureComboBox->itemData(i).toInt()) {
            if (this->signalsBlocked()) {
                this->structureComboBox->blockSignals(true);
            }
            
            this->structureComboBox->setCurrentIndex(i);
            
            if (this->signalsBlocked()) {
                this->structureComboBox->blockSignals(false);
            }
            break;
        }
    }
}

/**
 * @return  The selected structure.
 */
StructureEnum::Enum 
StructureEnumComboBox::getSelectedStructure() const
{
    StructureEnum::Enum structure = StructureEnum::INVALID;
    const int32_t indx = this->structureComboBox->currentIndex();
    if (indx >= 0) {
        const int32_t integerCode = this->structureComboBox->itemData(indx).toInt();
        structure = StructureEnum::fromIntegerCode(integerCode, NULL);
    }
    return structure;
}

/**
 * @return The widget for this control.
 */
QWidget* 
StructureEnumComboBox::getWidget()
{
    return this->structureComboBox;
}

/**
 * Called when a structure is selected 
 * @param indx
 *   Index of selection.
 */
void 
StructureEnumComboBox::structureComboBoxSelection(int indx)
{
    if (this->signalsBlocked() == false) {
        const int32_t integerCode = this->structureComboBox->itemData(indx).toInt();
        StructureEnum::Enum structure = StructureEnum::fromIntegerCode(integerCode, NULL);
        emit structureSelected(structure);
    }
    
}

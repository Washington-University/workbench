
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

#define __STRUCTURE_ENUM_COMBOBOX_DECLARE__
#include "StructureEnumComboBox.h"
#undef __STRUCTURE_ENUM_COMBOBOX_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "GuiManager.h"

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
 * @return Number of items in combo box.
 */
int
StructureEnumComboBox::count() const
{
    return structureComboBox->count();
}

/**
 * Limit the available structure to the given structures
 *
 * @param structures
 *    Structures for display in combo box.
 */
void
StructureEnumComboBox::listOnlyTheseStructures(const std::vector<StructureEnum::Enum>& structures)
{
    this->structureComboBox->clear();
    this->structureComboBox->blockSignals(true);
    
    const int32_t numStructures = static_cast<int32_t>(structures.size());
    for (int32_t i = 0; i < numStructures; i++) {
        const StructureEnum::Enum structure = structures[i];
        this->structureComboBox->addItem(StructureEnum::toGuiName(structure));
        this->structureComboBox->setItemData(i, StructureEnum::toIntegerCode(structure));
    }
    
    this->structureComboBox->blockSignals(false);
}

/**
 * Limit selections to those structures that are loaded.
 */
void
StructureEnumComboBox::listOnlyValidStructures()
{
    const StructureEnum::Enum selectedStructure = getSelectedStructure();
    
    this->structureComboBox->clear();
    this->structureComboBox->blockSignals(true);
    
    int32_t selectedStructureIndex = -1;
    const Brain* brain = GuiManager::get()->getBrain();
    const int32_t numStructures = brain->getNumberOfBrainStructures();
    for (int32_t i = 0; i < numStructures; i++) {
        const BrainStructure* bs = brain->getBrainStructure(i);
        const StructureEnum::Enum structure = bs->getStructure();
        this->structureComboBox->addItem(StructureEnum::toGuiName(structure));
        this->structureComboBox->setItemData(i, StructureEnum::toIntegerCode(structure));
        
        if (selectedStructure != StructureEnum::INVALID) {
            if (structure == selectedStructure) {
                selectedStructureIndex = this->structureComboBox->count() - 1;
            }
        }
    }
    
    if (selectedStructureIndex >= 0) {
        this->structureComboBox->setCurrentIndex(selectedStructureIndex);
    }
    
    this->structureComboBox->blockSignals(false);
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
        if ((indx >= 0) &&
            (indx < this->structureComboBox->count())) {
            const int32_t integerCode = this->structureComboBox->itemData(indx).toInt();
            StructureEnum::Enum structure = StructureEnum::fromIntegerCode(integerCode, NULL);
            emit structureSelected(structure);
        }
    }
    
}

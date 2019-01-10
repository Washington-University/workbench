
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
#include <QGridLayout>
#include <QLabel>

#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventModelGetAll.h"
#include "WuQFactory.h"
#include "GuiManager.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

#define __STRUCTURE_SURFACE_SELECTION_CONTROL_DECLARE__
#include "StructureSurfaceSelectionControl.h"
#undef __STRUCTURE_SURFACE_SELECTION_CONTROL_DECLARE__

#include "ModelSurface.h"
#include "ModelSurfaceSelector.h"
#include "Surface.h"

using namespace caret;

/**
 * \class caret::StructureSufaceSelectionControl 
 * \brief 
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param showLabels
 *     Show labels on controls
 * @param objectNamePrefix
 *     Object name prefix used for macros
 * @param descriptivePrefix
 *     Descriptive prefix for macros
 * @param parent
 *     Parent widget for controls
 */
StructureSurfaceSelectionControl::StructureSurfaceSelectionControl(const bool showLabels,
                                                                   const QString& objectNamePrefix,
                                                                   const QString& descriptivePrefix,
                                                                   QWidget* parent)
: QWidget(parent)
{
    this->surfaceControllerSelector = NULL;

    this->structureSelectionComboBox = WuQFactory::newComboBox();
    this->structureSelectionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QObject::connect(this->structureSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(structureSelected(int)));
    this->structureSelectionComboBox->setToolTip("Selects Structure of Surface");
    this->structureSelectionComboBox->setObjectName(objectNamePrefix
                                                    + ":StructureSelection");
    WuQMacroManager::instance()->addMacroSupportToObject(this->structureSelectionComboBox,
                                                         "Select structure for " + descriptivePrefix);
    
    this->surfaceControllerSelectionComboBox = WuQFactory::newComboBox();
    this->surfaceControllerSelectionComboBox->setSizeAdjustPolicy(QComboBox::AdjustToContents);
    QObject::connect(this->surfaceControllerSelectionComboBox, SIGNAL(currentIndexChanged(int)),
                     this, SLOT(surfaceControllerSelected(int)));
    this->surfaceControllerSelectionComboBox->setToolTip("Selects Surface");
    this->surfaceControllerSelectionComboBox->setObjectName(objectNamePrefix
                                                    + ":SurfaceSelection");
    WuQMacroManager::instance()->addMacroSupportToObject(this->surfaceControllerSelectionComboBox,
                                                         "Select surface for " + descriptivePrefix);

    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 3, 2);
    if (showLabels) {
        QLabel* structureLabel = new QLabel("Structure");
        QLabel* surfaceLabel = new QLabel("Surface");
        layout->addWidget(structureLabel, 0, 0);
        layout->addWidget(surfaceLabel, 1, 0);
    }
    layout->addWidget(this->structureSelectionComboBox, 0, 1);
    layout->addWidget(this->surfaceControllerSelectionComboBox, 1, 1);  
}

/**
 * Destructor.
 */
StructureSurfaceSelectionControl::~StructureSurfaceSelectionControl()
{
    
}

void 
StructureSurfaceSelectionControl::structureSelected(int currentIndex)
{
    CaretAssert((currentIndex >= 0) 
                && (currentIndex < this->structureSelectionComboBox->count()));
    int32_t structID = this->structureSelectionComboBox->itemData(currentIndex).toInt();
    StructureEnum::Enum selectedStructure = StructureEnum::fromIntegerCode(structID, NULL);
    this->surfaceControllerSelector->setSelectedStructure(selectedStructure);
    
    emitSelectionChangedSignal();
//    emit selectionChanged(this->surfaceControllerSelector->getSelectedStructure(),
//                          this->surfaceControllerSelector->getSelectedSurfaceModel());

    this->updateControlAfterSelection();
}

void 
StructureSurfaceSelectionControl::surfaceControllerSelected(int currentIndex)
{
    CaretAssert((currentIndex >= 0) 
                && (currentIndex < this->surfaceControllerSelectionComboBox->count()));
    void* pointer = this->surfaceControllerSelectionComboBox->itemData(currentIndex).value<void*>();
    ModelSurface* surfaceController = (ModelSurface*)pointer;
    this->surfaceControllerSelector->setSelectedSurfaceModel(surfaceController);
    
    emitSelectionChangedSignal();
//    emit selectionChanged(this->surfaceControllerSelector->getSelectedStructure(),
//                          this->surfaceControllerSelector->getSelectedSurfaceModel());
}

/**
 * Emit the selection changed signal.
 * Also preserves mouse focus that can be disrupted by user-interface updates.
 */
void
StructureSurfaceSelectionControl::emitSelectionChangedSignal()
{
    const bool structureHasFocus = this->structureSelectionComboBox->hasFocus();
    const bool surfaceHasFocus   = this->surfaceControllerSelectionComboBox->hasFocus();
    emit selectionChanged(this->surfaceControllerSelector->getSelectedStructure(),
                          this->surfaceControllerSelector->getSelectedSurfaceModel());
    if (structureHasFocus) {
        this->structureSelectionComboBox->setFocus();
    }
    else if (surfaceHasFocus) {
        this->surfaceControllerSelectionComboBox->setFocus();
    }
}



/**
 * @return The selected mode surface.
 */
ModelSurface* 
StructureSurfaceSelectionControl::getSelectedSurfaceModel()
{
    return this->surfaceControllerSelector->getSelectedSurfaceModel();
}

/**
 * @return The selected structure.
 */
StructureEnum::Enum 
StructureSurfaceSelectionControl::getSelectedStructure()
{
    return this->surfaceControllerSelector->getSelectedStructure();
}

/*
void 
StructureSurfaceSelectionControl::setSelectedSurfaceModel(
                        ModelSurface* surfaceController)
{
    this->surfaceControllerSelector->setSelectedSurfaceModel(surfaceController);
    this->updateControl();
}

void 
StructureSurfaceSelectionControl::setSelectedStructure(const StructureEnum::Enum selectedStructure)
{
    this->surfaceControllerSelector->setSelectedStructure(selectedStructure);
    this->updateControl();
}
*/

void 
StructureSurfaceSelectionControl::updateControl(ModelSurfaceSelector* surfaceControllerSelector)
{
    this->surfaceControllerSelector = surfaceControllerSelector;
    this->updateControlAfterSelection();
}

void
StructureSurfaceSelectionControl::updateControlAfterSelection()
{
    /*
     * Don't let any signals get sent by updating.
     */
    this->structureSelectionComboBox->blockSignals(true);
    this->surfaceControllerSelectionComboBox->blockSignals(true);
    
    this->structureSelectionComboBox->clear();
    this->surfaceControllerSelectionComboBox->clear();
    
    if (this->surfaceControllerSelector == NULL) {
        return;
    }
    
    std::vector<ModelSurface*> availableSurfaceModels;
    std::vector<StructureEnum::Enum> availableStructures;
    
    this->surfaceControllerSelector->getSelectableStructures(availableStructures);
    this->surfaceControllerSelector->getSelectableSurfaceModels(availableSurfaceModels);
    
    Surface* primaryAnatomicalSurface = NULL;
    
    /*
     * Update the structure selection.
     */
    StructureEnum::Enum selectedStructure = this->surfaceControllerSelector->getSelectedStructure();
    int32_t defaultStructureIndex = 0;
    for (int32_t i = 0; i < static_cast<int32_t>(availableStructures.size()); i++) {
        StructureEnum::Enum structType = availableStructures[i];
        if (structType == selectedStructure) {
            defaultStructureIndex = i;
        }
        this->structureSelectionComboBox->addItem(StructureEnum::toGuiName(structType),
                                                  StructureEnum::toIntegerCode(structType));
    }
    if (defaultStructureIndex < this->structureSelectionComboBox->count()) {
        this->structureSelectionComboBox->setCurrentIndex(defaultStructureIndex);
        
        BrainStructure* brainStructure = GuiManager::get()->getBrain()->getBrainStructure(availableStructures[defaultStructureIndex],
                                                                                          false);
        if (brainStructure != NULL) {
            primaryAnatomicalSurface = brainStructure->getPrimaryAnatomicalSurface();
        }
    }
    
    const bool allSelected (selectedStructure == StructureEnum::ALL);
    
    /*
     * Update the surface selection.
     */
    ModelSurface* selectedSurfaceController =
        this->surfaceControllerSelector->getSelectedSurfaceModel();
    int32_t defaultSurfaceIndex = -1;
    int32_t primaryAnatomicalSurfaceIndex = -1;
    for (std::vector<ModelSurface*>::const_iterator iter = availableSurfaceModels.begin();
         iter != availableSurfaceModels.end();
         iter++) {
        ModelSurface* surfaceController = *iter;
        this->surfaceControllerSelectionComboBox->addItem(surfaceController->getNameForGUI(allSelected),
                                                          qVariantFromValue((void*)surfaceController));
        if (selectedSurfaceController == surfaceController) {
            defaultSurfaceIndex = this->surfaceControllerSelectionComboBox->count() - 1;
        }
        if (surfaceController->getSurface() == primaryAnatomicalSurface) {
            primaryAnatomicalSurfaceIndex = this->surfaceControllerSelectionComboBox->count() - 1;
        }
    }
    
    if (defaultSurfaceIndex < 0) {
        if (primaryAnatomicalSurfaceIndex >= 0) {
            defaultSurfaceIndex = primaryAnatomicalSurfaceIndex;
        }
        else if (this->surfaceControllerSelectionComboBox->count() > 0) {
            defaultSurfaceIndex = 0;
        }
    }
    if (defaultSurfaceIndex < this->surfaceControllerSelectionComboBox->count()) {
        this->surfaceControllerSelectionComboBox->setCurrentIndex(defaultSurfaceIndex);
    }
    
    this->structureSelectionComboBox->blockSignals(false);
    this->surfaceControllerSelectionComboBox->blockSignals(false);
}


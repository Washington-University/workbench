
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

#include <set>
#include <limits>

#define __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG_DECLARE__
#include "RegionOfInterestCreateFromBorderDialog.h"
#undef __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG_DECLARE__

#include <QButtonGroup>
#include <QDoubleSpinBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>

using namespace caret;

#include "AlgorithmException.h"
#include "AlgorithmNodesInsideBorder.h"
#include "Border.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretMappableDataFileAndMapSelector.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GiftiLabelTableEditor.h"
#include "GuiManager.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Surface.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

    
/**
 * \class caret::RegionOfInterestCreateFromBorderDialog 
 * \brief Dailog for creating regions of interest from border(s).
 *
 * Dialog that creates regions of interest from nodes inside
 * borders.
 */

/**
 * Constructor.
 *
 * @param border
 *   Border used to create region of interest.
 * @param surface
 *   Surface on which border is drawn and whose nodes are selected.
 * @param parent
 *   Parent on which dialog is displayed.
 *   
 */
RegionOfInterestCreateFromBorderDialog::RegionOfInterestCreateFromBorderDialog(Border* border,
                                                                               Surface* surface,
                                                                               QWidget* parent)
: WuQDialogModal("Create Region of Interest",
                 parent)
{
    std::vector<Border*> borders;
    borders.push_back(border);
    
    std::vector<Surface*> surfaces;
    surfaces.push_back(surface);
    
    this->createDialog(borders, 
                       surfaces);
    
}

/**
 * Destructor.
 */
RegionOfInterestCreateFromBorderDialog::~RegionOfInterestCreateFromBorderDialog()
{
    
}

/**
 * Create the dialog.
 *
 * @param borders
 *    Borders used to create regions of interest.
 * @param surfaceFiles
 *    Surface files used for node selections.
 */
void 
RegionOfInterestCreateFromBorderDialog::createDialog(const std::vector<Border*>& borders,
                                                     std::vector<Surface*>& surfaces)
{
    this->borders   = borders;
    this->surfaces = surfaces;
    
    std::set<StructureEnum::Enum> requiredStructures;
    const int32_t numberOfBorders = static_cast<int32_t>(borders.size());
    for (int32_t i = 0; i < numberOfBorders; i++) {
        const StructureEnum::Enum structure = borders[i]->getStructure();
        requiredStructures.insert(structure);
    }
    
    QWidget* selectorWidget = this->createSelectors(requiredStructures, 
                                            this->surfaces, 
                                            this->mapFileTypeSelectors);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutMargins(dialogLayout, 
                                    2, 
                                    2);
    dialogLayout->addWidget(selectorWidget);
    
    this->setCentralWidget(widget);
}

/**
 * Create map files selectors for a map file type.
 *
 * @param mapFileType
 *    Type of map file for selector.
 * @param requiredStructures
 *    Structures needed.
 * @param surfaces
 *    Surfaces available.
 * @param mapFileSelectors
 *    Output containing the selectors.
 * @return Widget containing the map selector controls.
 */
QWidget* 
RegionOfInterestCreateFromBorderDialog::createSelectors(std::set<StructureEnum::Enum>& requiredStructures,
                                                        std::vector<Surface*>& surfaces,
                                                        STRUCTURE_MAP_FILE_SELECTOR_MAP& mapFileSelectors)
{
    QWidget* widget = new QWidget();
    QVBoxLayout* mapSelectionLayout = new QVBoxLayout(widget);
    
    for (std::set<StructureEnum::Enum>::iterator structureIter = requiredStructures.begin();
         structureIter != requiredStructures.end();
         structureIter++) {
        const StructureEnum::Enum structure = *structureIter;
        
        for (std::vector<Surface*>::iterator surfaceIter = this->surfaces.begin();
             surfaceIter != surfaces.end();
             surfaceIter++) {
            Surface* surface = *surfaceIter;
            if (surface->getStructure() == structure) {
                BrainStructure* brainStructure = surface->getBrainStructure();
                CaretMappableDataFileAndMapSelector* mapSelector =
                    new CaretMappableDataFileAndMapSelector(brainStructure,
                                                        this);
                QObject::connect(mapSelector, SIGNAL(selectionChanged(CaretMappableDataFileAndMapSelector*)),
                                 this, SLOT(fileSelectionWasChanged(CaretMappableDataFileAndMapSelector*)));
                
                mapSelectionLayout->addWidget(mapSelector->getWidget());
                
                mapFileSelectors.insert(std::make_pair(structure,
                                                       mapSelector));
            }
        }             
    }
    
    return widget;
}

/**
 * Called when a map type/file/name is selected.
 * @param selector
 *    Selector in which selection was made.
 */
void 
RegionOfInterestCreateFromBorderDialog::fileSelectionWasChanged(CaretMappableDataFileAndMapSelector* selector)
{
//    std::cout << "Selection changed. " << std::endl;    
}

/**
 * Called when the user presses the OK button.
 */
void 
RegionOfInterestCreateFromBorderDialog::okButtonPressed()
{
    AString errorMessage;
    
    for (STRUCTURE_MAP_FILE_SELECTOR_ITERATOR iter = this->mapFileTypeSelectors.begin();
         iter != this->mapFileTypeSelectors.end();
         iter++) {
        //const StructureEnum::Enum structure = iter->first;
        CaretMappableDataFileAndMapSelector* mapSelector = iter->second;
        
        AString msg;
        if (mapSelector->isValidSelections(msg) == false) {
            if (errorMessage.isEmpty() == false) {
                errorMessage += "\n";
            }
            errorMessage += msg;
        }
    }
    
    bool allowDialogToClose = true;
    if (errorMessage.isEmpty() == false) {
        allowDialogToClose = false;
    }
    else {
        for (std::vector<Border*>::iterator borderIterator = this->borders.begin();
             borderIterator != this->borders.end();
             borderIterator++) {
            Border* border = *borderIterator;
            const StructureEnum::Enum structure = border->getStructure();
            STRUCTURE_MAP_FILE_SELECTOR_ITERATOR structureMapIterator = this->mapFileTypeSelectors.find(structure);
            if (structureMapIterator != this->mapFileTypeSelectors.end()) {
                const StructureEnum::Enum structure = structureMapIterator->first;
                CaretMappableDataFileAndMapSelector* mapSelector = structureMapIterator->second;
                mapSelector->saveCurrentSelections(); // save for next time
                
                switch (mapSelector->getSelectedMapFileType()) {
                    case DataFileTypeEnum::LABEL:
                    {
                        LabelFile* labelFile = dynamic_cast<LabelFile*>(mapSelector->getSelectedMapFile());
                        const int32_t mapIndex = mapSelector->getSelectedMapIndex();
                        
                        const int32_t labelKey = mapSelector->getSelectedLabelKey(); 
                        
                        Surface* surface = NULL;
                        for (std::vector<Surface*>::iterator surfaceIterator = this->surfaces.begin();
                             surfaceIterator != this->surfaces.end();
                             surfaceIterator++) {
                            Surface* s = *surfaceIterator;
                            if (s->getStructure() == structure) {
                                surface = s;
                                break;
                            }
                        }
                        CaretAssert(surface);
                        
                        if (surface != NULL) {
                            try {
                                AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                                                 surface,
                                                                                 border,
                                                                                 mapIndex,
                                                                                 labelKey,
                                                                                 labelFile);
                            }
                            catch (const AlgorithmException& e) {
                                if (errorMessage.isEmpty() == false) {
                                    errorMessage += "\n";
                                }
                                errorMessage += e.whatString();
                            }
                        }
                        else {
                            if (errorMessage.isEmpty() == false) {
                                errorMessage += "\n";
                            }
                            errorMessage += ("Surface for structure "
                                             + StructureEnum::toGuiName(structure)
                                             + " was not found");
                        }
                        
                    }
                        break;
                    case DataFileTypeEnum::METRIC:
                    {
                        MetricFile* metricFile = dynamic_cast<MetricFile*>(mapSelector->getSelectedMapFile());
                        const int32_t mapIndex = mapSelector->getSelectedMapIndex();
                        const float value = mapSelector->getSelectedMetricValue();
                        
                        Surface* surface = NULL;
                        for (std::vector<Surface*>::iterator surfaceIterator = this->surfaces.begin();
                             surfaceIterator != this->surfaces.end();
                             surfaceIterator++) {
                            Surface* s = *surfaceIterator;
                            if (s->getStructure() == structure) {
                                surface = s;
                                break;
                            }
                        }
                        CaretAssert(surface);
                        
                        if (surface != NULL) {
                            try {
                                AlgorithmNodesInsideBorder algorithmInsideBorder(NULL,
                                                                                 surface,
                                                                                 border,
                                                                                 mapIndex,
                                                                                 value,
                                                                                 metricFile);
                            }
                            catch (const AlgorithmException& e) {
                                if (errorMessage.isEmpty() == false) {
                                    errorMessage += "\n";
                                }
                                errorMessage += e.whatString();
                            }
                        }
                        else {
                            if (errorMessage.isEmpty() == false) {
                                errorMessage += "\n";
                            }
                            errorMessage += ("Surface for structure "
                                             + StructureEnum::toGuiName(structure)
                                             + " was not found");
                        }
                        
                    }
                        break;
                    default:
                        CaretAssertMessage(0, "Unsupported file type.");
                        break;
                }
            }
            else {
                
            }
        }        
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());

    if (errorMessage.isEmpty() == false) {
        WuQMessageBox::errorOk(this, 
                               errorMessage);
    }
    
    if (allowDialogToClose) {
        WuQDialogModal::okButtonPressed();
    }
}



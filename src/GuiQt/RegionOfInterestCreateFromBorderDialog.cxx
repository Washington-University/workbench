
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

#include <set>
#include <limits>

#define __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG_DECLARE__
#include "RegionOfInterestCreateFromBorderDialog.h"
#undef __REGION_OF_INTEREST_CREATE_FROM_BORDER_DIALOG_DECLARE__

#include <QButtonGroup>
#include <QCheckBox>
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
#include "BorderFile.h"
#include "Brain.h"
#include "BrainStructure.h"
#include "CaretAssert.h"
#include "CaretMappableDataFileAndMapSelector.h"
#include "CiftiBrainordinateLabelFile.h"
#include "CiftiBrainordinateScalarFile.h"
#include "CursorDisplayScoped.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "Surface.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

    
/**
 * \class caret::RegionOfInterestCreateFromBorderDialog 
 * \brief Dailog for creating regions of interest from border(s).
 * \ingroup GuiQt
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
    
    this->inverseCheckBox = new QCheckBox("Invert Selected Vertices");
    
    QWidget* widget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(dialogLayout, 
                                    2, 
                                    2);
    dialogLayout->addWidget(selectorWidget);
    dialogLayout->addWidget(this->inverseCheckBox,
                            0,
                            Qt::AlignLeft);
    
    this->setCentralWidget(widget,
                           WuQDialog::SCROLL_AREA_NEVER);
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
    AString borderName;
    if (this->borders.empty() == false) {
        borderName = this->borders[0]->getName();
    }
    
    QWidget* widget = new QWidget();
    QVBoxLayout* mapSelectionLayout = new QVBoxLayout(widget);
    
    std::vector<DataFileTypeEnum::Enum> allowedMapFileTypes;
    allowedMapFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL);
    allowedMapFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL_DYNAMIC);
    allowedMapFileTypes.push_back(DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR);
    allowedMapFileTypes.push_back(DataFileTypeEnum::LABEL);
    allowedMapFileTypes.push_back(DataFileTypeEnum::METRIC);

    for (std::set<StructureEnum::Enum>::iterator structureIter = requiredStructures.begin();
         structureIter != requiredStructures.end();
         structureIter++) {
        const StructureEnum::Enum structure = *structureIter;
        
        for (std::vector<Surface*>::iterator surfaceIter = this->surfaces.begin();
             surfaceIter != surfaces.end();
             surfaceIter++) {
            Surface* surface = *surfaceIter;
            if (surface->getStructure() == structure) {
                std::vector<StructureEnum::Enum> allowedStructures;
                allowedStructures.push_back(surface->getStructure());
                CaretMappableDataFileAndMapSelector* mapSelector =
                    new CaretMappableDataFileAndMapSelector(borderName,
                                                            GuiManager::get()->getBrain(),
                                                            allowedMapFileTypes,
                                                            allowedStructures,
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
RegionOfInterestCreateFromBorderDialog::fileSelectionWasChanged(CaretMappableDataFileAndMapSelector* /*selector*/)
{
//    std::cout << "Selection changed. " << std::endl;    
}

/**
 * Called when the user presses the OK button.
 */
void 
RegionOfInterestCreateFromBorderDialog::okButtonClicked()
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
    
    BorderFile* debugBorderFile = NULL;
    
    const bool isInverseSelection = this->inverseCheckBox->isChecked();
    
    bool allowDialogToClose = true;
    if (errorMessage.isEmpty() == false) {
        allowDialogToClose = false;
    }
    else {
        /*
         * Show the wait cursor.
         */
        CursorDisplayScoped cursor;
        cursor.showWaitCursor();
        
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
                    case DataFileTypeEnum::CONNECTIVITY_DENSE_LABEL:
                    {
                        AString errorMessage;
                        CiftiBrainordinateLabelFile* ciftiLabelFile = dynamic_cast<CiftiBrainordinateLabelFile*>(mapSelector->getSelectedMapFile());
                        CaretAssert(ciftiLabelFile);
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
                                                                                 isInverseSelection,
                                                                                 mapIndex,
                                                                                 labelKey,
                                                                                 ciftiLabelFile);
                                const BorderFile* dbf = algorithmInsideBorder.getDebugBorderFile();
                                if (dbf != NULL) {
                                    debugBorderFile = new BorderFile(*dbf);
                                }
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
                    case DataFileTypeEnum::CONNECTIVITY_DENSE_SCALAR:
                    {
                        AString errorMessage;
                        CiftiBrainordinateScalarFile* ciftiScalarFile = dynamic_cast<CiftiBrainordinateScalarFile*>(mapSelector->getSelectedMapFile());
                        CaretAssert(ciftiScalarFile);
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
                                                                                 isInverseSelection,
                                                                                 mapIndex,
                                                                                 value,
                                                                                 ciftiScalarFile);
                                const BorderFile* dbf = algorithmInsideBorder.getDebugBorderFile();
                                if (dbf != NULL) {
                                    debugBorderFile = new BorderFile(*dbf);
                                }
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
                                                                                 isInverseSelection,
                                                                                 mapIndex,
                                                                                 labelKey,
                                                                                 labelFile);
                                const BorderFile* dbf = algorithmInsideBorder.getDebugBorderFile();
                                if (dbf != NULL) {
                                    debugBorderFile = new BorderFile(*dbf);
                                }
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
                                                                                 isInverseSelection,
                                                                                 mapIndex,
                                                                                 value,
                                                                                 metricFile);
                                const BorderFile* dbf = algorithmInsideBorder.getDebugBorderFile();
                                if (dbf != NULL) {
                                    debugBorderFile = new BorderFile(*dbf);
                                }
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
    
    if (debugBorderFile != NULL) {
        EventManager::get()->sendEvent(EventDataFileAdd(debugBorderFile).getPointer());
    }
    
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());

    if (errorMessage.isEmpty() == false) {
        WuQMessageBox::errorOk(this, 
                               errorMessage);
    }
    
    if (allowDialogToClose) {
        WuQDialogModal::okButtonClicked();
    }
}



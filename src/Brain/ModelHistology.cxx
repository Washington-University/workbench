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

#include "Brain.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "CaretDataFileSelectionModel.h"
#include "CaretLogger.h"
#include "EventDataFileAdd.h"
#include "EventManager.h"
#include "GraphicsRegionSelectionBox.h"
#include "HistologySlicesFile.h"
#include "MapFileDataSelector.h"
#include "ModelHistology.h"
#include "HistologyOverlay.h"
#include "HistologyOverlaySet.h"
#include "HistologyOverlaySetArray.h"
#include "OverlaySetArray.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * Constructor.
 *
 */
ModelHistology::ModelHistology(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_HISTOLOGY,
        brain)
{
    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    m_layerOverlaySetArray = new OverlaySetArray(overlaySurfaceStructures,
                                            Overlay::INCLUDE_VOLUME_FILES_FOR_HISTOLOGY_MODEL,
                                            "Histology View");

    m_histologyOverlaySetArray = new HistologyOverlaySetArray("Histology View");
    
    std::vector<DataFileTypeEnum::Enum> supportedDataFileTypes {
        DataFileTypeEnum::HISTOLOGY_SLICES
    };
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        CaretAssertArrayIndex(m_fileSelectionModel, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, i);
        m_fileSelectionModels[i].reset(CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes(supportedDataFileTypes));
    }
    
    initializeHistology();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
}

/**
 * Destructor
 */
ModelHistology::~ModelHistology()
{
    delete m_layerOverlaySetArray;
    delete m_histologyOverlaySetArray;
    EventManager::get()->removeAllEventsFromListener(this);
}

void
ModelHistology::initializeHistology()
{
    m_layerOverlaySetArray->initializeOverlaySelections();
}

/**
 * Update the model.
 */
void
ModelHistology::updateModel()
{
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
ModelHistology::receiveEvent(Event* /*event*/)
{
}

/**
 * @return File selection model for the given tab index
 * @param tabIndex
 *    Index of the tab
 */
CaretDataFileSelectionModel*
ModelHistology::getFileSelectionModel(const int32_t tabIndex)
{
    CaretAssertArrayIndex(m_fileSelectionModels, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_fileSelectionModels[tabIndex].get();
}

/**
 * @return File selection model for the given tab index
 * @param tabIndex
 *    Index of the tab
 */
const CaretDataFileSelectionModel*
ModelHistology::getFileSelectionModel(const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_fileSelectionModels, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, tabIndex);
    return m_fileSelectionModels[tabIndex].get();
}

/**
 * Get the name for use in a GUI.
 *
 * @param includeStructureFlag - Prefix label with structure to which
 *      this structure model belongs.
 * @return   Name for use in a GUI.
 *
 */
AString
ModelHistology::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    AString name = "Histology";
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model.
 */
AString 
ModelHistology::getNameForBrowserTab() const
{
    AString name = "Histology";
    return name;
}

/**
 * Get the histology overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Histology overlay set at the given tab index.
 */
HistologyOverlaySet*
ModelHistology::getHistologyOverlaySet(const int tabIndex)
{
    CaretAssertArrayIndex(m_histologyOverlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_histologyOverlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Get the histology overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Histology Overlay set at the given tab index.
 */
const HistologyOverlaySet*
ModelHistology::getHistologyOverlaySet(const int tabIndex) const
{
    CaretAssertArrayIndex(m_histologyOverlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_histologyOverlaySetArray->getOverlaySet(tabIndex);
}


/**
 * Initilize the overlays for this model.
 */
void 
ModelHistology::initializeOverlays()
{
    m_layerOverlaySetArray->initializeOverlaySelections();
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param sceneClass
 *    SceneClass to which model specific information is added.
 */
void
ModelHistology::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                   SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    if ( ! tabIndices.empty()) {
        SceneClass* histologySetClass = m_histologyOverlaySetArray->saveTabIndicesToScene(tabIndices,
                                                                                  sceneAttributes,
                                                                                  "m_histologyOverlaySetArray");
        if (histologySetClass != NULL) {
            sceneClass->addClass(histologySetClass);
        }
    }

    std::vector<SceneClass*> fileSelectionClasses;
    for (auto& fs : m_fileSelectionModels) {
        AString name("m_fileSelectionModels_"
                     + AString::number(fileSelectionClasses.size()));
        fileSelectionClasses.push_back(fs->saveToScene(sceneAttributes,
                                                       name));
    }
    
    SceneClassArray* fileModels = new SceneClassArray("fileSelectionClasses",
                                                      fileSelectionClasses);
    sceneClass->addChild(fileModels);
    
    /*
     * Brainordinate (volume) layers were added to display label
     * volumes over histology and this indicates their usage.
     */
    sceneClass->addBoolean("hasLayerOverlays",
                           true);
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
ModelHistology::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                     const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const SceneClassArray* fileModels = sceneClass->getClassArray("fileSelectionClasses");
    if (fileModels != NULL) {
        const int32_t numItems = fileModels->getNumberOfArrayElements();
        for (int32_t i = 0; i < numItems; i++) {
            CaretAssertArrayIndex(m_fileSelectionModels, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, i);
            m_fileSelectionModels[i]->restoreFromScene(sceneAttributes,
                                                       fileModels->getClassAtIndex(i));
        }
    }
    
    m_histologyOverlaySetArray->restoreFromScene(sceneAttributes,
                                                      sceneClass->getClass("m_histologyOverlaySetArray"));
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    if (sceneClass->getBooleanValue("hasLayerOverlays",
                                    false)) {
        /* Nothing */
    }
    else {
        /*
         * If we are here, it indicates a scene that was created
         * before brainordinate (volume) layers were used to display
         * volume label files.  We need to initialize the layers
         * so that it turns off any volume label files as they
         * are turned on by default.
         */
        m_layerOverlaySetArray->initializeOverlaySelections();
    }
}

/**
 * Copy the tab content from the source tab index to the
 * destination tab index.
 *
 * @param sourceTabIndex
 *    Source from which tab content is copied.
 * @param destinationTabIndex
 *    Destination to which tab content is copied.
 */
void
ModelHistology::copyTabContent(const int32_t sourceTabIndex,
                               const int32_t destinationTabIndex)
{
    Model::copyTabContent(sourceTabIndex,
                          destinationTabIndex);
    
    m_layerOverlaySetArray->copyOverlaySet(sourceTabIndex,
                                      destinationTabIndex);
    m_histologyOverlaySetArray->copyOverlaySet(sourceTabIndex,
                                          destinationTabIndex);
    CaretAssertArrayIndex(m_fileSelectionModels, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, sourceTabIndex);
    CaretAssertArrayIndex(m_fileSelectionModels, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, destinationTabIndex);
    m_fileSelectionModels[destinationTabIndex]->setSelectedFile(m_fileSelectionModels[sourceTabIndex]->getSelectedFile());
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet*
ModelHistology::getOverlaySet(const int tabIndex)
{
    CaretAssertArrayIndex(m_overlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_layerOverlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet*
ModelHistology::getOverlaySet(const int tabIndex) const
{
    CaretAssertArrayIndex(m_overlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_layerOverlaySetArray->getOverlaySet(tabIndex);
}

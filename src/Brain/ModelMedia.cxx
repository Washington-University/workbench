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
#include "ImageFile.h"
#include "MapFileDataSelector.h"
#include "ModelMedia.h"
#include "MediaOverlay.h"
#include "MediaOverlaySet.h"
#include "MediaOverlaySetArray.h"
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
ModelMedia::ModelMedia(Brain* brain)
: Model(ModelTypeEnum::MODEL_TYPE_MULTI_MEDIA,
        brain)
{
    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    m_layerOverlaySetArray = new OverlaySetArray(overlaySurfaceStructures,
                                            Overlay::INCLUDE_VOLUME_FILES_YES,
                                            "Chart View");

    m_mediaOverlaySetArray = new MediaOverlaySetArray("Media View");
    
    std::vector<DataFileTypeEnum::Enum> supportedDataFileTypes {
        DataFileTypeEnum::IMAGE
    };
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        CaretAssertArrayIndex(m_fileSelectionModel, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, i);
        m_fileSelectionModels[i].reset(CaretDataFileSelectionModel::newInstanceForCaretDataFileTypes(supportedDataFileTypes));
    }
    
    initializeMedia();
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
}

/**
 * Destructor
 */
ModelMedia::~ModelMedia()
{
    delete m_layerOverlaySetArray;
    delete m_mediaOverlaySetArray;
    EventManager::get()->removeAllEventsFromListener(this);
}

void
ModelMedia::initializeMedia()
{
    m_layerOverlaySetArray->initializeOverlaySelections();
}

/**
 * Update the model.
 */
void
ModelMedia::updateModel()
{
}

/**
 * Reset this model.
 */
void
ModelMedia::reset()
{
    initializeMedia();
}

/**
 * Receive an event.
 * 
 * @param event
 *     The event that the receive can respond to.
 */
void 
ModelMedia::receiveEvent(Event* /*event*/)
{
}

/**
 * @return File selection model for the given tab index
 * @param tabIndex
 *    Index of the tab
 */
CaretDataFileSelectionModel*
ModelMedia::getFileSelectionModel(const int32_t tabIndex)
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
ModelMedia::getFileSelectionModel(const int32_t tabIndex) const
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
ModelMedia::getNameForGUI(const bool /*includeStructureFlag*/) const
{
    AString name = "Media";
    return name;
}

/**
 * @return The name that should be displayed in the tab
 * displaying this model.
 */
AString 
ModelMedia::getNameForBrowserTab() const
{
    AString name = "Media";
    return name;
}

/**
 * Get the media overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Media overlay set at the given tab index.
 */
MediaOverlaySet*
ModelMedia::getMediaOverlaySet(const int tabIndex)
{
    CaretAssertArrayIndex(m_mediaOverlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_mediaOverlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Get the media overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Media Overlay set at the given tab index.
 */
const MediaOverlaySet*
ModelMedia::getMediaOverlaySet(const int tabIndex) const
{
    CaretAssertArrayIndex(m_mediaOverlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_mediaOverlaySetArray->getOverlaySet(tabIndex);
}


/**
 * Initilize the overlays for this model.
 */
void 
ModelMedia::initializeOverlays()
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
ModelMedia::saveModelSpecificInformationToScene(const SceneAttributes* sceneAttributes,
                                                   SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    if ( ! tabIndices.empty()) {
        SceneClass* mediaSetClass = m_mediaOverlaySetArray->saveTabIndicesToScene(tabIndices,
                                                                                  sceneAttributes,
                                                                                  "m_mediaOverlaySetArray");
        if (mediaSetClass != NULL) {
            sceneClass->addClass(mediaSetClass);
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
ModelMedia::restoreModelSpecificInformationFromScene(const SceneAttributes* sceneAttributes,
                                                     const SceneClass* sceneClass)
{
    reset();
    
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
    
    m_mediaOverlaySetArray->restoreFromScene(sceneAttributes,
                                                      sceneClass->getClass("m_mediaOverlaySetArray"));
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
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
ModelMedia::copyTabContent(const int32_t sourceTabIndex,
                      const int32_t destinationTabIndex)
{
    Model::copyTabContent(sourceTabIndex,
                          destinationTabIndex);
    
    m_layerOverlaySetArray->copyOverlaySet(sourceTabIndex,
                                      destinationTabIndex);
    m_mediaOverlaySetArray->copyOverlaySet(sourceTabIndex,
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
ModelMedia::getOverlaySet(const int tabIndex)
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
ModelMedia::getOverlaySet(const int tabIndex) const
{
    CaretAssertArrayIndex(m_overlaySetArray,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          tabIndex);
    return m_layerOverlaySetArray->getOverlaySet(tabIndex);
}

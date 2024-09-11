
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __DATA_FILE_LABEL_SELECTION_DELEGATE_DECLARE__
#include "DataFileLabelSelectionDelegate.h"
#undef __DATA_FILE_LABEL_SELECTION_DELEGATE_DECLARE__

#include "BrainConstants.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretMappableDataFile.h"
#include "GiftiLabelTable.h"
#include "LabelSelectionItemModel.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"

using namespace caret;


    
/**
 * \class caret::DataFileLabelSelectionDelegate 
 * \brief Handles label hierarchies for data files
 * \ingroup Files
 */

/**
 * Constructor.
 * @param mapFile
 *    The data file
 * @param mapIndex
 *    Index of the map
 */
DataFileLabelSelectionDelegate::DataFileLabelSelectionDelegate(CaretMappableDataFile* mapFile,
                                                               const int32_t mapIndex)
: CaretObject(),
m_mapFile(mapFile),
m_mapIndex(mapIndex)
{
    CaretAssert(mapFile);
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    m_tabSelectionModels.resize(BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS);
    m_displayGroupSelectionModels.resize(DisplayGroupEnum::NUMBER_OF_GROUPS);
    
    if ( ! mapFile->isMappedWithLabelTable()) {
        const AString txt ("Attempting to create LabelSelectionModel for non-label file: "
                           + m_mapFile->getFileName());
        CaretLogSevere(txt);
        CaretAssertMessage(0, txt);
    }
}

/**
 * Destructor.
 */
DataFileLabelSelectionDelegate::~DataFileLabelSelectionDelegate()
{
}

/**
 * @return Selection model for the given map and tab, NULL if not valid
 * @param displayGroup
 *    Display group selected in the tab
 * @param tabIndex
 *    Index of the tab
 */
LabelSelectionItemModel* 
DataFileLabelSelectionDelegate::getSelectionModelForMapAndTab(const DisplayGroupEnum::Enum displayGroup,
                                                              const int32_t tabIndex)
{
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        if ( ! m_tabSelectionModels[tabIndex]) {
            m_tabSelectionModels[tabIndex].reset(createModel(displayGroup,
                                                                      tabIndex));
        }
        return m_tabSelectionModels[tabIndex].get();
    }
    else {
        const int32_t displayGroupIndex(DisplayGroupEnum::toIntegerCode(displayGroup));
        if ( ! m_displayGroupSelectionModels[displayGroupIndex]) {
            m_displayGroupSelectionModels[displayGroupIndex].reset(createModel(displayGroup,
                                                                               tabIndex));
        }
        return m_displayGroupSelectionModels[displayGroupIndex].get();
    }

    CaretAssert(0);
    return NULL;
}

/**
 * @return A label selection model for the given map, display group, and tab
 * @param displayGroup
 *    Display group selected in the tab
 * @param tabIndex
 *    Index of the tab
 */
LabelSelectionItemModel*
DataFileLabelSelectionDelegate::createModel(const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t tabIndex)
{
    AString mapName(m_mapFile->getMapName(m_mapIndex));
    if (mapName.isEmpty()) {
        mapName = AString::number(m_mapIndex + 1);
    }
    const AString filenameAndMap("File: "
                                 + m_mapFile->getFileNameNoPath()
                                 + " Map: "
                                 + mapName);
    GiftiLabelTable* labelTable(m_mapFile->getMapLabelTable(m_mapIndex));
    CaretAssert(labelTable);
    
    /*
     * One LabelSelectionItemModel is created for each tab and display group.
     * This prevents logging missing labels more than one time.
     */
    const bool logMismatchedLabelsFlag( ! m_modelHasBeenCreatedFlag);
    
    LabelSelectionItemModel* modelOut(new LabelSelectionItemModel(filenameAndMap,
                                                                  labelTable,
                                                                  displayGroup,
                                                                  tabIndex,
                                                                  logMismatchedLabelsFlag));
    CaretAssert(modelOut);
    m_modelHasBeenCreatedFlag = true;

    return modelOut;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
DataFileLabelSelectionDelegate::toString() const
{
    return "DataFileLabelSelectionDelegate";
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
DataFileLabelSelectionDelegate::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DataFileLabelSelectionDelegate",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    const std::vector<int32_t> validTabIndices(sceneAttributes->getIndicesOfTabsForSavingToScene());
    
    {
        SceneObjectMapIntegerKey* tabSceneMap(new SceneObjectMapIntegerKey("TabModels",
                                                                           SceneObjectDataTypeEnum::SCENE_CLASS));
        const int32_t numTabs(validTabIndices.size());
        for (int32_t i = 0; i < numTabs; i++) {
            CaretAssertVectorIndex(validTabIndices, i);
            const int32_t tabIndex(validTabIndices[i]);
            if (m_tabSelectionModels[tabIndex]) {
                const AString tabName("Tab_" + AString::number(tabIndex));
                tabSceneMap->addClass(tabIndex,
                                      m_tabSelectionModels[tabIndex]->saveToScene(sceneAttributes, 
                                                                                  tabName));
            }
        }
        if (tabSceneMap->isEmpty()) {
            delete tabSceneMap;
            tabSceneMap = NULL;
        }
        else {
            sceneClass->addChild(tabSceneMap);
        }
    }
    
    {
        SceneObjectMapIntegerKey* dgSceneMap(new SceneObjectMapIntegerKey("DisplayGroupModels",
                                                                           SceneObjectDataTypeEnum::SCENE_CLASS));
        for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
            if (m_displayGroupSelectionModels[i]) {
                const AString dgName("DisplayGroup_" + AString::number(i));
                dgSceneMap->addClass(i,
                                     m_displayGroupSelectionModels[i]->saveToScene(sceneAttributes, 
                                                                                   dgName));
            }
        }
        if (dgSceneMap->isEmpty()) {
            delete dgSceneMap;
            dgSceneMap = NULL;
        }
        else {
            sceneClass->addChild(dgSceneMap);
        }
    }

    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
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
DataFileLabelSelectionDelegate::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    {
        const SceneObjectMapIntegerKey* tabSceneMap(sceneClass->getMapIntegerKey("TabModels"));
        if (tabSceneMap != NULL) {
            const std::vector<int32_t> tabIndices(tabSceneMap->getKeys());
            for (const int32_t tabIndex : tabIndices) {
                LabelSelectionItemModel* model(getSelectionModelForMapAndTab(DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                                             tabIndex));
                CaretAssert(model);
                model->restoreFromScene(sceneAttributes,
                                        tabSceneMap->classValue(tabIndex));
            }
        }
    }
    
    {
        const SceneObjectMapIntegerKey* dgSceneMap(sceneClass->getMapIntegerKey("DisplayGroupModels"));
        if (dgSceneMap != NULL) {
            const std::vector<int32_t> dgIndices(dgSceneMap->getKeys());
            for (const int32_t dgIndex : dgIndices) {
                bool validFlag(false);
                const DisplayGroupEnum::Enum displayGroup(DisplayGroupEnum::fromIntegerCode(dgIndex,
                                                                                            &validFlag));
                const int32_t invalidTabIndex(-1);
                LabelSelectionItemModel* model(getSelectionModelForMapAndTab(displayGroup,
                                                                             invalidTabIndex));
                CaretAssert(model);
                model->restoreFromScene(sceneAttributes, dgSceneMap->classValue(dgIndex));
            }
        }
    }
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}



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

#define __ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_DECLARE__
#include "AnnotationTextSubstitutionLayerSet.h"
#undef __ANNOTATION_TEXT_SUBSTITUTION_LAYER_SET_DECLARE__

#include "AnnotationFile.h"
#include "AnnotationTextSubstitutionFile.h"
#include "AnnotationTextSubstitutionLayer.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretDataFileSelectionModel.h"
#include "DisplayPropertiesAnnotationTextSubstitution.h"
#include "EventAnnotationTextSubstitutionGet.h"
#include "EventAnnotationTextSubstitutionGetAllGroupIDs.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::AnnotationTextSubstitutionLayerSet 
 * \brief Constains a set of AnnotationSubstitutionLayer items
 * \ingroup Brain
 */

/**
 * Constructor.
 * @param brain
 *    Parent brain
 */
AnnotationTextSubstitutionLayerSet::AnnotationTextSubstitutionLayerSet(Brain* brain)
: CaretObject(),
EventListenerInterface(),
m_brain(brain)
{
    CaretAssert(m_brain);
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    reset();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
}

/**
 * Destructor.
 */
AnnotationTextSubstitutionLayerSet::~AnnotationTextSubstitutionLayerSet()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Override to receive events
 * @param event
 *    The event
 */
void
AnnotationTextSubstitutionLayerSet::receiveEvent(Event* event)
{
    CaretAssert(event);
    if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_TEXT_SUBSTITUTION_GET) {
        EventAnnotationTextSubstitutionGet* textEvent(dynamic_cast<EventAnnotationTextSubstitutionGet*>(event));
        CaretAssert(textEvent);
        
        const int32_t numSubs(textEvent->getNumberOfSubstitutionIDs());
        for (int32_t i = 0; i < numSubs; i++) {
            const AnnotationTextSubstitution& ats = textEvent->getSubstitutionID(i);
            AString groupID(ats.getGroupID());
            
            if (groupID.isEmpty()) {
                /*
                 * Group ID is optional in which case the "no group" identifier is used
                 */
                groupID = AnnotationTextSubstitutionLayer::getNoGroupIdentifierText();
            }
            
            for (const auto& layer : m_layers) {
                if (layer->isSubstitutionsValid()) {
                    if (groupID == layer->getGroupIdentifer()) {
                        const AnnotationTextSubstitutionFile* asf(layer->getSelectedSubstitutionFile());
                        if (asf != NULL) {
                            const AString subsText(asf->getTextSubstitution(ats.getColumnID(),
                                                                            layer->getSubstitutionFileRowIndex()));
                            textEvent->setSubstitutionTextValue(i, subsText);
                        }
                    }
                }
            }
        }
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE) {
        updateContent();
    }
}


/**
 * @return True if any layer is valid for substitutions
 */
bool
AnnotationTextSubstitutionLayerSet::isSubstitutionsValid() const
{
    for (const auto& layer : m_layers) {
        if (layer->isSubstitutionsValid()) {
            return true;
        }
    }
    return false;
}

/**
 * @return Number of layers
 */
int32_t
AnnotationTextSubstitutionLayerSet::getNumberOfLayers() const
{
    return m_layers.size();
}

/**
 * Set the number of layers.  WIll not exceed a maximum.
 * @param numberOfLayers
 */
void
AnnotationTextSubstitutionLayerSet::setNumberOfLayers(const int32_t numberOfLayers)
{
    int32_t numLayers(std::min(numberOfLayers, s_MAXIMUM_NUMBER_OF_LAYERS));
    numLayers = std::max(numLayers, s_MINIMUM_NUMBER_OF_LAYERS);
    
    const int32_t numExistingLayers(m_layers.size());
    if (numLayers > numExistingLayers) {
        const int32_t numToAdd(numLayers - numExistingLayers);
        for (int32_t i = 0; i < numToAdd; i++) {
            m_layers.emplace_back(new AnnotationTextSubstitutionLayer());
        }
    }
    else {
        m_layers.resize(numLayers);
    }
}

/**
 * @return Layer at the given index
 * @param index
 *    Index of the layer
 */
AnnotationTextSubstitutionLayer*
AnnotationTextSubstitutionLayerSet::getLayer(const int32_t index)
{
    CaretAssertVectorIndex(m_layers, index);
    return m_layers[index].get();
}

/**
 * @return Layer at the given index
 * @param index
 *    Index of the layer
 */
const AnnotationTextSubstitutionLayer*
AnnotationTextSubstitutionLayerSet::getLayer(const int32_t index) const
{
    CaretAssertVectorIndex(m_layers, index);
    return m_layers[index].get();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
AnnotationTextSubstitutionLayerSet::toString() const
{
    return "AnnotationTextSubstitutionLayerSet";
}

/**
 * Reset this instance
 */
void
AnnotationTextSubstitutionLayerSet::reset()
{
    const int32_t fixedNumberOfLayers(10);
    setNumberOfLayers(fixedNumberOfLayers);
    
    for (auto& layer : m_layers) {
        layer->setEnabled(false);
    }
}

/**
 * Update contents of this instance
 */
void
AnnotationTextSubstitutionLayerSet::updateContent()
{
    std::vector<AnnotationTextSubstitutionFile*> annTextSubFiles;
    m_brain->getAnnotationTextSubstitutionFiles(annTextSubFiles);
    
    std::vector<AString> groupIDs;
    if ( ! annTextSubFiles.empty()) {
        EventAnnotationTextSubstitutionGetAllGroupIDs idsEvent;
        EventManager::get()->sendEvent(idsEvent.getPointer());
        groupIDs = idsEvent.getAllGroupIDs();
    }

    for (auto& layer : m_layers) {
        layer->updateContent(groupIDs);
    }
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
AnnotationTextSubstitutionLayerSet::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "AnnotationTextSubstitutionLayerSet",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    std::vector<SceneClass*> layerClassVector;
    const int32_t numLayers(getNumberOfLayers());
    for (int i = 0; i < numLayers; i++) {
        layerClassVector.push_back(getLayer(i)->saveToScene(sceneAttributes, 
                                                            ("m_layers[" + AString::number(i) + "]")));
    }
    
    SceneClassArray* layerClassArray = new SceneClassArray("m_layers",
                                                             layerClassVector);
    sceneClass->addChild(layerClassArray);

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
AnnotationTextSubstitutionLayerSet::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const SceneClassArray* layerClassArray = sceneClass->getClassArray("m_layers");
    if (layerClassArray != NULL) {
        const int32_t numLayers = std::min(layerClassArray->getNumberOfArrayElements(),
                                             (int32_t)s_MAXIMUM_NUMBER_OF_LAYERS);
        setNumberOfLayers(numLayers);
        
        for (int32_t i = 0; i < numLayers; i++) {
            m_layers[i]->restoreFromScene(sceneAttributes,
                                            layerClassArray->getClassAtIndex(i));
        }
    }
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * Restore from older scene before annotation text substitution layers
 * @param dispProbAnnSubs
 *    The old display properties
 */
void
AnnotationTextSubstitutionLayerSet::restoreFromOldScene(DisplayPropertiesAnnotationTextSubstitution* dispProbsAnnSubs)
{
    CaretAssert(dispProbsAnnSubs);
    
    reset();
    
    if (dispProbsAnnSubs->isEnableSubstitutions()) {
        AnnotationTextSubstitutionFile* textSubsFile(dispProbsAnnSubs->getSelectedFile());
        if (textSubsFile != NULL) {
            AnnotationTextSubstitutionLayer* layer(getLayer(0));
            layer->setEnabled(true);
            layer->getSubstitutionFileSelectionModel()->setSelectedFile(textSubsFile);
            layer->setMapYokingGroup(textSubsFile->getMapYokingGroup());
            layer->setSubstitutionFileRowIndex(textSubsFile->getSelectedMapIndex());
            layer->setGroupIdentifer(AnnotationTextSubstitutionLayer::getNoGroupIdentifierText());
        }
    }
}

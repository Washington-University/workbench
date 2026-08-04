
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __NEUROGLANCER_ANNOTATION_MODEL_DECLARE__
#include "NeuroglancerAnnotationModel.h"
#undef __NEUROGLANCER_ANNOTATION_MODEL_DECLARE__

#include "CaretAssert.h"
#include "EventManager.h"
#include "NeuroglancerAnnotation.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapStringKey.h"

using namespace caret;


    
/**
 * \class caret::NeuroglancerAnnotationModel 
 * \brief Model that contains NeuroglancerAnnotations
 * \ingroup Files
 */

/**
 * Constructor.
 */
NeuroglancerAnnotationModel::NeuroglancerAnnotationModel()
: QStandardItemModel()
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
NeuroglancerAnnotationModel::~NeuroglancerAnnotationModel()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
NeuroglancerAnnotationModel::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}

/**
 * Add an annotation and its properties
 * @param annotationAndProperties
 *    The annotation is first and then the properties
 */
void
NeuroglancerAnnotationModel::addAnnotation(const AString& annotationFilename,
                                           const QList<QStandardItem*>& annotationAndProperties)
{
    m_filenameToRowMap.insert(std::make_pair(annotationFilename,
                                             rowCount()));
    appendRow(annotationAndProperties);
}

/**
 * @return Number of annotations
 */
int32_t
NeuroglancerAnnotationModel::getNumberOfAnnotations() const
{
    return rowCount();
}

/**
 * @return Annotation at the given index or NULL if not found
 * @param index
 *    Index of annotation
 * @return
 *    Annotation at index or NULL if not found
 */
NeuroglancerAnnotation*
NeuroglancerAnnotationModel::getAnnotationAtIndex(const int32_t index)
{
    const int32_t column(0);
    CaretAssert((index >= 0)
                && (index < rowCount()));
    QStandardItem* standardItem(item(index, column));
    CaretAssert(standardItem);
    NeuroglancerAnnotation* neuroAnn(dynamic_cast<NeuroglancerAnnotation*>(standardItem));
    CaretAssert(neuroAnn);
    return neuroAnn;
}

/**
 * @return Annotation at the given index or NULL if not found
 * @param index
 *    Index of annotation
 * @return
 *    Annotation at index or NULL if not found
 */
const NeuroglancerAnnotation*
NeuroglancerAnnotationModel::getAnnotationAtIndex(const int32_t index) const
{
    const int32_t column(0);
    CaretAssert((index >= 0)
                && (index < rowCount()));
    const QStandardItem* standardItem(item(index, column));
    CaretAssert(standardItem);
    const NeuroglancerAnnotation* neuroAnn(dynamic_cast<const NeuroglancerAnnotation*>(standardItem));
    CaretAssert(neuroAnn);
    return neuroAnn;
}

/**
 * @return Annotation at the given filename or NULL if not found
 * @param fileName
 *    Name of file
 * @return
 *    Annotation with filename or NULL if not found
 */
NeuroglancerAnnotation*
NeuroglancerAnnotationModel::getAnnotationWithFileName(const AString& fileName)
{
    NeuroglancerAnnotation* ann(NULL);
    const auto iter(m_filenameToRowMap.find(fileName));
    if (iter != m_filenameToRowMap.end()) {
        const int32_t rowIndex(iter->second);
        ann = getAnnotationAtIndex(rowIndex);
    }
    return ann;
}


/**
 * Set display status of all annotations in this file
 * @param displayStatus
 *    If true, display all.
 */
void
NeuroglancerAnnotationModel::setAllAnnotationsDisplayed(const bool displayStatus)
{
    const int32_t column(0);
    const Qt::CheckState checkState(displayStatus
                                    ? Qt::Checked
                                    : Qt::Unchecked);
    const int32_t numAnn(rowCount());
    for (int32_t i = 0; i < numAnn; i++) {
        item(i, column)->setCheckState(checkState);
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
NeuroglancerAnnotationModel::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "NeuroglancerAnnotationModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    SceneObjectMapStringKey* annMap(new SceneObjectMapStringKey("annotationsMap",
                                                                SceneObjectDataTypeEnum::SCENE_CLASS));
    const int32_t numAnn(getNumberOfAnnotations());
    for (int32_t i = 0; i < numAnn; i++) {
        NeuroglancerAnnotation* ann(getAnnotationAtIndex(i));
        const AString className("NeuroAnn_"
                                + AString::number(i));
        annMap->addClass(ann->getFileName(), ann->saveToScene(sceneAttributes,
                                                              className));
    }
    
    sceneClass->addChild(annMap);
    
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
NeuroglancerAnnotationModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    const SceneObjectMapStringKey* annMap = sceneClass->getMapStringKey("annotationsMap");
    if (annMap != NULL) {
        const std::vector<AString> allKeys(annMap->getKeys());
        for (const AString& key : allKeys) {
            const SceneClass* annClass(annMap->classValue(key));
            if (annClass != NULL) {
                NeuroglancerAnnotation* neuroAnn(getAnnotationWithFileName(key));
                if (neuroAnn != NULL) {
                    neuroAnn->restoreFromScene(sceneAttributes,
                                               annClass);
                }
            }
        }
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}


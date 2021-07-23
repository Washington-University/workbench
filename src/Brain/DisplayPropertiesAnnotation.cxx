
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __DISPLAY_PROPERTIES_ANNOTATION_DECLARE__
#include "DisplayPropertiesAnnotation.h"
#undef __DISPLAY_PROPERTIES_ANNOTATION_DECLARE__

#include "Annotation.h"
#include "AnnotationManager.h"
#include "Brain.h"
#include "CaretLogger.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "SceneClassAssistant.h"
#include "SceneObjectMapIntegerKey.h"

using namespace caret;



/**
 * \class caret::DisplayPropertiesAnnotation
 * \brief Contains display properties for annotations.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
DisplayPropertiesAnnotation::DisplayPropertiesAnnotation(Brain* parentBrain)
: DisplayProperties(),
m_parentBrain(parentBrain)
{
    CaretAssert(parentBrain);
    
    resetPrivate();
    
    m_sceneAssistant->add("m_displayAnnotations",
                          &m_displayAnnotations);
    
    m_sceneAssistant->add("m_displayTextAnnotations",
                          &m_displayTextAnnotations);
    
    m_sceneAssistant->addTabIndexedEnumeratedTypeArray<DisplayGroupEnum,DisplayGroupEnum::Enum>("m_displayGroup",
                                                                                                m_displayGroup);
    m_sceneAssistant->addArray("m_displayWindowAnnotationsInSingleTabViews",
                               m_displayWindowAnnotationsInSingleTabViews,
                               BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS,
                               true);
}

/**
 * Destructor.
 */
DisplayPropertiesAnnotation::~DisplayPropertiesAnnotation()
{
}

/**
 * Update the properties for a new/modified annotation.
 *
 * @param annotation
 *     The new/updated annotation.
 */
void
DisplayPropertiesAnnotation::updateForNewAnnotation(const Annotation* annotation)
{
    CaretAssert(annotation);
    
    switch (annotation->getCoordinateSpace()) {
        case AnnotationCoordinateSpaceEnum::CHART:
            break;
        case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
            break;
        case AnnotationCoordinateSpaceEnum::SPACER:
            break;
        case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            break;
        case AnnotationCoordinateSpaceEnum::SURFACE:
            break;
        case AnnotationCoordinateSpaceEnum::TAB:
            break;
        case AnnotationCoordinateSpaceEnum::VIEWPORT:
            break;
        case AnnotationCoordinateSpaceEnum::WINDOW:
            setDisplayWindowAnnotationsInSingleTabViews(annotation->getWindowIndex(),
                                                        true);
            break;
    }
    
    setDisplayAnnotations(true);
}

/**
 * Copy the border display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void
DisplayPropertiesAnnotation::copyDisplayProperties(const int32_t sourceTabIndex,
                                                   const int32_t targetTabIndex)
{
    const DisplayGroupEnum::Enum displayGroup = this->getDisplayGroupForTab(sourceTabIndex);
    this->setDisplayGroupForTab(targetTabIndex, displayGroup);
}

/**
 * Reset all settings to default.
 * NOTE: reset() is virtual so can/should not be called from constructor.
 */
void
DisplayPropertiesAnnotation::resetPrivate()
{
    m_displayAnnotations = true;
    m_displayTextAnnotations = true;
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS; i++) {
        m_displayWindowAnnotationsInSingleTabViews[i] = true;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    }
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void
DisplayPropertiesAnnotation::reset()
{
    resetPrivate();
}

/**
 * Update due to changes in data.
 */
void
DisplayPropertiesAnnotation::update()
{
    
}

/**
 * @return Status for displaying annotations
 */
bool
DisplayPropertiesAnnotation::isDisplayAnnotations() const
{
    return m_displayAnnotations;
}

/**
 * Set the display status for annotations
 *
 * @param status
 *     New display status.
 */
void
DisplayPropertiesAnnotation::setDisplayAnnotations(const bool status)
{
    m_displayAnnotations = status;
}

/**
 * @return Status for displaying text annotations
 */
bool
DisplayPropertiesAnnotation::isDisplayTextAnnotations() const
{
    return m_displayTextAnnotations;
}

/**
 * Set the display status for text annotations
 *
 * @param status
 *     New display status.
 */
void
DisplayPropertiesAnnotation::setDisplayTextAnnotations(const bool status)
{
    m_displayTextAnnotations = status;
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum
DisplayPropertiesAnnotation::getDisplayGroupForTab(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(this->displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return m_displayGroup[browserTabIndex];
}

/**
 * Set the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayGroup
 *    New value for display group.
 */
void
DisplayPropertiesAnnotation::setDisplayGroupForTab(const int32_t browserTabIndex,
                                                const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}

/**
 * Is the window annotation displayed in the given window index?
 *
 * @param windowIndex
 *     Index of the window.
 * @return
 * True if displayed, else false.
 */
bool
DisplayPropertiesAnnotation::isDisplayWindowAnnotationsInSingleTabViews(const int32_t windowIndex) const
{
    CaretAssertArrayIndex(m_displayWindowAnnotationsInSingleTabViews, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    return m_displayWindowAnnotationsInSingleTabViews[windowIndex];
}

/**
 * Set the window annotation displayed in the given window index.
 *
 * @param windowIndex
 *     Index of the tab.
 * @param status
 *     True if displayed, else false.
 */
void
DisplayPropertiesAnnotation::setDisplayWindowAnnotationsInSingleTabViews(const int32_t windowIndex,
                                                         const bool status)
{
    CaretAssertArrayIndex(m_displayWindowAnnotationsInSingleTabViews, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS, windowIndex);
    m_displayWindowAnnotationsInSingleTabViews[windowIndex] = status;
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @return Pointer to SceneClass object representing the state of
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass*
DisplayPropertiesAnnotation::saveToScene(const SceneAttributes* sceneAttributes,
                                         const AString& instanceName)
{
    const std::vector<int32_t> tabIndices = sceneAttributes->getIndicesOfTabsForSavingToScene();
    
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesAnnotation",
                                            2);
    
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
    return sceneClass;
}

/**
 * Restore the state of an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
DisplayPropertiesAnnotation::restoreFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);

    if (sceneClass->getVersionNumber() <= 1) {
        restoreVersionOne(sceneClass);
    }
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
}

/**
 * Restore version one that contains a type display for each tab
 * (before the file->group->annotation hierarchy).
 *
 * @param sceneClass
 *     SceneClass containing the state that was previously
 *     saved and should be restored.
 */
void
DisplayPropertiesAnnotation::restoreVersionOne(const SceneClass* sceneClass)
{
    m_displayTextAnnotations = true;
    
    /*
     * Version one did not have display groups so default the display group
     * in each to to "Tab".
     */
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::DISPLAY_GROUP_TAB;
    }
    
    /*
     * Default version one selections to "on"
     */
    bool stereoStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    bool surfaceStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    bool tabStatusInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        stereoStatusInTab[i]  = true;
        surfaceStatusInTab[i] = true;
        tabStatusInTab[i]     = true;
    }
    
    /*
     * Look for version one selections in the scene
     */
    const SceneObjectMapIntegerKey* stereoAnnDisplay = sceneClass->getMapIntegerKey("m_displayModelAnnotations");
    if (stereoAnnDisplay != NULL) {
        const std::vector<int32_t> mapKeys = stereoAnnDisplay->getKeys();
        for (std::vector<int32_t>::const_iterator keyIter = mapKeys.begin();
             keyIter != mapKeys.end();
             keyIter++) {
            stereoStatusInTab[*keyIter] = stereoAnnDisplay->booleanValue(*keyIter);
        }
    }
    
    const SceneObjectMapIntegerKey* surfaceAnnDisplay = sceneClass->getMapIntegerKey("m_displaySurfaceAnnotations");
    if (surfaceAnnDisplay != NULL) {
        const std::vector<int32_t> mapKeys = surfaceAnnDisplay->getKeys();
        for (std::vector<int32_t>::const_iterator keyIter = mapKeys.begin();
             keyIter != mapKeys.end();
             keyIter++) {
            surfaceStatusInTab[*keyIter] = surfaceAnnDisplay->booleanValue(*keyIter);
        }
    }
    
    const SceneObjectMapIntegerKey* tabAnnDisplay = sceneClass->getMapIntegerKey("m_displayTabAnnotations");
    if (tabAnnDisplay != NULL) {
        const std::vector<int32_t> mapKeys = tabAnnDisplay->getKeys();
        for (std::vector<int32_t>::const_iterator keyIter = mapKeys.begin();
             keyIter != mapKeys.end();
             keyIter++) {
            tabStatusInTab[*keyIter] = tabAnnDisplay->booleanValue(*keyIter);
        }
    }
    
    bool windowStatus[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS];
    const int32_t numWindowStatus = sceneClass->getBooleanArrayValue("m_displayWindowAnnotations",
                                                                     windowStatus,
                                                                     BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS);
    
    /*
     * Apply version one selections to the annotations.
     */
    std::vector<Annotation*> allAnnotations = m_parentBrain->getAnnotationManager()->getAllAnnotations();
    for (std::vector<Annotation*>::iterator annIter = allAnnotations.begin();
         annIter != allAnnotations.end();
         annIter++) {
        Annotation* ann = *annIter;
        switch (ann->getCoordinateSpace()) {
            case AnnotationCoordinateSpaceEnum::CHART:
                CaretAssertMessage(0, "This should never happen as CHART SPACE was never available in a version one scene");
                break;
            case AnnotationCoordinateSpaceEnum::MEDIA_FILE_NAME_AND_PIXEL:
                CaretAssertMessage(0, "This should never happen as MEDIA SPACE was never available in a version one scene");
                break;
            case AnnotationCoordinateSpaceEnum::SPACER:
                CaretAssertMessage(0, "This should never happen as SPACER TAB SPACE was never available in a version one scene");
                break;
            case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
            if (stereoAnnDisplay != NULL) {
                for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
                    TriStateSelectionStatusEnum::Enum triStatus = TriStateSelectionStatusEnum::UNSELECTED;
                    if (stereoStatusInTab[iTab]) {
                        triStatus = TriStateSelectionStatusEnum::SELECTED;
                    }
                    ann->setItemDisplaySelected(DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                iTab,
                                                triStatus);
                }
            }
                break;
            case AnnotationCoordinateSpaceEnum::SURFACE:
            if (surfaceAnnDisplay != NULL) {
                for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
                    TriStateSelectionStatusEnum::Enum triStatus = TriStateSelectionStatusEnum::UNSELECTED;
                    if (surfaceStatusInTab[iTab]) {
                        triStatus = TriStateSelectionStatusEnum::SELECTED;
                    }
                    ann->setItemDisplaySelected(DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                iTab,
                                                triStatus);
                }
            }
                break;
            case AnnotationCoordinateSpaceEnum::TAB:
            if (tabAnnDisplay != NULL) {
                const int32_t tabIndex = ann->getTabIndex();
                if ((tabIndex >= 0)
                    && (tabIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS)) {
                    TriStateSelectionStatusEnum::Enum triStatus = TriStateSelectionStatusEnum::UNSELECTED;
                    if (tabStatusInTab[tabIndex]) {
                        triStatus = TriStateSelectionStatusEnum::SELECTED;
                    }
                    ann->setItemDisplaySelected(DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                tabIndex,
                                                triStatus);
                }
            }
                break;
            case AnnotationCoordinateSpaceEnum::VIEWPORT:
                /*
                 * Ignore annotations in viewport space.  These are usually text annotations
                 * in the graphics region such as chart labels.
                 */
                break;
            case AnnotationCoordinateSpaceEnum::WINDOW:
            if (numWindowStatus) {
                const int32_t windowIndex = ann->getWindowIndex();
                if ((windowIndex >= 0)
                    && (windowIndex < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_WINDOWS)) {
                    TriStateSelectionStatusEnum::Enum triStatus = TriStateSelectionStatusEnum::UNSELECTED;
                    if (windowStatus[windowIndex]) {
                        triStatus = TriStateSelectionStatusEnum::SELECTED;
                    }
                    /*
                     * Note: For window annotations, the display group
                     * and tab are ignored.  Window status is set using
                     * the window index contained in the annotation.
                     */
                    const int32_t tabIndex = 0;
                    ann->setItemDisplaySelected(DisplayGroupEnum::DISPLAY_GROUP_TAB,
                                                tabIndex,
                                                triStatus);
                }
            }
                break;
        }
    }
}




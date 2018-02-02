
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

#include <algorithm>

#define __VOLUME_SURFACE_OUTLINE_SET_MODEL_DECLARE__
#include "VolumeSurfaceOutlineSetModel.h"
#undef __VOLUME_SURFACE_OUTLINE_SET_MODEL_DECLARE__

#include "Brain.h"
#include "BrainStructure.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventBrowserTabGetAll.h"
#include "EventManager.h"
#include "ModelSurface.h"
#include "SceneClassAssistant.h"
#include "SceneClass.h"
#include "SceneClassArray.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"
#include "VolumeSurfaceOutlineColorOrTabModel.h"
#include "VolumeSurfaceOutlineModel.h"

#include "SceneableInterface.h"

using namespace caret;


    
/**
 * \class caret::VolumeSurfaceOutlineSetModel 
 * \brief Holds a set of VolumeSurfaceOutlineModels
 *
 * Holds a set of VolumeSurfaceOutlineModels.  Users 
 * may add additional surface outline models up to 
 * a fixed limit.  There is also a minimum number
 * that are displayed.
 */
/**
 * Constructor.
 */
VolumeSurfaceOutlineSetModel::VolumeSurfaceOutlineSetModel()
: CaretObject()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        m_outlineModels[i] = new VolumeSurfaceOutlineModel();
    }
    m_numberOfDisplayedVolumeSurfaceOutlines = 6; 
    
    m_sceneAssistant = new SceneClassAssistant();
    m_sceneAssistant->add("m_numberOfDisplayedVolumeSurfaceOutlines",
                          &m_numberOfDisplayedVolumeSurfaceOutlines);
}

/**
 * Destructor.
 */
VolumeSurfaceOutlineSetModel::~VolumeSurfaceOutlineSetModel()
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        delete m_outlineModels[i];
    }
    
    delete m_sceneAssistant;
}

/**
 * @return The number of volume surface outlines.
 */
int32_t 
VolumeSurfaceOutlineSetModel::getNumberOfDislayedVolumeSurfaceOutlines() const
{
    return m_numberOfDisplayedVolumeSurfaceOutlines;
}

/**
 * Copy the given volume surface outline set model.
 * @param setModel
 *     Model that is copied.
 */
void 
VolumeSurfaceOutlineSetModel::copyVolumeSurfaceOutlineSetModel(VolumeSurfaceOutlineSetModel* setModel)
{
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        m_outlineModels[i]->copyVolumeSurfaceOutlineModel(setModel->getVolumeSurfaceOutlineModel(i));
    }
    m_numberOfDisplayedVolumeSurfaceOutlines = setModel->getNumberOfDislayedVolumeSurfaceOutlines();
}

/**
 * Set the number of volume surface outlines.
 * @param numberDisplayed
 *    Number of displayed volume surface outlines.
 */
void 
VolumeSurfaceOutlineSetModel::setNumberOfDisplayedVolumeSurfaceOutlines(const int32_t numberDisplayed)
{
    m_numberOfDisplayedVolumeSurfaceOutlines = numberDisplayed;
    
    if (m_numberOfDisplayedVolumeSurfaceOutlines <
        BrainConstants::MINIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES) {
        m_numberOfDisplayedVolumeSurfaceOutlines = BrainConstants::MINIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES;
    }
    
    if (m_numberOfDisplayedVolumeSurfaceOutlines >
        BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES) {
        m_numberOfDisplayedVolumeSurfaceOutlines = BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES;
    }
}

/**
 * @return The volume surface outline model at the given index.
 * @param indx
 *    Index of volume surface outline model.
 */
VolumeSurfaceOutlineModel* 
VolumeSurfaceOutlineSetModel::getVolumeSurfaceOutlineModel(const int32_t indx)
{
    CaretAssertArrayIndex(m_outlineModels, 
                          BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES, 
                          indx);
    return m_outlineModels[indx];
}

/**
 * @return The volume surface outline model at the given index.
 * @param indx
 *    Index of volume surface outline model.
 */
const VolumeSurfaceOutlineModel* 
VolumeSurfaceOutlineSetModel::getVolumeSurfaceOutlineModel(const int32_t indx) const
{
    CaretAssertArrayIndex(m_outlineModels, 
                          BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES, 
                          indx);
    return m_outlineModels[indx];
}

/**
 * Set the default selected surfaces after a spec file is loaded.
 * @searchForTabs
 *   If true, examine the loaded tabs to find left and right surfaces.
 */ 
void 
VolumeSurfaceOutlineSetModel::selectSurfacesAfterSpecFileLoaded(Brain* brain,
                                                                const bool searchForTabs)
{
    EventBrowserTabGetAll getAllTabs;
    EventManager::get()->sendEvent(getAllTabs.getPointer());
    
    /*
     * Find tabs with left/right
     */
    int32_t leftTabIndex  = -1;
    int32_t rightTabIndex = -1;
    const int numTabs = getAllTabs.getNumberOfBrowserTabs();
    if (searchForTabs) {
        for (int32_t i = 0; i < numTabs; i++) {
            BrowserTabContent* tabContent = getAllTabs.getBrowserTab(i);
            ModelSurface* surfaceModel = tabContent->getDisplayedSurfaceModel();
            if (surfaceModel != NULL) {
                const StructureEnum::Enum structure = surfaceModel->getSurface()->getStructure();
                switch (structure) {
                    case StructureEnum::CORTEX_LEFT:
                        leftTabIndex = tabContent->getTabNumber();
                        break;
                    case StructureEnum::CORTEX_RIGHT:
                        rightTabIndex = tabContent->getTabNumber();
                        break;
                    default:
                        break;
                }
            }
        }
    }
    else {
        if (numTabs >= 1) {
            leftTabIndex = 0;
        }
        if (numTabs >= 2) {
            rightTabIndex = 1;
        }
    }
    
    Surface* leftMidThickSurface = NULL;
    Surface* leftWhiteSurface    = NULL;
    Surface* leftPialSurface     = NULL;
    BrainStructure* leftBrainStructure = brain->getBrainStructure(StructureEnum::CORTEX_LEFT, 
                                                                             false);
    if (leftBrainStructure != NULL) {
        leftMidThickSurface = leftBrainStructure->getSurfaceContainingTextInName("midthick");
        if (leftMidThickSurface == NULL) {
            leftMidThickSurface = leftBrainStructure->getPrimaryAnatomicalSurface();
        }
        
        leftWhiteSurface    = leftBrainStructure->getSurfaceContainingTextInName("white");
        leftPialSurface     = leftBrainStructure->getSurfaceContainingTextInName("pial");
    }
    
    Surface* rightMidThickSurface = NULL;
    Surface* rightWhiteSurface    = NULL;
    Surface* rightPialSurface     = NULL;
    BrainStructure* rightBrainStructure = brain->getBrainStructure(StructureEnum::CORTEX_RIGHT, 
                                                                              false);
    if (rightBrainStructure != NULL) {
        rightMidThickSurface = rightBrainStructure->getSurfaceContainingTextInName("midthick");
        if (rightMidThickSurface == NULL) {
            rightMidThickSurface = rightBrainStructure->getPrimaryAnatomicalSurface();
        }
        
        rightWhiteSurface    = rightBrainStructure->getSurfaceContainingTextInName("white");
        rightPialSurface     = rightBrainStructure->getSurfaceContainingTextInName("pial");
    }
    
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++) {
        m_outlineModels[i]->getColorOrTabModel()->setColor(CaretColorEnum::BLACK);
        m_outlineModels[i]->setThicknessPixelsObsolete(VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PIXELS_OBSOLETE);
    }
    
    int nextOutlineIndex = 0;
    
    addSurfaceOutline(leftMidThickSurface,
                      leftTabIndex,
                      CaretColorEnum::BLACK,
                      nextOutlineIndex);
    addSurfaceOutline(rightMidThickSurface,
                      rightTabIndex,
                      CaretColorEnum::BLACK,
                      nextOutlineIndex);
    
    addSurfaceOutline(leftWhiteSurface,
                      -1,
                      CaretColorEnum::LIME,
                      nextOutlineIndex);
    addSurfaceOutline(rightWhiteSurface,
                      -1,
                      CaretColorEnum::LIME,
                      nextOutlineIndex);
    
    addSurfaceOutline(leftPialSurface,
                      -1,
                      CaretColorEnum::BLUE,
                      nextOutlineIndex);
    addSurfaceOutline(rightPialSurface,
                      -1,
                      CaretColorEnum::BLUE,
                      nextOutlineIndex);
}

/**
 * Add a surface outline at the given outlineIndex.  The
 * outlineIndex is incremented.
 *
 * @param surface
 *    Surface that is added.  If NULL, no action is taken.
 * @param browserTabIndex
 *    If greater than or equal to zero, the color source
 *    is set to this tab index.
 * @param color
 *    If browserTabIndex is less than zero, the color source
 *    is set to this color.
 * @param outlineIndex
 *    If an outline was added, it is placed at this value
 *    and it is incremented.  If this index is greater 
 *    than or equal to the number of available surface 
 *    outlines, no action is taken.
 */
void 
VolumeSurfaceOutlineSetModel::addSurfaceOutline(Surface* surface,
                                           const int32_t browserTabIndex,
                                           const CaretColorEnum::Enum color,
                                           int32_t& outlineIndex)
{
    if (surface != NULL) {
        if (surface->getSurfaceType() == SurfaceTypeEnum::ANATOMICAL) {
            if (outlineIndex < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES) {
                VolumeSurfaceOutlineModel* vsos = m_outlineModels[outlineIndex];
                vsos->getSurfaceSelectionModel()->setSurface(surface);
                vsos->setThicknessPixelsObsolete(VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PIXELS_OBSOLETE);
                vsos->setThicknessPercentageViewportHeight(VolumeSurfaceOutlineModel::DEFAULT_LINE_THICKNESS_PERCENTAGE_VIEWPORT_HEIGHT);
                if (browserTabIndex >= 0) {
                    vsos->getColorOrTabModel()->setBrowserTabIndex(browserTabIndex);
                }
                else {
                    vsos->getColorOrTabModel()->setColor(color);
                }
                
                outlineIndex++;
            }
        }
    }
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
VolumeSurfaceOutlineSetModel::saveToScene(const SceneAttributes* sceneAttributes,
                                      const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "VolumeSurfaceOutlineSetModel",
                                            2);
    m_sceneAssistant->saveMembers(sceneAttributes, 
                                  sceneClass);
    
    std::vector<SceneClass*> outlineModelSceneClassVector;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES; i++ ) {
        outlineModelSceneClassVector.push_back(m_outlineModels[i]->saveToScene(sceneAttributes, 
                                                                               ("m_outlineModels[" + AString::number(i) + "]")));
    }
    sceneClass->addChild(new SceneClassArray("m_outlineModels",
                                             outlineModelSceneClassVector));
    
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
VolumeSurfaceOutlineSetModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                           const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const int32_t version = sceneClass->getVersionNumber();
    
    m_sceneAssistant->restoreMembers(sceneAttributes, 
                                     sceneClass);
    
    const SceneClassArray* outlineModelsArrayClass = sceneClass->getClassArray("m_outlineModels");
    if (outlineModelsArrayClass != NULL) {
        const int32_t maxNum = std::min(outlineModelsArrayClass->getNumberOfArrayElements(),
                                        (int32_t)BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES);
        for (int32_t i = 0; i < maxNum; i++) {
            m_outlineModels[i]->restoreFromScene(sceneAttributes,
                                                 outlineModelsArrayClass->getClassAtIndex(i));
        }
        
        /*
         * In older scenes, last was drawn on top.
         * Now, zero is drawn on top so need to reverse
         * the order of the volume surface outlines
         */
        if (version < 2) {
            if (maxNum > 1) {
                const int32_t numDisplayed = getNumberOfDislayedVolumeSurfaceOutlines();
                CaretAssertArrayIndex(m_outlineModels, BrainConstants::MAXIMUM_NUMBER_OF_VOLUME_SURFACE_OUTLINES, numDisplayed - 1);
                std::reverse(m_outlineModels,
                             m_outlineModels + numDisplayed);
            }
        }
    }
}


/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#define __SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_DECLARE__
#include "SurfaceMontageConfigurationCerebral.h"
#undef __SURFACE_MONTAGE_CONFIGURATION_CEREBRAL_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "EventBrainStructureGetAll.h"
#include "EventManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SurfaceSelectionModel.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageConfigurationCerebral 
 * \brief Surface montage cerebral cortext configuration.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SurfaceMontageConfigurationCerebral::SurfaceMontageConfigurationCerebral()
: SurfaceMontageConfigurationAbstract(SurfaceMontageConfigurationTypeEnum::CEREBRAL_CORTEX_CONFIGURATION,
                                      SUPPORTS_LAYOUT_ORIENTATION_YES)
{
    std::vector<SurfaceTypeEnum::Enum> validSurfaceTypes;
    validSurfaceTypes.push_back(SurfaceTypeEnum::ANATOMICAL);
    validSurfaceTypes.push_back(SurfaceTypeEnum::RECONSTRUCTION);
    validSurfaceTypes.push_back(SurfaceTypeEnum::INFLATED);
    validSurfaceTypes.push_back(SurfaceTypeEnum::VERY_INFLATED);
    
    m_leftFirstSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT,
                                                                 validSurfaceTypes);
    m_leftSecondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT,
                                                                  validSurfaceTypes);
    m_rightFirstSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT,
                                                                  validSurfaceTypes);
    m_rightSecondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT,
                                                                   validSurfaceTypes);
    m_leftEnabled = true;
    m_rightEnabled = true;
    m_firstSurfaceEnabled = true;
    m_secondSurfaceEnabled = false;
    m_lateralEnabled = true;
    m_medialEnabled  = true;
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("m_leftFirstSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_leftFirstSurfaceSelectionModel);
    m_sceneAssistant->add("m_leftSecondSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_leftSecondSurfaceSelectionModel);
    m_sceneAssistant->add("m_rightFirstSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_rightFirstSurfaceSelectionModel);
    m_sceneAssistant->add("m_rightSecondSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_rightSecondSurfaceSelectionModel);
    
    m_sceneAssistant->add("m_leftEnabled",
                          &m_leftEnabled);
    m_sceneAssistant->add("m_rightEnabled",
                          &m_rightEnabled);
    m_sceneAssistant->add("m_firstSurfaceEnabled",
                          &m_firstSurfaceEnabled);
    m_sceneAssistant->add("m_secondSurfaceEnabled",
                          &m_secondSurfaceEnabled);
    m_sceneAssistant->add("m_lateralEnabled",
                          &m_lateralEnabled);
    m_sceneAssistant->add("m_medialEnabled",
                          &m_medialEnabled);

    std::vector<StructureEnum::Enum> supportedStructures;
    supportedStructures.push_back(StructureEnum::CORTEX_LEFT);
    supportedStructures.push_back(StructureEnum::CORTEX_RIGHT);
    setupOverlaySet(supportedStructures);
}

/**
 * Destructor.
 */
SurfaceMontageConfigurationCerebral::~SurfaceMontageConfigurationCerebral()
{
    delete m_leftFirstSurfaceSelectionModel;
    delete m_leftSecondSurfaceSelectionModel;
    delete m_rightFirstSurfaceSelectionModel;
    delete m_rightSecondSurfaceSelectionModel;
    
    delete m_sceneAssistant;
}

/**
 * Initialize the selected surfaces.
 */
void
SurfaceMontageConfigurationCerebral::initializeSelectedSurfaces()
{
    EventBrainStructureGetAll brainStructureEvent;
    EventManager::get()->sendEvent(brainStructureEvent.getPointer());
    
    Surface* leftAnatSurface = NULL;
    BrainStructure* leftBrainStructure = brainStructureEvent.getBrainStructureByStructure(StructureEnum::CORTEX_LEFT);
    if (leftBrainStructure != NULL) {
        leftAnatSurface = leftBrainStructure->getVolumeInteractionSurface();
    }
    
    Surface* rightAnatSurface = NULL;
    BrainStructure* rightBrainStructure = brainStructureEvent.getBrainStructureByStructure(StructureEnum::CORTEX_RIGHT);
    if (rightBrainStructure != NULL) {
        rightAnatSurface = rightBrainStructure->getVolumeInteractionSurface();
    }
    
    m_leftFirstSurfaceSelectionModel->setSurfaceToType(SurfaceTypeEnum::ANATOMICAL);
    if (leftAnatSurface != NULL) {
        m_leftFirstSurfaceSelectionModel->setSurface(leftAnatSurface);
    }
    
    
    m_leftSecondSurfaceSelectionModel->setSurfaceToType(SurfaceTypeEnum::INFLATED,
                                                           SurfaceTypeEnum::VERY_INFLATED);
    
    m_rightFirstSurfaceSelectionModel->setSurfaceToType(SurfaceTypeEnum::ANATOMICAL);
    if (rightAnatSurface != NULL) {
        m_rightFirstSurfaceSelectionModel->setSurface(rightAnatSurface);
    }
    
    m_rightSecondSurfaceSelectionModel->setSurfaceToType(SurfaceTypeEnum::INFLATED,
                                                            SurfaceTypeEnum::VERY_INFLATED);
}

/**
 * @return Is this configuration valid?
 */
bool
SurfaceMontageConfigurationCerebral::isValid()
{
    const bool valid = ((getLeftFirstSurfaceSelectionModel()->getSurface() != NULL)
                        || (getRightFirstSurfaceSelectionModel()->getSurface() != NULL));
    return valid;
}

/**
 * Get the surface montage viewports for the current configuration.
 *
 * @param surfaceMontageViewports
 *    Output of surface montage viewports for drawing.
 */
void
SurfaceMontageConfigurationCerebral::getSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports)
{
    surfaceMontageViewports.clear();
    
    std::vector<SurfaceMontageViewport> leftLateralViewports;
    std::vector<SurfaceMontageViewport> leftMedialViewports;
    std::vector<SurfaceMontageViewport> rightLateralViewports;
    std::vector<SurfaceMontageViewport> rightMedialViewports;
    
    if (m_leftEnabled) {
        if (m_firstSurfaceEnabled) {
            Surface* leftSurface = m_leftFirstSurfaceSelectionModel->getSurface();
            if (leftSurface != NULL) {
                if (m_lateralEnabled) {
                    SurfaceMontageViewport smv(leftSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
                    leftLateralViewports.push_back(smv);
                }
                if (m_medialEnabled) {
                    SurfaceMontageViewport smv(leftSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL);
                    leftMedialViewports.push_back(smv);
                }
            }
        }
        if (m_secondSurfaceEnabled) {
            Surface* leftSurface = m_leftSecondSurfaceSelectionModel->getSurface();
            if (leftSurface != NULL) {
                if (m_lateralEnabled) {
                    SurfaceMontageViewport smv(leftSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_LATERAL);
                    leftLateralViewports.push_back(smv);
                }
                if (m_medialEnabled) {
                    SurfaceMontageViewport smv(leftSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_MEDIAL);
                    leftMedialViewports.push_back(smv);
                }
            }
        }
    }
    
    if (m_rightEnabled) {
        if (m_firstSurfaceEnabled) {
            Surface* rightSurface = m_rightFirstSurfaceSelectionModel->getSurface();
            if (rightSurface != NULL) {
                if (m_lateralEnabled) {
                    SurfaceMontageViewport smv(rightSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL);
                    rightLateralViewports.push_back(smv);
                }
                if (m_medialEnabled) {
                    SurfaceMontageViewport smv(rightSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL);
                    rightMedialViewports.push_back(smv);
                }
            }
        }
    }
    if (m_rightEnabled) {
        if (m_secondSurfaceEnabled) {
            Surface* rightSurface = m_rightSecondSurfaceSelectionModel->getSurface();
            if (rightSurface != NULL) {
                if (m_lateralEnabled) {
                    SurfaceMontageViewport smv(rightSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_LATERAL);
                    rightLateralViewports.push_back(smv);
                }
                if (m_medialEnabled) {
                    SurfaceMontageViewport smv(rightSurface,
                                               ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_MEDIAL);
                    rightMedialViewports.push_back(smv);
                }
            }
        }
    }

    std::vector<SurfaceMontageViewport> leftViewports;
    leftViewports.insert(leftViewports.end(),
                         leftLateralViewports.begin(),
                         leftLateralViewports.end());
    leftViewports.insert(leftViewports.end(),
                         leftMedialViewports.begin(),
                         leftMedialViewports.end());

    std::vector<SurfaceMontageViewport> rightViewports;
    rightViewports.insert(rightViewports.end(),
                         rightLateralViewports.begin(),
                         rightLateralViewports.end());
    rightViewports.insert(rightViewports.end(),
                         rightMedialViewports.begin(),
                         rightMedialViewports.end());
    
    const int32_t numLeft = static_cast<int32_t>(leftViewports.size());
    const int32_t numRight = static_cast<int32_t>(rightViewports.size());
    const int32_t totalNum = numLeft + numRight;
    
    if (totalNum == 1) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       leftViewports.begin(),
                                       leftViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       rightViewports.begin(),
                                       rightViewports.end());
        CaretAssert(surfaceMontageViewports.size() == 1);
        
        surfaceMontageViewports[0].setRowAndColumn(0, 0);
        
    }
    else if (totalNum == 2) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       leftViewports.begin(),
                                       leftViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       rightViewports.begin(),
                                       rightViewports.end());
        
        CaretAssert(surfaceMontageViewports.size() == 2);
        
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                break;
        }
        
    }
    else if (totalNum == 4) {
        if (numLeft == 4) {
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           leftLateralViewports.begin(),
                                           leftLateralViewports.end());
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           leftMedialViewports.begin(),
                                           leftMedialViewports.end());
            CaretAssert(surfaceMontageViewports.size() == 4);
            surfaceMontageViewports[0].setRowAndColumn(0, 0);
            surfaceMontageViewports[1].setRowAndColumn(1, 0);
            surfaceMontageViewports[2].setRowAndColumn(0, 1);
            surfaceMontageViewports[3].setRowAndColumn(1, 1);
        }
        else if (numRight == 4) {
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           rightLateralViewports.begin(),
                                           rightLateralViewports.end());
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           rightMedialViewports.begin(),
                                           rightMedialViewports.end());
            CaretAssert(surfaceMontageViewports.size() == 4);
            surfaceMontageViewports[0].setRowAndColumn(0, 0);
            surfaceMontageViewports[1].setRowAndColumn(1, 0);
            surfaceMontageViewports[2].setRowAndColumn(0, 1);
            surfaceMontageViewports[3].setRowAndColumn(1, 1);
        }
        else if (numLeft == numRight) {
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           leftViewports.begin(),
                                           leftViewports.end());
            surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                           rightViewports.begin(),
                                           rightViewports.end());
            CaretAssert(surfaceMontageViewports.size() == 4);
            
            surfaceMontageViewports[0].setRowAndColumn(0, 0);
            surfaceMontageViewports[1].setRowAndColumn(1, 0);
            surfaceMontageViewports[2].setRowAndColumn(0, 1);
            surfaceMontageViewports[3].setRowAndColumn(1, 1);
        }
        else {
            CaretAssert(0);
        }
    }
    else if (totalNum == 8) {
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       leftLateralViewports.begin(),
                                       leftLateralViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       leftMedialViewports.begin(),
                                       leftMedialViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       rightLateralViewports.begin(),
                                       rightLateralViewports.end());
        surfaceMontageViewports.insert(surfaceMontageViewports.end(),
                                       rightMedialViewports.begin(),
                                       rightMedialViewports.end());
        CaretAssert(surfaceMontageViewports.size() == 8);
        
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(0, 2);
                surfaceMontageViewports[5].setRowAndColumn(1, 2);
                surfaceMontageViewports[6].setRowAndColumn(0, 3);
                surfaceMontageViewports[7].setRowAndColumn(1, 3);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                surfaceMontageViewports[2].setRowAndColumn(0, 1);
                surfaceMontageViewports[3].setRowAndColumn(1, 1);
                surfaceMontageViewports[4].setRowAndColumn(2, 0);
                surfaceMontageViewports[5].setRowAndColumn(3, 0);
                surfaceMontageViewports[6].setRowAndColumn(2, 1);
                surfaceMontageViewports[7].setRowAndColumn(3, 1);
                break;
        }
    }
    else if (totalNum > 0) {
        CaretAssert(0);
    }
    
    const int32_t numViewports = static_cast<int32_t>(surfaceMontageViewports.size());
    CaretAssert(totalNum == numViewports);
    
//    std::cout << "Orientation: " << SurfaceMontageLayoutOrientationEnum::toName(getLayoutOrientation()) << std::endl;
//    for (int32_t i = 0; i < numViewports; i++) {
//        const SurfaceMontageViewport& svp = surfaceMontageViewports[i];
//        std::cout << qPrintable("("
//                                + AString::number(svp.getRow())
//                                + ","
//                                + AString::number(svp.getColumn())
//                                + ") "
//                                + ProjectionViewTypeEnum::toName(svp.getProjectionViewType()))
//        << std::endl;
//    }
//    std::cout << std::endl;
}

/**
 * Save members to the given scene class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 * @param sceneClass
 *     sceneClass to which information is added.
 */
void
SurfaceMontageConfigurationCerebral::saveMembersToScene(const SceneAttributes* sceneAttributes,
                                                          SceneClass* sceneClass)
{
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
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
 *     sceneClass from which information is restored.
 */
void
SurfaceMontageConfigurationCerebral::restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                                               const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * @return Is left  enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isLeftEnabled() const
{
    return m_leftEnabled;
}

/**
 * Set left enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setLeftEnabled(const bool enabled)
{
    m_leftEnabled = enabled;
}

/**
 * @return Is right enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isRightEnabled() const
{
    return m_rightEnabled;
}

/**
 * Set right enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setRightEnabled(const bool enabled)
{
    m_rightEnabled = enabled;
}

/**
 * @return Is lateral enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isLateralEnabled() const
{
    return m_lateralEnabled;
}

/**
 * Set lateral enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setLateralEnabled(const bool enabled)
{
    m_lateralEnabled = enabled;
}

/**
 * @return Is medial enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isMedialEnabled() const
{
    return m_medialEnabled;
}

/**
 * Set medial enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setMedialEnabled(const bool enabled)
{
    m_medialEnabled = enabled;
}

/**
 * @return Is  enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isFirstSurfaceEnabled() const
{
    return m_firstSurfaceEnabled;
}

/**
 * Set first surface enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setFirstSurfaceEnabled(const bool enabled)
{
    m_firstSurfaceEnabled = enabled;
}

/**
 * @return Is first surfce enabled?
 */
bool
SurfaceMontageConfigurationCerebral::isSecondSurfaceEnabled() const
{
    return m_secondSurfaceEnabled;
}

/**
 * Set second surface enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationCerebral::setSecondSurfaceEnabled(const bool enabled)
{
    m_secondSurfaceEnabled = enabled;
}

/**
 * @return the left first surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getLeftFirstSurfaceSelectionModel()
{
    return m_leftFirstSurfaceSelectionModel;
}

/**
 * @return the left second surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getLeftSecondSurfaceSelectionModel()
{
    return m_leftSecondSurfaceSelectionModel;
}

/**
 * @return the right first surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getRightFirstSurfaceSelectionModel()
{
    return m_rightFirstSurfaceSelectionModel;
}

/**
 * @return the right second surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebral::getRightSecondSurfaceSelectionModel()
{
    return m_rightSecondSurfaceSelectionModel;
}


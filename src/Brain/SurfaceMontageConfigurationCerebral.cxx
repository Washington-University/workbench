
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

#include "CaretAssert.h"
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
    m_firstSurfaceEnabled = false;
    m_secondSurfaceEnabled = true;
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


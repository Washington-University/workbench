
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

#define __SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_DECLARE__
#include "SurfaceMontageConfigurationCerebellar.h"
#undef __SURFACE_MONTAGE_CONFIGURATION_CEREBELLAR_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SurfaceSelectionModel.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageConfigurationCerebellar 
 * \brief Surface montage configuration for cerebellum.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SurfaceMontageConfigurationCerebellar::SurfaceMontageConfigurationCerebellar()
: SurfaceMontageConfigurationAbstract(SurfaceMontageConfigurationTypeEnum::CEREBELLAR_CORTEX_CONFIGURATION,
                                      SUPPORTS_LAYOUT_ORIENTATION_YES)
{
    std::vector<SurfaceTypeEnum::Enum> validSurfaceTypes;
    validSurfaceTypes.push_back(SurfaceTypeEnum::ANATOMICAL);
    validSurfaceTypes.push_back(SurfaceTypeEnum::RECONSTRUCTION);
    validSurfaceTypes.push_back(SurfaceTypeEnum::INFLATED);
    validSurfaceTypes.push_back(SurfaceTypeEnum::VERY_INFLATED);
    
    m_firstSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CEREBELLUM,
                                                             validSurfaceTypes);
    m_secondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CEREBELLUM,
                                                              validSurfaceTypes);
    m_firstSurfaceEnabled = true;
    m_secondSurfaceEnabled = false;
    m_dorsalEnabled = true;
    m_ventralEnabled  = true;
    m_anteriorEnabled = true;
    m_posteriorEnabled = true;
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("m_firstSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_firstSurfaceSelectionModel);
    m_sceneAssistant->add("m_secondSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_secondSurfaceSelectionModel);
    
    m_sceneAssistant->add("m_firstSurfaceEnabled",
                          &m_firstSurfaceEnabled);
    m_sceneAssistant->add("m_secondSurfaceEnabled",
                          &m_secondSurfaceEnabled);
    
    m_sceneAssistant->add("m_dorsalEnabled",
                          &m_dorsalEnabled);
    m_sceneAssistant->add("m_ventralEnabled",
                          &m_ventralEnabled);
    m_sceneAssistant->add("m_anteriorEnabled",
                          &m_anteriorEnabled);
    m_sceneAssistant->add("m_posteriorEnabled",
                          &m_posteriorEnabled);
    
    std::vector<StructureEnum::Enum> supportedStructures;
    supportedStructures.push_back(StructureEnum::CEREBELLUM);
    setupOverlaySet(supportedStructures);
}

/**
 * Destructor.
 */
SurfaceMontageConfigurationCerebellar::~SurfaceMontageConfigurationCerebellar()
{
    delete m_firstSurfaceSelectionModel;
    delete m_secondSurfaceSelectionModel;
    
    delete m_sceneAssistant;
}

/**
 * @return Is this configuration valid?
 */
bool
SurfaceMontageConfigurationCerebellar::isValid()
{
    const bool valid = (getFirstSurfaceSelectionModel()->getSurface() != NULL);
    return valid;
}

/**
 * Initialize the selected surfaces.
 */
void
SurfaceMontageConfigurationCerebellar::initializeSelectedSurfaces()
{
    
}

/**
 * @return First surface selection model.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebellar::getFirstSurfaceSelectionModel()
{
    return m_firstSurfaceSelectionModel;
}

/**
 * @return Second surface selection model.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationCerebellar::getSecondSurfaceSelectionModel()
{
    return m_secondSurfaceSelectionModel;
}

/**
 * @return Is first surface enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isFirstSurfaceEnabled() const
{
    return m_firstSurfaceEnabled;
}

/**
 * Set first surface enabled status.
 * 
 * @param enabled
 *     New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setFirstSurfaceEnabled(const bool enabled)
{
    m_firstSurfaceEnabled = enabled;
}

/**
 * @return Is second surface enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isSecondSurfaceEnabled() const
{
    return m_secondSurfaceEnabled;
}

/**
 * Set first surface enabled status.
 *
 * @param enabled
 *     New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setSecondSurfaceEnabled(const bool enabled)
{
    m_secondSurfaceEnabled = enabled;
}

/**
 * @return Is dorsal enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isDorsalEnabled() const
{
    return m_dorsalEnabled;
}

/**
 * Set dorsal enabled.
 *
 * @param enabled
 *    New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setDorsalEnabled(const bool enabled)
{
    m_dorsalEnabled = enabled;
}

/**
 * @return Is ventral enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isVentralEnabled() const
{
   return  m_ventralEnabled;
}

/**
 * Set ventral enabled.
 *
 * @param enabled
 *    New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setVentralEnabled(const bool enabled)
{
    m_ventralEnabled = enabled;
}

/**
 * @return Is anterior enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isAnteriorEnabled() const
{
    return m_anteriorEnabled;
}

/**
 * Set anterior enabled.
 *
 * @param enabled
 *    New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setAnteriorEnabled(const bool enabled)
{
    m_anteriorEnabled = enabled;
}

/**
 * @return Is posterior enabled.
 */
bool
SurfaceMontageConfigurationCerebellar::isPosteriorEnabled() const
{
    return m_posteriorEnabled;
}

/**
 * Set posterior enabled.
 *
 * @param enabled
 *    New enabled status.
 */
void
SurfaceMontageConfigurationCerebellar::setPosteriorEnabled(const bool enabled)
{
    m_posteriorEnabled = enabled;
}

/**
 * Get the surface montage viewports for the current configuration.
 *
 * @param surfaceMontageViewports
 *    Output of surface montage viewports for drawing.
 */
void
SurfaceMontageConfigurationCerebellar::getSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports)
{
    surfaceMontageViewports.clear();
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
SurfaceMontageConfigurationCerebellar::saveMembersToScene(const SceneAttributes* sceneAttributes,
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
SurfaceMontageConfigurationCerebellar::restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                     const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}


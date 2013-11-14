
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

#define __SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_DECLARE__
#include "SurfaceMontageConfigurationFlatMaps.h"
#undef __SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "SurfaceSelectionModel.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageConfigurationFlatMaps 
 * \brief Surface montage configuration for flat maps.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SurfaceMontageConfigurationFlatMaps::SurfaceMontageConfigurationFlatMaps()
: SurfaceMontageConfigurationAbstract(SurfaceMontageConfigurationTypeEnum::FLAT_CONFIGURATION,
                                      SUPPORTS_LAYOUT_ORIENTATION_NO)
{
    std::vector<SurfaceTypeEnum::Enum> validSurfaceTypes;
    validSurfaceTypes.push_back(SurfaceTypeEnum::FLAT);
    
    m_leftSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_LEFT,
                                                            validSurfaceTypes);
    m_rightSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CORTEX_RIGHT,
                                                             validSurfaceTypes);
    m_cerebellumSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::CEREBELLUM,
                                                                  validSurfaceTypes);
    m_leftEnabled = true;
    m_rightEnabled = true;
    m_cerebellumEnabled = false;
    
    m_sceneAssistant = new SceneClassAssistant();
    
    m_sceneAssistant->add("m_leftSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_leftSurfaceSelectionModel);
    m_sceneAssistant->add("m_rightSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_rightSurfaceSelectionModel);
    m_sceneAssistant->add("m_cerebellumSurfaceSelectionModel",
                          "SurfaceSelectionModel",
                          m_cerebellumSurfaceSelectionModel);
    
    m_sceneAssistant->add("m_leftEnabled",
                          &m_leftEnabled);
    m_sceneAssistant->add("m_rightEnabled",
                          &m_rightEnabled);
    m_sceneAssistant->add("m_cerebellumEnabled",
                          &m_cerebellumEnabled);

    std::vector<StructureEnum::Enum> supportedStructures;
    supportedStructures.push_back(StructureEnum::CEREBELLUM);
    supportedStructures.push_back(StructureEnum::CORTEX_LEFT);
    supportedStructures.push_back(StructureEnum::CORTEX_RIGHT);
    setupOverlaySet(supportedStructures);
}

/**
 * Destructor.
 */
SurfaceMontageConfigurationFlatMaps::~SurfaceMontageConfigurationFlatMaps()
{
    delete m_leftSurfaceSelectionModel;
    delete m_rightSurfaceSelectionModel;
    delete m_cerebellumSurfaceSelectionModel;
    
    delete m_sceneAssistant;
}

/**
 * Initialize the selected surfaces.
 */
void
SurfaceMontageConfigurationFlatMaps::initializeSelectedSurfaces()
{
    
}

/**
 * @return Is this configuration valid?
 */
bool
SurfaceMontageConfigurationFlatMaps::isValid()
{
    const bool valid = ((getLeftSurfaceSelectionModel()->getSurface() != NULL)
                        || (getRightSurfaceSelectionModel()->getSurface() != NULL)
                        || (getCerebellumSurfaceSelectionModel()->getSurface() != NULL));
    return valid;
}


/**
 * @return Is left  enabled?
 */
bool
SurfaceMontageConfigurationFlatMaps::isLeftEnabled() const
{
    return m_leftEnabled;
}

/**
 * Set left enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationFlatMaps::setLeftEnabled(const bool enabled)
{
    m_leftEnabled = enabled;
}

/**
 * @return Is right enabled?
 */
bool
SurfaceMontageConfigurationFlatMaps::isRightEnabled() const
{
    return m_rightEnabled;
}

/**
 * Set right enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationFlatMaps::setRightEnabled(const bool enabled)
{
    m_rightEnabled = enabled;
}

/**
 * @return Is cerebellum enabled?
 */
bool
SurfaceMontageConfigurationFlatMaps::isCerebellumEnabled() const
{
    return m_cerebellumEnabled;
}

/**
 * Set cerebellum enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationFlatMaps::setCerebellumEnabled(const bool enabled)
{
    m_cerebellumEnabled = enabled;
}

/**
 * @return the left surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationFlatMaps::getLeftSurfaceSelectionModel()
{
    return m_leftSurfaceSelectionModel;
}

/**
 * @return the right surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationFlatMaps::getRightSurfaceSelectionModel()
{
    return m_rightSurfaceSelectionModel;
}

/**
 * @return the cerebellum surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationFlatMaps::getCerebellumSurfaceSelectionModel()
{
    return m_cerebellumSurfaceSelectionModel;
}

/**
 * Get the surface montage viewports for the current configuration.
 *
 * @param surfaceMontageViewports
 *    Output of surface montage viewports for drawing.
 */
void
SurfaceMontageConfigurationFlatMaps::getSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports)
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
SurfaceMontageConfigurationFlatMaps::saveMembersToScene(const SceneAttributes* sceneAttributes,
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
SurfaceMontageConfigurationFlatMaps::restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                                               const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}


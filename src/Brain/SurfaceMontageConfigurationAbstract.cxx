
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

#define __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_DECLARE__
#include "SurfaceMontageConfigurationAbstract.h"
#undef __SURFACE_MONTAGE_CONFIGURATION_ABSTRACT_DECLARE__

#include "CaretAssert.h"
#include "OverlaySet.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageConfiguration 
 * \brief Abstract class for surface montage configurations
 * \ingroup Brain
 */

/**
 * Constructor.
 *
 * @param supportsLayoutOrientation
 *    True if the subclass supports layout orientation (landscape/portrait).
 */
SurfaceMontageConfigurationAbstract::SurfaceMontageConfigurationAbstract(const SurfaceMontageConfigurationTypeEnum::Enum configurationType,
                                                                         const SupportLayoutOrientation supportsLayoutOrientation)
: CaretObject(),
m_configurationType(configurationType),
m_supportsLayoutOrientation(supportsLayoutOrientation)
{
    m_sceneAssistant = new SceneClassAssistant();
    
    m_overlaySet = NULL;
    m_layoutOrientation = SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION;
    
    /*
     * Note: Since these members are initialized by the constructor,
     * they do not need to be saved to the scene:
     *    m_configuration
     *    m_supportsLayoutOrientation
     */
    m_sceneAssistant->add<SurfaceMontageLayoutOrientationEnum,
                          SurfaceMontageLayoutOrientationEnum::Enum>("m_layoutOrientation",
                                                                     &m_layoutOrientation);
}

/**
 * Destructor.
 */
SurfaceMontageConfigurationAbstract::~SurfaceMontageConfigurationAbstract()
{
    delete m_sceneAssistant;
    CaretAssertMessage(m_overlaySet,
                       "Did you forget to call setupOverlaySet() from subclass constructor?");
    delete m_overlaySet;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
SurfaceMontageConfigurationAbstract::toString() const
{
    return "SurfaceMontageConfiguration";
}

/**
 * Setup the overlay set for the subclass.
 *
 * @param includeSurfaceStructures
 *     Surface structures supported by subclass.
 */
void
SurfaceMontageConfigurationAbstract::setupOverlaySet(const std::vector<StructureEnum::Enum>& includeSurfaceStructures)
{
    m_overlaySet = new OverlaySet(includeSurfaceStructures,
                                  Overlay::INCLUDE_VOLUME_FILES_NO);
    
    m_sceneAssistant->add("m_overlaySet",
                          "OverlaySet",
                          m_overlaySet);
}

/**
 * @return The overlay set
 */
OverlaySet*
SurfaceMontageConfigurationAbstract::getOverlaySet()
{
    CaretAssertMessage(m_overlaySet,
                       "Did you forget to call setupOverlaySet() from subclass constructor?");
    return m_overlaySet;
}

/**
 * @return The overlay set (const method)
 */
const OverlaySet*
SurfaceMontageConfigurationAbstract::getOverlaySet() const
{
    CaretAssertMessage(m_overlaySet,
                       "Did you forget to call setupOverlaySet() from subclass constructor?");
    return m_overlaySet;
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
SurfaceMontageConfigurationAbstract::saveToScene(const SceneAttributes* sceneAttributes,
                                                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SurfaceMontageConfigurationAbstract",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    saveMembersToScene(sceneAttributes,
                       sceneClass);
    
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
SurfaceMontageConfigurationAbstract::restoreFromScene(const SceneAttributes* sceneAttributes,
                                                        const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    restoreMembersFromScene(sceneAttributes,
                            sceneClass);
}

/**
 * @return The configuration type.
 */
SurfaceMontageConfigurationTypeEnum::Enum
SurfaceMontageConfigurationAbstract::getConfigurationType() const
{
    return m_configurationType;
}

/**
 * @return Configuration has a layout orientation.
 */
bool
SurfaceMontageConfigurationAbstract::hasLayoutOrientation() const
{
    return (m_supportsLayoutOrientation == SUPPORTS_LAYOUT_ORIENTATION_YES);
}

/**
 * @return The selected layout orientation.
 */
SurfaceMontageLayoutOrientationEnum::Enum
SurfaceMontageConfigurationAbstract::getLayoutOrientation() const
{
    return m_layoutOrientation;
}

/**
 * Set the layout orientation.
 * 
 * @param layoutOrientation
 *    New value for layout orientation.
 */
void
SurfaceMontageConfigurationAbstract::setLayoutOrientation(const SurfaceMontageLayoutOrientationEnum::Enum layoutOrientation)
{
    m_layoutOrientation = layoutOrientation;
}




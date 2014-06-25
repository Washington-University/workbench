
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
     *    m_surfaceMontageViewports
     *    
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
SurfaceMontageConfigurationAbstract::setupOverlaySet(const AString& overlaySetName,
                                                     const int32_t tabIndex,
                                                     const std::vector<StructureEnum::Enum>& includeSurfaceStructures)
{
    m_overlaySet = new OverlaySet(overlaySetName,
                                  tabIndex,
                                  includeSurfaceStructures,
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

/**
 * Get the montage viewports for drawing by OpenGL.  The montage viewports
 * will be updated prior to returning them.   OpenGL will update
 * the viewing dimensions (x, y, width, height) in the returned montage
 * viewports.
 *
 * @param surfaceMontageViewports
 *    The montage viewports.
 */
void
SurfaceMontageConfigurationAbstract::getSurfaceMontageViewportsForDrawing(std::vector<SurfaceMontageViewport*>& surfaceMontageViewports)
{
    m_surfaceMontageViewports.clear();
    
    updateSurfaceMontageViewports(m_surfaceMontageViewports);

    for (std::vector<SurfaceMontageViewport>::iterator iter = m_surfaceMontageViewports.begin();
         iter != m_surfaceMontageViewports.end();
         iter++) {
        SurfaceMontageViewport& svp = *iter;
        surfaceMontageViewports.push_back(&svp);
    }
}

/**
 * Get the montage viewports that will be used by the mouse transformations.
 *
 * @param surfaceMontageViewports
 *    The montage viewports.
 */
void
SurfaceMontageConfigurationAbstract::getSurfaceMontageViewportsForTransformation(std::vector<const SurfaceMontageViewport*>& surfaceMontageViewports) const
{
    surfaceMontageViewports.clear();
    
    for (std::vector<SurfaceMontageViewport>::const_iterator iter = m_surfaceMontageViewports.begin();
         iter != m_surfaceMontageViewports.end();
         iter++) {
        const SurfaceMontageViewport& svp = *iter;
        surfaceMontageViewports.push_back(&svp);
    }
}

/**
 * Copy the given configuration to this configurtion.
 *
 * @param configuration.
 *    Configuration that is copied.
 */
void
SurfaceMontageConfigurationAbstract::copyConfiguration(SurfaceMontageConfigurationAbstract* configuration)
{
    CaretAssert(m_configurationType == configuration->m_configurationType);
    
    m_overlaySet->copyOverlaySet(configuration->m_overlaySet);
    m_layoutOrientation = configuration->m_layoutOrientation;
    m_surfaceMontageViewports = configuration->m_surfaceMontageViewports;
}




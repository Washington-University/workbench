
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

#define __SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_DECLARE__
#include "SurfaceMontageConfigurationFlatMaps.h"
#undef __SURFACE_MONTAGE_CONFIGURATION_FLAT_MAPS_DECLARE__

#include "CaretAssert.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
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
SurfaceMontageConfigurationFlatMaps::SurfaceMontageConfigurationFlatMaps(const int32_t tabIndex)
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
    m_cerebellumEnabled = true;
    
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
    setupOverlaySet("Flat Montage",
                    tabIndex,
                    supportedStructures);
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
 * @return the left surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationFlatMaps::getLeftSurfaceSelectionModel() const
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
 * @return the right surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationFlatMaps::getRightSurfaceSelectionModel() const
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
 * @return the cerebellum surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationFlatMaps::getCerebellumSurfaceSelectionModel() const
{
    return m_cerebellumSurfaceSelectionModel;
}

/**
 * Update the montage viewports using the current selected surfaces and settings.
 *
 * @param surfaceMontageViewports
 *     Will be loaded with the montage viewports.
 */
void
SurfaceMontageConfigurationFlatMaps::updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports)
{
    surfaceMontageViewports.clear();
    
    if (m_leftEnabled) {
        Surface* leftSurface = m_leftSurfaceSelectionModel->getSurface();
        if (leftSurface != NULL) {
            SurfaceMontageViewport smv(leftSurface,
                                       leftSurface,
                                       ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE);
            surfaceMontageViewports.push_back(smv);
        }
    }
    
    if (m_rightEnabled) {
        Surface* rightSurface = m_rightSurfaceSelectionModel->getSurface();
        if (rightSurface != NULL) {
            SurfaceMontageViewport smv(rightSurface,
                                       rightSurface,
                                       ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE);
            surfaceMontageViewports.push_back(smv);
        }
    }
    
    if (m_cerebellumEnabled) {
        Surface* cerebellumSurface = m_cerebellumSurfaceSelectionModel->getSurface();
        if (cerebellumSurface != NULL) {
            SurfaceMontageViewport smv(cerebellumSurface,
                                       cerebellumSurface,
                                       ProjectionViewTypeEnum::PROJECTION_VIEW_CEREBELLUM_FLAT_SURFACE);
            surfaceMontageViewports.push_back(smv);
        }
    }
    
    const int32_t numSurfaces = static_cast<int32_t>(surfaceMontageViewports.size());
    for (int32_t i = 0; i < numSurfaces; i++) {
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::COLUMN_LAYOUT_ORIENTATION:
                surfaceMontageViewports[i].setRowAndColumn(i, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[i].setRowAndColumn(0, i);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[i].setRowAndColumn(i, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::ROW_LAYOUT_ORIENTATION:
                surfaceMontageViewports[i].setRowAndColumn(0, i);
                break;
        }
    }
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

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
SurfaceMontageConfigurationFlatMaps::toString() const
{
    PlainTextStringBuilder tb;
    getDescriptionOfContent(tb);
    return tb.getText();
}

/**
 * Get a text description of the instance's content.
 *
 * @param descriptionOut
 *    Description of the instance's content.
 */
void
SurfaceMontageConfigurationFlatMaps::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    descriptionOut.addLine("Flat Montage: ");
    
    if (isLeftEnabled()) {
        const Surface* firstLeftSurface = getLeftSurfaceSelectionModel()->getSurface();
        if (firstLeftSurface != NULL) {
            descriptionOut.addLine("Left Surface:");
            descriptionOut.pushIndentation();
            firstLeftSurface->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
        }
    }
    
    if (isRightEnabled()) {
        const Surface* firstRightSurface = getRightSurfaceSelectionModel()->getSurface();
        if (firstRightSurface != NULL) {
            descriptionOut.addLine("Right Surface:");
            descriptionOut.pushIndentation();
            firstRightSurface->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
        }
    }
    
    if (isCerebellumEnabled()) {
        const Surface* cerebellumSurface = getCerebellumSurfaceSelectionModel()->getSurface();
        if (cerebellumSurface != NULL) {
            descriptionOut.addLine("Cerebellum Surface:");
            descriptionOut.pushIndentation();
            cerebellumSurface->getDescriptionOfContent(descriptionOut);
            descriptionOut.popIndentation();
        }
    }
}

/**
 * Get all surfaces displayed in this configuration.
 *
 * @param surfaceOut
 *    Will contain all displayed surfaces upon exit.
 */
void
SurfaceMontageConfigurationFlatMaps::getDisplayedSurfaces(std::vector<Surface*>& surfacesOut) const
{
    surfacesOut.clear();
    
    if (isLeftEnabled()) {
        const Surface* firstLeftSurface = getLeftSurfaceSelectionModel()->getSurface();
        if (firstLeftSurface != NULL) {
            surfacesOut.push_back(const_cast<Surface*>(firstLeftSurface));
        }
    }
    
    if (isRightEnabled()) {
        const Surface* firstRightSurface = getRightSurfaceSelectionModel()->getSurface();
        if (firstRightSurface != NULL) {
            surfacesOut.push_back(const_cast<Surface*>(firstRightSurface));
        }
    }
    
    if (isCerebellumEnabled()) {
        const Surface* cerebellumSurface = getCerebellumSurfaceSelectionModel()->getSurface();
        if (cerebellumSurface != NULL) {
            surfacesOut.push_back(const_cast<Surface*>(cerebellumSurface));
        }
    }
}


/**
 * Copy the given configuration to this configurtion.
 *
 * @param configuration.
 *    Configuration that is copied.
 */
void
SurfaceMontageConfigurationFlatMaps::copyConfiguration(SurfaceMontageConfigurationAbstract* configuration)
{
    SurfaceMontageConfigurationAbstract::copyConfiguration(configuration);

    SurfaceMontageConfigurationFlatMaps* flatConfiguration = dynamic_cast<SurfaceMontageConfigurationFlatMaps*>(configuration);
    CaretAssert(flatConfiguration);

    m_leftSurfaceSelectionModel->setSurface(flatConfiguration->m_leftSurfaceSelectionModel->getSurface());
    
    m_rightSurfaceSelectionModel->setSurface(flatConfiguration->m_rightSurfaceSelectionModel->getSurface());
    
    m_cerebellumSurfaceSelectionModel->setSurface(flatConfiguration->m_cerebellumSurfaceSelectionModel->getSurface());
    
    m_leftEnabled = flatConfiguration->m_leftEnabled;
    
    m_rightEnabled = flatConfiguration->m_rightEnabled;
    
    m_cerebellumEnabled = flatConfiguration->m_cerebellumEnabled;
}


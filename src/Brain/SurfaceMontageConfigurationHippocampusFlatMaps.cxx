
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

#define __SURFACE_MONTAGE_CONFIGURATION_HIPPOCAMPUS_FLAT_MAPS_DECLARE__
#include "SurfaceMontageConfigurationHippocampusFlatMaps.h"
#undef __SURFACE_MONTAGE_CONFIGURATION_HIPPOCAMPUS_FLAT_MAPS_DECLARE__

#include "BrainStructure.h"
#include "CaretAssert.h"
#include "PlainTextStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "Surface.h"
#include "SurfaceSelectionModel.h"

using namespace caret;


    
/**
 * \class caret::SurfaceMontageConfigurationHippocampusFlatMaps 
 * \brief Surface montage hippocampus flat maps configuration.
 * \ingroup Brain
 */

/**
 * Constructor.
 */
SurfaceMontageConfigurationHippocampusFlatMaps::SurfaceMontageConfigurationHippocampusFlatMaps(const int32_t tabIndex)
: SurfaceMontageConfigurationAbstract(SurfaceMontageConfigurationTypeEnum::HIPPOCAMPUS_FLAT_CONFIGURATION,
                                      SUPPORTS_LAYOUT_ORIENTATION_YES)
{
    const std::vector<SurfaceTypeEnum::Enum> validFlatSurfaceTypes { SurfaceTypeEnum::FLAT };
    
    m_leftFirstSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::HIPPOCAMPUS_LEFT,
                                                                 validFlatSurfaceTypes);
    m_leftSecondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::HIPPOCAMPUS_DENTATE_LEFT,
                                                                  validFlatSurfaceTypes);
    m_rightFirstSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::HIPPOCAMPUS_RIGHT,
                                                                  validFlatSurfaceTypes);
    m_rightSecondSurfaceSelectionModel = new SurfaceSelectionModel(StructureEnum::HIPPOCAMPUS_DENTATE_RIGHT,
                                                                   validFlatSurfaceTypes);
    m_leftEnabled = true;
    m_rightEnabled = true;
    m_firstSurfaceEnabled = true;
    m_secondSurfaceEnabled = true;
    
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

    std::vector<StructureEnum::Enum> supportedStructures;
    supportedStructures.push_back(StructureEnum::HIPPOCAMPUS_DENTATE_LEFT);
    supportedStructures.push_back(StructureEnum::HIPPOCAMPUS_DENTATE_RIGHT);
    supportedStructures.push_back(StructureEnum::HIPPOCAMPUS_LEFT);
    supportedStructures.push_back(StructureEnum::HIPPOCAMPUS_RIGHT);
    setupOverlaySet("Hippocampus Montage",
                    tabIndex,
                    supportedStructures);
}

/**
 * Destructor.
 */
SurfaceMontageConfigurationHippocampusFlatMaps::~SurfaceMontageConfigurationHippocampusFlatMaps()
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
SurfaceMontageConfigurationHippocampusFlatMaps::initializeSelectedSurfaces()
{
    /* Nothing to initialize since only one surface type (FLAT) */
}

/**
 * @return Is this configuration valid?
 */
bool
SurfaceMontageConfigurationHippocampusFlatMaps::isValid()
{
    const bool valid = ((getLeftFirstSurfaceSelectionModel()->getSurface() != NULL)
                        || (getRightFirstSurfaceSelectionModel()->getSurface() != NULL)
                        || (getLeftSecondSurfaceSelectionModel()->getSurface() != NULL)
                        || (getRightSecondSurfaceSelectionModel()->getSurface() != NULL));
    return valid;
}

/**
 * Update the montage viewports using the current selected surfaces and settings.
 *
 * @param surfaceMontageViewports
 *     Will be loaded with the montage viewports.
 */
void
SurfaceMontageConfigurationHippocampusFlatMaps::updateSurfaceMontageViewports(std::vector<SurfaceMontageViewport>& surfaceMontageViewports)
{
    surfaceMontageViewports.clear();
    
    std::vector<SurfaceMontageViewport> leftViewports;
    std::vector<SurfaceMontageViewport> rightViewports;
    
    if (m_leftEnabled) {
        Surface* leftFirstSurface(NULL);
        if (m_firstSurfaceEnabled) {
            leftFirstSurface = m_leftFirstSurfaceSelectionModel->getSurface();
            if (leftFirstSurface != NULL) {
                SurfaceMontageViewport smv(leftFirstSurface,
                                           leftFirstSurface,
                                           ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_SURFACE);
                leftViewports.push_back(smv);
            }
        }
        if (m_secondSurfaceEnabled) {
            Surface* leftSecondSurface = m_leftSecondSurfaceSelectionModel->getSurface();
            if (leftSecondSurface != NULL) {
                /*
                 * Note: Using first surface for projection results in second surface being
                 * drawn about the same size as first surface
                 */
                SurfaceMontageViewport smv(leftSecondSurface,
                                           (leftFirstSurface != NULL) ? leftFirstSurface : leftSecondSurface,
                                           ProjectionViewTypeEnum::PROJECTION_VIEW_LEFT_FLAT_DENTATE_SURFACE);
                leftViewports.push_back(smv);
            }
        }
    }
    
    if (m_rightEnabled) {
        Surface* rightFirstSurface(NULL);
        if (m_firstSurfaceEnabled) {
            rightFirstSurface = m_rightFirstSurfaceSelectionModel->getSurface();
            if (rightFirstSurface != NULL) {
                SurfaceMontageViewport smv(rightFirstSurface,
                                           rightFirstSurface,
                                           ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_SURFACE);
                rightViewports.push_back(smv);
            }
        }

        if (m_secondSurfaceEnabled) {
            Surface* rightSecondSurface = m_rightSecondSurfaceSelectionModel->getSurface();
            if (rightSecondSurface != NULL) {
                /*
                 * Note: Using first surface for projection results in second surface being
                 * drawn about the same size as first surface
                 */
                SurfaceMontageViewport smv(rightSecondSurface,
                                           (rightFirstSurface != NULL) ? rightFirstSurface : rightSecondSurface,
                                           ProjectionViewTypeEnum::PROJECTION_VIEW_RIGHT_FLAT_DENTATE_SURFACE);
                rightViewports.push_back(smv);
            }
        }
    }

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
            case SurfaceMontageLayoutOrientationEnum::COLUMN_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(1, 0);
                break;
            case SurfaceMontageLayoutOrientationEnum::ROW_LAYOUT_ORIENTATION:
                surfaceMontageViewports[0].setRowAndColumn(0, 0);
                surfaceMontageViewports[1].setRowAndColumn(0, 1);
                break;
        }
        
    }
    else if (totalNum == 4) {
        if (numLeft == numRight) {
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
        
        /*
         * Override layout for row/column layout
         */
        switch (getLayoutOrientation()) {
            case SurfaceMontageLayoutOrientationEnum::COLUMN_LAYOUT_ORIENTATION:
                for (int32_t i = 0; i < 4; i++) {
                    surfaceMontageViewports[i].setRowAndColumn(i, 0);
                }
                break;
            case SurfaceMontageLayoutOrientationEnum::LANDSCAPE_LAYOUT_ORIENTATION:
                break;
            case SurfaceMontageLayoutOrientationEnum::PORTRAIT_LAYOUT_ORIENTATION:
                break;
            case SurfaceMontageLayoutOrientationEnum::ROW_LAYOUT_ORIENTATION:
                for (int32_t i = 0; i < 4; i++) {
                    surfaceMontageViewports[i].setRowAndColumn(0, i);
                }
                break;
        }
    }
    else if (totalNum > 0) {
        CaretAssert(0);
    }
    
    CaretAssert(totalNum == static_cast<int32_t>(surfaceMontageViewports.size()));
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
SurfaceMontageConfigurationHippocampusFlatMaps::saveMembersToScene(const SceneAttributes* sceneAttributes,
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
SurfaceMontageConfigurationHippocampusFlatMaps::restoreMembersFromScene(const SceneAttributes* sceneAttributes,
                                                               const SceneClass* sceneClass)
{
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}

/**
 * @return Is left  enabled?
 */
bool
SurfaceMontageConfigurationHippocampusFlatMaps::isLeftEnabled() const
{
    return m_leftEnabled;
}

/**
 * Set left enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationHippocampusFlatMaps::setLeftEnabled(const bool enabled)
{
    m_leftEnabled = enabled;
}

/**
 * @return Is right enabled?
 */
bool
SurfaceMontageConfigurationHippocampusFlatMaps::isRightEnabled() const
{
    return m_rightEnabled;
}

/**
 * Set right enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationHippocampusFlatMaps::setRightEnabled(const bool enabled)
{
    m_rightEnabled = enabled;
}

/**
 * @return Is  enabled?
 */
bool
SurfaceMontageConfigurationHippocampusFlatMaps::isFirstSurfaceEnabled() const
{
    return m_firstSurfaceEnabled;
}

/**
 * Set first surface enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationHippocampusFlatMaps::setFirstSurfaceEnabled(const bool enabled)
{
    m_firstSurfaceEnabled = enabled;
}

/**
 * @return Is first surfce enabled?
 */
bool
SurfaceMontageConfigurationHippocampusFlatMaps::isSecondSurfaceEnabled() const
{
    return m_secondSurfaceEnabled;
}

/**
 * Set second surface enabled
 * @param enabled
 *    New status
 */
void
SurfaceMontageConfigurationHippocampusFlatMaps::setSecondSurfaceEnabled(const bool enabled)
{
    m_secondSurfaceEnabled = enabled;
}

/**
 * @return the left first surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationHippocampusFlatMaps::getLeftFirstSurfaceSelectionModel()
{
    return m_leftFirstSurfaceSelectionModel;
}

/**
 * @return the left first surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationHippocampusFlatMaps::getLeftFirstSurfaceSelectionModel() const
{
    return m_leftFirstSurfaceSelectionModel;
}

/**
 * @return the left second surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationHippocampusFlatMaps::getLeftSecondSurfaceSelectionModel()
{
    return m_leftSecondSurfaceSelectionModel;
}

/**
 * @return the left second surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationHippocampusFlatMaps::getLeftSecondSurfaceSelectionModel() const
{
    return m_leftSecondSurfaceSelectionModel;
}

/**
 * @return the right first surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationHippocampusFlatMaps::getRightFirstSurfaceSelectionModel()
{
    return m_rightFirstSurfaceSelectionModel;
}

/**
 * @return the right first surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationHippocampusFlatMaps::getRightFirstSurfaceSelectionModel() const
{
    return m_rightFirstSurfaceSelectionModel;
}

/**
 * @return the right second surface selection in this configuration.
 */
SurfaceSelectionModel*
SurfaceMontageConfigurationHippocampusFlatMaps::getRightSecondSurfaceSelectionModel()
{
    return m_rightSecondSurfaceSelectionModel;
}

/**
 * @return the right second surface selection in this configuration.
 */
const SurfaceSelectionModel*
SurfaceMontageConfigurationHippocampusFlatMaps::getRightSecondSurfaceSelectionModel() const
{
    return m_rightSecondSurfaceSelectionModel;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
SurfaceMontageConfigurationHippocampusFlatMaps::toString() const
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
SurfaceMontageConfigurationHippocampusFlatMaps::getDescriptionOfContent(PlainTextStringBuilder& descriptionOut) const
{
    AString msg;
    
    descriptionOut.addLine("Hippocampus Montage: ");
    
    descriptionOut.pushIndentation();
    
    if (isLeftEnabled()) {
        if (isFirstSurfaceEnabled()) {
            const Surface* firstLeftSurface = getLeftFirstSurfaceSelectionModel()->getSurface();
            if (firstLeftSurface != NULL) {
                descriptionOut.addLine("Left Surface:");
                descriptionOut.pushIndentation();
                firstLeftSurface->getDescriptionOfContent(descriptionOut);
                descriptionOut.popIndentation();
            }
        }
        
        if (isSecondSurfaceEnabled()) {
            const Surface* secondLeftSurface = getLeftSecondSurfaceSelectionModel()->getSurface();
            if (secondLeftSurface != NULL) {
                descriptionOut.addLine("Left Surface:");
                descriptionOut.pushIndentation();
                secondLeftSurface->getDescriptionOfContent(descriptionOut);
                descriptionOut.popIndentation();
            }
        }
    }
    
    if (isRightEnabled()) {
        if (isFirstSurfaceEnabled()) {
            const Surface* firstRightSurface = getRightFirstSurfaceSelectionModel()->getSurface();
            if (firstRightSurface != NULL) {
                descriptionOut.addLine("Right Surface:");
                descriptionOut.pushIndentation();
                firstRightSurface->getDescriptionOfContent(descriptionOut);
                descriptionOut.popIndentation();
            }
        }
        if (isSecondSurfaceEnabled()) {
            const Surface* secondRightSurface = getRightSecondSurfaceSelectionModel()->getSurface();
            if (secondRightSurface != NULL) {
                descriptionOut.addLine("Right Surface:");
                descriptionOut.pushIndentation();
                secondRightSurface->getDescriptionOfContent(descriptionOut);
                descriptionOut.popIndentation();
            }
        }
    }
        
    descriptionOut.popIndentation();
}

/**
 * Get all surfaces displayed in this configuration.
 *
 * @param surfaceOut
 *    Will contain all displayed surfaces upon exit.
 */
void
SurfaceMontageConfigurationHippocampusFlatMaps::getDisplayedSurfaces(std::vector<Surface*>& surfacesOut) const
{
    surfacesOut.clear();
    
    if (isLeftEnabled()) {
        Surface* firstLeftSurface = NULL;;
        if (isFirstSurfaceEnabled()) {
            firstLeftSurface = const_cast<Surface*>(getLeftFirstSurfaceSelectionModel()->getSurface());
            if (firstLeftSurface != NULL) {
                surfacesOut.push_back(const_cast<Surface*>(firstLeftSurface));
            }
        }
        
        if (isSecondSurfaceEnabled()) {
            const Surface* secondLeftSurface = getLeftSecondSurfaceSelectionModel()->getSurface();
            if (secondLeftSurface != NULL) {
                if (secondLeftSurface != firstLeftSurface) {
                    surfacesOut.push_back(const_cast<Surface*>(secondLeftSurface));
                }
            }
        }
    }
    
    if (isRightEnabled()) {
        Surface* firstRightSurface = NULL;
        if (isFirstSurfaceEnabled()) {
            firstRightSurface = const_cast<Surface*>(getRightFirstSurfaceSelectionModel()->getSurface());
            if (firstRightSurface != NULL) {
                surfacesOut.push_back(const_cast<Surface*>(firstRightSurface));
            }
        }
        if (isSecondSurfaceEnabled()) {
            const Surface* secondRightSurface = getRightSecondSurfaceSelectionModel()->getSurface();
            if (secondRightSurface != NULL) {
                if (secondRightSurface != firstRightSurface) {
                    surfacesOut.push_back(const_cast<Surface*>(secondRightSurface));
                }
            }
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
SurfaceMontageConfigurationHippocampusFlatMaps::copyConfiguration(SurfaceMontageConfigurationAbstract* configuration)
{
    SurfaceMontageConfigurationAbstract::copyConfiguration(configuration);
    
    SurfaceMontageConfigurationHippocampusFlatMaps* cerebralConfiguration = dynamic_cast<SurfaceMontageConfigurationHippocampusFlatMaps*>(configuration);
    CaretAssert(cerebralConfiguration);

    m_leftFirstSurfaceSelectionModel->setSurface(cerebralConfiguration->m_leftFirstSurfaceSelectionModel->getSurface());
    
    m_leftSecondSurfaceSelectionModel->setSurface(cerebralConfiguration->m_leftSecondSurfaceSelectionModel->getSurface());
    
    m_rightFirstSurfaceSelectionModel->setSurface(cerebralConfiguration->m_rightFirstSurfaceSelectionModel->getSurface());
    
    m_rightSecondSurfaceSelectionModel->setSurface(cerebralConfiguration->m_rightSecondSurfaceSelectionModel->getSurface());
    
    m_leftEnabled = cerebralConfiguration->m_leftEnabled;
    
    m_rightEnabled = cerebralConfiguration->m_rightEnabled;
    
    m_firstSurfaceEnabled = cerebralConfiguration->m_firstSurfaceEnabled;
    
    m_secondSurfaceEnabled = cerebralConfiguration->m_secondSurfaceEnabled;
}



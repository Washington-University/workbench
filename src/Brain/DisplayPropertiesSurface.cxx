
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

#define __DISPLAY_PROPERTIES_SURFACE_DECLARE__
#include "DisplayPropertiesSurface.h"
#undef __DISPLAY_PROPERTIES_SURFACE_DECLARE__

#include "SceneClassAssistant.h"
#include "SceneAttributes.h"
#include "SceneClass.h"

using namespace caret;
    
/**
 * \class DisplayPropertiesSurface 
 * \brief Display properties for surface drawing attributes.
 *
 * Display properties for surface drawing attributes.
 */


/**
 * Constructor.
 */
DisplayPropertiesSurface::DisplayPropertiesSurface()
: DisplayProperties()
{
    m_displayNormalVectors = false;
    m_linkSize = 2.0;
    m_nodeSize = 2.0;
    m_surfaceDrawingType = SurfaceDrawingTypeEnum::DRAW_AS_TRIANGLES;
    m_opacity = 1.0;
    resetDefaultColorRGB();
    
    m_sceneAssistant->add("m_displayNormalVectors",
                          &m_displayNormalVectors);
    m_sceneAssistant->add("m_linkSize",
                          &m_linkSize);
    m_sceneAssistant->add("m_nodeSize",
                          &m_nodeSize);
    m_sceneAssistant->add("m_opacity",
                          &m_opacity);
    m_sceneAssistant->add<SurfaceDrawingTypeEnum, SurfaceDrawingTypeEnum::Enum>("m_surfaceDrawingType",
                                                                &m_surfaceDrawingType);
    m_sceneAssistant->addArray("m_defaultColorRGB",
                               &m_defaultColorRGB[0],
                               m_defaultColorRGB.size(),
                               178);
    
}

/**
 * Destructor.
 */
DisplayPropertiesSurface::~DisplayPropertiesSurface()
{
}

/**
 * Reset all settings to their defaults
 * and remove any data.
 */
void 
DisplayPropertiesSurface::reset()
{
}

/**
 * Update due to changes in data.
 */
void 
DisplayPropertiesSurface::update()
{
    
}

/**
 * Copy the display properties from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which properties are copied.
 * @param targetTabIndex
 *    Index of tab to which properties are copied.
 */
void
DisplayPropertiesSurface::copyDisplayProperties(const int32_t /*sourceTabIndex*/,
                                               const int32_t /*targetTabIndex*/)
{
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
DisplayPropertiesSurface::saveToScene(const SceneAttributes* sceneAttributes,
                   const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "DisplayPropertiesSurface",
                                            1);
    
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
DisplayPropertiesSurface::restoreFromScene(const SceneAttributes* sceneAttributes,
                        const SceneClass* sceneClass)
{
    resetDefaultColorRGB();
    
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    switch (sceneAttributes->getSceneType()) {
        case SceneTypeEnum::SCENE_TYPE_FULL:
            break;
        case SceneTypeEnum::SCENE_TYPE_GENERIC:
            break;
    }
    
}

/**
 * @return The surface drawing type
 */
SurfaceDrawingTypeEnum::Enum
DisplayPropertiesSurface::getSurfaceDrawingType() const
{
    return m_surfaceDrawingType;
}

/**
 * Set the surface drawing type.
 *
 * @param surfaceDrawingType
 *   The surface drawing type
 */
void
DisplayPropertiesSurface::setSurfaceDrawingType(const SurfaceDrawingTypeEnum::Enum surfaceDrawingType)
{
    m_surfaceDrawingType = surfaceDrawingType;
}

/**
 * @return Node size.
 */
float
DisplayPropertiesSurface::getNodeSize() const
{
    return m_nodeSize;
}

/**
 * Set node size
 *
 * @param nodeSize
 *    New node size.
 */
void
DisplayPropertiesSurface::setNodeSize(const float nodeSize)
{
    m_nodeSize = nodeSize;
}

/**
 * @return Link size.
 */
float
DisplayPropertiesSurface::getLinkSize() const
{
    return m_linkSize;
}

/**
 * Set link size.
 *
 * @param linkSize
 *    New link size.
 */
void
DisplayPropertiesSurface::setLinkSize(const float linkSize)
{
    m_linkSize = linkSize;
}

/**
 * @return Display normal vectors.
 */
bool
DisplayPropertiesSurface::isDisplayNormalVectors() const
{
    return m_displayNormalVectors;
}

/**
 * Set display normal vectors.
 *
 * @param displayNormalVectors
 *   New value for display normal vectors.
 */
void
DisplayPropertiesSurface::setDisplayNormalVectors(const bool displayNormalVectors)
{
    m_displayNormalVectors = displayNormalVectors;
}

/**
 * @return The overall surface opacity.
 */
float
DisplayPropertiesSurface::getOpacity() const
{
    return m_opacity;
}

/**
 * Set the overall surface opacity.
 *
 * @param opacity
 *    New value for opacity.
 */
void
DisplayPropertiesSurface::setOpacity(const float opacity)
{
    m_opacity = opacity;
}

/**
 * Set the default color used when no overlays color vertices
 * @param defaultColorRGB
 *    The new default color
 */
void
DisplayPropertiesSurface::setDefaultColorRGB(const std::array<uint8_t, 3>& defaultColorRGB)
{
    m_defaultColorRGB = defaultColorRGB;
}

/**
 * @return The default color used when no overlays color vertices
 */
std::array<uint8_t, 3>
DisplayPropertiesSurface::getDefaultColorRGB() const
{
    return m_defaultColorRGB;
}

/**
 * Reset the default surface color
 */
void
DisplayPropertiesSurface::resetDefaultColorRGB()
{
    m_defaultColorRGB.fill(178);
}




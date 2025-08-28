
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
    m_displayFrontNormalVectors = false;
    m_displayBackNormalVectors = false;
    m_displayTriangleFrontNormalVectors = false;
    m_displayTriangleBackNormalVectors = false;
    m_normalVectorLength = 10.0;
    m_linkSize = 2.0;
    m_nodeSize = 2.0;
    m_surfaceDrawingType = SurfaceDrawingTypeEnum::DRAW_AS_TRIANGLES;
    m_opacity = 1.0;
    m_backfaceCullingEnabled = false;
    resetDefaultColorRGB();
    
    m_sceneAssistant->add("m_displayNormalVectors", /* use old name for scenes so old scenes work */
                          &m_displayFrontNormalVectors);
    m_sceneAssistant->add("m_displayBackNormalVectors",
                          &m_displayBackNormalVectors);
    m_sceneAssistant->add("m_displayTriangleFrontNormalVectors",
                          &m_displayTriangleFrontNormalVectors);
    m_sceneAssistant->add("m_displayTriangleBackNormalVectors",
                          &m_displayTriangleBackNormalVectors);
    m_sceneAssistant->add("m_normalVectorLength",
                          &m_normalVectorLength);
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
    m_sceneAssistant->add("m_backfaceCullingEnabled",
                          &m_backfaceCullingEnabled);
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
 * @return Display front normal vectors.
 */
bool
DisplayPropertiesSurface::isDisplayFrontNormalVectors() const
{
    return m_displayFrontNormalVectors;
}

/**
 * Set display front normal vectors.
 *
 * @param displayFrontNormalVectors
 *   New value for display normal vectors.
 */
void
DisplayPropertiesSurface::setDisplayFrontNormalVectors(const bool displayNormalVectors)
{
    m_displayFrontNormalVectors = displayNormalVectors;
}

/**
 * @return Display back normal vectors.
 */
bool
DisplayPropertiesSurface::isDisplayBackNormalVectors() const
{
    return m_displayBackNormalVectors;
}

/**
 * Set display back normal vectors.
 *
 * @param displayNormalVectors
 *   New value for display normal vectors.
 */
void
DisplayPropertiesSurface::setDisplayBackNormalVectors(const bool displayNormalVectors)
{
    m_displayBackNormalVectors = displayNormalVectors;
}

/**
 * @return Display triangle front normal vectors
 */
bool
DisplayPropertiesSurface::isDisplayTriangleFrontNormalVectors() const
{
    return m_displayTriangleFrontNormalVectors;
}

/**
 * Set display triangle front normal vectors.
 *
 * @param displayNormalVectors
 *   New value for display normal vectors.
 */
void
DisplayPropertiesSurface::setDisplayTriangleFrontNormalVectors(const bool displayNormalVectors)
{
    m_displayTriangleFrontNormalVectors = displayNormalVectors;
}

/**
 * @return Display triangle back normal vectors
 */
bool
DisplayPropertiesSurface::isDisplayTriangleBackNormalVectors() const
{
    return m_displayTriangleBackNormalVectors;
}

/**
 * Set display triangles back normal vectors.
 *
 * @param displayNormalVectors
 *   New value for display normal vectors.
 */
void
DisplayPropertiesSurface::setDisplayTriangleBackNormalVectors(const bool displayNormalVectors)
{
    m_displayTriangleBackNormalVectors = displayNormalVectors;
}

/**
 * @return Length for drawing normal vectors
 */
float
DisplayPropertiesSurface::getNormalVectorLength() const
{
    return m_normalVectorLength;
}

/**
 * Set length for drawing normal vectors
 * @param length
 *    New length
 */
void
DisplayPropertiesSurface::setNormalVectorLength(const float length)
{
    m_normalVectorLength = length;
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

/**
 * @return True if backface culling is enabled
 */
bool
DisplayPropertiesSurface::isBackfaceCullingEnabled() const
{
    return m_backfaceCullingEnabled;
}

/**
 * Set backface culling enabled
 * @param enabled
 *    New status
 */
void
DisplayPropertiesSurface::setBackfaceCullingEnabled(const bool enabled)
{
    m_backfaceCullingEnabled = enabled;
}



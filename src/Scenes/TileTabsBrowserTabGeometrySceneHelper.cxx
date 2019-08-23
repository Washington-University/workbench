
/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __TILE_TABS_BROWSER_TAB_GEOMETRY_SCENE_HELPER_DECLARE__
#include "TileTabsBrowserTabGeometrySceneHelper.h"
#undef __TILE_TABS_BROWSER_TAB_GEOMETRY_SCENE_HELPER_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "TileTabsBrowserTabGeometry.h"

using namespace caret;


    
/**
 * \class caret::TileTabsBrowserTabGeometrySceneHelper 
 * \brief Helps with saving/restoring tile tabs geometry from scene
 * \ingroup Scenes
 */

/**
 * Constructor.
 *
 * @param geometry
 *     Tile tabs geometry saved to or restored from scene
 */
TileTabsBrowserTabGeometrySceneHelper::TileTabsBrowserTabGeometrySceneHelper(TileTabsBrowserTabGeometry* geometry)
: CaretObject(),
m_geometry(geometry)
{
    CaretAssert(m_geometry);
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
 
    m_sceneAssistant->add("m_displayFlag",
                          &m_geometry->m_displayFlag);
    m_sceneAssistant->add("m_tabIndex",
                          &m_geometry->m_tabIndex);
    m_sceneAssistant->add("m_minX",
                          &m_geometry->m_minX);
    m_sceneAssistant->add("m_maxX",
                          &m_geometry->m_maxX);
    m_sceneAssistant->add("m_minY",
                          &m_geometry->m_minY);
    m_sceneAssistant->add("m_maxY",
                          &m_geometry->m_maxY);
    m_sceneAssistant->add("m_stackingOrder",
                          &m_geometry->m_stackingOrder);
    m_sceneAssistant->add<TileTabsLayoutBackgroundTypeEnum, TileTabsLayoutBackgroundTypeEnum::Enum>("m_backgroundType",
                                                                                                    &m_geometry->m_backgroundType);
}

/**
 * Destructor.
 */
TileTabsBrowserTabGeometrySceneHelper::~TileTabsBrowserTabGeometrySceneHelper()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
TileTabsBrowserTabGeometrySceneHelper::toString() const
{
    return "TileTabsBrowserTabGeometrySceneHelper";
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
TileTabsBrowserTabGeometrySceneHelper::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "TileTabsBrowserTabGeometrySceneHelper",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
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
TileTabsBrowserTabGeometrySceneHelper::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    if ( ! sceneClass->getPrimitive("m_displayFlag")) {
        m_geometry->m_displayFlag = true;
    }
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * @return True if the geometry was restored from the scene, else false.
 */
bool
TileTabsBrowserTabGeometrySceneHelper::wasRestoredFromScene() const
{
    return m_wasRestoredFromSceneFlag;
}


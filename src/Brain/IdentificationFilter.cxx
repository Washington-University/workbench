
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

#define __IDENTIFICATION_FILTER_DECLARE__
#include "IdentificationFilter.h"
#undef __IDENTIFICATION_FILTER_DECLARE__

#include "CaretAssert.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;


    
/**
 * \class caret::IdentificationFilter 
 * \brief Filtering seledtions for identification operations
 * \ingroup Brain
 */

/**
 * Constructor.
 */
IdentificationFilter::IdentificationFilter()
: CaretObject()
{
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    m_sceneAssistant->add("m_surfaceVertexEnabled", &m_surfaceVertexEnabled);
    m_sceneAssistant->add("m_volumeVoxelEnabled", &m_volumeVoxelEnabled);
    m_sceneAssistant->add("m_borderEnabled", &m_borderEnabled);
    m_sceneAssistant->add("m_fociEnabled", &m_fociEnabled);
}

/**
 * Destructor.
 */
IdentificationFilter::~IdentificationFilter()
{
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationFilter::toString() const
{
    return "IdentificationFilter";
}

/**
 * @return show surface vertex information
 */
bool
IdentificationFilter::isSurfaceVertexEnabled() const
{
    return m_surfaceVertexEnabled;
}

/**
 * Set show surface vertex information
 *
 * @param surfaceVertexEnabled
 *    New value for show surface vertex information
 */
void
IdentificationFilter::setSurfaceVertexEnabled(const bool surfaceVertexEnabled)
{
    m_surfaceVertexEnabled = surfaceVertexEnabled;
}

/**
 * @return show volume voxel information
 */
bool
IdentificationFilter::isVolumeVoxelEnabled() const
{
    return m_volumeVoxelEnabled;
}

/**
 * Set show volume voxel information
 *
 * @param volumeVoxelEnabled
 *    New value for show volume voxel information
 */
void
IdentificationFilter::setVolumeVoxelEnabled(const bool volumeVoxelEnabled)
{
    m_volumeVoxelEnabled = volumeVoxelEnabled;
}

/**
 * @return show border information
 */
bool
IdentificationFilter::isBorderEnabled() const
{
    return m_borderEnabled;
}

/**
 * Set show border information
 *
 * @param borderEnabled
 *    New value for show border information
 */
void
IdentificationFilter::setBorderEnabled(const bool borderEnabled)
{
    m_borderEnabled = borderEnabled;
}

/**
 * @return show foci information
 */
bool
IdentificationFilter::isFociEnabled() const
{
    return m_fociEnabled;
}

/**
 * Set show foci information
 *
 * @param fociEnabled
 *    New value for show foci information
 */
void
IdentificationFilter::setFociEnabled(const bool fociEnabled)
{
    m_fociEnabled = fociEnabled;
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
IdentificationFilter::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentificationFilter",
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
IdentificationFilter::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}


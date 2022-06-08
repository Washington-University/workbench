
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
    m_sceneAssistant->add<IdentificationFilterTabSelectionEnum, IdentificationFilterTabSelectionEnum::Enum>("m_tabFiltering", &m_tabFiltering);
    m_sceneAssistant->add("m_showCiftiLoadingEnabled", &m_showCiftiLoadingEnabled);
    m_sceneAssistant->add("m_showBorderEnabled", &m_showBorderEnabled);
    m_sceneAssistant->add("m_showFociEnabled", &m_showFociEnabled);
    m_sceneAssistant->add("m_showVertexVoxelEnabled", &m_showVertexVoxelEnabled);
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
 * @return The tab filtering
 */
IdentificationFilterTabSelectionEnum::Enum
IdentificationFilter::getTabFiltering() const
{
    return m_tabFiltering;
}

/**
 * Set the tab filtering
 *
 * @param tabFiltering
 *  New tab filtering selection
 */
void
IdentificationFilter::setTabFiltering(const IdentificationFilterTabSelectionEnum::Enum tabFiltering)
{
    m_tabFiltering = tabFiltering;
}

/**
 * @return show CIFTI loading information
 */
bool
IdentificationFilter::isShowCiftiLoadingEnabled() const
{
    return m_showCiftiLoadingEnabled;
}

/**
 * Set show CIFTI loading information
 *
 * @param staus
 *    New  status
 */
void
IdentificationFilter::setShowCiftiLoadingEnabled(const bool status)
{
    m_showCiftiLoadingEnabled = status;
}

/**
 * @return show border information
 */
bool
IdentificationFilter::isShowBorderEnabled() const
{
    return m_showBorderEnabled;
}

/**
 * Set show border information
 *
 * @param staus
 *    New  status
 */
void
IdentificationFilter::setShowBorderEnabled(const bool status)
{
    m_showBorderEnabled = status;
}

/**
 * @return show foci information
 */
bool
IdentificationFilter::isShowFociEnabled() const
{
    return m_showFociEnabled;
}

/**
 * Set show foci information
 *
 * @param staus
 *    New  status
 */
void
IdentificationFilter::setShowFociEnabled(const bool status)
{
    m_showFociEnabled = status;
}

/**
 * @return show vertex/voxel information
 */
bool
IdentificationFilter::isShowVertexVoxelEnabled() const
{
    return m_showVertexVoxelEnabled;
}

/**
 * Set show vertex/voxel information
 *
 * @param staus
 *    New  status
 */
void
IdentificationFilter::setShowVertexVoxelEnabled(const bool status)
{
    m_showVertexVoxelEnabled = status;
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


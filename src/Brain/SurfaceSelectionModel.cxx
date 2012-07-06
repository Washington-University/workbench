
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <algorithm>

#define __SURFACE_SELECTION_MODEL_DECLARE__
#include "SurfaceSelectionModel.h"
#undef __SURFACE_SELECTION_MODEL_DECLARE__

#include "BrainStructure.h"
#include "EventManager.h"
#include "EventSurfacesGet.h"
#include "SceneAttributes.h"
#include "SceneClass.h"
#include "Surface.h"

using namespace caret;


    
/**
 * \class SurfaceSelection 
 * \brief Maintains selection of a surface.
 *
 * Maintains selection of a surface.  If the surface 
 * becomes invalid, a different surface will become selected.
 */

/**
 * Constructor for surfaces in any structure.
 */
SurfaceSelectionModel::SurfaceSelectionModel()
: CaretObject()
{
    m_mode = MODE_STRUCTURE;
}

/**
 * Constructor for surface from a specific structure.
 * @param structure
 *   Limit to surfaces from this structure.
 */
SurfaceSelectionModel::SurfaceSelectionModel(const StructureEnum::Enum structure)
: CaretObject()
{
    m_allowableStructures.push_back(structure);
    m_mode = MODE_STRUCTURE;
}

/**
 * Constructor for surface from a brain structure.
 * WARNING: If the brain structure becomes invalid, any further
 * use of this instance may cause a crash.
 *
 * @param brainStructure
 *   Only surfaces in brain structure are available.
 */
SurfaceSelectionModel::SurfaceSelectionModel(BrainStructure* brainStructure)
{
    m_brainStructure = brainStructure;
    m_mode = MODE_BRAIN_STRUCTURE;
}

/**
 * Constructor for surface of a specific type.
 * @param surfaceType
 *   Limit to surfaces from this of these types.
 */
SurfaceSelectionModel::SurfaceSelectionModel(const SurfaceTypeEnum::Enum surfaceType)
{
    m_allowableSurfaceTypes.push_back(surfaceType);
    m_mode = MODE_SURFACE_TYPE;
}

/**
 * Destructor.
 */
SurfaceSelectionModel::~SurfaceSelectionModel()
{
    
}

/**
 * @return The selected surface (NULL if none)
 */
Surface* 
SurfaceSelectionModel::getSurface()
{
    updateSelection();
    return m_selectedSurface;
}

/**
 * @return The selected surface (NULL if none)
 */
const Surface* 
SurfaceSelectionModel::getSurface() const
{
    updateSelection();
    return m_selectedSurface;
}

/**
 * Set the selected surface.
 * @param surface
 *   New seleted surface.
 */
void 
SurfaceSelectionModel::setSurface(Surface* surface)
{
    m_selectedSurface = surface;
}

/**
 * @return A vector containing surfaces available
 * for selection.
 */
std::vector<Surface*> 
SurfaceSelectionModel::getAvailableSurfaces() const
{
    std::vector<Surface*> surfaces;
    
    switch (m_mode) {
        case MODE_BRAIN_STRUCTURE:
        {
            const int32_t numSurfaces = m_brainStructure->getNumberOfSurfaces();
            for (int32_t i = 0; i < numSurfaces; i++) {
                surfaces.push_back(m_brainStructure->getSurface(i));
            }
        }
            break;
        case MODE_STRUCTURE:
        {
            EventSurfacesGet getSurfacesEvent;
            for (int32_t i = 0; i < static_cast<int32_t>(m_allowableStructures.size()); i++) {
                getSurfacesEvent.addStructureConstraint(m_allowableStructures[i]);
            }
            EventManager::get()->sendEvent(getSurfacesEvent.getPointer());
            surfaces = getSurfacesEvent.getSurfaces();
        }
            break;
        case MODE_SURFACE_TYPE:
        {
            EventSurfacesGet getSurfacesEvent;
            EventManager::get()->sendEvent(getSurfacesEvent.getPointer());
            std::vector<Surface*> allSurfaces = getSurfacesEvent.getSurfaces();
            for (std::vector<Surface*>::iterator iter = allSurfaces.begin();
                 iter != allSurfaces.end();
                 iter++) {
                Surface* s = *iter;
                const SurfaceTypeEnum::Enum surfaceType = s->getSurfaceType();
                if (std::find(m_allowableSurfaceTypes.begin(),
                              m_allowableSurfaceTypes.end(),
                              surfaceType) != m_allowableSurfaceTypes.end()) {
                    surfaces.push_back(s);
                }
            }
        }
            break;
            
    }
    

    return surfaces;
}

/**
 * Update the selected surface.
 */
void 
SurfaceSelectionModel::updateSelection() const
{
    std::vector<Surface*> surfaces = getAvailableSurfaces();
    
    if (m_selectedSurface != NULL) {
        if (std::find(surfaces.begin(),
                      surfaces.end(),
                      m_selectedSurface) == surfaces.end()) {
            m_selectedSurface = NULL;
        }
    }
    
    if (m_selectedSurface == NULL) {
        if (surfaces.empty() == false) {
            switch (m_mode) {
                case MODE_BRAIN_STRUCTURE:
                    m_selectedSurface = m_brainStructure->getVolumeInteractionSurface();
                    break;
                case MODE_STRUCTURE:
                    break;
                case MODE_SURFACE_TYPE:
                    break;
            }
            
            if (m_selectedSurface == NULL) {
                m_selectedSurface = surfaces[0];
            }
        }
    }
}

/**
 * Create a scene for an instance of a class.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of the class' instance.
 *
 * @return Pointer to SceneClass object representing the state of 
 *    this object.  Under some circumstances a NULL pointer may be
 *    returned.  Caller will take ownership of returned object.
 */
SceneClass* 
SurfaceSelectionModel::saveToScene(const SceneAttributes* /*sceneAttributes*/,
                                  const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "SurfaceSelectionModel",
                                            1);
    
    Surface* surface = getSurface();
    if (surface != NULL) {
        sceneClass->addString("m_selectedSurface",
                              surface->getFileNameNoPath());
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
 *     sceneClass for the instance of a class that implements
 *     this interface.  May be NULL for some types of scenes.
 */
void 
SurfaceSelectionModel::restoreFromScene(const SceneAttributes* /*sceneAttributes*/,
                                       const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const AString& surfaceFileName = sceneClass->getStringValue("m_selectedSurface",
                                                                "");
    if (surfaceFileName.isEmpty() == false) {
        std::vector<Surface*> surfaces = getAvailableSurfaces();
        
        for (std::vector<Surface*>::iterator iter = surfaces.begin();
             iter != surfaces.end();
             iter++) {
            Surface* s = *iter;
            if (s->getFileNameNoPath() == surfaceFileName) {
                setSurface(s);
                break;
            }
        }
        
    }
}




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

#include <algorithm>

#define __SURFACE_SELECTION_MODEL_DECLARE__
#include "SurfaceSelectionModel.h"
#undef __SURFACE_SELECTION_MODEL_DECLARE__

#include "BrainStructure.h"
#include "EventBrainStructureGetAll.h"
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
 * Maintains selection of a surface.  
 *
 * The constructors allow one to limit that available surfaces to those
 * from specified structures and surface types.
 *
 * If the selected surface becomes invalid, a different surface will 
 * be selected.
 */

/**
 * Constructor for surfaces from a specific structure and of the given
 * surface types.
 *
 * @param structure
 *   Limit to surfaces from this structure.
 * @param surfaceTypes
 *   Types of surfaces that are available for selection.
 */
SurfaceSelectionModel::SurfaceSelectionModel(const StructureEnum::Enum structure,
                                             const std::vector<SurfaceTypeEnum::Enum>& surfaceTypes)
: CaretObject()
{
    m_allowableStructures.push_back(structure);
    m_allowableSurfaceTypes = surfaceTypes;
    CaretAssert( ! surfaceTypes.empty());
}

/**
 * Constructor for surfaces from any structure and of the given
 * surface types.
 *
 * @param surfaceTypes
 *   Types of surfaces that are available for selection.
 */
SurfaceSelectionModel::SurfaceSelectionModel(const std::vector<SurfaceTypeEnum::Enum>& surfaceTypes)
: CaretObject()
{
    m_allowableSurfaceTypes = surfaceTypes;
    CaretAssert( ! surfaceTypes.empty());
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
    updateModel();
    return m_selectedSurface;
}

/**
 * @return The selected surface (NULL if none)
 */
const Surface* 
SurfaceSelectionModel::getSurface() const
{
    updateModel();
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
 * Set the selected surface to a surface of any of the given types with
 * first type having highest priority and last type having lowest priority.
 *
 * @param surfaceType
 *    Highest priority type.
 * @param surfaceType2
 *    Second priority type.
 * @param surfaceType3
 *    Third priority type.
 * @param surfaceType4
 *    Fourth priority type.
 * @param surfaceType5
 *    Lowest priority type.
 */
void
SurfaceSelectionModel::setSurfaceToType(const SurfaceTypeEnum::Enum surfaceType,
                                        const SurfaceTypeEnum::Enum surfaceType2,
                                        const SurfaceTypeEnum::Enum surfaceType3,
                                        const SurfaceTypeEnum::Enum surfaceType4,
                                        const SurfaceTypeEnum::Enum surfaceType5)
{
    std::vector<Surface*> surfaces = getAvailableSurfaces();

    std::vector<SurfaceTypeEnum::Enum> surfaceTypes;
    surfaceTypes.push_back(surfaceType);
    if (surfaceType2 != SurfaceTypeEnum::UNKNOWN) {
        surfaceTypes.push_back(surfaceType2);
    }
    if (surfaceType3 != SurfaceTypeEnum::UNKNOWN) {
        surfaceTypes.push_back(surfaceType3);
    }
    if (surfaceType4 != SurfaceTypeEnum::UNKNOWN) {
        surfaceTypes.push_back(surfaceType4);
    }
    if (surfaceType5 != SurfaceTypeEnum::UNKNOWN) {
        surfaceTypes.push_back(surfaceType5);
    }
    
    for (std::vector<SurfaceTypeEnum::Enum>::iterator typeIter = surfaceTypes.begin();
         typeIter != surfaceTypes.end();
         typeIter++) {
        const SurfaceTypeEnum::Enum type = *typeIter;
        for (std::vector<Surface*>::iterator surfaceIter = surfaces.begin();
             surfaceIter != surfaces.end();
             surfaceIter++) {
            Surface* s = *surfaceIter;
            if (s->getSurfaceType() == type) {
                setSurface(s);
                return;
            }
        }
    }
}

/**
 * @return A vector containing surfaces available
 * for selection.
 */
std::vector<Surface*> 
SurfaceSelectionModel::getAvailableSurfaces() const
{
    std::vector<Surface*> unknownSurfaces;
    std::vector<Surface*> reconstructionSurfaces;
    std::vector<Surface*> anatomicalSurfaces;
    std::vector<Surface*> inflatedSurfaces;
    std::vector<Surface*> veryInflatedSurfaces;
    std::vector<Surface*> sphericalSurfaces;
    std::vector<Surface*> semiSphericalSurfaces;
    std::vector<Surface*> ellipsoidSurfaces;
    std::vector<Surface*> flatSurfaces;
    std::vector<Surface*> hullSurfaces;
    
    /*
     * Get ALL surfaces
     */
    EventSurfacesGet getSurfacesEvent;
    EventManager::get()->sendEvent(getSurfacesEvent.getPointer());
    std::vector<Surface*> allSurfaces = getSurfacesEvent.getSurfaces();
    
    for (std::vector<Surface*>::iterator iter = allSurfaces.begin();
         iter != allSurfaces.end();
         iter++) {
        Surface* surface = *iter;
        
        /*
         * Filter by structure
         */
        bool passesStructureTestFlag = false;
        if (m_allowableStructures.empty()) {
            passesStructureTestFlag = true;
        }
        else {
            const StructureEnum::Enum structure = surface->getStructure();
            if (std::find(m_allowableStructures.begin(),
                          m_allowableStructures.end(),
                          structure) != m_allowableStructures.end()) {
                passesStructureTestFlag = true;
            }
                          
        }
        
        if (passesStructureTestFlag) {
            const SurfaceTypeEnum::Enum surfaceType = surface->getSurfaceType();
            if (std::find(m_allowableSurfaceTypes.begin(),
                          m_allowableSurfaceTypes.end(),
                          surfaceType) != m_allowableSurfaceTypes.end()) {
                switch (surfaceType) {
                    case SurfaceTypeEnum::UNKNOWN:
                        unknownSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::RECONSTRUCTION:
                        reconstructionSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::ANATOMICAL:
                        anatomicalSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::INFLATED:
                        inflatedSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::VERY_INFLATED:
                        veryInflatedSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::SPHERICAL:
                        sphericalSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::SEMI_SPHERICAL:
                        semiSphericalSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::ELLIPSOID:
                        ellipsoidSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::FLAT:
                        flatSurfaces.push_back(surface);
                        break;
                    case SurfaceTypeEnum::HULL:
                        hullSurfaces.push_back(surface);
                        break;
                }
            }
        }
    }

    std::vector<Surface*> surfacesOut;
    
    surfacesOut.insert(surfacesOut.end(),
                    anatomicalSurfaces.begin(),
                    anatomicalSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    reconstructionSurfaces.begin(),
                    reconstructionSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    inflatedSurfaces.begin(),
                    inflatedSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    veryInflatedSurfaces.begin(),
                    veryInflatedSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    sphericalSurfaces.begin(),
                    sphericalSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    semiSphericalSurfaces.begin(),
                    semiSphericalSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    ellipsoidSurfaces.begin(),
                    ellipsoidSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    hullSurfaces.begin(),
                    hullSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    flatSurfaces.begin(),
                    flatSurfaces.end());
    surfacesOut.insert(surfacesOut.end(),
                    unknownSurfaces.begin(),
                    unknownSurfaces.end());
    
    return surfacesOut;
}

/**
 * Update the model.
 * May be needed if the loaded surfaces change.
 */
void
SurfaceSelectionModel::updateModel() const
{
    std::vector<Surface*> surfaces = getAvailableSurfaces();
    
    if (surfaces.empty()) {
        m_selectedSurface = NULL;
        return;
    }
    
    if (m_selectedSurface != NULL) {
        if (std::find(surfaces.begin(),
                      surfaces.end(),
                      m_selectedSurface) == surfaces.end()) {
            m_selectedSurface = NULL;
        }
    }
    
    if (m_selectedSurface == NULL) {
        EventBrainStructureGetAll brainStructureEvent;
        EventManager::get()->sendEvent(brainStructureEvent.getPointer());
        
        const int32_t numBrainStructures = brainStructureEvent.getNumberOfBrainStructures();
        for (int32_t i = 0; i < numBrainStructures; i++) {
            BrainStructure* bs = brainStructureEvent.getBrainStructureByIndex(i);
            
            /*
             * Use the primary anatomical surface if it is acceptable
             */
            Surface* primaryAnatomicalSurface = NULL;
            
            if (m_allowableStructures.empty()) {
                primaryAnatomicalSurface = bs->getPrimaryAnatomicalSurface();
            }
            else {
                const StructureEnum::Enum structure = bs->getStructure();
                if (std::find(m_allowableStructures.begin(),
                              m_allowableStructures.end(),
                              structure) != m_allowableStructures.end()) {
                    primaryAnatomicalSurface = bs->getPrimaryAnatomicalSurface();
                    break;
                }
            }
            
            if (primaryAnatomicalSurface != NULL) {
                if (std::find(surfaces.begin(),
                              surfaces.end(),
                              primaryAnatomicalSurface) != surfaces.end()) {
                    m_selectedSurface = primaryAnatomicalSurface;
                    break;
                }
            }
        }
        
        if (m_selectedSurface == NULL) {
            m_selectedSurface = surfaces[0];
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
        sceneClass->addPathName("m_selectedSurfacePathName",
                                surface->getFileName());
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
    
    std::vector<Surface*> allSurfaces = getAvailableSurfaces();
    
    /*
     * Element "m_selectedSurfacePathName" replaces older elements
     * and fixes problem with absolute paths in the scene file
     */
    const AString surfacePathName = sceneClass->getPathNameValue("m_selectedSurfacePathName");
    Surface* pathNameMatchSurface = NULL;
    Surface* nameMatchSurface = NULL;
    if ( ! surfacePathName.isEmpty()) {
        for (auto surface : allSurfaces) {
            if (surface->getFileName() == surfacePathName) {
                pathNameMatchSurface = surface;
                break;
            }
        }
    }
    else {
        /*
         * For full path, find the best match using the right-most characters that
         * will contain any relative path.  When scene files are moved to different
         * computers the full path may change the parts of the path nearest the
         * name of the file will match.
         */
        const AString surfaceFileNameFullPath = sceneClass->getStringValue("m_selectedSurfaceFullPath",
                                                                           "");
        int32_t pathNameMatchLength = 0;
        if ( ! surfaceFileNameFullPath.isEmpty()) {
            for (auto surface : allSurfaces) {
                const AString name = surface->getFileName();
                const int32_t numMatch = name.countMatchingCharactersFromEnd(surfaceFileNameFullPath);
                if (numMatch > pathNameMatchLength) {
                    pathNameMatchLength  = numMatch;
                    pathNameMatchSurface = surface;
                }
            }
        }
        
        /*
         * Match name of file with NO path
         * Always restore this so that the object is marked as restored
         * (within the 'get' method).  Otherwise if compiled debug, this
         * object will get logged as 'not restored'.
         */
        const AString surfaceFileName = sceneClass->getStringValue("m_selectedSurface",
                                                                   "");
        if ( ! surfaceFileName.isEmpty()) {
            for (auto surface : allSurfaces) {
                if (surface->getFileNameNoPath() == surfaceFileName) {
                    nameMatchSurface = surface;
                    break;
                }
            }
        }
    }
    
    if (pathNameMatchSurface != NULL) {
        setSurface(pathNameMatchSurface);
    }
    else if (nameMatchSurface != NULL) {
        setSurface(nameMatchSurface);
    }
    else {
        setSurface(NULL);
    }
}



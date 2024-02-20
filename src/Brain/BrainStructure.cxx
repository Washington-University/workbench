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
#include <limits>


#define __BRAIN_STRUCTURE_DEFINE__
#include "BrainStructure.h"
#undef __BRAIN_STRUCTURE_DEFINE__
#include "Brain.h"
#include "BrainStructureNodeAttributes.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPointLocator.h"
#include "CaretPreferences.h"
#include "DataFileException.h"
#include "ElapsedTimer.h"
#include "EventBrainStructureGetAll.h"
#include "EventManager.h"
#include "EventNodeDataFilesGet.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventSurfacesGet.h"
#include "EventSurfaceStructuresValidGet.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentificationManager.h"
#include "SelectionManager.h"
#include "LabelFile.h"
#include "MathFunctions.h"
#include "MetricDynamicConnectivityFile.h"
#include "MetricFile.h"
#include "ModelSurface.h"
#include "OverlaySet.h"
#include "OverlaySetArray.h"
#include "RgbaFile.h"
#include "SceneClass.h"
#include "ScenePathName.h"
#include "SessionManager.h"
#include "Surface.h"

using namespace caret;

/**
 * Constructor.
 * 
 */
BrainStructure::BrainStructure(Brain* brain,
                               StructureEnum::Enum structure)
{
    m_brainStructureIdentifier = BrainStructure::s_brainStructureIdentifierCounter++;
    
    m_brain = brain;
    m_structure = structure;
    m_nodeAttributes = new BrainStructureNodeAttributes();
    m_primaryAnatomicalSurface = NULL;
    
    std::vector<StructureEnum::Enum> overlaySurfaceStructures;
    overlaySurfaceStructures.push_back(m_structure);

    m_overlaySetArray = new OverlaySetArray(overlaySurfaceStructures,
                                            Overlay::INCLUDE_VOLUME_FILES_NO,
                                            "Structure " + StructureEnum::toGuiName(m_structure));
    
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_BRAIN_STRUCTURE_GET_ALL);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_GET_NODE_DATA_FILES);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_SURFACES_GET);
    EventManager::get()->addEventListener(this,
                                          EventTypeEnum::EVENT_SURFACE_STRUCTURES_VALID_GET);
}

/**
 * Destructor.
 */
BrainStructure::~BrainStructure()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_overlaySetArray;
    
    /*
     * Make a copy of all surface pointers since
     * deleting surfaces will alter the actual
     * vector that stores the surfaces.
     */
    std::vector<Surface*> allSurfaces(m_surfaces);
    
    for (uint64_t i = 0; i < allSurfaces.size(); i++) {
        removeAndMaybeDeleteSurface(allSurfaces[i],
                      true);
    }
    m_surfaces.clear();
    
    for (uint64_t i = 0; i < m_labelFiles.size(); i++) {
        delete m_labelFiles[i];
        m_labelFiles[i] = NULL;
    }
    m_labelFiles.clear();
    
    for (uint64_t i = 0; i < m_metricFiles.size(); i++) {
        delete m_metricFiles[i];
        m_metricFiles[i] = NULL;
    }
    m_metricFiles.clear();
    
    for (uint64_t i = 0; i < m_rgbaFiles.size(); i++) {
        delete m_rgbaFiles[i];
        m_rgbaFiles[i] = NULL;
    }
    m_rgbaFiles.clear();

    delete m_nodeAttributes;
}

/**
 * Get the structure for this BrainStructure.
 *
 * @return The structure.
 */
StructureEnum::Enum 
BrainStructure::getStructure() const
{
    return m_structure;
}

/**
 * Add a label file.
 *
 * @param labelFile
 *    Label file that is added.
 * @param addFileToBrainStructure
 *    If true, add the file to the brain structure.  This value is false
 *    when a file is reloaded and already part of the brain structure.
 * @throw DataFileException
 *    If the number of nodes in the label file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addLabelFile(LabelFile* labelFile,
                             const bool addFileToBrainStructure)
{
    CaretAssert(labelFile);
    
    int32_t numNodes = getNumberOfNodes();
    if (numNodes > 0) {
        if (labelFile->getNumberOfNodes() != numNodes) {
            AString message = ("Label file contains "
                               + AString::number(labelFile->getNumberOfNodes())
                               + " vertices but the "
                               + StructureEnum::toGuiName(getStructure())
                               + " contains "
                               + AString::number(numNodes)
                               + " vertices.");
            
            DataFileException e(labelFile->getFileName(),
                                message);
            CaretLogThrowing(e);

            throw e;
        }
    }
        
    if (labelFile->getStructure() != getStructure()) {
        AString message = ("Trying to add label file with structure \""
                           + StructureEnum::toGuiName(labelFile->getStructure())
                           + " to BrainStructure for \""
                           + StructureEnum::toGuiName(getStructure())
                           + "\n");
        DataFileException e(labelFile->getFileName(),
                            message);
        CaretLogThrowing(e);

        throw e;
    }
    
    if (addFileToBrainStructure) {
        m_labelFiles.push_back(labelFile);
    }
}

/**
 * Add a metric file.
 *
 * @param metricFile
 *    Metric file that is added.
 * @param addFileToBrainStructure
 *    If true, add the file to the brain structure.  This value is false
 *    when a file is reloaded and already part of the brain structure.
 * @throw DataFileException
 *    If the number of nodes in the metric file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addMetricFile(MetricFile* metricFile,
                              const bool addFileToBrainStructure)
{
    CaretAssert(metricFile);
    
    int32_t numNodes = getNumberOfNodes();
    if (numNodes > 0) {
        if (metricFile->getNumberOfNodes() != numNodes) {
            AString message = ("Metric file contains "
                               + AString::number(metricFile->getNumberOfNodes())
                               + " vertices but the "
                               + StructureEnum::toGuiName(getStructure())
                               + " contains "
                               + AString::number(numNodes)
                               + " vertices.");
            
            DataFileException e(metricFile->getFileName(),
                                message);
            CaretLogThrowing(e);
            
            throw e;
        }
    }
    
    if (metricFile->getStructure() != getStructure()) {
        AString message = ("Trying to add metric file with structure \""
                           + StructureEnum::toGuiName(metricFile->getStructure())
                           + " to BrainStructure for \""
                           + StructureEnum::toGuiName(getStructure())
                           + "\n");
        DataFileException e(metricFile->getFileName(),
                            message);
        CaretLogThrowing(e);
                
        throw e;
    }
    
    if (addFileToBrainStructure) {
        m_metricFiles.push_back(metricFile);

        /*
         * Enable dynamic connectivity using preferences
         */
        CaretPreferences* prefs = SessionManager::get()->getCaretPreferences();
        MetricDynamicConnectivityFile* metricDynConn = metricFile->getMetricDynamicConnectivityFile();
        if (metricDynConn != NULL) {
            metricDynConn->setEnabledAsLayer(prefs->isDynamicConnectivityDefaultedOn());
        }
    }
}

/**
 * Add an RGBA file.
 *
 * @param rgbaFile
 *    RGBA file that is added.
 * @param addFileToBrainStructure
 *    If true, add the file to the brain structure.  This value is false
 *    when a file is reloaded and already part of the brain structure.
 * @throw DataFileException
 *    If the number of nodes in the RGBA file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addRgbaFile(RgbaFile* rgbaFile,
                            const bool addFileToBrainStructure)
{
    CaretAssert(rgbaFile);
    
    int32_t numNodes = getNumberOfNodes();
    if (numNodes > 0) {
        if (rgbaFile->getNumberOfNodes() != numNodes) {
            AString message = ("RGBA File contains "
                               + AString::number(rgbaFile->getNumberOfNodes())
                               + " vertices but the "
                               + StructureEnum::toGuiName(getStructure())
                               + " contains "
                               + AString::number(numNodes)
                               + " vertices.");
            
            DataFileException e(rgbaFile->getFileName(),
                                message);
            CaretLogThrowing(e);
            
            throw e;
        }
    }
    
    
    if (rgbaFile->getStructure() != getStructure()) {
        AString message = ("Trying to add metric file with structure \""
                           + StructureEnum::toGuiName(rgbaFile->getStructure())
                           + " to BrainStructure for \""
                           + StructureEnum::toGuiName(getStructure())
                           + "\n");
        DataFileException e(rgbaFile->getFileName(),
                            message);
        CaretLogThrowing(e);

        throw e;
    }
    
    if (addFileToBrainStructure) {
        m_rgbaFiles.push_back(rgbaFile);
    }
}

/**
 * Add a surface.
 *
 * @param surface
 *    Surface that is added.
 * @param addFileToBrainStructure
 *    If true, add the file to the brain structure.  This value is false
 *    when a file is reloaded and already part of the brain structure.
 * @throw DataFileException
 *    If the number of nodes in the surface does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addSurface(Surface* surface,
                           const bool addFileToBrainStructure,
                           const bool initilizeOverlaysFlag)
{
    CaretAssert(surface);
    
    int32_t numNodes = getNumberOfNodes();
    if (numNodes > 0) {
        if (surface->getNumberOfNodes() != numNodes) {
            AString message = ("Surface file contains "
                               + AString::number(surface->getNumberOfNodes())
                               + " vertices but the "
                               + StructureEnum::toGuiName(getStructure())
                               + " contains "
                               + AString::number(numNodes)
                               + " vertices.");
            
            DataFileException e(surface->getFileName(),
                                message);
            CaretLogThrowing(e);
            
            throw e;
        }
    }
    
    if (surface->getStructure() != getStructure()) {
        AString message = ("Trying to add surface file with structure \""
                           + StructureEnum::toGuiName(surface->getStructure())
                           + " to BrainStructure for \""
                           + StructureEnum::toGuiName(getStructure())
                           + "\n");
        DataFileException e(surface->getFileName(),
                            message);
        CaretLogThrowing(e);

        throw e;
    }
    if (numNodes == 0) {
        const int32_t numSurfaceNodes = surface->getNumberOfNodes();
        m_nodeAttributes->update(numSurfaceNodes);
    }
    
    surface->setBrainStructure(this);
    
    if (addFileToBrainStructure) {
        m_surfaces.push_back(surface);
        
        /*
         * Create a model for the surface.
         */
        ModelSurface* mdcs = new ModelSurface(m_brain,
                                              surface);
        m_surfaceModelMap.insert(std::make_pair(surface, mdcs));
        
        if (initilizeOverlaysFlag) {
            initializeOverlays();
        }
        
        /*
         * Send the model added event.
         */
        EventModelAdd addEvent(mdcs);
        EventManager::get()->sendEvent(addEvent.getPointer());
    }
}

/**
 * Remove a surface from this brain structure and the surface
 * model and maybe delete the surface.
 * 
 * @param surface
 *    Surface that is removed.
 * @param deleteSurfaceFile
 *    If true, delete the surface file.  If false, surface is removed 
 *    but not delete and caller is responsible for deleting the surface.
 * @return
 *    True if the surface was removed, else false.
 */
bool
BrainStructure::removeAndMaybeDeleteSurface(Surface* surface,
                              const bool deleteSurfaceFile)
{
    CaretAssert(surface);
    
    std::vector<Surface*>::iterator iter =
    std::find(m_surfaces.begin(),
              m_surfaces.end(),
              surface);
    
    CaretAssertMessage((iter != m_surfaces.end()),
                       "Trying to delete surface not in brain structure.");
    if (iter == m_surfaces.end()) {
        CaretLogSevere("Trying to delete surface not in brain structure.");
        return false;
    }
    
    std::map<Surface*, ModelSurface*>::iterator modelIter =
        m_surfaceModelMap.find(surface);

    CaretAssertMessage((modelIter != m_surfaceModelMap.end()),
                       "Surface does not map to a model");
    if (modelIter == m_surfaceModelMap.end()) {
        CaretLogSevere("Surface does not map to a model");
        return false;
    }

    ModelSurface* mdcs = modelIter->second;
    
    /*
     * Remove from surface to model map.
     */
    m_surfaceModelMap.erase(modelIter);
    
    /*
     * Remove the surface.
     */
    m_surfaces.erase(iter);
    
    /*
     * Send the model deleted event.
     */
    EventModelDelete deleteEvent(mdcs);
    EventManager::get()->sendEvent(deleteEvent.getPointer());
    
    /*
     * Delete the model and the surface.
     */
    delete mdcs;
    
    if (deleteSurfaceFile) {
        delete surface;
    }
    
    return true;
}

/**
 * Get the number of surfaces.
 *
 * @return
 *    Number of surfaces.
 */
int 
BrainStructure::getNumberOfSurfaces() const
{
    return static_cast<int>(m_surfaces.size());
}

/**
 * Get a surface at the specified index.
 * 
 * @param indx
 *    Index of surface.
 * @return 
 *    Surface at the specified index.
 */
Surface* 
BrainStructure::getSurface(int indx)
{
    CaretAssertVectorIndex(m_surfaces, indx);
    
    return m_surfaces[indx];
}

/**
 * Get a surface at the specified index.
 *
 * @param indx
 *    Index of surface.
 * @return
 *    Surface at the specified index.
 */
const Surface*
BrainStructure::getSurface(int indx) const
{
    CaretAssertVectorIndex(m_surfaces, indx);
    
    return m_surfaces[indx];
}

/**
 * Get all surfaces of the given type in this brain structure.
 * @param surfaceType
 *   Type of surface
 * @param surfacesOut
 *   Output that will contain the surfaces.
 */
void 
BrainStructure::getSurfacesOfType(const SurfaceTypeEnum::Enum surfaceType,
                                  std::vector<Surface*>& surfacesOut) const
{
    surfacesOut.clear();
    
    const int32_t numSurfaces = getNumberOfSurfaces();
    for (int32_t i = 0; i < numSurfaces; i++) {
        if (m_surfaces[i]->getSurfaceType() == surfaceType) {
            surfacesOut.push_back(m_surfaces[i]);
        }
    }
}

/**
 * Get all surfaces.
 *
 * @param surfaceOut
 *    Output containing all surfaces.
 */
void
BrainStructure::getSurfaces(std::vector<Surface*>& surfacesOut) const
{
    surfacesOut = m_surfaces;
}

/**
 * @return The surface used for primary anatomical.
 * Returns NULL if no anatomical surfaces.
 */
const Surface* 
BrainStructure::getPrimaryAnatomicalSurfacePrivate() const
{
    bool valid = false;
    if (m_primaryAnatomicalSurface != NULL) {
        const int32_t numSurfaces = getNumberOfSurfaces();
        for (int32_t i = 0; i < numSurfaces; i++) {
            if (m_surfaces[i] == m_primaryAnatomicalSurface) {
                valid = true;
                break;
            }
        }
    }
    if (valid) {
        return m_primaryAnatomicalSurface;
    }
    m_primaryAnatomicalSurface = NULL;
    
    /*
     * Give preference to anatomical surfaces but if there are none
     * (perhaps the surface types are missing), use all surfaces.
     */
    std::vector<Surface*> primaryAnatomicalSurfaces;
    getSurfacesOfType(SurfaceTypeEnum::ANATOMICAL, 
                            primaryAnatomicalSurfaces);
    if (primaryAnatomicalSurfaces.empty()) {
        primaryAnatomicalSurfaces = m_surfaces;
    }
    
    
    if (primaryAnatomicalSurfaces.empty() == false) {
        /*
         * Default to first surface
         */
        m_primaryAnatomicalSurface = primaryAnatomicalSurfaces[0];
        
        /*
         * Now look for a surface with certain strings in their name
         */
        Surface* midThicknessSurface = NULL;
        Surface* whiteMatterSurface  = NULL;
        Surface* pialSurface         = NULL;
        Surface* anatomicalSurface   = NULL;
        Surface* fiducialSurface     = NULL;
        const int32_t numSurfaces = static_cast<int32_t>(primaryAnatomicalSurfaces.size());
        
        for (int32_t i = 0; i < numSurfaces; i++) {
            /*
             * First, look for anatomical surfaces that are midthickness,
             * gray/white, and pial.
             */
            if (primaryAnatomicalSurfaces[i]->getSurfaceType() == SurfaceTypeEnum::ANATOMICAL) {
                const SecondarySurfaceTypeEnum::Enum secondType = primaryAnatomicalSurfaces[i]->getSecondaryType();
                const AString name = primaryAnatomicalSurfaces[i]->getFileNameNoPath().toLower();
                if (secondType == SecondarySurfaceTypeEnum::MIDTHICKNESS) {
                    if (midThicknessSurface == NULL) {
                        midThicknessSurface = primaryAnatomicalSurfaces[i];
                    }
                }
                if (secondType == SecondarySurfaceTypeEnum::GRAY_WHITE) {
                    if (whiteMatterSurface == NULL) {
                        whiteMatterSurface = primaryAnatomicalSurfaces[i];
                    }
                }
                if (secondType == SecondarySurfaceTypeEnum::PIAL) {
                    if (pialSurface == NULL) {
                        pialSurface = primaryAnatomicalSurfaces[i];
                    }
                }
            }
        }
        
        /*
         * Since it is possible surfaces may not have valid types,
         * perform an additional search using name substrings.
         */
        for (int32_t i = 0; i < numSurfaces; i++) {
            const AString name = primaryAnatomicalSurfaces[i]->getFileNameNoPath().toLower();
            if (name.indexOf("midthick") >= 0) {
                if (midThicknessSurface == NULL) {
                    midThicknessSurface = primaryAnatomicalSurfaces[i];
                }
            }
            if (name.indexOf("white") >= 0) {
                if (whiteMatterSurface == NULL) {
                    whiteMatterSurface = primaryAnatomicalSurfaces[i];
                }
            }
            if (name.indexOf("pial") >= 0) {
                if (pialSurface == NULL) {
                    pialSurface = primaryAnatomicalSurfaces[i];
                }
            }
            if (name.indexOf("anatomical") >= 0) {
                if (anatomicalSurface == NULL) {
                    anatomicalSurface = primaryAnatomicalSurfaces[i];
                }
            }
            if (name.indexOf("fiducial") >= 0) {
                if (fiducialSurface == NULL) {
                    fiducialSurface = primaryAnatomicalSurfaces[i];
                }
            }
        }
        
        if (midThicknessSurface != NULL) {
            m_primaryAnatomicalSurface = midThicknessSurface;
        }
        else if (whiteMatterSurface != NULL) {
            m_primaryAnatomicalSurface = whiteMatterSurface;
        }
        else if (pialSurface != NULL) {
            m_primaryAnatomicalSurface = pialSurface;
        }
        else if (anatomicalSurface != NULL) {
            m_primaryAnatomicalSurface = anatomicalSurface;
        }
        else if (fiducialSurface != NULL) {
            m_primaryAnatomicalSurface = fiducialSurface;
        }
    }
    
    if (m_primaryAnatomicalSurface != NULL) {
        CaretLogFiner("Primary Anatomical Surface for "
                      + StructureEnum::toGuiName(m_structure)
                      + ": " 
                      + m_primaryAnatomicalSurface->getFileNameNoPath());
    }
    else {
        CaretLogFiner("Primary Anatomical Surface for "
                      + StructureEnum::toGuiName(m_structure)
                      + " is invalid.");
    }
    
    return m_primaryAnatomicalSurface;
}

/**
 * @return The surface used for primary anatomical.
 * Returns NULL if no anatomical surfaces.
 */
const Surface* 
BrainStructure::getPrimaryAnatomicalSurface() const
{
    return getPrimaryAnatomicalSurfacePrivate();
}

/**
 * @return The surface used for primary anatomical.
 * Returns NULL if no anatomical surfaces.
 */
Surface* 
BrainStructure::getPrimaryAnatomicalSurface()
{
    /*
     * Kludge to avoid duplicated code and ease maintenance
     */
    const Surface* constSurface = getPrimaryAnatomicalSurfacePrivate();
    Surface* s = (Surface*)constSurface;
    return s;
}

/**
 * Set the primary anatomical surface.
 * @param primaryAnatomicalSurface
 *    New primary anatomical surface.
 */
void 
BrainStructure::setPrimaryAnatomicalSurface(Surface* primaryAnatomicalSurface)
{
    m_primaryAnatomicalSurface = primaryAnatomicalSurface;
}

/**
 * Find and return the first surface encountered that contains the
 * given text in the name of the surface's filename.  Text is searched
 * in an case-insensitive mode.
 *
 * @param text
 *   Text that is to bound in the surface's filenname.
 * @return
 *   Surface that contains the given text in its filename or 
 *   NULL if no surface matches.
 */
Surface* 
BrainStructure::getSurfaceContainingTextInName(const AString& text)
{
    /*
     * Kludge to avoid duplicated code and ease maintenance
     */
    const Surface* constSurface = getSurfaceContainingTextInNamePrivate(text);
    Surface* s = (Surface*)constSurface;
    return s;
}


/**
 * Find and return the first surface encountered that contains the
 * given text in the name of the surface's filename.  Text is searched
 * in an case-insensitive mode.
 *
 * @param text
 *   Text that is to bound in the surface's filenname.
 * @return
 *   Surface that contains the given text in its filename or 
 *   NULL if no surface matches.
 */
const Surface* 
BrainStructure::getSurfaceContainingTextInName(const AString& text) const
{
    return getSurfaceContainingTextInNamePrivate(text);
}

/**
 * Find and return the first surface encountered that contains the
 * given text in the name of the surface's filename.  Text is searched
 * in an case-insensitive mode.
 *
 * @param text
 *   Text that is to bound in the surface's filenname.
 * @return
 *   Surface that contains the given text in its filename or 
 *   NULL if no surface matches.
 */
const Surface* 
BrainStructure::getSurfaceContainingTextInNamePrivate(const AString& text) const
{
    for (std::vector<Surface*>::const_iterator iter = m_surfaces.begin();
         iter != m_surfaces.end();
         iter++) {
        const Surface* surface = *iter;
        const AString name = surface->getFileNameNoPath();
        if (name.indexOf(text,
                         0,
                         Qt::CaseInsensitive) >= 0) {
            return surface;
        }
    }
    
    return NULL;
}

/**
 * Is the surface in this brain structure?
 * @param surface
 *   Surface that is tested for being in this brain structure.
 * @return Returns true if surface in brain structure, else false.
 */
bool 
BrainStructure::containsSurface(const Surface* surface)
{
    CaretAssert(surface);
    if (std::find(m_surfaces.begin(),
                  m_surfaces.end(),
                  surface) != m_surfaces.end()) {
        return true;
    }
    return false;
}

/**
 * Find the surface with the given name.
 * @param surfaceFileName
 *    Name of surface.
 * @param useAbsolutePath
 *    If true the given surfaceFileName is an absolute path.
 *    If false, the given surfaceFileName is just the file 
 *    name without any path.
 */
Surface*
BrainStructure::getSurfaceWithName(const AString& surfaceFileName,
                                   const bool useAbsolutePath)
{
    for (std::vector<Surface*>::iterator iter = m_surfaces.begin();
         iter != m_surfaces.end();
         iter++) {
        Surface* surface = *iter;
        const AString name = (useAbsolutePath
                              ? surface->getFileName()
                              : surface->getFileNameNoPath());
        if (surfaceFileName == name) {
            return surface;
        }
    }
    
    return NULL;
}

/**
 * Get the brain that this brain structure is in.
 */
Brain* 
BrainStructure::getBrain()
{
    return m_brain;    
}

/**
 * Get the brain that this brain structure is in.
 */
const Brain* 
BrainStructure::getBrain() const
{
    return m_brain;    
}

/**
 * Get the number of nodes used by this brain structure.
 *
 * @return Number of nodes.
 */
int32_t 
BrainStructure::getNumberOfNodes() const
{
    if (m_surfaces.empty() == false) {
        return m_surfaces[0]->getNumberOfNodes();
    }
    return 0;
}

/**
 * Get all of the label files.
 * @param labelFilesOut
 *    Will contain all label files after this method exits.
 */
void 
BrainStructure::getLabelFiles(std::vector<LabelFile*>& labelFilesOut) const
{
    labelFilesOut.clear();
    labelFilesOut.insert(labelFilesOut.end(),
                          m_labelFiles.begin(),
                          m_labelFiles.end());
}

/**
 * Get the number of label files.
 * @return Number of label files.
 */
int32_t 
BrainStructure::getNumberOfLabelFiles() const
{
    return m_labelFiles.size();
}

/**
 * Get a label file at the specified index.
 * @param fileIndex
 *    Index of the label file.
 * @return
 *    Metric file at the index.
 */
LabelFile* 
BrainStructure::getLabelFile(const int32_t fileIndex)
{
    CaretAssertVectorIndex(m_labelFiles, fileIndex);
    return m_labelFiles[fileIndex];
}

/**
 * Get a label file at the specified index.
 * @param fileIndex
 *    Index of the label file.
 * @return
 *    Metric file at the index.
 */
const LabelFile* 
BrainStructure::getLabelFile(const int32_t fileIndex) const
{
    CaretAssertVectorIndex(m_labelFiles, fileIndex);
    return m_labelFiles[fileIndex];
}

/**
 * Get all of the metric files.
 * @param metricFilesOut
 *    Will contain all metric files after this method exits.
 */
void 
BrainStructure::getMetricFiles(std::vector<MetricFile*>& metricFilesOut) const
{
    metricFilesOut.clear();
    metricFilesOut.insert(metricFilesOut.end(),
                          m_metricFiles.begin(),
                          m_metricFiles.end());
}

/**
 * Get the number of metric files.
 * @return Number of metric files.
 */
int32_t 
BrainStructure::getNumberOfMetricFiles() const
{
    return m_metricFiles.size();
}

/**
 * Get a metric file at the specified index.
 * @param fileIndex
 *    Index of the metric file.
 * @return
 *    Metric file at the index.
 */
MetricFile* 
BrainStructure::getMetricFile(const int32_t fileIndex)
{
    CaretAssertVectorIndex(m_metricFiles, fileIndex);
    return m_metricFiles[fileIndex];
}

/**
 * Get a metric file at the specified index.
 * @param fileIndex
 *    Index of the metric file.
 * @return
 *    Metric file at the index.
 */
const MetricFile* 
BrainStructure::getMetricFile(const int32_t fileIndex) const
{
    CaretAssertVectorIndex(m_metricFiles, fileIndex);
    return m_metricFiles[fileIndex];
}

/**
 * Get the number of rgba files.
 * @return Number of rgba files.
 */
int32_t 
BrainStructure::getNumberOfRgbaFiles() const
{
    return m_rgbaFiles.size();
}

/**
 * Get a rgba file at the specified index.
 * @param fileIndex
 *    Index of the rgba file.
 * @return
 *    Metric file at the index.
 */
RgbaFile* 
BrainStructure::getRgbaFile(const int32_t fileIndex)
{
    CaretAssertVectorIndex(m_rgbaFiles, fileIndex);
    return m_rgbaFiles[fileIndex];
}

/**
 * Get a rgba file at the specified index.
 * @param fileIndex
 *    Index of the rgba file.
 * @return
 *    Metric file at the index.
 */
const RgbaFile* 
BrainStructure::getRgbaFile(const int32_t fileIndex) const
{
    CaretAssertVectorIndex(m_rgbaFiles, fileIndex);
    return m_rgbaFiles[fileIndex];
}

/**
 * Get all of the rgba files.
 * @param rgbaFilesOut
 *    Will contain all rgba files after this method exits.
 */
void 
BrainStructure::getRgbaFiles(std::vector<RgbaFile*>& rgbaFilesOut) const
{
    rgbaFilesOut.clear();
    rgbaFilesOut.insert(rgbaFilesOut.end(),
                          m_rgbaFiles.begin(),
                          m_rgbaFiles.end());
}


/**
 * Receive events from the event manager.
 * 
 * @param event
 *   The event.
 */
void 
BrainStructure::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BRAIN_STRUCTURE_GET_ALL) {
        EventBrainStructureGetAll* brainStructureEvent =
                    dynamic_cast<EventBrainStructureGetAll*>(event);
        CaretAssert(brainStructureEvent);
        brainStructureEvent->addBrainStructure(this);
        brainStructureEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_GET_NODE_DATA_FILES) {
        EventNodeDataFilesGet* dataFilesEvent =
            dynamic_cast<EventNodeDataFilesGet*>(event);
        CaretAssert(dataFilesEvent);
        
        const Surface* associatedSurface = dataFilesEvent->getSurface();
        if (associatedSurface != NULL) {
            if (containsSurface(associatedSurface) == false) {
                return;
            }
        }
        
        for (std::vector<LabelFile*>::iterator labelIter = m_labelFiles.begin();
             labelIter != m_labelFiles.end();
             labelIter++) {
            dataFilesEvent->addFile(*labelIter);
        }
        
        for (std::vector<MetricFile*>::iterator metricIter = m_metricFiles.begin();
             metricIter != m_metricFiles.end();
             metricIter++) {
            dataFilesEvent->addFile(*metricIter);
        }
        
        for (std::vector<RgbaFile*>::iterator rgbaIter = m_rgbaFiles.begin();
             rgbaIter != m_rgbaFiles.end();
             rgbaIter++) {
            dataFilesEvent->addFile(*rgbaIter);
        }
        
        dataFilesEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_SURFACES_GET) {
        EventSurfacesGet* getSurfacesEvent =
            dynamic_cast<EventSurfacesGet*>(event);
        CaretAssert(getSurfacesEvent);
        
        const int32_t numSurfaces = getNumberOfSurfaces();
        for (int32_t i = 0; i < numSurfaces; i++) {
            getSurfacesEvent->addSurface(getSurface(i));
        }
        getSurfacesEvent->setEventProcessed();
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_SURFACE_STRUCTURES_VALID_GET) {
        EventSurfaceStructuresValidGet* structEvent = dynamic_cast<EventSurfaceStructuresValidGet*>(event);
        CaretAssert(structEvent);

        structEvent->addStructure(m_structure, getNumberOfNodes());
    }
}

/**
 * @return Return the unique identifier for this brain structure.
 */
int64_t 
BrainStructure::getBrainStructureIdentifier() const
{
    return m_brainStructureIdentifier;
}

/**
 * Get the attributes for this brain structure.
 * @return
 *    Attributes for this brain structure.
 */
BrainStructureNodeAttributes* 
BrainStructure::getNodeAttributes()
{
    return m_nodeAttributes;
}

/**
 * Get the attributes for this brain structure.
 * @return
 *    Attributes for this brain structure.
 */
const BrainStructureNodeAttributes* 
BrainStructure::getNodeAttributes() const
{
    return m_nodeAttributes;
}

/**
 * @return True if this brain structure contains any data files, else false.
 */
bool
BrainStructure::hasDataFiles() const
{
    std::vector<CaretDataFile*> allDataFiles;
    getAllDataFiles(allDataFiles);
    
    return ( ! allDataFiles.empty());
}


/**
 * Get all loaded data files.
 * @param allDataFilesOut
 *    Data files are loaded into this parameter.
 */
void 
BrainStructure::getAllDataFiles(std::vector<CaretDataFile*>& allDataFilesOut) const
{
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_surfaces.begin(),
                           m_surfaces.end());
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_labelFiles.begin(),
                           m_labelFiles.end());
    for (auto mf : m_metricFiles) {
        allDataFilesOut.push_back(mf);
        MetricDynamicConnectivityFile* dynFile = mf->getMetricDynamicConnectivityFile();
        if (dynFile != NULL) {
            if (dynFile->isDataValid()) {
                allDataFilesOut.push_back(dynFile);
            }
        }
    }
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_rgbaFiles.begin(),
                           m_rgbaFiles.end());
}

/**
 * Remove the data file from memory but DO NOT delete it.
 *
 * @param caretDataFile
 *    Caret file that is removed from the Brain.  After calling this method
 *    and the file was removed( true was returned), the caller is responsible
 *    for deleting the file when it is no longer needed.
 * @return
 *    True if the file was removed, else false.
 */
bool
BrainStructure::removeWithoutDeleteDataFile(const CaretDataFile* caretDataFile)
{
    std::vector<Surface*>::iterator surfaceIterator =
    std::find(m_surfaces.begin(),
              m_surfaces.end(),
              caretDataFile);
    if (surfaceIterator != m_surfaces.end()) {
        Surface* s = *surfaceIterator;
        removeAndMaybeDeleteSurface(s,
                      false);
        return true;
    }
    
    std::vector<LabelFile*>::iterator labelIterator =
    std::find(m_labelFiles.begin(),
              m_labelFiles.end(),
              caretDataFile);
    if (labelIterator != m_labelFiles.end()) {
        m_labelFiles.erase(labelIterator);
        return true;
    }
    
    std::vector<MetricFile*>::iterator metricIterator =
    std::find(m_metricFiles.begin(),
              m_metricFiles.end(),
              caretDataFile);
    if (metricIterator != m_metricFiles.end()) {
        m_metricFiles.erase(metricIterator);
        return true;
    }
    
    std::vector<RgbaFile*>::iterator rgbaIterator =
    std::find(m_rgbaFiles.begin(),
              m_rgbaFiles.end(),
              caretDataFile);
    if (rgbaIterator != m_rgbaFiles.end()) {
        m_rgbaFiles.erase(rgbaIterator);
        return true;
    }
    
    return false;
}

/**
 * Find a map in a metric file that contains shape data.
 * It first looks for a shape NIFTI intent code.  If that
 * is not found it looks for curvature, shape, depth, etc.
 * @param metricFileOut
 *    Output metric file that contains the shape map.
 * @param shapeMapIndexOut
 *    Output containing index of shape map.
 * @return
 *    True if a shape map was found, else false.
 */
bool 
BrainStructure::getMetricShapeMap(MetricFile* &shapeMetricFileOut,
                                  int32_t& shapeMapIndexOut) const
{
    shapeMetricFileOut = NULL;
    shapeMapIndexOut   = -1;
    
    MetricFile* depthMetricFile = NULL;
    int32_t     depthMapIndex = -1;
    MetricFile* depthNamedMetricFile = NULL;
    MetricFile* curvatureMetricFile = NULL;
    int32_t     curvatureMapIndex = -1;
    MetricFile* curvatureNamedMetricFile = NULL;
    MetricFile* shapeNamedMetricFile = NULL;
    MetricFile* sulcMetricFile = NULL;
    int32_t     sulcMapIndex = -1;
    MetricFile* sulcNamedMetricFile = NULL;
    
    const int numFiles = m_metricFiles.size();
    for (int32_t i = 0; i < numFiles; i++) {
        MetricFile* mf = m_metricFiles[i];
        const AString filename = mf->getFileNameNoPath().toLower();
        const int32_t numMaps = mf->getNumberOfMaps();
        for (int32_t j = 0; j < numMaps; j++) {
            const AString mapName = mf->getMapName(j).toLower();
            if (mapName.contains("sulc")) {
                if (sulcMetricFile == NULL) {
                    sulcMetricFile = mf;
                    sulcMapIndex   = j;
                }
            }
            else if (mapName.contains("depth")) {
                if (depthMetricFile == NULL) {
                    depthMetricFile = mf;
                    depthMapIndex   = j;
                }
            }
            else if (mapName.contains("curv")) {
                if (curvatureMetricFile == NULL) {
                    curvatureMetricFile = mf;
                    curvatureMapIndex   = j;
                }
            }
        }
        
        if (filename.contains("sulc")) {
            if (numMaps > 0) {
                sulcNamedMetricFile = mf;
            }
        }
        if (filename.contains("shape")) {
            if (numMaps > 0) {
                shapeNamedMetricFile = mf;
            }
        }
        if (filename.contains("curv")) {
            if (numMaps > 0) {
                curvatureNamedMetricFile = mf;
            }
        }
        if (filename.contains("depth")) {
            if (numMaps > 0) {
                depthNamedMetricFile = mf;
            }
        }
    }
    
    if (sulcMetricFile != NULL) {
        shapeMetricFileOut = sulcMetricFile;
        shapeMapIndexOut   = sulcMapIndex;
    }
    else if (depthMetricFile != NULL) {
        shapeMetricFileOut = depthMetricFile;
        shapeMapIndexOut   = depthMapIndex;
    }
    else if (curvatureMetricFile != NULL) {
        shapeMetricFileOut = curvatureMetricFile;
        shapeMapIndexOut   = curvatureMapIndex;
    }
    else if (sulcNamedMetricFile != NULL) {
        shapeMetricFileOut = sulcNamedMetricFile;
        shapeMapIndexOut   = 0;
    }
    else if (depthNamedMetricFile != NULL) {
        shapeMetricFileOut = depthNamedMetricFile;
        shapeMapIndexOut   = 0;
    }
    else if (curvatureNamedMetricFile != NULL) {
        shapeMetricFileOut = curvatureNamedMetricFile;
        shapeMapIndexOut   = 0;
    }
    else if (shapeNamedMetricFile != NULL) {
        shapeMetricFileOut = shapeNamedMetricFile;
        shapeMapIndexOut   = 0;
    }
    
    if ((shapeMetricFileOut != NULL) 
        && (shapeMapIndexOut >= 0)) {
        return true;
    }
    
    return false;
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
OverlaySet* 
BrainStructure::getOverlaySet(const int tabIndex)
{
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Get the overlay set for the given tab.
 * @param tabIndex
 *   Index of tab.
 * @return
 *   Overlay set at the given tab index.
 */
const OverlaySet* 
BrainStructure::getOverlaySet(const int tabIndex) const
{
    return m_overlaySetArray->getOverlaySet(tabIndex);
}

/**
 * Initilize the overlays for this model.
 */
void 
BrainStructure::initializeOverlays()
{
    m_overlaySetArray->initializeOverlaySelections();
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
BrainStructure::saveToScene(const SceneAttributes* sceneAttributes,
                                          const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "BrainStructure",
                                            1);
    
    sceneClass->addInteger("numberOfNodes", 
                           getNumberOfNodes());
    sceneClass->addEnumeratedType<StructureEnum, StructureEnum::Enum>("m_structure", m_structure);
    sceneClass->addClass(m_nodeAttributes->saveToScene(sceneAttributes, 
                                                       "m_nodeAttributes"));
    
    /*
     * Save Group/Name Selection Hierarchies
     */
    for (std::vector<LabelFile*>::iterator labelIter = m_labelFiles.begin();
         labelIter != m_labelFiles.end();
         labelIter++) {
        LabelFile* lf = *labelIter;
        sceneClass->addClass(lf->getGroupAndNameHierarchyModel()->saveToScene(sceneAttributes,
                                                         lf->getFileNameNoPath()));
    }
    
    const Surface* primAnatSurface = getPrimaryAnatomicalSurface();
    if (primAnatSurface != NULL) {
        sceneClass->addPathName("primaryAnatomicalSurface",
                                primAnatSurface->getFileName());
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
BrainStructure::restoreFromScene(const SceneAttributes* sceneAttributes,
                                               const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    const int32_t numNodes = sceneClass->getIntegerValue("numberOfNodes",
                                                         0);
    const StructureEnum::Enum structure = sceneClass->getEnumeratedTypeValue<StructureEnum,StructureEnum::Enum>("m_structure", 
                                                                                                                StructureEnum::INVALID);
    
    /*
     * Since there may be multiple brain structures in scene,
     * match by structure-type and number of nodes
     */
    if ((numNodes == getNumberOfNodes())
        && (structure == m_structure)) {
        m_nodeAttributes->restoreFromScene(sceneAttributes, 
                                           sceneClass->getClass("m_nodeAttributes"));

        /*
         * Save Group/Name Selection Hierarchies
         */
        for (std::vector<LabelFile*>::iterator labelIter = m_labelFiles.begin();
             labelIter != m_labelFiles.end();
             labelIter++) {
            LabelFile* lf = *labelIter;
            const SceneClass* labelClass = sceneClass->getClass(lf->getFileNameNoPath());
            lf->getGroupAndNameHierarchyModel()->restoreFromScene(sceneAttributes,
                                                                  labelClass);
        }
    }
    
    const ScenePathName* primAnatScenePathName = sceneClass->getPathName("primaryAnatomicalSurface");
    if (primAnatScenePathName != NULL) {
        const AString surfaceFileName = primAnatScenePathName->stringValue();
        if ( ! surfaceFileName.isEmpty()) {
            for (std::vector<Surface*>::iterator iter = m_surfaces.begin();
                 iter != m_surfaces.end();
                 iter++) {
                Surface* surface = *iter;
                CaretAssert(surface);
                if (surface->getFileName() == surfaceFileName) {
                    setPrimaryAnatomicalSurface(surface);
                    break;
                }
            }
        }
    }
    
}

/**
 * Match surface sizes to the primary anatomical surface
 *
 * @param matchStatus
 *     The match status
 */
void
BrainStructure::matchSurfacesToPrimaryAnatomical(const bool matchStatus)
{
    const Surface* primaryAnatomical = getPrimaryAnatomicalSurface();
    if (primaryAnatomical == NULL) {
        return;
    }
    
    for (auto s : m_surfaces) {
        if (s != primaryAnatomical) {
            s->matchToAnatomicalSurface(primaryAnatomical,
                                        matchStatus);
        }
    }
}

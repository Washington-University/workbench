/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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
#include <limits>

#include "CaretAssert.h"
#include "CaretLogger.h"

#define __BRAIN_STRUCTURE_DEFINE__
#include "BrainStructure.h"
#undef __BRAIN_STRUCTURE_DEFINE__
#include "Brain.h"
#include "BrainStructureNodeAttributes.h"
#include "CaretPointLocator.h"
#include "CaretPreferences.h"
#include "ElapsedTimer.h"
#include "EventCaretMappableDataFilesGet.h"
#include "EventManager.h"
#include "EventNodeDataFilesGet.h"
#include "EventModelAdd.h"
#include "EventModelDelete.h"
#include "EventSurfacesGet.h"
#include "GroupAndNameHierarchyModel.h"
#include "IdentificationManager.h"
#include "SelectionManager.h"
#include "LabelFile.h"
#include "MathFunctions.h"
#include "MetricFile.h"
#include "ModelSurface.h"
#include "OverlaySet.h"
#include "OverlaySetArray.h"
#include "RgbaFile.h"
#include "SceneClass.h"
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
    m_volumeInteractionSurface = NULL;
    
    m_overlaySetArray = new OverlaySetArray(this);
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        m_overlaySet[i] = new OverlaySet(this);
//    }
    
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_GET_NODE_DATA_FILES);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_IDENTIFICATION_SYMBOL_REMOVAL);
    EventManager::get()->addEventListener(this, 
                                          EventTypeEnum::EVENT_SURFACES_GET);
}

/**
 * Destructor.
 */
BrainStructure::~BrainStructure()
{
    EventManager::get()->removeAllEventsFromListener(this);
    
    delete m_overlaySetArray;
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        delete m_overlaySet[i];
//    }
    
    /*
     * Make a copy of all surface pointers since
     * deleting surfaces will alter the actual
     * vector that stores the surfaces.
     */
    std::vector<Surface*> allSurfaces(m_surfaces);
    
    for (uint64_t i = 0; i < allSurfaces.size(); i++) {
        removeSurface(allSurfaces[i]);
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
 * @param isReloadingFile
 *    If true, file is being 'reloaded' so it does not get added
 *    since it should already be in this brain structure.
 * @throw DataFileException
 *    If the number of nodes in the label file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addLabelFile(LabelFile* labelFile,
                             const bool isReloadingFile) throw (DataFileException)
{
    CaretAssert(labelFile);
    
    int32_t numNodes = getNumberOfNodes();
    if (numNodes > 0) {
        if (labelFile->getNumberOfNodes() != numNodes) {
            AString message = (labelFile->getFileNameNoPath()
                               + " contains "
                               + AString::number(labelFile->getNumberOfNodes())
                               + " vertices but the "
                               + StructureEnum::toGuiName(getStructure())
                               + " contains "
                               + AString::number(numNodes)
                               + " vertices.");
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
        
    if (labelFile->getStructure() != getStructure()) {
        AString message = ("Trying to add metric file named \""
                           + labelFile->getFileNameNoPath()
                           + "\" with structure \""
                           + StructureEnum::toGuiName(labelFile->getStructure())
                           + " to BrainStructure for \""
                           + StructureEnum::toGuiName(getStructure())
                           + "\n");
        DataFileException(e);
        CaretLogThrowing(e);
        throw e;        
    }
    
    if (isReloadingFile == false) {
        m_labelFiles.push_back(labelFile);
    }
}

/**
 * Add a metric file.
 *
 * @param metricFile
 *    Metric file that is added.
 * @param isReloadingFile
 *    If true, file is being 'reloaded' so it does not get added
 *    since it should already be in this brain structure.
 * @throw DataFileException
 *    If the number of nodes in the metric file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addMetricFile(MetricFile* metricFile,
                              const bool isReloadingFile) throw (DataFileException)
{
    CaretAssert(metricFile);
    
    int32_t numNodes = getNumberOfNodes();
    if (numNodes > 0) {
        if (metricFile->getNumberOfNodes() != numNodes) {
            AString message = (metricFile->getFileNameNoPath()
                               + " contains "
                               + AString::number(metricFile->getNumberOfNodes())
                               + " vertices but the "
                               + StructureEnum::toGuiName(getStructure())
                               + " contains "
                               + AString::number(numNodes)
                               + " vertices.");
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    if (metricFile->getStructure() != getStructure()) {
        AString message = ("Trying to add metric file named \""
                           + metricFile->getFileNameNoPath()
                           + "\" with structure \""
                           + StructureEnum::toGuiName(metricFile->getStructure())
                           + " to BrainStructure for \""
                           + StructureEnum::toGuiName(getStructure())
                           + "\n");
        DataFileException(e);
        CaretLogThrowing(e);
        throw e;        
    }
    
    if (isReloadingFile == false) {
        m_metricFiles.push_back(metricFile);
    }
}

/**
 * Add an RGBA file.
 *
 * @param rgbaFile
 *    RGBA file that is added.
 * @param isReloadingFile
 *    If true, file is being 'reloaded' so it does not get added
 *    since it should already be in this brain structure.
 * @throw DataFileException
 *    If the number of nodes in the RGBA file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addRgbaFile(RgbaFile* rgbaFile,
                            const bool isReloadingFile) throw (DataFileException)
{
    CaretAssert(rgbaFile);
    
    int32_t numNodes = getNumberOfNodes();
    if (numNodes > 0) {
        if (rgbaFile->getNumberOfNodes() != numNodes) {
            AString message = (rgbaFile->getFileNameNoPath()
                               + " contains "
                               + AString::number(rgbaFile->getNumberOfNodes())
                               + " vertices but the "
                               + StructureEnum::toGuiName(getStructure())
                               + " contains "
                               + AString::number(numNodes)
                               + " vertices.");
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    
    if (rgbaFile->getStructure() != getStructure()) {
        AString message = ("Trying to add metric file named \""
                           + rgbaFile->getFileNameNoPath()
                           + "\" with structure \""
                           + StructureEnum::toGuiName(rgbaFile->getStructure())
                           + " to BrainStructure for \""
                           + StructureEnum::toGuiName(getStructure())
                           + "\n");
        DataFileException(e);
        CaretLogThrowing(e);
        throw e;        
    }
    
    if (isReloadingFile == false) {
        m_rgbaFiles.push_back(rgbaFile);
    }
}

/**
 * Add a surface.
 *
 * @param surface
 *    Surface that is added.
 * @param isReloadingFile
 *    If true, file is being 'reloaded' so it does not get added
 *    since it should already be in this brain structure.
 * @throw DataFileException
 *    If the number of nodes in the surface does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addSurface(Surface* surface,
                           const bool isReloadingFile,
                           const bool initilizeOverlaysFlag) throw (DataFileException)
{
    CaretAssert(surface);
    
    int32_t numNodes = getNumberOfNodes();
    if (numNodes > 0) {
        if (surface->getNumberOfNodes() != numNodes) {
            AString message = (surface->getFileNameNoPath()
                               + "  contains "
                               + AString::number(surface->getNumberOfNodes())
                               + " vertices but the "
                               + StructureEnum::toGuiName(getStructure())
                               + " contains "
                               + AString::number(numNodes)
                               + " vertices.");
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    if (surface->getStructure() != getStructure()) {
        AString message = ("Trying to add metric file named \""
                           + surface->getFileNameNoPath()
                           + "\" with structure \""
                           + StructureEnum::toGuiName(surface->getStructure())
                           + " to BrainStructure for \""
                           + StructureEnum::toGuiName(getStructure())
                           + "\n");
        DataFileException(e);
        CaretLogThrowing(e);
        throw e;        
    }
    if (numNodes == 0) {
        const int32_t numSurfaceNodes = surface->getNumberOfNodes();
        m_nodeAttributes->update(numSurfaceNodes);
    }
    
    surface->setBrainStructure(this);
    
    if (isReloadingFile == false) {
        m_surfaces.push_back(surface);
        
        /*
         * Create a model controller for the surface.
         */
        ModelSurface* mdcs = new ModelSurface(m_brain,
                                              surface);
        m_surfaceControllerMap.insert(std::make_pair(surface, mdcs));
        
        if (initilizeOverlaysFlag) {
            initializeOverlays();
        }
        
        /*
         * Send the controller added event.
         */
        EventModelAdd addEvent(mdcs);
        EventManager::get()->sendEvent(addEvent.getPointer());
    }
}

/**
 * Remove a surface from this brain structure and delete both the surface
 * and its model.
 * 
 * @surface
 *    Surface that is removed.
 */
void 
BrainStructure::removeSurface(Surface* surface)
{
    CaretAssert(surface);
    
    std::vector<Surface*>::iterator iter =
    std::find(m_surfaces.begin(),
              m_surfaces.end(),
              surface);
    
    CaretAssertMessage((iter != m_surfaces.end()),
                       "Trying to delete surface not in brain structure.");
    
    std::map<Surface*, ModelSurface*>::iterator controllerIter = 
        m_surfaceControllerMap.find(surface);

    CaretAssertMessage((controllerIter != m_surfaceControllerMap.end()),
                       "Surface does not map to a model controller");

    ModelSurface* mdcs = controllerIter->second;
    
    /*
     * Remove from surface to controller map.
     */
    m_surfaceControllerMap.erase(controllerIter);
    
    /*
     * Remove the surface.
     */
    m_surfaces.erase(iter);
    
    /*
     * Send the controller deleted event.
     */
    EventModelDelete deleteEvent(mdcs);
    EventManager::get()->sendEvent(deleteEvent.getPointer());
    
    /*
     * Delete the controller and the surface.
     */
    delete mdcs;
    delete surface;
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
 * @return The surface used for volume interaction.
 * Returns NULL if no anatomical surfaces.
 */
const Surface* 
BrainStructure::getVolumeInteractionSurfacePrivate() const
{
    bool valid = false;
    if (m_volumeInteractionSurface != NULL) {
        const int32_t numSurfaces = getNumberOfSurfaces();
        for (int32_t i = 0; i < numSurfaces; i++) {
            if (m_surfaces[i] == m_volumeInteractionSurface) {
                valid = true;
                break;
            }
        }
    }
    if (valid) {
        return m_volumeInteractionSurface;
    }
    m_volumeInteractionSurface = NULL;
    
    /*
     * Give preference to anatomical surfaces but if there are none
     * (perhaps the surface types are missing), use all surfaces.
     */
    std::vector<Surface*> interactionSurfaces;
    getSurfacesOfType(SurfaceTypeEnum::ANATOMICAL, 
                            interactionSurfaces);
    if (interactionSurfaces.empty()) {
        interactionSurfaces = m_surfaces;
    }
    
    if (interactionSurfaces.empty() == false) {
        /*
         * Default to first surface
         */
        m_volumeInteractionSurface = interactionSurfaces[0];
        
        /*
         * Now look for a surface with certain strings in their name
         */
        Surface* midThicknessSurface = NULL;
        Surface* whiteMatterSurface  = NULL;
        Surface* pialSurface         = NULL;
        Surface* anatomicalSurface   = NULL;
        Surface* fiducialSurface     = NULL;
        const int32_t numSurfaces = static_cast<int32_t>(interactionSurfaces.size());
        for (int32_t i = 0; i < numSurfaces; i++) {
            const AString name = interactionSurfaces[i]->getFileNameNoPath().toLower();
            if (name.indexOf("midthick") >= 0) {
                midThicknessSurface = interactionSurfaces[i];
            }
            if (name.indexOf("white") >= 0) {
                whiteMatterSurface = interactionSurfaces[i];
            }
            if (name.indexOf("pial") >= 0) {
                pialSurface = interactionSurfaces[i];
            }
            if (name.indexOf("anatomical") >= 0) {
                anatomicalSurface = interactionSurfaces[i];
            }
            if (name.indexOf("fiducial") >= 0) {
                fiducialSurface = interactionSurfaces[i];
            }
        }
        
        if (midThicknessSurface != NULL) {
            m_volumeInteractionSurface = midThicknessSurface;
        }
        else if (whiteMatterSurface != NULL) {
            m_volumeInteractionSurface = whiteMatterSurface;
        }
        else if (pialSurface != NULL) {
            m_volumeInteractionSurface = pialSurface;
        }
        else if (anatomicalSurface != NULL) {
            m_volumeInteractionSurface = anatomicalSurface;
        }
        else if (fiducialSurface != NULL) {
            m_volumeInteractionSurface = fiducialSurface;
        }
    }
    
    if (m_volumeInteractionSurface != NULL) {
        CaretLogFiner("Volume Interaction Surface for "
                      + StructureEnum::toGuiName(m_structure)
                      + ": " 
                      + m_volumeInteractionSurface->getFileNameNoPath());
    }
    else {
        CaretLogFiner("Volume Interaction Surface for "
                      + StructureEnum::toGuiName(m_structure)
                      + " is invalid.");
    }
    
    return m_volumeInteractionSurface;
}

/**
 * @return The surface used for volume interaction.
 * Returns NULL if no anatomical surfaces.
 */
const Surface* 
BrainStructure::getVolumeInteractionSurface() const
{
    return getVolumeInteractionSurfacePrivate();
}

/**
 * @return The surface used for volume interaction.
 * Returns NULL if no anatomical surfaces.
 */
Surface* 
BrainStructure::getVolumeInteractionSurface()
{
    /*
     * Kludge to avoid duplicated code and ease maintenance
     */
    const Surface* constSurface = getVolumeInteractionSurfacePrivate();
    Surface* s = (Surface*)constSurface;
    return s;
}

/**
 * Set the volume interaction surface.
 * @param volumeInteractionSurface
 *    New volume interaction surface.
 */
void 
BrainStructure::setVolumeInteractionSurface(Surface* volumeInteractionSurface)
{
    m_volumeInteractionSurface = volumeInteractionSurface;
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
    if (event->getEventType() == EventTypeEnum::EVENT_GET_NODE_DATA_FILES) {
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
    else if (event->getEventType() == EventTypeEnum::EVENT_CARET_MAPPABLE_DATA_FILES_GET) {
        EventCaretMappableDataFilesGet* dataFilesEvent =
            dynamic_cast<EventCaretMappableDataFilesGet*>(event);
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
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_metricFiles.begin(),
                           m_metricFiles.end());
    allDataFilesOut.insert(allDataFilesOut.end(),
                           m_rgbaFiles.begin(),
                           m_rgbaFiles.end());
}

/**
 * Remove a data file from memory (does NOT delete file on disk.)
 * @param caretDataFile
 *    Data file to remove.
 * @return
 *    true if file was written, else false.
 */
bool 
BrainStructure::removeDataFile(CaretDataFile* caretDataFile)
{
    std::vector<Surface*>::iterator surfaceIterator = 
        std::find(m_surfaces.begin(),
                  m_surfaces.end(),
                  caretDataFile);
    if (surfaceIterator != m_surfaces.end()) {
        Surface* s = *surfaceIterator;
        removeSurface(s);
        return true;
    }
    
    std::vector<LabelFile*>::iterator labelIterator = 
    std::find(m_labelFiles.begin(),
              m_labelFiles.end(),
              caretDataFile);
    if (labelIterator != m_labelFiles.end()) {
        delete caretDataFile;
        m_labelFiles.erase(labelIterator);
        return true;
    }
    
    std::vector<MetricFile*>::iterator metricIterator = 
    std::find(m_metricFiles.begin(),
              m_metricFiles.end(),
              caretDataFile);
    if (metricIterator != m_metricFiles.end()) {
        delete caretDataFile;
        m_metricFiles.erase(metricIterator);
        return true;
    }
    
    std::vector<RgbaFile*>::iterator rgbaIterator = 
    std::find(m_rgbaFiles.begin(),
              m_rgbaFiles.end(),
              caretDataFile);
    if (rgbaIterator != m_rgbaFiles.end()) {
        delete caretDataFile;
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
//    CaretAssertArrayIndex(m_overlaySet, 
//                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
//                          tabIndex);
//    return m_overlaySet[tabIndex];
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
//    CaretAssertArrayIndex(m_overlaySet,
//                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
//                          tabIndex);
//    return m_overlaySet[tabIndex];
}

/**
 * Initilize the overlays for this controller.
 */
void 
BrainStructure::initializeOverlays()
{
    m_overlaySetArray->initializeOverlaySelections();
//    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
//        m_overlaySet[i]->initializeOverlays();
//    }
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
}


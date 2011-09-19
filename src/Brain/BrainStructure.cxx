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

#include "CaretAssert.h"
#include "CaretLogger.h"

#include "BrainStructure.h"
#include "EventManager.h"
#include "EventModelDisplayControllerAdd.h"
#include "EventModelDisplayControllerDelete.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "ModelDisplayControllerSurface.h"
#include "RgbaFile.h"
#include "Surface.h"

#include <algorithm>

using namespace caret;

/**
 * Constructor.
 * 
 */
BrainStructure::BrainStructure(Brain* brain,
                               StructureEnum::Enum structure)
{
    this->brain = brain;
    this->structure = structure;
}

/**
 * Destructor.
 */
BrainStructure::~BrainStructure()
{
    /*
     * Make a copy of all surface pointers since
     * deleting surfaces will alter the actual
     * vector that stores the surfaces.
     */
    std::vector<Surface*> allSurfaces(this->surfaces);
    
    for (uint64_t i = 0; i < allSurfaces.size(); i++) {
        this->deleteSurface(allSurfaces[i]);
    }
    this->surfaces.clear();
    
    for (uint64_t i = 0; i < labelFiles.size(); i++) {
        delete this->labelFiles[i];
        this->labelFiles[i] = NULL;
    }
    this->labelFiles.clear();
    
    for (uint64_t i = 0; i < metricFiles.size(); i++) {
        delete this->metricFiles[i];
        this->metricFiles[i] = NULL;
    }
    this->metricFiles.clear();
    
    for (uint64_t i = 0; i < rgbaFiles.size(); i++) {
        delete this->rgbaFiles[i];
        this->rgbaFiles[i] = NULL;
    }
    this->rgbaFiles.clear();
}

/**
 * Get the structure for this BrainStructure.
 *
 * @return The structure.
 */
StructureEnum::Enum 
BrainStructure::getStructure() const
{
    return this->structure;
}

/**
 * Add a label file.
 *
 * @param labelFile
 *    Label file that is added.
 * @throw DataFileException
 *    If the number of nodes in the label file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addLabelFile(LabelFile* labelFile) throw (DataFileException)
{
    CaretAssert(labelFile);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (labelFile->getNumberOfNodes() != numNodes) {
            AString message = " File contains "
            + AString::number(labelFile->getNumberOfNodes())
            + " but the "
            + StructureEnum::toGuiName(this->getStructure())
            + " contains "
            + AString::number(numNodes)
            + " nodes.";
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    this->labelFiles.push_back(labelFile);
}

/**
 * Add a metric file.
 *
 * @param metricFile
 *    Metric file that is added.
 * @throw DataFileException
 *    If the number of nodes in the metric file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addMetricFile(MetricFile* metricFile) throw (DataFileException)
{
    CaretAssert(metricFile);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (metricFile->getNumberOfNodes() != numNodes) {
            AString message = " File contains "
            + AString::number(metricFile->getNumberOfNodes())
            + " but the "
            + StructureEnum::toGuiName(this->getStructure())
            + " contains "
            + AString::number(numNodes)
            + " nodes.";
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    this->metricFiles.push_back(metricFile);
}

/**
 * Add an RGBA file.
 *
 * @param rgbaFile
 *    RGBA file that is added.
 * @throw DataFileException
 *    If the number of nodes in the RGBA file does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addRgbaFile(RgbaFile* rgbaFile) throw (DataFileException)
{
    CaretAssert(rgbaFile);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (rgbaFile->getNumberOfNodes() != numNodes) {
            AString message = " File contains "
            + AString::number(rgbaFile->getNumberOfNodes())
            + " but the "
            + StructureEnum::toGuiName(this->getStructure())
            + " contains "
            + AString::number(numNodes)
            + " nodes.";
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    this->rgbaFiles.push_back(rgbaFile);
}

/**
 * Add a surface.
 *
 * @param surface
 *    Surface that is added.
 * @throw DataFileException
 *    If the number of nodes in the surface does not match
 *    the number of nodes in this brain structure.
 */
void 
BrainStructure::addSurface(Surface* surface) throw (DataFileException)
{
    CaretAssert(surface);
    
    int32_t numNodes = this->getNumberOfNodes();
    if (numNodes > 0) {
        if (surface->getNumberOfNodes() != numNodes) {
            AString message = " Surface contains "
            + AString::number(surface->getNumberOfNodes())
            + " but the "
            + StructureEnum::toGuiName(this->getStructure())
            + " contains "
            + AString::number(numNodes)
            + " nodes.";
            
            DataFileException e(message);
            CaretLogThrowing(e);
            throw e;
        }
    }
    
    this->surfaces.push_back(surface);

    /*
     * Create a model controller for the surface.
     */
    ModelDisplayControllerSurface* mdcs = new ModelDisplayControllerSurface(surface);
    this->surfaceControllerMap.insert(std::make_pair(surface, mdcs));
    
    /*
     * Send the controller added event.
     */
    EventModelDisplayControllerAdd addEvent(mdcs);
    EventManager::get()->sendEvent(addEvent.getPointer());
}

void 
BrainStructure::deleteSurface(Surface* surface)
{
    CaretAssert(surface);
    
    std::vector<Surface*>::iterator iter =
    std::find(this->surfaces.begin(),
              this->surfaces.end(),
              surface);
    
    CaretAssertMessage((iter != this->surfaces.end()),
                       "Trying to delete surface not in brain structure.");
    
    std::map<Surface*, ModelDisplayControllerSurface*>::iterator controllerIter = 
        this->surfaceControllerMap.find(surface);

    CaretAssertMessage((controllerIter != this->surfaceControllerMap.end()),
                       "Surface does not map to a model controller");

    ModelDisplayControllerSurface* mdcs = controllerIter->second;
    
    /*
     * Remove from surface to controller map.
     */
    this->surfaceControllerMap.erase(controllerIter);
    
    /*
     * Send the controller deleted event.
     */
    EventModelDisplayControllerDelete deleteEvent(mdcs);
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
    return static_cast<int>(this->surfaces.size());
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
    CaretAssertVectorIndex(this->surfaces, indx);
    
    return this->surfaces[indx];
}

/**
 * Get the brain that this brain structure is in.
 */
Brain* 
BrainStructure::getBrain()
{
    return this->brain;    
}

/**
 * Get the number of nodes used by this brain structure.
 *
 * @return Number of nodes.
 */
int32_t 
BrainStructure::getNumberOfNodes() const
{
    if (this->surfaces.empty() == false) {
        return surfaces[0]->getNumberOfNodes();
    }
    return 0;
}

/**
 * Get the number of label files.
 * @return Number of label files.
 */
int32_t 
BrainStructure::getNumberOfLabelFiles() const
{
    return this->labelFiles.size();
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
    CaretAssertVectorIndex(this->labelFiles, fileIndex);
    return this->labelFiles[fileIndex];
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
    CaretAssertVectorIndex(this->labelFiles, fileIndex);
    return this->labelFiles[fileIndex];
}

/**
 * Get the number of metric files.
 * @return Number of metric files.
 */
int32_t 
BrainStructure::getNumberOfMetricFiles() const
{
    return this->metricFiles.size();
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
    CaretAssertVectorIndex(this->metricFiles, fileIndex);
    return this->metricFiles[fileIndex];
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
    CaretAssertVectorIndex(this->metricFiles, fileIndex);
    return this->metricFiles[fileIndex];
}

/**
 * Get the number of rgba files.
 * @return Number of rgba files.
 */
int32_t 
BrainStructure::getNumberOfRgbaFiles() const
{
    return this->rgbaFiles.size();
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
    CaretAssertVectorIndex(this->rgbaFiles, fileIndex);
    return this->rgbaFiles[fileIndex];
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
    CaretAssertVectorIndex(this->rgbaFiles, fileIndex);
    return this->rgbaFiles[fileIndex];
}




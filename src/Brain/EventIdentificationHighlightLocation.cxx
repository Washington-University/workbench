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

#include "EventIdentificationHighlightLocation.h"

using namespace caret;

/**
 * Constructor for identification event on surface.
 * @param modelDisplayController
 *    Controller on which identification took place.
 * @param surfaceStructure
 *    Surface's structure.
 * @param surfaceNodeNumber
 *    The node number in the surface.
 * @param surfaceNumberOfNodes
 *    Number of nodes in the surface.
 * @param xyz
 *    Stereotaxic location of identified item.
 */
EventIdentificationHighlightLocation::EventIdentificationHighlightLocation(IdentificationManager* identificationManager,
                                                                           BrainStructure* brainStructure,
                                                                           const StructureEnum::Enum surfaceStructure,
                                                                           const int32_t surfaceNodeNumber,
                                                                           const int32_t surfaceNumberOfNodes,
                                                                           const float xyz[3])
: Event(EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION)
{
    this->identificationManager = identificationManager;
    this->identificationType = IDENTIFICATION_SURFACE;
    this->brainStructure = brainStructure;
    this->surfaceStructure = surfaceStructure;
    this->surfaceNodeNumber = surfaceNodeNumber;
    this->surfaceNumberOfNodes = surfaceNumberOfNodes;
    this->xyz[0] = xyz[0];
    this->xyz[1] = xyz[1];
    this->xyz[2] = xyz[2];
}

/**
 * Constructor for identification event on volume.
 * @param modelDisplayController
 *    Controller on which identification took place.
 * @param xyz
 *    Stereotaxic location of identified item.
 */
EventIdentificationHighlightLocation::EventIdentificationHighlightLocation(IdentificationManager* identificationManager,
                                                                           const VolumeFile* volumeFile,
                                                                           const int64_t volumeSliceIndices[3],
                                                                           const float xyz[3])
: Event(EventTypeEnum::EVENT_IDENTIFICATION_HIGHLIGHT_LOCATION),
  volumeFile(volumeFile)
{
    this->identificationManager = identificationManager;
    this->identificationType = IDENTIFICATION_VOLUME;
    this->volumeSliceIndices[0] = volumeSliceIndices[0];
    this->volumeSliceIndices[1] = volumeSliceIndices[1];
    this->volumeSliceIndices[2] = volumeSliceIndices[2];
    this->xyz[0] = xyz[0];
    this->xyz[1] = xyz[1];
    this->xyz[2] = xyz[2];
}

/**
 *  Destructor.
 */
EventIdentificationHighlightLocation::~EventIdentificationHighlightLocation()
{
    
}

/**
 * Initialize members of the class. 
 */
void 
EventIdentificationHighlightLocation::initializeMembers()
{
    this->brainStructure = NULL;
    this->surfaceStructure = StructureEnum::INVALID;
    this->surfaceNodeNumber = -1;
    this->surfaceNumberOfNodes = -1;
    this->xyz[0] = 0.0;
    this->xyz[1] = 0.0;
    this->xyz[2] = 0.0;    
    this->volumeFile = NULL;
    this->volumeSliceIndices[0] = -1;
    this->volumeSliceIndices[1] = -1;
    this->volumeSliceIndices[2] = -1;
}

/**
 * @return Identification manager for additional node identifications.
 */
IdentificationManager* 
EventIdentificationHighlightLocation::getIdentificationManager()
{
    return this->identificationManager;
}

/**
 * @return BrainStructure of surface on which identification took place
 * (valid only for surface identification)
 */
BrainStructure* 
EventIdentificationHighlightLocation::getSurfaceBrainStructure()
{
    return this->brainStructure;
}

/**
 * @return  The surface's structure  (only valid for surface 
 * identification events).
 */
StructureEnum::Enum 
EventIdentificationHighlightLocation::getSurfaceStructure() const
{
    return this->surfaceStructure;
}

/**
 * @return The surface's node number  (only valid for surface 
 * identification events).
 */
int32_t 
EventIdentificationHighlightLocation::getSurfaceNodeNumber() const
{
    return this->surfaceNodeNumber;
}

/**
 * @return The surface's number of nodes (only valid for surface 
 * identification events).
 */
int32_t 
EventIdentificationHighlightLocation::getSurfaceNumberOfNodes() const
{
    return this->surfaceNumberOfNodes;
}

/**
 * @return The stereotaxic location of the identification (valid for all).
 */
const float* 
EventIdentificationHighlightLocation::getXYZ() const
{
    return this->xyz;
}

/**
 * @return The volume file on which identification took place
 * (valid only for volume identification)
 */
const VolumeFile* 
EventIdentificationHighlightLocation::getVolumeFile() const
{
    return this->volumeFile;
}

/**
 * @return The stereotaxic location of the identification (valid only
 * for volume identification).
 */
const int64_t* 
EventIdentificationHighlightLocation::getVolumeSliceIndices() const
{
    return this->volumeSliceIndices;
}

/**
 * @return The type of the identification.
 */
EventIdentificationHighlightLocation::IdentificationType 
EventIdentificationHighlightLocation::getIdentificationType() const
{
    return this->identificationType;
}


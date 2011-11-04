
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

#define __IDENTIFICATION_TEXT_GENERATOR_DECLARE__
#include "IdentificationTextGenerator.h"
#undef __IDENTIFICATION_TEXT_GENERATOR_DECLARE__

#include "IdentificationItemSurfaceNode.h"
#include "IdentificationItemVoxel.h"
#include "IdentificationManager.h"
#include "IdentificationStringBuilder.h"
#include "Surface.h"
#include "VolumeFile.h"

using namespace caret;


    
/**
 * \class IdentificationTextGenerator 
 * \brief Creates text describing identified data.
 *
 * Examine the identified data and generate descriptive text.
 */

/**
 * Constructor.
 */
IdentificationTextGenerator::IdentificationTextGenerator()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
IdentificationTextGenerator::~IdentificationTextGenerator()
{
    
}

/**
 * Create identification text from selection in the identification manager.
 * @param idManager
 *    Identification manager containing selection.
 * @param browserTabContent
 *    
 */
AString 
IdentificationTextGenerator::createIdentificationText(const IdentificationManager* idManager,
                                                      const BrowserTabContent* /*browserTabConent*/) const
{
    IdentificationStringBuilder idText;
    
    const IdentificationItemSurfaceNode* surfaceID = idManager->getSurfaceNodeIdentification();
    const Surface* surface = surfaceID->getSurface();
    const int32_t nodeNumber = surfaceID->getNodeNumber();
    
    if ((surface != NULL) 
        && (nodeNumber >= 0)) {
        idText.addLine(false, "NODE", nodeNumber, false);
        
        const float* xyz = surface->getCoordinate(nodeNumber);
        
        idText.addLine(true, SurfaceTypeEnum::toGuiName(surface->getSurfaceType())
                       + " XYZ: "
                       + AString::number(xyz[0])
                       + ", "
                       + AString::number(xyz[1])
                       + ", "
                       + AString::number(xyz[2]));
    }
    
    const IdentificationItemVoxel* voxelID = idManager->getVoxelIdentification();
    if (voxelID->isValid()) {
        int64_t ijk[3];
        const VolumeFile* vf = voxelID->getVolumeFile();
        voxelID->getVoxelIJK(ijk);
        
        idText.addLine(true,
                       vf->getFileNameNoPath()
                       + " "
                       + AString::number(ijk[0])
                       + ", "
                       + AString::number(ijk[1])
                       + ", "
                       + AString::number(ijk[2])
                       + ": "
                       + AString::number(vf->getValue(ijk)));
    }
    
    return idText.toString();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
IdentificationTextGenerator::toString() const
{
    return "IdentificationTextGenerator";
}

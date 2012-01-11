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


#include "SurfaceProjectedItem.h"
#include "SurfaceProjectionBarycentric.h"
#include "SurfaceProjectionVanEssen.h"

using namespace caret;

/**
 * Constructor.
 *
 */
SurfaceProjectedItem::SurfaceProjectedItem()
    : CaretObjectTracksModification()
{
    this->initializeMembersSurfaceProjectedItem();
}

/**
 * Constructor that creates an unprojected item at the coordiante.
 * @param xyz - coordinate of projected item.
 *
 */
SurfaceProjectedItem::SurfaceProjectedItem(const float xyz[3])
    : CaretObjectTracksModification()
{
    this->initializeMembersSurfaceProjectedItem();
    this->originalXYZ[0] = xyz[0];
    this->originalXYZ[1] = xyz[1];
    this->originalXYZ[2] = xyz[2];
}

/**
 * Destructor
 */
SurfaceProjectedItem::~SurfaceProjectedItem()
{
    delete this->barycentricProjection;
    delete this->vanEssenProjection;
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
SurfaceProjectedItem::SurfaceProjectedItem(const SurfaceProjectedItem& o)
    : CaretObjectTracksModification(o)
{
    this->initializeMembersSurfaceProjectedItem();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
SurfaceProjectedItem&
SurfaceProjectedItem::operator=(const SurfaceProjectedItem& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
SurfaceProjectedItem::copyHelper(const SurfaceProjectedItem& spi)
{
    this->projectionType = spi.getProjectionType();
    spi.getOriginalXYZ(this->originalXYZ);
    spi.getVolumeXYZ(this->volumeXYZ);
    this->structure = spi.structure;
    
    *this->barycentricProjection = *spi.barycentricProjection;
    *this->vanEssenProjection    = *spi.vanEssenProjection;
}

/**
 * Reset to default values as if no projection of any type.
 */
void 
SurfaceProjectedItem::reset()
{
    this->projectionType = SurfaceProjectionTypeEnum::UNPROJECTED;
    this->originalXYZ[0] = 0.0;
    this->originalXYZ[1] = 0.0;
    this->originalXYZ[2] = 0.0;
    this->volumeXYZ[0] = 0.0;
    this->volumeXYZ[1] = 0.0;
    this->volumeXYZ[2] = 0.0;
    this->structure = StructureEnum::INVALID;
    this->barycentricProjection->reset();
    this->vanEssenProjection->reset();
}

void
SurfaceProjectedItem::initializeMembersSurfaceProjectedItem()
{
    this->barycentricProjection = new SurfaceProjectionBarycentric();
    this->vanEssenProjection    = new SurfaceProjectionVanEssen();
    this->reset();
}

/**
 * Unproject the item to the original XYZ coordinates. 
 * 
 * @param sf - Surface on which unprojection takes place.
 * @param pasteOntoSurfaceFlag - place item directly on surface.
 *
 */
void
SurfaceProjectedItem::unprojectToOriginalXYZ(const SurfaceFile& sf,
                                             const bool isUnprojectedOntoSurface)
{
    float xyz[3];
    if (getProjectedPosition(sf, 
                             xyz, 
                             isUnprojectedOntoSurface)) {
        this->setOriginalXYZ(xyz);
    }
}

/**
 * Unproject the item to the volume XYZ coordinates.
 *
 * @param sf - Surface on which unprojection takes place.
 * @param pasteOntoSurfaceFlag - place item directly on surface.
 *
 */
void
SurfaceProjectedItem::unprojectToVolumeXYZ(const SurfaceFile& sf,
                                           const bool isUnprojectedOntoSurface)
{
    float xyz[3];
    if (getProjectedPosition(sf, 
                             xyz, 
                             isUnprojectedOntoSurface)) {
        this->setVolumeXYZ(xyz);
    }
}

/**
 * Get the projected position of this item.
 * 
 * @param surfaceFile  Surface File for positioning.
 * @param 
 * @param pasteOntoSurfaceFlag   Place directly on the surface.
 * 
 * @return  An array containing the coordinate of the projected point or
 *    null if the projected position is invalid.
 *
 */
bool
SurfaceProjectedItem::getProjectedPosition(const SurfaceFile& surfaceFile,
                                           float xyzOut[3],
                                           const bool isUnprojectedOntoSurface) const
{
    bool valid = false;
    
    switch (this->projectionType) {
        case SurfaceProjectionTypeEnum::BARYCENTRIC:
            valid = this->barycentricProjection->unprojectToSurface(surfaceFile, 
                                                            xyzOut, 
                                                            isUnprojectedOntoSurface);
            break;
        case SurfaceProjectionTypeEnum::UNPROJECTED:
            if (this->isOriginalXYZValid()) {
                valid = true;
                this->getOriginalXYZ(xyzOut);
            }
            break;
        case SurfaceProjectionTypeEnum::VANESSEN:
            valid = this->barycentricProjection->unprojectToSurface(surfaceFile, 
                                                            xyzOut, 
                                                            isUnprojectedOntoSurface);
            break;
    }
    
    return valid;
}

/**
 * Get the focus' position.
 * 
 * @return  Position of the focus.
 *
 */
const float*
SurfaceProjectedItem::getOriginalXYZ() const
{
    return this->originalXYZ;
}

/**
 * Get the original XYZ position.
 * @param xyzOut  Position placed into here.
 *
 */
void
SurfaceProjectedItem::getOriginalXYZ(float xyzOut[3]) const
{
    xyzOut[0] = this->originalXYZ[0];
    xyzOut[1] = this->originalXYZ[1];
    xyzOut[2] = this->originalXYZ[2];
}

/**
 * Get the validity of the original XYZ coordinate.
 * @return Validity of original XYZ coordinate.
 *
 */
bool
SurfaceProjectedItem::isOriginalXYZValid() const
{
    if ((this->originalXYZ[0] != 0.0) 
        || (this->originalXYZ[1] != 0.0) 
        || (this->originalXYZ[2] != 0.0)) {
        return true;
    }
    return false;
}

/**
 * Set the focus' position.
 * 
 * @param xyz  New position of the focus.
 *
 */
void
SurfaceProjectedItem::setOriginalXYZ(const float xyz[3])
{
    this->originalXYZ[0] = xyz[0];
    this->originalXYZ[1] = xyz[1];
    this->originalXYZ[2] = xyz[2];
}

/**
 * Get the value of volumeXYZ
 *
 * @return the value of volumeXYZ
 *
 */
const float*
SurfaceProjectedItem::getVolumeXYZ() const
{
    return this->volumeXYZ;
}

/**
 * Get the volume XYZ coordinates.
 * @param xyzOut  Volume XYZ coordinates.
 *
 */
void
SurfaceProjectedItem::getVolumeXYZ(float xyzOut[3]) const
{
    xyzOut[0] = this->volumeXYZ[0];
    xyzOut[1] = this->volumeXYZ[1];
    xyzOut[2] = this->volumeXYZ[2];
}

/**
 * Get the validity of the volume XYZ coordinate.
 * @return Validity of volume XYZ coordinate.
 *
 */
bool
SurfaceProjectedItem::isVolumeXYZValid() const
{
    if ((this->originalXYZ[0] != 0.0) 
        || (this->originalXYZ[1] != 0.0) 
        || (this->originalXYZ[2] != 0.0)) {
        return true;
    }
    return false;
}

/**
 * Set the value of volumeXYZ
 *
 * @param volumeXYZ new value of volumeXYZ
 *
 */
void
SurfaceProjectedItem::setVolumeXYZ(const float volumeXYZ[3])
{
    this->volumeXYZ[0] = volumeXYZ[0];
    this->volumeXYZ[1] = volumeXYZ[1];
    this->volumeXYZ[2] = volumeXYZ[2];
}

/**
 * Get the value of projectionType
 *
 * @return the value of projectionType
 *
 */
SurfaceProjectionTypeEnum::Enum
SurfaceProjectedItem::getProjectionType() const
{
    return this->projectionType;
}

/**
 * Set the value of projectionType
 *
 * @param projectionType new value of projectionType
 *
 */
void
SurfaceProjectedItem::setProjectionType(const SurfaceProjectionTypeEnum::Enum projectionType)
{
    this->projectionType = projectionType;
}

/**
 * Get the structure of this projected item.
 * @return The structure.
 *
 */
StructureEnum::Enum
SurfaceProjectedItem::getStructure() const
{
    return this->structure;
}

/**
 * Set the structure of this projected item.
 * @param s - new structure.
 *
 */
void
SurfaceProjectedItem::setStructure(const StructureEnum::Enum structure)
{
    this->structure = structure;
}

/** 
 * @return the barycentric projection 
 */
SurfaceProjectionBarycentric* 
SurfaceProjectedItem::getBarycentricProjection()
{
    return this->barycentricProjection;
}

/** 
 * @return the barycentric projection 
 */
const SurfaceProjectionBarycentric* 
SurfaceProjectedItem::getBarycentricProjection() const
{
    return this->barycentricProjection;
}

/** 
 * @return the Van Essen projection 
 */
SurfaceProjectionVanEssen* 
SurfaceProjectedItem::getVanEssenProjection()
{
    return this->vanEssenProjection;
}

/** 
 * @return the Van Essen projection 
 */
const SurfaceProjectionVanEssen* 
SurfaceProjectedItem::getVanEssenProjection() const
{
    return this->vanEssenProjection;
}

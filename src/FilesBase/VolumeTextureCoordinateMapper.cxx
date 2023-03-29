
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __VOLUME_TEXTURE_COORDINATE_MAPPER_DECLARE__
#include "VolumeTextureCoordinateMapper.h"
#undef __VOLUME_TEXTURE_COORDINATE_MAPPER_DECLARE__

#include <iostream>

#include "CaretAssert.h"
#include "DataFile.h"
#include "CaretLogger.h"
#include "LinearEquationTransform.h"
#include "Vector3D.h"
#include "VolumeMappableInterface.h"

using namespace caret;


    
/**
 * \class caret::VolumeTextureCoordinateMapper 
 * \brief Convert volume coordinates to texture coordinates
 * \ingroup FilesBase
 */

/**
 * Constructor.
 */
VolumeTextureCoordinateMapper::VolumeTextureCoordinateMapper(const VolumeMappableInterface* volumeInterface)
: CaretObject(),
m_volumeInterface(volumeInterface)
{
    CaretAssert(volumeInterface);
    
    std::vector<int64_t> dims;
    volumeInterface->getDimensions(dims);
    if (dims.size() < 3) {
        return;
    }
    
    if ((dims[0] < 1)
        || (dims[1] < 1)
        || (dims[2] < 1)) {
        const DataFile* df(dynamic_cast<const DataFile*>(m_volumeInterface));
        AString filename;
        if (df != NULL) {
            filename = df->getFileName();
        }
        CaretLogWarning("Volume type file has a dimension less than one: "
                        + filename);
        return;
    }
    
    for (int32_t i = 0; i < 3; i++) {
        /*
         * Volume file may consist of single slice (a dimension is one)
         */
        const int64_t dimension(dims[i]);
        if (dimension > 1) {
            /*
             * Texture coordinates are at the EDGES of the voxel.
             * Texture coordinates range [0.0, 1.0]
             * Voxel coordinates and dimensions are at the CENTER of the voxel.
             * So, create a mapping from CENTER of voxel to texture coordinate at
             * the CENTER of the voxel.
             */
            const float textureCoordCenterFirstVoxel((1.0 / static_cast<float>(dimension))
                                               * 0.5);
            const float textureCoordCenterLastVoxel(1.0 - textureCoordCenterFirstVoxel);
            AString errorMessage;
            m_transforms[i] = LinearEquationTransform::newInstance(0,
                                                                   dimension - 1,
                                                                   textureCoordCenterFirstVoxel,
                                                                   textureCoordCenterLastVoxel,
                                                                   0,
                                                                   textureCoordCenterFirstVoxel,
                                                                   errorMessage);
        }
    }
    
    m_validFlag = true;
}

/**
 * Destructor.
 */
VolumeTextureCoordinateMapper::~VolumeTextureCoordinateMapper()
{
}

/**
 * @return True if this texture coordinate mapper is valid.
 */
bool
VolumeTextureCoordinateMapper::isValid() const
{
    return m_validFlag;
}

/**
 * @return A  texture STR coordinate converted from the XYZ coordinate
 * @param xyz
 *    Input geometric XYZ coordinate
 * @param strOut
 *    Output STR texture coordinate
 */
Vector3D
VolumeTextureCoordinateMapper::mapXyzToStr(const Vector3D& xyz) const
{
    /*
     * Initialize all texture coords to center of voxel
     */
    Vector3D strOut;
    strOut.fill(0.5);
    
    if (isValid()) {
        const VolumeSpace& volumeSpace = m_volumeInterface->getVolumeSpace();
        Vector3D ijk;
        volumeSpace.spaceToIndex(xyz, ijk);
        
        for (int32_t i = 0; i < 3; i++) {
            if (m_transforms[i]) {
                strOut[i] = m_transforms[i]->transform(ijk[i]);
            }
        }
    }

    return strOut;
}

/**
 * Test by converting the XYZ to a texture coordinate and printing the texture
 * coordinate to std::cout.
 * @param prefixText
 *    Text printed before results
 * @param xyz
 *    Input geometric XYZ coordinate
 */
void
VolumeTextureCoordinateMapper::testMapXyzToStr(const AString& prefixText,
                                               const Vector3D& xyz) const
{
    if ( ! isValid()) {
        std::cout << "VolumeTextureCoordinateMapper is invalid" << std::endl;
        return;
    }
    
    const VolumeSpace& volumeSpace = m_volumeInterface->getVolumeSpace();
    Vector3D ijk;
    volumeSpace.spaceToIndex(xyz, ijk);
    
    Vector3D str;
    for (int32_t i = 0; i < 3; i++) {
        if (m_transforms[i]) {
            str[i] = m_transforms[i]->transform(ijk[i]);
        }
    }

    const DataFile* df(dynamic_cast<const DataFile*>(m_volumeInterface));
    AString filename;
    if (df != NULL) {
        filename = df->getFileName();
    }
    std::cout << prefixText << " " << filename << std::endl;
    std::cout << "   "
    << " XYZ: " << xyz.toString(8)
    << " IJK: " << ijk.toString(3)
    << " STR: " << str.toString(8)
    << std::endl;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
VolumeTextureCoordinateMapper::toString() const
{
    return "VolumeTextureCoordinateMapper";
}


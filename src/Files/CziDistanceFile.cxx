
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __CZI_DISTANCE_FILE_H_DECLARE__
#include "CziDistanceFile.h"
#undef __CZI_DISTANCE_FILE_H_DECLARE__


#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "Matrix4x4.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class caret::CziDistanceFile
 * \brief Loads and uses a NIFTI file containing distance data for image
 * \ingroup Files
 */

/**
 * Constructor.
 * @param filename
 *    Name of file
 */
CziDistanceFile::CziDistanceFile(const AString& filename)
: CaretObject(),
m_filename(filename)
{
    
}

/**
 * Destructor.
 */
CziDistanceFile::~CziDistanceFile()
{
}

/**
 * @return Status of the transform
 */
CziDistanceFile::Status
CziDistanceFile::getStatus() const
{
    return m_status;
}

/**
 * @return Name of file used by this non-linear transform
 */
AString
CziDistanceFile::getFilename() const
{
    return m_filename;
}

/**
 * Load the NIFTI file with the given name
 * @param filename
 *    Name of NIFTI file
 */
void
CziDistanceFile::load(const AString& filename) const
{
    switch (m_status) {
        case Status::INVALID:
            return;
            break;
        case Status::UNREAD:
            break;
        case Status::VALID:
            return;
            break;
    }
    
    m_status = Status::INVALID;
    
    m_niftiFile.reset(new VolumeFile());
    try {
        m_niftiFile->readFile(filename);
        
        std::vector<int64_t> dims;
        m_niftiFile->getDimensions(dims);
        if (dims.size() < 5) {
            throw DataFileException("Dimensions should be 5 but are "
                                    + AString::number(dims.size()));
        }
        
        if (dims[3] != 1) {
            throw DataFileException("4th dimension should be 1 but is "
                                    + AString::number(dims[3]));
        }
        
        std::vector<std::vector<float>> sform(m_niftiFile->getSform());
        m_sformMatrix.reset(new Matrix4x4(sform));
        
        /*
         * Generate inverse of sform matrix
         */
        m_inverseSformMatrix.reset(new Matrix4x4(*m_sformMatrix));
        if ( ! m_inverseSformMatrix->invert()) {
            throw DataFileException("Failed to invert sform matrix: "
                                    + m_sformMatrix->toFormattedString("  "));
        }

        m_status = Status::VALID;
        
        std::cout << getFilename() << std::endl;
        std::cout << "   Distance File W=" << dims[0] << ", H=" << dims[1] << std::endl;
        
        
        Vector3D planeTopLeft(0.0, 0.0, 0.0);
        m_sformMatrix->multiplyPoint3(planeTopLeft);
        std::cout << "   Plane top left: " << planeTopLeft.toString(5) << std::endl;
        
        Vector3D planeBottomRight(dims[0], dims[1], 0.0);
        m_sformMatrix->multiplyPoint3(planeBottomRight);
        std::cout << "   Plane bottom right: " << planeBottomRight.toString(5) << std::endl;
        
    }
    catch (const DataFileException& dfe) {
        CaretLogWarning("Failed to read "
                        + filename
                        + " "
                        + " ERROR: "
                        + dfe.whatString());
        m_niftiFile.reset();
    }
}

/**
 * @param planeXYZ
 *    The plane coordinate
 * @param distanceValueOut
 *    The output distance value
 * @return True if output distance value is valid, else false.
 */
bool
CziDistanceFile::getDistanceValue(const Vector3D& planeXYZ,
                                  float& distanceValueOut) const
{
    distanceValueOut = 0.0;
    
    if (m_status == Status::UNREAD) {
        load(m_filename);
    }

    if (m_status == Status::VALID) {
        Vector3D indexIJK(planeXYZ);
        m_inverseSformMatrix->multiplyPoint3(indexIJK);
        const int64_t niftiI(static_cast<int64_t>(indexIJK[0]));
        const int64_t niftiJ(static_cast<int64_t>(indexIJK[1]));
        if (m_debugFlag) {
            std::cout << "TO millimeters Index: " << niftiI << ", " << niftiJ << std::endl;
        }
        
        const int64_t niftiK(0);
        if (m_niftiFile->indexValid(niftiI, niftiJ, niftiK)) {
            /*
             * Use pixel index to obtain non-linearity from NIFTI data
             */
            distanceValueOut = m_niftiFile->getValue(niftiI, niftiJ, niftiK, 0);
            return true;
        }
    }
    
    return false;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CziDistanceFile::toString() const
{
    return "CziDistanceFile";
}


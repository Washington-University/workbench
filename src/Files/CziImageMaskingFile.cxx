
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

#define __CZI_IMAGE_MASKING_FILE_H_DECLARE__
#include "CziImageMaskingFile.h"
#undef __CZI_IMAGE_MASKING_FILE_H_DECLARE__


#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "Matrix4x4.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class caret::CziImageMaskingFile
 * \brief Loads and uses a NIFTI file containing masking data for image
 * \ingroup Files
 */

/**
 * Constructor.
 * @param filename
 *    Name of file
 */
CziImageMaskingFile::CziImageMaskingFile(const AString& filename)
: CaretObject(),
m_filename(filename)
{
    
}

/**
 * Destructor.
 */
CziImageMaskingFile::~CziImageMaskingFile()
{
}

/**
 * @return Status of the transform
 */
CziImageMaskingFile::Status
CziImageMaskingFile::getStatus() const
{
    return m_status;
}

/**
 * @return Name of file used by this non-linear transform
 */
AString
CziImageMaskingFile::getFilename() const
{
    return m_filename;
}

/**
 * Load the NIFTI file with the given name
 * @param filename
 *    Name of NIFTI file
 */
void
CziImageMaskingFile::load(const AString& filename) const
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
//        m_dimensionX   = dims[0];
//        m_dimensionY   = dims[1];
//        m_numberOfMaps = dims[3];
        
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
 * @return The masking value for the given plane coordinate
 * @param planeXYZ
 *    The plane coordinate
 * @param defaultValue
 *    Value returned if a masking value is unavailable for the given plane coordinate
 */
float
CziImageMaskingFile::getMaskingValue(const Vector3D& planeXYZ,
                                     const float defaultValue) const
{
    float maskingValue(defaultValue);
    
    if (m_status == Status::UNREAD) {
        load(m_filename);
    }

    if (m_status == Status::VALID) {
        Vector3D indexIJK(planeXYZ);
        m_inverseSformMatrix->multiplyPoint3(indexIJK);
        const int64_t niftiI(static_cast<int64_t>(indexIJK[0]));
        const int64_t niftiJ(static_cast<int64_t>(indexIJK[1]));
//        const int64_t indexK(static_cast<int64_t>(indexIJK[2]));
        if (m_debugFlag) {
            std::cout << "TO millimeters Index: " << niftiI << ", " << niftiJ << std::endl;
        }
        
        const int64_t niftiK(0);
        if (m_niftiFile->indexValid(niftiI, niftiJ, niftiK)) {
            /*
             * Use pixel index to obtain non-linearity from NIFTI data
             */
            maskingValue = m_niftiFile->getValue(niftiI, niftiJ, niftiK, 0);
        }
        else {
            CaretLogWarning("("
                            + AString::number(niftiI)
                            + ", "
                            + AString::number(niftiJ)
                            + ", "
                            + AString::number(niftiK)
                            + ") is not a valid NIFTI index");
        }
    }
    
    return maskingValue;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CziImageMaskingFile::toString() const
{
    return "CziImageMaskingFile";
}


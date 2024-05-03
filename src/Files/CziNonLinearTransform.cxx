
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

#define __CZI_NON_LINEAR_TRANSFORM_DECLARE__
#include "CziNonLinearTransform.h"
#undef __CZI_NON_LINEAR_TRANSFORM_DECLARE__


#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DeveloperFlagsEnum.h"
#include "DataFileException.h"
#include "FileInformation.h"
#include "Matrix4x4.h"
#include "VolumeFile.h"

using namespace caret;

/**
 * \class caret::CziNonLinearTransform
 * \brief Loads and uses a NIFTI file containing non-linear transform information.
 * \ingroup Files
 */

/**
 * Constructor.
 * @param mode
 *    Mode from/to millimeters
 * @param filename
 *    Name of file
 */
CziNonLinearTransform::CziNonLinearTransform(const Mode mode,
                                             const AString& filename)
: CaretObject(),
m_mode(mode),
m_filename(filename)
{
    
}

/**
 * Destructor.
 */
CziNonLinearTransform::~CziNonLinearTransform()
{
}

/**
 * @return Status of the transform
 */
CziNonLinearTransform::Status
CziNonLinearTransform::getStatus() const
{
    return m_status;
}

/**
 * @return Name of file used by this non-linear transform
 */
AString
CziNonLinearTransform::getFilename() const
{
    return m_filename;
}

/**
 * Load the NIFTI file with the given name
 * @param filename
 *    Name of NIFTI file
 */
void
CziNonLinearTransform::load(const AString& filename)
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
        
        if ((dims[3] < 2)
            || (dims[3] > 3)) {
            throw DataFileException("4th dimension should be 2 to 3 but is "
                                    + AString::number(dims[3]));
        }
        m_dimensionX   = dims[0];
        m_dimensionY   = dims[1];
        m_numberOfMaps = dims[3];
        
        VolumeSpace::OrientTypes orientation[3];
        m_niftiFile->getOrientation(orientation);
        
        std::vector<AString> orientationNames(3);
        for (int32_t i = 0; i < 3; i++) {
            AString orientName;
            
            switch (orientation[i]) {
                case VolumeSpace::ANTERIOR_TO_POSTERIOR:
                    orientName = "Anterior to Posterior";
                    break;
                case VolumeSpace::INFERIOR_TO_SUPERIOR:
                    orientName = "Inferior to Superior";
                    break;
                case VolumeSpace::LEFT_TO_RIGHT:
                    orientName = "Left to Right";
                    break;
                case VolumeSpace::POSTERIOR_TO_ANTERIOR:
                    orientName = "Posterior to Anterior";
                    break;
                case VolumeSpace::RIGHT_TO_LEFT:
                    orientName = "Right to Left";
                    break;
                case VolumeSpace::SUPERIOR_TO_INFERIOR:
                    orientName = "Superior to Inferior";
                    break;
            }
            orientationNames[i] = orientName;
        }
        
        AString orientationErrorMessage;
        if (orientation[0] == VolumeSpace::LEFT_TO_RIGHT) {
            m_xLeftToRightFlag = true;
        }
        else if (orientation[0] == VolumeSpace::RIGHT_TO_LEFT) {
            m_xLeftToRightFlag = false;
        }
        else {
            orientationErrorMessage.appendWithNewLine("Orientation for first axis "
                                                      + orientationNames[0]
                                                      + " not supported.  Should be a right/left orientation");
        }
        if (orientation[1] == VolumeSpace::ANTERIOR_TO_POSTERIOR) {
            m_yTopToBottomFlag = true;
        }
        else if (orientation[1] == VolumeSpace::POSTERIOR_TO_ANTERIOR) {
            m_yTopToBottomFlag = false;
        }
        else {
            orientationErrorMessage.appendWithNewLine("Orientation for first axis "
                                                      + orientationNames[0]
                                                      + " not supported.  Should be a anterior/posterior orientation");
        }
        
        if ( ! orientationErrorMessage.isEmpty()) {
            throw DataFileException(orientationErrorMessage);
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

        AString pixelStepText;
        if (CaretLogger::getLogger()->isFine()) {
            Vector3D p1 { 0.0, 0.0, 0.0 };
            m_sformMatrix->multiplyPoint3(p1);
            Vector3D p2 { 1.0, 1.0, 1.0 };
            m_sformMatrix->multiplyPoint3(p2);
            Vector3D stepXYZ {
                p2[0] - p1[0],
                p2[1] - p1[1],
                p2[2] - p1[2],
            };
            pixelStepText = ("Pixel Step (0, 0, 0) to (1, 1, 1): "
                             + AString::fromNumbers(stepXYZ));
        }
            
        m_status = Status::VALID;

        
        {
            AString modeName;
            switch (m_mode) {
                case Mode::FROM_MILLIMETERS:
                    modeName = "FROM_MILLIMETERS";
                    break;
                case Mode::TO_MILLIMETERS:
                    modeName = "TO_MILLIMETERS";
                    break;
            }
            
            if (s_debugFlag) {
                std::cout << "Mode: " << modeName << std::endl;
                std::cout << "   NIFTI File: " << filename << std::endl;
                std::cout << "   Dimensions:";
                for (int32_t i = 0; i < 3; i++) {
                    std::cout << dims[i] << " ";
                }
                std::cout << std::endl;
                std::cout << "   Orientations:";
                for (int32_t i = 0; i < 3; i++) {
                    std::cout << orientationNames[i] << " ";
                }
                std::cout << "   " << pixelStepText << std::endl;
                std::cout << std::endl;
            }
        }
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
 * Get the non-linear offsets for the given coordinate
 * @param xyz
 *    Input coordinate, plane for "to millimeters"
 * @param offsetXyzOut
 *    Output with offsets
 */
void
CziNonLinearTransform::getNonLinearOffset(const Vector3D& xyz,
                                          Vector3D& offsetXyzOut)
{
    offsetXyzOut[0] = 0.0;
    offsetXyzOut[1] = 0.0;
    offsetXyzOut[2] = 0.0;
    
    if ( ! DeveloperFlagsEnum::isFlag(DeveloperFlagsEnum::DEVELOPER_FLAG_HISTOLOGY_CORRECT_FOR_NON_LINEAR_DISTORTION)) {
        /*
         * Non-linear offsets disabled
         */
        return;
    }
    
    if (m_status == Status::UNREAD) {
        load(m_filename);
    }
    
    if (m_status == Status::VALID) {
        switch (m_mode) {
            case Mode::FROM_MILLIMETERS:
                getNonLinearOffsetFromMillimeters(xyz,
                                                  offsetXyzOut);
                break;
            case Mode::TO_MILLIMETERS:
                getNonLinearOffsetToMillimeters(xyz,
                                                offsetXyzOut);
                break;
        }
    }
}

/**
 * Get the non-linear offsets for the given coordinate for a "from millimeters" transform
 * @param stereotaxicXYZ   Is it PlaneXYZ?
 *    Input coordinate
 * @param offsetXyzOut
 *    Output with offsets
 */
void
CziNonLinearTransform::getNonLinearOffsetFromMillimeters(const Vector3D& stereotaxicXYZ,
                                                         Vector3D& offsetXyzOut)
{
    Vector3D indexIJK(stereotaxicXYZ);
    m_inverseSformMatrix->multiplyPoint3(indexIJK);
    int64_t indexI(static_cast<int64_t>(indexIJK[0]));
    const int64_t indexJ(static_cast<int64_t>(indexIJK[1]));
    const int64_t indexK(static_cast<int64_t>(indexIJK[2]));
    if (s_debugFlag) {
        std::cout << "FROM millimeters Index: " << indexI << ", " << indexJ << ", " << indexK << std::endl;
    }

    if ((indexI >= 0)
        && (indexI < m_dimensionX)
        && (indexJ >= 0)
        && (indexJ < m_dimensionY)) {
        CaretAssert((indexI >= 0)
                    && (indexI < m_dimensionX)
                    && (indexJ >= 0)
                    && (indexJ < m_dimensionY));
        
        const int64_t niftiI(indexI);
        const int64_t niftiJ(indexJ);
        const int64_t niftiK(0);
        if (m_niftiFile->indexValid(niftiI, niftiJ, niftiK)) {
            /*
             * Use pixel index to obtain non-linearity from NIFTI data
             */
            offsetXyzOut[0] = m_niftiFile->getValue(niftiI, niftiJ, niftiK, 0);
            offsetXyzOut[1] = m_niftiFile->getValue(niftiI, niftiJ, niftiK, 1);
            if (m_numberOfMaps >= 3) {
                offsetXyzOut[2] = m_niftiFile->getValue(niftiI, niftiJ, niftiK, 2);
            }
        }
        else {
            CaretLogFine("("
                         + AString::number(niftiI)
                         + ", "
                         + AString::number(niftiJ)
                         + ", "
                         + AString::number(niftiK)
                         + ") is not a valid pixel index");
        }
    }
    else {
        CaretLogFine("("
                        + AString::number(indexI)
                        + ", "
                        + AString::number(indexJ)
                        + ", "
                        + AString::number(indexK)
                        + ") is not a valid NIFTI index into " + m_niftiFile->getFileName());
        
    }
}

/**
 * Get the non-linear offsets for the given coordinate for a "to millimeters" transform
 * @param planeXYZ
 *    Input coordinate
 * @param offsetXyzOut
 *    Output with offsets
 */
void
CziNonLinearTransform::getNonLinearOffsetToMillimeters(const Vector3D& planeXYZ,
                                                                  Vector3D& offsetXyzOut)
{
    Vector3D indexIJK(planeXYZ);
    m_inverseSformMatrix->multiplyPoint3(indexIJK);
    const int64_t indexI(static_cast<int64_t>(indexIJK[0]));
    const int64_t indexJ(static_cast<int64_t>(indexIJK[1]));
    const int64_t indexK(static_cast<int64_t>(indexIJK[2]));
    if (s_debugFlag) {
        std::cout << "TO millimeters Index: " << indexI << ", " << indexJ << ", " << indexK << std::endl;
    }

    if ((indexI >= 0)
        && (indexI < m_dimensionX)
        && (indexJ >= 0)
        && (indexJ < m_dimensionY)) {
        
        const int64_t niftiI(indexI);
        const int64_t niftiJ(indexJ);
        const int64_t niftiK(0);
        if (m_niftiFile->indexValid(niftiI, niftiJ, niftiK)) {
            /*
             * Use pixel index to obtain non-linearity from NIFTI data
             */
            offsetXyzOut[0] = m_niftiFile->getValue(niftiI, niftiJ, niftiK, 0);
            offsetXyzOut[1] = m_niftiFile->getValue(niftiI, niftiJ, niftiK, 1);
            if (m_numberOfMaps >= 3) {
                offsetXyzOut[2] = m_niftiFile->getValue(niftiI, niftiJ, niftiK, 2);
            }
        }
        else {
            CaretLogFine("("
                         + AString::number(niftiI)
                         + ", "
                         + AString::number(niftiJ)
                         + ", "
                         + AString::number(niftiK)
                         + ") is not a valid NIFTI index");
        }
    }
    else {
        CaretLogFine("("
                        + AString::number(indexI)
                        + ", "
                        + AString::number(indexJ)
                        + ", "
                        + AString::number(indexK)
                        + ") is not a valid NIFTI index into " + m_niftiFile->getFileName());

    }
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
CziNonLinearTransform::toString() const
{
    return "CziNonLinearTransform";
}


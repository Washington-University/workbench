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

#include "GiftiFile.h"
#include "MathFunctions.h"
#include "SurfaceFile.h"

using namespace caret;

/**
 * Constructor.
 */
SurfaceFile::SurfaceFile()
: GiftiTypeFile()
{
    this->initializeMembersSurfaceFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
SurfaceFile::SurfaceFile(const SurfaceFile& sf)
: GiftiTypeFile(sf)
{
    this->copyHelperSurfaceFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the SurfaceFile parameter.
 */
SurfaceFile& 
SurfaceFile::operator=(const SurfaceFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperSurfaceFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
SurfaceFile::~SurfaceFile()
{
    
}

/**
 * Clear the surface file.
 */
void 
SurfaceFile::clear()
{
    GiftiTypeFile::clear();
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
SurfaceFile::validateDataArraysAfterReading() throw (DataFileException)
{
    this->initializeMembersSurfaceFile();
    
    int numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays != 2) {
        throw new DataFileException("Number of data arrays MUST be two in a SurfaceFile.");
    }
    
    /*
     * Find the coordinate and topology data arrays.
     */
    for (int i = 0; i < numDataArrays; i++) {
        GiftiDataArray* gda = this->giftiFile->getDataArray(i);
        if (gda->getIntent() == NiftiIntentEnum::NIFTI_INTENT_POINTSET) {
            if (gda->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32) {
                if (gda->getNumberOfDimensions() == 2) {
                    int64_t dim0 = gda->getDimension(0);
                    int64_t dim1 = gda->getDimension(1);
                    if ((dim0 > 0) && (dim1 == 3)) {
                        this->coordinateDataArray = gda;
                        this->coordinatePointer = gda->getDataPointerFloat();
                    }
                }
            }
        }
        else if (gda->getIntent() == NiftiIntentEnum::NIFTI_INTENT_TRIANGLE) {
            if (gda->getDataType() == NiftiDataTypeEnum::NIFTI_TYPE_INT32) {
                if (gda->getNumberOfDimensions() == 2) {
                    int64_t dim0 = gda->getDimension(0);
                    int64_t dim1 = gda->getDimension(1);
                    if ((dim0 > 0) && (dim1 == 3)) {
                        this->triangleDataArray = gda;
                        this->trianglePointer = gda->getDataPointerInt();
                    }
                }
            }
        }
        
    }
    
    QString errorMessage;
    if (this->coordinateDataArray == NULL) {
        if (errorMessage.isEmpty() == false) {
        }
        errorMessage += "Unable to find coordinate data array which "
            " contains data type FLOAT32, Intent POINTSET, and two "
            " dimensions with the second dimension set to three.  ";
    }
    if (this->triangleDataArray == NULL) {
        errorMessage += "Unable to find topology data array which "
        " contains data type INT32, Intent TRIANGLE, and two "
        " dimensions with the second dimension set to three.";
    }
    if (errorMessage.isEmpty() == false) {
        throw DataFileException(errorMessage);
    }
    
    this->computeNormals();
}

/**
 * Get the number of coordinates.
 *
 * @return
 *    The number of coordinates.
 */
int 
SurfaceFile::getNumberOfCoordinates() const
{
    if (this->coordinatePointer == NULL) {
        return 0;
    }
    CaretAssert(this->coordinateDataArray);
    return this->coordinateDataArray->getDimension(0);
}

/**
 * Get a coordinate.
 *
 * @param
 *    Index of coordinate.
 *
 * @return
 *    Pointer to memory containing the XYZ coordinate.
 */
const float* 
SurfaceFile::getCoordinate(const int32_t indx) const
{
    CaretAssert(this->coordinatePointer);
    const int32_t offset = indx * 3;
    CaretAssert((offset >= 0) && (offset < (this->getNumberOfCoordinates() * 3)));
    return &(this->coordinatePointer[offset]);    
}

/**
 * Get the number of triangles.
 *
 * @return
 *    Number of triangles.
 */
int 
SurfaceFile::getNumberOfTriangles() const
{
    if (this->trianglePointer == NULL) {
        return 0;
    }
    CaretAssert(this->triangleDataArray);
    return this->triangleDataArray->getDimension(0);
    
}

/**
 * Get a triangle.
 *
 * @param indx
 *    Index of triangle.
 *
 * @return
 *    Pointer to memory containing the three nodes
 *    in the triangle.
 */
const int32_t* 
SurfaceFile::getTriangle(const int32_t indx) const
{
    CaretAssert(this->trianglePointer);
    const int32_t offset = indx * 3;
    CaretAssert((offset >= 0) && (offset < (this->getNumberOfTriangles() * 3)));
    return &(this->trianglePointer[offset]);    
}

/**
 * Initialize members of this class.
 */
void 
SurfaceFile::initializeMembersSurfaceFile()
{
    this->coordinateDataArray = NULL;
    this->coordinatePointer   = NULL;
    this->triangleDataArray   = NULL;
    this->trianglePointer     = NULL;
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
SurfaceFile::copyHelperSurfaceFile(const SurfaceFile& sf)
{
    this->validateDataArraysAfterReading();
}

/**
 * Get a normal vector for a coordinate.
 *
 * @param
 *    Index of coordinate.
 *
 * @return
 *    Pointer to memory containing the normal vector.
 */
const float* 
SurfaceFile::getNormalVector(const int32_t indx) const
{
    const int32_t offset = indx * 3;
    CaretAssert((offset >= 0) && (offset < static_cast<int>(this->normalVectors.size())));
    return &(this->normalVectors[offset]);    
}

/**
 * Compute surface normals.
 */
void 
SurfaceFile::computeNormals()
{
    int32_t numCoords = this->getNumberOfCoordinates();
    if (numCoords > 0) {
        this->normalVectors.resize(numCoords * 3);
    }
    else {
        this->normalVectors.clear();
    }
    
    const int32_t numTriangles = this->getNumberOfTriangles();
    if ((numCoords > 0) && (numTriangles > 0)) {
        float* normalPointer = &this->normalVectors[0];
        std::vector<float> numContribute(numCoords, 0.0f);

        float triangleNormal[3];

        for (int32_t i = 0; i < numTriangles; i++) {
            const int32_t it3 = i * 3;
            const int n1 = this->trianglePointer[it3];
            const int n2 = this->trianglePointer[it3 + 1];
            const int n3 = this->trianglePointer[it3 + 2];
            const int32_t c1 = n1 * 3;
            const int32_t c2 = n2 * 3;
            const int32_t c3 = n3 * 3;
            if ((n1 >= 0)
                && (n2 >= 0)
                && (n3 >= 0)) {
                
                MathFunctions::normalVector(&this->coordinatePointer[c1],
                                            &this->coordinatePointer[c2],
                                            &this->coordinatePointer[c3],
                                            triangleNormal);
                
                normalPointer[c1 + 0] += triangleNormal[0];
                normalPointer[c1 + 1] += triangleNormal[1];
                normalPointer[c1 + 2] += triangleNormal[2];
                numContribute[n1] += 1.0;
                normalPointer[c2 + 0] += triangleNormal[0];
                normalPointer[c2 + 1] += triangleNormal[1];
                normalPointer[c2 + 2] += triangleNormal[2];
                numContribute[n2] += 1.0;
                normalPointer[c3 + 0] += triangleNormal[0];
                normalPointer[c3 + 1] += triangleNormal[1];
                normalPointer[c3 + 2] += triangleNormal[2];
                numContribute[n3] += 1.0;
            }
        }
        
        float normal[3];
        for (int32_t i = 0; i < numCoords; i++) {
            if (numContribute[i] > 0.0) {
                const int32_t i3 = i * 3;
                normal[0] = normalPointer[i3 + 0] / numContribute[i];
                normal[1] = normalPointer[i3 + 1] / numContribute[i];
                normal[2] = normalPointer[i3 + 2] / numContribute[i];
                MathFunctions::normalizeVector(normal);
                normalPointer[i3 + 0] = normal[0];
                normalPointer[i3 + 1] = normal[1];
                normalPointer[i3 + 2] = normal[2];
            }
        }
    }
}


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
#include "RgbaFile.h"

using namespace caret;

/**
 * Constructor.
 */
RgbaFile::RgbaFile()
: GiftiTypeFile()
{
    this->initializeMembersRgbaFile();
}

/**
 * Copy constructor.
 *
 * @param sf
 *     Surface file that is copied.
 */
RgbaFile::RgbaFile(const RgbaFile& sf)
: GiftiTypeFile(sf)
{
    this->copyHelperRgbaFile(sf);
}


/**
 * Assignment operator.
 *
 * @param sf
 *     Surface file that is copied.
 * @return
 *     This surface file with content replaced
 *     by the RgbaFile parameter.
 */
RgbaFile& 
RgbaFile::operator=(const RgbaFile& sf)
{
    if (this != &sf) {
        GiftiTypeFile::operator=(sf);
        this->copyHelperRgbaFile(sf);
    }
    return *this;
}

/**
 * Destructor.
 */
RgbaFile::~RgbaFile()
{
    
}

/**
 * Clear the surface file.
 */
void 
RgbaFile::clear()
{
    GiftiTypeFile::clear();
}

/**
 * Validate the contents of the file after it
 * has been read such as correct number of 
 * data arrays and proper data types/dimensions.
 */
void 
RgbaFile::validateDataArraysAfterReading() throw (DataFileException)
{
    this->initializeMembersRgbaFile();
    
    int numDataArrays = this->giftiFile->getNumberOfDataArrays();
    if (numDataArrays != 2) {
        throw DataFileException("Number of data arrays MUST be two in a RgbaFile.");
    }
    
    /*
     * Find the coordinate and topology data arrays.
     */
    /*
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
    
    AString errorMessage;
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

    const int64_t numNodes = this->getNumberOfCoordinates();
    const uint64_t numColorComponents = numNodes * 4;
    
    if (numColorComponents != this->nodeColoring.size()) {
        this->nodeColoring.resize(numColorComponents);
        
        for (int64_t i = 0; i < numNodes; i++) {
            const int64_t i4 = i * 4;
            this->nodeColoring[i4]   = 0.75f;
            this->nodeColoring[i4+1] = 0.75f;
            this->nodeColoring[i4+2] = 0.75f;
            this->nodeColoring[i4+3] = 1.0f;
        }
    }
    */
}

/**
 * Get the number of nodes.
 *
 * @return
 *    The number of nodes.
 */
int 
RgbaFile::getNumberOfNodes() const
{
        return 0;
}

/**
 * Initialize members of this class.
 */
void 
RgbaFile::initializeMembersRgbaFile()
{
}

/**
 * Helps copying files.
 *
 * @param sf
 *    File that is copied.
 */
void 
RgbaFile::copyHelperRgbaFile(const RgbaFile& sf)
{
    this->validateDataArraysAfterReading();
}



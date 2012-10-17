
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __CIFTI_FIBER_ORIENTATION_DECLARE__
#include "CiftiFiberOrientationAdapter.h"
#undef __CIFTI_FIBER_ORIENTATION_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiInterface.h"
#include "CiftiXML.h"
#include "ConnectivityLoaderFile.h"
#include "Fiber.h"
#include "FiberOrientation.h"
#include "MathFunctions.h"
#include "VolumeFile.h"

#include <limits>

using namespace caret;


    
/**
 * \class caret::CiftiFiberOrientationAdapter 
 * \brief Wraps around a CIFTI file to simplify access to fiber orientations
 */

/**
 * Constructor.
 */
CiftiFiberOrientationAdapter::CiftiFiberOrientationAdapter()
: CaretObject()
{
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = true;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayStatusInTab[i] = true;
    }
}

/**
 * Destructor.
 */
CiftiFiberOrientationAdapter::~CiftiFiberOrientationAdapter()
{
    for (std::vector<FiberOrientation*>::iterator iter = m_fiberOrientations.begin();
         iter != m_fiberOrientations.end();
         iter++) {
        delete *iter;
    }
    m_fiberOrientations.clear();
}

/**
 * Initialize the contents of this file with the content of the
 * given connectivity loader file.
 * @param clf
 *     The connectivity loader file.
 * @throws
 *     DataFileException if there is an error.
 */
void
CiftiFiberOrientationAdapter::initializeWithConnectivityLoaderFile(ConnectivityLoaderFile* clf) throw (DataFileException)
{
    CaretAssert(clf);
    
    CiftiInterface* interface = clf->ciftiInterface;
    CaretAssert(clf->ciftiInterface);
    
    const int64_t numRows = interface->getNumberOfRows();
    if (numRows <= 0) {
        throw DataFileException(clf->getFileNameNoPath()
                                + " does not contain any data (no rows)");
    }
    const int64_t numCols = interface->getNumberOfColumns();
    if (numCols <= 0) {
        throw DataFileException(clf->getFileNameNoPath()
                                + " does not contain any data (no columns)");
    }
    
    /*
     * Each set of fibers contains XYZ (3 elements) 
     * plus number of elements per fiber.
     */
    const int64_t numberOfFibers = ((numCols - FiberOrientation::NUMBER_OF_ELEMENTS_IN_FILE) /
                                    Fiber::NUMBER_OF_ELEMENTS_PER_FIBER_IN_FILE);
    const int64_t expectedNumberOfColumns =
        (numberOfFibers * Fiber::NUMBER_OF_ELEMENTS_PER_FIBER_IN_FILE) + FiberOrientation::NUMBER_OF_ELEMENTS_IN_FILE;
    if (expectedNumberOfColumns != numCols) {
        throw DataFileException("Validation of column count failed: expected "
                                + AString::number(expectedNumberOfColumns)
                                + " but have "
                                + AString::number(numCols)
                                + " columns.");
    }
    
    /*
     * Create the fiber groups
     */
    std::vector<float> rowData(numCols);
    float* rowPointer = &rowData[0];
    m_fiberOrientations.reserve(numRows);
    for (int64_t i = 0; i < numRows; i++) {
        clf->ciftiInterface->getRow(rowPointer, i);
        FiberOrientation* fiberOrient = new FiberOrientation(numberOfFibers,
                                                             rowPointer);
        if (fiberOrient->m_valid) {
            m_fiberOrientations.push_back(fiberOrient);
        }
        else {
            CaretLogSevere("Fiber invalid at row "
                           + QString::number(i)
                           + " is invalid: "
                           + fiberOrient->m_invalidMessage);
            delete fiberOrient;
        }
    }
    
    const CiftiXML& ciftiXML = clf->ciftiInterface->getCiftiXML();
    VolumeFile::OrientTypes orient[3];
    int64_t dims[3];
    float origin[3];
    ciftiXML.getVolumeAttributesForPlumb(orient,
                                         dims,
                                         origin,
                                         m_volumeSpacing);
}

/**
 * Initialize with test data.
 */
void
CiftiFiberOrientationAdapter::initializeWithTestData()
{
    const int64_t fiberDataSizeInFloats = (Fiber::NUMBER_OF_ELEMENTS_PER_FIBER_IN_FILE * 3) + 3;
    
    {
        float* fiberData = new float[fiberDataSizeInFloats];
        int64_t offset = 0;

        /*
         * Coordinate of fiber orientation
         */
        fiberData[offset+0] = 12.8;
        fiberData[offset+1] = 125.8;
        fiberData[offset+2] = 2.4;
        offset += 3;

        /*
         * Along Positive X-Axis
         */
        fiberData[offset+0] = 0.3;  // meanF
        fiberData[offset+1] = 2.0;   // varF
        fiberData[offset+2] = MathFunctions::toRadians(90.0); // theta
        fiberData[offset+3] = 0.0;   // phi
        fiberData[offset+4] = MathFunctions::toRadians(50.0);   // k1
        fiberData[offset+5] = MathFunctions::toRadians(10.0);   // k2
        fiberData[offset+6] = 0.0;   // psi
        offset += 7;

        /*
         * Along Positive Y-Axis
         */
        fiberData[offset+0] = 0.6;  // meanF
        fiberData[offset+1] = 2.0;   // varF
        fiberData[offset+2] = MathFunctions::toRadians(90.0); // theta
        fiberData[offset+3] = MathFunctions::toRadians(90.0);   // phi
        fiberData[offset+4] = MathFunctions::toRadians(50.0);   // k1
        fiberData[offset+5] = MathFunctions::toRadians(10.0);   // k2
        fiberData[offset+6] = MathFunctions::toRadians(20.0);   // psi
        offset += 7;

        /*
         * Along Positive Z-Axis
         */
        fiberData[offset+0] = 1.0;  // meanF
        fiberData[offset+1] = 2.0;   // varF
        fiberData[offset+2] = 0.0; // theta
        fiberData[offset+3] = 0.0;   // phi
        fiberData[offset+4] = MathFunctions::toRadians(20.0);   // k1
        fiberData[offset+5] = MathFunctions::toRadians(10.0);   // k2
        fiberData[offset+6] = MathFunctions::toRadians(70.0);   // psi
        offset += 7;
        
        FiberOrientation* fiberOrientation = new FiberOrientation(3,
                                                                  fiberData);
        
        m_fiberOrientations.push_back(fiberOrientation);
    }

    
    {
        float* fiberData = new float[fiberDataSizeInFloats];
        int64_t offset = 0;
        
        /*
         * Coordinate of fiber orientation
         */
        fiberData[offset+0] = -60.8;
        fiberData[offset+1] = -55.8;
        fiberData[offset+2] = -2.4;
        offset += 3;
        
        /*
         * Pointing towards forward right and up
         */
        fiberData[offset+0] = 0.3;  // meanF
        fiberData[offset+1] = 2.0;   // varF
        fiberData[offset+2] = MathFunctions::toRadians(45.0); // theta
        fiberData[offset+3] = MathFunctions::toRadians(45.0);   // phi
        fiberData[offset+4] = MathFunctions::toRadians(40.0);   // k1
        fiberData[offset+5] = MathFunctions::toRadians(10.0);   // k2
        fiberData[offset+6] = 0.0;   // psi
        offset += 7;
        
        /*
         * Pointing towards forward left and down
         */
        fiberData[offset+0] = 0.6;  // meanF
        fiberData[offset+1] = 2.0;   // varF
        fiberData[offset+2] = MathFunctions::toRadians(45.0); // theta
        fiberData[offset+3] = MathFunctions::toRadians(135.0);   // phi
        fiberData[offset+4] = MathFunctions::toRadians(20.0);   // k1
        fiberData[offset+5] = MathFunctions::toRadians(15.0);   // k2
        fiberData[offset+6] = 0.0;   // psi
        offset += 7;
        
        /*
         * Pointing towards backward right and up
         */
        fiberData[offset+0] = 1.0;  // meanF
        fiberData[offset+1] = 2.0;   // varF
        fiberData[offset+2] = MathFunctions::toRadians( 45.0); // theta
        fiberData[offset+3] = MathFunctions::toRadians(-45.0);   // phi
        fiberData[offset+4] = MathFunctions::toRadians(40.0);   // k1
        fiberData[offset+5] = MathFunctions::toRadians(20.0);   // k2
        fiberData[offset+6] = MathFunctions::toRadians(25.0);   // psi
        offset += 7;
        
        FiberOrientation* fiberOrientation = new FiberOrientation(3,
                                                                  fiberData);
        
        m_fiberOrientations.push_back(fiberOrientation);
    }
}

/**
 * @return The number of orientation fiber groups.
 */
int64_t
CiftiFiberOrientationAdapter::getNumberOfFiberOrientations() const
{
    return m_fiberOrientations.size();
}

/**
 * Get the orientation fiber group at the given index.
 * @param indx
 *     Index of the desired fiber orientation group.
 */
FiberOrientation*
CiftiFiberOrientationAdapter::getFiberOrientations(const int64_t indx)
{
    return m_fiberOrientations[indx];
}

/**
 * Get Fiber orientation nearest coordinate and within the maximum
 * distance. 
 *
 * @param xyz
 *     The coordinate.
 * @param maximumDistance
 *     The maximum distance.  If negative, any distance is allowed.
 *
 * @return Fiber found or NULL if not found.
 */
FiberOrientation*
CiftiFiberOrientationAdapter::getFiberOrientationNearestCoordinate(const float xyz[3],
                                                                   const float maximumDistance) const
{
    FiberOrientation* nearestFiberOrientation = NULL;
    float nearestDistance = std::numeric_limits<float>::max();
    
    const int64_t numFiberOrientations = getNumberOfFiberOrientations();
    for (int64_t i = 0; i < numFiberOrientations; i++) {
        const float distance = MathFunctions::distanceSquared3D(xyz,
                                                                m_fiberOrientations[i]->m_xyz);
        if (distance < nearestDistance) {
            if (maximumDistance > 0.0) {
                if (distance > maximumDistance) {
                    continue;
                }
            }
            nearestDistance = distance;
            nearestFiberOrientation = m_fiberOrientations[i];
        }
    }
    
    return nearestFiberOrientation;
}


/**
 * Get the orientation fiber group at the given index.
 * @param indx
 *     Index of the desired fiber orientation group.
 */
const FiberOrientation*
CiftiFiberOrientationAdapter::getFiberOrientations(const int64_t indx) const
{
    return m_fiberOrientations[indx];
}

/**
 * @return The display status.
 */
bool
CiftiFiberOrientationAdapter::isDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                          const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_displayStatusInTab[tabIndex];
    }
    return m_displayStatusInDisplayGroup[displayIndex];
}

/**
 * Set the display status.
 * @param displayed
 *   New display status.
 */
void
CiftiFiberOrientationAdapter::setDisplayed(const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const bool displayed)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_displayStatusInTab[tabIndex] = displayed;
    }
    else {
        m_displayStatusInDisplayGroup[displayIndex] = displayed;
    }
}

/**
 * Get the volume spacing.
 * @param volumeSpacingOut
 *    Will contain volume spacing for (I, J, K) axes upon exit.
 */
void
CiftiFiberOrientationAdapter::getVolumeSpacing(float volumeSpacingOut[3]) const
{
    volumeSpacingOut[0] = m_volumeSpacing[0];
    volumeSpacingOut[1] = m_volumeSpacing[1];
    volumeSpacingOut[2] = m_volumeSpacing[2];
}



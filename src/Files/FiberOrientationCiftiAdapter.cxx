
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

#define __FIBER_ORIENTATION_CIFTI_ADAPTER_DECLARE__
#include "FiberOrientationCiftiAdapter.h"
#undef __FIBER_ORIENTATION_CIFTI_ADAPTER_DECLARE__

#include "CaretAssert.h"
#include "CiftiInterface.h"
#include "Fiber.h"
#include "FiberOrientation.h"
#include "ConnectivityLoaderFile.h"

using namespace caret;


    
/**
 * \class caret::FiberOrientationCiftiAdapter 
 * \brief Wraps around a CIFTI file to simplify access to fiber orientations
 */

/**
 * Constructor.
 */
FiberOrientationCiftiAdapter::FiberOrientationCiftiAdapter()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
FiberOrientationCiftiAdapter::~FiberOrientationCiftiAdapter()
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
FiberOrientationCiftiAdapter::initializeWithConnectivityLoaderFile(ConnectivityLoaderFile* clf) throw (DataFileException)
{
    CiftiInterface* interface = clf->ciftiInterface;
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
    const int64_t elementsPerFiber = sizeof(struct Fiber) / 4;
    const int64_t numberOfFibers = ((numCols - 3) /
                                    elementsPerFiber);
    const int64_t expectedNumberOfColumns =
        (numberOfFibers * elementsPerFiber) + 3;
    if (expectedNumberOfColumns != numCols) {
        throw DataFileException("Validation of column count failed: expected "
                                + AString::number(expectedNumberOfColumns)
                                + " but have "
                                + AString::number(numCols)
                                + " columns.");
    }
    
    /*
     * Step between consecutive fibers
     */
    const int64_t stepSizeInFloats = (3
                                      + (numberOfFibers * elementsPerFiber));
    
    /*
     * Create the fiber groups
     */
    m_fiberOrientations.reserve(numRows);
    for (int64_t i = 0; i < numRows; i++) {
        float* fiberGroupPointer = &clf->data[stepSizeInFloats * i];
        FiberOrientation* fiberOrient = new FiberOrientation(numberOfFibers,
                                                                fiberGroupPointer);
        m_fiberOrientations.push_back(fiberOrient);
    }
}

/**
 * @return The number of orientation fiber groups.
 */
int64_t
FiberOrientationCiftiAdapter::getNumberOfFiberOrientationGroups() const
{
    return m_fiberOrientations.size();
}

/**
 * Get the orientation fiber group at the given index.
 * @param indx
 *     Index of the desired fiber orientation group.
 */
FiberOrientation*
FiberOrientationCiftiAdapter::getFiberOrientationGroup(const int64_t indx)
{
    return m_fiberOrientations[indx];
}

/**
 * Get the orientation fiber group at the given index.
 * @param indx
 *     Index of the desired fiber orientation group.
 */
const FiberOrientation*
FiberOrientationCiftiAdapter::getFiberOrientationGroup(const int64_t indx) const
{
    return m_fiberOrientations[indx];
}


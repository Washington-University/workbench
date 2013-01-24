
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
 * SUBSTITUTE GOODS OR  SERVICES LOSS OF USE, DATA, OR PROFITS OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#define __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__
#include "CiftiFiberTrajectoryFile.h"
#undef __CIFTI_FIBER_TRAJECTORY_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretSparseFile.h"
#include "CiftiFiberOrientationFile.h"
#include "CiftiInterface.h"
#include "ConnectivityLoaderFile.h"
#include "FiberOrientationTrajectory.h"
#include "GiftiMetaData.h"

using namespace caret;


    
/**
 * \class caret::CiftiFiberTrajectoryFile 
 * \brief File that contains trajectories
 */

/**
 * Constructor.
 */
CiftiFiberTrajectoryFile::CiftiFiberTrajectoryFile()
: CaretDataFile(DataFileTypeEnum::CONNECTIVITY_FIBER_TRAJECTORY_TEMPORARY)
{
    m_metadata = new GiftiMetaData();
    m_sparseFile = NULL;
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        m_displayGroup[i] = DisplayGroupEnum::getDefaultValue();
        m_displayStatusInTab[i] = true;
    }
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        m_displayStatusInDisplayGroup[i] = true;
    }
}

/**
 * Destructor.
 */
CiftiFiberTrajectoryFile::~CiftiFiberTrajectoryFile()
{
    clearPrivate();
    
    delete m_metadata;
    
}

/**
 * Cleare data in this file.
 */
void
CiftiFiberTrajectoryFile::clear()
{
    CaretDataFile::clear();
    
    clearPrivate();
}


/**
 * Cleare data in this file but not the parent class.
 */
void
CiftiFiberTrajectoryFile::clearPrivate()
{
    m_metadata->clear();
    
    clearLoadedFiberOrientations();
    
    if (m_sparseFile != NULL) {
        delete m_sparseFile;
        m_sparseFile = NULL;
    }
}


/**
 * @return True if the file is empty.
 */
bool
CiftiFiberTrajectoryFile::isEmpty() const
{
    return true;
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum
CiftiFiberTrajectoryFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void
CiftiFiberTrajectoryFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    /* nothing */
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData*
CiftiFiberTrajectoryFile::getFileMetaData()
{
    return m_metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData*
CiftiFiberTrajectoryFile::getFileMetaData() const
{
    return m_metadata;
}

/**
 * @return  Display status of borders.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 */
bool
CiftiFiberTrajectoryFile::isDisplayed(const DisplayGroupEnum::Enum  displayGroup,
                                              const int32_t tabIndex) const
{
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return m_displayStatusInTab[tabIndex];
    }
    return m_displayStatusInDisplayGroup[displayGroup];
}

/**
 * Set the display status for borders for the given display group.
 * @param displayGroup
 *    The display group.
 * @param tabIndex
 *    Index of browser tab.
 * @param displayStatus
 *    New status.
 */
void
CiftiFiberTrajectoryFile::setDisplayed(const DisplayGroupEnum::Enum  displayGroup,
                                               const int32_t tabIndex,
                                               const bool displayStatus)
{
    CaretAssertArrayIndex(m_displayStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          static_cast<int32_t>(displayGroup));
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(m_displayStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        m_displayStatusInTab[tabIndex] = displayStatus;
    }
    else {
        m_displayStatusInDisplayGroup[displayGroup] = displayStatus;
    }
}

/**
 * Get the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 */
DisplayGroupEnum::Enum
CiftiFiberTrajectoryFile::getDisplayGroupForTab(const int32_t browserTabIndex) const
{
    CaretAssertArrayIndex(this->displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    return m_displayGroup[browserTabIndex];
}

/**
 * Set the display group for a given browser tab.
 * @param browserTabIndex
 *    Index of browser tab.
 * @param displayGroup
 *    New value for display group.
 */
void
CiftiFiberTrajectoryFile::setDisplayGroupForTab(const int32_t browserTabIndex,
                                                        const DisplayGroupEnum::Enum  displayGroup)
{
    CaretAssertArrayIndex(this->displayGroup,
                          BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                          browserTabIndex);
    m_displayGroup[browserTabIndex] = displayGroup;
}


/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void
CiftiFiberTrajectoryFile::readFile(const AString& filename) throw (DataFileException)
{
    clear();

    checkFileReadability(filename);
    
    try {
        m_sparseFile = new CaretSparseFile();
        m_sparseFile->readFile(filename);
        setFileName(filename);
        
        clearModified();
    }
    catch (const DataFileException& dfe) {
        clear();
        throw dfe;
    }
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void
CiftiFiberTrajectoryFile::writeFile(const AString& /*filename*/) throw (DataFileException)
{
    throw DataFileException("Writing of Cifti Trajectory Files not supported.");
}

/**
 * Clear the loaded fiber orientations.
 */
void
CiftiFiberTrajectoryFile::clearLoadedFiberOrientations()
{
    const int64_t numFibers = static_cast<int64_t>(m_fiberOrientationTrajectories.size());
    for (int64_t i = 0; i < numFibers; i++) {
        delete m_fiberOrientationTrajectories[i];
    }
    m_fiberOrientationTrajectories.clear();
    
    m_fiberFractions.clear();
}


/**
 * Load date for the given surface node.
 * @param fiberAdapter
 *    Adapter containing fiber orientations.
 * @param structure
 *    Structure in which surface node is located.
 * @param nodeIndex
 *    Index of the surface node.
 */
void
CiftiFiberTrajectoryFile::loadDataForSurfaceNode(CiftiFiberOrientationFile* fiberOrientFile,
                                                 const StructureEnum::Enum structure,
                                                 const int32_t nodeIndex) throw (DataFileException)
{
    clearLoadedFiberOrientations();
    
    if (m_sparseFile == NULL) {
        throw DataFileException("No data has been loaded.");
    }
    const CiftiXML& trajXML = m_sparseFile->getCiftiXML();
    const CiftiXML* orientXML = fiberOrientFile->getCiftiXML();
    if (trajXML.mappingMatches(CiftiXML::ALONG_ROW,
                               *orientXML,
                               CiftiXML::ALONG_COLUMN) == false) {
        QString msg = (getFileNameNoPath()
                       + " rows="
                       + QString::number(trajXML.getNumberOfRows())
                       + " cols="
                       + QString::number(trajXML.getNumberOfColumns())
                       + "   "
                       + fiberOrientFile->getFileNameNoPath()
                       + " rows="
                       + QString::number(orientXML->getNumberOfRows())
                       + " cols="
                       + QString::number(orientXML->getNumberOfColumns()));
        throw DataFileException("Row to Columns do not match: "
                                + msg);
    }
    
    if (trajXML.hasColumnSurfaceData(structure) == false) {
        return;
    }
    const int64_t rowIndex = trajXML.getRowIndexForNode(nodeIndex,
                                                        structure);
    if (rowIndex < 0) {
        return;
    }
    
    std::vector<int64_t> fiberIndices;
    m_sparseFile->getFibersRowSparse(rowIndex,
                                     fiberIndices,
                                     m_fiberFractions);
    CaretAssert(fiberIndices.size() == m_fiberFractions.size());
    
    const int64_t numFibers = static_cast<int64_t>(fiberIndices.size());
    
    if (numFibers > 0) {
        m_fiberOrientationTrajectories.reserve(numFibers);
        
        for (int64_t iFiber = 0; iFiber < numFibers; iFiber++) {
            const int64_t numFiberOrientations = fiberOrientFile->getNumberOfFiberOrientations();
            const int64_t fiberIndex = fiberIndices[iFiber];
            if (fiberIndex < numFiberOrientations) {
                const FiberFractions* fiberFraction = &m_fiberFractions[iFiber];
                const FiberOrientation* fiberOrientation = fiberOrientFile->getFiberOrientations(fiberIndex);
                FiberOrientationTrajectory* fot = new FiberOrientationTrajectory(fiberFraction,
                                                                                 fiberOrientation,
                                                                                 rowIndex);
                m_fiberOrientationTrajectories.push_back(fot);
            }
            else{
                CaretLogSevere("Inavlid index="
                               + QString::number(fiberIndex)
                               + " into fiber orientations");
            }
        }
    }
}

/**
 * @return a REFERENCE to the fiber fractions that were loaded.
 */
const std::vector<FiberOrientationTrajectory*>&
CiftiFiberTrajectoryFile::getLoadedFiberOrientationTrajectories() const
{
    return m_fiberOrientationTrajectories;
}


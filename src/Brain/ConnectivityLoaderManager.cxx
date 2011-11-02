
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __CONNECTIVITY_LOADER_MANAGER_DECLARE__
#include "ConnectivityLoaderManager.h"
#undef __CONNECTIVITY_LOADER_MANAGER_DECLARE__

using namespace caret;

#include "CaretAssert.h"
#include "ConnectivityLoaderFile.h"
    
/**
 * \class ConnectivityLoaderManager 
 * \brief Manages on-demand loading of connectivity data.
 *
 * Manages connectivity files for on-demand loading of data.
 */
/**
 * Constructor.
 */
ConnectivityLoaderManager::ConnectivityLoaderManager()
: CaretObject()
{
    this->reset();    
}

/**
 * Destructor.
 */
ConnectivityLoaderManager::~ConnectivityLoaderManager()
{
    for (LoaderContainerIterator iter = this->connectivityLoaderFiles.begin();
         iter != this->connectivityLoaderFiles.end();
         iter++) {
        delete *iter;
    }
    this->connectivityLoaderFiles.clear();
}

/**
 * @return Number of connectivity loader files.
 */
int32_t 
ConnectivityLoaderManager::getNumberOfConnectivityLoaderFiles() const
{
    return this->connectivityLoaderFiles.size();
}

/**
 * Get the connectivity loader file at the given index.
 * @indx
 *    Index of file.
 * @return
 *    File at given index.
 *    loader has not been setup.
 */
ConnectivityLoaderFile* 
ConnectivityLoaderManager::getConnectivityLoaderFile(const int32_t indx)
{
    CaretAssertVectorIndex(this->connectivityLoaderFiles, indx);
    return this->connectivityLoaderFiles[indx];
}

/**
 * Get the connectivity loader file at the given index.
 * @param indx
 *    Index of file.
 * @return
 *    File at given index.
 *    loader has not been setup.
 */
const ConnectivityLoaderFile* 
ConnectivityLoaderManager::getConnectivityLoaderFile(const int32_t indx) const
{
    CaretAssertVectorIndex(this->connectivityLoaderFiles, indx);
    return this->connectivityLoaderFiles[indx];
}

/**
 * Add a connectivity loader.
 * @return 
 *    Connectivity loader file that was created.
 */
ConnectivityLoaderFile* 
ConnectivityLoaderManager::addConnectivityLoaderFile()
{
    ConnectivityLoaderFile* clf = new ConnectivityLoaderFile();
    this->connectivityLoaderFiles.push_back(clf);
    return clf;
}

/**
 * Remove the connectivity loader at the given index.
 * @parm indx
 *    Index of connectivity loader for removal.
 */
void 
ConnectivityLoaderManager::removeConnectivityLoaderFile(const int32_t indx)
{
    CaretAssertVectorIndex(this->connectivityLoaderFiles, indx);
    
    if (this->getNumberOfConnectivityLoaderFiles() <=
        ConnectivityLoaderManager::MINIMUM_NUMBER_OF_LOADERS) {
        return;
    }

    delete this->connectivityLoaderFiles[indx];
    this->connectivityLoaderFiles.erase(this->connectivityLoaderFiles.begin() + indx);
    
}

/**
 * Remove the given connectivity loader.
 * Do not use the pointer after calling this method!!
 * @param clf
 *    Connectivity loader for removal.
 */
void 
ConnectivityLoaderManager::removeConnectivityLoaderFile(const ConnectivityLoaderFile* clf)
{    
    if (this->getNumberOfConnectivityLoaderFiles() <=
        ConnectivityLoaderManager::MINIMUM_NUMBER_OF_LOADERS) {
        return;
    }
    
    for (LoaderContainerIterator iter = this->connectivityLoaderFiles.begin();
         iter != this->connectivityLoaderFiles.end();
         iter++) {
        if (*iter == clf) {
            delete clf;
            this->connectivityLoaderFiles.erase(iter);
            return;
        }
    }
    CaretAssertMessage(0, "Trying to delete connectivity file that is not in the loader manager" 
                       + clf->getFileName());
}

/**
 * Load data for the given surface node index.
 * @param surfaceFile
 *    Surface File that contains the node (uses its structure).
 * @param nodeIndex
 *    Index of the surface node.
 */
void 
ConnectivityLoaderManager::loadDataForSurfaceNode(const SurfaceFile* surfaceFile,
                            const int32_t nodeIndex) throw (DataFileException)
{
    for (LoaderContainerIterator iter = this->connectivityLoaderFiles.begin();
         iter != this->connectivityLoaderFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        clf->loadDataForSurfaceNode(surfaceFile, nodeIndex);
    }
}

/**
 * Load data for the voxel near the given coordinate.
 * @param xyz
 *     Coordinate of voxel.
 */
void 
ConnectivityLoaderManager::loadDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException)
{
    for (LoaderContainerIterator iter = this->connectivityLoaderFiles.begin();
         iter != this->connectivityLoaderFiles.end();
         iter++) {
        ConnectivityLoaderFile* clf = *iter;
        clf->loadDataForVoxelAtCoordinate(xyz);
    }
}

/**
 * Reset all connectivity loaders.
 */
void 
ConnectivityLoaderManager::reset()
{
    for (LoaderContainerIterator iter = this->connectivityLoaderFiles.begin();
         iter != this->connectivityLoaderFiles.end();
         iter++) {
        delete *iter;
    }
    this->connectivityLoaderFiles.clear();
    
    for (int32_t i = 0; i < ConnectivityLoaderManager::MINIMUM_NUMBER_OF_LOADERS; i++) {
        this->addConnectivityLoaderFile();
    }
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
ConnectivityLoaderManager::toString() const
{
    return "ConnectivityLoaderManager";
}

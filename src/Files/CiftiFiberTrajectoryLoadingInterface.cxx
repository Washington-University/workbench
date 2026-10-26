
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __CIFTI_FIBER_TRAJECTORY_LOADING_INTERFACE_DECLARE__
#include "CiftiFiberTrajectoryLoadingInterface.h"
#undef __CIFTI_FIBER_TRAJECTORY_LOADING_INTERFACE_DECLARE__

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
using namespace caret;


    
/**
 * \class caret::CiftiFiberTrajectoryLoadingInterface2 
 * \brief <REPLACE-WITH-ONE-LINE-DESCRIPTION>
 * \ingroup Files
 *
 * <REPLACE-WITH-THOROUGH DESCRIPTION>
 */

/**
 * Constructor.
 */
CiftiFiberTrajectoryLoadingInterface::CiftiFiberTrajectoryLoadingInterface()
{
    
}

/**
 * Destructor.
 */
CiftiFiberTrajectoryLoadingInterface::~CiftiFiberTrajectoryLoadingInterface()
{
}

/**
 * @return cast to CaretMappableDataFile and return filename without path
 */
AString
CiftiFiberTrajectoryLoadingInterface::getMapFileNameNoPath() const
{
    return getMapFile()->getFileNameNoPath();
}

/**
 * @return cast to CaretMappableDataFile and return filename
 */
AString
CiftiFiberTrajectoryLoadingInterface::getMapFileName()
{
    return getMapFile()->getFileName();
}

/**
 * @return cast to CaretMappableDataFile and return
 */
CaretMappableDataFile*
CiftiFiberTrajectoryLoadingInterface::getMapFile()
{
    CaretMappableDataFile* mapFile(dynamic_cast<CaretMappableDataFile*>(this));
    CaretAssert(mapFile);
    return mapFile;
}

/**
 * @return cast to CaretMappableDataFile and return (const method)
 */
const CaretMappableDataFile*
CiftiFiberTrajectoryLoadingInterface::getMapFile() const
{
    const CaretMappableDataFile* mapFile(dynamic_cast<const CaretMappableDataFile*>(this));
    CaretAssert(mapFile);
    return mapFile;
}


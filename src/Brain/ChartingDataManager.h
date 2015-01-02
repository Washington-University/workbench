#ifndef __CHARTING_DATA_MANAGER_H__
#define __CHARTING_DATA_MANAGER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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


#include "CaretObject.h"
#include "EventListenerInterface.h"

namespace caret {

    class Brain;
    class CiftiMappableDataFile;
    class SurfaceFile;
    
    class ChartingDataManager : public CaretObject, public EventListenerInterface {
        
    public:
        ChartingDataManager(Brain* brain);
        
        virtual ~ChartingDataManager();
        
        void loadAverageChartForSurfaceNodes(const SurfaceFile* surfaceFile,
                                             const std::vector<int32_t>& nodeIndices) const;
        
        void loadChartForSurfaceNode(const SurfaceFile* surfaceFile,
                                     const int32_t nodeIndex) const;
        
        void loadChartForVoxelAtCoordinate(const float xyz[3]) const;
        
        void loadChartForCiftiMappableFileRow(CiftiMappableDataFile* ciftiMapFile,
                                              const int32_t rowIndex) const;
        
        bool hasNetworkFiles() const;
        
        virtual void receiveEvent(Event* event);
        
    private:
        ChartingDataManager(const ChartingDataManager&);

        ChartingDataManager& operator=(const ChartingDataManager&);
        
        Brain* m_brain;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTING_DATA_MANAGER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHARTING_DATA_MANAGER_DECLARE__

} // namespace
#endif  //__CHARTING_DATA_MANAGER_H__

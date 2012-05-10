#ifndef __CONNECTIVITY_LOADER_MANAGER__H_
#define __CONNECTIVITY_LOADER_MANAGER__H_

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


#include "CaretObject.h"
#include "DataFileException.h"
#include "DataFileTypeEnum.h"
#include "EventListenerInterface.h"
#include "TimeLine.h"

namespace caret {

    class Brain;
    class ConnectivityLoaderFile;
    class DataFileException;
    class SurfaceFile;
    
    class ConnectivityLoaderManager : public CaretObject, public EventListenerInterface {
        
    public:
        ConnectivityLoaderManager(Brain* brain);
        
        virtual ~ConnectivityLoaderManager();
        
        bool loadDataForSurfaceNode(const SurfaceFile* surfaceFile,
                                    const int32_t nodeIndex,
                                    AString* rowColumnInformationOut = NULL) throw (DataFileException);
        
        bool loadAverageDataForSurfaceNodes(const SurfaceFile* surfaceFile,
                                            const std::vector<int32_t>& nodeIndices) throw (DataFileException);

        bool loadAverageTimeSeriesForSurfaceNodes(const SurfaceFile* surfaceFile,
                                                const std::vector<int32_t>& nodeIndices, const TimeLine &timeLine) throw (DataFileException);

        
        bool loadDataForVoxelAtCoordinate(const float xyz[3],
                                          AString* rowColumnInformationOut = NULL) throw (DataFileException);

        void reset();
        
        void receiveEvent(Event* event);

        bool loadTimePointAtTime(ConnectivityLoaderFile* clf,
                                 const float seconds) throw (DataFileException);
        
        void getVolumeTimeLines(QList<TimeLine> &tlV);
        void getSurfaceTimeLines(QList<TimeLine> &tlV);

        bool loadTimeLineForSurfaceNode(const SurfaceFile* surfaceFile,
                                        const int32_t nodeIndex, 
                                        const TimeLine &timeLine,
                                        AString* rowColumnInformationOut = NULL) throw (DataFileException);
        
        bool loadTimeLineForVoxelAtCoordinate(const float xyz[3],
                                              AString* rowColumnInformationOut = NULL) throw (DataFileException);
        
    private:
        ConnectivityLoaderManager(const ConnectivityLoaderManager&);

        ConnectivityLoaderManager& operator=(const ConnectivityLoaderManager&);
        
    public:
        virtual AString toString() const;
        
    private:
        void colorConnectivityData();
        
        Brain* brain;
    };
    
#ifdef __CONNECTIVITY_LOADER_MANAGER_DECLARE__
#endif // __CONNECTIVITY_LOADER_MANAGER_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_LOADER_MANAGER__H_

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
        
        ConnectivityLoaderFile* addConnectivityLoaderFile(const AString& path,
                                       const DataFileTypeEnum::Enum connectivityFileType)  throw (DataFileException);
        
        int32_t getNumberOfConnectivityLoaderFiles() const;
        
        ConnectivityLoaderFile* getConnectivityLoaderFile(const int32_t indx);
        
        const ConnectivityLoaderFile* getConnectivityLoaderFile(const int32_t indx) const;
        
        ConnectivityLoaderFile* addConnectivityLoaderFile();
        
        void removeConnectivityLoaderFile(const int32_t indx);
        
        void removeConnectivityLoaderFile(const ConnectivityLoaderFile* clf);
        
        bool loadDataForSurfaceNode(const SurfaceFile* surfaceFile,
                                    const int32_t nodeIndex) throw (DataFileException);
        
        bool loadDataForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException);

        void reset();
        
        void receiveEvent(Event* event);

        bool loadTimePointAtTime(ConnectivityLoaderFile* clf,
                                 const float seconds) throw (DataFileException);
        
        static const int32_t MINIMUM_NUMBER_OF_LOADERS;
        void getVolumeTimeLines(QList<TimeLine> &tlV);
        void getSurfaceTimeLines(QList<TimeLine> &tlV);

        bool loadTimeLineForSurfaceNode(const SurfaceFile* surfaceFile,
                                    const int32_t nodeIndex) throw (DataFileException);
        
        bool loadTimeLineForVoxelAtCoordinate(const float xyz[3]) throw (DataFileException);
        
    private:
        ConnectivityLoaderManager(const ConnectivityLoaderManager&);

        ConnectivityLoaderManager& operator=(const ConnectivityLoaderManager&);
        
    public:
        virtual AString toString() const;
        
    private:
        void colorConnectivityData();
        
        Brain* brain;
        
        typedef std::vector<ConnectivityLoaderFile*> LoaderContainer;
        typedef LoaderContainer::iterator LoaderContainerIterator;
        typedef LoaderContainer::const_iterator LoaderContainerConstIterator;
        
        LoaderContainer connectivityLoaderFiles;
        
    };
    
#ifdef __CONNECTIVITY_LOADER_MANAGER_DECLARE__
    const int32_t ConnectivityLoaderManager::MINIMUM_NUMBER_OF_LOADERS = 2;
#endif // __CONNECTIVITY_LOADER_MANAGER_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_LOADER_MANAGER__H_

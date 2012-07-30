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
#include "SceneableInterface.h"
#include "TimeLine.h"

namespace caret {

    class Brain;
    class ConnectivityLoaderFile;
    class DataFileException;
    class SurfaceFile;
    
    class ConnectivityLoaderManager : public CaretObject, public EventListenerInterface, public SceneableInterface {
        
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

        /*bool loadTimePointAtTime(ConnectivityLoaderFile* clf,
                                 const float seconds) throw (DataFileException);*/

        bool loadFrame(ConnectivityLoaderFile* clf,
            const int frame) throw (DataFileException);
        
        void getVolumeTimeLines(QList<TimeLine> &tlV);
        void getSurfaceTimeLines(QList<TimeLine> &tlV);

        bool loadTimeLineForSurfaceNode(const SurfaceFile* surfaceFile,
                                        const int32_t nodeIndex, 
                                        const TimeLine &timeLine,
                                        AString* rowColumnInformationOut = NULL) throw (DataFileException);
        
        bool loadTimeLineForVoxelAtCoordinate(const float xyz[3],
                                              AString* rowColumnInformationOut = NULL) throw (DataFileException);
        
        bool hasNetworkFiles() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
        
    private:
        ConnectivityLoaderManager(const ConnectivityLoaderManager&);

        ConnectivityLoaderManager& operator=(const ConnectivityLoaderManager&);
        
    public:
        virtual AString toString() const;
        
    private:
        class DenseDataLoaded {
        public:
            DenseDataLoaded();
            
            ~DenseDataLoaded();
            
            void reset();
            
            void setSurfaceLoading(const SurfaceFile* surfaceFile,
                                   const int32_t nodeInde);
            
            void setSurfaceAverageLoading(const SurfaceFile* surfaceFile,
                                          const std::vector<int32_t>& nodeIndices);
            
            void setVolumeLoading(const float xyz[3]);
            
            void restoreFromScene(const SceneAttributes* sceneAttributes,
                                  const SceneClass* sceneClass,
                                  Brain* brain,
                                  ConnectivityLoaderManager* connMan);
            
            SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                    const AString& instanceName);
            
        private:
            enum Mode {
                MODE_NONE,
                MODE_SURFACE_AVERAGE,
                MODE_SURFACE_NODE,
                MODE_VOXEL_XYZ
            };
            
            Mode m_mode;
            
            AString m_surfaceFileName;
            
            std::vector<int32_t> m_surfaceFileNodeIndices;
            
            float m_voxelXYZ[3];
        };
        
        void colorConnectivityData();
        
        Brain* m_brain;
        
        /** 
         * Holds information about last dense connectivity data that
         * was loaded.  This information is then saved/restored 
         * during scene operations.
         */
        DenseDataLoaded m_denseDataLoadedForScene;
        
    };
    
#ifdef __CONNECTIVITY_LOADER_MANAGER_DECLARE__
#endif // __CONNECTIVITY_LOADER_MANAGER_DECLARE__

} // namespace
#endif  //__CONNECTIVITY_LOADER_MANAGER__H_

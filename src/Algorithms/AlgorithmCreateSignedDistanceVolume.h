#ifndef __ALGORITHM_CREATE_SIGNED_DISTANCE_VOLUME_H__
#define __ALGORITHM_CREATE_SIGNED_DISTANCE_VOLUME_H__

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

#include "AbstractAlgorithm.h"
#include "Vector3D.h"
#include "CaretMutex.h"
#include <vector>

namespace caret {

    struct DistVoxOffset
    {
        float m_dist;
        int m_offset[3];
    };
    
    struct VoxelIndex
    {//needed in order to contain the data in a heap, operator= doesn't work on a static array
        VoxelIndex(int64_t ijk[3])
        {
            m_ijk[0] = ijk[0];
            m_ijk[1] = ijk[1];
            m_ijk[2] = ijk[2];
        }
        int64_t m_ijk[3];
    };
    
    class AlgorithmCreateSignedDistanceVolume : public AbstractAlgorithm
    {
        AlgorithmCreateSignedDistanceVolume();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCreateSignedDistanceVolume(ProgressObject* myProgObj, SurfaceFile* mySurf, VolumeFile* myVolOut, float exactLim = 3.0f, float approxLim = 20.0f, int approxNeighborhood = 3);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters*, ProgressObject*);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };
    
    class SignedDistToSurfIndexedBase
    {
        struct triIndexPoint
        {
            Vector3D m_loc;
            std::vector<int> m_triList;
            triIndexPoint(const Vector3D& location)
            {
                m_loc = location;
            }
        };
        std::vector<triIndexPoint> m_indexing;
        SurfaceFile* m_surface;
        float m_avgEdge, m_maxEdge, m_indexLength;
        SignedDistToSurfIndexedBase();
    public:
        SignedDistToSurfIndexedBase(SurfaceFile* mySurf);
        friend class SignedDistToSurfIndexed;
    };
    
    class TopologyHelper;
    
    class SignedDistToSurfIndexed
    {
        CaretPointer<TopologyHelper> m_topoHelp;
        CaretMutex m_mutex;
        CaretPointer<SignedDistToSurfIndexedBase> m_base;
        CaretArray<int> m_triMarked;
        CaretArray<int> m_triMarkChanged;
        SignedDistToSurfIndexed();
        float distToTri(float coord[3], int triangle);
    public:
        SignedDistToSurfIndexed(CaretPointer<SignedDistToSurfIndexedBase> myBase);
        float dist(float coord[3]);
    };

    typedef TemplateAutoOperation<AlgorithmCreateSignedDistanceVolume> AutoAlgorithmCreateSignedDistanceVolume;

}

#endif //__ALGORITHM_CREATE_SIGNED_DISTANCE_VOLUME_H__


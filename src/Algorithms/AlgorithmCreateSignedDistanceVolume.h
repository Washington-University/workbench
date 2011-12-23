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
#include "OctTree.h"
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
        AlgorithmCreateSignedDistanceVolume(ProgressObject* myProgObj, SurfaceFile* mySurf, VolumeFile* myVolOut, float exactLim = 5.0f, float approxLim = 20.0f, int approxNeighborhood = 2);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters*, ProgressObject*);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };
    
    class TopologyHelper;
    
    class SignedDistToSurfIndexedBase
    {
        struct TriVector
        {//specifically so we can cleanly deallocate the vector from non-leaf nodes when they split
            std::vector<int32_t>* m_triList;
            TriVector() { m_triList = new std::vector<int>(); }
            ~TriVector() { if (m_triList != NULL) delete m_triList; }
            void freeData()
            {
                if (m_triList != NULL)
                {
                    delete m_triList;
                    m_triList = NULL;
                }
            }
        };
        static const int NUM_TRIS_TO_TEST = 50;//test for whether to split leaf at this number
        static const int NUM_TRIS_TEST_INCR = 50;//and again at further multiples of this
        Oct<TriVector>* m_indexRoot;
        SurfaceFile* m_surface;
        CaretPointer<TopologyHelper> m_topoHelp;
        SignedDistToSurfIndexedBase();
        void addTriangle(Oct<TriVector>* thisOct, int32_t triangle, float minCoord[3], float maxCoord[3]);
    public:
        SignedDistToSurfIndexedBase(SurfaceFile* mySurf);
        friend class SignedDistToSurfIndexed;
    };
    
    class SignedDistToSurfIndexed
    {
        CaretPointer<TopologyHelper> m_topoHelp;
        CaretMutex m_mutex;
        CaretPointer<SignedDistToSurfIndexedBase> m_base;
        CaretArray<int> m_triMarked;
        CaretArray<int> m_triMarkChanged;
        SignedDistToSurfIndexed();
        struct ClosestPointInfo
        {
            int type;
            int32_t node1, node2, triangle;
            Vector3D tempPoint;
        };
        float unsignedDistToTri(float coord[3], int32_t triangle, ClosestPointInfo& myInfo);
        int computeSign(float coord[3], ClosestPointInfo myInfo);
        bool pointInTri(Vector3D verts[3], Vector3D inPlane, int majAxis, int midAxis);
    public:
        SignedDistToSurfIndexed(CaretPointer<SignedDistToSurfIndexedBase> myBase);
        float dist(float coord[3]);
    };

    typedef TemplateAutoOperation<AlgorithmCreateSignedDistanceVolume> AutoAlgorithmCreateSignedDistanceVolume;

}

#endif //__ALGORITHM_CREATE_SIGNED_DISTANCE_VOLUME_H__

#ifndef __SIGNED_DISTANCE_HELPER_H__
#define __SIGNED_DISTANCE_HELPER_H__

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

#include "Vector3D.h"
#include "CaretMutex.h"
#include "CaretPointer.h"
#include "OctTree.h"
#include <vector>

namespace caret {

    class SurfaceFile;
    class TopologyHelper;
    
    class SignedDistanceHelperBase
    {
        struct TriVector
        {//specifically so we can cleanly deallocate the vector from non-leaf nodes when they split
            std::vector<int32_t>* m_triList;
            TriVector() { m_triList = new std::vector<int32_t>(); }
            ~TriVector() { freeData(); }
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
        CaretPointer<Oct<TriVector> > m_indexRoot;
        int32_t m_numTris, m_numNodes;
        std::vector<float> m_coordList;//make a copy of what we need from SurfaceFile so that if the SurfaceFile gets destroyed, we don't crash
        std::vector<int32_t> m_triangleList;
        CaretPointer<TopologyHelper> m_topoHelp;
        SignedDistanceHelperBase();
        void addTriangle(Oct<TriVector>* thisOct, int32_t triangle, float minCoord[3], float maxCoord[3]);
        const float* getCoordinate(const int32_t nodeIndex) const;//make these public? probably don't want them to be widely used, that is what SurfaceFile is for (but we don't want to store a SurfaceFile pointer)
        const int32_t* getTriangle(const int32_t tileIndex) const;
    public:
        ~SignedDistanceHelperBase();//in order to let us not include TopologyHelper
        SignedDistanceHelperBase(const SurfaceFile* mySurf);
        friend class SignedDistanceHelper;
    };
    
    struct BarycentricInfo
    {
        enum POINT_TYPE
        {
            NODE,
            EDGE,
            TRIANGLE
        };
        int32_t triangle;
        Vector3D point;
        POINT_TYPE type;
        float absDistance;
        int32_t nodes[3];
        float baryWeights[3];
    };
    
    class SignedDistanceHelper
    {
    public:
        enum WindingLogic
        {
            EVEN_ODD,
            NEGATIVE,
            NONZERO,
            NORMALS
        };
    private:
        CaretMutex m_mutex;
        CaretPointer<SignedDistanceHelperBase> m_base;
        CaretArray<int> m_triMarked;
        CaretArray<int> m_triMarkChanged;
        SignedDistanceHelper();
        struct ClosestPointInfo
        {
            int type;
            int32_t node1, node2, triangle;
            Vector3D tempPoint;
        };
        float unsignedDistToTri(const float coord[3], int32_t triangle, ClosestPointInfo& myInfo);
        int computeSign(const float coord[3], ClosestPointInfo myInfo, WindingLogic myWinding);
        bool pointInTri(Vector3D verts[3], Vector3D inPlane, int majAxis, int midAxis);
    public:
        SignedDistanceHelper(CaretPointer<SignedDistanceHelperBase> myBase);
        
        ///return the signed distance value at the point
        float dist(const float coord[3], WindingLogic myWinding);
        
        ///find the closest point ON the surface, and return information about it
        ///will never have negative barycentric weights, or a point outside the triangle
        void barycentricWeights(const float coordIn[3], BarycentricInfo& baryInfoOut);
    };

}

#endif //__SIGNED_DISTANCE_HELPER_H__

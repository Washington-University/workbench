
#ifndef __GEODESIC_HELPER_H__
#define __GEODESIC_HELPER_H__

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
/*LICENSE_END*/

#include <vector>
#include <stdint.h>
#include <cmath>
//for inlining

#include "CaretMutex.h"
#include "CaretPointer.h"
#include "CaretHeap.h"

namespace caret {

    class SurfaceFile;

    //NOTE: this class does NOT stay associated with the coord passed into it, it takes a snapshot of the surface in the constructor
    //This is because it is designed to be fast on repeated calls on a single surface
    //For only a few calls, the constructor may take longer than simply using a well restricted BranModelSurfaceGeodesic
    //This is because it copies neighbors and precomputes all 1 hop and 2 hop shared edge distances in the constructor

    class GeodesicHelperBase
    {//This does the neighbor computation, create a GeodesicHelper to contain the temporary arrays and actually do stuff
        GeodesicHelperBase();//can't construct without arguments
        GeodesicHelperBase& operator=(const GeodesicHelperBase& right);//can't assign
        GeodesicHelperBase(const GeodesicHelperBase& right);//can't use copy constructor
        float** distances, **distances2;//use primitives for speed, and they don't need to change size
        int32_t** nodeNeighbors, **nodeNeighbors2;//copy neighbors at constructor, because I don't want to mess with inheritance, and I want speed of repeated calls
        int32_t* numNeighbors, *numNeighbors2;
        int32_t numNodes;
        static void crossProd(const float in1[3], const float in2[3], float out[3]);//DO NOT PASS AN INPUT AS OUT
        static float dotProd(const float in1[3], const float in2[3]);
        static float normalize(float in[3]);
        static void coordDiff(const float* coord1, const float* coord2, float out[3]);
    public:
        GeodesicHelperBase(const SurfaceFile* surfaceIn);
        ~GeodesicHelperBase() {
            if (numNeighbors) {
                delete[] numNeighbors;
                delete[] numNeighbors2;
                for (int32_t i = 0; i < numNodes; ++i)
                {
                    delete[] nodeNeighbors[i];
                    delete[] nodeNeighbors2[i];
                    delete[] distances[i];
                    delete[] distances2[i];
                }
                delete[] nodeNeighbors;
                delete[] nodeNeighbors2;
                delete[] distances;
                delete[] distances2;
            }
        }
        friend class GeodesicHelper;//let it grab the private variables it needs
    };

    class GeodesicHelper
    {
        CaretMinHeap<int32_t, float> m_active;//save and reuse the allocated space
        float* output, **distances, **distances2;//use primitives for speed, and they don't need to change size
        int32_t** nodeNeighbors, **nodeNeighbors2;//copy neighbors at constructor, because I don't want to mess with inheritance, and I want speed of repeated calls
        int32_t* numNeighbors, *numNeighbors2, *marked, *changed, *parent;
        CaretArray<int64_t> m_heapIdent;
        int32_t numNodes;
        GeodesicHelper();//Don't allow construction without arguments
        GeodesicHelper& operator=(const GeodesicHelper& right);//can't assign
        GeodesicHelper(const GeodesicHelper&);//can't use copy constructor
        void dijkstra(const int32_t root, const float maxdist, std::vector<int32_t>& nodes, std::vector<float>& dists, bool smooth);//geodesic distance restricted
        void dijkstra(const int32_t root, bool smooth);//full surface
        void alltoall(float** out, int32_t** parents, bool smooth);//must be fully allocated
        void dijkstra(const int32_t root, const std::vector<int32_t>& interested, bool smooth);//partial surface
        CaretPointer<GeodesicHelperBase> m_myBase;//mostly just for automatic memory management
        CaretMutex inUse;//could add a function and a locker pointer to be able to lock to thread once, then call repeatedly without locking, if mutex overhead is actually a factor
    public:
        GeodesicHelper(const CaretPointer<GeodesicHelperBase>& baseIn);
        ~GeodesicHelper() {
            if (marked) {
                delete[] output;
                delete[] marked;
                delete[] changed;
                delete[] parent;
            }
        }
        /// Get distances from root node, up to a geodesic distance cutoff (stops computing when no more nodes are within that distance)
        void getNodesToGeoDist(const int32_t node, const float maxdist, std::vector<int32_t>& neighborsOut, std::vector<float>& distsOut, const bool smoothflag = true);

        /// Get distances from root node, up to a geodesic distance cutoff, and also return their parents (root node has -1 as parent)
        void getNodesToGeoDist(const int32_t node, const float maxdist, std::vector<int32_t>& neighborsOut, std::vector<float>& distsOut, std::vector<int32_t>& parentsOut, const bool smoothflag = true);

        /// Get distances from root node to entire surface - allocate the array first
        void getGeoFromNode(const int32_t node, float* valuesOut, const bool smoothflag = true);//MUST be already allocated to number of nodes

        /// Get distances from root node to entire surface, vector method
        void getGeoFromNode(const int32_t node, std::vector<float>& valuesOut, const bool smoothflag = true);

        /// Get distances from root node to entire surface and parents - allocate both arrays first (root node has -1 as parent)
        void getGeoFromNode(const int32_t node, float* valuesOut, int32_t* parentsOut, const bool smoothflag = true);

        /// Get distances from root node to entire surface, and their parents, vector method (root node has -1 as parent)
        void getGeoFromNode(const int32_t node, std::vector<float>& valuesOut, std::vector<int32_t>& parentsOut, const bool smoothflag = true);

        /// Get distances from all nodes to all nodes, passes back NULL if cannot allocate, if successful you must eventually delete the memory
        float** getGeoAllToAll(const bool smooth = true);//i really don't think this needs an overloaded function that outputs parents

        /// Get distances to a restricted set of nodes - output vector is in the SAME ORDER and same size as the input vector ofInterest
        void getGeoToTheseNodes(const int32_t root, const std::vector<int32_t>& ofInterest, std::vector<float>& distsOut, bool smoothflag = true);
    };

    inline void GeodesicHelperBase::crossProd(const float in1[3], const float in2[3], float out[3])
    {//avoid loops for speed - NOT SAFE TO PASS AN INPUT AS OUTPUT
        out[0] = in1[1] * in2[2] - in1[2] * in2[1];
        out[1] = in1[2] * in2[0] - in1[0] * in2[2];
        out[2] = in1[0] * in2[1] - in1[1] * in2[0];
    }

    inline float GeodesicHelperBase::dotProd(const float in1[3], const float in2[3])
    {
        return in1[0] * in2[0] + in1[1] * in2[1] + in1[2] * in2[2];
    }

    inline float GeodesicHelperBase::normalize(float in[3])
    {
        float mag = std::sqrt(in[0] * in[0] + in[1] * in[1] + in[2] * in[2]);
        in[0] /= mag;
        in[1] /= mag;
        in[2] /= mag;
        return mag;
    }

    inline void GeodesicHelperBase::coordDiff(const float* coord1, const float* coord2, float out[3])
    {
        out[0] = coord1[0] - coord2[0];
        out[1] = coord1[1] - coord2[1];
        out[2] = coord1[2] - coord2[2];
    }

} //namespace caret

#endif


#ifndef __GEODESIC_HELPER_H__
#define __GEODESIC_HELPER_H__

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

#include <vector>
#include <stdint.h>

#include "CaretMutex.h"
#include "CaretPointer.h"
#include "CaretHeap.h"
#include "Vector3D.h"

namespace caret {

    class SurfaceFile;

    //NOTE: this class does NOT stay associated with the coord passed into it, it takes a snapshot of the surface in the constructor
    //This is because it is designed to be fast on repeated calls on a single surface

    class GeodesicHelperBase
    {//This does the neighbor computation, create a GeodesicHelper to contain the temporary arrays and actually do stuff
    public:
        struct CrawlInfo
        {
            int32_t edgeNodes[2];
            float edgeWeight, pieceDists[2];
        };
    private:
        GeodesicHelperBase();//can't construct without arguments
        GeodesicHelperBase& operator=(const GeodesicHelperBase& right);//can't assign
        GeodesicHelperBase(const GeodesicHelperBase& right);//can't use copy constructor
        std::vector<std::vector<float> > distances, distances2;
        std::vector<std::vector<int32_t> > nodeNeighbors, nodeNeighbors2;
        std::vector<std::vector<CrawlInfo> > neighbors2PathInfo;
        std::vector<Vector3D> nodeCoords;//for line-following and A*
        int32_t numNodes;
        float m_avgNodeSpacing;//to use for balancing line following penalty
        float m_corrAreaSmallestFactor;//so that heuristics can be consistent despite corrected areas
    public:
        explicit GeodesicHelperBase(const SurfaceFile* surfaceIn, const float* correctedAreas = NULL);//NOTE: this is only an APPROXIMATE correction, use the real surface whenever possible
        friend class GeodesicHelper;//let it grab the private variables it needs
    };

    class GeodesicHelper
    {
        CaretPointer<const GeodesicHelperBase> m_myBase;//mostly just for automatic memory management
        CaretMutex inUse;//could add a function and a locker pointer to be able to lock to thread once, then call repeatedly without locking, if mutex overhead is actually a factor
        CaretMinHeap<int32_t, float> m_active;//save and reuse the allocated space
        const std::vector<float>* distances, *distances2;
        const std::vector<int32_t>* nodeNeighbors, *nodeNeighbors2;
        const std::vector<GeodesicHelperBase::CrawlInfo>* neighbors2PathInfo;
        const Vector3D* nodeCoords;
        float* output;
        int32_t* parent;
        std::vector<float> outputStore;
        std::vector<float> heurVal;
        std::vector<int32_t> marked, changed, parentStore;
        std::vector<int64_t> m_heapIdent;
        int32_t numNodes;
        float m_avgNodeSpacing;
        float m_corrAreaSmallestFactor;
        GeodesicHelper();//Don't allow construction without arguments
        GeodesicHelper& operator=(const GeodesicHelper& right);//can't assign
        GeodesicHelper(const GeodesicHelper&);//can't use copy constructor
        void dijkstra(const int32_t root, const float maxdist, std::vector<int32_t>& nodes, std::vector<float>& dists, bool smooth);//geodesic distance restricted
        void dijkstra(const int32_t root, bool smooth);//full surface
        void dijkstra(const int32_t root, const std::vector<int32_t>& interested, bool smooth);//partial surface
        int32_t dijkstra(const std::vector<int32_t>& startList, const std::vector<int32_t>& endList, const float& maxDist, bool smooth);//one path that connects lists
        int32_t closest(const int32_t& root, const char* roi, const float& maxdist, float& distOut, bool smooth);//just closest node
        int32_t closest(const int32_t& root, const char* roi, bool smooth);//just closest node
        void aStar(const int32_t root, const int32_t endpoint, bool smooth);//faster method for path
        float linePenalty(const Vector3D& pos, const Vector3D& linep1, const Vector3D& linep2, const bool& segment);
        float lineHeuristic(const Vector3D& pos, const Vector3D& linep1, const Vector3D& linep2, const float& remainEucl, const bool& segment);
        void aStarLine(const int32_t& root, const int32_t& endpoint, const Vector3D& linep1, const Vector3D& linep2, const bool& segment);//to single endpoint, following line
        void aStarData(const int32_t& root, const int32_t& endpoint, const float* data, const float& followStrength, const float* roiData, const bool& smooth);//to single endpoint, following data
    public:
        explicit GeodesicHelper(const CaretPointer<const GeodesicHelperBase>& baseIn);
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

        /// Get distances to a restricted set of nodes - output vector is in the SAME ORDER and same size as the input vector ofInterest
        void getGeoToTheseNodes(const int32_t root, const std::vector<int32_t>& ofInterest, std::vector<float>& distsOut, bool smoothflag = true);
        
        ///get the distances and nodes along the path to a node - NOTE: default is not smooth distances, so that all nodes in the path are connected in the surface
        void getPathToNode(const int32_t root, const int32_t endpoint, std::vector<int32_t>& pathNodesOut, std::vector<float>& pathDistsOut, bool smoothflag = false);
        
        ///shortest path between two sets of nodes (for instance, clusters), with distance limit
        void getPathBetweenNodeLists(const std::vector<int32_t>& startList, const std::vector<int32_t>& endList, const float& maxDist, std::vector<int32_t>& pathNodesOut, std::vector<float>& pathDistsOut, bool smoothflag);
        
        ///get the distances and nodes along the path to a node - NOTE: does not do smooth distances, so that all nodes in the path are connected in the surface
        void getPathAlongLine(const int32_t root, const int32_t endpoint, const Vector3D& linep1, const Vector3D& linep2, std::vector<int32_t>& pathNodesOut, std::vector<float>& pathDistsOut);
        
        ///get the distances and nodes along the path to a node - NOTE: does not do smooth distances, so that all nodes in the path are connected in the surface
        void getPathAlongLineSegment(const int32_t root, const int32_t endpoint, const Vector3D& linep1, const Vector3D& linep2, std::vector<int32_t>& pathNodesOut, std::vector<float>& pathDistsOut);
        
        ///path drawing by peaks or troughs of supplied data, controlled by followMaximum
        void getPathFollowingData(const int32_t root, const int32_t endpoint, const float* data, std::vector<int32_t>& pathNodesOut, std::vector<float>& pathDistsOut,
                                  const float& followStrength = 5.0f, const float* roiData = NULL, const bool& followMaximum = true, const bool& smoothFlag = false);
        
        ///get just the closest node in the region and max distance given, returns -1 if no such node found - roi value of 0 means not in region, anything else is in region
        int32_t getClosestNodeInRoi(const int32_t& root, const char* roi, const float& maxdist, float& distOut, bool smoothflag = true);
        int32_t getClosestNodeInRoi(const int32_t& root, const char* roi, std::vector<int32_t>& pathNodesOut, std::vector<float>& pathDistsOut, bool smoothflag);
    };

} //namespace caret

#endif

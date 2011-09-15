
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
      int** nodeNeighbors, **nodeNeighbors2;//copy neighbors at constructor, because I don't want to mess with inheritance, and I want speed of repeated calls
      int* numNeighbors, *numNeighbors2;
      int numNodes;
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
            for (int i = 0; i < numNodes; ++i)
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
      };
      friend class GeodesicHelper;//let it grab the private variables it needs
   };

   class GeodesicHelper
   {
      class myheap
      {//yes, I know its in the stl algorithms, sue me
         struct data
         {
            int node;
            float dist;
         };
         std::vector<data> store;
      public:
         inline bool isEmpty() { return store.empty(); };
         inline int pop()
         {
            int ret = store[0].node;
            int prev = 0, next = 1, next1 = 2, size = store.size();
            int last = size - 1;
            if (next1 < size && store[next].dist > store[next1].dist) next = next1;
            while (next < size && store[last].dist > store[next].dist)
            {
               store[prev] = store[next];
               prev = next;
               next = (prev << 1) + 1;
               next1 = next + 1;
               if (next1 < size && store[next].dist > store[next1].dist) next = next1;
            }
            store[prev] = store[last];
            store.pop_back();
            return ret;
         };
         inline void push(int node, float dist)
         {
            int prev = store.size();
            data mydata;
            mydata.node = node;
            mydata.dist = dist;
            store.push_back(mydata);//placeholder
            int next = (prev - 1) >> 1;
            while (prev > 0 && store[next].dist > dist)
            {
               store[prev] = store[next];
               prev = next;
               next = (prev - 1) >> 1;
            }
            store[prev] = mydata;
         };
         inline void clear() { store.clear(); };
      };
      float* output, **distances, **distances2;//use primitives for speed, and they don't need to change size
      int** nodeNeighbors, **nodeNeighbors2;//copy neighbors at constructor, because I don't want to mess with inheritance, and I want speed of repeated calls
      int* numNeighbors, *numNeighbors2, *marked, *changed, *parent;
      int numNodes;
      GeodesicHelper();//Don't allow construction without arguments
      GeodesicHelper& operator=(const GeodesicHelper& right);//can't assign
      GeodesicHelper(const GeodesicHelperBase&);//can't use copy constructor
      void dijkstra(const int root, const float maxdist, std::vector<int>& nodes, std::vector<float>& dists, bool smooth);//geodesic distance restricted
      void dijkstra(const int root, bool smooth);//full surface
      void alltoall(float** out, int** parents, bool smooth);//must be fully allocated
      void dijkstra(const int root, const std::vector<int>& interested, bool smooth);//partial surface
      CaretMutex inUse;//could add a function and a locker pointer to be able to lock to thread once, then call repeatedly without locking, if mutex overhead is actually a factor
   public:
      GeodesicHelper(GeodesicHelperBase& baseIn);
      ~GeodesicHelper() {
         if (marked) {
            delete[] output;
            delete[] marked;
            delete[] changed;
            delete[] parent;
         }
      };
      /// Get distances from root node, up to a geodesic distance cutoff (stops computing when no more nodes are within that distance)
      void getNodesToGeoDist(const int node, const float maxdist, std::vector<int>& neighborsOut, std::vector<float>& distsOut, const bool smoothflag = true);
      
      /// Get distances from root node, up to a geodesic distance cutoff, and also return their parents (root node has itself as parent)
      void getNodesToGeoDist(const int node, const float maxdist, std::vector<int>& neighborsOut, std::vector<float>& distsOut, std::vector<int>& parentsOut, const bool smoothflag = true);
      
      /// Get distances from root node to entire surface - fastest method for full surface for only one node, but allocate the array first
      void getGeoFromNode(const int node, float* valuesOut, const bool smoothflag = true);//MUST be already allocated to number of nodes

      /// Get distances from root node to entire surface, vector method (not as fast)
      void getGeoFromNode(const int node, std::vector<float>& valuesOut, const bool smoothflag = true);
      
      /// Get distances from root node to entire surface and parents - fastest method with parents for full surface, single node, allocate both arrays first (root node has self as parent)
      void getGeoFromNode(const int node, float* valuesOut, int* parentsOut, const bool smoothflag = true);
      
      /// Get distances from root node to entire surface, and their parents, vector method (not as fast, root node has self as parent)
      void getGeoFromNode(const int node, std::vector<float>& valuesOut, std::vector<int>& parentsOut, const bool smoothflag = true);
      
      /// Get distances from all nodes to all nodes, passes back NULL if cannot allocate, if successful you must eventually delete the memory
      float** getGeoAllToAll(const bool smooth = true);//i really don't think this needs an overloaded function that outputs parents
      
      /// Get distances to a restricted set of nodes - output vector is in the SAME ORDER and same size as the input vector ofInterest
      void getGeoToTheseNodes(const int root, const std::vector<int>& ofInterest, std::vector<float>& distsOut, bool smoothflag = true);
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

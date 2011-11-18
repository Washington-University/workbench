#ifndef __ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__
#define __ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__

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

/*
file->save as... and enter what you will name the class, plus .h

find and replace these strings, without matching "whole word only" (plain text mode):

AlgorithmVolumeToSurfaceMapping     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_VOLUME_TO_SURFACE_MAPPING    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-volume-to-surface-mapping   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
MAP VOLUME TO SURFACE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

if the algorithm takes no parameters (???) uncomment the line below for takesParameters(), otherwise delete it

next, make AlgorithmVolumeToSurfaceMapping.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeToSurfaceMapping/g' | sed 's/-[c]ommand-switch/-volume-to-surface-mapping/g' | sed 's/[S]HORT DESCRIPTION/MAP VOLUME TO SURFACE/g' > AlgorithmVolumeToSurfaceMapping.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeToSurfaceMapping/g' | sed 's/-[c]ommand-switch/-volume-to-surface-mapping/g' | sed 's/[S]HORT DESCRIPTION/MAP VOLUME TO SURFACE/g' > Algorithms/AlgorithmVolumeToSurfaceMapping.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeToSurfaceMapping/g' | sed 's/-[c]ommand-switch/-volume-to-surface-mapping/g' | sed 's/[S]HORT DESCRIPTION/MAP VOLUME TO SURFACE/g' > src/Algorithms/AlgorithmVolumeToSurfaceMapping.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmVolumeToSurfaceMapping.h
AlgorithmVolumeToSurfaceMapping.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmVolumeToSurfaceMapping.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeToSurfaceMapping()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"
#include <vector>
#include <algorithm>

namespace caret {
    
    struct VoxelDist
    {
        float dist;
        int64_t ijk[3];
        VoxelDist(const float& distIn, const int64_t* ijkIn)
        {
            dist = distIn;
            ijk[0] = ijkIn[0];
            ijk[1] = ijkIn[1];
            ijk[2] = ijkIn[2];
        }
        VoxelDist(const float& distIn, const int64_t& i, const int64_t& j, const int64_t& k)
        {
            dist = distIn;
            ijk[0] = i;
            ijk[1] = j;
            ijk[2] = k;
        }
    };
    
    struct VoxelWeight
    {//for precomputation in ribbon mapping
        float weight;
        int64_t ijk[3];
    };
    
    class VoxelDistMinHeap
    {//because STL is kinda painful for minheaps, especially without an interface
        struct MyMinHeapCompare
        {
            bool operator()(const VoxelDist& left, const VoxelDist& right)
            {
                return (left.dist > right.dist);
            }
        };
        std::vector<VoxelDist> m_heap;//because 4 assignments beat the new's it would take to build the heap on naive pointers, and I'm too lazy to put advanced memory handling in here
        MyMinHeapCompare m_comp;
    public:
        void clear()
        {
            m_heap.clear();
        }
        void push(const VoxelDist& toPush)
        {
            m_heap.push_back(toPush);
            std::push_heap(m_heap.begin(), m_heap.end(), m_comp);
        }
        bool isEmpty()
        {
            return (m_heap.size() == 0);
        }
        VoxelDist pop()
        {
            VoxelDist ret = m_heap[0];
            std::pop_heap(m_heap.begin(), m_heap.end(), m_comp);
            m_heap.pop_back();
            return ret;
        }
    };
    
    class AlgorithmVolumeToSurfaceMapping : public AbstractAlgorithm
    {
        AlgorithmVolumeToSurfaceMapping();
        void precomputeWeights(std::vector<std::vector<VoxelWeight> >& myWeights, VolumeFile* ribbonVol, MetricFile* thickness, SurfaceFile* mySurface, float ribbonValue, float kernel);
        void dijkstra(VolumeFile* mask, VoxelDistMinHeap& myHeap, float voxNeighDist[3][3][3], int* marked, int* changed, float* distances, std::vector<VoxelDist>& myVoxelsOut, float maxDist);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        enum Method
        {
            TRILINEAR,
            NEAREST_NEIGHBOR,
            RIBBON_CONSTRAINED
        };
        AlgorithmVolumeToSurfaceMapping(ProgressObject* myProgObj, VolumeFile* myVolume, SurfaceFile* mySurface, MetricFile* myMetricOut, Method myMethod, int64_t mySubVol = -1, MetricFile* thickness = NULL, VolumeFile* ribbonVol = NULL, float ribbonValue = 1.0f, float kernel = 1.0f, bool averageNormals = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters*, ProgressObject*);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeToSurfaceMapping> AutoAlgorithmVolumeToSurfaceMapping;

}

#endif //__ALGORITHM_VOLUME_TO_SURFACE_MAPPING_H__

#ifndef __ALGORITHM_VOLUME_SMOOTHING_H__
#define __ALGORITHM_VOLUME_SMOOTHING_H__

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

AlgorithmVolumeSmoothing     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_VOLUME_SMOOTHING    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-volume-smoothing   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
SMOOTH A VOLUME FILE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

if the algorithm takes no parameters (???) uncomment the line below for takesParameters(), otherwise delete it

next, make AlgorithmVolumeSmoothing.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeSmoothing/g' | sed 's/-[c]ommand-switch/-volume-smoothing/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH A VOLUME FILE/g' > AlgorithmVolumeSmoothing.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeSmoothing/g' | sed 's/-[c]ommand-switch/-volume-smoothing/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH A VOLUME FILE/g' > Algorithms/AlgorithmVolumeSmoothing.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeSmoothing/g' | sed 's/-[c]ommand-switch/-volume-smoothing/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH A VOLUME FILE/g' > src/Algorithms/AlgorithmVolumeSmoothing.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmVolumeSmoothing.h
AlgorithmVolumeSmoothing.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmVolumeSmoothing.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeSmoothing()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmVolumeSmoothing : public AbstractAlgorithm
    {
        AlgorithmVolumeSmoothing();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
        void smoothFrame(const float* inFrame, std::vector<int64_t> myDims, CaretArray<float> scratchFrame, CaretArray<float> scratchFrame2, const VolumeFile* inVol, const VolumeFile* roiVol, CaretArray<float> iweights, CaretArray<float> jweights, CaretArray<float> kweights, int irange, int jrange, int krange);
        void smoothFrameNonOrth(const float* inFrame, const std::vector<int64_t>& myDims, CaretArray<float>& scratchFrame, const VolumeFile* inVol, const VolumeFile* roiVol, const CaretArray<float**>& weights, const int& irange, const int& jrange, const int& krange);
    public:
        AlgorithmVolumeSmoothing(ProgressObject* myProgObj, const VolumeFile* inVol, const float& kernel, VolumeFile* outVol, const VolumeFile* roiVol = NULL, const int& subvol = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeSmoothing> AutoAlgorithmVolumeSmoothing;

}

#endif //__ALGORITHM_VOLUME_SMOOTHING_H__

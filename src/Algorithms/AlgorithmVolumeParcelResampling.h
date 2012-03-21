#ifndef __ALGORITHM_VOLUME_PARCEL_RESAMPLING_H__
#define __ALGORITHM_VOLUME_PARCEL_RESAMPLING_H__

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

AlgorithmVolumeParcelResampling     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_VOLUME_PARCEL_RESAMPLING    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-volume-parcel-resampling   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
SMOOTH AND RESAMPLE VOLUME PARCELS : short description of the command, uppercase, three to five words, often just command switch with more verbosity

if the algorithm takes no parameters (???) uncomment the line below for takesParameters(), otherwise delete it

next, make AlgorithmVolumeParcelResampling.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeParcelResampling/g' | sed 's/-[c]ommand-switch/-volume-parcel-resampling/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH AND RESAMPLE VOLUME PARCELS/g' > AlgorithmVolumeParcelResampling.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeParcelResampling/g' | sed 's/-[c]ommand-switch/-volume-parcel-resampling/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH AND RESAMPLE VOLUME PARCELS/g' > Algorithms/AlgorithmVolumeParcelResampling.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeParcelResampling/g' | sed 's/-[c]ommand-switch/-volume-parcel-resampling/g' | sed 's/[S]HORT DESCRIPTION/SMOOTH AND RESAMPLE VOLUME PARCELS/g' > src/Algorithms/AlgorithmVolumeParcelResampling.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmVolumeParcelResampling.h
AlgorithmVolumeParcelResampling.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmVolumeParcelResampling.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeParcelResampling()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmVolumeParcelResampling : public AbstractAlgorithm
    {
        AlgorithmVolumeParcelResampling();
        void matchLabels(const caret::VolumeFile* curLabel, const caret::VolumeFile* newLabel, std::vector<std::pair<int, int> > matchedLabels);
        void generateVoxelLists(std::vector<std::pair<int, int> > matchedLabels, const VolumeFile* curLabel, const VolumeFile* newLabel, std::vector<std::vector<int64_t> > voxelLists);
        void resampleFixZeros(LevelProgress& myProgress, std::vector<std::pair<int, int> > matchedLabels, std::vector<std::vector<int64_t> > voxelLists, const VolumeFile* inVol, const VolumeFile* curLabel, const caret::VolumeFile* newLabel, const float& kernel, VolumeFile* outVol, const int& subvolNum);
        void resample(LevelProgress& myProgress, std::vector<std::pair<int, int> > matchedLabels, std::vector<std::vector<int64_t> > voxelLists, const VolumeFile* inVol, const VolumeFile* curLabel, const caret::VolumeFile* newLabel, const float& kernel, VolumeFile* outVol, const int& subvolNum);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeParcelResampling(ProgressObject* myProgObj, const VolumeFile* inVol, const VolumeFile* curLabel, const VolumeFile* newLabel, const float& kernel, VolumeFile* outVol, const bool& fixZeros = false, const int& subvolNum = -1);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeParcelResampling> AutoAlgorithmVolumeParcelResampling;

}

#endif //__ALGORITHM_VOLUME_PARCEL_RESAMPLING_H__

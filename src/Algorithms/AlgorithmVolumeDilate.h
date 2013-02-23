#ifndef __ALGORITHM_VOLUME_DILATE_H__
#define __ALGORITHM_VOLUME_DILATE_H__

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

AlgorithmVolumeDilate     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_VOLUME_DILATE    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-volume-dilate   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
DILATE A VOLUME FILE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmVolumeDilate.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeDilate/g' | sed 's/-[c]ommand-switch/-volume-dilate/g' | sed 's/[S]HORT DESCRIPTION/DILATE A VOLUME FILE/g' > AlgorithmVolumeDilate.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeDilate/g' | sed 's/-[c]ommand-switch/-volume-dilate/g' | sed 's/[S]HORT DESCRIPTION/DILATE A VOLUME FILE/g' > Algorithms/AlgorithmVolumeDilate.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeDilate/g' | sed 's/-[c]ommand-switch/-volume-dilate/g' | sed 's/[S]HORT DESCRIPTION/DILATE A VOLUME FILE/g' > src/Algorithms/AlgorithmVolumeDilate.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmVolumeDilate.h
AlgorithmVolumeDilate.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmVolumeDilate.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeDilate()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmVolumeDilate : public AbstractAlgorithm
    {
        AlgorithmVolumeDilate();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        enum Method
        {
            NEAREST,
            WEIGHTED
        };
        AlgorithmVolumeDilate(ProgressObject* myProgObj, const VolumeFile* volIn, const float& distance, const Method& myMethod,
                              VolumeFile* volOut, const VolumeFile* badRoi, const int& subvol);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    private:
        void dilateFrame(const VolumeFile* volIn, const int& insubvol, const int& component, VolumeFile* volOut, const int& outsubvol, const VolumeFile* badRoi,
                         const Method& myMethod, const std::vector<int>& stencil, const std::vector<float>& stenWeights);
    };

    typedef TemplateAutoOperation<AlgorithmVolumeDilate> AutoAlgorithmVolumeDilate;

}

#endif //__ALGORITHM_VOLUME_DILATE_H__

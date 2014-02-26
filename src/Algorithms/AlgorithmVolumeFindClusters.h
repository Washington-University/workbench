#ifndef __ALGORITHM_VOLUME_FIND_CLUSTERS_H__
#define __ALGORITHM_VOLUME_FIND_CLUSTERS_H__

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

AlgorithmVolumeFindClusters     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_VOLUME_FIND_CLUSTERS    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-volume-find-clusters   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
FILTER CLUSTERS BY VOLUME : short description of the command, uppercase, three to five words, often just command switch with more verbosity

next, make AlgorithmVolumeFindClusters.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeFindClusters/g' | sed 's/-[c]ommand-switch/-volume-find-clusters/g' | sed 's/[S]HORT DESCRIPTION/FILTER CLUSTERS BY VOLUME/g' > AlgorithmVolumeFindClusters.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeFindClusters/g' | sed 's/-[c]ommand-switch/-volume-find-clusters/g' | sed 's/[S]HORT DESCRIPTION/FILTER CLUSTERS BY VOLUME/g' > Algorithms/AlgorithmVolumeFindClusters.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmVolumeFindClusters/g' | sed 's/-[c]ommand-switch/-volume-find-clusters/g' | sed 's|[S]HORT DESCRIPTION|FILTER CLUSTERS BY VOLUME|g' > src/Algorithms/AlgorithmVolumeFindClusters.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmVolumeFindClusters.h
AlgorithmVolumeFindClusters.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmVolumeFindClusters.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmVolumeFindClusters()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include "AbstractAlgorithm.h"

namespace caret {
    
    class AlgorithmVolumeFindClusters : public AbstractAlgorithm
    {
        AlgorithmVolumeFindClusters();
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmVolumeFindClusters(ProgressObject* myProgObj /*INSERT PARAMETERS HERE//*/);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmVolumeFindClusters> AutoAlgorithmVolumeFindClusters;

}

#endif //__ALGORITHM_VOLUME_FIND_CLUSTERS_H__

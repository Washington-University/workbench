#ifndef __ALGORITHM_CIFTI_CORRELATION_H__
#define __ALGORITHM_CIFTI_CORRELATION_H__

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

AlgorithmCiftiCorrelation     : algorithm name, in CamelCase, with initial capital, same as what you saved the header file to
ALGORITHM_CIFTI_CORRELATION    : uppercase of algorithm name, with underscore between words, used in #ifdef guards
-cifti-correlation   : switch for the command line to use, often hyphenated version of algorithm name, lowercase, minus "algorithm"
GENERATE CORRELATION OF ROWS IN A CIFTI FILE : short description of the command, uppercase, three to five words, often just command switch with more verbosity

if the algorithm takes no parameters (???) uncomment the line below for takesParameters(), otherwise delete it

next, make AlgorithmCiftiCorrelation.cxx from AlgorithmTemplate.cxx.txt via one of the following (depending on working directory):

cat AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiCorrelation/g' | sed 's/-[c]ommand-switch/-cifti-correlation/g' | sed 's/[S]HORT DESCRIPTION/GENERATE CORRELATION OF ROWS IN A CIFTI FILE/g' > AlgorithmCiftiCorrelation.cxx
cat Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiCorrelation/g' | sed 's/-[c]ommand-switch/-cifti-correlation/g' | sed 's/[S]HORT DESCRIPTION/GENERATE CORRELATION OF ROWS IN A CIFTI FILE/g' > Algorithms/AlgorithmCiftiCorrelation.cxx
cat src/Algorithms/AlgorithmTemplate.cxx.txt | sed 's/[A]lgorithmName/AlgorithmCiftiCorrelation/g' | sed 's/-[c]ommand-switch/-cifti-correlation/g' | sed 's/[S]HORT DESCRIPTION/GENERATE CORRELATION OF ROWS IN A CIFTI FILE/g' > src/Algorithms/AlgorithmCiftiCorrelation.cxx

or manually copy and replace

next, implement its functions - the algorithm work goes in the CONSTRUCTOR

add these into Algorithms/CMakeLists.txt:

AlgorithmCiftiCorrelation.h
AlgorithmCiftiCorrelation.cxx

place the following lines into Commands/CommandOperationManager.cxx:

#include "AlgorithmCiftiCorrelation.h"
    //near the top

    this->commandOperations.push_back(new CommandParser(new AutoAlgorithmCiftiCorrelation()));
        //in CommandOperationManager()

finally, remove this block comment
*/

#include <vector>
#include "AbstractAlgorithm.h"
#include "CaretPointer.h"

namespace caret {
    
    class CiftiFile;
    
    class AlgorithmCiftiCorrelation : public AbstractAlgorithm
    {
        AlgorithmCiftiCorrelation();
        struct CacheRow
        {
            int m_ciftiIndex;
            CaretArray<float> m_row;
        };
        struct RowInfo
        {
            bool m_haveCalculated;
            float m_mean, m_stddev;
            int m_cacheIndex;
            RowInfo()
            {
                m_haveCalculated = false;
                m_cacheIndex = -1;
            }
        };
        std::vector<CacheRow> m_rowCache;
        std::vector<RowInfo> m_rowInfo;
        std::vector<CaretArray<float> > m_tempRows;//reuse return values in getRow instead of reallocating
        int m_tempRowPos;
        int m_cacheUsed;//reuse cache entries instead of reallocating them
        int m_numCols;
        const CiftiFile* m_inputCifti;//so that accesses work through the cache functions
        void cacheRow(const int& ciftiIndex);
        void clearCache();
        CaretArray<float> getRow(const int& ciftiIndex, float& mean, float& stdev);
        CaretArray<float> getTempRow();
        float correlate(const float* row1, const float& mean1, const float& dev1, const float* row2, const float& mean2, const float& dev2, const bool& fisherZ);
        void init(const CiftiFile* input);
        int numRowsForMem(const float& memLimitGB);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut,
                                                     const bool& fisherZ = false, const float& memLimitGB = -1.0f);
        AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut,
                                                     const MetricFile* leftRoi, const MetricFile* rightRoi = NULL, const MetricFile* cerebRoi = NULL,
                                                     const VolumeFile* volRoi = NULL, const bool& fisherZ = false, const float& memLimitGB = -1.0f);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiCorrelation> AutoAlgorithmCiftiCorrelation;

}

#endif //__ALGORITHM_CIFTI_CORRELATION_H__

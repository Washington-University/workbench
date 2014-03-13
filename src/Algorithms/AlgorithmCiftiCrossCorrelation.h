#ifndef __ALGORITHM_CIFTI_CROSS_CORRELATION_H__
#define __ALGORITHM_CIFTI_CROSS_CORRELATION_H__

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

#include "AbstractAlgorithm.h"

#include "CaretPointer.h"

#include <vector>

namespace caret {
    
    class AlgorithmCiftiCrossCorrelation : public AbstractAlgorithm
    {
        struct CacheRow
        {
            int m_ciftiIndex;
            std::vector<float> m_row;
        };
        struct RowInfo
        {
            bool m_haveCalculated;
            float m_mean, m_rootResidSqr;
            int m_cacheIndex;
            RowInfo()
            {
                m_haveCalculated = false;
                m_cacheIndex = -1;
            }
        };
        int64_t m_numCols, m_numRowsA, m_numRowsB;
        const CiftiFile* m_ciftiA, *m_ciftiB, *m_ciftiOut;//output is really only to check if it is in-memory for numRowsForMem
        std::vector<CacheRow> m_rowCacheA;//we only cache from cifti A
        std::vector<RowInfo> m_rowInfoA, m_rowInfoB;
        std::vector<CaretArray<float> > m_tempRowsB;//reuse return values in getRowB instead of reallocating
        std::vector<float> m_weights;
        std::vector<int> m_weightIndexes;
        bool m_binaryWeights, m_weightedMode;
        double m_weightSum;
        AlgorithmCiftiCrossCorrelation();
        void init(const CiftiFile* myCiftiA, const CiftiFile* myCiftiB, const CiftiFile* myCiftiOut, const std::vector<float>* weights);
        int64_t numRowsForMem(const float& memLimitGB);//call after init()
        float* getTempRowB();//only used for getRowB, A rows are pulled from cache
        const float* getCachedRowA(const int64_t& ciftiIndex, float& rootResidSqr);//retrieve already cached rows
        const float* getRowB(const int64_t& ciftiIndex, float& rootResidSqr);
        void adjustRow(float* row, RowInfo& info);
        float correlate(const float* row1, const float& rrs1, const float* row2, const float& rrs2, const bool& fisherZ);
        void cacheRowsA(const int64_t& begin, const int64_t& end);//grabs the rows and does whatever it needs to, using as much IO bandwidth and CPU resources as available/needed
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiCrossCorrelation(ProgressObject* myProgObj, const CiftiFile* myCiftiA, const CiftiFile* myCiftiB, CiftiFile* myCiftiOut,
                                       const std::vector<float>* weights, const bool& fisherZ, const float& memLimitGB);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiCrossCorrelation> AutoAlgorithmCiftiCrossCorrelation;

}

#endif //__ALGORITHM_CIFTI_CROSS_CORRELATION_H__

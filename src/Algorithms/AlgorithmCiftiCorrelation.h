#ifndef __ALGORITHM_CIFTI_CORRELATION_H__
#define __ALGORITHM_CIFTI_CORRELATION_H__

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
#include "AbstractAlgorithm.h"
#include "CaretPointer.h"

namespace caret {
    
    class AlgorithmCiftiCorrelation : public AbstractAlgorithm
    {
        AlgorithmCiftiCorrelation();
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
        std::vector<CacheRow> m_rowCache;
        std::vector<RowInfo> m_rowInfo;
        std::vector<CaretArray<float> > m_tempRows;//reuse return values in getRow instead of reallocating
        std::vector<float> m_weights;
        std::vector<int> m_weightIndexes;
        bool m_binaryWeights, m_weightedMode, m_noDemean, m_covariance;
        int m_cacheUsed;//reuse cache entries instead of reallocating them
        int m_numCols;
        const CiftiFile* m_inputCifti;//so that accesses work through the cache functions
        void cacheRow(const int& ciftiIndex);
        void computeRowStats(const float* row, float& mean, float& rootResidSqr);
        void doSubtract(float* row, const float& mean);
        void clearCache();
        const float* getRow(const int& ciftiIndex, float& rootResidSqr, const bool& mustBeCached = false);
        float* getTempRow();
        float correlate(const float* row1, const float& rrs1, const float* row2, const float& rrs2, const bool& fisherZ);
        void init(const CiftiFile* input, const std::vector<float>* weights, const bool& noDemean, const bool& covariance);
        int numRowsForMem(const float& memLimitGB, bool& cacheFullInput);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut, const std::vector<float>* weights = NULL,
                                  const bool& fisherZ = false, const float& memLimitGB = -1.0f, const bool& noDemean = false, const bool& covariance = false);
        AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut,
                                  const MetricFile* leftRoi, const MetricFile* rightRoi = NULL, const MetricFile* cerebRoi = NULL,
                                  const VolumeFile* volRoi = NULL, const std::vector<float>* weights = NULL, const bool& fisherZ = false,
                                  const float& memLimitGB = -1.0f, const bool& noDemean = false, const bool& covariance = false);
        AlgorithmCiftiCorrelation(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut, const CiftiFile* ciftiRoi,
                                  const std::vector<float>* weights = NULL, const bool& fisherZ = false, const float& memLimitGB = -1.0f,
                                  const bool& noDemean = false, const bool& covariance = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiCorrelation> AutoAlgorithmCiftiCorrelation;

}

#endif //__ALGORITHM_CIFTI_CORRELATION_H__

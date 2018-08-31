#ifndef __ALGORITHM_CIFTI_CORRELATION_GRADIENT_H__
#define __ALGORITHM_CIFTI_CORRELATION_GRADIENT_H__

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
#include "StructureEnum.h"

namespace caret {
    
    class AlgorithmCiftiCorrelationGradient : public AbstractAlgorithm
    {
        AlgorithmCiftiCorrelationGradient();
    public:
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
    private:
        struct CacheRow
        {
            int m_ciftiIndex;
            std::vector<float> m_row;
            CacheRow(int64_t rowLength)
            {
                m_ciftiIndex = -1;
                m_row.resize(rowLength);
            }
        };
        std::vector<CacheRow> m_rowCache;
        std::vector<RowInfo> m_rowInfo, m_firstCorrInfo;
        std::vector<float> m_outColumn;
        int64_t m_numCols;
        bool m_undoFisherInput, m_applyFisher, m_covariance;
        const CiftiFile* m_inputCifti;//so that accesses work through the cache functions
        int64_t m_rowLengthFirst;//for -double-correlation
        bool m_doubleCorr, m_firstCovar, m_firstNoDemean, m_firstFisher;
        float m_memLimitGB;
        void cacheRows(const std::vector<int64_t>& ciftiIndices);//grabs the rows and does whatever it needs to, using as much IO bandwidth and CPU resources as available/needed
        void clearCache();
        const float* getRow(const int& ciftiIndex, float& rootResidSqr, float* scratchStorage);
        void init(const CiftiFile* input, const float& memLimitGB, const bool& undoFisherInput, const bool& applyFisher, const bool& covariance,
                  const bool doubleCorr, const bool firstFisher, const bool firstNoDemean, const bool firstCovar);
        int numRowsForMem(const int64_t& inrowBytes, const int64_t& outrowBytes, const int& numRows, bool& cacheFullInput);
        //void processSurfaceComponentLocal(StructureEnum::Enum& myStructure, const float& surfKern, const float& memLimitGB, SurfaceFile* mySurf);
        void processSurfaceComponent(StructureEnum::Enum& myStructure, const float& surfKern, const float& memLimitGB, SurfaceFile* mySurf, const MetricFile* myAreas);
        void processSurfaceComponent(StructureEnum::Enum& myStructure, const float& surfKern, const float& surfExclude, const float& memLimitGB, SurfaceFile* mySurf, const MetricFile* myAreas);
        //void processVolumeComponentLocal(StructureEnum::Enum& myStructure, const float& volKern, const float& memLimitGB);
        void processVolumeComponent(StructureEnum::Enum& myStructure, const float& volKern, const float& memLimitGB);
        void processVolumeComponent(StructureEnum::Enum& myStructure, const float& volKern, const float& volExclude, const float& memLimitGB);
    protected:
        static float getSubAlgorithmWeight();
        static float getAlgorithmInternalWeight();
    public:
        AlgorithmCiftiCorrelationGradient(ProgressObject* myProgObj, const CiftiFile* myCifti, CiftiFile* myCiftiOut,
                                          SurfaceFile* myLeftSurf = NULL, SurfaceFile* myRightSurf = NULL, SurfaceFile* myCerebSurf = NULL,
                                          const MetricFile* myLeftAreas = NULL, const MetricFile* myRightAreas = NULL, const MetricFile* myCerebAreas = NULL,
                                          const float& surfKern = -1.0f, const float& volKern = -1.0f, const bool& undoFisherInput = false, const bool& applyFisher = false,
                                          const float& surfaceExclude = -1.0f, const float& volumeExclude = -1.0f,
                                          const bool& covariance = false,
                                          const float& memLimitGB = -1.0f,
                                          const bool doubleCorr = false, const bool firstFisher = false, const bool firstNoDemean = false, const bool firstCovar = false);
        static OperationParameters* getParameters();
        static void useParameters(OperationParameters* myParams, ProgressObject* myProgObj);
        static AString getCommandSwitch();
        static AString getShortDescription();
    };

    typedef TemplateAutoOperation<AlgorithmCiftiCorrelationGradient> AutoAlgorithmCiftiCorrelationGradient;

}

#endif //__ALGORITHM_CIFTI_CORRELATION_GRADIENT_H__

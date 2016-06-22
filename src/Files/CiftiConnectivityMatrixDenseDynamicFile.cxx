
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

#include <cmath>

#define __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_DECLARE__
#include "CiftiConnectivityMatrixDenseDynamicFile.h"
#undef __CIFTI_CONNECTIVITY_MATRIX_DENSE_DYNAMIC_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CiftiBrainordinateDataSeriesFile.h"
#include "CiftiFile.h"
#include "ElapsedTimer.h"

using namespace caret;


    
/**
 * \class caret::CiftiConnectivityMatrixDenseDynamicFile 
 * \brief Connectivity Dynamic Dense x Dense File version of data-series
 * \ingroup Files
 *
 * Contains dynamic connectivity from brainordinates to brainordinates.
 * Internally, the file format is the same as a data series file.  When
 * a row is requested, the row is correlated with all other rows
 * producing the connectivity from that row to all other rows.
 */

/**
 * Constructor.
 */
CiftiConnectivityMatrixDenseDynamicFile::CiftiConnectivityMatrixDenseDynamicFile()
: CiftiMappableConnectivityMatrixDataFile(DataFileTypeEnum::CONNECTIVITY_DENSE_DYNAMIC),
m_numberOfBrainordinates(-1),
m_numberOfTimePoints(-1),
m_meanSumSquaredValidFlag(false)
{
    
}

/**
 * Destructor.
 */
CiftiConnectivityMatrixDenseDynamicFile::~CiftiConnectivityMatrixDenseDynamicFile()
{
    
}

/**
 * This method is intended for overriding by subclasess so that they
 * can examine and verify the data that was read.  This method is
 * called after successfully reading a file.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::validateAfterFileReading()
{
    /*
     * Need dimensions of data
     */
    const CiftiXML& ciftiXML = getCiftiFile()->getCiftiXML();
    m_numberOfBrainordinates = ciftiXML.getBrainModelsMap(CiftiXML::ALONG_COLUMN).getLength();
    m_numberOfTimePoints     = ciftiXML.getSeriesMap(CiftiXML::ALONG_ROW).getLength();
    
    m_rowData.clear();
    m_meanSumSquaredValidFlag = false;
    
    if ((m_numberOfBrainordinates > 0)
        && (m_numberOfTimePoints > 0)) {
        m_rowData.resize(m_numberOfBrainordinates);
        
        const CiftiFile* ciftiFile = getCiftiFile();
        
        /*
         * Read all of the data.  Time-series type files are not
         * too large and by caching the data, it eliminates 
         * numerous calls to read the data when correlation
         * is performed.
         */
        for (int32_t i = 0; i < m_numberOfBrainordinates; i++) {
            CaretAssertVectorIndex(m_rowData, i);
            m_rowData[i].m_data.resize(m_numberOfTimePoints);
            ciftiFile->getRow(&m_rowData[i].m_data[0],
                              i);
        }
    }
}


/**
 * Load data for the given column.
 *
 * @param dataOut
 *     Output with data.
 * @param index of the column.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::getDataForColumn(float* /*dataOut*/, const int64_t& /*index*/) const
{
    const AString msg("Should never be called for Dense Dynamic File");
    CaretAssertMessage(0, msg);
    CaretLogSevere(msg);
}

/**
 * Load data for the given row.
 *
 * @param dataOut
 *     Output with data.
 * @param index of the row.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::getDataForRow(float* dataOut, const int64_t& index) const
{
    if ((m_numberOfBrainordinates <= 0)
        || (m_numberOfTimePoints <= 0)) {
        return;
    }
    
    
    if ( ! m_meanSumSquaredValidFlag) {
        const_cast<CiftiConnectivityMatrixDenseDynamicFile*>(this)->computeRowMeanAndSumSquared();
        m_meanSumSquaredValidFlag = true;
    }
    
    CaretAssertVectorIndex(m_rowData, index);
    const float* myData = &m_rowData[index].m_data[0];
    
    std::vector<float> otherData(m_numberOfTimePoints);
    for (int32_t iRow = 0; iRow < m_numberOfBrainordinates; iRow++) {
        float coefficient = 1.0;
        
        if (iRow != index) {
//            CaretAssertVectorIndex(m_rowData, iRow);
//            coefficient = correlation(myData,
//                                      &m_rowData[iRow].m_data[0],
//                                      m_numberOfTimePoints);
            
            coefficient = correlation(index, iRow, m_numberOfTimePoints);
        }
        
        dataOut[iRow] = coefficient;
    }

//    std::cout << "Row: " << index << std::endl;
}

///**
// * Load data for the given row.
// *
// * @param dataOut
// *     Output with data.
// * @param index of the row.
// */
//void
//CiftiConnectivityMatrixDenseDynamicFile::getDataForRow(float* dataOut, const int64_t& index) const
//{
//    if ((m_numberOfBrainordinates <= 0)
//        || (m_numberOfTimePoints <= 0)) {
//        return;
//    }
//
//    ElapsedTimer timer;
//    timer.start();
//    
//    std::vector<float> myData(m_numberOfTimePoints);
//    getCiftiFile()->getRow(&myData[0], index);
//    
//    std::vector<float> otherData(m_numberOfTimePoints);
//    for (int32_t ib = 0; ib < m_numberOfBrainordinates; ib++) {
//        float coefficient = 1.0;
//        
//        if (ib != index) {
//            coefficient = 0.0;
//            
//            getCiftiFile()->getRow(&otherData[0], ib);
//            
//            coefficient = correlation(&myData[0],
//                                      &otherData[0],
//                                      m_numberOfTimePoints);
//            
//            
//            
//        }
//        
//        dataOut[ib] = coefficient;
//    }
//    
//    std::cout << "Time to correlate brainordinate: " << timer.getElapsedTimeSeconds() << std::endl;
//}

/**
 * Compute the mean and sum-squared for each row so that they 
 * are only calculated once.
 */
void
CiftiConnectivityMatrixDenseDynamicFile::computeRowMeanAndSumSquared()
{
    CaretAssert(m_numberOfBrainordinates > 0);
    CaretAssert(m_numberOfTimePoints > 0);
    
    const float numPointsFloat = m_numberOfTimePoints;
    
    for (int32_t iRow = 0; iRow < m_numberOfBrainordinates; iRow++) {
        double sum = 0.0;
        double sumSquared = 0.0;

        CaretAssertVectorIndex(m_rowData, iRow);
        const float* data = &m_rowData[iRow].m_data[0];
        
        for (int32_t iPoint = 0; iPoint < m_numberOfTimePoints; iPoint++) {
            CaretAssertVectorIndex(m_rowData[iRow].m_data, iPoint);
            sum        += data[iPoint];
            sumSquared += (data[iPoint] * data[iPoint]);
        }
        
        const float mean       = (sum / numPointsFloat);
        m_rowData[iRow].m_mean = mean;
        m_rowData[iRow].m_ssxx = (sumSquared - (numPointsFloat * mean * mean)); // equation (4) and (8)
    }
}

/**
 * Correlation
 * Citation:
 * Weisstein, Eric W. "Correlation Coefficient."
 * From MathWorld--A Wolfram Web Resource.
 * http://mathworld.wolfram.com/CorrelationCoefficient.html
 *
 * @param rowIndex
 *     Index of a row
 * @param y
 *     Index of another row
 * @param numberOfPoints
 *     Number of points int the two arrays
 * @return
 *     The correlation coefficient computed on the two arrays.
 */
float
CiftiConnectivityMatrixDenseDynamicFile::correlation(const int32_t rowIndex,
                                                     const int32_t otherRowIndex,
                                                     const int32_t numberOfPoints) const
{
    const double numFloat = numberOfPoints;
    double xySum = 0.0;
    
    CaretAssertVectorIndex(m_rowData, rowIndex);
    CaretAssertVectorIndex(m_rowData, otherRowIndex);
    const RowData& data = m_rowData[rowIndex];
    const RowData& otherData = m_rowData[otherRowIndex];
    
    for (int i = 0; i < numberOfPoints; i++) {
        CaretAssertVectorIndex(data.m_data, i);
        xySum += data.m_data[i] * otherData.m_data[i]; //  x[i] * y[i];
    }
    
    const double ssxx = data.m_ssxx; // equation (4)
    const double ssyy = otherData.m_ssxx; // equation (8)
    const double ssxy = xySum - (numFloat * data.m_mean * otherData.m_mean); // equation (12)
    
    float correlationCoefficient = 0.0;
    const double denom = ssxx * ssyy;
    if (denom != 0.0) {
        /*
         * R-Squared is Coefficient of Determination
         * https://en.wikipedia.org/wiki/Coefficient_of_determination
         */
        const float rSquared = static_cast<float>((ssxy * ssxy) / denom); // equation (22)
        correlationCoefficient = std::sqrt(rSquared);
    }
    
    return correlationCoefficient;
}


/**
 * Correlation
 * Citation:
 * Weisstein, Eric W. "Correlation Coefficient."
 * From MathWorld--A Wolfram Web Resource.
 * http://mathworld.wolfram.com/CorrelationCoefficient.html
 *
 * @param x
 *     Array of points
 * @param y
 *     Array of points
 * @param numberOfPoints
 *     Number of points int the two arrays
 * @return
 *     The correlation coefficient computed on the two arrays.
 */
float
CiftiConnectivityMatrixDenseDynamicFile::correlation(const float x[],
                                                     const float y[],
                                                     const int32_t numberOfPoints) const
{
    const double numFloat = numberOfPoints;
    double xSum  = 0.0;
    double x2Sum = 0.0;
    double ySum  = 0.0;
    double y2Sum = 0.0;
    double xySum = 0.0;
    
    for (int i = 0; i < numberOfPoints; i++) {
        xSum  += x[i];
        x2Sum += x[i] * x[i];
        ySum  += y[i];
        y2Sum += y[i] * y[i];
        xySum += x[i] * y[i];
    }
    
    const double xMean = xSum / numFloat;
    const double yMean = ySum / numFloat;
    
    const double ssxx = x2Sum - (numFloat * xMean * xMean); // equation (4)
    const double ssyy = y2Sum - (numFloat * yMean * yMean); // equation (8)
    const double ssxy = xySum - (numFloat * xMean * yMean); // equation (12)
    
    float correlationCoefficient = 0.0;
    const double denom = ssxx * ssyy;
    if (denom != 0.0) {
        /*
         * R-Squared is Coefficient of Determination
         * https://en.wikipedia.org/wiki/Coefficient_of_determination
         */
        const float rSquared = static_cast<float>((ssxy * ssxy) / denom); // equation (22)
        correlationCoefficient = std::sqrt(rSquared);
    }
    
    //            //
    //            // T-Value calculation from:
    //            //           Statistics for Psychology
    //            //           Arthur Aron & Elaine Aron
    //            //           2nd Edition, 1999
    //            //           page 98-99
    //            //
    //            float tDenom = 1.0 - correlationCoefficientR2;
    //            if (tDenom <= 1.0) {
    //                tDegreesOfFreedom = numFloat - 2.0;
    //                if (tDegreesOfFreedom >= 0.0) {
    //                    const float tNum = correlationCoefficientR * std::sqrt(tDegreesOfFreedom);
    //                    tValue = tNum / std::sqrt(tDenom);
    //                    pValue = StatisticGeneratePValue::getOneTailTTestPValue(tDegreesOfFreedom, tValue);
    //                }
    //            }
    
    return correlationCoefficient;
}


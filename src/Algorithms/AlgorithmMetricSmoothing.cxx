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

#include "AlgorithmMetricSmoothing.h"
#include "AlgorithmException.h"
#include "CaretOMP.h"
#include "CaretPointer.h"
#include "GeodesicHelper.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

#include <cmath>

using namespace caret;
using namespace std;

AString AlgorithmMetricSmoothing::getCommandSwitch()
{
    return "-metric-smoothing";
}

AString AlgorithmMetricSmoothing::getShortDescription()
{
    return "SMOOTH A METRIC FILE";
}

OperationParameters* AlgorithmMetricSmoothing::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to smooth on");
    
    ret->addMetricParameter(2, "metric-in", "the metric to smooth");
    
    ret->addDoubleParameter(3, "smoothing-kernel", "the size of the gaussian smoothing kernel in mm, as sigma by default");
    
    ret->addMetricOutputParameter(4, "metric-out", "the output metric");
    
    ret->createOptionalParameter(10, "-fwhm", "kernel size is FWHM, not sigma");
    
    OptionalParameter* roiOption = ret->createOptionalParameter(5, "-roi", "select a region of interest to smooth");
    roiOption->addMetricParameter(1, "roi-metric", "the roi to smooth within, as a metric");
    roiOption->createOptionalParameter(2, "-match-columns", "for each input column, use the corresponding column from the roi");
    
    ret->createOptionalParameter(6, "-fix-zeros", "treat zero values as not being data");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(7, "-column", "select a single column to smooth");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(8, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    OptionalParameter* methodSelect = ret->createOptionalParameter(9, "-method", "select smoothing method, default GEO_GAUSS_AREA");
    methodSelect->addStringParameter(1, "method", "the name of the smoothing method");
    
    ret->setHelpText(
        AString("Smooth a metric file on a surface.  ") +
        "By default, smooths all input columns on the entire surface, specify -column to use only one input column, and -roi to smooth only where " +
        "the roi metric is greater than 0, outputting zeros elsewhere.\n\n" +
        
        "When using -roi, input data outside the ROI is not used to compute the smoothed values.  " +
        "By default, the first column of the roi metric is used for all input columns.  " +
        "When -match-columns is specified to the -roi option, the input and roi metrics must have the same number of columns, " +
        "and for each input column's index, the same column index is used in the roi metric.  " +
        "If the -match-columns option to -roi is used while the -column option is also used, the number of columns must match between the roi and input metric, " +
        "and it will use the roi column with the index of the selected input column.\n\n" +
        
        "The -fix-zeros option causes the smoothing to not use an input value if it is zero, but still write a smoothed value to the vertex.  " +
        "This is useful for zeros that indicate lack of information, preventing them from pulling down the intensity of nearby vertices, while " +
        "giving the zero an extrapolated value.\n\n" +
        
        "The -corrected-areas option is intended for when it is unavoidable to smooth on a group average surface, it is only an approximate correction " +
        "for the reduction of structure in a group average surface.  It is better to smooth the data on individuals before averaging, when feasible.\n\n" +
        
        "Valid values for <method> are:\n\n" +
        "GEO_GAUSS_AREA - uses a geodesic gaussian kernel, and normalizes based on vertex area in order to work more reliably on irregular surfaces\n\n" +
        "GEO_GAUSS_EQUAL - uses a geodesic gaussian kernel, and normalizes assuming each vertex has equal importance\n\n" +
        "GEO_GAUSS - matches geodesic gaussian smoothing from caret5, but does not check kernels for having unequal importance\n\n" +
        
        "The GEO_GAUSS_AREA method is the default because it is usually the correct choice.  " +
        "GEO_GAUSS_EQUAL may be the correct choice when the sum of vertex values is more meaningful then the surface integral (sum of values .* areas), " +
        "for instance when smoothing vertex areas (the sum is the total surface area, while the surface integral is the sum of squares of the vertex areas).  " +
        "The GEO_GAUSS method is not recommended, it exists mainly to replicate methods of studies done with caret5's geodesic smoothing."
    );
    return ret;
}

void AlgorithmMetricSmoothing::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    double myKernel = myParams->getDouble(3);
    if (myParams->getOptionalParameter(10)->m_present)
    {
        myKernel = myKernel / (2.0 * sqrt(2.0 * log(2.0)));
    }
    MetricFile* myMetricOut = myParams->getOutputMetric(4);
    MetricFile* myRoi = NULL;
    bool matchRoiColumns = false;
    OptionalParameter* roiOption = myParams->getOptionalParameter(5);
    if (roiOption->m_present)
    {
        myRoi = roiOption->getMetric(1);
        matchRoiColumns = roiOption->getOptionalParameter(2)->m_present;
    }
    OptionalParameter* fixZerosOpt = myParams->getOptionalParameter(6);
    bool fixZeros = fixZerosOpt->m_present;
    int64_t columnNum = -1;
    OptionalParameter* columnSelect = myParams->getOptionalParameter(7);
    if (columnSelect->m_present)
    {
        columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    MetricFile* corrAreaMetric = NULL;
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(8);
    if (corrAreaOpt->m_present)
    {
        corrAreaMetric = corrAreaOpt->getMetric(1);
    }
    MetricSmoothingObject::Method myMethod = MetricSmoothingObject::GEO_GAUSS_AREA;
    OptionalParameter* methodSelect = myParams->getOptionalParameter(9);
    if (methodSelect->m_present)
    {
        AString methodName = methodSelect->getString(1);
        if (methodName == "GEO_GAUSS_AREA")
        {
            myMethod = MetricSmoothingObject::GEO_GAUSS_AREA;
        } else if (methodName == "GEO_GAUSS_EQUAL") {
            myMethod = MetricSmoothingObject::GEO_GAUSS_EQUAL;
        } else if (methodName == "GEO_GAUSS") {
            myMethod = MetricSmoothingObject::GEO_GAUSS;
        } else {
            throw AlgorithmException("unknown smoothing method name");
        }
    }
    AlgorithmMetricSmoothing(myProgObj, mySurf, myMetric, myKernel, myMetricOut, myRoi, matchRoiColumns, fixZeros, columnNum, corrAreaMetric, myMethod);
}

AlgorithmMetricSmoothing::AlgorithmMetricSmoothing(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric,
                                                   const double myKernel, MetricFile* myMetricOut, const MetricFile* myRoi, const bool matchRoiColumns,
                                                   const bool fixZeros, const int64_t columnNum, const MetricFile* corrAreaMetric, const MetricSmoothingObject::Method myMethod) : AbstractAlgorithm(myProgObj)
{
    float precomputeWeightWork = 5.0f;//TODO: adjust this based on number of columns to smooth, if we ever end up using progress indicators
    LevelProgress myProgress(myProgObj, 1.0f + precomputeWeightWork);
    int32_t numNodes = mySurf->getNumberOfNodes();
    if (numNodes != myMetric->getNumberOfNodes())
    {
        throw AlgorithmException("metric does not match surface in number of vertices");
    }
    if (myRoi != NULL && myRoi->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("roi metric does not match surface in number of vertices");
    }
    int32_t numCols = myMetric->getNumberOfColumns();
    if (columnNum < -1 || columnNum >= numCols)
    {
        throw AlgorithmException("invalid column number");
    }
    if (myKernel <= 0.0)
    {
        throw AlgorithmException("invalid kernel size");
    }
    if (myRoi != NULL && matchRoiColumns && numCols != myRoi->getNumberOfColumns())
    {
        throw AlgorithmException("match roi columns specified, but roi metric has the wrong number of columns");
    }
    CaretPointer<MetricSmoothingObject> mySmoothObj;
    const float* areaData = NULL;
    if (corrAreaMetric != NULL)
    {
        if (corrAreaMetric->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("corrected vertex areas metric does not match surface in number of vertices");
        }
        areaData = corrAreaMetric->getValuePointerForColumn(0);
    }
    myProgress.setTask("Precomputing Smoothing Weights");
    if (matchRoiColumns)
    {
        mySmoothObj.grabNew(new MetricSmoothingObject(mySurf, myKernel, NULL, myMethod, areaData));//don't use an ROI to build weights when the ROI changes each time
    } else {
        mySmoothObj.grabNew(new MetricSmoothingObject(mySurf, myKernel, myRoi, myMethod, areaData));
    }
    myProgress.reportProgress(precomputeWeightWork);
    if (columnNum == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, myMetric->getNumberOfColumns());
        myMetricOut->setStructure(mySurf->getStructure());
        for (int32_t col = 0; col < numCols; ++col)
        {
            myProgress.setTask("Smoothing Column " + AString::number(col));
            myMetricOut->setColumnName(col, myMetric->getColumnName(col) + ", smooth " + AString::number(myKernel));
            *(myMetricOut->getPaletteColorMapping(col)) = *(myMetric->getPaletteColorMapping(col));//copy the palette settings
            if (myRoi != NULL && matchRoiColumns)
            {
                mySmoothObj->smoothColumn(myMetric, col, myMetricOut, col, myRoi, col, fixZeros);
            } else {
                mySmoothObj->smoothColumn(myMetric, col, myMetricOut, col, myRoi, 0, fixZeros);
            }
            myProgress.reportProgress(precomputeWeightWork + ((float)col + 1) / numCols);
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(mySurf->getStructure());
        myMetricOut->setColumnName(0, myMetric->getColumnName(columnNum) + ", smooth " + AString::number(myKernel));
        *(myMetricOut->getPaletteColorMapping(0)) = *(myMetric->getPaletteColorMapping(columnNum));//copy the palette settings
        myProgress.setTask("Smoothing Column " + AString::number(columnNum));
        if (myRoi != NULL && matchRoiColumns)
        {
            mySmoothObj->smoothColumn(myMetric, columnNum, myMetricOut, 0, myRoi, columnNum, fixZeros);
        } else {
            mySmoothObj->smoothColumn(myMetric, columnNum, myMetricOut, 0, myRoi, 0, fixZeros);
        }
    }
}

float AlgorithmMetricSmoothing::getAlgorithmInternalWeight()
{
    return 1.0f;
}

float AlgorithmMetricSmoothing::getSubAlgorithmWeight()
{
    return 0.0f;
}

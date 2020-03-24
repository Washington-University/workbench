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

#include "AlgorithmMetricRegression.h"
#include "AlgorithmException.h"

#include "FloatMatrix.h"
#include "MetricFile.h"
#include "PaletteColorMapping.h"

using namespace caret;
using namespace std;

AString AlgorithmMetricRegression::getCommandSwitch()
{
    return "-metric-regression";
}

AString AlgorithmMetricRegression::getShortDescription()
{
    return "REGRESS METRICS OUT OF A METRIC FILE";
}

OperationParameters* AlgorithmMetricRegression::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addMetricParameter(1, "metric-in", "the metric to regress from");
    
    ret->addMetricOutputParameter(2, "metric-out", "the output metric");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(3, "-roi", "only regress inside an roi");
    roiOpt->addMetricParameter(1, "roi-metric", "the area to use for regression, as a metric");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(4, "-column", "select a single column to regress from");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    
    ParameterComponent* removeOpt = ret->createRepeatableParameter(5, "-remove", "specify a metric to regress out");
    removeOpt->addMetricParameter(1, "metric", "the metric file to use");
    OptionalParameter* removeColOpt = removeOpt->createOptionalParameter(2, "-remove-column", "select a column to use, rather than all");
    removeColOpt->addStringParameter(1, "column", "the column number or name");
    
    ParameterComponent* keepOpt = ret->createRepeatableParameter(6, "-keep", "specify a metric to include in regression, but not remove");
    keepOpt->addMetricParameter(1, "metric", "the metric file to use");
    OptionalParameter* keepColOpt = keepOpt->createOptionalParameter(2, "-keep-column", "select a column to use, rather than all");
    keepColOpt->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("For each regressor, its mean across the surface is subtracted from its data.  ") +
        "Each input map is then regressed against these, and a constant term.  The resulting regressed slopes of all regressors specified with -remove " +
        "are multiplied with their respective regressor maps, and these are subtracted from the input map."
    );
    return ret;
}

void AlgorithmMetricRegression::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    MetricFile* myMetricIn = myParams->getMetric(1);
    MetricFile* myMetricOut = myParams->getOutputMetric(2);
    MetricFile* myRoi = NULL;
    OptionalParameter* roiOpt = myParams->getOptionalParameter(3);
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getMetric(1);
    }
    int32_t myColumn = -1;
    OptionalParameter* columnOpt = myParams->getOptionalParameter(4);
    if (columnOpt->m_present)
    {
        myColumn = (int)myMetricIn->getMapIndexFromNameOrNumber(columnOpt->getString(1));
        if (myColumn < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    vector<pair<const MetricFile*, int> > remove, keep;
    const vector<ParameterComponent*>& removeInstances = myParams->getRepeatableParameterInstances(5);
    int numRemove = (int)removeInstances.size();
    if (numRemove == 0) throw AlgorithmException("you must specify at least one 'remove' metric");
    for (int i = 0; i < numRemove; ++i)
    {
        MetricFile* toRemove = removeInstances[i]->getMetric(1);
        int removeCol = -1;
        OptionalParameter* removeColOpt = removeInstances[i]->getOptionalParameter(2);
        if (removeColOpt->m_present)
        {
            removeCol = toRemove->getMapIndexFromNameOrNumber(removeColOpt->getString(1));
            if (removeCol < 0)
            {
                throw AlgorithmException("invalid column specified for 'remove' metric '" + toRemove->getFileName() + "'");
            }
        }
        remove.push_back(pair<const MetricFile*, int>(toRemove, removeCol));
    }
    const vector<ParameterComponent*>& keepInstances = myParams->getRepeatableParameterInstances(6);
    int numKeep = (int)keepInstances.size();
    for (int i = 0; i < numKeep; ++i)
    {
        MetricFile* toKeep = keepInstances[i]->getMetric(1);
        int keepCol = -1;
        OptionalParameter* keepColOpt = keepInstances[i]->getOptionalParameter(2);
        if (keepColOpt->m_present)
        {
            keepCol = toKeep->getMapIndexFromNameOrNumber(keepColOpt->getString(1));
            if (keepCol < 0)
            {
                throw AlgorithmException("invalid column specified for 'keep' metric '" + toKeep->getFileName() + "'");
            }
        }
        keep.push_back(pair<const MetricFile*, const int>(toKeep, keepCol));
    }
    AlgorithmMetricRegression(myProgObj, myMetricIn, myMetricOut, remove, keep, myColumn, myRoi);
}

AlgorithmMetricRegression::AlgorithmMetricRegression(ProgressObject* myProgObj, const MetricFile* myMetricIn, MetricFile* myMetricOut, const vector<pair<const MetricFile*, int> >& remove,
                                                     const vector<pair<const MetricFile*, int> >& keep, const int& myColumn, const MetricFile* myRoi) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    vector<vector<float> > regressCols;//because we are going to de-mean the input data
    int removeCount = 0;
    int numNodes = myMetricIn->getNumberOfNodes();
    int numColumns = myMetricIn->getNumberOfColumns();
    if (myColumn < -1 || myColumn >= numColumns)
    {
        throw AlgorithmException("invalid column number");
    }
    int numRemove = (int)remove.size();
    if (numRemove == 0) throw AlgorithmException("empty remove list in AlgorithmMetricRegression");
    const float* roiData = NULL;
    int numUsedNodes = numNodes;
    if (myRoi != NULL)
    {
        if (myRoi->getNumberOfNodes() != numNodes) throw AlgorithmException("roi metric has different number of nodes");
        roiData = myRoi->getValuePointerForColumn(0);
        numUsedNodes = 0;
        for (int i = 0; i < numNodes; ++i)
        {
            if (roiData[i] > 0.0f) ++numUsedNodes;
        }
    }
    for (int i = 0; i < numRemove; ++i)
    {
        const MetricFile* thisMetric = remove[i].first;
        if (thisMetric->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("'remove' metric '" + thisMetric->getFileName() + "' has a different number of nodes than the input metric");
        }
        int thisCol = remove[i].second;
        if (thisCol == -1)
        {
            int endCol = thisMetric->getNumberOfColumns();
            removeCount += endCol;
            for (int j = 0; j < endCol; ++j)
            {
                regressCols.push_back(vector<float>());
                demeanCol(thisMetric->getValuePointerForColumn(j), numNodes, roiData, regressCols.back());
            }
        } else {
            if (thisCol < 0 || thisCol >= thisMetric->getNumberOfColumns()) throw AlgorithmException("invalid column specified for metric '" + thisMetric->getFileName() + "'");
            ++removeCount;
            regressCols.push_back(vector<float>());
            demeanCol(thisMetric->getValuePointerForColumn(thisCol), numNodes, roiData, regressCols.back());
        }
    }
    int numKeep = (int)keep.size();//repeat, without increasing removeCount - this separates what gets removed after regression
    for (int i = 0; i < numKeep; ++i)
    {
        const MetricFile* thisMetric = keep[i].first;
        if (thisMetric->getNumberOfNodes() != numNodes)
        {
            throw AlgorithmException("'keep' metric '" + thisMetric->getFileName() + "' has a different number of nodes than the input metric");
        }
        int thisCol = keep[i].second;
        if (thisCol == -1)
        {
            int endCol = thisMetric->getNumberOfColumns();
            for (int j = 0; j < endCol; ++j)
            {
                regressCols.push_back(vector<float>());
                demeanCol(thisMetric->getValuePointerForColumn(j), numNodes, roiData, regressCols.back());
            }
        } else {
            if (thisCol < 0 || thisCol >= thisMetric->getNumberOfColumns()) throw AlgorithmException("invalid column specified for metric '" + thisMetric->getFileName() + "'");
            regressCols.push_back(vector<float>());
            demeanCol(thisMetric->getValuePointerForColumn(thisCol), numNodes, roiData, regressCols.back());
        }
    }
    FloatMatrix xtrans(regressCols);//use naive matrix math - not particularly efficient, but it works
    regressCols.clear();//don't need this any more, should call destructor on each member vector and release the memory
    xtrans = xtrans.concatVert(FloatMatrix::ones(1, numUsedNodes));//add constant term
    FloatMatrix toInvert = xtrans * xtrans.transpose();
    int invertSize = toInvert.getNumberOfRows();
    //a bit of a hack, but it should work, though this causes the work of inversion to be done twice
    if (toInvert.reducedRowEchelon()[invertSize - 1][invertSize - 1] != 1.0f) throw AlgorithmException("regression encountered a non-invertible matrix, check your inputs for linear independence");
    FloatMatrix solver = toInvert.inverse() * xtrans;//do most of the math in temporaries
    if (myColumn == -1)
    {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, numColumns);
        myMetricOut->setStructure(myMetricIn->getStructure());
        for (int i = 0; i < numColumns; ++i)
        {
            myMetricOut->setColumnName(i, myMetricIn->getColumnName(i) + " regressed");
            *(myMetricOut->getPaletteColorMapping(i)) = *(myMetricIn->getPaletteColorMapping(i));
            FloatMatrix y(numUsedNodes, 1);//create column vector for input - note that column vectors are extremely inefficient currently, as is copying the data...
            const float* data = myMetricIn->getValuePointerForColumn(i);
            int m = 0;
            for (int j = 0; j < numNodes; ++j)
            {
                if (roiData == NULL || roiData[j] > 0.0f)
                {
                    y[m][0] = data[j];
                    ++m;
                }
            }
            FloatMatrix regressed = solver * y;
            vector<float> outscratch(numNodes);
            m = 0;
            for (int j = 0; j < numNodes; ++j)
            {
                if (roiData == NULL || roiData[j] > 0.0f)
                {
                    outscratch[j] = data[j];
                    for (int k = 0; k < removeCount; ++k)
                    {
                        outscratch[j] -= regressed[k][0] * xtrans[k][m];
                    }
                    ++m;
                } else {
                    outscratch[j] = 0.0f;
                }
            }
            myMetricOut->setValuesForColumn(i, outscratch.data());
        }
    } else {
        myMetricOut->setNumberOfNodesAndColumns(numNodes, 1);
        myMetricOut->setStructure(myMetricIn->getStructure());
        myMetricOut->setColumnName(0, myMetricIn->getColumnName(myColumn) + " regressed");
        *(myMetricOut->getPaletteColorMapping(0)) = *(myMetricIn->getPaletteColorMapping(myColumn));
        FloatMatrix y(numUsedNodes, 1);//create column vector for input - note that column vectors are extremely inefficient currently, as is copying the data...
        const float* data = myMetricIn->getValuePointerForColumn(myColumn);
        int m = 0;
        for (int j = 0; j < numNodes; ++j)
        {
            if (roiData == NULL || roiData[j] > 0.0f)
            {
                y[m][0] = data[j];
                ++m;
            }
        }
        FloatMatrix regressed = solver * y;
        vector<float> outscratch(numNodes);
        m = 0;
        for (int j = 0; j < numNodes; ++j)
        {
            if (roiData == NULL || roiData[j] > 0.0f)
            {
                outscratch[j] = data[j];
                for (int k = 0; k < removeCount; ++k)
                {
                    outscratch[j] -= regressed[k][0] * xtrans[k][m];
                }
                ++m;
            } else {
                outscratch[j] = 0.0f;
            }
        }
        myMetricOut->setValuesForColumn(0, outscratch.data());
    }
}

void AlgorithmMetricRegression::demeanCol(const float* data, const int& count, const float* roiData, std::vector< float >& out)
{
    if (roiData == NULL)
    {
        double accum = 0.0;
        for (int i = 0; i < count; ++i)
        {
            accum += data[i];
        }
        accum /= count;
        out.resize(count);
        for (int i = 0; i < count; ++i)
        {
            out[i] = data[i] - accum;
        }
    } else {
        int usedCount = 0;
        double accum = 0.0;
        for (int i = 0; i < count; ++i)
        {
            if (roiData[i] > 0.0f)
            {
                accum += data[i];
                ++usedCount;
            }
        }
        accum /= usedCount;
        out.resize(usedCount);
        int m = 0;
        for (int i = 0; i < count; ++i)
        {
            if (roiData[i] > 0.0f)
            {
                out[m] = data[i] - accum;
                ++m;
            }
        }
    }
}

float AlgorithmMetricRegression::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricRegression::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}

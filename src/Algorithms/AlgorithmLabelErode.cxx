/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
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

#include "AlgorithmLabelErode.h"
#include "AlgorithmException.h"

#include "GeodesicHelper.h"
#include "LabelFile.h"
#include "GiftiLabelTable.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmLabelErode::getCommandSwitch()
{
    return "-label-erode";
}

AString AlgorithmLabelErode::getShortDescription()
{
    return "ERODE A LABEL FILE";
}

OperationParameters* AlgorithmLabelErode::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addLabelParameter(1, "label", "the input label");
    
    ret->addSurfaceParameter(2, "surface", "the surface to erode on");
    
    ret->addDoubleParameter(3, "erode-dist", "distance in mm to erode the labels");
    
    ret->addLabelOutputParameter(4, "label-out", "the output label file");
    
    OptionalParameter* dataRoiOpt = ret->createOptionalParameter(5, "-roi", "assume values outside this roi are labeled");
    dataRoiOpt->addMetricParameter(1, "roi-metric", "metric file, positive values denote vertices that have data");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(6, "-column", "select a single column to erode");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(7, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    ret->setHelpText(
        AString("Around each vertex that is unlabeled, set surrounding vertices to unlabeled.  ") +
        "The surrounding vertices are all immediate neighbors and all vertices within the specified distance." +
        "\n\nNote that the -corrected-areas option uses an approximate correction for distance along the surface."
    );
    return ret;
}

void AlgorithmLabelErode::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* myLabel = myParams->getLabel(1);
    SurfaceFile* mySurf = myParams->getSurface(2);
    float myDist = (float)myParams->getDouble(3);
    LabelFile* myLabelOut = myParams->getOutputLabel(4);
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    MetricFile* myRoi = NULL;
    if (roiOpt->m_present)
    {
        myRoi = roiOpt->getMetric(1);
    }
    OptionalParameter* columnSelect = myParams->getOptionalParameter(6);
    int columnNum = -1;
    if (columnSelect->m_present)
    {//set up to use the single column
        columnNum = (int)myLabel->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    OptionalParameter* corrAreaOpt = myParams->getOptionalParameter(7);
    MetricFile* corrAreas = NULL;
    if (corrAreaOpt->m_present)
    {
        corrAreas = corrAreaOpt->getMetric(1);
    }
    AlgorithmLabelErode(myProgObj, myLabel, mySurf, myDist, myLabelOut, myRoi, columnNum, corrAreas);
}

namespace
{
    vector<vector<int32_t> > precomputeStencils(const SurfaceFile* mySurf, const float& distance, const MetricFile* corrAreas, const float* roiCol)
    {
        int numNodes = mySurf->getNumberOfNodes();
        vector<vector<int32_t> > ret(numNodes);
        CaretPointer<GeodesicHelperBase> myGeoBase;
        if (corrAreas != NULL)
        {
            myGeoBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));
        }
        CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();//we aren't using to depth, so share the topology helper
#pragma omp CARET_PAR
        {//use parallel here so each thread gets its own geodesic helper
            CaretPointer<GeodesicHelper> myGeoHelp;
            if (corrAreas == NULL)
            {
                myGeoHelp = mySurf->getGeodesicHelper();
            } else {
                myGeoHelp.grabNew(new GeodesicHelper(myGeoBase));
            }
#pragma omp CARET_FOR schedule(dynamic)
            for (int i = 0; i < numNodes; ++i)
            {
                if (roiCol == NULL || roiCol[i] > 0.0f)
                {
                    vector<int32_t> geoNodes;
                    vector<float> geoDists;
                    myGeoHelp->getNodesToGeoDist(i, distance, geoNodes, geoDists);
                    const vector<int32_t>& topoNodes = myTopoHelp->getNodeNeighbors(i);
                    set<int32_t> mergeSet(geoNodes.begin(), geoNodes.end());
                    mergeSet.insert(topoNodes.begin(), topoNodes.end());
                    mergeSet.erase(i);//center of stencil is already 0 if stencil is used, so don't set it again
                    ret[i] = vector<int32_t>(mergeSet.begin(), mergeSet.end());
                }
            }
        }
        return ret;
    }
}

AlgorithmLabelErode::AlgorithmLabelErode(ProgressObject* myProgObj, const LabelFile* myLabel, const SurfaceFile* mySurf, const float& myDist, LabelFile* myLabelOut,
                                         const MetricFile* myRoi, const int& columnNum, const MetricFile* corrAreas) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int numNodes = mySurf->getNumberOfNodes();
    if (numNodes != myLabel->getNumberOfNodes())
    {
        throw AlgorithmException("surface and metric number of vertices do not match");
    }
    if (myRoi != NULL && myRoi->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("roi number of vertices does not match");
    }
    if (corrAreas != NULL && corrAreas->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("corrected areas metric number of vertices does not match");
    }
    int numInColumns = myLabel->getNumberOfColumns();
    if (columnNum < -1 || columnNum >= numInColumns)
    {
        throw AlgorithmException("invalid column specified");
    }
    if (myDist < 0.0f)
    {
        throw AlgorithmException("distance cannot be negative");
    }
    if (columnNum == -1)
    {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, numInColumns);
    } else {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, 1);
    }
    myLabelOut->setStructure(mySurf->getStructure());
    const int32_t unlabeledKey = myLabel->getLabelTable()->getUnassignedLabelKey();//will have same key for both
    *(myLabelOut->getLabelTable()) = *(myLabel->getLabelTable());//file type has only one label table for the whole file
    const float* roiCol = NULL;
    if (myRoi != NULL)
    {
        roiCol = myRoi->getValuePointerForColumn(0);
    }
    vector<vector<int32_t> > stencils = precomputeStencils(mySurf, myDist, corrAreas, roiCol);
    if (columnNum == -1)
    {
        for (int c = 0; c < numInColumns; ++c)
        {
            const int32_t* inData = myLabel->getLabelKeyPointerForColumn(c);
            vector<int32_t> scratchCol(inData, inData + numNodes);
            for (int i = 0; i < numNodes; ++i)
            {
                if (roiCol == NULL || roiCol[i] > 0.0f)
                {
                    if (inData[i] == unlabeledKey)
                    {
                        for (int n = 0; n < (int)stencils[i].size(); ++n)
                        {
                            scratchCol[stencils[i][n]] = unlabeledKey;
                        }
                    }
                } else {
                    scratchCol[i] = unlabeledKey;//zero things outside the data roi
                }
            }
            myLabelOut->setMapName(c, myLabel->getMapName(c));
            myLabelOut->setLabelKeysForColumn(c, scratchCol.data());
        }
    } else {
        const int32_t* inData = myLabel->getLabelKeyPointerForColumn(columnNum);
        vector<int32_t> scratchCol(inData, inData + numNodes);
        for (int i = 0; i < numNodes; ++i)
        {
            if (roiCol == NULL || roiCol[i] > 0.0f)
            {
                if (inData[i] == unlabeledKey)
                {
                    for (int n = 0; n < (int)stencils[i].size(); ++n)
                    {
                        scratchCol[stencils[i][n]] = unlabeledKey;
                    }
                }
            } else {
                scratchCol[i] = unlabeledKey;//zero things outside the data roi
            }
        }
        myLabelOut->setMapName(0, myLabel->getMapName(columnNum));
        myLabelOut->setLabelKeysForColumn(0, scratchCol.data());
    }
}

float AlgorithmLabelErode::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelErode::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}

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

#include "AlgorithmLabelDilate.h"

#include "AlgorithmException.h"
#include "CaretOMP.h"
#include "GeodesicHelper.h"
#include "LabelFile.h"
#include "MetricFile.h"
#include "GiftiLabelTable.h"
#include "SurfaceFile.h"
#include "TopologyHelper.h"

using namespace caret;
using namespace std;

AString AlgorithmLabelDilate::getCommandSwitch()
{
    return "-label-dilate";
}

AString AlgorithmLabelDilate::getShortDescription()
{
    return "DILATE A LABEL FILE";
}

OperationParameters* AlgorithmLabelDilate::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addLabelParameter(1, "label", "the input label");
    
    ret->addSurfaceParameter(2, "surface", "the surface to dilate on");
    
    ret->addDoubleParameter(3, "dilate-dist", "distance in mm to dilate the labels");
    
    ret->addLabelOutputParameter(4, "label-out", "the output label file");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-bad-vertex-roi", "specify an roi of vertices to overwrite, rather than vertices with the unlabeled key");
    roiOpt->addMetricParameter(1, "roi-metric", "metric file, positive values denote vertices to have their values replaced");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(6, "-column", "select a single column to dilate");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    OptionalParameter* corrAreaOpt = ret->createOptionalParameter(7, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreaOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
        
    ret->setHelpText(
        AString("Fills in label information for all vertices designated as bad, up to the specified distance away from other labels.  ") +
        "If -bad-vertex-roi is specified, all vertices, including those with the unlabeled key, are good, except for vertices with a positive value in the ROI.  " +
        "If it is not specified, only vertices with the unlabeled key are bad."
    );
    return ret;
}

void AlgorithmLabelDilate::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LabelFile* myLabel = myParams->getLabel(1);
    SurfaceFile* mySurf = myParams->getSurface(2);
    float myDist = (float)myParams->getDouble(3);
    LabelFile* myLabelOut = myParams->getOutputLabel(4);
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    MetricFile* badNodeRoi = NULL;
    if (roiOpt->m_present)
    {
        badNodeRoi = roiOpt->getMetric(1);
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
    AlgorithmLabelDilate(myProgObj, myLabel, mySurf, myDist, myLabelOut, badNodeRoi, columnNum, corrAreas);
}

AlgorithmLabelDilate::AlgorithmLabelDilate(ProgressObject* myProgObj, const LabelFile* myLabel, const SurfaceFile* mySurf, float myDist, LabelFile* myLabelOut,
                                           const MetricFile* badNodeRoi, int columnNum, const MetricFile* corrAreas) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int32_t unusedLabel = myLabel->getLabelTable()->getUnassignedLabelKey();
    int numColumns = myLabel->getNumberOfColumns();
    if (myDist < 0.0f)
    {
        throw AlgorithmException("invalid distance specified");
    }
    if (columnNum < -1 || columnNum >= numColumns)
    {
        throw AlgorithmException("invalid column specified");
    }
    int numNodes = myLabel->getNumberOfNodes();
    if (mySurf->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("surface has wrong number of vertices for this label file");
    }
    if (corrAreas != NULL && corrAreas->getNumberOfNodes() != numNodes)
    {
        throw AlgorithmException("corrected areas metric number of vertices does not match");
    }
    CaretArray<int32_t> colScratch(numNodes);
    CaretArray<int> markArray(numNodes);
    if (badNodeRoi != NULL)
    {
        const float* myRoiData = badNodeRoi->getValuePointerForColumn(0);
        for (int i = 0; i < numNodes; ++i)
        {
            if (myRoiData[i] > 0.0f)
            {
                markArray[i] = 0;
            } else {
                markArray[i] = 1;
            }
        }
    }
    CaretPointer<GeodesicHelperBase> myCorrBase;
    if (corrAreas != NULL)
    {
        myCorrBase.grabNew(new GeodesicHelperBase(mySurf, corrAreas->getValuePointerForColumn(0)));
    }
    if (columnNum == -1)
    {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, myLabel->getNumberOfColumns());
        *(myLabelOut->getLabelTable()) = *(myLabel->getLabelTable());
        myLabelOut->setStructure(mySurf->getStructure());
        for (int thisCol = 0; thisCol < myLabel->getNumberOfColumns(); ++thisCol)
        {
            const int32_t* myInputData = myLabel->getLabelKeyPointerForColumn(thisCol);
            myLabelOut->setColumnName(thisCol, myLabel->getColumnName(thisCol) + " dilated");
            if (badNodeRoi == NULL)
            {
                for (int i = 0; i < numNodes; ++i)
                {
                    if (myInputData[i] == unusedLabel)
                    {
                        markArray[i] = 0;
                    } else {
                        markArray[i] = 1;
                    }
                }
            }
#pragma omp CARET_PAR
            {
                CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
                CaretPointer<GeodesicHelper> myGeoHelp;
                if (corrAreas == NULL)
                {
                    myGeoHelp = mySurf->getGeodesicHelper();
                } else {
                    myGeoHelp.grabNew(new GeodesicHelper(myCorrBase));
                }
#pragma omp CARET_FOR schedule(dynamic)
                for (int i = 0; i < numNodes; ++i)
                {
                    if (markArray[i] == 0)
                    {
                        vector<int32_t> nodeList;
                        vector<float> distList;
                        myGeoHelp->getNodesToGeoDist(i, myDist, nodeList, distList);
                        int numInRange = (int)nodeList.size();
                        bool first = true;
                        float bestDist = -1.0f;
                        int32_t bestLabel = unusedLabel;
                        for (int j = 0; j < numInRange; ++j)
                        {
                            if (markArray[nodeList[j]] == 1)
                            {
                                if (first || distList[j] < bestDist)
                                {
                                    first = false;
                                    bestDist = distList[j];
                                    bestLabel = myInputData[nodeList[j]];
                                }
                            }
                        }
                        if (!first)
                        {
                            colScratch[i] = bestLabel;
                        } else {
                            nodeList = myTopoHelp->getNodeNeighbors(i);
                            nodeList.push_back(i);
                            myGeoHelp->getGeoToTheseNodes(i, nodeList, distList);//ok, its a little silly to do this
                            numInRange = (int)nodeList.size();
                            for (int j = 0; j < numInRange; ++j)
                            {
                                if (markArray[nodeList[j]] == 1)
                                {
                                    if (first || distList[j] < bestDist)
                                    {
                                        first = false;
                                        bestDist = distList[j];
                                        bestLabel = myInputData[nodeList[j]];
                                    }
                                }
                            }
                            if (!first)
                            {
                                colScratch[i] = bestLabel;
                            } else {
                                colScratch[i] = unusedLabel;
                            }
                        }
                    } else {
                        colScratch[i] = myInputData[i];
                    }
                }
            }
            myLabelOut->setLabelKeysForColumn(thisCol, colScratch.getArray());
        }
    } else {
        myLabelOut->setNumberOfNodesAndColumns(numNodes, 1);
        *(myLabelOut->getLabelTable()) = *(myLabel->getLabelTable());
        myLabelOut->setStructure(mySurf->getStructure());
        const int32_t* myInputData = myLabel->getLabelKeyPointerForColumn(columnNum);
        myLabelOut->setColumnName(0, myLabel->getColumnName(columnNum) + " dilated");
        if (badNodeRoi == NULL)
        {
            for (int i = 0; i < numNodes; ++i)
            {
                if (myInputData[i] == unusedLabel)
                {
                    markArray[i] = 0;
                } else {
                    markArray[i] = 1;
                }
            }
        }
#pragma omp CARET_PAR
        {
            CaretPointer<TopologyHelper> myTopoHelp = mySurf->getTopologyHelper();
            CaretPointer<GeodesicHelper> myGeoHelp;
            if (corrAreas == NULL)
            {
                myGeoHelp = mySurf->getGeodesicHelper();
            } else {
                myGeoHelp.grabNew(new GeodesicHelper(myCorrBase));
            }
#pragma omp CARET_FOR schedule(dynamic)
            for (int i = 0; i < numNodes; ++i)
            {
                if (markArray[i] == 0)
                {
                    vector<int32_t> nodeList;
                    vector<float> distList;
                    myGeoHelp->getNodesToGeoDist(i, myDist, nodeList, distList);
                    int numInRange = (int)nodeList.size();
                    bool first = true;
                    float bestDist = -1.0f;
                    int32_t bestLabel = unusedLabel;
                    for (int j = 0; j < numInRange; ++j)
                    {
                        if (markArray[nodeList[j]] == 1)
                        {
                            if (first || distList[j] < bestDist)
                            {
                                first = false;
                                bestDist = distList[j];
                                bestLabel = myInputData[nodeList[j]];
                            }
                        }
                    }
                    if (!first)
                    {
                        colScratch[i] = bestLabel;
                    } else {
                        nodeList = myTopoHelp->getNodeNeighbors(i);
                        nodeList.push_back(i);
                        myGeoHelp->getGeoToTheseNodes(i, nodeList, distList);//ok, its a little silly to do this
                        numInRange = (int)nodeList.size();
                        for (int j = 0; j < numInRange; ++j)
                        {
                            if (markArray[nodeList[j]] == 1)
                            {
                                if (first || distList[j] < bestDist)
                                {
                                    first = false;
                                    bestDist = distList[j];
                                    bestLabel = myInputData[nodeList[j]];
                                }
                            }
                        }
                        if (!first)
                        {
                            colScratch[i] = bestLabel;
                        } else {
                            colScratch[i] = unusedLabel;
                        }
                    }
                } else {
                    colScratch[i] = myInputData[i];
                }
            }
        }
        myLabelOut->setLabelKeysForColumn(0, colScratch.getArray());
    }
}

float AlgorithmLabelDilate::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelDilate::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}

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

#include "AlgorithmMetricROIsToBorder.h"
#include "AlgorithmException.h"

#include "Border.h"
#include "BorderFile.h"
#include "BorderTracingHelper.h"
#include "GiftiLabelTable.h"
#include "MetricFile.h"
#include "SurfaceFile.h"

#include <cstdlib>

using namespace caret;
using namespace std;

AString AlgorithmMetricROIsToBorder::getCommandSwitch()
{
    return "-metric-rois-to-border";
}

AString AlgorithmMetricROIsToBorder::getShortDescription()
{
    return "DRAW BORDERS AROUND METRIC ROIS";
}

OperationParameters* AlgorithmMetricROIsToBorder::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addSurfaceParameter(1, "surface", "the surface to use for neighbor information");
    
    ret->addMetricParameter(2, "metric", "the input metric containing ROIs");
    
    ret->addStringParameter(3, "class-name", "the name to use for the class of the output borders");
    
    ret->addBorderOutputParameter(4, "border-out", "the output border file");
    
    OptionalParameter* placeOpt = ret->createOptionalParameter(5, "-placement", "set how far along the edge border points are drawn");
    placeOpt->addDoubleParameter(1, "fraction", "fraction along edge from inside vertex (default 0.33)");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(6, "-column", "select a single column");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("For each ROI column, finds all edges on the mesh that cross the boundary of the ROI, and draws borders through them.  ") +
        "By default, this is done on all columns in the input file, using the map name as the name for the border."
    );
    return ret;
}

void AlgorithmMetricROIsToBorder::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    MetricFile* myMetric = myParams->getMetric(2);
    AString className = myParams->getString(3);
    BorderFile* myBorderOut = myParams->getOutputBorder(4);
    float placement = 0.33f;
    OptionalParameter* placeOpt = myParams->getOptionalParameter(5);
    if (placeOpt->m_present)
    {
        placement = (float)placeOpt->getDouble(1);
    }
    OptionalParameter* columnSelect = myParams->getOptionalParameter(6);
    int columnNum = -1;
    if (columnSelect->m_present)
    {//set up to use the single column
        columnNum = (int)myMetric->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    AlgorithmMetricROIsToBorder(myProgObj, mySurf, myMetric, className, myBorderOut, placement, columnNum);
}

AlgorithmMetricROIsToBorder::AlgorithmMetricROIsToBorder(ProgressObject* myProgObj, const SurfaceFile* mySurf, const MetricFile* myMetric, const AString& className,
                                                         BorderFile* myBorderOut, const float& placement, const int& columnNum) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (mySurf->getNumberOfNodes() != myMetric->getNumberOfNodes()) throw AlgorithmException("label file does not match surface file number of vertices");
    if (placement < 0.0f || placement > 1.0f || placement != placement) throw AlgorithmException("placement must be between 0 and 1");
    if (columnNum < -1 || columnNum > myMetric->getNumberOfColumns()) throw AlgorithmException("invalid column specified");
    myBorderOut->setStructure(mySurf->getStructure());
    myBorderOut->setNumberOfNodes(mySurf->getNumberOfNodes());
    BorderTracingHelper myHelper(mySurf);
    GiftiLabelTable myNameTable;
    if (columnNum == -1)
    {
        for (int col = 0; col < myMetric->getNumberOfColumns(); ++col)
        {
            vector<CaretPointer<Border> > result = myHelper.traceData(myMetric->getValuePointerForColumn(col), BorderTracingHelper::GreaterThan<float>(0.0f, false), placement);
            myNameTable.addLabel(myMetric->getMapName(col), rand() & 255, rand() & 255, rand() & 255);
            for (int i = 0; i < (int)result.size(); ++i)
            {
                result[i]->setClassName(className);
                result[i]->setName(myMetric->getMapName(col));
                myBorderOut->addBorder(result[i].releasePointer());//NOTE: addBorder takes ownership of a RAW POINTER, shared_ptr won't release the pointer
            }
        }
    } else {
        myNameTable.addLabel(myMetric->getMapName(columnNum), rand() & 255, rand() & 255, rand() & 255);
        vector<CaretPointer<Border> > result = myHelper.traceData(myMetric->getValuePointerForColumn(columnNum), BorderTracingHelper::GreaterThan<float>(0.0f, false), placement);
        for (int i = 0; i < (int)result.size(); ++i)
        {
            result[i]->setClassName(className);
            result[i]->setName(myMetric->getMapName(columnNum));
            myBorderOut->addBorder(result[i].releasePointer());
        }
    }
    *(myBorderOut->getNameColorTable()) = myNameTable;
}

float AlgorithmMetricROIsToBorder::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmMetricROIsToBorder::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}

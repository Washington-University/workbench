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

#include "AlgorithmLabelToBorder.h"
#include "AlgorithmException.h"

#include "Border.h"
#include "BorderFile.h"
#include "BorderTracingHelper.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString AlgorithmLabelToBorder::getCommandSwitch()
{
    return "-label-to-border";
}

AString AlgorithmLabelToBorder::getShortDescription()
{
    return "DRAW BORDERS AROUND LABELS";
}

OperationParameters* AlgorithmLabelToBorder::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addSurfaceParameter(1, "surface", "the surface to use for neighbor information");
    
    ret->addLabelParameter(2, "label-in", "the input label file");
    
    ret->addBorderOutputParameter(3, "border-out", "the output border file");
    
    OptionalParameter* placeOpt = ret->createOptionalParameter(4, "-placement", "set how far along the edge border points are drawn");
    placeOpt->addDoubleParameter(1, "fraction", "fraction along edge from inside vertex (default 0.33)");
    
    OptionalParameter* columnSelect = ret->createOptionalParameter(5, "-column", "select a single column");
    columnSelect->addStringParameter(1, "column", "the column number or name");
    
    ret->setHelpText(
        AString("For each label, finds all edges on the mesh that cross the boundary of the label, and draws borders through them.  ") +
        "By default, this is done on all columns in the input file, using the map name as the class name for the border."
    );
    return ret;
}

void AlgorithmLabelToBorder::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    SurfaceFile* mySurf = myParams->getSurface(1);
    LabelFile* myLabel = myParams->getLabel(2);
    BorderFile* myBorderOut = myParams->getOutputBorder(3);
    float placement = 0.33f;
    OptionalParameter* placeOpt = myParams->getOptionalParameter(4);
    if (placeOpt->m_present)
    {
        placement = (float)placeOpt->getDouble(1);
    }
    OptionalParameter* columnSelect = myParams->getOptionalParameter(5);
    int columnNum = -1;
    if (columnSelect->m_present)
    {//set up to use the single column
        columnNum = (int)myLabel->getMapIndexFromNameOrNumber(columnSelect->getString(1));
        if (columnNum < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    AlgorithmLabelToBorder(myProgObj, mySurf, myLabel, myBorderOut, placement, columnNum);
}

AlgorithmLabelToBorder::AlgorithmLabelToBorder(ProgressObject* myProgObj, const SurfaceFile* mySurf, const LabelFile* myLabel, BorderFile* myBorderOut,
                                               const float& placement, const int& columnNum) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    if (mySurf->getNumberOfNodes() != myLabel->getNumberOfNodes()) throw AlgorithmException("label file does not match surface file number of vertices");
    if (placement < 0.0f || placement > 1.0f || placement != placement) throw AlgorithmException("placement must be between 0 and 1");
    if (columnNum < -1 || columnNum >= myLabel->getNumberOfColumns()) throw AlgorithmException("invalid column specified");
    myBorderOut->setStructure(mySurf->getStructure());
    myBorderOut->setNumberOfNodes(mySurf->getNumberOfNodes());
    BorderTracingHelper myHelper(mySurf);
    if (columnNum == -1)
    {
        for (int col = 0; col < myLabel->getNumberOfColumns(); ++col)
        {
            const GiftiLabelTable* myTable = myLabel->getLabelTable();
            set<int32_t> myKeys = myTable->getKeys();
            int32_t unassignedKey = myTable->getUnassignedLabelKey();
            for (set<int32_t>::iterator iter = myKeys.begin(); iter != myKeys.end(); ++iter)
            {
                if (*iter == unassignedKey) continue;
                vector<CaretPointer<Border> > result = myHelper.traceData(myLabel->getLabelKeyPointerForColumn(col), BorderTracingHelper::LabelSelect(*iter), placement);
                AString borderName = myTable->getLabelName(*iter);
                myBorderOut->getNameColorTable()->addLabel(myTable->getLabel(*iter));
                for (int i = 0; i < (int)result.size(); ++i)
                {
                    result[i]->setClassName(myLabel->getMapName(col));
                    result[i]->setName(borderName);
                    myBorderOut->addBorder(result[i].releasePointer());//NOTE: addBorder takes ownership of a RAW POINTER, shared_ptr won't release the pointer
                }
            }
        }
    } else {
        const GiftiLabelTable* myTable = myLabel->getLabelTable();
        set<int32_t> myKeys = myTable->getKeys();
        int32_t unassignedKey = myTable->getUnassignedLabelKey();
        for (set<int32_t>::iterator iter = myKeys.begin(); iter != myKeys.end(); ++iter)
        {
            if (*iter == unassignedKey) continue;
            vector<CaretPointer<Border> > result = myHelper.traceData(myLabel->getLabelKeyPointerForColumn(columnNum), BorderTracingHelper::LabelSelect(*iter), placement);
            AString borderName = myTable->getLabelName(*iter);
            myBorderOut->getNameColorTable()->addLabel(myTable->getLabel(*iter));
            for (int i = 0; i < (int)result.size(); ++i)
            {
                result[i]->setClassName(myLabel->getMapName(columnNum));
                result[i]->setName(borderName);
                myBorderOut->addBorder(result[i].releasePointer());//ditto
            }
        }
    }
}

float AlgorithmLabelToBorder::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmLabelToBorder::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}

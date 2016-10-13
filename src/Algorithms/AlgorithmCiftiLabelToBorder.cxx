/*LICENSE_START*/
/*
 *  Copyright (C) 2016  Washington University School of Medicine
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

#include "AlgorithmCiftiLabelToBorder.h"
#include "AlgorithmException.h"

#include "AlgorithmCiftiSeparate.h"
#include "AlgorithmLabelToBorder.h"
#include "CaretLogger.h"
#include "BorderFile.h"
#include "CiftiFile.h"
#include "LabelFile.h"
#include "SurfaceFile.h"

using namespace caret;
using namespace std;

AString AlgorithmCiftiLabelToBorder::getCommandSwitch()
{
    return "-cifti-label-to-border";
}

AString AlgorithmCiftiLabelToBorder::getShortDescription()
{
    return "DRAW BORDERS AROUND CIFTI LABELS";
}

OperationParameters* AlgorithmCiftiLabelToBorder::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the input cifti dlabel file");
    
    OptionalParameter* placeOpt = ret->createOptionalParameter(2, "-placement", "set how far along the edge border points are drawn");
    placeOpt->addDoubleParameter(1, "fraction", "fraction along edge from inside vertex (default 0.33)");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(3, "-column", "select a single column");
    columnOpt->addStringParameter(1, "column", "the column number or name");
    
    ParameterComponent* borderOpt = ret->createRepeatableParameter(4, "-border", "specify output file for a surface structure");
    borderOpt->addSurfaceParameter(1, "surface", "the surface to use for neighbor and structure information");
    borderOpt->addBorderOutputParameter(2, "border-out", "the output border file");
    
    ret->setHelpText(
        AString("For each surface, takes the labels on the matching structure and draws borders around the labels.  ") +
        "Use -column to only draw borders around one label map."
    );
    return ret;
}

void AlgorithmCiftiLabelToBorder::useParameters(OperationParameters* myParams, ProgressObject* /*myProgObj*/)
{
    CiftiFile* myCifti = myParams->getCifti(1);
    float placement = 0.33f;
    OptionalParameter* placeOpt = myParams->getOptionalParameter(2);
    if (placeOpt->m_present)
    {
        placement = (float)placeOpt->getDouble(1);
    }
    int64_t column = -1;
    OptionalParameter* columnOpt = myParams->getOptionalParameter(3);
    if (columnOpt->m_present)
    {
        //row is first dimension, 0D cifti won't load, so don't need a test
        column = myCifti->getCiftiXML().getMap(CiftiXML::ALONG_ROW)->getIndexFromNumberOrName(columnOpt->getString(2));
        if (column < 0)
        {
            throw AlgorithmException("invalid column specified");
        }
    }
    const vector<ParameterComponent*>& borderOpts = *(myParams->getRepeatableParameterInstances(4));
    if (borderOpts.empty()) CaretLogWarning("no output requested from -cifti-label-to-border, command will do nothing");
    for (int i = 0; i < (int)borderOpts.size(); ++i)
    {
        ParameterComponent& thisBorderOpt = *(borderOpts[i]);
        AlgorithmCiftiLabelToBorder(NULL, myCifti, thisBorderOpt.getSurface(1), thisBorderOpt.getOutputBorder(2), placement, column);
    }
}

AlgorithmCiftiLabelToBorder::AlgorithmCiftiLabelToBorder(ProgressObject* myProgObj, const CiftiFile* myCifti, const SurfaceFile* mySurf,
                                                         BorderFile* borderOut, const float& placement, const int& column) : AbstractAlgorithm(myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiXML& myXML = myCifti->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2) throw AlgorithmException("input cifti file should have 2 dimensions, has " + AString::number(myXML.getNumberOfDimensions()));
    if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) != CiftiMappingType::BRAIN_MODELS) throw AlgorithmException("input cifti file does not have brain models mapping along column");
    if (myXML.getMappingType(CiftiXML::ALONG_ROW) != CiftiMappingType::LABELS) throw AlgorithmException("input cifti file does not have labels mapping along row");
    LabelFile tempLabel;
    AlgorithmCiftiSeparate(NULL, myCifti, CiftiXML::ALONG_COLUMN, mySurf->getStructure(), &tempLabel);//NOTE: bad surface structure errors are printed from this algorithm
    AlgorithmLabelToBorder(NULL, mySurf, &tempLabel, borderOut, placement, column);
}

float AlgorithmCiftiLabelToBorder::getAlgorithmInternalWeight()
{
    return 1.0f;//override this if needed, if the progress bar isn't smooth
}

float AlgorithmCiftiLabelToBorder::getSubAlgorithmWeight()
{
    //return AlgorithmInsertNameHere::getAlgorithmWeight();//if you use a subalgorithm
    return 0.0f;
}

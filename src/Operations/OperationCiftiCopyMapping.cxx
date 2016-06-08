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

#include "OperationCiftiCopyMapping.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "MultiDimIterator.h"

using namespace caret;
using namespace std;

AString OperationCiftiCopyMapping::getCommandSwitch()
{
    return "-cifti-copy-mapping";
}

AString OperationCiftiCopyMapping::getShortDescription()
{
    return "DEPRECATED: use -cifti-change-mapping";
}

OperationParameters* OperationCiftiCopyMapping::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "data-cifti", "the cifti file to use the data from");
    
    ret->addStringParameter(2, "replace-dir", "which direction on <data-cifti> to replace the mapping");
    
    ret->addCiftiParameter(3, "template-cifti", "a cifti file containing the desired mapping");
    
    ret->addStringParameter(4, "template-dir", "which direction on <template-cifti> to use the mapping from");
    
    ret->addCiftiOutputParameter(5, "cifti-out", "the output cifti file");
    
    ret->setHelpText(
        AString("DEPRECATED: this command may be removed in a future release, use -cifti-change-mapping.\n\n") +
        "<data-cifti> must have the same length along the replace direction as <template-cifti> has along the template direction.  " +
        CiftiXML::directionFromStringExplanation()
    );
    return ret;
}

void OperationCiftiCopyMapping::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const CiftiFile* dataCifti = myParams->getCifti(1);
    int dataDir = CiftiXML::directionFromString(myParams->getString(2));
    const CiftiFile* templateCifti = myParams->getCifti(3);
    int templateDir = CiftiXML::directionFromString(myParams->getString(4));
    CiftiFile* ciftiOut = myParams->getOutputCifti(5);
    CiftiXML outXML = dataCifti->getCiftiXML();//we need a copy of it so we can modify
    if (dataDir >= dataCifti->getCiftiXML().getNumberOfDimensions())
    {
        throw OperationException("specified direction doesn't exist in data file");
    }
    if (templateDir >= templateCifti->getCiftiXML().getNumberOfDimensions())
    {
        throw OperationException("specified direction doesn't exist in template file");
    }
    if (outXML.getDimensionLength(dataDir) != templateCifti->getCiftiXML().getDimensionLength(templateDir))
    {
        throw OperationException("selected directions on files have different length");
    }
    outXML.setMap(dataDir, *(templateCifti->getCiftiXML().getMap(templateDir)));
    ciftiOut->setCiftiXML(outXML);
    vector<int64_t> outDims = outXML.getDimensions();
    vector<float> scratchrow(outDims[0]);
    for (MultiDimIterator<int64_t> iter(vector<int64_t>(outDims.begin() + 1, outDims.end())); !iter.atEnd(); ++iter)
    {//drop the first dimension, row length
        dataCifti->getRow(scratchrow.data(), *iter);
        ciftiOut->setRow(scratchrow.data(), *iter);
    }
}

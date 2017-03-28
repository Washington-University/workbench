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

#include "OperationConvertMatrix4ToMatrix2.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "CaretSparseFile.h"

using namespace caret;
using namespace std;

AString OperationConvertMatrix4ToMatrix2::getCommandSwitch()
{
    return "-convert-matrix4-to-matrix2";
}

AString OperationConvertMatrix4ToMatrix2::getShortDescription()
{
    return "GENERATES A MATRIX2 CIFTI FROM MATRIX4 WBSPARSE";
}

OperationParameters* OperationConvertMatrix4ToMatrix2::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "matrix4-wbsparse", "a wbsparse matrix4 file");
    
    ret->addCiftiOutputParameter(2, "counts-out", "the total fiber counts, as a cifti file");
    
    OptionalParameter* distanceOpt = ret->createOptionalParameter(3, "-distances", "output average trajectory distance");
    distanceOpt->addCiftiOutputParameter(1, "distance-out", "the distances, as a cifti file");
    
    OptionalParameter* fibersOpt = ret->createOptionalParameter(4, "-individual-fibers", "output files for each fiber direction");
    fibersOpt->addCiftiOutputParameter(1, "fiber-1", "output file for first fiber");
    fibersOpt->addCiftiOutputParameter(2, "fiber-2", "output file for second fiber");
    fibersOpt->addCiftiOutputParameter(3, "fiber-3", "output file for third fiber");
    
    ret->setHelpText(
        AString("This command makes a cifti file from the fiber counts in a matrix4 wbsparse file, and optionally a second cifti file from the distances.  ") +
        "Note that while the total count is stored exactly, the per-fiber counts are stored as approximate fractions, so the output of -individual-fibers will contain nonintegers."
    );
    return ret;
}

void OperationConvertMatrix4ToMatrix2::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString matrix4Name = myParams->getString(1);
    CiftiFile* myCountsOut = myParams->getOutputCifti(2);
    CiftiFile* myDistOut = NULL;
    OptionalParameter* distanceOpt = myParams->getOptionalParameter(3);
    if (distanceOpt->m_present)
    {
        myDistOut = distanceOpt->getOutputCifti(1);
    }
    CiftiFile* fiber1 = NULL, *fiber2 = NULL, *fiber3 = NULL;
    OptionalParameter* fibersOpt = myParams->getOptionalParameter(4);
    if (fibersOpt->m_present)
    {
        fiber1 = fibersOpt->getOutputCifti(1);
        fiber2 = fibersOpt->getOutputCifti(2);
        fiber3 = fibersOpt->getOutputCifti(3);
    }
    CaretSparseFile matrix4(matrix4Name);
    const CiftiXML& myXML = matrix4.getCiftiXML();
    myCountsOut->setCiftiXML(myXML);
    if (myDistOut != NULL) myDistOut->setCiftiXML(myXML);
    if (fiber1 != NULL)
    {
        fiber1->setCiftiXML(myXML);
        fiber2->setCiftiXML(myXML);
        fiber3->setCiftiXML(myXML);
    }
    int rowSize = myXML.getDimensionLength(CiftiXML::ALONG_ROW), numRows = myXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
    for (int i = 0; i < numRows; ++i)
    {
        vector<float> scratchRow(rowSize, 0.0f);
        vector<float> scratchFiber1 = scratchRow, scratchFiber2 = scratchRow, scratchFiber3 = scratchRow;
        vector<int64_t> indices;
        vector<FiberFractions> fibers;
        matrix4.getFibersRowSparse(i, indices, fibers);
        int numIndices = (int)indices.size();
        for (int j = 0; j < numIndices; ++j)
        {
            scratchRow[indices[j]] = fibers[j].totalCount;
            if (fiber1 != NULL)
            {
                CaretAssert(fibers[j].fiberFractions.size() == 3);
                scratchFiber1[indices[j]] = fibers[j].totalCount * fibers[j].fiberFractions[0];
                scratchFiber2[indices[j]] = fibers[j].totalCount * fibers[j].fiberFractions[1];
                scratchFiber3[indices[j]] = fibers[j].totalCount * fibers[j].fiberFractions[2];
            }
        }
        myCountsOut->setRow(scratchRow.data(), i);
        if (fiber1 != NULL)
        {
            fiber1->setRow(scratchFiber1.data(), i);
            fiber2->setRow(scratchFiber2.data(), i);
            fiber3->setRow(scratchFiber3.data(), i);
        }
        if (myDistOut != NULL)
        {
            for (int j = 0; j < numIndices; ++j)
            {
                scratchRow[indices[j]] = fibers[j].distance;
            }
            myDistOut->setRow(scratchRow.data(), i);
        }
    }
}

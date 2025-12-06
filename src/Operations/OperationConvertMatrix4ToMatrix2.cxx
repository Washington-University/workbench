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

#include "CaretCommandGlobalOptions.h"
#include "CaretPointer.h"
#include "CaretSparseFile.h"
#include "CiftiFile.h"

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
    
    ret->addStringParameter(2, "counts-out", "output - the total fiber counts");//HACK: fake the output formatting
    
    ret->createOptionalParameter(5, "-wbsparse", "write all outputs as .wbsparse files, not cifti");
    
    OptionalParameter* distanceOpt = ret->createOptionalParameter(3, "-distances", "output average trajectory distance");
    distanceOpt->addStringParameter(1, "distance-out", "the distances");
    
    OptionalParameter* fibersOpt = ret->createOptionalParameter(4, "-individual-fibers", "output files for each fiber direction");
    fibersOpt->addStringParameter(1, "fiber-1", "output - output file for first fiber");
    fibersOpt->addStringParameter(2, "fiber-2", "output - output file for second fiber");
    fibersOpt->addStringParameter(3, "fiber-3", "output - output file for third fiber");
    
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
    AString myCountsOutName = myParams->getString(2);
    AString myDistOutName;
    bool writeDist = false, writeFibers = false;
    OptionalParameter* distanceOpt = myParams->getOptionalParameter(3);
    if (distanceOpt->m_present)
    {
        myDistOutName = distanceOpt->getString(1);
        writeDist = true;
    }
    AString fiber1Name, fiber2Name, fiber3Name;
    OptionalParameter* fibersOpt = myParams->getOptionalParameter(4);
    if (fibersOpt->m_present)
    {
        fiber1Name = fibersOpt->getString(1);
        fiber2Name = fibersOpt->getString(2);
        fiber3Name = fibersOpt->getString(3);
        writeFibers = true;
    }
    bool wbsparseOut = myParams->getOptionalParameter(5)->m_present;
    CaretSparseFile matrix4(matrix4Name);
    const CiftiXML& myXML = matrix4.getCiftiXML();
    CiftiFile myCountsOutCifti, myDistOutCifti, fiber1Cifti, fiber2Cifti, fiber3Cifti;
    CaretPointer<CaretSparseFileWriter> myCountsOutSparse, myDistOutSparse, fiber1Sparse, fiber2Sparse, fiber3Sparse;
    if (wbsparseOut)
    {
        myCountsOutSparse.grabNew(new CaretSparseFileWriter(myCountsOutName, myXML, CaretSparseFile::Float32));
        if (writeDist) myDistOutSparse.grabNew(new CaretSparseFileWriter(myDistOutName, myXML, CaretSparseFile::Float32));
        if (writeFibers)
        {
            fiber1Sparse.grabNew(new CaretSparseFileWriter(fiber1Name, myXML, CaretSparseFile::Float32));
            fiber2Sparse.grabNew(new CaretSparseFileWriter(fiber2Name, myXML, CaretSparseFile::Float32));
            fiber3Sparse.grabNew(new CaretSparseFileWriter(fiber3Name, myXML, CaretSparseFile::Float32));
        }
    } else {
        myCountsOutCifti.setCiftiXML(myXML);
        myCountsOutCifti.setWritingFile(myCountsOutName);
        caret_global_command_options.applyOptions(&myCountsOutCifti);
        if (writeDist)
        {
            myDistOutCifti.setCiftiXML(myXML);
            myDistOutCifti.setWritingFile(myDistOutName);
            caret_global_command_options.applyOptions(&myDistOutCifti);
        }
        if (writeFibers)
        {
            fiber1Cifti.setCiftiXML(myXML);
            fiber1Cifti.setWritingFile(fiber1Name);
            caret_global_command_options.applyOptions(&fiber1Cifti);
            fiber2Cifti.setCiftiXML(myXML);
            fiber2Cifti.setWritingFile(fiber2Name);
            caret_global_command_options.applyOptions(&fiber2Cifti);
            fiber3Cifti.setCiftiXML(myXML);
            fiber3Cifti.setWritingFile(fiber3Name);
            caret_global_command_options.applyOptions(&fiber3Cifti);
        }
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
            if (writeFibers)
            {
                CaretAssert(fibers[j].fiberFractions.size() == 3);
                scratchFiber1[indices[j]] = fibers[j].totalCount * fibers[j].fiberFractions[0];
                scratchFiber2[indices[j]] = fibers[j].totalCount * fibers[j].fiberFractions[1];
                scratchFiber3[indices[j]] = fibers[j].totalCount * fibers[j].fiberFractions[2];
            }
        }
        if (wbsparseOut)
        {
            myCountsOutSparse->writeRow(i, scratchRow.data());
        } else {
            myCountsOutCifti.setRow(scratchRow.data(), i);
        }
        if (writeFibers)
        {
            if (wbsparseOut)
            {
                fiber1Sparse->writeRow(i, scratchFiber1.data());
                fiber2Sparse->writeRow(i, scratchFiber2.data());
                fiber3Sparse->writeRow(i, scratchFiber3.data());
            } else {
                fiber1Cifti.setRow(scratchFiber1.data(), i);
                fiber2Cifti.setRow(scratchFiber2.data(), i);
                fiber3Cifti.setRow(scratchFiber3.data(), i);
            }
        }
        if (writeDist)
        {
            for (int j = 0; j < numIndices; ++j)
            {
                scratchRow[indices[j]] = fibers[j].distance;
            }
            if (wbsparseOut)
            {
                myDistOutSparse->writeRow(i, scratchRow.data());
            } else {
                myDistOutCifti.setRow(scratchRow.data(), i);
            }
        }
    }
}

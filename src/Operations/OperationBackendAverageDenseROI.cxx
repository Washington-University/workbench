/*LICENSE_START*/
/*
 *  Copyright 1995-2002 Washington University School of Medicine
 *
 *  http://brainmap.wustl.edu
 *
 *  This file is part of CARET.
 *
 *  CARET is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  CARET is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with CARET; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include "AString.h"
#include "ByteOrderEnum.h"
#include "ByteSwapping.h"
#include "CiftiFile.h"
#include "FileInformation.h"
#include "OperationBackendAverageDenseROI.h"
#include "OperationException.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace caret;
using namespace std;

AString OperationBackendAverageDenseROI::getCommandSwitch()
{
    return "-backend-average-dense-roi";
}

AString OperationBackendAverageDenseROI::getShortDescription()
{
    return "CONNECTOME DB BACKEND COMMAND FOR CIFTI AVERAGE DENSE ROI";
}

OperationParameters* OperationBackendAverageDenseROI::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "index-list", "comma separated list of cifti indexes to average");
    ret->addStringParameter(2, "out-file", "file to write the average row to");
    ret->setHelpText(
        AString("This command is probably not the one you are looking for, try -cifti-average-dense-roi.  ") +
        "It takes the list of cifti files to average from standard input, and writes its output as little endian, " +
        "32-bit integer of row size followed by the row as 32-bit floats."
    );
    return ret;
}

void OperationBackendAverageDenseROI::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString indexListString, outfileName;
    indexListString = myParams->getString(1);
    outfileName = myParams->getString(2);
    CiftiXML baseXML;//TODO: remove when switching to raw reading
    bool ok = false;
    vector<int> indexList;
    QStringList indexStrings = indexListString.split(",");
    int numStrings = (int)indexStrings.size();
    indexList.resize(numStrings);
    for (int i = 0; i < numStrings; ++i)
    {
        indexList[i] = indexStrings[i].toInt(&ok);
        if (!ok)
        {
            throw OperationException("failed to parse '" + indexStrings[i] + "' as integer");
        }
        if (indexList[i] < 0)
        {
            throw OperationException("negative integers are not valid cifti indexes");
        }
    }
    vector<const CiftiInterface*> ciftiList;
    try
    {
        string myLine;
        while (cin.good())
        {
            if (!getline(cin, myLine))
            {
                break;
            }
            if (myLine == "")
            {
                continue;//skip blank lines
            }
            FileInformation ciftiFileInfo(myLine.c_str());
            if (!ciftiFileInfo.exists())
            {
                throw OperationException(AString("file does not exist: ") + myLine.c_str());//throw inside try block so that the error handling path is the same
            }
            CiftiFile* tempCifti = new CiftiFile(myLine.c_str(), ON_DISK);//TODO: skip CiftiFile to avoid XML parsing overhead
            ciftiList.push_back(tempCifti);
        }
        int numCifti = (int)ciftiList.size();
        if (numCifti > 0)
        {
            baseXML = ciftiList[0]->getCiftiXML();
            int numRows = baseXML.getNumberOfRows();
            int rowSize = baseXML.getNumberOfColumns();
            vector<double> accum(rowSize, 0.0);
            vector<float> rowScratch(rowSize);
            for (int i = 0; i < numCifti; ++i)
            {
                if (baseXML != ciftiList[i]->getCiftiXML())//equality testing is smart, compares mapping equivalence, despite potential ambiguity
                {
                    throw OperationException("error, cifti header of file #" + AString::number(i + 1) + " doesn't match");
                }
                for (int j = 0; j < numStrings; ++j)
                {
                    if (indexList[j] >= numRows)
                    {
                        throw OperationException("error, cifti index outside number of rows");
                    }
                    ciftiList[i]->getRow(rowScratch.data(), indexList[j]);
                    for (int k = 0; k < rowSize; ++k)
                    {
                        accum[k] += rowScratch[k];
                    }
                }
            }
            for (int k = 0; k < rowSize; ++k)
            {
                rowScratch[k] = accum[k] / numCifti / numStrings;
            }
            int32_t outSize = rowSize;
            if (ByteOrderEnum::isSystemBigEndian())
            {
                ByteSwapping::swapBytes(rowScratch.data(), rowSize);//beware, we are doing the byteswapping in place
                ByteSwapping::swapBytes(&outSize, 1);
            }
            ofstream outfile(outfileName.toLocal8Bit().constData(), ios_base::out | ios_base::binary | ios_base::trunc);
            if (!outfile.write((char*)&outSize, 4))
            {
                throw OperationException("error writing output");
            }
            if (!outfile.write((char*)rowScratch.data(), rowSize * sizeof(float)))
            {
                throw OperationException("error writing output");
            }
            outfile.close();
        }
    } catch (CaretException& e) {//catch exceptions to prevent memory leaks
        for (size_t i = 0; i < ciftiList.size(); ++i)
        {
            delete ciftiList[i];
        }
        throw e;
    } catch (std::exception& e) {
        for (size_t i = 0; i < ciftiList.size(); ++i)
        {
            delete ciftiList[i];
        }
        throw OperationException(e.what());
    } catch (...) {
        for (size_t i = 0; i < ciftiList.size(); ++i)
        {
            delete ciftiList[i];
        }
        throw;
    }
    for (size_t i = 0; i < ciftiList.size(); ++i)
    {
        delete ciftiList[i];
    }
}

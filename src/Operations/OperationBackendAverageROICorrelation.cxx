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

#include "AString.h"
#include "ByteOrderEnum.h"
#include "ByteSwapping.h"
#include "CaretOMP.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "FileInformation.h"
#include "OperationBackendAverageROICorrelation.h"
#include "OperationException.h"

#include <iostream>
#include <fstream>
#include <string>

using namespace caret;
using namespace std;

AString OperationBackendAverageROICorrelation::getCommandSwitch()
{
    return "-backend-average-roi-correlation";
}

AString OperationBackendAverageROICorrelation::getShortDescription()
{
    return "CONNECTOME DB BACKEND COMMAND FOR CIFTI AVERAGE ROI CORRELATION";
}

OperationParameters* OperationBackendAverageROICorrelation::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "index-list", "comma separated list of cifti indexes to average and then correlate");
    ret->addStringParameter(2, "out-file", "file to write the average row to");
    ret->setHelpText(
        AString("This command is probably not the one you are looking for, try -cifti-average-roi-correlation.  ") +
        "It takes the list of cifti files to average from standard input, and writes its output as little endian, " +
        "32-bit integer of row size followed by the row as 32-bit floats."
    );
    return ret;
}

void OperationBackendAverageROICorrelation::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
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
    vector<CaretPointer<const CiftiFile> > ciftiList;
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
            throw OperationException(AString("file does not exist: ") + myLine.c_str());
        }
        CaretPointer<CiftiFile> tempCifti(new CiftiFile(myLine.c_str()));//can't skip parsing XML, as different arguments could be different cifti versions, which results in different dimension order
        ciftiList.push_back(tempCifti);
    }
    int numCifti = (int)ciftiList.size();
    if (numCifti > 0)
    {
        baseXML = ciftiList[0]->getCiftiXML();
        if (baseXML.getNumberOfDimensions() != 2) throw OperationException("operation only supports 2D cifti files");
        int rowSize = baseXML.getDimensionLength(CiftiXML::ALONG_ROW);
        vector<double> accum(rowSize, 0.0);
        vector<float> rowScratch(rowSize);
        for (int i = 0; i < numCifti; ++i)
        {
            if (!baseXML.approximateMatch(ciftiList[i]->getCiftiXML()))//equality testing is smart, compares mapping equivalence, despite multiple ways to specify some mappings
            {
                throw OperationException("error, cifti header of file #" + AString::number(i + 1) + " doesn't match");
            }
            processCifti(ciftiList[i], indexList, rowScratch);
            for (int k = 0; k < rowSize; ++k)
            {
                accum[k] += rowScratch[k];
            }
        }
        for (int k = 0; k < rowSize; ++k)
        {
            rowScratch[k] = accum[k] / numCifti / numStrings;
        }
        int32_t outSize = rowSize;
        if (ByteOrderEnum::isSystemBigEndian())
        {
            ByteSwapping::swapArray(rowScratch.data(), rowSize);//beware, we are doing the byteswapping in place
            ByteSwapping::swap(outSize);
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
}

void OperationBackendAverageROICorrelation::processCifti(const CiftiFile* myCifti, const vector<int>& indexList, vector<float>& output)
{
    int rowSize = myCifti->getNumberOfColumns();
    int colSize = myCifti->getNumberOfRows();
    int listSize = (int)indexList.size();
    vector<double> accumarray(rowSize, 0.0);
    vector<float> average(rowSize);
    for (int i = 0; i < listSize; ++i)
    {
        if (indexList[i] >= colSize)//we already checked for negatives
        {
            throw OperationException("cifti index too large");
        }
        myCifti->getRow(average.data(), indexList[i]);
        for (int j = 0; j < rowSize; ++j)
        {
            accumarray[j] += average[j];
        }
    }
    double accum = 0.0;
    for (int i = 0; i < rowSize; ++i)
    {
        average[i] = accumarray[i] / listSize;
        accum += average[i];
    }
    float mean = accum / rowSize;
    accum = 0.0;
    for (int i = 0; i < rowSize; ++i)
    {
        average[i] -= mean;
        accum += average[i] * average[i];
    }
    float rrs = sqrt(accum);
    int curRow = 0;
#pragma omp CARET_PAR
    {
        vector<float> rowscratch(rowSize);
#pragma omp CARET_FOR schedule(dynamic)
        for (int i = 0; i < colSize; ++i)
        {
            int myRow;
#pragma omp critical
            {
                myRow = curRow;//force sequential reading
                ++curRow;
                myCifti->getRow(rowscratch.data(), myRow);//but never read multiple rows at once from the same file
            }
            double tempaccum = 0.0;//compute mean of new row
            for (int j = 0; j < rowSize; ++j)
            {
                tempaccum += rowscratch[j];
            }
            float thismean = tempaccum / rowSize;
            tempaccum = 0.0;
            double corraccum = 0.0;//correlate
            for (int j = 0; j < rowSize; ++j)
            {
                float tempf = rowscratch[j] - thismean;
                tempaccum += tempf * tempf;//compute rrs on the fly
                corraccum += tempf * average[j];//gather the correlation
            }
            corraccum /= rrs * sqrt(tempaccum);
            if (corraccum > 0.999999) corraccum = 0.999999;
            if (corraccum < -0.999999) corraccum = -0.999999;
            output[i] = 0.5 * log((1 + corraccum) / (1 - corraccum));
        }
    }
}

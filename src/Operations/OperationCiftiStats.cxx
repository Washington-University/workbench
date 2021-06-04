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

#include "OperationCiftiStats.h"
#include "OperationException.h"

#include "CiftiFile.h"
#include "ReductionOperation.h"

#include <algorithm>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <vector>

using namespace caret;
using namespace std;

AString OperationCiftiStats::getCommandSwitch()
{
    return "-cifti-stats";
}

AString OperationCiftiStats::getShortDescription()
{
    return "STATISTICS ALONG CIFTI COLUMNS";
}

OperationParameters* OperationCiftiStats::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiParameter(1, "cifti-in", "the input cifti");
    
    OptionalParameter* reduceOpt = ret->createOptionalParameter(2, "-reduce", "use a reduction operation");
    reduceOpt->addStringParameter(1, "operation", "the reduction operation");
    
    OptionalParameter* percentileOpt = ret->createOptionalParameter(3, "-percentile", "give the value at a percentile");
    percentileOpt->addDoubleParameter(1, "percent", "the percentile to find, must be between 0 and 100");
    
    OptionalParameter* columnOpt = ret->createOptionalParameter(4, "-column", "only display output for one column");
    columnOpt->addIntegerParameter(1, "column", "the column index (starting from 1)");
    
    OptionalParameter* roiOpt = ret->createOptionalParameter(5, "-roi", "only consider data inside an roi");
    roiOpt->addCiftiParameter(1, "roi-cifti", "the roi, as a cifti file");
    roiOpt->createOptionalParameter(2, "-match-maps", "each column of input uses the corresponding column from the roi file");
    
    ret->createOptionalParameter(6, "-show-map-name", "print column index and name before each output");
    
    ret->setHelpText(
        AString("For each column of the input, a line of text is printed, resulting from the specified reduction or percentile operation.  ") +
        "If -roi is specified without -match-maps, then each line will contain as many numbers as there are maps in the ROI file, separated by tab characters.  " +
        "Use -column to only give output for a single data column.  " +
        "Exactly one of -reduce or -percentile must be specified.\n\n" +
        "The argument to the -reduce option must be one of the following:\n\n" +
        ReductionOperation::getHelpInfo());
    return ret;
}

namespace
{
    float reduce(const vector<float>& data, const ReductionEnum::Enum& myop, const vector<float>& roiData)
    {
        if (roiData.empty())
        {
            return ReductionOperation::reduce(data.data(), data.size(), myop);
        } else {
            int64_t numElems = (int64_t)data.size();
            CaretAssert(numElems == (int64_t)roiData.size());
            vector<float> toUse;
            toUse.reserve(numElems);
            for (int64_t i = 0; i < numElems; ++i)
            {
                if (roiData[i] > 0.0f)
                {
                    toUse.push_back(data[i]);
                }
            }
            if (toUse.empty()) throw OperationException("roi column is empty");
            return ReductionOperation::reduce(toUse.data(), toUse.size(), myop);
        }
    }
    
    float percentile(const vector<float>& data, const float& percent, const vector<float>& roiData)
    {
        CaretAssert(percent >= 0.0f && percent <= 100.0f);
        vector<float> toUse;
        if (roiData.empty())
        {
            toUse = data;
        } else {
            int64_t numElems = (int64_t)data.size();
            CaretAssert(numElems == (int64_t)roiData.size());
            toUse.reserve(numElems);
            for (int i = 0; i < numElems; ++i)
            {
                if (roiData[i] > 0.0f)
                {
                    toUse.push_back(data[i]);
                }
            }
        }
        if (toUse.empty()) throw OperationException("roi is empty");
        sort(toUse.begin(), toUse.end());
        const float index = percent / 100.0f * (toUse.size() - 1);
        if (index <= 0) return toUse[0];
        if (index >= toUse.size() - 1) return toUse.back();
        float ipart, fpart;
        fpart = modf(index, &ipart);
        return (1.0f - fpart) * toUse[(int)ipart] + fpart * toUse[((int)ipart) + 1];
    }
}

void OperationCiftiStats::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    CiftiFile* myInput = myParams->getCifti(1);
    const CiftiXML& myXML = myInput->getCiftiXML();
    if (myXML.getNumberOfDimensions() != 2) throw OperationException("only 2D cifti are supported in this command");
    int64_t numCols = myXML.getDimensionLength(CiftiXML::ALONG_ROW);
    int64_t colLength = myXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
    OptionalParameter* reduceOpt = myParams->getOptionalParameter(2);
    OptionalParameter* percentileOpt = myParams->getOptionalParameter(3);
    if (reduceOpt->m_present == percentileOpt->m_present)//use == as logical xor
    {
        throw OperationException("you must use exactly one of -reduce or -percentile");
    }
    ReductionEnum::Enum myop = ReductionEnum::INVALID;
    if (reduceOpt->m_present)
    {
        bool ok = false;
        myop = ReductionEnum::fromName(reduceOpt->getString(1), &ok);
        if (!ok) throw OperationException("unrecognized reduction operation: " + reduceOpt->getString(1));
    }
    float percent = 0.0f;
    if (percentileOpt->m_present)
    {
        percent = (float)percentileOpt->getDouble(1);//use not within range to trap NaNs, just in case
        if (!(percent >= 0.0f && percent <= 100.0f)) throw OperationException("percentile must be between 0 and 100");
    }
    int useColumn = -1;
    OptionalParameter* columnOpt = myParams->getOptionalParameter(4);
    if (columnOpt->m_present)
    {
        useColumn = columnOpt->getInteger(1) - 1;
        if (useColumn < 0 || useColumn >= numCols) throw OperationException("invalid column specified");
    }
    vector<float> roiData;
    bool matchColumnMode = false;
    CiftiFile* roiCifti = NULL;
    int64_t numRois = 1;//trick: pretend we have 1 roi map when we don't have an roi file, for fewer special cases
    OptionalParameter* roiOpt = myParams->getOptionalParameter(5);
    if (roiOpt->m_present)
    {
        roiCifti = roiOpt->getCifti(1);
        if (!roiCifti->getCiftiXML().getMap(CiftiXML::ALONG_COLUMN)->approximateMatch(*(myXML.getMap(CiftiXML::ALONG_COLUMN))))
        {
            throw OperationException("roi cifti does not match input cifti along columns");
        }
        roiData.resize(colLength);
        if (roiOpt->getOptionalParameter(2)->m_present)
        {
            if (myXML.getMap(CiftiXML::ALONG_ROW)->getLength() != roiCifti->getCiftiXML().getMap(CiftiXML::ALONG_ROW)->getLength())
            {
                throw OperationException("-match-maps specified, but roi has different number of columns than input");
            }
            matchColumnMode = true;
        }
        numRois = roiCifti->getCiftiXML().getDimensionLength(CiftiXML::ALONG_ROW);
    }
    bool showMapName = myParams->getOptionalParameter(6)->m_present;
    const CiftiMappingType* rowMap = myXML.getMap(CiftiXML::ALONG_ROW);
    vector<float> colScratch(colLength);
    int64_t columnStart, columnEnd;
    if (useColumn == -1)
    {
        myInput->convertToInMemory();//we will be getting all columns, so read it all in first
        if (roiCifti != NULL) roiCifti->convertToInMemory();//ditto
        columnStart = 0;
        columnEnd = numCols;
    } else {
        if (roiCifti != NULL && !matchColumnMode) roiCifti->convertToInMemory();//matching maps with one column selected is the only time we don't need the whole ROI file
        columnStart = useColumn;
        columnEnd = useColumn + 1;
    }
    for (int i = columnStart; i < columnEnd; ++i)
    {
        myInput->getColumn(colScratch.data(), i);
        if (showMapName)
        {
            cout << AString::number(i + 1) << ":\t" << rowMap->getIndexName(i) << ":\t";
        }
        if (matchColumnMode)
        {//trick: matchColumn is only true when we have an roi
            roiCifti->getColumn(roiData.data(), i);
            float result;
            if (reduceOpt->m_present)
            {
                result = reduce(colScratch, myop, roiData);
            } else {
                CaretAssert(percentileOpt->m_present);
                result = percentile(colScratch, percent, roiData);
            }
            stringstream resultsstr;
            resultsstr << setprecision(7) << result;
            cout << resultsstr.str() << endl;
        } else {
            for (int64_t j = 0; j < numRois; ++j)
            {
                if (roiCifti != NULL) roiCifti->getColumn(roiData.data(), j);
                float result;
                if (reduceOpt->m_present)
                {
                    result = reduce(colScratch, myop, roiData);
                } else {
                    CaretAssert(percentileOpt->m_present);
                    result = percentile(colScratch, percent, roiData);
                }
                stringstream resultsstr;
                resultsstr << setprecision(7) << result;
                if (j != 0) cout << "\t";
                cout << resultsstr.str();
            }
            cout << endl;
        }
    }
}

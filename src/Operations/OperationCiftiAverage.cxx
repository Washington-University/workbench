/*LICENSE_START*/
/*
 *  Copyright (C) 2021  Washington University School of Medicine
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

#include "OperationCiftiAverage.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretOMP.h"
#include "CiftiFile.h"
#include "MathFunctions.h"

#include <vector>

using namespace caret;
using namespace std;

AString OperationCiftiAverage::getCommandSwitch()
{
    return "-cifti-average";
}

AString OperationCiftiAverage::getShortDescription()
{
    return "AVERAGE CIFTI FILES";
}

OperationParameters* OperationCiftiAverage::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addCiftiOutputParameter(1, "cifti-out", "output cifti file");
    
    OptionalParameter* excludeOpt = ret->createOptionalParameter(2, "-exclude-outliers", "exclude outliers by standard deviation of each element across files");
    excludeOpt->addDoubleParameter(1, "sigma-below", "number of standard deviations below the mean to include");
    excludeOpt->addDoubleParameter(2, "sigma-above", "number of standard deviations above the mean to include");

    ParameterComponent* ciftiOpt = ret->createRepeatableParameter(3, "-cifti", "specify an input file");
    ciftiOpt->addCiftiParameter(1, "cifti-in", "the input cifti file");
    
    OptionalParameter* weightOpt = ciftiOpt->createOptionalParameter(1, "-weight", "give a weight for this file");
    weightOpt->addDoubleParameter(1, "weight", "the weight to use");
    
    OptionalParameter* memLimitOpt = ret->createOptionalParameter(4, "-mem-limit", "restrict memory used for file reading efficiency");
    memLimitOpt->addDoubleParameter(1, "limit-GB", "memory limit in gigabytes");
    
    ret->setHelpText(
        AString("Averages cifti files together.  ") +
        "Files without -weight specified are given a weight of 1.  " +
        "If -exclude-outliers is specified, at each element, the data across all files is taken as a set, its unweighted mean and sample standard deviation are found, " +
        "and values outside the specified number of standard deviations are excluded from the (potentially weighted) average at that element."
    );
    return ret;
}

void OperationCiftiAverage::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    const vector<ParameterComponent*>& myInstances = myParams->getRepeatableParameterInstances(3);
    if (myInstances.size() < 1)
    {
        throw OperationException("no input files specified");
    }
    bool exclude = false;
    float sigmaBelow = -1.0f, sigmaAbove = -1.0f;
    OptionalParameter* excludeOpt = myParams->getOptionalParameter(2);
    if (excludeOpt->m_present)
    {
        exclude = true;
        sigmaBelow = float(excludeOpt->getDouble(1));
        sigmaAbove = float(excludeOpt->getDouble(2));
        if (sigmaBelow < 0.0f || sigmaAbove < 0.0f)
        {
            CaretLogWarning("outlier exclusion sigmas are not intended to be negative");
        }
    }
    float memLimitGB = -1.0f;
    OptionalParameter* memLimitOpt = myParams->getOptionalParameter(4);
    if (memLimitOpt->m_present)
    {
        memLimitGB = float(memLimitOpt->getDouble(1));
        if (memLimitGB < 0.0f)
        {
            throw OperationException("memory limit must be positive");
        }
    }
    const CiftiXML& firstXML = myInstances[0]->getCifti(1)->getCiftiXML();
    vector<int64_t> firstdims = firstXML.getDimensions();
    int64_t totalRows = 1;
    for (size_t i = 1; i < firstdims.size(); ++i)
    {
        totalRows *= firstdims[i];
    }
    int64_t chunkRows = -1;//invalid value
    //checking first cifti for "in memory" should catch both -cifti-read-memory and possible future GUI-based operation
    if (myInstances[0]->getCifti(1)->isInMemory())
    {
        chunkRows = 1;//chunking is for reading (and memory) efficiency, if we have already read all cifti into memory, reading efficiency is moot, so minimize additional memory
    } else {
        if (memLimitGB > 0.0f)
        {
            int64_t chunkMaxBytes = int64_t(memLimitGB * (1<<30));
            int64_t computeBytes = sizeof(double) * 2;//accum and weight accum (because exclude non-numeric)
            if (exclude)
            {//exclude needs to load some rows from all files, but can then compute one output row at a time
                computeBytes = sizeof(float) * myInstances.size();//exclusion needs to measure across *files*, so we need to cache some rows from all files before we can start computing any output
            }//also, weights with exclusion needs to be tracked per element
            for (size_t i = 0; i < firstdims.size(); ++i)
            {
                computeBytes *= firstdims[i];
            }
            if (exclude)
            {//add the "one row at a time" output computation memory for completeness
                computeBytes += sizeof(double) * 2 * firstdims[0];
            }
            int64_t numPasses = (computeBytes - 1) / chunkMaxBytes + 1;
            chunkRows = (totalRows - 1) / numPasses + 1;
        } else {//by default, do enough rows to read at least 10MB (assuming float) from each file before moving to the next
            int64_t rowBytes = sizeof(float) * firstdims[0];
            chunkRows = ((10<<20) - 1) / rowBytes + 1;
        }
    }
    CaretAssert(chunkRows > 0);
    exception_ptr exPtr;
    int64_t exceptedFile = -1;
    //NOTE: throwing inside omp parallel causes an uninformative abort, so catch, skip the rest, and rethrow later
    //our build/processing setup seems to bottleneck on lots of multithreaded memory allocation, so limit to 4 threads for now
    //windows compiler doesn't like unsigned omp loop variables
#pragma omp CARET_PARFOR schedule(dynamic) num_threads(4)
    for (int64_t i = 1; i < int64_t(myInstances.size()); ++i)
    {//don't delete the first one, we have a live reference to it
        if (exceptedFile > -1) continue;//"abort" checking any more files
        try
        {
            CiftiFile* thisCifti = myInstances[i]->getCifti(1);
            if (!firstXML.approximateMatch(thisCifti->getCiftiXML()))//requires at least length to match, often more restrictive
            {
                throw OperationException("cifti file '" + thisCifti->getFileName() + "' does not match the first input");
            }
            //HACK: don't need the info in the XML anymore, so free the memory
            for (size_t i = 0; i < thisCifti->getDimensions().size(); ++i)
            {//since this is a hack, probably not a good idea to add a convenience function to do this loop
                thisCifti->forgetMapping(i);
            }
        } catch (...) {
#pragma omp critical
            {
                if (exceptedFile < 0 || int64_t(i) < exceptedFile)
                {//emulate serial order or processing, because why not
                    exceptedFile = i;
                    exPtr = current_exception();
                }
            }//exit from critical flushes everything
        }
    }
    if (exceptedFile > -1)
    {
        rethrow_exception(exPtr);
    }
    CiftiFile* ciftiOut = myParams->getOutputCifti(1);//need to get it after all the inputs in order for provenance to work with lazy loading
    ciftiOut->setCiftiXML(firstXML);
    if (!exclude)
    {
        vector<float> scratchRow(firstdims[0], 0.0f);
        MultiDimIterator<int64_t> iter = ciftiOut->getIteratorOverRows();
        for (int64_t chunkStart = 0; chunkStart < totalRows; chunkStart += chunkRows)
        {
            vector<MultiDimIterator<int64_t> > rowIndices;
            for (int64_t i = chunkStart; i < chunkStart + chunkRows && i < totalRows; ++i)
            {
                rowIndices.push_back(iter);
                ++iter;
            }
            vector<vector<double> > accum(rowIndices.size(), vector<double>(firstdims[0], 0.0));
            vector<vector<double> > weightAccum = accum;
            for (size_t i = 0; i < myInstances.size(); ++i)
            {
                CiftiFile* thisCifti = myInstances[i]->getCifti(1);
                float thisWeight = 1.0f;
                OptionalParameter* weightOpt = myInstances[i]->getOptionalParameter(1);
                if (weightOpt->m_present)
                {
                    thisWeight = float(weightOpt->getDouble(1));
                }
                for (size_t j = 0; j < rowIndices.size(); ++j)
                {
                    thisCifti->getRow(scratchRow.data(), *(rowIndices[j]));
                    for (int64_t k = 0; k < firstdims[0]; ++k)
                    {
                        if (MathFunctions::isNumeric(scratchRow[k]))
                        {
                            accum[j][k] += thisWeight * scratchRow[k];
                            weightAccum[j][k] += thisWeight;
                        }
                    }
                }
            }
            for (size_t j = 0; j < rowIndices.size(); ++j)
            {
                for (int64_t k = 0; k < firstdims[0]; ++k)
                {
                    if (weightAccum[j][k] != 0.0)
                    {
                        scratchRow[k] = float(accum[j][k] / weightAccum[j][k]);
                    } else {
                        scratchRow[k] = 0.0f;
                    }
                }
                ciftiOut->setRow(scratchRow.data(), *(rowIndices[j]));
            }
        }
    } else {
        MultiDimIterator<int64_t> iter = ciftiOut->getIteratorOverRows();
        for (int64_t chunkStart = 0; chunkStart < totalRows; chunkStart += chunkRows)
        {
            vector<MultiDimIterator<int64_t> > rowIndices;
            for (int64_t i = chunkStart; i < chunkStart + chunkRows && i < totalRows; ++i)
            {
                rowIndices.push_back(iter);
                ++iter;
            }
            //chunk x files x rowlength
            vector<vector<vector<float> > > scratchRows(rowIndices.size(), vector<vector<float> >(myInstances.size(), vector<float>(firstdims[0])));
            //parallel probably won't help here, and will hurt on spinning disks
            for (size_t i = 0; i < myInstances.size(); ++i)
            {
                CiftiFile* thisCifti = myInstances[i]->getCifti(1);
                for (size_t j = 0; j < rowIndices.size(); ++j)
                {
                    thisCifti->getRow(scratchRows[j][i].data(), *(rowIndices[j]));
                }
            }
            for (size_t j = 0; j < rowIndices.size(); ++j)
            {
                vector<double> accum(firstdims[0], 0.0), weightAccum(firstdims[0], 0.0);
                for (int64_t k = 0; k < firstdims[0]; ++k)
                {
                    double thisAccum = 0.0;
                    int64_t numeric = 0;
                    for (size_t i = 0; i < myInstances.size(); ++i)
                    {
                        if (MathFunctions::isNumeric(scratchRows[j][i][k]))
                        {
                            thisAccum += scratchRows[j][i][k];
                            ++numeric;
                        }
                    }
                    float thisMean = float(thisAccum / numeric);
                    thisAccum = 0.0;
                    for (size_t i = 0; i < myInstances.size(); ++i)
                    {
                        if (MathFunctions::isNumeric(scratchRows[j][i][k]))
                        {
                            float tempf = scratchRows[j][i][k] - thisMean;
                            thisAccum += tempf * tempf;
                        }
                    }
                    float thisStdev = float(sqrt(thisAccum / (numeric - 1)));
                    float cutoffLow = thisMean - sigmaBelow * thisStdev;
                    float cutoffHigh = thisMean + sigmaAbove * thisStdev;
                    for (size_t i = 0; i < myInstances.size(); ++i)
                    {
                        float thisVal = scratchRows[j][i][k];
                        float thisWeight = 1.0f;
                        OptionalParameter* weightOpt = myInstances[i]->getOptionalParameter(1);
                        if (weightOpt->m_present)
                        {
                            thisWeight = float(weightOpt->getDouble(1));
                        }
                        if (MathFunctions::isNumeric(thisVal) && (numeric <= 1 || (thisVal > cutoffLow && thisVal < cutoffHigh)))//don't allow too-few numeric to make the exclusion go NaN
                        {
                            accum[k] += thisWeight * thisVal;
                            weightAccum[k] += thisWeight;
                        }
                    }
                }
                vector<float> outRow(firstdims[0]);
                for (int64_t k = 0; k < firstdims[0]; ++k)
                {
                    if (weightAccum[k] != 0.0)
                    {
                        outRow[k] = accum[k] / weightAccum[k];
                    }
                }
                ciftiOut->setRow(outRow.data(), *(rowIndices[j]));
            }
        }
    }
}

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

#include "OperationCiftiConvert.h"
#include "OperationException.h"

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPointer.h"
#include "CiftiFile.h"
#include "CiftiXML.h"
#include "FloatMatrix.h"
#include "GiftiFile.h"
#include "VolumeFile.h"

#include <fstream>
#include <cmath>
#include <string>
#include <limits>
#include <vector>

#include <QFile>
#include <QRegularExpression>
#include <QStringList>

using namespace caret;
using namespace std;

AString OperationCiftiConvert::getCommandSwitch()
{
    return "-cifti-convert";
}

AString OperationCiftiConvert::getShortDescription()
{
    return "DUMP CIFTI MATRIX INTO OTHER FORMATS";
}

OperationParameters* OperationCiftiConvert::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    OptionalParameter* toGiftiExt = ret->createOptionalParameter(1, "-to-gifti-ext", "convert to GIFTI external binary");
    toGiftiExt->addCiftiParameter(1, "cifti-in", "the input cifti file");
    toGiftiExt->addStringParameter(2, "gifti-out", "output - the output gifti file");
    
    OptionalParameter* fromGiftiExt = ret->createOptionalParameter(2, "-from-gifti-ext", "convert a GIFTI made with this command back into a CIFTI");
    fromGiftiExt->addStringParameter(1, "gifti-in", "the input gifti file");
    fromGiftiExt->addCiftiOutputParameter(2, "cifti-out", "the output cifti file");
    OptionalParameter* fgresetTimeOpt = fromGiftiExt->createOptionalParameter(3, "-reset-timepoints", "reset the mapping along rows to timepoints, taking length from the gifti file");
    fgresetTimeOpt->addDoubleParameter(1, "timestep", "the desired time between frames");
    fgresetTimeOpt->addDoubleParameter(2, "timestart", "the desired time offset of the initial frame");
    OptionalParameter* fgresetTimeunitsOpt = fgresetTimeOpt->createOptionalParameter(3, "-unit", "use a unit other than time");
    fgresetTimeunitsOpt->addStringParameter(1, "unit", "unit identifier (default SECOND)");
    fromGiftiExt->createOptionalParameter(4, "-reset-scalars", "reset mapping along rows to scalars, taking length from the gifti file");
    fromGiftiExt->createOptionalParameter(6, "-column-reset-scalars", "reset mapping along columns to scalar (useful for changing number of series in a sdseries file)");
    OptionalParameter* fromGiftiReplace = fromGiftiExt->createOptionalParameter(5, "-replace-binary", "replace data with a binary file");
    fromGiftiReplace->addStringParameter(1, "binary-in", "the binary file that contains replacement data");
    fromGiftiReplace->createOptionalParameter(2, "-flip-endian", "byteswap the binary file");
    fromGiftiReplace->createOptionalParameter(3, "-transpose", "transpose the binary file");
    
    OptionalParameter* toNifti = ret->createOptionalParameter(3, "-to-nifti", "convert to NIFTI1");
    toNifti->addCiftiParameter(1, "cifti-in", "the input cifti file");
    toNifti->addVolumeOutputParameter(2, "nifti-out", "the output nifti file");
    toNifti->createOptionalParameter(3, "-smaller-file", "use better-fitting dimension lengths");
    toNifti->createOptionalParameter(4, "-smaller-dims", "minimize the largest dimension, for tools that don't like large indices");
    
    OptionalParameter* fromNifti = ret->createOptionalParameter(4, "-from-nifti", "convert a NIFTI (1 or 2) file made with this command back into CIFTI");
    fromNifti->addVolumeParameter(1, "nifti-in", "the input nifti file");
    fromNifti->addCiftiParameter(2, "cifti-template", "a cifti file with the dimension(s) and mapping(s) that should be used");
    fromNifti->addCiftiOutputParameter(3, "cifti-out", "the output cifti file");
    OptionalParameter* fnresetTimeOpt = fromNifti->createOptionalParameter(4, "-reset-timepoints", "reset the mapping along rows to timepoints, taking length from the nifti file");
    fnresetTimeOpt->addDoubleParameter(1, "timestep", "the desired time between frames");
    fnresetTimeOpt->addDoubleParameter(2, "timestart", "the desired time offset of the initial frame");
    OptionalParameter* fnresetTimeunitsOpt = fnresetTimeOpt->createOptionalParameter(3, "-unit", "use a unit other than time");
    fnresetTimeunitsOpt->addStringParameter(1, "unit", "unit identifier (default SECOND)");
    fromNifti->createOptionalParameter(5, "-reset-scalars", "reset mapping along rows to scalars, taking length from the nifti file");
    
    OptionalParameter* toText = ret->createOptionalParameter(5, "-to-text", "convert to a plain text file");
    toText->addCiftiParameter(1, "cifti-in", "the input cifti file");
    toText->addStringParameter(2, "text-out", "output - the output text file");
    OptionalParameter* toTextColDelimOpt = toText->createOptionalParameter(3, "-col-delim", "choose string to put between elements in a row");
    toTextColDelimOpt->addStringParameter(1, "delim-string", "the string to use (default is a tab character)");
    
    OptionalParameter* fromText = ret->createOptionalParameter(6, "-from-text", "convert from plain text to cifti");
    fromText->addStringParameter(1, "text-in", "the input text file");
    fromText->addCiftiParameter(2, "cifti-template", "a cifti file with the dimension(s) and mapping(s) that should be used");
    fromText->addCiftiOutputParameter(3, "cifti-out", "the output cifti file");
    OptionalParameter* fromTextColDelimOpt = fromText->createOptionalParameter(4, "-col-delim", "specify string that is between elements in a row");
    fromTextColDelimOpt->addStringParameter(1, "delim-string", "the string to use (default is any whitespace)");
    OptionalParameter* ftresetTimeOpt = fromText->createOptionalParameter(5, "-reset-timepoints", "reset the mapping along rows to timepoints, taking length from the text file");
    ftresetTimeOpt->addDoubleParameter(1, "timestep", "the desired time between frames");
    ftresetTimeOpt->addDoubleParameter(2, "timestart", "the desired time offset of the initial frame");
    OptionalParameter* ftresetTimeunitsOpt = ftresetTimeOpt->createOptionalParameter(3, "-unit", "use a unit other than time");
    ftresetTimeunitsOpt->addStringParameter(1, "unit", "unit identifier (default SECOND)");
    fromText->createOptionalParameter(6, "-reset-scalars", "reset mapping along rows to scalars, taking length from the text file");
    
    AString myText = AString("This command is used to convert a full CIFTI matrix to/from formats that can be used by programs that don't understand CIFTI.  ") +
        "Its output does not have a valid spatial shape, it merely uses the other formats to encode the packed cifti matrix as-is.  " +
        "You must specify exactly one of -to-gifti-ext, -from-gifti-ext, -to-nifti, -from-nifti, -to-text, or -from-text.\n\n" +
        "This command cannot map surface-based parts of the cifti file to a spatially-correct volume file, or map volume-based data to the surface, see -volume-to-surface-mapping and -metric-to-volume-mapping instead (other commands such as -cifti-separate are also required).\n\n" +
        "If you want to write an existing CIFTI file with a different CIFTI version, see -file-convert, and its -cifti-version-convert option.\n\n" +
        "If you want part of the CIFTI file as a standard metric, label, or volume file, see -cifti-separate.  " +
        "If you want to create a CIFTI file from metric and/or volume files, see the -cifti-create-* commands.\n\n" +
        "If you want to import a matrix in non-CIFTI format that is restricted to an ROI, first obtain a template CIFTI file matching that ROI (you can use -cifti-create-dense-scalar if you don't have such a file).  " +
        "Use -cifti-convert to import it to CIFTI format, and you can then expand the file into a standard brainordinates space with -cifti-create-dense-from-template.  " +
        "If you want to export only part of a CIFTI file, first create an roi-restricted CIFTI file with -cifti-restrict-dense-mapping.\n\n" +
        "The -transpose option to -from-gifti-ext is needed if the replacement binary file is in column-major order.\n\n" +
        "The -unit options accept these values:\n";
    vector<CiftiSeriesMap::Unit> units = CiftiSeriesMap::getAllUnits();
    for (int i = 0; i < (int)units.size(); ++i)
    {
        myText += "\n" + CiftiSeriesMap::unitToString(units[i]);
    }
    ret->setHelpText(myText);
    return ret;
}

namespace
{
    
    bool haveWarned = false;
    
    float toFloat(const AString& input)
    {
        bool ok = false;
        double converted = input.toDouble(&ok);
        if (!ok) throw OperationException("failed to convert text to number: '" + input + "'");
        float ret = float(converted);//this will turn some non-inf values into +/- inf, so let's fix that
        if (converted != 0.0 && !std::isinf(converted) && (abs(converted) > numeric_limits<float>::max() || abs(converted) < numeric_limits<float>::denorm_min()))
        {
            if (!haveWarned)
            {
                CaretLogWarning("input number(s) changed to fit range of float32, first instance: '" + input + "'");
                haveWarned = true;
            }
            if (std::isinf(ret))
            {
                if (ret > 0.0f)
                {
                    ret = numeric_limits<float>::max();
                } else {
                    ret = -numeric_limits<float>::max();
                }
            }
        }
        return ret;
    }
}

void OperationCiftiConvert::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    int modes = 0;
    OptionalParameter* toGiftiExt = myParams->getOptionalParameter(1);
    OptionalParameter* fromGiftiExt = myParams->getOptionalParameter(2);
    OptionalParameter* toNifti = myParams->getOptionalParameter(3);
    OptionalParameter* fromNifti = myParams->getOptionalParameter(4);
    OptionalParameter* toText = myParams->getOptionalParameter(5);
    OptionalParameter* fromText = myParams->getOptionalParameter(6);
    if (toGiftiExt->m_present) ++modes;
    if (fromGiftiExt->m_present) ++modes;
    if (toNifti->m_present) ++modes;
    if (fromNifti->m_present) ++modes;
    if (toText->m_present) ++modes;
    if (fromText->m_present) ++modes;
    if (modes != 1)
    {
        throw OperationException("you must specify exactly one conversion mode");
    }
    if (toGiftiExt->m_present)
    {
        CiftiFile* myInFile = toGiftiExt->getCifti(1);
        AString myGiftiName = toGiftiExt->getString(2);
        vector<int64_t> myDims;
        myDims.push_back(myInFile->getNumberOfRows());
        myDims.push_back(myInFile->getNumberOfColumns());
        const CiftiXML& myXML = myInFile->getCiftiXML();//soft of hack - metric files use "normal" when they really mean none, using the same thing as metric files means it should just work
        if (myXML.getNumberOfDimensions() != 2) throw OperationException("conversion only supported for 2D cifti");
        GiftiDataArray* myArray = new GiftiDataArray(NiftiIntentEnum::NIFTI_INTENT_NORMAL, NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32, myDims, GiftiEncodingEnum::EXTERNAL_FILE_BINARY);
        float* myOutData = myArray->getDataPointerFloat();
        for (int i = 0; i < myInFile->getNumberOfRows(); ++i)
        {
            myInFile->getRow(myOutData + i * myInFile->getNumberOfColumns(), i);
        }
        AString myCiftiXML = myXML.writeXMLToString();
        myArray->getMetaData()->set("CiftiXML", myCiftiXML);
        GiftiFile myOutFile;
        myOutFile.setEncodingForWriting(GiftiEncodingEnum::EXTERNAL_FILE_BINARY);
        myOutFile.addDataArray(myArray);
        myOutFile.writeFile(myGiftiName);
    }
    if (fromGiftiExt->m_present)
    {
        AString myGiftiName = fromGiftiExt->getString(1);
        GiftiFile myInFile;
        myInFile.readFile(myGiftiName);
        if (myInFile.getNumberOfDataArrays() != 1)
        {
            throw OperationException("gifti file was not created by -cifti-convert, please use a -cifti-create-* command");
        }
        GiftiDataArray* dataArrayRef = myInFile.getDataArray(0);
        if (!dataArrayRef->getMetaData()->exists("CiftiXML")) throw OperationException("gifti file was not created by -cifti-convert, please use a -cifti-create-* command");
        if (dataArrayRef->getDataType() != NiftiDataTypeEnum::NIFTI_TYPE_FLOAT32)//this may not be needed
        {
            throw OperationException("input gifti has the wrong data type");
        }
        CiftiFile* myOutFile = fromGiftiExt->getOutputCifti(2);
        CiftiXML myXML;
        myXML.readXML(dataArrayRef->getMetaData()->get("CiftiXML"));
        if (myXML.getNumberOfDimensions() != 2) throw OperationException("conversion only supported for 2D cifti");
        int64_t numCols = dataArrayRef->getNumberOfComponents();
        int64_t numRows = dataArrayRef->getNumberOfRows();
        OptionalParameter* fgresetTimeOpt = fromGiftiExt->getOptionalParameter(3);
        if (fgresetTimeOpt->m_present)
        {//-reset-timepoints
            CiftiSeriesMap::Unit myUnit = CiftiSeriesMap::SECOND;
            OptionalParameter* fgresetTimeunitsOpt = fgresetTimeOpt->getOptionalParameter(3);
            if (fgresetTimeunitsOpt->m_present)
            {
                bool ok = false;
                myUnit = CiftiSeriesMap::stringToUnit(fgresetTimeunitsOpt->getString(1), ok);
                if (!ok) throw OperationException("unrecognized unit name: '" + fgresetTimeunitsOpt->getString(1) + "'");
            }
            myXML.setMap(CiftiXML::ALONG_ROW, CiftiSeriesMap(numCols, fgresetTimeOpt->getDouble(2), fgresetTimeOpt->getDouble(1), myUnit));
        } else {
            if (myXML.getMappingType(CiftiXML::ALONG_ROW) == CiftiMappingType::SERIES)
            {
                myXML.getSeriesMap(CiftiXML::ALONG_ROW).setLength(numCols);
            }
        }
        if (myXML.getMappingType(CiftiXML::ALONG_COLUMN) == CiftiMappingType::SERIES)
        {
            myXML.getSeriesMap(CiftiXML::ALONG_COLUMN).setLength(numRows);
        }
        if (fromGiftiExt->getOptionalParameter(4)->m_present)
        {//-reset-scalars
            if (fgresetTimeOpt->m_present) throw OperationException("only one of -reset-timepoints and -reset-scalars may be specified");
            CiftiScalarsMap newMap;
            newMap.setLength(numCols);
            myXML.setMap(CiftiXML::ALONG_ROW, newMap);
        }
        if (fromGiftiExt->getOptionalParameter(6)->m_present)
        {//-column-reset-scalars
            CiftiScalarsMap newMap;
            newMap.setLength(numRows);
            myXML.setMap(CiftiXML::ALONG_COLUMN, newMap);
        }
        if (myXML.getDimensionLength(CiftiXML::ALONG_ROW) != numCols || myXML.getDimensionLength(CiftiXML::ALONG_COLUMN) != numRows)
        {
            throw OperationException("dimensions of input gifti array (" + AString::number(numRows) + " rows, " + AString::number(numCols) + " columns)" +
                                     " do not match dimensions of the Cifti XML (" + AString::number(myXML.getDimensionLength(CiftiXML::ALONG_COLUMN)) + " rows, " +
                                         AString::number(myXML.getDimensionLength(CiftiXML::ALONG_ROW)) + " columns)");
        }
        myOutFile->setCiftiXML(myXML);
        OptionalParameter* fromGiftiReplace = fromGiftiExt->getOptionalParameter(5);
        if (fromGiftiReplace->m_present)
        {
            AString replaceFileName = fromGiftiReplace->getString(1);
            QFile replaceFile(replaceFileName);
            if (replaceFile.size() != (int64_t)(sizeof(float) * numCols * numRows))
            {
                throw OperationException("replacement file is the wrong size, size is " + AString::number(replaceFile.size()) + ", needed " + AString::number(sizeof(float) * numCols * numRows));
            }
            if (!replaceFile.open(QIODevice::ReadOnly))
            {
                throw OperationException("unable to open replacement file for reading");
            }
            OptionalParameter* swapBytes = fromGiftiReplace->getOptionalParameter(2);
            OptionalParameter* transpose = fromGiftiReplace->getOptionalParameter(3);
            int64_t readSize = numCols, numReads = numRows;
            if (transpose->m_present)
            {
                readSize = numRows;
                numReads = numCols;
            }
            CaretArray<float> myScratch(readSize);
            for (int i = 0; i < numReads; ++i)
            {
                if (replaceFile.read((char*)(myScratch.getArray()), sizeof(float) * readSize) != (int64_t)(sizeof(float) * readSize))
                {
                    throw OperationException("short read from replacement file, aborting");
                }
                float tempVal;
                char* tempValPointer = (char*)&tempVal;//copy method isn't as fast, but it is clean
                for (int j = 0; j < readSize; ++j)
                {
                    if (swapBytes->m_present)
                    {
                        char* elemPointer = (char*)(myScratch.getArray() + j);
                        for (int k = 0; k < (int)sizeof(float); ++k)
                        {
                            tempValPointer[k] = elemPointer[sizeof(float) - 1 - k];
                        }
                    } else {
                        tempVal = myScratch[j];
                    }
                    if (transpose->m_present)
                    {
                        int32_t indices[] = {j, i};
                        dataArrayRef->setDataFloat32(indices, tempVal);
                    } else {
                        int32_t indices[] = {i, j};
                        dataArrayRef->setDataFloat32(indices, tempVal);
                    }
                }
            }
        }
        vector<float> scratchRow(numCols);
        for (int i = 0; i < numRows; ++i)
        {
            for (int j = 0; j < numCols; ++j)
            {
                int32_t indices[] = {i, j};
                scratchRow[j] = dataArrayRef->getDataFloat32(indices);
            }
            myOutFile->setRow(scratchRow.data(), i);
        }
    }
    if (toNifti->m_present)
    {
        CiftiFile* myCiftiIn = toNifti->getCifti(1);
        if (myCiftiIn->getCiftiXML().getNumberOfDimensions() != 2) throw OperationException("conversion only supported for 2D cifti");
        VolumeFile* myNiftiOut = toNifti->getOutputVolume(2);
        bool betterDims = toNifti->getOptionalParameter(3)->m_present;
        bool smallerDims = toNifti->getOptionalParameter(4)->m_present;
        if (betterDims && smallerDims) throw OperationException("-smaller-file and -smaller-dims may not be specified together");
        vector<int64_t> outDims(4, 1);
        outDims[3] = myCiftiIn->getNumberOfColumns();
        const int64_t SHORTMAX = numeric_limits<short>::max();//less ugly than writing it out every time
        if (outDims[3] > SHORTMAX) throw OperationException("cifti rows are too long for nifti-1, failing");
        int64_t numRows = myCiftiIn->getNumberOfRows();
        if (numRows > SHORTMAX * SHORTMAX && !(betterDims || smallerDims))
        {//don't let the default math hit the pathological case of an extra billion elements, behavior change okay because it is likely nobody has hit it yet
            CaretLogWarning("input has a very large number of rows, and neither -smaller-file or -smaller-dims were specified, using -smaller-file instead");
            betterDims = true;
        }
        if (smallerDims)
        {
            outDims[0] = int64_t(ceil(pow(numRows, 1.0f / 3.0f))) - 1;//deliberately start 1 below what floating point says for a cube
            while (outDims[0] * outDims[0] * outDims[0] < numRows) ++outDims[0];//use integer math to get the exact answer
            if (outDims[0] > SHORTMAX) throw OperationException("too many cifti rows for nifti-1 spatial dimensions, failing");
            outDims[1] = outDims[0];
            outDims[2] = outDims[0];
            if (outDims[0] * outDims[0] * (outDims[0] - 1) >= numRows)//see whether we can subtract one from a different dimension
            {
                outDims[2] = outDims[0] - 1;
                if (outDims[0] * (outDims[0] - 1) * outDims[2] >= numRows) outDims[1] = outDims[0] - 1;
            }
        } else {
            if (betterDims)
            {//find the minimum needed third dimension, then compute from that how many elements the first two dimensions must encompass, etc
                outDims[2] = (numRows - 1) / (SHORTMAX * SHORTMAX) + 1;//round up
                if (outDims[2] > SHORTMAX) throw OperationException("too many cifti rows for nifti-1 spatial dimensions, failing");
                int64_t temp = (numRows - 1) / outDims[2] + 1;//and again...
                outDims[1] = (temp - 1) / SHORTMAX + 1;
                outDims[0] = (temp - 1) / outDims[1] + 1;
            } else {
                int64_t temp = numRows;
                int index = 0;
                while (temp > SHORTMAX)
                {
                    if (index > 1) throw OperationException("too many cifti rows for nifti-1 spatial dimensions, failing");
                    outDims[index] = SHORTMAX;
                    temp = (temp - 1) / SHORTMAX + 1;//round up
                    ++index;
                }
                outDims[index] = temp;
            }
        }
        CaretAssert(outDims[0] * outDims[1] * outDims[2] >= numRows);//make sure we didn't screw up the math
        myNiftiOut->reinitialize(outDims, FloatMatrix::identity(4).getMatrix());
        myNiftiOut->setValueAllVoxels(0.0f);
        int64_t ijk[3] = { 0, 0, 0 };
        vector<float> rowscratch(outDims[3]);
        for (int64_t i = 0; i < numRows; ++i)
        {
            myCiftiIn->getRow(rowscratch.data(), i);
            for (int64_t j = 0; j < outDims[3]; ++j)
            {
                myNiftiOut->setValue(rowscratch[j], ijk, j);
            }
            ++ijk[0];
            if (ijk[0] >= outDims[0])
            {
                ijk[0] = 0;
                ++ijk[1];
                if (ijk[1] >= outDims[1])
                {
                    ijk[1] = 0;
                    ++ijk[2];
                    CaretAssert(i == numRows - 1 || ijk[2] < outDims[2]);//in case it divided exactly
                }
            }
        }
    }
    if (fromNifti->m_present)
    {
        VolumeFile* myNiftiIn = fromNifti->getVolume(1);
        CiftiFile* myTemplate = fromNifti->getCifti(2);
        CiftiFile* myCiftiOut = fromNifti->getOutputCifti(3);
        vector<int64_t> myDims;
        myNiftiIn->getDimensions(myDims);
        if (myDims[4] != 1) throw OperationException("input nifti has multiple components, aborting");
        CiftiXML outXML = myTemplate->getCiftiXML();
        if (outXML.getNumberOfDimensions() != 2) throw OperationException("conversion only supported for 2D cifti");
        OptionalParameter* fnresetTimeOpt = fromNifti->getOptionalParameter(4);
        if (fnresetTimeOpt->m_present)
        {//-reset-timepoints
            CiftiSeriesMap::Unit myUnit = CiftiSeriesMap::SECOND;
            OptionalParameter* fnresetTimeunitsOpt = fnresetTimeOpt->getOptionalParameter(3);
            if (fnresetTimeunitsOpt->m_present)
            {
                bool ok = false;
                myUnit = CiftiSeriesMap::stringToUnit(fnresetTimeunitsOpt->getString(1), ok);
                if (!ok) throw OperationException("unrecognized unit name: '" + fnresetTimeunitsOpt->getString(1) + "'");
            }
            outXML.setMap(CiftiXML::ALONG_ROW, CiftiSeriesMap(myDims[3], fnresetTimeOpt->getDouble(2), fnresetTimeOpt->getDouble(1), myUnit));
        }
        if (fromNifti->getOptionalParameter(5)->m_present)
        {//-reset-scalars
            if (fnresetTimeOpt->m_present) throw OperationException("only one of -reset-timepoints and -reset-scalars may be specified");
            CiftiScalarsMap newMap;
            newMap.setLength(myDims[3]);
            outXML.setMap(CiftiXML::ALONG_ROW, newMap);
        }
        int64_t numRows = outXML.getDimensionLength(CiftiXML::ALONG_COLUMN), numCols = outXML.getDimensionLength(CiftiXML::ALONG_ROW);
        if (myDims[3] != numCols)
        {
            throw OperationException("input nifti has the different size than cifti template for row length (nifti says " + AString::number(myDims[3]) +
                                     ", cifti XML says " + AString::number(numCols) + ")");
        }
        if (numRows > myDims[0] * myDims[1] * myDims[2])
        {
            throw OperationException("input nifti is too small for column length (need at least " + AString::number(numRows) +
                                     ", product of first three nifti dimensions is " + AString::number(myDims[0] * myDims[1] * myDims[2]) + ")");
        }
        if (2 * numRows < myDims[0] * myDims[1] * myDims[2])
        {
            CaretLogWarning(AString("Input nifti has more than twice as many 'spatial' voxels as the\n") +
                            "template cifti column length, this is probably not a valid 'fake-nifti' file.\n\nnumber of cifti column elements: " + AString::number(numRows) + "\nproduct of first three nifti dimensions: " + AString::number(myDims[0] * myDims[1] * myDims[2]) + "\n\n" +
                            "'-cifti-convert -from-nifti' is only intended to be used after '-cifti-convert\n" +
                            "-to-nifti' (and spatially-naive processing of the produced 'fake-nifti' file).");
        }
        myCiftiOut->setCiftiXML(outXML);
        vector<float> rowscratch(numCols);
        for (int64_t i = 0; i < numRows; ++i)
        {
            for (int64_t j = 0; j < numCols; ++j)
            {
                rowscratch[j] = myNiftiIn->getFrame(j)[i];
            }
            myCiftiOut->setRow(rowscratch.data(), i);
        }
    }
    if (toText->m_present)
    {
        CiftiFile* ciftiIn = toText->getCifti(1);
        AString textOutName = toText->getString(2);
        AString delim = "\t";
        OptionalParameter* colDelimOpt = toText->getOptionalParameter(3);
        if (colDelimOpt->m_present)
        {
            delim = colDelimOpt->getString(1);
            if (delim.isEmpty()) throw OperationException("delimiter string must not be empty");//catch some possible stupid mistakes
        }
        const CiftiXML& myXML = ciftiIn->getCiftiXML();
        if (myXML.getNumberOfDimensions() != 2) throw OperationException("conversion only supported for 2D cifti");
        if (myXML.getDimensionLength(0) < 1) throw OperationException("input cifti has zero-length rows");
        vector<int64_t> dims = myXML.getDimensions();
        vector<float> scratchRow(dims[0]);
        fstream textOut(textOutName.toLocal8Bit().constData(), fstream::out | fstream::trunc | fstream::binary);//write the same file, no newline translation, regardless of OS
        for (int64_t i = 0; i < dims[1]; ++i)
        {
            ciftiIn->getRow(scratchRow.data(), i);
            textOut << AString::number(scratchRow[0]);
            for (int64_t j = 1; j < dims[0]; ++j)
            {
                textOut << delim << AString::number(scratchRow[j]);
            }
            textOut << "\n";
        }
    }
    if (fromText->m_present)
    {
        AString textInName = fromText->getString(1);
        CiftiFile* ciftiTemplate = fromText->getCifti(2);
        CiftiFile* ciftiOut = fromText->getOutputCifti(3);
        AString delim = "";//empty is special value for "any whitespace"
        OptionalParameter* colDelimOpt = fromText->getOptionalParameter(4);
        if (colDelimOpt->m_present)
        {
            delim = colDelimOpt->getString(1);
            if (delim.isEmpty()) throw OperationException("delimiter string must not be empty");
        }
        CiftiXML outXML = ciftiTemplate->getCiftiXML();
        if (outXML.getNumberOfDimensions() != 2) throw OperationException("conversion only supported for 2D cifti");
        int64_t numRows = outXML.getDimensionLength(CiftiXML::ALONG_COLUMN);
        fstream textIn(textInName.toLocal8Bit().constData(), fstream::in);//do newline translation on input
        string templine;//need to extract a row from the file first to set the row length
        if (!getline(textIn, templine)) throw OperationException("failed to read from input text file");
        QStringList entries;
        if (delim.isEmpty())
        {
#if QT_VERSION >= 0x060000
            entries = QString(templine.c_str()).split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
#else
            entries = QString(templine.c_str()).split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
#endif
        } else {
#if QT_VERSION >= 0x060000
            entries = QString(templine.c_str()).split(delim, Qt::SkipEmptyParts);
#else
            entries = QString(templine.c_str()).split(delim, QString::SkipEmptyParts);
#endif
        }
        if (numRows < 1) throw OperationException("template cifti file has no data");//this probably throws an exception in CiftiFile, but double check
        int textRowLength = entries.size();
        OptionalParameter* ftresetTimeOpt = fromText->getOptionalParameter(5);
        if (ftresetTimeOpt->m_present)
        {
            CiftiSeriesMap::Unit myUnit = CiftiSeriesMap::SECOND;
            OptionalParameter* ftresetTimeunitsOpt = ftresetTimeOpt->getOptionalParameter(3);
            if (ftresetTimeunitsOpt->m_present)
            {
                bool ok = false;
                myUnit = CiftiSeriesMap::stringToUnit(ftresetTimeunitsOpt->getString(1), ok);
                if (!ok) throw OperationException("unrecognized unit name: '" + ftresetTimeunitsOpt->getString(1) + "'");
            }
            outXML.setMap(CiftiXML::ALONG_ROW, CiftiSeriesMap(textRowLength, ftresetTimeOpt->getDouble(2), ftresetTimeOpt->getDouble(1), myUnit));
        }
        if (fromText->getOptionalParameter(6)->m_present)
        {
            if (ftresetTimeOpt->m_present) throw OperationException("only one of -reset-timepoints and -reset-scalars may be specified");
            CiftiScalarsMap newMap;
            newMap.setLength(textRowLength);
            outXML.setMap(CiftiXML::ALONG_ROW, newMap);
        }
        if (textRowLength != outXML.getDimensionLength(CiftiXML::ALONG_ROW))
        {
            throw OperationException("text file has different row length than cifti template (text file says " + AString::number(textRowLength) +
                                     ", cifti XML says " + AString::number(outXML.getDimensionLength(CiftiXML::ALONG_ROW)) + ")");
        }
        ciftiOut->setCiftiXML(outXML);
        vector<float> temprow(textRowLength);
        for (int i = 0; i < textRowLength; ++i)
        {
            temprow[i] = toFloat(entries[i]);
        }
        ciftiOut->setRow(temprow.data(), 0);
        for (int64_t j = 1; j < numRows; ++j)
        {
            if (!getline(textIn, templine)) throw OperationException("failed to read from input text file (not enough rows)");
            if (delim.isEmpty())
            {
#if QT_VERSION >= 0x060000
                entries = QString(templine.c_str()).split(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
#else
                entries = QString(templine.c_str()).split(QRegularExpression("\\s+"), QString::SkipEmptyParts);
#endif
            } else {
#if QT_VERSION >= 0x060000
                entries = QString(templine.c_str()).split(delim, Qt::SkipEmptyParts);
#else
                entries = QString(templine.c_str()).split(delim, QString::SkipEmptyParts);
#endif
            }
            if (entries.size() != textRowLength) throw OperationException("text file has inconsistent line length");
            for (int i = 0; i < textRowLength; ++i)
            {
                temprow[i] = toFloat(entries[i]);
            }
            ciftiOut->setRow(temprow.data(), j);
        }
    }
}

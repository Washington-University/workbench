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

#include "OperationCiftiCreateScalarSeries.h"
#include "OperationException.h"

#include "CaretLogger.h"
#include "CiftiFile.h"
#include "FloatMatrix.h"

#include <QStringList>

#include <fstream>
#include <string>
#include <vector>

using namespace caret;
using namespace std;

AString OperationCiftiCreateScalarSeries::getCommandSwitch()
{
    return "-cifti-create-scalar-series";
}

AString OperationCiftiCreateScalarSeries::getShortDescription()
{
    return "IMPORT SERIES DATA INTO CIFTI";
}

OperationParameters* OperationCiftiCreateScalarSeries::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "input", "input file");
    
    ret->addCiftiOutputParameter(2, "cifti-out", "output cifti file");
    
    ret->createOptionalParameter(3, "-transpose", "use if the rows of the text file are along the scalar dimension");
    
    OptionalParameter* nameFileOpt = ret->createOptionalParameter(4, "-name-file", "use a text file to set names on scalar dimension");
    nameFileOpt->addStringParameter(1, "file", "text file containing names, one per line");
    
    OptionalParameter* seriesOpt = ret->createOptionalParameter(5, "-series", "set the units and values of the series");
    seriesOpt->addStringParameter(1, "unit", "the unit to use");
    seriesOpt->addDoubleParameter(2, "start", "the value at the first series point");
    seriesOpt->addDoubleParameter(3, "step", "the interval between series points");
    
    AString myHelp = AString("Convert a text file containing series of equal length into a cifti file.  ") +
        "The text file should have lines made up of numbers separated by whitespace, with no extra newlines between lines.\n\n" +
        "The <unit> argument must be one of the following:\n";
    vector<CiftiSeriesMap::Unit> units = CiftiSeriesMap::getAllUnits();
    for (int i = 0; i < (int)units.size(); ++i)
    {
        myHelp += "\n" + CiftiSeriesMap::unitToString(units[i]);
    }
    ret->setHelpText(myHelp);
    return ret;
}

void OperationCiftiCreateScalarSeries::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString inFileName = myParams->getString(1);
    CiftiFile* outFile = myParams->getOutputCifti(2);
    bool transpose = myParams->getOptionalParameter(3)->m_present;
    ifstream nameFile;
    OptionalParameter* nameFileOpt = myParams->getOptionalParameter(4);
    if (nameFileOpt->m_present)
    {
        nameFile.open(nameFileOpt->getString(1).toLocal8Bit().constData());
        if (!nameFile) throw OperationException("failed to open name file");
    }
    ifstream inputFile(inFileName.toLocal8Bit().constData());
    vector<vector<float> > inFileData;
    if (!inputFile.good()) throw OperationException("failed to open input file '" + inFileName + "'");
    string inputLine;
    while (inputFile)
    {
        getline(inputFile, inputLine);
        QStringList tokens = QString(inputLine.c_str()).split(QRegExp("\\s+"), Qt::SkipEmptyParts);
        if (tokens.empty()) break;//in case there are extra newlines on the end
        if (!inFileData.empty() && (int)inFileData.back().size() != tokens.size())
            throw OperationException("input file is not a rectangular matrix, starting at line " + AString::number(inFileData.size() + 2));//1 for 0-indexing, 1 for line not added to matrix yet
        inFileData.push_back(vector<float>());
        for (int i = 0; i < tokens.size(); ++i)
        {
            bool ok = false;
            inFileData.back().push_back(tokens[i].toFloat(&ok));
            if (!ok) throw OperationException("input file contains non-number '" + tokens[i] + "'");
        }
    }
    if (inFileData.empty() || inFileData[0].empty()) throw OperationException("input file contains no data");
    if (transpose)
    {
        inFileData = FloatMatrix(inFileData).transpose().getMatrix();
    }
    CiftiScalarsMap colMap;
    colMap.setLength(inFileData.size());
    if (nameFileOpt->m_present)
    {
        for (int i = 0; i < (int)inFileData.size(); ++i)
        {
            getline(nameFile, inputLine);
            if (!nameFile)
            {
                CaretLogWarning("name file contained " + AString::number(i) + " names, expected " + AString::number(inFileData.size()));
                break;
            }
            colMap.setMapName(i, inputLine.c_str());
        }
    }
    CiftiSeriesMap rowMap;
    rowMap.setLength(inFileData[0].size());
    OptionalParameter* seriesOpt = myParams->getOptionalParameter(5);
    if (seriesOpt->m_present)
    {
        AString unitName = seriesOpt->getString(1);
        bool ok = false;
        CiftiSeriesMap::Unit myUnit = CiftiSeriesMap::stringToUnit(unitName, ok);
        if (!ok)
        {
            throw OperationException("unrecognized unit name: '" + unitName + "'");
        }
        rowMap.setUnit(myUnit);
        rowMap.setStart(seriesOpt->getDouble(2));
        rowMap.setStep(seriesOpt->getDouble(3));
    }
    CiftiXML outXML;
    outXML.setNumberOfDimensions(2);
    outXML.setMap(CiftiXML::ALONG_ROW, rowMap);
    outXML.setMap(CiftiXML::ALONG_COLUMN, colMap);
    outFile->setCiftiXML(outXML);
    for (int i = 0; i < (int)inFileData.size(); ++i)
    {
        outFile->setRow(inFileData[i].data(), i);
    }
}

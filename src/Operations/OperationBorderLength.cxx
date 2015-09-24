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

#include "OperationBorderLength.h"
#include "OperationException.h"

#include "BorderFile.h"
#include "BorderLengthHelper.h"
#include "MetricFile.h"
#include "SurfaceFile.h"
#include "Border.h"

#include <iostream>

using namespace caret;
using namespace std;

AString OperationBorderLength::getCommandSwitch()
{
    return "-border-length";
}

AString OperationBorderLength::getShortDescription()
{
    return "REPORT LENGTH OF BORDERS";
}

OperationParameters* OperationBorderLength::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    
    ret->addBorderParameter(1, "border", "the input border file");
    
    ret->addSurfaceParameter(2, "surface", "the surface to measure the borders on");
    
    OptionalParameter* corrAreasOpt = ret->createOptionalParameter(3, "-corrected-areas", "vertex areas to use instead of computing them from the surface");
    corrAreasOpt->addMetricParameter(1, "area-metric", "the corrected vertex areas, as a metric");
    
    ret->createOptionalParameter(4, "-separate-pieces", "report lengths for multi-part borders as separate numbers");
    
    ret->createOptionalParameter(5, "-hide-border-name", "don't print border name before each output");

    ret->setHelpText(
        AString("For each border, print its length along the surface, in mm.  ") +
        "If a border has multiple parts, their lengths are summed before printing, unless -separate-pieces is specified."
    );
    return ret;
}

void OperationBorderLength::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    BorderFile* myBorderFile = myParams->getBorder(1);
    SurfaceFile* mySurface = myParams->getSurface(2);
    MetricFile* corrAreas = NULL;
    const float* corrAreaData = NULL;
    OptionalParameter* corrAreasOpt = myParams->getOptionalParameter(3);
    if (corrAreasOpt->m_present)
    {
        corrAreas = corrAreasOpt->getMetric(1);
        corrAreaData = corrAreas->getValuePointerForColumn(0);
    }
    bool separate = myParams->getOptionalParameter(4)->m_present;
    bool hideNames = myParams->getOptionalParameter(5)->m_present;
    checkStructureMatch(myBorderFile, mySurface->getStructure(), "border file", "surface file has");
    checkStructureMatch(corrAreas, mySurface->getStructure(), "corrected areas metric", "surface file has");
    BorderMultiPartHelper myMultiHelper(myBorderFile);
    BorderLengthHelper myLengthHelper(mySurface, corrAreaData);
    int numBorders = (int)myMultiHelper.borderPieceList.size();
    for (int i = 0; i < numBorders; ++i)
    {
        if (!hideNames)
        {
            cout << myBorderFile->getBorder(myMultiHelper.borderPieceList[i][0])->getName() << ": ";
        }
        int numParts = (int)myMultiHelper.borderPieceList[i].size();
        if (separate)
        {
            for (int j = 0; j < numParts; ++j)
            {
                if (j > 0)
                {
                    cout << " ";
                }
                cout << myLengthHelper.length(myBorderFile->getBorder(myMultiHelper.borderPieceList[i][j]));
            }
        } else {
            float total = 0.0f;
            for (int j = 0; j < numParts; ++j)
            {
                total += myLengthHelper.length(myBorderFile->getBorder(myMultiHelper.borderPieceList[i][j]));
            }
            cout << total;
        }
        cout << endl;
    }
}

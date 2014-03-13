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

#include "OperationCiftiChangeTimestep.h"
#include "OperationException.h"
#include "CiftiFile.h"
#include "CaretLogger.h"

using namespace caret;
using namespace std;

AString OperationCiftiChangeTimestep::getCommandSwitch()
{
    return "-cifti-change-timestep";
}

AString OperationCiftiChangeTimestep::getShortDescription()
{
    return "CHANGE THE TIMESTEP OF A CIFTI FILE";
}

OperationParameters* OperationCiftiChangeTimestep::getParameters()
{
    OperationParameters* ret = new OperationParameters();
    ret->addStringParameter(1, "cifti", "the cifti file to modify");
    OptionalParameter* rowTimestep = ret->createOptionalParameter(2, "-row-timestep", "set the timestep along rows");
    rowTimestep->addDoubleParameter(1, "seconds", "seconds per timestep");
    OptionalParameter* columnTimestep = ret->createOptionalParameter(3, "-column-timestep", "set the timestep along columns");
    columnTimestep->addDoubleParameter(1, "seconds", "seconds per timestep");
    ret->setHelpText(
        AString("Warns if a dimension specified is not timepoints, otherwise modifies the timestep, and finally writes the result to ") +
        "the same filename if any dimensions were modified.\nNOTE: you probably want -row-timestep, as that matches the .dtseries.nii specification.  " +
        "The other option is available just for completeness."
    );
    return ret;
}

void OperationCiftiChangeTimestep::useParameters(OperationParameters* myParams, ProgressObject* myProgObj)
{
    LevelProgress myProgress(myProgObj);
    AString ciftiName = myParams->getString(1);
    OptionalParameter* rowTimestep = myParams->getOptionalParameter(2);
    OptionalParameter* columnTimestep = myParams->getOptionalParameter(3);
    if (!columnTimestep->m_present && !rowTimestep->m_present)
    {
        return;
    }
    CiftiFile myCifti;
    myCifti.openFile(ciftiName);
    bool modified = false;
    if (rowTimestep->m_present)
    {
        float step = (float)rowTimestep->getDouble(1);
        if (myCifti.setRowTimestep(step))
        {
            modified = true;
        } else {
            CaretLogWarning("could not set row timestep");
        }
    }
    if (columnTimestep->m_present)
    {
        float step = (float)columnTimestep->getDouble(1);
        if (myCifti.setColumnTimestep(step))
        {
            modified = true;
        } else {
            CaretLogWarning("could not set column timestep");
        }
    }
    if (modified)
    {
        myCifti.writeFile(ciftiName);
    }
}

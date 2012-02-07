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
    OptionalParameter* rowTimestep = ret->createOptionalParameter(2, "-row-timestep", "set the row timestep");
    rowTimestep->addDoubleParameter(1, "seconds", "seconds per timestep");
    OptionalParameter* columnTimestep = ret->createOptionalParameter(3, "-column-timestep", "set the column timestep");
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

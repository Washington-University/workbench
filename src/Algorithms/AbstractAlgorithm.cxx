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

#include "AbstractAlgorithm.h"

using namespace std;
using namespace caret;

AbstractAlgorithm::AbstractAlgorithm(ProgressObject* myProgressObject)
{
    m_progObj = myProgressObject;
    m_finish = true;
    if (m_progObj == NULL)
    {
        m_finish = false;
        return;
    }
    myProgressObject->algorithmStartSentinel();
    if (myProgressObject->isDisabled())
    {
        m_finish = false;//don't let a subalgorithm finish the progress bar if the main algorithm ignores it
    }
}

float AbstractAlgorithm::getAlgorithmInternalWeight()
{
    return 1.0f;
}

float AbstractAlgorithm::getSubAlgorithmWeight()
{
    return 0.0f;
}

float AbstractAlgorithm::getAlgorithmWeight()
{
    return getAlgorithmInternalWeight() + getSubAlgorithmWeight();
}

AbstractAlgorithm::~AbstractAlgorithm()
{
    if ((m_progObj != NULL) && m_finish)
    {
        m_progObj->forceFinish();
    }
}

AlgorithmParserInterface::~AlgorithmParserInterface()
{
    delete m_dummy;
}

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
/*LICENSE_END*/

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretMutex.h"

using namespace caret;

CaretMutexLocker::CaretMutexLocker(CaretMutex* theMutex) : QMutexLocker(theMutex)
{
}

CaretMutex::CaretMutex(QMutex::RecursionMode mode): QMutex(mode)
{
}

CaretThrowMutex::CaretThrowMutex()
{
    m_inUse = false;
}

CaretThrowMutexLocker::CaretThrowMutexLocker(CaretThrowMutex* theMutex, const char* message)
{
    CaretAssert(theMutex != NULL);
    m_Mutex = theMutex;
    if (m_Mutex->m_inUse) throw CaretException(message);//don't bother making this atomic, we don't need it to be guaranteed to catch all problems
    m_Mutex->m_inUse = true;
}

CaretThrowMutexLocker::~CaretThrowMutexLocker()
{
    m_Mutex->m_inUse = false;
}

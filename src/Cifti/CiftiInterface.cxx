/*LICENSE_START*/
/*
 *  Copyright 1995-2011 Washington University School of Medicine
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

#include "CiftiInterface.h"

using namespace caret;
using namespace std;

bool CiftiInterface::checkColumnIndex(int64_t index) const
{
    if (index < 0 || index >= getNumberOfColumns())
    {
        return false;
    }
    return true;
}

bool CiftiInterface::checkRowIndex(int64_t index) const
{
    if (index < 0 || index >= getNumberOfRows())
    {
        return false;
    }
    return true;
}

bool CiftiInterface::getColumnFromNode(float* columnOut, const int64_t node, const caret::StructureEnum::Enum structure) const
{
    int64_t myIndex = m_xml.getColumnIndexForNode(node, structure);
    if (!checkColumnIndex(myIndex))
    {
        return false;
    }
    getColumn(columnOut, myIndex);
    return true;
}

bool CiftiInterface::getColumnFromVoxel(float* columnOut, const int64_t* ijk) const
{
    int64_t myIndex = m_xml.getColumnIndexForVoxel(ijk);
    if (!checkColumnIndex(myIndex))
    {
        return false;
    }
    getColumn(columnOut, myIndex);
    return true;
}

bool CiftiInterface::getRowFromNode(float* rowOut, const int64_t node, const caret::StructureEnum::Enum structure) const
{
    int64_t myIndex = m_xml.getRowIndexForNode(node, structure);
    if (!checkRowIndex(myIndex))
    {
        return false;
    }
    getRow(rowOut, myIndex);
    return true;
}

bool CiftiInterface::getRowFromVoxel(float* rowOut, const int64_t* ijk) const
{
    int64_t myIndex = m_xml.getRowIndexForVoxel(ijk);
    if (!checkRowIndex(myIndex))
    {
        return false;
    }
    getRow(rowOut, myIndex);
    return true;
}

bool CiftiInterface::getColumnFromVoxelCoordinate(float* columnOut, const float* xyz) const
{
    int64_t myIndex = m_xml.getColumnIndexForVoxelCoordinate(xyz);
    if (!checkColumnIndex(myIndex))
    {
        return false;
    }
    getColumn(columnOut, myIndex);
    return true;
}

bool CiftiInterface::getRowFromVoxelCoordinate(float* rowOut, const float* xyz) const
{
    int64_t myIndex = m_xml.getRowIndexForVoxelCoordinate(xyz);
    if (!checkRowIndex(myIndex))
    {
        return false;
    }
    getRow(rowOut, myIndex);
    return true;
}

bool CiftiInterface::getColumnFromTimepoint(float* columnOut, const float seconds) const
{
    int64_t myIndex = m_xml.getColumnIndexForTimepoint(seconds);
    if (!checkColumnIndex(myIndex))
    {
        return false;
    }
    getColumn(columnOut, myIndex);
    return true;
}

bool CiftiInterface::getRowFromTimepoint(float* rowOut, const float seconds) const
{
    int64_t myIndex = m_xml.getRowIndexForTimepoint(seconds);
    if (!checkRowIndex(myIndex))
    {
        return false;
    }
    getRow(rowOut, myIndex);
    return true;
}

CiftiInterface::~CiftiInterface()
{
}

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

#include <limits>
#include <vector>

using namespace caret;
using namespace std;

CiftiInterface::CiftiInterface()
{
    m_dataRangeValid = false;
}

void CiftiInterface::invalidateDataRange()
{
    m_dataRangeValid = false;
}

bool CiftiInterface::getDataRangeFromAllMaps(float& minOut, float& maxOut) const
{
    if (!m_dataRangeValid)
    {
        int64_t numRows = getNumberOfRows(), rowSize = getNumberOfColumns();
        if (numRows <= 0 || rowSize <= 0)
        {
            maxOut = numeric_limits<float>::max();
            minOut = -maxOut;
            return false;
        }
        m_dataRangeMin = numeric_limits<float>::max();
        m_dataRangeMax = -m_dataRangeMin;
        vector<float> tempRow(rowSize);
        for (int64_t row = 0; row < numRows; ++row)
        {
            getRow(tempRow.data(), row);
            for (int64_t i = 0; i < rowSize; ++i)
            {
                if (tempRow[i] > m_dataRangeMax) m_dataRangeMax = tempRow[i];
                if (tempRow[i] < m_dataRangeMin) m_dataRangeMin = tempRow[i];
            }
        }
        m_dataRangeValid = true;
    }
    minOut = m_dataRangeMin;
    maxOut = m_dataRangeMax;
    return true;
}

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

bool CiftiInterface::getRowFromNode(float* rowOut, const int64_t node, const caret::StructureEnum::Enum structure, int64_t& rowIndexOut) const
{
    rowIndexOut = -1;
    int64_t myIndex = m_xml.getRowIndexForNode(node, structure);
    if (!checkRowIndex(myIndex))
    {
        return false;
    }
    getRow(rowOut, myIndex);
    rowIndexOut = myIndex;
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

bool CiftiInterface::getRowFromVoxelCoordinate(float* rowOut, const float* xyz, int64_t& rowIndexOut) const
{
    rowIndexOut = -1;
    int64_t myIndex = m_xml.getRowIndexForVoxelCoordinate(xyz);
    if (!checkRowIndex(myIndex))
    {
        return false;
    }
    getRow(rowOut, myIndex);
    rowIndexOut = myIndex;
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

//column and frame are the same value currently, this function exists only
//to keep the concepts of frame and time separate from being conflated
bool CiftiInterface::getColumnFromFrame(float* columnOut, const int frame) const
{
    if(!checkColumnIndex(frame))
    {
        return false;
    }
    getColumn(columnOut, frame);
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

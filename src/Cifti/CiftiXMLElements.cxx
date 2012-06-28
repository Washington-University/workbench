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
#include "CiftiXMLElements.h"
#include "CiftiFileException.h"
#include "GiftiLabelTable.h"

using namespace caret;
using namespace std;

void CiftiBrainModelElement::setupLookup()
{
    if (m_modelType == CIFTI_MODEL_TYPE_SURFACE)
    {
        if (m_nodeIndices.size() == 0 && m_indexCount != 0)
        {
            if (m_indexCount != m_surfaceNumberOfNodes)
            {
                throw CiftiFileException("empty index list found with nonzero indexCount, but indexCount and surfaceNumberOfNodes don't match");
            }
            m_nodeToIndexLookup.resize(m_surfaceNumberOfNodes);
            for (int i = 0; i < (int)m_surfaceNumberOfNodes; ++i)
            {
                m_nodeToIndexLookup[i] = i + m_indexOffset;
            }
        } else {
            if (m_indexCount != (int64_t)m_nodeIndices.size())
            {
                throw CiftiFileException("indexCount and size of nodeIndices don't match");
            }
            m_nodeToIndexLookup.resize(m_surfaceNumberOfNodes);
            for (int i = 0; i < (int)m_surfaceNumberOfNodes; ++i)
            {
                m_nodeToIndexLookup[i] = -1;
            }
            for (int i = 0; i < (int)m_indexCount; ++i)
            {
                m_nodeToIndexLookup[m_nodeIndices[i]] = i + m_indexOffset;
            }
        }
    }
}

bool CiftiMatrixIndicesMapElement::operator==(const CiftiMatrixIndicesMapElement& rhs) const
{//NOTE: don't check the applies to dimension vector, this should check only the mapping, not how it is used
    if (this == &rhs) return true;//compare pointers to skip checking object against itself
    if (m_indicesMapToDataType != rhs.m_indicesMapToDataType) return false;
    switch (m_indicesMapToDataType)
    {
        case CIFTI_INDEX_TYPE_INVALID:
            break;//is there anything to check?
        case CIFTI_INDEX_TYPE_BRAIN_MODELS:
            {
                if (m_brainModels.size() != rhs.m_brainModels.size()) return false;
                vector<bool> used(rhs.m_brainModels.size(), false);//prevent reuse, in case some idiocy winds up having overlapping mappings
                for (size_t i = 0; i < m_brainModels.size(); ++i)//need to allow the mappings to be placed in a different order, as long as the cifti index ranges line up
                {
                    bool found = false;
                    for (size_t j = 0; j < rhs.m_brainModels.size(); ++j)
                    {
                        if (!used[j] && m_brainModels[i] == rhs.m_brainModels[j])
                        {
                            used[j] = true;
                            found = true;
                            break;
                        }
                    }
                    if (!found) return false;
                }
            }
            break;
        case CIFTI_INDEX_TYPE_FIBERS:
            break;//???
        case CIFTI_INDEX_TYPE_PARCELS:
            break;//???
        case CIFTI_INDEX_TYPE_TIME_POINTS:
            {
                if (m_numTimeSteps != rhs.m_numTimeSteps) return false;
                float timestep, rhtimestep;
                switch (m_timeStepUnits)
                {
                    case NIFTI_UNITS_SEC:
                        timestep = m_timeStep;
                        break;
                    case NIFTI_UNITS_MSEC:
                        timestep = m_timeStep * 0.001f;
                        break;
                    case NIFTI_UNITS_USEC:
                        timestep = m_timeStep * 0.000001f;
                        break;
                    default:
                        return false;
                }
                switch (rhs.m_timeStepUnits)
                {
                    case NIFTI_UNITS_SEC:
                        rhtimestep = rhs.m_timeStep;
                        break;
                    case NIFTI_UNITS_MSEC:
                        rhtimestep = rhs.m_timeStep * 0.001f;
                        break;
                    case NIFTI_UNITS_USEC:
                        rhtimestep = rhs.m_timeStep * 0.000001f;
                        break;
                    default:
                        return false;
                }
                const float TOLERANCE = 0.999f;//if they don't match exactly, and either one of them is zero, or their ratio is far from 1, say they don't match
                if (timestep != rhtimestep && (timestep == 0.0f || rhtimestep == 0.0f || timestep / rhtimestep < TOLERANCE || rhtimestep / timestep < TOLERANCE)) return false;
            }
            break;
        case CIFTI_INDEX_TYPE_LABELS:
        case CIFTI_INDEX_TYPE_SCALARS:
            {
                size_t size = m_namedMaps.size();
                if (rhs.m_namedMaps.size() != size) return false;
                vector<bool> used(size, false);
                for (size_t i = 0; i < size; ++i)
                {
                    if (m_namedMaps[i] != rhs.m_namedMaps[i]) return false;
                }
            }
            break;
    }
    return true;
}

bool CiftiBrainModelElement::operator==(const caret::CiftiBrainModelElement& rhs) const
{
    if (m_indexOffset != rhs.m_indexOffset) return false;
    if (m_indexCount != rhs.m_indexCount) return false;
    if (m_modelType != rhs.m_modelType) return false;
    if (m_brainStructure != rhs.m_brainStructure) return false;
    switch (m_modelType)
    {
        case CIFTI_MODEL_TYPE_SURFACE:
            {
                size_t numIndices = m_nodeIndices.size(), rhsIndices = rhs.m_nodeIndices.size();
                if (m_surfaceNumberOfNodes != rhs.m_surfaceNumberOfNodes) return false;
                if (numIndices == 0)
                {
                    if (rhsIndices != 0)
                    {
                        if ((int64_t)rhsIndices != rhs.m_indexCount) return false;
                        for (size_t i = 0; i < rhsIndices; ++i)
                        {
                            if (rhs.m_nodeIndices[i] != (int64_t)i) return false;
                        }
                    }
                } else {
                    if (rhsIndices == 0)
                    {
                        if ((int64_t)numIndices != m_indexCount) return false;
                        for (size_t i = 0; i < numIndices; ++i)
                        {
                            if (m_nodeIndices[i] != (int64_t)i) return false;
                        }
                    } else {
                        if (numIndices != rhsIndices) return false;
                        for (size_t i = 0; i < numIndices; ++i)
                        {
                            if (m_nodeIndices[i] != rhs.m_nodeIndices[i]) return false;
                        }
                    }
                }
            }
            break;
        case CIFTI_MODEL_TYPE_VOXELS:
            {
                size_t numIndices = m_voxelIndicesIJK.size(), rhsIndices = m_voxelIndicesIJK.size();
                if (numIndices != rhsIndices) return false;
                for (size_t i = 0; i < numIndices; ++i)//treat them as flat, even though they aren't
                {
                    if (m_voxelIndicesIJK[i] != rhs.m_voxelIndicesIJK[i]) return false;
                }
            }
            break;
    }
    return true;
}

bool CiftiNamedMapElement::operator==(const CiftiNamedMapElement& rhs) const
{
    if (m_mapName != rhs.m_mapName) return false;
    if (m_labelTable == NULL)
    {
        if (rhs.m_labelTable != NULL) return false;
    } else {
        if (rhs.m_labelTable == NULL) return false;
        if (!m_labelTable->matches(*(rhs.m_labelTable))) return false;
    }
    return true;
}

CiftiNamedMapElement::CiftiNamedMapElement(const CiftiNamedMapElement& rhs)
{
    m_mapName = rhs.m_mapName;
    m_labelTable.grabNew(new GiftiLabelTable(*(rhs.m_labelTable)));
}

CiftiNamedMapElement& CiftiNamedMapElement::operator=(const CiftiNamedMapElement& rhs)
{
    if (this == &rhs) return *this;
    m_mapName = rhs.m_mapName;
    m_labelTable.grabNew(new GiftiLabelTable(*(rhs.m_labelTable)));
    return *this;
}

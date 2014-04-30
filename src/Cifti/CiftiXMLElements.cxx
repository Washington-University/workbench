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
#include "CiftiXMLElements.h"

#include "DataFileException.h"
#include "GiftiLabelTable.h"
#include "PaletteColorMapping.h"
#include "VoxelIJK.h"

#include <algorithm>
#include <set>

using namespace caret;
using namespace std;

void CiftiBrainModelElement::setupLookup(CiftiMatrixIndicesMapElement& myMap)
{
    if (m_modelType == CIFTI_MODEL_TYPE_SURFACE)
    {
        if (m_nodeIndices.size() == 0 && m_indexCount != 0)
        {
            if (m_indexCount != m_surfaceNumberOfNodes)
            {
                throw DataFileException("empty index list found with nonzero indexCount, but indexCount and surfaceNumberOfNodes don't match");
            }
            m_nodeToIndexLookup.resize(m_surfaceNumberOfNodes);
            for (int i = 0; i < (int)m_surfaceNumberOfNodes; ++i)
            {
                m_nodeToIndexLookup[i] = i + m_indexOffset;
            }
        } else {
            if (m_indexCount != (int64_t)m_nodeIndices.size())
            {
                throw DataFileException("indexCount and size of nodeIndices don't match");
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
    if (m_modelType == CIFTI_MODEL_TYPE_VOXELS)
    {//we need access to the Volume element to support the "empty list" behavior, but since it is never used, and will be removed from the spec, ignore it
        int64_t voxListSize = (int64_t)m_voxelIndicesIJK.size();
        for (int64_t i = 0; i < voxListSize; i += 3)
        {
            myMap.m_voxelToIndexLookup.at(m_voxelIndicesIJK[i], m_voxelIndicesIJK[i + 1], m_voxelIndicesIJK[i + 2]) = m_indexOffset + (i / 3);
        }
    }
}

void CiftiMatrixIndicesMapElement::setupLookup()
{
    if (m_indicesMapToDataType == CIFTI_INDEX_TYPE_BRAIN_MODELS)
    {
        stable_sort(m_brainModels.begin(), m_brainModels.end());//in order to make the overlapping range check easy, and so that iterating through maps goes linearly through the file
        int numModels = (int)m_brainModels.size();
        set<StructureEnum::Enum> usedSurf, usedVol;
        for (int i = 0; i < numModels; ++i)
        {
            if (i > 0 && m_brainModels[i - 1].m_indexOffset + m_brainModels[i - 1].m_indexCount > m_brainModels[i].m_indexOffset)
            {
                throw DataFileException("overlapping index ranges found in a brain models dimension");
            }
            switch (m_brainModels[i].m_modelType)
            {
                case CIFTI_MODEL_TYPE_SURFACE:
                    if (usedSurf.find(m_brainModels[i].m_brainStructure) != usedSurf.end())
                    {
                        throw DataFileException("structure " + StructureEnum::toName(m_brainModels[i].m_brainStructure) + " used multiple times in surface models");
                    }
                    usedSurf.insert(m_brainModels[i].m_brainStructure);
                    break;
                case CIFTI_MODEL_TYPE_VOXELS:
                    if (usedVol.find(m_brainModels[i].m_brainStructure) != usedVol.end())
                    {
                        throw DataFileException("structure " + StructureEnum::toName(m_brainModels[i].m_brainStructure) + " used multiple times in volume models");
                    }
                    usedVol.insert(m_brainModels[i].m_brainStructure);
                    break;
                case CIFTI_MODEL_TYPE_INVALID:
                    CaretAssert(false);
                    throw DataFileException("found 'invalid' brain model type, tell the developers");
                    break;
            }
            m_brainModels[i].setupLookup(*this);
        }
    }
    if (m_indicesMapToDataType != CIFTI_INDEX_TYPE_PARCELS) return;
    int numSurfs = (int)m_parcelSurfaces.size();
    for (int i = 0; i < numSurfs; ++i)
    {
        m_parcelSurfaces[i].m_lookup.resize(m_parcelSurfaces[i].m_numNodes);
        for (int j = 0; j < m_parcelSurfaces[i].m_numNodes; ++j)
        {
            m_parcelSurfaces[i].m_lookup[j] = -1;
        }
        for (int j = i + 1; j < numSurfs; ++j)
        {
            if (m_parcelSurfaces[i].m_structure == m_parcelSurfaces[j].m_structure)
            {
                throw DataFileException("multiple surfaces with same structure found in a parcel map");
            }
        }
    }
    int numParcels = (int)m_parcels.size();
    for (int i = 0; i < numParcels; ++i)
    {
        const CiftiParcelElement& myParcel = m_parcels[i];
        int numSurfParts = (int)myParcel.m_nodeElements.size();
        vector<bool> surfUsed(m_parcelSurfaces.size(), false);
        for (int j = 0; j < numSurfParts; ++j)
        {
            const CiftiParcelNodesElement& myNodeElement = myParcel.m_nodeElements[j];
            StructureEnum::Enum myStruct = myNodeElement.m_structure;
            int whichSurf;
            for (whichSurf = 0; whichSurf < numSurfs; ++whichSurf)
            {
                if (m_parcelSurfaces[whichSurf].m_structure == myStruct) break;
            }
            if (whichSurf >= numSurfs)
            {
                throw DataFileException("parcel '" + myParcel.m_parcelName + "' specifies a structure that doesn't match a specified surface");
            }
            if (surfUsed[whichSurf])
            {
                throw DataFileException("parcel '" + myParcel.m_parcelName + "' specified a surface structure more than once");
            }
            CiftiParcelSurfaceElement& mySurf = m_parcelSurfaces[whichSurf];
            surfUsed[whichSurf] = true;
            int numNodes = myNodeElement.m_nodes.size();
            for (int k = 0; k < numNodes; ++k)
            {
                int64_t node = myNodeElement.m_nodes[k];
                if (node < 0 || node >= mySurf.m_numNodes)
                {
                    throw DataFileException("node number " + AString::number(node) + " is invalid for surface " + StructureEnum::toName(myStruct));
                }
                if (mySurf.m_lookup[node] != -1)
                {
                    throw DataFileException("surface node " + AString::number(node) + " in surface " + StructureEnum::toName(myStruct) + " specified more than once");
                }
                mySurf.m_lookup[node] = i;
            }
        }
        int64_t voxListSize = (int64_t)myParcel.m_voxelIndicesIJK.size();
        for (int64_t j = 0; j < voxListSize; j += 3)
        {
            int64_t* test = m_voxelToIndexLookup.find(myParcel.m_voxelIndicesIJK[j], myParcel.m_voxelIndicesIJK[j + 1], myParcel.m_voxelIndicesIJK[j + 2]);
            if (test != NULL && *test != i)
            {
                throw DataFileException("voxel " + AString::number(myParcel.m_voxelIndicesIJK[j]) + ", " +
                                                    AString::number(myParcel.m_voxelIndicesIJK[j + 1]) + ", " +
                                                    AString::number(myParcel.m_voxelIndicesIJK[j + 2]) + " used by more than one parcel");
            }
            m_voxelToIndexLookup.at(myParcel.m_voxelIndicesIJK[j], myParcel.m_voxelIndicesIJK[j + 1], myParcel.m_voxelIndicesIJK[j + 2]) = i;
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
            {
                if (m_parcels.size() != rhs.m_parcels.size() || m_parcelSurfaces.size() != rhs.m_parcelSurfaces.size()) return false;
                vector<bool> used(rhs.m_parcelSurfaces.size(), false);
                for (size_t i = 0; i < m_parcelSurfaces.size(); ++i)
                {
                    bool found = false;
                    for (size_t j = 0; j < rhs.m_parcelSurfaces.size(); ++j)
                    {
                        if (!used[j] && m_parcelSurfaces[i] == rhs.m_parcelSurfaces[j])
                        {
                            used[j] = true;
                            found = true;
                            break;
                        }
                    }
                    if (!found) return false;
                }
                for (size_t i = 0; i < m_parcels.size(); ++i)
                {
                    if (m_parcels[i] != rhs.m_parcels[i]) return false;
                }
            }
            break;
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
                for (size_t i = 0; i < size; ++i)
                {
                    if (m_namedMaps[i] != rhs.m_namedMaps[i]) return false;
                }
            }
            break;
    }
    return true;
}

bool CiftiBrainModelElement::operator==(const CiftiBrainModelElement& rhs) const
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
        case CIFTI_MODEL_TYPE_INVALID:
            CaretAssert(false);
            throw DataFileException("found 'invalid' brain model type, tell the developers");
            break;
    }
    return true;
}

bool CiftiBrainModelElement::operator<(const CiftiBrainModelElement& rhs) const
{
    return m_indexOffset < rhs.m_indexOffset;
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
    m_defaultPalette = rhs.m_defaultPalette;
    m_mapMetaData = rhs.m_mapMetaData;
    if (rhs.m_palette != NULL) m_palette.grabNew(new PaletteColorMapping(*(rhs.m_palette)));
    if (rhs.m_labelTable != NULL) m_labelTable.grabNew(new GiftiLabelTable(*(rhs.m_labelTable)));
}

CiftiNamedMapElement& CiftiNamedMapElement::operator=(const CiftiNamedMapElement& rhs)
{
    if (this == &rhs) return *this;
    m_mapName = rhs.m_mapName;
    m_mapMetaData = rhs.m_mapMetaData;
    if (rhs.m_palette != NULL)
    {
        m_palette.grabNew(new PaletteColorMapping(*(rhs.m_palette)));
    } else {
        m_palette.grabNew(NULL);
    }
    if (rhs.m_labelTable != NULL)
    {
        m_labelTable.grabNew(new GiftiLabelTable(*(rhs.m_labelTable)));
    } else {
        m_labelTable.grabNew(NULL);
    }
    return *this;
}

///NOTE: this is not a standard equality test, it skips checking the nodes list because we do that elsewhere
bool CiftiParcelNodesElement::operator==(const CiftiParcelNodesElement& rhs) const
{
    if (m_structure != rhs.m_structure) return false;
    if (m_nodes.size() != rhs.m_nodes.size()) return false;
    /*for (size_t i = 0; i < m_nodes.size(); ++i)
    {
        if (m_nodes[i] != rhs.m_nodes[i]) return false;
    }//*///not needed, we check node equivalence by checking the lookup in each surface, this way wouldn't work for same nodes in different order anyway
    return true;
}

bool CiftiParcelElement::operator==(const CiftiParcelElement& rhs) const
{
    if (m_parcelName != rhs.m_parcelName) return false;
    if (m_nodeElements.size() != rhs.m_nodeElements.size() || m_voxelIndicesIJK.size() != rhs.m_voxelIndicesIJK.size()) return false;
    vector<bool> used(rhs.m_nodeElements.size(), false);
    for (size_t i = 0; i < m_nodeElements.size(); ++i)
    {
        bool found = false;
        for (size_t j = 0; j < rhs.m_nodeElements.size(); ++j)
        {
            if (!used[j] && m_nodeElements[i] == rhs.m_nodeElements[j])
            {
                found = true;
                used[j] = true;
                break;
            }
        }
        if (!found) return false;
    }
    set<VoxelIJK> myvoxset, rhsvoxset;//different orders mean the same thing, so sort voxels, then compare
    for (size_t i = 0; i < m_voxelIndicesIJK.size(); i += 3)
    {
        myvoxset.insert(VoxelIJK(m_voxelIndicesIJK.data() + i));
        rhsvoxset.insert(VoxelIJK(rhs.m_voxelIndicesIJK.data() + i));
    }
    return (myvoxset == rhsvoxset);
}

bool CiftiParcelSurfaceElement::operator==(const CiftiParcelSurfaceElement& rhs) const
{
    if (m_structure != rhs.m_structure) return false;
    if (m_numNodes != rhs.m_numNodes) return false;
    CaretAssert(m_numNodes == (int64_t)m_lookup.size());
    CaretAssert(rhs.m_lookup.size() == m_lookup.size());
    for (size_t i = 0; i < m_lookup.size(); ++i)
    {//check lookup instead of checking node lists in parcels
        if (m_lookup[i] != rhs.m_lookup[i]) return false;
    }
    return true;
}

CiftiMatrixElement::CiftiMatrixElement(const CiftiMatrixElement& rhs)
{
    m_labelTable = rhs.m_labelTable;
    m_userMetaData = rhs.m_userMetaData;
    if (rhs.m_palette != NULL) m_palette.grabNew(new PaletteColorMapping(*(rhs.m_palette)));
    m_defaultPalette = rhs.m_defaultPalette;
    m_matrixIndicesMap = rhs.m_matrixIndicesMap;
    m_volume = rhs.m_volume;
}

CiftiMatrixElement& CiftiMatrixElement::operator=(const caret::CiftiMatrixElement& rhs)
{
    if (this == &rhs) return *this;
    m_labelTable = rhs.m_labelTable;
    m_userMetaData = rhs.m_userMetaData;
    if (rhs.m_palette != NULL) m_palette.grabNew(new PaletteColorMapping(*(rhs.m_palette)));
    m_defaultPalette = rhs.m_defaultPalette;
    m_matrixIndicesMap = rhs.m_matrixIndicesMap;
    m_volume = rhs.m_volume;
    return *this;
}

CiftiNamedMapElement::CiftiNamedMapElement()
{//just so that the destructors of GiftiLabelTable and PaletteColorMapping are resolved without including them into the .h
    m_defaultPalette = false;
}

CiftiNamedMapElement::~CiftiNamedMapElement()
{
}

CiftiMatrixElement::CiftiMatrixElement()
{//ditto
    m_defaultPalette = false;
}

CiftiMatrixElement::~CiftiMatrixElement()
{
}

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

#include "CiftiBrainModelsMap.h"

#include "DataFileException.h"

#include <QRegularExpression>
#include <QStringList>

#include <algorithm>

using namespace std;
using namespace caret;

void CiftiBrainModelsMap::addSurfaceModel(const int64_t& numberOfNodes, const StructureEnum::Enum& structure, const float* roi)
{
    vector<int64_t> tempVector;//pass-through to the other addSurfaceModel after converting roi to vector of indices
    tempVector.reserve(numberOfNodes);//to make it allocate only once
    for (int64_t i = 0; i < numberOfNodes; ++i)
    {
        if (roi == NULL || roi[i] > 0.0f)
        {
            tempVector.push_back(i);
        }
    }
    addSurfaceModel(numberOfNodes, structure, tempVector);
}

void CiftiBrainModelsMap::addSurfaceModel(const int64_t& numberOfNodes, const StructureEnum::Enum& structure, const vector<int64_t>& nodeList)
{
    if (m_surfUsed.find(structure) != m_surfUsed.end())
    {
        throw DataFileException("surface structures cannot be repeated in a brain models map");
    }
    BrainModelPriv myModel;
    myModel.m_type = SURFACE;
    myModel.m_brainStructure = structure;
    myModel.m_surfaceNumberOfNodes = numberOfNodes;
    myModel.m_nodeIndices = nodeList;
    myModel.setupSurface(getNextStart());//do internal setup - also does error checking
    m_modelsInfo.push_back(myModel);
    m_surfUsed[structure] = m_modelsInfo.size() - 1;
}

void CiftiBrainModelsMap::BrainModelPriv::setupSurface(const int64_t& start)
{
    if (m_surfaceNumberOfNodes < 1)
    {
        throw DataFileException("surface must have at least 1 vertex");
    }
    m_modelStart = start;
    int64_t listSize = (int64_t)m_nodeIndices.size();
    if (listSize == 0)
    {
        throw DataFileException("vertex list must have nonzero length");//NOTE: technically not required by Cifti-1, remove if problematic
    }
    m_modelEnd = start + listSize;//one after last
    vector<bool> used(m_surfaceNumberOfNodes, false);
    m_nodeToIndexLookup = vector<int64_t>(m_surfaceNumberOfNodes, -1);//reset all to -1 to start
    for (int64_t i = 0; i < listSize; ++i)
    {
        if (m_nodeIndices[i] < 0)
        {
            throw DataFileException("vertex list contains negative index");
        }
        if (m_nodeIndices[i] >= m_surfaceNumberOfNodes)
        {
            throw DataFileException("vertex list contains an index that don't exist in the surface");
        }
        if (used[m_nodeIndices[i]])
        {
            throw DataFileException("vertex list contains reused index");
        }
        used[m_nodeIndices[i]] = true;
        m_nodeToIndexLookup[m_nodeIndices[i]] = start + i;
    }
}

void CiftiBrainModelsMap::addVolumeModel(const StructureEnum::Enum& structure, const vector<int64_t>& ijkList)
{
    if (m_volUsed.find(structure) != m_volUsed.end())
    {
        throw DataFileException("volume structures cannot be repeated in a brain models map");
    }
    int64_t listSize = (int64_t)ijkList.size();
    if (listSize == 0)
    {
        throw DataFileException("voxel list must have nonzero length");//NOTE: technically not required by Cifti-1, remove if problematic
    }
    if (listSize % 3 != 0)
    {
        throw DataFileException("voxel list must have a length that is a multiple of 3");
    }
    int64_t numElems = listSize / 3;
    const int64_t* dims = NULL;
    if (!m_ignoreVolSpace)
    {
        if (!m_haveVolumeSpace)
        {
            throw DataFileException("you must set the volume space before adding volume models");
        }
        dims = m_volSpace.getDims();
    }
    CaretCompact3DLookup<std::pair<int64_t, StructureEnum::Enum> > tempLookup = m_voxelToIndexLookup;//a copy of the lookup should be faster than other methods of checking for overlap and repeat
    int64_t nextStart = getNextStart();
    for (int64_t index = 0; index < numElems; ++index)//do all error checking before adding to lookup
    {
        int64_t index3 = index * 3;
        if (ijkList[index3] < 0 || ijkList[index3 + 1] < 0 || ijkList[index3 + 2] < 0)
        {
            throw DataFileException("found negative index in voxel list");
        }
        if (!m_ignoreVolSpace && (ijkList[index3] >= dims[0] ||
                                    ijkList[index3 + 1] >= dims[1] ||
                                    ijkList[index3 + 2] >= dims[2]))
        {
            throw DataFileException("found invalid index triple in voxel list: (" + AString::number(ijkList[index3]) + ", "
                                  + AString::number(ijkList[index3 + 1]) + ", " + AString::number(ijkList[index3 + 2]) + ")");
        }
        if (tempLookup.find(ijkList[index3], ijkList[index3 + 1], ijkList[index3 + 2]) != NULL)
        {
            throw DataFileException("volume models may not reuse voxels, either internally or from other structures");
        }
        tempLookup.at(ijkList[index3], ijkList[index3 + 1], ijkList[index3 + 2]) = pair<int64_t, StructureEnum::Enum>(nextStart + index, structure);
    }
    m_voxelToIndexLookup = tempLookup;
    BrainModelPriv myModel;
    myModel.m_type = VOXELS;
    myModel.m_brainStructure = structure;
    myModel.m_voxelIndicesIJK = ijkList;
    myModel.m_modelStart = nextStart;
    myModel.m_modelEnd = nextStart + numElems;//one after last
    m_modelsInfo.push_back(myModel);
    m_volUsed[structure] = m_modelsInfo.size() - 1;
}

void CiftiBrainModelsMap::clear()
{
    m_modelsInfo.clear();
    m_haveVolumeSpace = false;
    m_ignoreVolSpace = false;
    m_voxelToIndexLookup.clear();
    m_surfUsed.clear();
    m_volUsed.clear();
}

int64_t CiftiBrainModelsMap::getIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const
{
    CaretAssert(node >= 0);
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    if (iter == m_surfUsed.end())
    {
        return -1;
    }
    CaretAssertVectorIndex(m_modelsInfo, iter->second);
    const BrainModelPriv& myModel = m_modelsInfo[iter->second];
    if (node >= myModel.m_surfaceNumberOfNodes) return -1;
    CaretAssertVectorIndex(myModel.m_nodeToIndexLookup, node);
    return myModel.m_nodeToIndexLookup[node];
}

int64_t CiftiBrainModelsMap::getIndexForVoxel(const int64_t* ijk, StructureEnum::Enum* structureOut) const
{
    return getIndexForVoxel(ijk[0], ijk[1], ijk[2], structureOut);
}

int64_t CiftiBrainModelsMap::getIndexForVoxel(const int64_t& i, const int64_t& j, const int64_t& k, StructureEnum::Enum* structureOut) const
{
    const pair<int64_t, StructureEnum::Enum>* iter = m_voxelToIndexLookup.find(i, j, k);//the lookup tolerates weirdness like negatives
    if (iter == NULL) return -1;
    if (structureOut != NULL) *structureOut = iter->second;
    return iter->first;
}

CiftiBrainModelsMap::IndexInfo CiftiBrainModelsMap::getInfoForIndex(const int64_t index) const
{
    CaretAssert(index >= 0 && index < getLength());
    IndexInfo ret;
    int numModels = (int)m_modelsInfo.size();
    int low = 0, high = numModels - 1;//bisection search
    while (low != high)
    {
        int guess = (low + high) / 2;
        if (m_modelsInfo[guess].m_modelEnd > index)//modelEnd is 1 after last valid index, equal to next start if there is a next
        {
            if (m_modelsInfo[guess].m_modelStart > index)
            {
                high = guess - 1;
            } else {
                high = guess;
                low = guess;
            }
        } else {
            low = guess + 1;
        }
    }
    CaretAssert(index >= m_modelsInfo[low].m_modelStart && index < m_modelsInfo[low].m_modelEnd);//otherwise we have a broken invariant
    ret.m_structure = m_modelsInfo[low].m_brainStructure;
    ret.m_type = m_modelsInfo[low].m_type;
    if (ret.m_type == SURFACE)
    {
        ret.m_surfaceNode = m_modelsInfo[low].m_nodeIndices[index - m_modelsInfo[low].m_modelStart];
    } else {
        int64_t baseIndex = 3 * (index - m_modelsInfo[low].m_modelStart);
        ret.m_ijk[0] = m_modelsInfo[low].m_voxelIndicesIJK[baseIndex];
        ret.m_ijk[1] = m_modelsInfo[low].m_voxelIndicesIJK[baseIndex + 1];
        ret.m_ijk[2] = m_modelsInfo[low].m_voxelIndicesIJK[baseIndex + 2];
    }
    return ret;
}

int64_t CiftiBrainModelsMap::getLength() const
{
    return getNextStart();
}

vector<CiftiBrainModelsMap::ModelInfo> CiftiBrainModelsMap::getModelInfo() const
{
    vector<ModelInfo> ret;
    int numModels = (int)m_modelsInfo.size();
    ret.resize(numModels);
    for (int i = 0; i < numModels; ++i)
    {
        ret[i].m_structure = m_modelsInfo[i].m_brainStructure;
        ret[i].m_type = m_modelsInfo[i].m_type;
        ret[i].m_indexStart = m_modelsInfo[i].m_modelStart;
        ret[i].m_indexCount = m_modelsInfo[i].m_modelEnd - m_modelsInfo[i].m_modelStart;
    }
    return ret;
}

int64_t CiftiBrainModelsMap::getNextStart() const
{
    if (m_modelsInfo.size() == 0) return 0;
    return m_modelsInfo.back().m_modelEnd;//NOTE: the models are sorted by their index range, so this works
}

const vector<int64_t>& CiftiBrainModelsMap::getNodeList(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    if (iter == m_surfUsed.end())
    {
        throw DataFileException("getNodeList called for nonexistant structure");//throw if it doesn't exist, because we don't have a reference to return - things should identify which structures exist before calling this
    }
    CaretAssertVectorIndex(m_modelsInfo, iter->second);
    return m_modelsInfo[iter->second].m_nodeIndices;
}

vector<CiftiBrainModelsMap::SurfaceMap> CiftiBrainModelsMap::getSurfaceMap(const StructureEnum::Enum& structure) const
{
    vector<SurfaceMap> ret;
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    if (iter == m_surfUsed.end())
    {
        throw DataFileException("getSurfaceMap called for nonexistant structure");//also throw, for consistency
    }
    CaretAssertVectorIndex(m_modelsInfo, iter->second);
    const BrainModelPriv& myModel = m_modelsInfo[iter->second];
    int64_t numUsed = (int64_t)myModel.m_nodeIndices.size();
    ret.resize(numUsed);
    for (int64_t i = 0; i < numUsed; ++i)
    {
        ret[i].m_ciftiIndex = myModel.m_modelStart + i;
        ret[i].m_surfaceNode = myModel.m_nodeIndices[i];
    }
    return ret;
}

int64_t CiftiBrainModelsMap::getSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    if (iter == m_surfUsed.end())
    {
        return -1;
    }
    CaretAssertVectorIndex(m_modelsInfo, iter->second);
    const BrainModelPriv& myModel = m_modelsInfo[iter->second];
    return myModel.m_surfaceNumberOfNodes;
}

vector<StructureEnum::Enum> CiftiBrainModelsMap::getSurfaceStructureList() const
{
    vector<StructureEnum::Enum> ret;
    ret.reserve(m_surfUsed.size());//we can use this to tell us how many there are, but it has reordered them
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)//we need them in the order they occur in
    {
        if (m_modelsInfo[i].m_type == SURFACE)
        {
            ret.push_back(m_modelsInfo[i].m_brainStructure);
        }
    }
    return ret;
}

bool CiftiBrainModelsMap::hasSurfaceData(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_surfUsed.find(structure);
    return (iter != m_surfUsed.end());
}

vector<CiftiBrainModelsMap::VolumeMap> CiftiBrainModelsMap::getFullVolumeMap() const
{
    vector<VolumeMap> ret;
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)
    {
        if (m_modelsInfo[i].m_type == VOXELS)
        {
            const BrainModelPriv& myModel = m_modelsInfo[i];
            int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
            CaretAssert(listSize % 3 == 0);
            int64_t numUsed = listSize / 3;
            if (ret.size() == 0) ret.reserve(numUsed);//keep it from doing multiple expansion copies on the first model
            for (int64_t i = 0; i < numUsed; ++i)
            {
                int64_t i3 = i * 3;
                VolumeMap temp;
                temp.m_ciftiIndex = myModel.m_modelStart + i;
                temp.m_ijk[0] = myModel.m_voxelIndicesIJK[i3];
                temp.m_ijk[1] = myModel.m_voxelIndicesIJK[i3 + 1];
                temp.m_ijk[2] = myModel.m_voxelIndicesIJK[i3 + 2];
                ret.push_back(temp);
            }
        }
    }
    return ret;
}

const VolumeSpace& CiftiBrainModelsMap::getVolumeSpace() const
{
    CaretAssert(!m_ignoreVolSpace);
    if (!m_haveVolumeSpace)
    {
        throw DataFileException("getVolumeSpace called when no volume space exists");
    }
    return m_volSpace;
}

vector<StructureEnum::Enum> CiftiBrainModelsMap::getVolumeStructureList() const
{
    vector<StructureEnum::Enum> ret;
    ret.reserve(m_volUsed.size());//we can use this to tell us how many there are, but it has reordered them
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)//we need them in the order they occur in
    {
        if (m_modelsInfo[i].m_type == VOXELS)
        {
            ret.push_back(m_modelsInfo[i].m_brainStructure);
        }
    }
    return ret;
}

vector<CiftiBrainModelsMap::VolumeMap> CiftiBrainModelsMap::getVolumeStructureMap(const StructureEnum::Enum& structure) const
{
    vector<VolumeMap> ret;
    map<StructureEnum::Enum, int>::const_iterator iter = m_volUsed.find(structure);
    if (iter == m_volUsed.end())
    {
        throw DataFileException("getVolumeStructureMap called for nonexistant structure");//also throw, for consistency
    }
    CaretAssertVectorIndex(m_modelsInfo, iter->second);
    const BrainModelPriv& myModel = m_modelsInfo[iter->second];
    int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
    CaretAssert(listSize % 3 == 0);
    int64_t numUsed = listSize / 3;
    ret.resize(numUsed);
    for (int64_t i = 0; i < numUsed; ++i)
    {
        int64_t i3 = i * 3;
        ret[i].m_ciftiIndex = myModel.m_modelStart + i;
        ret[i].m_ijk[0] = myModel.m_voxelIndicesIJK[i3];
        ret[i].m_ijk[1] = myModel.m_voxelIndicesIJK[i3 + 1];
        ret[i].m_ijk[2] = myModel.m_voxelIndicesIJK[i3 + 2];
    }
    return ret;
}

const vector<int64_t>& CiftiBrainModelsMap::getVoxelList(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_volUsed.find(structure);
    if (iter == m_volUsed.end())
    {
        throw DataFileException("getVoxelList called for nonexistant structure");//throw if it doesn't exist, because we don't have a reference to return - things should identify which structures exist before calling this
    }
    CaretAssertVectorIndex(m_modelsInfo, iter->second);
    return m_modelsInfo[iter->second].m_voxelIndicesIJK;
}

bool CiftiBrainModelsMap::hasVolumeData() const
{
    return (m_volUsed.size() != 0);
}

bool CiftiBrainModelsMap::hasVolumeData(const StructureEnum::Enum& structure) const
{
    map<StructureEnum::Enum, int>::const_iterator iter = m_volUsed.find(structure);
    return (iter != m_volUsed.end());
}

void CiftiBrainModelsMap::setVolumeSpace(const VolumeSpace& space)
{
    for (map<StructureEnum::Enum, int>::const_iterator iter = m_volUsed.begin(); iter != m_volUsed.end(); ++iter)//the main time this loop isn't empty is parsing cifti-1
    {
        CaretAssertVectorIndex(m_modelsInfo, iter->second);
        const BrainModelPriv& myModel = m_modelsInfo[iter->second];
        int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
        CaretAssert(listSize % 3 == 0);
        for (int64_t i3 = 0; i3 < listSize; i3 += 3)
        {
            if (!space.indexValid(myModel.m_voxelIndicesIJK[i3], myModel.m_voxelIndicesIJK[i3 + 1], myModel.m_voxelIndicesIJK[i3 + 2]))
            {
                throw DataFileException("invalid voxel found for volume space");
            }
        }
    }
    m_ignoreVolSpace = false;
    m_haveVolumeSpace = true;
    m_volSpace = space;
}

bool CiftiBrainModelsMap::operator==(const CiftiMappingType& rhs) const
{
    if (rhs.getType() != getType()) return false;
    const CiftiBrainModelsMap& myrhs = dynamic_cast<const CiftiBrainModelsMap&>(rhs);
    CaretAssert(!m_ignoreVolSpace && !myrhs.m_ignoreVolSpace);//these should only be true while in the process of parsing cifti-1, never otherwise
    if (m_haveVolumeSpace != myrhs.m_haveVolumeSpace) return false;
    if (m_haveVolumeSpace && (m_volSpace != myrhs.m_volSpace)) return false;
    return (m_modelsInfo == myrhs.m_modelsInfo);//NOTE: these are sorted by index range, so this works
}

bool CiftiBrainModelsMap::approximateMatch(const CiftiMappingType& rhs, QString* explanation) const
{
    if (rhs.getType() != getType())
    {
        if (explanation != NULL) *explanation = CiftiMappingType::mappingTypeToName(rhs.getType()) + " mapping never matches " + CiftiMappingType::mappingTypeToName(getType());
        return false;
    }
    const CiftiBrainModelsMap& myrhs = dynamic_cast<const CiftiBrainModelsMap&>(rhs);//there is no user-specified metadata, but we want informative messages, so copy and modify the code from ==
    CaretAssert(!m_ignoreVolSpace && !myrhs.m_ignoreVolSpace);//these should only be true while in the process of parsing cifti-1, never otherwise
    if (m_haveVolumeSpace != myrhs.m_haveVolumeSpace)
    {
        if (explanation != NULL) *explanation = "one of the mappings has no volume data";
        return false;
    }
    if (m_haveVolumeSpace && (m_volSpace != myrhs.m_volSpace))
    {
        if (explanation != NULL) *explanation = "mappings have a different volume space";
        return false;
    }
    if (m_modelsInfo != myrhs.m_modelsInfo)
    {
        if (explanation != NULL) *explanation = "mappings include different brainordinates";
        return false;
    }
    return true;
}

/**
 * @return EQUAL if 'this' and 'rhs' contain the same models
 *         SUBSET if each model in 'this' is contained in 'rhs' and 'rhs' contains models not in 'this'
 *         NO if 'this' contains a model not in 'rhs'
 */
CiftiBrainModelsMap::MatchResult CiftiBrainModelsMap::testMatch(const CiftiMappingType& rhs) const
{
    if (rhs.getType() != getType()) return CiftiBrainModelsMap::MatchResult::NO;
    const CiftiBrainModelsMap& myrhs = dynamic_cast<const CiftiBrainModelsMap&>(rhs);
    CaretAssert(!m_ignoreVolSpace && !myrhs.m_ignoreVolSpace);//these should only be true while in the process of parsing cifti-1, never otherwise
    if (m_haveVolumeSpace
        && ( ! myrhs.m_haveVolumeSpace)) return CiftiBrainModelsMap::MatchResult::NO;
    if (m_haveVolumeSpace && (m_volSpace != myrhs.m_volSpace)) return CiftiBrainModelsMap::MatchResult::NO;

    for (const auto& modelInfo : m_modelsInfo) {
        bool matched = false;
        for (const auto& rhsModelInfo : myrhs.m_modelsInfo) {
            if (modelInfo == rhsModelInfo) {
                matched = true;
                break;
            }
        }        
        if (! matched) {
            return CiftiBrainModelsMap::MatchResult::NO;
        }
    }
    
    if (m_modelsInfo.size() == myrhs.m_modelsInfo.size()) {
        /* 
         * A note in the equality operator indicates that models are
         * sorted by index range so if 'this' and 'rhs' have the
         * exact same models, they must be equal.  The assertion
         * will fail if this wrong.
         */
        CaretAssert(*this == rhs);
        return CiftiBrainModelsMap::MatchResult::EQUAL;
    }
    return CiftiBrainModelsMap::MatchResult::SUBSET;
}

bool CiftiBrainModelsMap::BrainModelPriv::operator==(const BrainModelPriv& rhs) const
{
    if (m_brainStructure != rhs.m_brainStructure) return false;
    if (m_type != rhs.m_type) return false;
    if (m_modelStart != rhs.m_modelStart) return false;
    if (m_modelEnd != rhs.m_modelEnd) return false;
    if (m_type == SURFACE)
    {
        if (m_surfaceNumberOfNodes != rhs.m_surfaceNumberOfNodes) return false;
        int64_t listSize = (int64_t)m_nodeIndices.size();
        CaretAssert((int64_t)rhs.m_nodeIndices.size() == listSize);//this should already be checked by start/end above
        for (int64_t i = 0; i < listSize; ++i)
        {
            if (m_nodeIndices[i] != rhs.m_nodeIndices[i]) return false;
        }
    } else {
        int64_t listSize = (int64_t)m_voxelIndicesIJK.size();
        CaretAssert((int64_t)rhs.m_voxelIndicesIJK.size() == listSize);//this should already be checked by start/end above
        for (int64_t i = 0; i < listSize; ++i)
        {
            if (m_voxelIndicesIJK[i] != rhs.m_voxelIndicesIJK[i]) return false;
        }
    }
    return true;
}

void CiftiBrainModelsMap::readXML1(QXmlStreamReader& xml)
{
    clear();
    m_ignoreVolSpace = true;//because in cifti-1, the volume space is not in this element - so, we rely on CiftiXML to check for volume data, and set the volume space afterwards
    vector<ParseHelperModel> parsedModels;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                auto name = xml.name();
                if (name != QLatin1String("BrainModel"))
                {
                    throw DataFileException("unexpected element in brain models map: " + name.toString());
                }
                ParseHelperModel thisModel;
                thisModel.parseBrainModel1(xml);
                if (xml.hasError()) return;
                parsedModels.push_back(thisModel);
                break;//the readNext in the for will remove the BrainModel end element
            }
            default:
                break;
        }
    }
    if (xml.hasError()) return;
    sort(parsedModels.begin(), parsedModels.end());
    int64_t numModels = (int64_t)parsedModels.size();//because we haven't checked them for unique values of BrainStructure yet...yeah, its paranoid
    int64_t curOffset = 0;
    for (int64_t i = 0; i < numModels; ++i)
    {
        if (parsedModels[i].m_offset != curOffset)
        {
            if (parsedModels[i].m_offset < curOffset)
            {
                throw DataFileException("models overlap at index " + QString::number(parsedModels[i].m_offset) + ", model " + QString::number(i));
            } else {
                throw DataFileException("index " + QString::number(curOffset) + " is not assigned to any model");
            }
        }
        curOffset += parsedModels[i].m_count;
    }
    for (int64_t i = 0; i < numModels; ++i)
    {
        if (parsedModels[i].m_type == SURFACE)
        {
            addSurfaceModel(parsedModels[i].m_surfaceNumberOfNodes,
                            parsedModels[i].m_brainStructure,
                            parsedModels[i].m_nodeIndices);
        } else {
            addVolumeModel(parsedModels[i].m_brainStructure,
                           parsedModels[i].m_voxelIndicesIJK);
        }
    }
    m_ignoreVolSpace = false;//in case there are no voxels, but some will be added later
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("MatrixIndicesMap"));
}

void CiftiBrainModelsMap::readXML2(QXmlStreamReader& xml)
{
    clear();
    vector<ParseHelperModel> parsedModels;
    for (xml.readNext(); !xml.atEnd() && !xml.isEndElement(); xml.readNext())
    {
        switch (xml.tokenType())
        {
            case QXmlStreamReader::StartElement:
            {
                auto name = xml.name();
                if (name == QLatin1String("BrainModel"))
                {
                    ParseHelperModel thisModel;
                    thisModel.parseBrainModel2(xml);
                    if (xml.hasError()) break;
                    parsedModels.push_back(thisModel);
                } else if (name == QLatin1String("Volume")) {
                    if (m_haveVolumeSpace)
                    {
                        throw DataFileException("Volume specified more than once in Brain Models mapping type");
                    } else {
                        m_volSpace.readCiftiXML2(xml);
                        if (xml.hasError()) return;
                        m_haveVolumeSpace = true;
                    }
                } else {
                    throw DataFileException("unexpected element in brain models map: " + name.toString());
                }
                break;//the readNext in the for will remove the BrainModel or Volume end element
            }
            default:
                break;
        }
    }
    if (xml.hasError()) return;
    sort(parsedModels.begin(), parsedModels.end());
    int64_t numModels = (int64_t)parsedModels.size();//because we haven't checked them for unique values of BrainStructure yet...yeah, its paranoid
    int64_t curOffset = 0;
    for (int64_t i = 0; i < numModels; ++i)
    {
        if (parsedModels[i].m_offset != curOffset)
        {
            if (parsedModels[i].m_offset < curOffset)
            {
                throw DataFileException("models overlap at index " + QString::number(parsedModels[i].m_offset) + ", model " + QString::number(i));
            } else {
                throw DataFileException("index " + QString::number(curOffset) + " is not assigned to any model");
            }
        }
        curOffset += parsedModels[i].m_count;
    }
    for (int64_t i = 0; i < numModels; ++i)
    {
        if (parsedModels[i].m_type == SURFACE)
        {
            addSurfaceModel(parsedModels[i].m_surfaceNumberOfNodes,
                            parsedModels[i].m_brainStructure,
                            parsedModels[i].m_nodeIndices);
        } else {
            addVolumeModel(parsedModels[i].m_brainStructure,
                           parsedModels[i].m_voxelIndicesIJK);
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("MatrixIndicesMap"));
}

void CiftiBrainModelsMap::ParseHelperModel::parseBrainModel1(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attrs = xml.attributes();
    if (!attrs.hasAttribute("ModelType"))
    {
        throw DataFileException("BrainModel missing required attribute ModelType");
    }
    auto value = attrs.value("ModelType");
    if (value == QLatin1String("CIFTI_MODEL_TYPE_SURFACE"))
    {
        m_type = SURFACE;
    } else if (value == QLatin1String("CIFTI_MODEL_TYPE_VOXELS")) {
        m_type = VOXELS;
    } else {
        throw DataFileException("invalid value for ModelType: " + value.toString());
    }
    if (!attrs.hasAttribute("BrainStructure"))
    {
        throw DataFileException("BrainModel missing required attribute BrainStructure");
    }
    value = attrs.value("BrainStructure");
    bool ok = false;
    m_brainStructure = StructureEnum::fromCiftiName(value.toString(), &ok);
    if (!ok)
    {
        throw DataFileException("invalid value for BrainStructure: " + value.toString());
    }
    if (!attrs.hasAttribute("IndexOffset"))
    {
        throw DataFileException("BrainModel missing required attribute IndexOffset");
    }
    value = attrs.value("IndexOffset");
    m_offset = value.toString().toLongLong(&ok);
    if (!ok || m_offset < 0)
    {
        throw DataFileException("IndexOffset must be a non-negative integer");
    }
    if (!attrs.hasAttribute("IndexCount"))
    {
        throw DataFileException("BrainModel missing required attribute IndexCount");
    }
    value = attrs.value("IndexCount");
    m_count = value.toString().toLongLong(&ok);
    if (!ok || m_count < 1)//NOTE: not technically required by cifti-1, would need some rewriting to support empty brainmodels
    {
        throw DataFileException("IndexCount must be a positive integer");
    }
    if (m_type == SURFACE)
    {
        if (!attrs.hasAttribute("SurfaceNumberOfNodes"))
        {
            throw DataFileException("BrainModel missing required attribute SurfaceNumberOfNodes");
        }
        value = attrs.value("SurfaceNumberOfNodes");
        m_surfaceNumberOfNodes = value.toString().toLongLong(&ok);
        if (!ok || m_surfaceNumberOfNodes < 1)
        {
            throw DataFileException("SurfaceNumberOfNodes must be a positive integer");
        }
        if (!xml.readNextStartElement())//special case in cifti-1
        {
            m_nodeIndices.resize(m_count);
            for (int64_t i = 0; i < m_count; ++i)
            {
                m_nodeIndices[i] = i;
            }
        } else {
            if (xml.name() != QLatin1String("NodeIndices"))
            {
                throw DataFileException("unexpected element in BrainModel of SURFACE type: " + xml.name().toString());
            }
            m_nodeIndices = readIndexArray(xml);
            xml.readNext();//remove the end element of NodeIndices
        }
        if (xml.hasError()) return;
        if ((int64_t)m_nodeIndices.size() != m_count)
        {
            throw DataFileException("number of vertex indices does not match IndexCount");
        }
    } else {
        if (!xml.readNextStartElement())
        {
            throw DataFileException("BrainModel requires a child element");
        }
        if (xml.name() != QLatin1String("VoxelIndicesIJK"))
        {
            throw DataFileException("unexpected element in BrainModel of VOXELS type: " + xml.name().toString());
        }
        m_voxelIndicesIJK = readIndexArray(xml);
        if (xml.hasError()) return;
        if (m_voxelIndicesIJK.size() % 3 != 0)
        {
            throw DataFileException("number of voxel indices is not a multiple of 3");
        }
        if ((int64_t)m_voxelIndicesIJK.size() != m_count * 3)
        {
            throw DataFileException("number of voxel indices does not match IndexCount");
        }
        xml.readNext();//remove the end element of VoxelIndicesIJK
    }
    while (!xml.atEnd() && !xml.isEndElement())//locate the end element of BrainModel
    {
        switch(xml.readNext())
        {
            case QXmlStreamReader::StartElement:
                throw DataFileException("unexpected second element in BrainModel: " + xml.name().toString());
            default:
                break;
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("BrainModel"));
}

void CiftiBrainModelsMap::ParseHelperModel::parseBrainModel2(QXmlStreamReader& xml)
{
    QXmlStreamAttributes attrs = xml.attributes();
    if (!attrs.hasAttribute("ModelType"))
    {
        throw DataFileException("BrainModel missing required attribute ModelType");
    }
    auto value = attrs.value("ModelType");
    if (value == QLatin1String("CIFTI_MODEL_TYPE_SURFACE"))
    {
        m_type = SURFACE;
    } else if (value == QLatin1String("CIFTI_MODEL_TYPE_VOXELS")) {
        m_type = VOXELS;
    } else {
        throw DataFileException("invalid value for ModelType: " + value.toString());
    }
    if (!attrs.hasAttribute("BrainStructure"))
    {
        throw DataFileException("BrainModel missing required attribute BrainStructure");
    }
    value = attrs.value("BrainStructure");
    bool ok = false;
    m_brainStructure = StructureEnum::fromCiftiName(value.toString(), &ok);
    if (!ok)
    {
        throw DataFileException("invalid value for BrainStructure: " + value.toString());
    }
    if (!attrs.hasAttribute("IndexOffset"))
    {
        throw DataFileException("BrainModel missing required attribute IndexOffset");
    }
    value = attrs.value("IndexOffset");
    m_offset = value.toString().toLongLong(&ok);
    if (!ok || m_offset < 0)
    {
        throw DataFileException("IndexOffset must be a non-negative integer");
    }
    if (!attrs.hasAttribute("IndexCount"))
    {
        throw DataFileException("BrainModel missing required attribute IndexCount");
    }
    value = attrs.value("IndexCount");
    m_count = value.toString().toLongLong(&ok);
    if (!ok || m_count < 1)
    {
        throw DataFileException("IndexCount must be a positive integer");
    }
    if (m_type == SURFACE)
    {
        if (!attrs.hasAttribute("SurfaceNumberOfVertices"))
        {
            throw DataFileException("BrainModel missing required attribute SurfaceNumberOfVertices");
        }
        value = attrs.value("SurfaceNumberOfVertices");
        m_surfaceNumberOfNodes = value.toString().toLongLong(&ok);
        if (!ok || m_surfaceNumberOfNodes < 1)
        {
            throw DataFileException("SurfaceNumberOfVertices must be a positive integer");
        }
        if (!xml.readNextStartElement())
        {
            throw DataFileException("BrainModel requires a child element");
        }
        if (xml.name() != QLatin1String("VertexIndices"))
        {
            throw DataFileException("unexpected element in BrainModel of SURFACE type: " + xml.name().toString());
        }
        m_nodeIndices = readIndexArray(xml);
        if (xml.hasError()) return;
        if ((int64_t)m_nodeIndices.size() != m_count)
        {
            throw DataFileException("number of vertex indices does not match IndexCount");
        }
        xml.readNext();//remove the end element of NodeIndices
    } else {
        if (!xml.readNextStartElement())
        {
            throw DataFileException("BrainModel requires a child element");
        }
        if (xml.name() != QLatin1String("VoxelIndicesIJK"))
        {
            throw DataFileException("unexpected element in BrainModel of VOXELS type: " + xml.name().toString());
        }
        m_voxelIndicesIJK = readIndexArray(xml);
        if (xml.hasError()) return;
        if (m_voxelIndicesIJK.size() % 3 != 0)
        {
            throw DataFileException("number of voxel indices is not a multiple of 3");
        }
        if ((int64_t)m_voxelIndicesIJK.size() != m_count * 3)
        {
            throw DataFileException("number of voxel indices does not match IndexCount");
        }
        xml.readNext();//remove the end element of VoxelIndicesIJK
    }
    while (!xml.atEnd() && !xml.isEndElement())//locate the end element of BrainModel
    {
        switch(xml.readNext())
        {
            case QXmlStreamReader::StartElement:
                throw DataFileException("unexpected second element in BrainModel: " + xml.name().toString());
            default:
                break;
        }
    }
    CaretAssert(xml.isEndElement() && xml.name() == QLatin1String("BrainModel"));
}

vector<int64_t> CiftiBrainModelsMap::ParseHelperModel::readIndexArray(QXmlStreamReader& xml)
{
    vector<int64_t> ret;
    QString text = xml.readElementText();//raises error if it encounters a start element
    if (xml.hasError()) return ret;
#if QT_VERSION >= 0x060000
    auto separated = text.tokenize(QRegularExpression("\\s+"), Qt::SkipEmptyParts);
    //can't do an exact reserve, tokenize is one at a time, there is no splitRef in qt6
#else
    //in QT5, QRegExp is slightly faster
    auto separated = text.splitRef(QRegExp("\\s+"), QString::SkipEmptyParts);
    ret.reserve(separated.size());
#endif
    for (const auto& elem : separated)
    {
        bool ok = false;
        ret.push_back(elem.toLongLong(&ok));
        if (!ok)
        {
            throw DataFileException("found noninteger in index array: " + elem.toString());
        }
        if (ret.back() < 0)
        {
            throw DataFileException("found negative integer in index array: " + elem.toString());
        }
    }
    ret.shrink_to_fit();//qt6 can't predict in advance without making QStrings, so try not to have a bunch of unused memory allocated
    return ret;
}

void CiftiBrainModelsMap::writeXML1(QXmlStreamWriter& xml) const
{
    CaretAssert(!m_ignoreVolSpace);
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_BRAIN_MODELS");
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)
    {
        const BrainModelPriv& myModel = m_modelsInfo[i];
        xml.writeStartElement("BrainModel");
        xml.writeAttribute("IndexOffset", QString::number(myModel.m_modelStart));
        xml.writeAttribute("IndexCount", QString::number(myModel.m_modelEnd - myModel.m_modelStart));
        xml.writeAttribute("BrainStructure", StructureEnum::toCiftiName(myModel.m_brainStructure));
        if (myModel.m_type == SURFACE)
        {
            xml.writeAttribute("ModelType", "CIFTI_MODEL_TYPE_SURFACE");
            xml.writeAttribute("SurfaceNumberOfNodes", QString::number(myModel.m_surfaceNumberOfNodes));
            xml.writeStartElement("NodeIndices");
            QString text = "";
            int64_t numNodes = (int64_t)myModel.m_nodeIndices.size();
            for (int64_t j = 0; j < numNodes; ++j)
            {
                if (j != 0) text += " ";
                text += QString::number(myModel.m_nodeIndices[j]);
            }
            xml.writeCharacters(text);
            xml.writeEndElement();
        } else {
            xml.writeAttribute("ModelType", "CIFTI_MODEL_TYPE_VOXELS");
            xml.writeStartElement("VoxelIndicesIJK");
            QString text = "";
            int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
            CaretAssert(listSize % 3 == 0);
            for (int64_t j = 0; j < listSize; j += 3)
            {
                text += QString::number(myModel.m_voxelIndicesIJK[j]) + " " + QString::number(myModel.m_voxelIndicesIJK[j + 1]) + " " + QString::number(myModel.m_voxelIndicesIJK[j + 2]) + "\n";
            }
            xml.writeCharacters(text);
            xml.writeEndElement();
        }
        xml.writeEndElement();
    }
}

void CiftiBrainModelsMap::writeXML2(QXmlStreamWriter& xml) const
{
    CaretAssert(!m_ignoreVolSpace);
    xml.writeAttribute("IndicesMapToDataType", "CIFTI_INDEX_TYPE_BRAIN_MODELS");
    if (hasVolumeData())//could be m_haveVolumeSpace if we want to be able to write a volspace without having voxels, but that seems silly
    {
        m_volSpace.writeCiftiXML2(xml);
    }
    int numModels = (int)m_modelsInfo.size();
    for (int i = 0; i < numModels; ++i)
    {
        const BrainModelPriv& myModel = m_modelsInfo[i];
        xml.writeStartElement("BrainModel");
        xml.writeAttribute("IndexOffset", QString::number(myModel.m_modelStart));
        xml.writeAttribute("IndexCount", QString::number(myModel.m_modelEnd - myModel.m_modelStart));
        xml.writeAttribute("BrainStructure", StructureEnum::toCiftiName(myModel.m_brainStructure));
        if (myModel.m_type == SURFACE)
        {
            xml.writeAttribute("ModelType", "CIFTI_MODEL_TYPE_SURFACE");
            xml.writeAttribute("SurfaceNumberOfVertices", QString::number(myModel.m_surfaceNumberOfNodes));
            xml.writeStartElement("VertexIndices");
            QString text = "";
            int64_t numNodes = (int64_t)myModel.m_nodeIndices.size();
            for (int64_t j = 0; j < numNodes; ++j)
            {
                if (j != 0) text += " ";
                text += QString::number(myModel.m_nodeIndices[j]);
            }
            xml.writeCharacters(text);
            xml.writeEndElement();
        } else {
            xml.writeAttribute("ModelType", "CIFTI_MODEL_TYPE_VOXELS");
            xml.writeStartElement("VoxelIndicesIJK");
            QString text = "";
            int64_t listSize = (int64_t)myModel.m_voxelIndicesIJK.size();
            CaretAssert(listSize % 3 == 0);
            for (int64_t j = 0; j < listSize; j += 3)
            {
                text += QString::number(myModel.m_voxelIndicesIJK[j]) + " " + QString::number(myModel.m_voxelIndicesIJK[j + 1]) + " " + QString::number(myModel.m_voxelIndicesIJK[j + 2]) + "\n";
            }
            xml.writeCharacters(text);
            xml.writeEndElement();
        }
        xml.writeEndElement();
    }
}

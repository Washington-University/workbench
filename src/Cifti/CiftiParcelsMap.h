#ifndef __CIFTI_PARCELS_MAP_H__
#define __CIFTI_PARCELS_MAP_H__

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

#include "CiftiMappingType.h"

#include "CaretCompact3DLookup.h"
#include "StructureEnum.h"
#include "VolumeSpace.h"
#include "VoxelIJK.h"

#include <map>
#include <set>
#include <vector>

namespace caret
{
    class CiftiParcelsMap : public CiftiMappingType
    {
    public:
        struct Parcel
        {
            std::map<StructureEnum::Enum, std::set<int64_t> > m_surfaceNodes;
            std::set<VoxelIJK> m_voxelIndices;
            QString m_name;
            bool operator==(const Parcel& rhs) const;
            bool operator!=(const Parcel& rhs) const { return !((*this) == rhs); }
            bool approximateMatch(const Parcel& rhs, QString* explanation = NULL) const;
        };
        bool hasVolumeData() const;
        bool hasSurface(const StructureEnum::Enum& structure) const;//only checks whether surface has been added/read
        bool hasSurfaceData(const StructureEnum::Enum& structure) const;
        const VolumeSpace& getVolumeSpace() const;
        int64_t getSurfaceNumberOfNodes(const StructureEnum::Enum& structure) const;
        int64_t getIndexForNode(const int64_t& node, const StructureEnum::Enum& structure) const;
        int64_t getIndexForVoxel(const int64_t* ijk) const;
        int64_t getIndexForVoxel(const int64_t& i, const int64_t& j, const int64_t& k) const;
        std::vector<StructureEnum::Enum> getParcelSurfaceStructures() const;
        const std::vector<Parcel>& getParcels() const { return m_parcels; }
        int64_t getIndexFromNumberOrName(const QString& numberOrName) const;
        QString getIndexName(const int64_t& index) const;
        
        CiftiParcelsMap() { m_haveVolumeSpace = false; m_ignoreVolSpace = false; }
        void addSurface(const int64_t& numberOfNodes, const StructureEnum::Enum& structure);
        void setVolumeSpace(const VolumeSpace& space);
        void addParcel(const Parcel& parcel);
        void clear();
        
        CiftiMappingType* clone() const { return new CiftiParcelsMap(*this); }
        MappingType getType() const { return PARCELS; }
        int64_t getLength() const { return m_parcels.size(); }
        bool operator==(const CiftiMappingType& rhs) const;
        bool approximateMatch(const CiftiMappingType& rhs, QString* explanation = NULL) const;
        void readXML1(QXmlStreamReader& xml);
        void readXML2(QXmlStreamReader& xml);
        void writeXML1(QXmlStreamWriter& xml) const;
        void writeXML2(QXmlStreamWriter& xml) const;
    private:
        std::vector<Parcel> m_parcels;
        VolumeSpace m_volSpace;
        bool m_haveVolumeSpace, m_ignoreVolSpace;//second is needed for parsing cifti-1
        struct SurfaceInfo
        {
            int64_t m_numNodes;
            std::vector<int64_t> m_lookup;
        };
        CaretCompact3DLookup<int64_t> m_volLookup;
        std::map<StructureEnum::Enum, SurfaceInfo> m_surfInfo;
        static Parcel readParcel1(QXmlStreamReader& xml);
        static Parcel readParcel2(QXmlStreamReader& xml);
        static std::vector<int64_t> readIndexArray(QXmlStreamReader& xml);
    };
}

#endif //__CIFTI_PARCELS_MAP_H__

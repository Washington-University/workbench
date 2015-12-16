#ifndef __CIFTI_LABELS_MAP_H__
#define __CIFTI_LABELS_MAP_H__

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

#include "CaretPointer.h"
#include "GiftiMetaData.h"
#include "GiftiLabelTable.h"

#include <map>
#include <vector>

namespace caret
{
    class CiftiLabelsMap : public CiftiMappingType
    {
    public:
        CiftiLabelsMap();
        CiftiLabelsMap(const CiftiLabelsMap& rhs);
        CiftiLabelsMap& operator=(const CiftiLabelsMap& rhs);
        
        GiftiMetaData* getMapMetadata(const int64_t& index) const;//HACK: allow modification of label table and metadata within XML without setting the xml on a file again
        GiftiLabelTable* getMapLabelTable(const int64_t& index) const;
        const QString& getMapName(const int64_t& index) const;
        int64_t getIndexFromNumberOrName(const QString& numberOrName) const;
        QString getIndexName(const int64_t& index) const { return getMapName(index); }
        
        void setMapName(const int64_t& index, const QString& mapName) const;//HACK: ditto
        void setLength(const int64_t& length);
        void clear();
        
        CiftiMappingType* clone() const { return new CiftiLabelsMap(*this); }
        MappingType getType() const { return LABELS; }
        int64_t getLength() const { return m_maps.size(); }
        bool operator==(const CiftiMappingType& rhs) const;
        bool approximateMatch(const CiftiMappingType& rhs, QString* explanation = NULL) const;
        void readXML1(QXmlStreamReader& xml);
        void readXML2(QXmlStreamReader& xml);
        void writeXML1(QXmlStreamWriter& xml) const;
        void writeXML2(QXmlStreamWriter& xml) const;
        bool mutablesModified() const;
        void clearMutablesModified() const;
    private:
        mutable bool m_namesModified;
        struct LabelMap
        {
            mutable QString m_name;//we need a better way to change metadata in an in-memory file
            mutable GiftiMetaData m_metaData;//ditto
            mutable GiftiLabelTable m_labelTable;//ditto
            bool operator==(const LabelMap& rhs) const;
            void readXML1(QXmlStreamReader& xml);
            void readXML2(QXmlStreamReader& xml);
        };
        std::vector<LabelMap> m_maps;
    };
}

#endif //__CIFTI_LABELS_MAP_H__

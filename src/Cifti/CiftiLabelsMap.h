#ifndef __CIFTI_LABELS_MAP_H__
#define __CIFTI_LABELS_MAP_H__

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

#include "CiftiIndexMap.h"

#include "AString.h"
#include "CaretPointer.h"
#include "GiftiMetaData.h"
#include "GiftiLabelTable.h"

#include <map>
#include <vector>

namespace caret
{
    class CiftiLabelsMap : public CiftiIndexMap
    {
    public:
        GiftiMetaData* getMapMetadata(const int64_t& index) const;//HACK: allow modification of label table and metadata within XML without setting the xml on a file again
        GiftiLabelTable* getMapLabelTable(const int64_t& index) const;
        const AString& getMapName(const int64_t& index) const;
        
        void setMapName(const int64_t& index, const AString& mapName);
        void setLength(const int64_t& length);
        void clear();//do we need this?
        
        CiftiIndexMap* clone() const { return new CiftiLabelsMap(*this); }
        MappingType getType() const { return SCALARS; }
        int64_t getLength() const { return m_maps.size(); }
        bool operator==(const CiftiIndexMap& rhs) const;
        bool approximateMatch(const CiftiIndexMap& rhs) const;
        void readXML1(QXmlStreamReader& xml);
        void readXML2(QXmlStreamReader& xml);
        void writeXML1(QXmlStreamWriter& xml) const;
        void writeXML2(QXmlStreamWriter& xml) const;
    private:
        struct LabelMap
        {
            AString m_name;
            mutable GiftiMetaData m_metaData;//we need a better way to change metadata in an in-memory file
            mutable GiftiLabelTable m_labelTable;//ditto
            bool operator==(const LabelMap& rhs) const;
            void readXML1(QXmlStreamReader& xml);
            void readXML2(QXmlStreamReader& xml);
        };
        std::vector<LabelMap> m_maps;
    };
}

#endif //__CIFTI_LABELS_MAP_H__

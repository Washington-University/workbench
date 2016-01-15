#ifndef __CIFTI_SERIES_MAP_H__
#define __CIFTI_SERIES_MAP_H__

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

namespace caret
{
    class CiftiSeriesMap : public CiftiMappingType
    {
    public:
        enum Unit
        {
            HERTZ,
            METER,
            RADIAN,
            SECOND
        };//should this go somewhere else?
        float getStart() const { return m_start; }//using getter/setter as style choice to match other mapping types
        float getStep() const { return m_step; }//getter for number of series points is getLength(), specified by CiftiIndexMap
        Unit getUnit() const { return m_unit; }
        
        CiftiSeriesMap()
        {
            m_start = 0.0f;
            m_step = 1.0f;
            m_unit = SECOND;
            m_length = -1;//to make it clear an improperly initialized series map object was used
        }
        CiftiSeriesMap(const int64_t& length, const float& start = 0.0f, const float& step = 1.0f, const Unit& unit = SECOND)
        {
            m_start = start;
            m_step = step;
            m_unit = unit;
            m_length = length;
        }
        void setStart(const float& start) { m_start = start; }
        void setStep(const float& step) { m_step = step; }
        void setUnit(const Unit& unit) { m_unit = unit; }
        void setLength(const int64_t& length);
        
        static Unit stringToUnit(const QString& string, bool& ok);
        static QString unitToString(const Unit& theUnit);
        static std::vector<Unit> getAllUnits();
        
        CiftiMappingType* clone() const { return new CiftiSeriesMap(*this); }
        MappingType getType() const { return SERIES; }
        int64_t getLength() const { return m_length; }
        bool operator==(const CiftiMappingType& rhs) const
        {
            if (rhs.getType() != getType()) return false;
            const CiftiSeriesMap& temp = dynamic_cast<const CiftiSeriesMap&>(rhs);
            return (temp.m_length == m_length &&
                    temp.m_unit == m_unit &&
                    temp.m_start == m_start &&
                    temp.m_step == m_step);
        }
        bool approximateMatch(const CiftiMappingType& rhs, QString* explanation = NULL) const
        {
            switch (rhs.getType())
            {
                case SCALARS://maybe?
                case SERIES:
                case LABELS://maybe?
                    if (getLength() != rhs.getLength())
                    {
                        if (explanation != NULL) *explanation = "mappings have different length";
                        return false;
                    } else return true;
                default:
                    if (explanation != NULL) *explanation = CiftiMappingType::mappingTypeToName(rhs.getType()) + " mapping never matches " + CiftiMappingType::mappingTypeToName(getType());
                    return false;
            }
        }
        void readXML1(QXmlStreamReader& xml);
        void readXML2(QXmlStreamReader& xml);
        void writeXML1(QXmlStreamWriter& xml) const;
        void writeXML2(QXmlStreamWriter& xml) const;
    private:
        int64_t m_length;
        float m_start, m_step;//exponent gets applied to these on reading
        Unit m_unit;
    };
}

#endif //__CIFTI_SERIES_MAP_H__

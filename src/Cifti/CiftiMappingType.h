#ifndef __CIFTI_MAPPING_TYPE_H__
#define __CIFTI_MAPPING_TYPE_H__

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

#include "stdint.h"

#include <QString>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

namespace caret
{
    class CiftiMappingType
    {
    public:
        enum MappingType
        {
            BRAIN_MODELS = 1,//compatibility values with old XML enum, in case someone uses the wrong enum
            PARCELS = 3,//fibers doesn't exist in 2.0
            SERIES = 4,
            SCALARS = 5,
            LABELS = 6
        };
        virtual CiftiMappingType* clone() const = 0;//make a copy, preserving the actual type - NOTE: this returns a dynamic allocation that is not owned by anything
        virtual MappingType getType() const = 0;
        virtual int64_t getLength() const = 0;
        virtual int64_t getIndexFromNumberOrName(const QString& numberOrName) const;
        virtual QString getIndexName(const int64_t& index) const;
        virtual bool operator==(const CiftiMappingType& rhs) const = 0;//used to check for merging mappings when writing the XML - must compare EVERYTHING that goes into the XML
        bool operator!=(const CiftiMappingType& rhs) const { return !((*this) == rhs); }
        virtual bool approximateMatch(const CiftiMappingType& rhs, QString* explanation = NULL) const = 0;//check if things like doing index-wise math would make sense
        virtual void readXML1(QXmlStreamReader& xml) = 0;//mainly to shorten the type-specific code in CiftiXML
        virtual void readXML2(QXmlStreamReader& xml) = 0;
        virtual void writeXML1(QXmlStreamWriter& xml) const = 0;
        virtual void writeXML2(QXmlStreamWriter& xml) const = 0;
        virtual bool mutablesModified() const;
        virtual void clearMutablesModified() const;//HACK: clear modified status on a const object
        virtual ~CiftiMappingType();
        
        static QString mappingTypeToName(const MappingType& type);
    };
}

#endif //__CIFTI_MAPPING_TYPE_H__

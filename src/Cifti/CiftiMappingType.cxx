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

#include "CaretAssert.h"

using namespace caret;

CiftiMappingType::~CiftiMappingType()
{//to ensure that the class's vtable gets defined in an object file
}

int64_t CiftiMappingType::getIndexFromNumberOrName(const QString& numberOrName) const
{
    bool ok = false;
    int64_t ret = numberOrName.toLongLong(&ok) - 1;//quirk: use string "1" as the first index
    if (!ok) return -1;
    if (ret < 0 || ret >= getLength()) return -1;
    return ret;
}

QString CiftiMappingType::getIndexName(const int64_t&) const
{
    return "";
}

QString CiftiMappingType::mappingTypeToName(const CiftiMappingType::MappingType& type)
{
    switch (type)
    {
        case BRAIN_MODELS:
            return "BRAIN_MODELS";
        case PARCELS:
            return "PARCELS";
        case SERIES:
            return "SERIES";
        case SCALARS:
            return "SCALARS";
        case LABELS:
            return "LABELS";
    }
    CaretAssert(0);
    return "";
}

bool CiftiMappingType::mutablesModified() const
{
    return false;
}

void CiftiMappingType::clearMutablesModified() const
{
    //nothing
}

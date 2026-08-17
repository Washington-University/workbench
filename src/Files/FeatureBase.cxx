
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __FEATURE_BASE_DECLARE__
#include "FeatureBase.h"
#undef __FEATURE_BASE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::FeatureBase 
 * \brief Base class for some Feature classes that are placed in Qt model classes
 * \ingroup Files
 */

/**
 * Constructor.
 * @param baseType
 *   The base type
 */
FeatureBase::FeatureBase(const BaseType baseType)
: QStandardItem(),
m_baseType(baseType)
{
    
}

/**
 * Destructor.
 */
FeatureBase::~FeatureBase()
{
}

/**
 * @return The base type
 */
FeatureBase::BaseType
FeatureBase::getBaseType() const
{
    return m_baseType;
}


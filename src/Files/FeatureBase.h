#ifndef __FEATURE_BASE_H__
#define __FEATURE_BASE_H__

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



#include <memory>

#include <QStandardItem>

#include "AString.h"

namespace caret {

    class FeatureBase : public QStandardItem {
        
    public:
        enum class BaseType {
            FEATURE_ITEM,
            FEATURE_LABEL,
            FEATURE_PROPERTY
        };
        
        FeatureBase(const BaseType baseType);
        
        virtual ~FeatureBase();
        
        FeatureBase(const FeatureBase&) = delete;

        FeatureBase& operator=(const FeatureBase&) = delete;
        
        BaseType getBaseType() const;

        virtual AString toString() const = 0;
        
        // ADD_NEW_METHODS_HERE

    private:
        const BaseType m_baseType;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FEATURE_BASE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FEATURE_BASE_DECLARE__

} // namespace
#endif  //__FEATURE_BASE_H__

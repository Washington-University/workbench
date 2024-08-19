#ifndef __OME_AXIS_H__
#define __OME_AXIS_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#include "CaretObject.h"
#include "OmeAxisTypeEnum.h"
#include "OmeSpaceUnitEnum.h"
#include "OmeTimeUnitEnum.h"

namespace caret {

    class OmeAxis : public CaretObject {
        
    public:
        OmeAxis();
        
        virtual ~OmeAxis();
        
        OmeAxis(const OmeAxis& obj);

        OmeAxis& operator=(const OmeAxis& obj);
        
        OmeAxisTypeEnum::Enum getAxisType() const;
        
        void setAxisType(const OmeAxisTypeEnum::Enum axisType);
        
        OmeSpaceUnitEnum::Enum getSpaceUnit() const;
        
        void setSpaceUnit(const OmeSpaceUnitEnum::Enum spaceUnit);
        
        OmeTimeUnitEnum::Enum getTimeUnit() const;
        
        void setTimeUnit(const OmeTimeUnitEnum::Enum timeUnit);
        
        QString getName() const;
        
        void setName(const QString& name);
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperOmeAxis(const OmeAxis& obj);

        OmeAxisTypeEnum::Enum m_axisType = OmeAxisTypeEnum::UNKNOWN;
        
        OmeSpaceUnitEnum::Enum m_spaceUnit = OmeSpaceUnitEnum::UNKNOWN;
        
        OmeTimeUnitEnum::Enum m_timeUnit = OmeTimeUnitEnum::UNKNOWN;
        
        QString m_name;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __OME_AXIS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OME_AXIS_DECLARE__

} // namespace
#endif  //__OME_AXIS_H__

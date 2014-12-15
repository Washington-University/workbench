#ifndef __EVENT_CHART_MATRIX_PARCEL_LOADING_YOKING_H__
#define __EVENT_CHART_MATRIX_PARCEL_LOADING_YOKING_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include <map>

#include "Event.h"
#include "YokingGroupEnum.h"



namespace caret {

    class ChartableMatrixParcelInterface;
    
    class EventChartMatrixParcelYokingValidation : public Event {
        
    public:
        enum Mode {
            MODE_APPLY_YOKING,
            MODE_VALIDATE_YOKING
        };
        
        EventChartMatrixParcelYokingValidation(const ChartableMatrixParcelInterface* chartableInterface,
                                      const YokingGroupEnum::Enum yokingGroup);
        
        virtual ~EventChartMatrixParcelYokingValidation();
        
        Mode getMode() const;
        
        void addValidateYokingChartableInterface(const ChartableMatrixParcelInterface* chartableInterface,
                                                 const int32_t selectedRowOrColumnIndex);
        
        bool isValidateYokingCompatible(AString& messageOut,
                                        int32_t& selectedRowOrColumnIndexOut) const;

        const ChartableMatrixParcelInterface* getChartableMatrixParcelInterface() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        EventChartMatrixParcelYokingValidation(const EventChartMatrixParcelYokingValidation&);

        EventChartMatrixParcelYokingValidation& operator=(const EventChartMatrixParcelYokingValidation&);
        
        const Mode m_mode;
        
        const ChartableMatrixParcelInterface* m_chartableInterface;
        
        const YokingGroupEnum::Enum m_yokingGroup;
        
        AString m_incompatibilityMessage;
        
        std::map<int32_t, int32_t> m_compatibleRowColumnIndicesCount;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_CHART_MATRIX_PARCEL_LOADING_YOKING_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_CHART_MATRIX_PARCEL_LOADING_YOKING_DECLARE__

} // namespace
#endif  //__EVENT_CHART_MATRIX_PARCEL_LOADING_YOKING_H__

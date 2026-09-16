#ifndef __DYNAMIC_CONNECTIVITY_FILE_INTERFACE_H__
#define __DYNAMIC_CONNECTIVITY_FILE_INTERFACE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include "ConnectivityCorrelationTwo.h"
#include "ConnectivityFileInterface.h"
#include "GeneralYokingGroupEnum.h"

namespace caret {

    class DynamicConnectivityFileInterface : public ConnectivityFileInterface {
        
    public:
        DynamicConnectivityFileInterface() { };
        
        virtual ~DynamicConnectivityFileInterface() { };
        
        DynamicConnectivityFileInterface(const DynamicConnectivityFileInterface&) = delete;

        DynamicConnectivityFileInterface& operator=(const DynamicConnectivityFileInterface&) = delete;
        
        virtual ConnectivityCorrelationSettings* getCorrelationSettings() = 0;
        
        virtual const ConnectivityCorrelationSettings* getCorrelationSettings() const = 0;

        virtual bool isEnabledAsLayer() const = 0;
        
        virtual void setEnabledAsLayer(const bool enabled) = 0;
        
        virtual GeneralYokingGroupEnum::Enum getDynamicConnectivityYokingGroup() const = 0;
        
        virtual void setDynamicConnectivityYokingGroup(const GeneralYokingGroupEnum::Enum yokingGroup) = 0;
        
        virtual bool loadDataForCorrelationWithDataSet(const ConnectivityCorrelationTwo::DataSet& dataSet,
                                                       const AString& dataSetName) = 0;
         
        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DYNAMIC_CONNECTIVITY_FILE_INTERFACE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DYNAMIC_CONNECTIVITY_FILE_INTERFACE_DECLARE__

} // namespace
#endif  //__DYNAMIC_CONNECTIVITY_FILE_INTERFACE_H__

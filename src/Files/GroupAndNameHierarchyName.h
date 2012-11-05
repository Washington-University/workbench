#ifndef __CLASS_AND_NAME_HIERARCHY_NAME__H_
#define __CLASS_AND_NAME_HIERARCHY_NAME__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include "BrainConstants.h"
#include "CaretObject.h"
#include "DisplayGroupEnum.h"
#include "GroupAndNameCheckStateEnum.h"

namespace caret {

    class GroupAndNameHierarchyName : public CaretObject {
    public:
        GroupAndNameHierarchyName(const AString& name,
                                  const int32_t key);
        
        ~GroupAndNameHierarchyName();
        
        void copySelections(const int32_t sourceTabIndex,
                            const int32_t targetTabIndex);
        
        AString getName() const;
        
        int32_t getKey() const;
        
        virtual bool isSelected(const DisplayGroupEnum::Enum displayGroup,
                        const int32_t tabIndex) const;
        
        virtual GroupAndNameCheckStateEnum::Enum getCheckState(const DisplayGroupEnum::Enum displayGroup,
                                                     const int32_t tabIndex) const;
        
        virtual void setSelected(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex,
                         const bool status);
        
        void clearCounter();
        
        void incrementCounter();
        
        int32_t getCounter() const;
        
        const float* getIconColorRGBA() const;
        
        void setIconColorRGBA(const float rgba[4]);
        
        // ADD_NEW_METHODS_HERE
    
    private:
        GroupAndNameHierarchyName(const GroupAndNameHierarchyName&);
        
        GroupAndNameHierarchyName& operator=(const GroupAndNameHierarchyName&);
        
        /** Name of an item (border, focus, etc) */
        AString name;
        
        /** Key for quickly locating item */
        int32_t key;
        
        /** Selection for each display group */
        bool selectedInDisplayGroup[DisplayGroupEnum::NUMBER_OF_GROUPS];
        
        /** Selection for each tab */
        bool selectedInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /** Counter for tracking usage of item */
        int32_t counter;
        
        /** Color for icon, valid when (iconRGBA[3] > 0.0) */
        float iconRGBA[4];

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_NAME_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLASS_AND_NAME_HIERARCHY_NAME_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_NAME__H_

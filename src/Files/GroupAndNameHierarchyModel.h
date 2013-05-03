#ifndef __CLASS_AND_NAME_HIERARCHY_MODEL_H_
#define __CLASS_AND_NAME_HIERARCHY_MODEL_H_

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

#include <deque>
#include <map>

#include "GroupAndNameHierarchyItem.h"
#include "GroupAndNameCheckStateEnum.h"

namespace caret {

    class BorderFile;
    class CiftiMappableDataFile;
    class FociFile;
    class LabelFile;
    
    class GroupAndNameHierarchyModel : public GroupAndNameHierarchyItem {
    public:
        GroupAndNameHierarchyModel();
        
        virtual ~GroupAndNameHierarchyModel();
        
        virtual void clear();
        
        bool isGroupValid(const int32_t groupKey) const;
        
        void setAllSelected(const bool status);
        
        void setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const bool status);
        
        void update(BorderFile* borderFile,
                    const bool forceUpdate);
        
        void update(FociFile* fociFile,
                    const bool forceUpdate);
        
        void update(LabelFile* labelFile,
                    const bool forceUpdate);
        
        void update(CiftiMappableDataFile* ciftiMappableDataFile,
                    const bool forceUpdate);
        
        bool needsUserInterfaceUpdate(const DisplayGroupEnum::Enum displayGroup,
                         const int32_t tabIndex) const;
        
    private:
        GroupAndNameHierarchyModel(const GroupAndNameHierarchyModel&);

        GroupAndNameHierarchyModel& operator=(const GroupAndNameHierarchyModel&);
        
        void clearModelPrivate();
        
        void setUserInterfaceUpdateNeeded();
        
        /**
         * Contains label keys and names from previous update with Label File.
         */
        std::map<int32_t, AString> m_previousLabelFileKeysAndNames;
        
        /**
         * Contains label keys and names from previous update with CIFTI label file.
         */
        std::vector<std::map<int32_t, AString> > m_previousCiftiLabelFileMapKeysAndNames;
        
        /**
         * Update needed status of DISPLAY GROUP in EACH TAB.
         * Used when user has set to a display group.
         * Indicates that an update is needed for the given display group in the given tab.
         */
        mutable bool m_updateNeededInDisplayGroupAndTab[DisplayGroupEnum::NUMBER_OF_GROUPS][BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
        /**
         * Update needed in TAB.
         */
        mutable bool m_updateNeededInTab[BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS];
        
    };
    
#ifdef __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__

} // namespace
#endif  //__CLASS_AND_NAME_HIERARCHY_MODEL_H_

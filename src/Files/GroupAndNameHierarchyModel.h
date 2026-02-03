#ifndef __CLASS_AND_NAME_HIERARCHY_MODEL_H_
#define __CLASS_AND_NAME_HIERARCHY_MODEL_H_

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

#include <deque>
#include <map>

#include "GroupAndNameHierarchyItem.h"
#include "GroupAndNameCheckStateEnum.h"

namespace caret {

    class BorderFile;
    class CaretMappableDataFile;
    class CiftiMappableDataFile;
    class FociFile;
    class LabelFile;
    class VolumeFile;
    
    class GroupAndNameHierarchyModel : public GroupAndNameHierarchyItem {
    public:
        GroupAndNameHierarchyModel(GroupAndNameHierarchyUserInterface* groupAndNameHierarchyUserInterface);
        
        virtual ~GroupAndNameHierarchyModel();
        
        virtual void clear();
        
        bool isGroupValid(const int32_t groupKey) const;
        
        void setAllSelected(const bool status);
        
        void setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                            const int32_t tabIndex,
                            const bool status);
        
        void setAllSelectedWithNames(const DisplayGroupEnum::Enum displayGroup,
                                     const int32_t tabIndex,
                                     const std::vector<AString>& names,
                                     const bool checked);
        
        void update(BorderFile* borderFile,
                    const bool forceUpdate);
        
        void update(FociFile* fociFile,
                    const bool forceUpdate);
        
        void update(LabelFile* labelFile,
                    const bool forceUpdate);
        
        void update(CiftiMappableDataFile* ciftiMappableDataFile,
                    const bool forceUpdate);
        
        void update(VolumeFile* volumeFile,
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
        
        CaretMappableDataFile* m_caretMappableDataFile = NULL;
        
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

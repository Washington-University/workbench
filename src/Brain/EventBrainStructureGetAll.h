#ifndef __EVENT_BRAIN_STRUCTURE_GET_ALL_H__
#define __EVENT_BRAIN_STRUCTURE_GET_ALL_H__

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


#include "Event.h"
#include "StructureEnum.h"


namespace caret {
    class BrainStructure;
    
    class EventBrainStructureGetAll : public Event {
        
    public:
        EventBrainStructureGetAll();
        
        virtual ~EventBrainStructureGetAll();
        
        void addBrainStructure(BrainStructure* brainStructure);
        
        int32_t getNumberOfBrainStructures() const;
        
        BrainStructure* getBrainStructureByIndex(const int32_t indx);
        
        BrainStructure* getBrainStructureByStructure(const StructureEnum::Enum structure);
        
    private:
        EventBrainStructureGetAll(const EventBrainStructureGetAll&);

        EventBrainStructureGetAll& operator=(const EventBrainStructureGetAll&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        std::vector<BrainStructure*> m_brainStructures;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_BRAIN_STRUCTURE_GET_ALL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_BRAIN_STRUCTURE_GET_ALL_DECLARE__

} // namespace
#endif  //__EVENT_BRAIN_STRUCTURE_GET_ALL_H__

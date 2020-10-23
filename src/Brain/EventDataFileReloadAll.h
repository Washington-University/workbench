#ifndef __EVENT_DATA_FILE_RELOAD_ALL_H__
#define __EVENT_DATA_FILE_RELOAD_ALL_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#include "Event.h"

namespace caret {

    class Brain;
    class CaretDataFile;
    class CaretResult;
    
    class EventDataFileReloadAll : public Event {
        
    public:
        enum class Mode {
            GET_FILES,
            RELOAD_FILES
        };
        
        static std::unique_ptr<CaretResult> getReloadableFiles(Brain* brain,
                                                               std::vector<CaretDataFile*>& reloadableFilesOut);
        
        static std::unique_ptr<CaretResult> reloadAllFiles(Brain* brain);
        
        virtual ~EventDataFileReloadAll();
        
        Mode getMode();
        
        Brain* getBrain();
        
        EventDataFileReloadAll(const EventDataFileReloadAll&) = delete;

        EventDataFileReloadAll& operator=(const EventDataFileReloadAll&) = delete;

        std::vector<CaretDataFile*> getReloadableFiles() const;
        
        void addReloadableFile(CaretDataFile* caretDataFile);

        // ADD_NEW_METHODS_HERE

    private:
        EventDataFileReloadAll(const Mode mode,
                               Brain* brain);
        
        const Mode m_mode;
        
        Brain* m_brain;
        
        std::vector<CaretDataFile*> m_reloadableFiles;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_DATA_FILE_RELOAD_ALL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_DATA_FILE_RELOAD_ALL_DECLARE__

} // namespace
#endif  //__EVENT_DATA_FILE_RELOAD_ALL_H__

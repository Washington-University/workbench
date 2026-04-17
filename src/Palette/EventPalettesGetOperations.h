#ifndef __EVENT_PALETTES_GET_OPERATIONS_H__
#define __EVENT_PALETTES_GET_OPERATIONS_H__

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

#include "Event.h"



namespace caret {

    class PaletteBase;
    class PaletteNew;
    
    class EventPalettesGetOperations : public Event {
        
    public:
        /**
         * Type of operation
         */
        enum class Operation {
            /** Get all palettes */
            GET_ALL_PALETTES,
            /** Get a palette by name */
            GET_PALETTE_WITH_NAME
        };
        
        static std::vector<const PaletteBase*> getAllPaletteTypesSortedByName();
        
        static std::vector<const PaletteNew*> getAllPaletteNewSortedByName();
        
        
        static const PaletteBase* getPaletteWithName(const AString& name);
        
        virtual ~EventPalettesGetOperations();
        
        EventPalettesGetOperations(const EventPalettesGetOperations&) = delete;

        EventPalettesGetOperations& operator=(const EventPalettesGetOperations&) = delete;
        
        Operation getOperation() const;
        
        // ADD_NEW_METHODS_HERE

    private:
        static std::vector<const PaletteBase*> getAllPalettesSortedByName(const bool includePaletteFlag,
                                                                          const bool includePaletteNewFlag);
        
        EventPalettesGetOperations(const Operation m_operation);
        
        void addPalette(const PaletteBase* palette);
        
        AString getPaletteName() const;
        
        const Operation m_operation;
        
        std::vector<const PaletteBase*> m_palettes;
        
        AString m_paletteName;
        
        // ADD_NEW_MEMBERS_HERE

        friend class Brain;
        friend class PaletteFile;
        
    };
    
#ifdef __EVENT_PALETTES_GET_OPERATIONS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_PALETTES_GET_OPERATIONS_DECLARE__

} // namespace
#endif  //__EVENT_PALETTES_GET_OPERATIONS_H__

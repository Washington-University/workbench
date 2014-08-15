#ifndef __EVENT_PALETTE_COLOR_MAPPING_EDITOR_DIALOG_REQUEST_H__
#define __EVENT_PALETTE_COLOR_MAPPING_EDITOR_DIALOG_REQUEST_H__

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


#include "Event.h"



namespace caret {

    class CaretMappableDataFile;
    
    class EventPaletteColorMappingEditorDialogRequest : public Event {
        
    public:
        EventPaletteColorMappingEditorDialogRequest(const int32_t browserWindowIndex,
                                                    CaretMappableDataFile* mapFile,
                                                    const int32_t mapIndex);
        
        virtual ~EventPaletteColorMappingEditorDialogRequest();
        

        /**
         * @return Get the index of the browser window for palette being edited.
         */
        int32_t getBrowserWindowIndex() const { return m_browserWindowIndex; }
        
        /**
         * @return Map file containing map whose color palette is edited
         */
        CaretMappableDataFile* getCaretMappableDataFile() const { return m_mapFile; }
        
        /**
         * @return Index of map in the map file
         */
        int32_t getMapIndex() const { return m_mapIndex; }
        
        // ADD_NEW_METHODS_HERE

    private:
        EventPaletteColorMappingEditorDialogRequest(const EventPaletteColorMappingEditorDialogRequest&);

        EventPaletteColorMappingEditorDialogRequest& operator=(const EventPaletteColorMappingEditorDialogRequest&);
        
        /** index of browser window for palette editing */
        int32_t m_browserWindowIndex;

        /** Map file containing map whose color palette is edited */
        CaretMappableDataFile* m_mapFile;
        
        /** Index of map in the map file */
        int32_t m_mapIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_PALETTE_COLOR_MAPPING_EDITOR_DIALOG_REQUEST_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_PALETTE_COLOR_MAPPING_EDITOR_DIALOG_REQUEST_DECLARE__

} // namespace
#endif  //__EVENT_PALETTE_COLOR_MAPPING_EDITOR_DIALOG_REQUEST_H__

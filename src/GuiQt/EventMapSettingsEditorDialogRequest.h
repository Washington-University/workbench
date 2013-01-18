#ifndef __EVENT_MAP_SETTINGS_EDITOR_DIALOG_REQUEST_H__
#define __EVENT_MAP_SETTINGS_EDITOR_DIALOG_REQUEST_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "Event.h"

namespace caret {

    class CaretMappableDataFile;
    class Overlay;
    
    /// Event for showing edit map scalar color mapping editor
    class EventMapSettingsEditorDialogRequest : public Event {
        
    public:
        EventMapSettingsEditorDialogRequest(const int32_t browserWindowIndex,
                                            Overlay* overlay,
                                            CaretMappableDataFile* mapFile,
                                            const int32_t mapIndex);
        
        virtual ~EventMapSettingsEditorDialogRequest();
        
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
        
        /**
         * @return The overlay for the editor.
         */
        Overlay* getOverlay() { return m_overlay; }
        
        /**
         * @return The overlay for the editor.
         */
        const Overlay* getOverlay() const { return m_overlay; }
        
    private:
        EventMapSettingsEditorDialogRequest(const EventMapSettingsEditorDialogRequest&);
        
        EventMapSettingsEditorDialogRequest& operator=(const EventMapSettingsEditorDialogRequest&);
        
        /** Map file containing map whose color palette is edited */
        CaretMappableDataFile* m_mapFile;
        
        /** Index of map in the map file */
        int32_t m_mapIndex;
        
        /** index of browser window for palette editing */
        int32_t m_browserWindowIndex;
        
        /** Overlay for editor. */
        Overlay* m_overlay;
    };

} // namespace

#endif // __EVENT_MAP_SETTINGS_EDITOR_DIALOG_REQUEST_H__

#ifndef __EVENT_MAP_SETTINGS_EDITOR_DIALOG_REQUEST_H__
#define __EVENT_MAP_SETTINGS_EDITOR_DIALOG_REQUEST_H__

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

namespace caret {

    class CaretMappableDataFile;
    class ChartTwoOverlay;
    class Overlay;
    
    /// Event for showing edit map scalar color mapping editor
    class EventOverlaySettingsEditorDialogRequest : public Event {
        
    public:
        enum Mode {
            MODE_SHOW_EDITOR,
            MODE_OVERLAY_MAP_CHANGED,
            MODE_UPDATE_ALL
        };
        EventOverlaySettingsEditorDialogRequest(const Mode mode,
                                                const int32_t browserWindowIndex,
                                                Overlay* overlay,
                                                CaretMappableDataFile* mapFile,
                                                const int32_t mapIndex);
        
        EventOverlaySettingsEditorDialogRequest(const Mode mode,
                                                const int32_t browserWindowIndex,
                                                ChartTwoOverlay* chartOverlay,
                                                CaretMappableDataFile* mapFile,
                                                const int32_t mapIndex);
        
        virtual ~EventOverlaySettingsEditorDialogRequest();
        
        /**
         * @return The mode (show or update)
         */
        Mode getMode() const { return m_mode; }
        
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
        
        /**
         * @return The chart overlay for the editor.
         */
        ChartTwoOverlay* getChartTwoOverlay() { return m_chartOverlay; }
        
        /**
         * @return The overlay for the editor.
         */
        const ChartTwoOverlay* getChartTwoOverlay() const { return m_chartOverlay; }
        
    private:
        EventOverlaySettingsEditorDialogRequest(const EventOverlaySettingsEditorDialogRequest&);
        
        EventOverlaySettingsEditorDialogRequest& operator=(const EventOverlaySettingsEditorDialogRequest&);

        /** The mode show/update */
        const Mode m_mode;
        
        /** index of browser window for palette editing */
        int32_t m_browserWindowIndex;
        
        /** Overlay for editor. */
        Overlay* m_overlay;

        /** Chart Overlay for editor. */
        ChartTwoOverlay* m_chartOverlay;
        
        /** Map file containing map whose color palette is edited */
        CaretMappableDataFile* m_mapFile;
        
        /** Index of map in the map file */
        int32_t m_mapIndex;
    };

} // namespace

#endif // __EVENT_MAP_SETTINGS_EDITOR_DIALOG_REQUEST_H__

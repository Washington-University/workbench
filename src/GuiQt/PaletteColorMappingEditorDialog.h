#ifndef __PALETTE_COLOR_MAPPING_EDITOR_DIALOG_H__
#define __PALETTE_COLOR_MAPPING_EDITOR_DIALOG_H__

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

#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QLabel;
class QTabWidget;

namespace caret {
    class CaretMappableDataFile;
    class MapSettingsColorBarWidget;
    class MapSettingsPaletteColorMappingWidget;

    class PaletteColorMappingEditorDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        PaletteColorMappingEditorDialog(QWidget* parent = 0);
        
        virtual ~PaletteColorMappingEditorDialog();
        
        virtual void receiveEvent(Event* event);
        
        /** May be called requesting the dialog to update its content */
        virtual void updateDialog();
        
        void updateDialogContent(CaretMappableDataFile* mapFile,
                                 const int32_t mapIndex,
                                 const int32_t browserTabIndex);
        
    protected:        
        virtual void focusInEvent(QFocusEvent* event);
        
        // ADD_NEW_METHODS_HERE

    private:
        PaletteColorMappingEditorDialog(const PaletteColorMappingEditorDialog&);

        PaletteColorMappingEditorDialog& operator=(const PaletteColorMappingEditorDialog&);

        CaretMappableDataFile* m_mapFile;
        
        int32_t m_mapIndex;
        
        int32_t m_browserTabIndex;
        
        QTabWidget* m_tabWidget;
        
        MapSettingsColorBarWidget* m_paletteColorBarWidget;
        
        MapSettingsPaletteColorMappingWidget* m_paletteColorMappingEditor;
        
        QLabel* m_fileNameValueLabel;
        
        QLabel* m_mapNameValueLabel;
        
        int32_t m_paletteColorBarWidgetTabIndex;
        
        int32_t m_paletteEditorWidgetTabIndex;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_COLOR_MAPPING_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_COLOR_MAPPING_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__PALETTE_COLOR_MAPPING_EDITOR_DIALOG_H__

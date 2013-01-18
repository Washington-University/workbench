#ifndef __MAP_SETTINGS_EDITOR_DIALOG__H_
#define __MAP_SETTINGS_EDITOR_DIALOG__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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


#include "WuQDialogNonModal.h"

class QCheckBox;
class QLabel;
class QLayout;


namespace caret {
    
    class CaretMappableDataFile;
    class MapSettingsOverlayWidget;
    class MapSettingsPaletteColorMappingWidget;
    class Overlay;
    
    class MapSettingsEditorDialog : public WuQDialogNonModal {
        Q_OBJECT
        
    public:
        MapSettingsEditorDialog(QWidget* parent);
        
        void updateDialogContent(Overlay* overlay);
        
        void updateDialog();
        
        virtual ~MapSettingsEditorDialog();

        bool isDoNotReplaceSelected() const;
        
    protected:
        virtual void closeButtonPressed();
        
        virtual void focusInEvent(QFocusEvent* event);
        
    private:
        MapSettingsEditorDialog(const MapSettingsEditorDialog&);

        MapSettingsEditorDialog& operator=(const MapSettingsEditorDialog&);
        
    private slots:
        void doNotReplaceCheckBoxStateChanged(int state);
        
    private:
        QWidget* createWindowOptionsSection();
        
        QWidget* createMapFileAndNameSection();
        
        void setLayoutMargins(QLayout* layout);
        
        QCheckBox* m_doNotReplaceCheckBox;
        
        CaretMappableDataFile* m_caretMappableDataFile;
        
        Overlay* m_overlay;
        
        int32_t m_mapIndex;
        
        MapSettingsPaletteColorMappingWidget* m_paletteColorMappingWidget;
        
        MapSettingsOverlayWidget* m_overlayWidget;
        
        QLabel* m_selectedMapFileNameLabel;
        
        QLabel* m_selectedMapNameLabel;

    };
    
#ifdef __MAP_SETTINGS_EDITOR_DIALOG_DECLARE__
#endif // __MAP_SETTINGS_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__MAP_SETTINGS_EDITOR_DIALOG__H_

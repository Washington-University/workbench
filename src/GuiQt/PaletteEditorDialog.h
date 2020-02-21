
#ifndef __PALETTE_EDITOR_DIALOG_H__
#define __PALETTE_EDITOR_DIALOG_H__

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

#include "WuQDialogNonModal.h"

class QButtonGroup;
class QColorDialog;
class QComboBox;
class QLabel;
class QTabWidget;

namespace caret {

    class WuQScrollArea;
    
    class PaletteEditorControlPointGroupWidget;
    
    class PaletteEditorDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        PaletteEditorDialog(QWidget* parent);
        
        virtual ~PaletteEditorDialog();
        
        PaletteEditorDialog(const PaletteEditorDialog&) = delete;

        PaletteEditorDialog& operator=(const PaletteEditorDialog&) = delete;
        
        virtual void updateDialog();

        // ADD_NEW_METHODS_HERE

    private slots:
        void editColor(const uint8_t rgb[3]);
        
    private:
        QWidget* createControlPointsWidget();
        
        QWidget* createPaletteSelectionWidget();
        
        QWidget* createPaletteMovementButtonsWidget();
        
        QWidget* createPaletteWidget();
        
        void updateControlPointWidgets();
        
        PaletteEditorControlPointGroupWidget* m_positiveControlPointsWidget;
        
        PaletteEditorControlPointGroupWidget* m_zeroControlPointsWidget;
        
        PaletteEditorControlPointGroupWidget* m_negativeControlPointsWidget;
        
        QLabel* m_colorBarImageLabel;
        
        QColorDialog* m_colorEditorDialog;
        
        QButtonGroup* m_colorEditButtonGroup;

        QTabWidget* m_paletteTypeTabWidget;
        
        QComboBox* m_paletteSelectionComboBox;
        
        WuQScrollArea* m_scrollArea;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__PALETTE_EDITOR_DIALOG_H__


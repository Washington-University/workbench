
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

#include "PaletteNew.h"
#include "PalettePixmapPainter.h"
#include "WuQDialogNonModal.h"

class QButtonGroup;
class QComboBox;
class QLabel;
class QPushButton;
class QTabWidget;
class QToolButton;

namespace caret {

    class WuQColorEditorWidget;
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
        void editColor(const int32_t controlPointIndex, const uint8_t red, const uint8_t green, const uint8_t blue);
        
        void newPaletteButtonClicked();
        
        void colorEditorColorChanged(const uint8_t red, const uint8_t green, const uint8_t blue);
        
        void userPaletteComboBoxActivated(int index);
        
    private:
        enum class IconType {
            ARROW_LEFT_DOWN,
            ARROW_UP_RIGHT
        };
        
        void loadPalette(const PaletteNew* palette);
        
        QWidget* createControlPointsWidget();
        
        QWidget* createPaletteWidget();
        
        QWidget* createPaletteSelectionWidget();
        
        QPixmap createIcon(QWidget* widget,
                           const IconType iconType);
        
        void createUserPalettes();
        
        void updatePaletteColorBarImage();
        
        PaletteEditorControlPointGroupWidget* m_positiveControlPointsWidget;
        
        PaletteEditorControlPointGroupWidget* m_zeroControlPointsWidget;
        
        PaletteEditorControlPointGroupWidget* m_negativeControlPointsWidget;
        
        QLabel* m_colorBarImageLabel;
        
        WuQColorEditorWidget* m_colorEditorWidget;
        
        QButtonGroup* m_colorEditButtonGroup;

        QComboBox* m_paletteSourceComboBox;
        
        QComboBox* m_userPaletteSelectionComboBox;
        
        PalettePixmapPainter::Mode m_pixmapMode = PalettePixmapPainter::Mode::INTERPOLATE_OFF;
        
        std::vector<std::unique_ptr<PaletteNew>> m_userPalettes;
        
        AString m_currentPaletteName;
        std::vector<PaletteNew::ScalarColor> m_currentPalettePositive;
        std::vector<PaletteNew::ScalarColor> m_currentPaletteNegative;
        std::vector<PaletteNew::ScalarColor> m_currentPaletteZero;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__PALETTE_EDITOR_DIALOG_H__



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

#include <QColor>

#include "PaletteNew.h"
#include "PalettePixmapPainter.h"
#include "WuQDialogNonModal.h"

class QButtonGroup;
class QComboBox;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QTabWidget;
class QToolButton;

namespace caret {

    class CaretRgb;
    
    class WuQColorEditorWidget;
    class WuQScrollArea;
    
    class PaletteEditorRangeWidget;
    
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
        void editColor(const CaretRgb& rgb);
        
        void newPaletteButtonClicked();
        
        void colorEditorColorChanged(const QColor& color);
        
        void userPaletteListWidgetActivated(QListWidgetItem* item);
        
        void rangeWidgetDataChanged();
        
        void addPalettePushButtonClicked();
        
        void replacePalettePushButtonClicked();
        
        void loadPalettePushButtonClicked();
        
    private:
        enum class IconType {
            ARROW_LEFT_DOWN,
            ARROW_UP_RIGHT
        };
        
        std::unique_ptr<PaletteNew> getPaletteFromEditor() const;
        
        void loadPalette(const PaletteNew* palette);
        
        QWidget* createControlPointsWidget();
        
        QWidget* createPaletteBarWidget();
        
        QWidget* createPaletteSelectionWidget();
        
        QPixmap createIcon(QWidget* widget,
                           const IconType iconType);
        
        void createUserPalettes();
        
        void updatePaletteColorBarImage();
        
        QWidget* createMovePaletteButtonsWidget();
        
        void updatePaletteMovementButtons();
        
        bool isPaletteModified() const;
        
        PaletteEditorRangeWidget* m_positiveRangeWidget;
        
        PaletteEditorRangeWidget* m_zeroRangeWidget;
        
        PaletteEditorRangeWidget* m_negativeRangeWidget;
        
        QLabel* m_colorBarImageLabel;
        
        WuQColorEditorWidget* m_colorEditorWidget;
        
        QButtonGroup* m_colorEditButtonGroup;

        QComboBox* m_paletteSourceComboBox;
        
        QListWidget* m_userPaletteSelectionListWidget;
        
        PalettePixmapPainter::Mode m_pixmapMode = PalettePixmapPainter::Mode::INTERPOLATE_OFF;
        
        std::vector<std::unique_ptr<PaletteNew>> m_userPalettes;
        
        QPushButton* m_addPalettePushButton;
        
        QPushButton* m_replacePalettePushButton;
        
        QPushButton* m_loadPalettePushButton;
        
        QPushButton* m_newPalettePushButton;
        
        struct UnmodifiedPalette {
            std::vector<PaletteNew::ScalarColor> m_positiveMapping;
            std::vector<PaletteNew::ScalarColor> m_negativeMapping;
            std::vector<PaletteNew::ScalarColor> m_zeroMapping;
        } m_unmodifiedPalette;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__PALETTE_EDITOR_DIALOG_H__


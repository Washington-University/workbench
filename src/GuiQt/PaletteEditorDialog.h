
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

#include "EventListenerInterface.h"
#include "PaletteNew.h"
#include "PalettePixmapPainter.h"
#include "WuQDialogNonModal.h"

class QButtonGroup;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPushButton;
class QRadioButton;
class QTabWidget;
class QToolButton;

namespace caret {

    class CaretRgb;
    class PaletteEditorRangeRow;
    class PaletteEditorRangeWidget;
    class PaletteNew;
    class PaletteSelectionWidget;
    class WuQColorEditorWidget;
    class WuQDataEntryDialog;
    class WuQScrollArea;
    
    class PaletteEditorDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        PaletteEditorDialog(QWidget* parent);
        
        virtual ~PaletteEditorDialog();
        
        PaletteEditorDialog(const PaletteEditorDialog&) = delete;

        PaletteEditorDialog& operator=(const PaletteEditorDialog&) = delete;
        
        virtual void updateDialog() override;

        virtual void receiveEvent(Event* event) override;

        // ADD_NEW_METHODS_HERE

    private slots:
        void editColor(const CaretRgb& rgb);
        
        void colorEditorColorChanged(const QColor& color);
        
        void paletteSelected(const PaletteBase* paletteBase);
        
        void rangeWidgetDataChanged();
        
        void controlPointButtonClicked(QAbstractButton* button);
        
        void insertControlPointAboveActionTriggered();
        
        void insertControlPointBelowActionTriggered();
        
        void removeControlPointActionTriggered();

        void newPaletteActionTriggered();
        
        void renamePaletteActionTriggered();
        
        void deletePaletteActionTriggered();
        
        void savePaletteActionTriggered();
        
        void revertPaletteActionTriggered();
        
        void exportPaletteActionTriggered();
        
        void importPaletteActionTriggered();
        
    protected:
        virtual DialogUserButtonResult userButtonPressed(QPushButton* userPushButton) override;
        
    private:
        enum class IconType {
            ARROW_LEFT_DOWN,
            ARROW_UP_RIGHT
        };
        
        void updateDialogInternal(const bool updatePaletteListFlag);
        
        std::unique_ptr<PaletteNew> getPaletteFromEditor() const;
        
        void loadPaletteIntoEditor();
        
        QWidget* createControlPointsWidget();
        
        QWidget* createPaletteBarWidget();
        
        QWidget* createPaletteSelectionWidget();
        
        QPixmap createIcon(QWidget* widget,
                           const IconType iconType);
        
        void updatePaletteListWidget(const bool forceUpdate = false);
        
        void updatePaletteColorBarImage();
        
        void updatePaletteMovementButtons();
        
        void clearEditorModified();
        
        bool isPaletteModified() const;
        
        void updateModifiedLabel();
        
        bool modifiedPaletteWarningDialog();
        
        void updateAfterPalettesChanged();
        
        virtual void closeEvent(QCloseEvent* event) override;
        
        QString getDocumentationHtml() const;
        
        std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*> getSelectedControlPointInfo();
        
        std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*> getControlPointWithScalar(const float scalar);
        
        PaletteEditorRangeWidget* m_positiveRangeWidget;
        
        PaletteEditorRangeWidget* m_zeroRangeWidget;
        
        PaletteEditorRangeWidget* m_negativeRangeWidget;
        
        QAction* m_insertControlPointAboveAction;
        
        QAction* m_insertControlPointBelowAction;

        QAction* m_removeControlPointAction;
        
        QLabel* m_colorBarImageLabel;
        
        QAction* m_paletteRevertAction;
        
        QAction* m_paletteSaveAction;
        
        QToolButton* m_paletteSaveToolButton;
        
        WuQColorEditorWidget* m_colorEditorWidget = NULL;
        
        QButtonGroup* m_colorEditButtonGroup;

        PalettePixmapPainter::Mode m_pixmapMode = PalettePixmapPainter::Mode::INTERPOLATE_ON_LINES_AT_SCALARS;

        PaletteSelectionWidget* m_paletteSelectionWidget;
        
        QAction* m_newPaletteAction;
        
        QAction* m_renamePaletteAction;
        
        QAction* m_deletePaletteAction;

        QAction* m_importPaletteAction;
        
        QAction* m_exportPaletteAction;
        
        QPushButton* m_helpPushbutton;
        
        struct UnmodifiedPalette {
            std::vector<PaletteNew::ScalarColor> m_positiveMapping;
            std::vector<PaletteNew::ScalarColor> m_negativeMapping;
            std::vector<PaletteNew::ScalarColor> m_zeroMapping;
        } m_unmodifiedPalette;
        
        std::vector<const PaletteNew*> m_previouslyLoadedPalettes;
        
        const PaletteNew* m_paletteBeingEdited = NULL;
        
        bool m_ingoreUserInterfaceUpdateEventFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__PALETTE_EDITOR_DIALOG_H__


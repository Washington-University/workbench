#ifndef __WU_Q_MACRO_DIALOG_H__
#define __WU_Q_MACRO_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#include <QDialog>

#include "WuQMacroShortCutKeyEnum.h"
#include "WuQMacroStandardItemTypeEnum.h"

class QAbstractButton;
class QCheckBox;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QLabel;
class QMenu;
class QPlainTextEdit;
class QStackedWidget;
class QStandardItem;
class QTreeView;
class QToolButton;

namespace caret {

    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroGroup;
    class WuQMacroShortCutKeyComboBox;

    class WuQMacroDialog : public QDialog {
        
        Q_OBJECT

    public:
        WuQMacroDialog(QWidget* parent = 0);
        
        virtual ~WuQMacroDialog();
        
        WuQMacroDialog(const WuQMacroDialog&) = delete;

        WuQMacroDialog& operator=(const WuQMacroDialog&) = delete;
        
        void updateDialogContents();

        // ADD_NEW_METHODS_HERE

    private slots:
        void treeViewItemClicked(const QModelIndex& modelIndex);
        
        void macroGroupComboBoxActivated(int);
        
        void buttonBoxButtonClicked(QAbstractButton* button);
        
        void importMacroGroupActionTriggered();
        
        void exportMacroGroupActionTriggered();
        
        void macroGroupToolButtonClicked();
        
        void macroNameEditButtonClicked();
        
        void macroDescriptionEditButtonClicked();
        
        void macroShortCutKeySelected(const WuQMacroShortCutKeyEnum::Enum);
        
        void runOptionMoveMouseCheckBoxClicked(bool);
        
        void runOptionLoopCheckBoxClicked(bool);
        
        void editingMoveUpToolButtonClicked();

        void editingMoveDownToolButtonClicked();

        void editingDeleteToolButtonClicked();

        void editingInsertToolButtonClicked();
        
        void runMacroToolButtonClicked();

        void macroCommandDelaySpinBoxValueChanged(double);
        
    private:
        enum class ValueIndex {
            ONE,
            TWO
        };
        
        enum class EditButton {
            DELETER,  /* "DELETE" does not compile on an operating system */
            INSERTER,
            MOVE_DOWN,
            MOVE_UP,
            RUN
        };
        
        QWidget* createMacroAndCommandSelectionWidget();
        
        QWidget* createRunOptionsWidget();
        
        QWidget* createMacroDisplayWidget();
        
        QWidget* createCommandDisplayWidget();
        
        void updateMacroWidget(WuQMacro* macro);
        
        void updateCommandWidget(WuQMacroCommand* command);
        
        void setMacroCommandValue(const ValueIndex valueIndex);
        
        WuQMacroGroup* getSelectedMacroGroup();
        
        WuQMacro* getSelectedMacro();
        
        WuQMacroCommand* getSelectedMacroCommand();
        
        WuQMacroStandardItemTypeEnum::Enum getSelectedItemType() const;
        
        QStandardItem* getSelectedItem() const;
        
        QWidget* createHorizontalLine() const;
        
        QMenu* createMacroGroupMenu();
        
        QWidget* createMacroRunAndEditingToolButtons();
        
        QPixmap createEditingToolButtonPixmap(const QWidget* widget,
                                              const EditButton editButton);
        
        void updateEditingToolButtons();
        
        std::vector<WuQMacroGroup*> m_macroGroups;
        
        QComboBox* m_macroGroupComboBox;
        
        QToolButton* m_macroGroupToolButton;
        
        QTreeView* m_treeView;
        
        QLabel* m_macroNameLabel;
        
        WuQMacroShortCutKeyComboBox* m_macroShortCutKeyComboBox;
        
        QPlainTextEdit* m_macroDescriptionTextEdit;
        
        QWidget* m_macroWidget;
        
        QWidget* m_commandWidget;
        
        QStackedWidget* m_stackedWidget;
        
        QDialogButtonBox* m_dialogButtonBox;
        
        QCheckBox* m_runOptionLoopCheckBox;
        
        QCheckBox* m_runOptionMoveMouseCheckBox;
        
        QLabel* m_commandTitleLabel;
        
        QLabel* m_commandTypeLabel;
        
        QLabel* m_commandNameLabel;
        
        QPushButton* m_commandValueOnePushButton;
        
        QLabel* m_commandValueOneLabel;
        
        QPushButton* m_commandValueTwoPushButton;
        
        QLabel* m_commandValueTwoLabel;
        
        QDoubleSpinBox* m_commandDelaySpinBox;
        
        QPlainTextEdit* m_commandToolTipTextEdit;
        
        QToolButton* m_runMacroToolButton;
        
        QToolButton* m_editingMoveUpToolButton;
        
        QToolButton* m_editingMoveDownToolButton;
        
        QToolButton* m_editingDeleteToolButton;
        
        QToolButton* m_editingInsertToolButton;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_MACRO_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_MACRO_DIALOG_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_DIALOG_H__

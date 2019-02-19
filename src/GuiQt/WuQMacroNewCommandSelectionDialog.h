#ifndef __WU_Q_MACRO_NEW_COMMAND_SELECTION_DIALOG_H__
#define __WU_Q_MACRO_NEW_COMMAND_SELECTION_DIALOG_H__

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
#include <vector>

#include <QDialog>

#include "WuQMacroCommandTypeEnum.h"

class QAbstractButton;
class QComboBox;
class QDialogButtonBox;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QSplitter;
class QStackedWidget;

namespace caret {

    class EnumComboBoxTemplate;
    class WuQMacro;
    class WuQMacroCommand;
    class WuQMacroGroup;
    class WuQMacroShortCutKeyComboBox;
    class WuQMacroSignalWatcher;

    class WuQMacroNewCommandSelectionDialog : public QDialog {
        
        Q_OBJECT

    public:
        WuQMacroNewCommandSelectionDialog(QWidget* parent = 0);
        
        virtual ~WuQMacroNewCommandSelectionDialog();
        
        WuQMacroNewCommandSelectionDialog(const WuQMacroNewCommandSelectionDialog&) = delete;

        WuQMacroNewCommandSelectionDialog& operator=(const WuQMacroNewCommandSelectionDialog&) = delete;

        // ADD_NEW_METHODS_HERE
        
    signals:
        void signalNewMacroCommandCreated(WuQMacroCommand* command);
        
    public slots:
        void commandTypeComboBoxActivated();
        
        void customCommandListWidgetItemClicked(QListWidgetItem* item);
        
        void widgetCommandListWidgetItemClicked(QListWidgetItem* item);
        
        virtual void done(int r) override;
        
        void otherButtonClicked(QAbstractButton* button);
        
    private:
        WuQMacroCommand* getNewInstanceOfSelectedCommand(QString& errorMessageOut);
        
        void loadCustomCommandListWidget();
        
        void loadWidgetCommandListWidget();
        
        bool processApplyButtonClicked(const bool okButtonClicked);
        
        EnumComboBoxTemplate* m_commandTypeComboBox;
        
        QStackedWidget* m_stackedWidget;
        
        QSplitter* m_splitter;
        
        QListWidget* m_customCommandListWidget;
        
        QListWidget* m_widgetCommandListWidget;
        
        QPlainTextEdit* m_macroDescriptionTextEdit;
        
        QDialogButtonBox* m_dialogButtonBox;
        
        std::vector<WuQMacroCommand*> m_customCommands;
        
        std::vector<WuQMacroSignalWatcher*> m_widgetCommands;
        
        WuQMacroCommand* m_lastCustomCommandAdded = NULL;
        
        bool m_commandSelectionChangedSinceApplyClickedFlag = false;

        static WuQMacroCommandTypeEnum::Enum s_lastCommandTypeSelected;

        static QByteArray s_previousDialogGeometry;

        static QByteArray s_previousSplitterState;
    };
    
#ifdef __WU_Q_MACRO_NEW_COMMAND_SELECTION_DIALOG_DECLARE__
    WuQMacroCommandTypeEnum::Enum WuQMacroNewCommandSelectionDialog::s_lastCommandTypeSelected = WuQMacroCommandTypeEnum::CUSTOM_OPERATION;
    QByteArray WuQMacroNewCommandSelectionDialog::s_previousDialogGeometry;
    QByteArray WuQMacroNewCommandSelectionDialog::s_previousSplitterState;
#endif // __WU_Q_MACRO_NEW_COMMAND_SELECTION_DIALOG_DECLARE__

} // namespace
#endif  //__WU_Q_MACRO_NEW_COMMAND_SELECTION_DIALOG_H__

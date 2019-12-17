#ifndef __RECENT_FILES_DIALOG_H__
#define __RECENT_FILES_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

class QAction;
class QActionGroup;
class QCheckBox;
class QLineEdit;
class QPushButton;
class QTableWidget;

#include "RecentFilesModeEnum.h"

namespace caret {

    class RecentFilesDialog : public QDialog {
        
        Q_OBJECT

    public:
        enum class ResultModeEnum {
            CANCEL,
            OPEN_DIRECTORY,
            OPEN_FILE,
            OPEN_OTHER
        };
        
        static ResultModeEnum runDialog(AString& nameOut,
                                        QWidget* parent = 0);
        
        virtual ~RecentFilesDialog();
        
        RecentFilesDialog(const RecentFilesDialog&) = delete;

        RecentFilesDialog& operator=(const RecentFilesDialog&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void cancelButtonClicked();
        
        void openButtonClicked();
        
        void openOtherButtonClicked();
        
        void filesModeActionTriggered(QAction* action);
        
        void showSceneFilesCheckBoxClicked(bool checked);
        
        void showSpecFilesCheckBoxClicked(bool checked);
        
        void nameFilterTextEdited(const QString& text);
        
    private:
        RecentFilesDialog(QWidget* parent = 0);
        
        ResultModeEnum getResultMode();
        
        AString getSelectedDirectoryOrFileName();
        
        QWidget* createDialogButtonsWidget();
        
        QWidget* createFileTypesButtonWidget();
        
        QWidget* createFilesFilteringWidget();
        
        RecentFilesModeEnum::Enum getSelectedFilesMode() const;
        
        QAction* getActionForMode(const RecentFilesModeEnum::Enum recentFilesMode) const;
        
        QPushButton* m_openPushButton;
        
        QActionGroup* m_filesModeActionGroup;
        
        QCheckBox* m_showSceneFilesCheckBox;
        
        QCheckBox* m_showSpecFilesCheckBox;
        
        QLineEdit* m_nameFilterLineEdit;
        
        ResultModeEnum m_resultMode = ResultModeEnum::CANCEL;
        
        QTableWidget* m_filesTableWidget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RECENT_FILES_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __RECENT_FILES_DIALOG_DECLARE__

} // namespace
#endif  //__RECENT_FILES_DIALOG_H__

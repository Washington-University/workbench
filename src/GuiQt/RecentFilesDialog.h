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

#include "RecentFileItemsContainerModeEnum.h"

namespace caret {

    class RecentFileItem;
    class RecentFileItemsContainer;
    class RecentFilesTableWidget;
    
    class RecentFilesDialog : public QDialog {
        
        Q_OBJECT

    public:
        enum class RunMode {
            OPEN_RECENT,
            SPLASH_SCREEN
        };
        
        enum class ResultModeEnum {
            CANCEL,
            LOAD_FILES_IN_SPEC_FILE,
            LOAD_SCENE_FROM_SCENE_FILE,
            OPEN_DIRECTORY,
            OPEN_FILE,
            OPEN_OTHER
        };
        
        static ResultModeEnum runDialog(const RunMode runMode,
                                        AString& nameOut,
                                        int32_t& sceneIndexOut,
                                        QWidget* parent = 0);
        
        virtual ~RecentFilesDialog();
        
        RecentFilesDialog(const RecentFilesDialog&) = delete;

        RecentFilesDialog& operator=(const RecentFilesDialog&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void cancelButtonClicked();
        
        void loadButtonClicked();
        
        void openButtonClicked();
        
        void openOtherButtonClicked();
        
        void testButtonClicked();
        
        void filesModeActionTriggered(QAction* action);
        
        void showDirectoriesCheckBoxClicked(bool checked);
        
        void showSceneFilesCheckBoxClicked(bool checked);
        
        void showSpecFilesCheckBoxClicked(bool checked);
        
        void nameFilterTextEdited(const QString& text);
        
        void hcpWebsiteButtonClicked();
        
        void twitterButtonClicked();
        
        void tableWidgetItemClicked(RecentFileItem* item);
        
        void tableWidgetItemDoubleClicked(RecentFileItem* item);
        
        void loadSceneOrSpecFileFromItem(RecentFileItem* item,
                                         const QPoint& globalPosition,
                                         const bool showMenuForSpecFileFlag);
        
    private slots:
        void updateFilesTableContent();
        
    protected:
        virtual void keyPressEvent(QKeyEvent* event) override;
        
    private:
        RecentFilesDialog(const RunMode runMode,
                          QWidget* parent = 0);
        
        ResultModeEnum getResultMode() const;
        
        AString getSelectedDirectoryOrFileName() const;
        
        int32_t getSelectedSceneIndex() const;
        
        QWidget* createDialogButtonsWidget();
        
        QWidget* createFileTypesButtonWidget();
        
        QWidget* createFilesFilteringWidget();
        
        QWidget* createInternetButtonsWidget();
        
        RecentFileItemsContainerModeEnum::Enum getSelectedFilesMode() const;
        
        QAction* getActionForMode(const RecentFileItemsContainerModeEnum::Enum recentFilesMode) const;
        
        void websiteLinkActivated(const QString& link);
        
        void updateFavoritesContainer();
        
        void loadSceneOrSpecFile(const AString& pathAndFileName,
                                 const int32_t sceneIndex);
        
        const RunMode m_runMode;
        
        QPushButton* m_loadPushButton;
        
        QPushButton* m_openPushButton;
        
        QActionGroup* m_fileTypeModeActionGroup;
        
        QCheckBox* m_showDirectoriesCheckBox;
        
        QCheckBox* m_showSceneFilesCheckBox;
        
        QCheckBox* m_showSpecFilesCheckBox;
        
        QLineEdit* m_nameFilterLineEdit;
        
        ResultModeEnum m_resultMode = ResultModeEnum::CANCEL;
        
        AString m_resultFilePathAndName;
        
        int32_t m_resultSceneIndex = -1;
        
        RecentFilesTableWidget* m_recentFilesTableWidget;
        
        std::unique_ptr<RecentFileItemsContainer> m_currentDirectoryItemsContainer;
        
        std::unique_ptr<RecentFileItemsContainer> m_recentFilesItemsContainer;
        
        std::unique_ptr<RecentFileItemsContainer> m_recentDirectoryItemsContainer;
        
        std::unique_ptr<RecentFileItemsContainer> m_favoriteItemsContainer;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RECENT_FILES_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __RECENT_FILES_DIALOG_DECLARE__

} // namespace
#endif  //__RECENT_FILES_DIALOG_H__

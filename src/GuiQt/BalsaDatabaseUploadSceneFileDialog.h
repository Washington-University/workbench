#ifndef __BALSA_DATABASE_UPLOAD_SCENE_FILE_DIALOG_H__
#define __BALSA_DATABASE_UPLOAD_SCENE_FILE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include <QIcon>

#include "WuQDialogModal.h"

class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QRadioButton;
class QRegularExpressionValidator;

namespace caret {
    class BalsaDatabaseManager;
    class SceneFile;
    
    class BalsaDatabaseUploadSceneFileDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        BalsaDatabaseUploadSceneFileDialog(SceneFile* sceneFile,
                                           QWidget* parent);
        
        virtual ~BalsaDatabaseUploadSceneFileDialog();
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void labelHtmlLinkClicked(const QString&);

        void browseBaseDirectoryPushButtonClicked();

        void validateData();
        
        void selectStudyTitleButtonClicked();
        
        void zipFileDirectoryRadioButtonClicked(int);
        
        void browseZipFileCustomDirectoryPushButtonClicked();
        
        void findBaseDirectoryPushButtonClicked();
        
    protected:
        virtual void okButtonClicked();
        
    private:
        enum class LabelName {
            LABEL_BASE_DIRECTORY,
            LABEL_DATABASE,
            LABEL_EXTRACT_DIRECTORY,
            LABEL_PASSWORD,
            LABEL_STUDY_ID,
            LABEL_STUDY_TITLE,
            LABEL_USERNAME
        };
        
        BalsaDatabaseUploadSceneFileDialog(const BalsaDatabaseUploadSceneFileDialog&);

        BalsaDatabaseUploadSceneFileDialog& operator=(const BalsaDatabaseUploadSceneFileDialog&);
        
        QWidget* createUploadTab();
        
        QWidget* createAdvancedTab();
        
        void loadSceneFileMetaDataWidgets();
        
        AString getDataBaseURL() const;
        
        QRegularExpressionValidator* createValidator(const LabelName labelName);
        
        AString getZipFileNameWithPath(AString& errorMessageOut) const;
        
        void updateAllLabels();
        
        void setLabelText(const LabelName labelName);
        
        SceneFile* m_sceneFile;
        
        std::unique_ptr<BalsaDatabaseManager> m_balsaDatabaseManager;
        
        QLabel* m_databaseLabel;
        QComboBox* m_databaseComboBox;
        
        QLabel* m_usernameLabel;
        QLineEdit* m_usernameLineEdit;
        
        QLabel* m_passwordLabel;
        QLineEdit* m_passwordLineEdit;
        
        QRadioButton* m_zipFileTemporaryDirectoryRadioButton;
        QRadioButton* m_zipFileCustomDirectoryRadioButton;
        QLineEdit* m_zipFileCustomDirectoryLineEdit;
        
        QLabel* m_extractDirectoryNameLabel;
        QLineEdit* m_extractDirectoryNameLineEdit;

        QLabel* m_balsaStudyIDLabel;
        QLineEdit* m_balsaStudyIDLineEdit;
        
        QLabel* m_balsaStudyTitleLabel;
        QLineEdit* m_balsaStudyTitleLineEdit;
        QPushButton* m_selectStudyTitlePushButton;
        
        QPushButton* m_browseBaseDirectoryPushButton;
        QPushButton* m_findBaseDirectoryPushButton;
        
        QLabel* m_baseDirectoryLabel;
        QLineEdit* m_baseDirectoryLineEdit;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BALSA_DATABASE_UPLOAD_SCENE_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BALSA_DATABASE_UPLOAD_SCENE_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__BALSA_DATABASE_UPLOAD_SCENE_FILE_DIALOG_H__

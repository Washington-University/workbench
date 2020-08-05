#ifndef __SCENE_DIALOG__H_
#define __SCENE_DIALOG__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <QGroupBox>
#include <QIcon>

#include "EventListenerInterface.h"
#include "GuiManager.h"
#include "WuQDialogNonModal.h"

class QCheckBox;
class QCloseEvent;
class QComboBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QVBoxLayout;

namespace caret {

    class Scene;
    class SceneClassInfoWidget;
    class SceneFile;
    class SceneInfo;
    class WuQDataEntryDialog;
    class WuQWidgetObjectGroup;
    
    class SceneDialog : public WuQDialogNonModal, public EventListenerInterface {
        Q_OBJECT
        
    public:
        SceneDialog(QWidget* parent = 0);
        
        virtual ~SceneDialog();
        
        void updateDialog();
        
        void receiveEvent(Event* event);
        
        bool displayScene(SceneFile* sceneFile,
                          Scene* scene);
        
        void createDefaultSceneFile();
        
        static bool isInformUserAboutScenesOnExit();

        static bool checkForModifiedFiles(const GuiManager::TestModifiedMode testMode,
                                          QWidget* parent);
        
        static bool displaySceneWithErrorMessageDialog(QWidget* dialogParent,
                                                       SceneFile* sceneFile,
                                                       Scene* scene);

    private:
        SceneDialog(const SceneDialog&);

        SceneDialog& operator=(const SceneDialog&);
        
    private slots:
        virtual void closeButtonClicked();
        
        void sceneFileSelected(int);
        
        void newSceneFileButtonClicked();
        
        void openSceneFileButtonClicked();
        
        void saveSceneFileButtonClicked();
        
        void saveAsSceneFileButtonClicked();
        
        void uploadSceneFileButtonClicked();
        
        void zipSceneFileButtonClicked();
        
        void addNewSceneButtonClicked();
                
        void deleteSceneButtonClicked();
        
        void insertSceneButtonClicked();
        
        void moveSceneUpButtonClicked();
        
        void moveSceneDownButtonClicked();
        
        void replaceSceneButtonClicked();
        
        void showSceneButtonClicked();
        
        void showImagePreviewButtonClicked();
        
        void showSceneOptionsButtonClicked();
        
        void validateContentOfCreateSceneDialog(WuQDataEntryDialog*);
        
        void sceneHighlighted(const int32_t sceneIndex);
        
        void sceneActivated(const int32_t sceneIndex);
        
        void useSceneColorsCheckBoxClicked(bool checked);
        
        void testScenesPushButtonClicked();
        
        void replaceAllScenesPushButtonClicked();
        
        void showFileStructure();
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void closeEvent(QCloseEvent* event);
    
        virtual void helpButtonClicked() override;
        
    private:
        SceneFile* getSelectedSceneFile();
        
        Scene* getSelectedScene();
        
        void loadSceneFileComboBox(SceneFile* selectedSceneFileIn);
        
        void updateSceneFileComboBoxModifiedIcons();
        
        void loadScenesIntoDialog(Scene* selectedSceneIn);
        
        int32_t getNumberOfValidSceneInfoWidgets() const;
        
        void addImageToScene(Scene* scene);
        
        void highlightSceneAtIndex(const int32_t sceneIndex);
        
        void highlightScene(const Scene* scene);
        
        QWidget* createScenesWidget();
        
        QWidget* createSceneFileWidget();
        
        static bool displayScenePrivateWithErrorMessageDialog(QWidget* dialogParent,
                                                              SceneFile* sceneFile,
                                                              Scene* scene,
                                                              const bool showWaitCursor);

        static bool displayScenePrivateWithErrorMessage(SceneFile* sceneFile,
                                                        Scene* scene,
                                                        const bool showWaitCursor,
                                                        AString& errorMessageOut,
                                                        AString& warningMessageOut);

        bool displayNewSceneWarning();
        
        void enableSceneMoveUpAndDownButtons();
        
        void updateSceneFileModifiedStatusLabel();
        
        QImage* getQImageFromSceneInfo(const SceneInfo* sceneInfo) const;
        
        enum class ModifiedWarningType {
            CLOSE_DIALOG_BUTTON,
            FILE_COMBO_BOX_CHANGE_FILE,
            NEW_FILE_BUTTON,
            OPEN_FILE_BUTTON,
            UPLOAD_FILE_BUTTON,
            ZIP_FILE_BUTTON
        };
        
        enum class MissingFilesMode {
            UPLOAD,
            ZIP
        };
        
        bool warnIfSceneFileIsModified(const ModifiedWarningType warningType);
        
        bool saveSelectedSceneFile();
        
        bool saveAsSelectedSceneFile();
        
        bool warnIfMissingFilesInSceneFile(SceneFile* sceneFile,
                                           const MissingFilesMode missingFilesMode);
        
        // ADD_NEW_MEMBERS_HERE

        QLabel*    m_sceneFileModifiedStatusLabel;
        
        QComboBox* m_sceneFileSelectionComboBox;
        
        QPushButton* m_showFileStructurePushButton;
        
        QPushButton* m_newSceneFilePushButton;
        
        QPushButton* m_openSceneFilePushButton;
        
        QPushButton* m_saveSceneFilePushButton;
        
        QPushButton* m_saveAsSceneFilePushButton;
        
        QPushButton* m_uploadSceneFilePushButton;
        
        QPushButton* m_zipSceneFilePushButton;
        
        QPushButton* m_addNewScenePushButton;
        
        QPushButton* m_insertNewScenePushButton;
        
        QPushButton* m_deleteScenePushButton;
        
        QPushButton* m_replaceAllScenesPushButton;
        
        QPushButton* m_testScenesPushButton;
        
        QPushButton* m_moveSceneUpPushButton;
        
        QPushButton* m_moveSceneDownPushButton;
        
        QPushButton* m_replaceScenePushButton;
        
        QPushButton* m_showScenePushButton;
        
        QPushButton* m_showSceneImagePreviewPushButton;
        
        QPushButton* m_showSceneOptionsPushButton;
        
        QScrollArea* m_sceneSelectionScrollArea;
        
        QWidget* m_sceneSelectionWidget;
        
        QVBoxLayout* m_sceneSelectionLayout;
        
        std::vector<SceneClassInfoWidget*> m_sceneClassInfoWidgets;
        
        int32_t m_selectedSceneClassInfoIndex;
        
        WuQWidgetObjectGroup* m_sceneFileButtonsGroup;
        
        AString m_replaceAllScenesDescription;
        
        AString m_testAllScenesDescription;
        
        QIcon m_cautionIcon;
        
        bool m_cautionIconValid = false;
        
        static const AString PREFERRED_IMAGE_FORMAT;
        
        static bool s_informUserAboutScenesOnExitFlag;
        
        static bool s_warnUserWhenCreatingSceneFlag;
        
        static bool s_useSceneForegroundBackgroundColorsFlag;
    };
    
    class SceneClassInfoWidget : public QGroupBox {
        Q_OBJECT
      
    public:
        SceneClassInfoWidget();
        
        ~SceneClassInfoWidget();
        
        void updateContent(Scene* scene,
                           const int32_t sceneIndex,
                           const bool activeSceneFlag);
        
        void setBackgroundForSelected(const bool selected);
        
        Scene* getScene();
        
        int32_t getSceneIndex() const;
        
        bool isValid() const;
        
        static void getFormattedTextForSceneNameAndDescription(const SceneInfo* sceneInfo,
                                                               const int32_t sceneIndex,
                                                               AString& nameTextOut,
                                                               AString& sceneIdTextOut,
                                                               AString& descriptionTextOut,
                                                               const int32_t maximumLinesInDescription);
        
    signals:
        /**
         * Emited when user activates (double clicks) this widget.
         */
        void activated(const int32_t sceneIndex);
        
        /**
         * Emited when user highlights (clicks) this widget.
         */
        void highlighted(const int32_t sceneIndex);
        
    protected:
        virtual void mousePressEvent(QMouseEvent* event);
        
        virtual void mouseDoubleClickEvent(QMouseEvent* event);
        
    private:
        static void limitToNumberOfLines(AString& textLines,
                                         const int32_t maximumNumberOfLines);
        
        QWidget* m_leftSideWidget;
        
        QWidget* m_rightSideWidget;
        
        QLabel* m_previewImageLabel;
        
        QLabel* m_activeSceneLabel;
        
        QLabel* m_nameLabel;
        
        QLabel* m_sceneIdLabel;
        
        QLabel* m_descriptionLabel;
        
        Scene* m_scene;
        
        int32_t m_sceneIndex;
        
        bool m_previewImageValid;
        
        bool m_defaultAutoFillBackgroundStatus;
        
        QPalette::ColorRole m_defaultBackgroundRole;
        
    };
#ifdef __SCENE_DIALOG_DECLARE__
    const AString SceneDialog::PREFERRED_IMAGE_FORMAT = "jpg";
    bool SceneDialog::s_informUserAboutScenesOnExitFlag = true;
    bool SceneDialog::s_warnUserWhenCreatingSceneFlag = true;
    bool SceneDialog::s_useSceneForegroundBackgroundColorsFlag = true;
    
#endif // __SCENE_DIALOG_DECLARE__

} // namespace
#endif  //__SCENE_DIALOG__H_

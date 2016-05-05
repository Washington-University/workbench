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

#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QCheckBox;
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
    
    class SceneDialog : public WuQDialogNonModal, public EventListenerInterface {
        Q_OBJECT
        
    public:
        SceneDialog(QWidget* parent = 0);
        
        virtual ~SceneDialog();
        
        void updateDialog();
        
        void receiveEvent(Event* event);
        
        bool displayScene(SceneFile* sceneFile,
                          Scene* scene);
        
        static bool isInformUserAboutScenesOnExit();

    private:
        SceneDialog(const SceneDialog&);

        SceneDialog& operator=(const SceneDialog&);
        
    private slots:
        void sceneFileSelected();
        
        void newSceneFileButtonClicked();
        
        void addNewSceneButtonClicked();
                
        void deleteSceneButtonClicked();
        
        void insertSceneButtonClicked();
        
        void moveSceneUpButtonClicked();
        
        void moveSceneDownButtonClicked();
        
        void replaceSceneButtonClicked();
        
        void showSceneButtonClicked();
        
        void showImagePreviewButtonClicked();
        
        void validateContentOfCreateSceneDialog(WuQDataEntryDialog*);
        
        void sceneHighlighted(const int32_t sceneIndex);
        
        void sceneActivated(const int32_t sceneIndex);
        
        void useSceneColorsCheckBoxClicked(bool checked);
        
        void editFileBalsaStudyIDButtonClicked();
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        SceneFile* getSelectedSceneFile();
        
        Scene* getSelectedScene();
        
        void loadSceneFileComboBox(SceneFile* selectedSceneFileIn);
        
        void loadScenesIntoDialog(Scene* selectedSceneIn);
        
        void addImageToScene(Scene* scene);
        
        void highlightSceneAtIndex(const int32_t sceneIndex);
        
        void highlightScene(const Scene* scene);
        
        QWidget* createMainPage();
        
        QWidget* createShowOptionsWidget();
        
        bool displayScenePrivate(SceneFile* sceneFile,
                                 Scene* scene,
                                 const bool showWaitCursor);
        
        bool checkForModifiedFiles(const bool creatingSceneFlag);
        
        void enableSceneMoveUpAndDownButtons();
        
        void loadSceneFileBalsaStudyIDLineEdit();
        
        // ADD_NEW_MEMBERS_HERE

        QComboBox* m_sceneFileSelectionComboBox;
        
        QPushButton* m_addNewScenePushButton;
        
        QPushButton* m_insertNewScenePushButton;
        
        QPushButton* m_deleteScenePushButton;
        
        QPushButton* m_moveSceneUpPushButton;
        
        QPushButton* m_moveSceneDownPushButton;
        
        QPushButton* m_replaceScenePushButton;
        
        QPushButton* m_showScenePushButton;
        
        QPushButton* m_showSceneImagePreviewPushButton;
        
        QCheckBox* m_useSceneColorsCheckBox;
        
        QScrollArea* m_sceneSelectionScrollArea;
        
        QWidget* m_sceneSelectionWidget;
        
        QVBoxLayout* m_sceneSelectionLayout;
        
        std::vector<SceneClassInfoWidget*> m_sceneClassInfoWidgets;
        
        int32_t m_selectedSceneClassInfoIndex;
        
        QPushButton* m_fileBalsaStudyIDPushButton;
        
        QLineEdit* m_fileBalsaStudyIDLineEdit;
        
        static const AString PREFERRED_IMAGE_FORMAT;
        
        static bool s_informUserAboutScenesOnExitFlag;
    };
    
    class SceneClassInfoWidget : public QGroupBox {
        Q_OBJECT
      
    public:
        SceneClassInfoWidget();
        
        ~SceneClassInfoWidget();
        
        void updateContent(Scene* scene,
                           const int32_t sceneIndex);
        
        void setBackgroundForSelected(const bool selected);
        
        Scene* getScene();
        
        int32_t getSceneIndex() const;
        
        bool isValid() const;
        
        static void getFormattedTextForSceneNameAndDescription(const SceneInfo* sceneInfo,
                                                               AString& nameTextOut,
                                                               AString& sceneIdTextOut,
                                                               AString& descriptionTextOut);
        
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
        QWidget* m_leftSideWidget;
        
        QWidget* m_rightSideWidget;
        
        QLabel* m_previewImageLabel;
        
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
#endif // __SCENE_DIALOG_DECLARE__

} // namespace
#endif  //__SCENE_DIALOG__H_

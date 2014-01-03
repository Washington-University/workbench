#ifndef __SCENE_DIALOG__H_
#define __SCENE_DIALOG__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <QGroupBox>

#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QCheckBox;
class QComboBox;
class QLabel;
class QPushButton;
class QVBoxLayout;

namespace caret {

    class Scene;
    class SceneClassInfoWidget;
    class SceneFile;
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
        
    private:
        SceneDialog(const SceneDialog&);

        SceneDialog& operator=(const SceneDialog&);
        
    private slots:
        void sceneFileSelected();
        
        void newSceneFileButtonClicked();
        
        void addNewSceneButtonClicked();
                
        void deleteSceneButtonClicked();
        
        void replaceSceneButtonClicked();
        
        void showSceneButtonClicked();
        
        void showImagePreviewButtonClicked();
        
        void validateContentOfCreateSceneDialog(WuQDataEntryDialog*);
        
        void sceneWasDropped();
        
        void sceneHighlighted(const int32_t sceneIndex);
        
        void sceneActivated(const int32_t sceneIndex);
        
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
        
        bool displayScenePrivate(SceneFile* sceneFile,
                                 Scene* scene,
                                 const bool showWaitCursor);
        
        bool checkForModifiedFiles();
        
        // ADD_NEW_MEMBERS_HERE

        QComboBox* m_sceneFileSelectionComboBox;
        
        QPushButton* m_addNewScenePushButton;
        
        QPushButton* m_deleteScenePushButton;
        
        QPushButton* m_replaceScenePushButton;
        
        QPushButton* m_showScenePushButton;
        
        QPushButton* m_showSceneImagePreviewPushButton;
        
        QWidget* m_sceneSelectionWidget;
        
        QVBoxLayout* m_sceneSelectionLayout;
        
        std::vector<SceneClassInfoWidget*> m_sceneClassInfoWidgets;
        
        int32_t m_selectedSceneClassInfoIndex;
        
        static const AString PREFERRED_IMAGE_FORMAT;
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
        QLabel* m_previewImageLabel;
        
        QLabel* m_nameLabel;
        
        QLabel* m_descriptionLabel;
        
        Scene* m_scene;
        
        int32_t m_sceneIndex;
        
        bool m_previewImageValid;
        
        bool m_defaultAutoFillBackgroundStatus;
        QPalette::ColorRole m_defaultBackgroundRole;
        
    };
#ifdef __SCENE_DIALOG_DECLARE__
    const AString SceneDialog::PREFERRED_IMAGE_FORMAT = "jpg";
#endif // __SCENE_DIALOG_DECLARE__

} // namespace
#endif  //__SCENE_DIALOG__H_

#ifndef __SCENE_CREATE_REPLACE_DIALOG_H__
#define __SCENE_CREATE_REPLACE_DIALOG_H__

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


#include "WuQDialogModal.h"

class QCheckBox;
class QLineEdit;
class QPlainTextEdit;


namespace caret {

    class Scene;
    class SceneFile;
    
    class SceneCreateReplaceDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static Scene* createNewScene(QWidget* parent,
                                     SceneFile* sceneFile);
        
        static void editSceneInfo(QWidget* parent,
                                  SceneFile* sceneFile,
                                  Scene* scene);
        
        static Scene* createNewSceneInsertBeforeScene(QWidget* parent,
                                                     SceneFile* sceneFile,
                                                     const Scene* insertBeforeScene);
        
        static Scene* replaceExistingScene(QWidget* parent,
                                    SceneFile* sceneFile,
                                    Scene* sceneToReplace);
        
        static void addImageAndWorkbenchInfoToScene(Scene* scene,
                                                    const bool cropImageFlag,
                                                    AString& errorMessageOut);

        static bool createSceneImage(QImage& imageOut,
                                     const bool cropImageFlag,
                                     AString& errorMessageOut);
        
        virtual ~SceneCreateReplaceDialog();
        
    private:
        enum Mode {
            MODE_ADD_NEW_SCENE,
            MODE_EDIT_SCENE_INFO,
            MODE_INSERT_NEW_SCENE,
            MODE_REPLACE_SCENE
        };
        
        SceneCreateReplaceDialog(const AString& dialogTitle,
                                 QWidget* parent,
                                 SceneFile* sceneFile,
                                 const Mode mode,
                                 Scene* sceneToInsertOrReplace);
        
        SceneCreateReplaceDialog(const SceneCreateReplaceDialog&);

        SceneCreateReplaceDialog& operator=(const SceneCreateReplaceDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private slots:
        void addWindowContentToolButtonClicked();
        
    private:
        // ADD_NEW_MEMBERS_HERE

        SceneFile* m_sceneFile;
        
        Mode m_mode;
        
        Scene* m_sceneToInsertOrReplace;
        
        /** Scene that was created DO NOT DESTROY SINCE RETURNED TO CALLER */
        Scene* m_sceneThatWasCreated;
        
        QWidget* createSceneOptionsWidget();
        
        QLineEdit* m_nameLineEdit;
        
        QLineEdit* m_balsaSceneIDLineEdit;
        
        QPlainTextEdit* m_descriptionTextEdit;
        
        QCheckBox* m_addSpecFileNameToSceneCheckBox;
        
        QCheckBox* m_addAllTabsCheckBox;
        
        QCheckBox* m_addAllLoadedFilesCheckBox;
        
        QCheckBox* m_addModifiedPaletteSettingsCheckBox;
        
        QCheckBox* m_cropImageCheckBox;
        
        AString m_sceneWindowDescription;
        
        struct PreviousSelections {
            bool m_addSpecFileNameToScene;
            bool m_addAllTabs;
            bool m_addAllLoadedFiles;
            bool m_addModifiedPaletteSettings;
            bool m_cropImage;
        };
        
        static PreviousSelections s_previousSelections;
        static bool s_previousSelectionsValid;
    };
    
#ifdef __SCENE_CREATE_REPLACE_DIALOG_DECLARE__
    SceneCreateReplaceDialog::PreviousSelections SceneCreateReplaceDialog::s_previousSelections;
    bool SceneCreateReplaceDialog::s_previousSelectionsValid = false;
#endif // __SCENE_CREATE_REPLACE_DIALOG_DECLARE__

} // namespace
#endif  //__SCENE_CREATE_REPLACE_DIALOG_H__

#ifndef __SCENE_CREATE_REPLACE_DIALOG_H__
#define __SCENE_CREATE_REPLACE_DIALOG_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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
        
        static Scene* replaceExistingScene(QWidget* parent,
                                    SceneFile* sceneFile,
                                    Scene* sceneToReplace);
        
        virtual ~SceneCreateReplaceDialog();
        
    private:
        SceneCreateReplaceDialog(const AString& dialogTitle,
                                 QWidget* parent,
                                 SceneFile* sceneFile,
                                 Scene* sceneToReplace);
        
        SceneCreateReplaceDialog(const SceneCreateReplaceDialog&);

        SceneCreateReplaceDialog& operator=(const SceneCreateReplaceDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private:
        // ADD_NEW_MEMBERS_HERE

        void addImageToScene(Scene* scene);
        
        SceneFile* m_sceneFile;
        
        Scene* m_sceneToReplace;
        
        Scene* m_sceneThatWasCreated;
        
        QWidget* createSceneOptionsWidget();
        
        QLineEdit* m_nameLineEdit;
        
        QPlainTextEdit* m_descriptionTextEdit;
        
        QCheckBox* m_addSpecFileNameToSceneCheckBox;
        
        QCheckBox* m_addAllTabsCheckBox;
        
        QCheckBox* m_addAllLoadedFilesCheckBox;
        
        QCheckBox* m_addModifiedPaletteSettingsCheckBox;
        
        struct PreviousSelections {
            bool m_addSpecFileNameToScene;
            bool m_addAllTabs;
            bool m_addAllLoadedFiles;
            bool m_addModifiedPaletteSettings;
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

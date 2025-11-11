#ifndef __SCENE_FILE_RESTRUCTURE_DIALOG_H__
#define __SCENE_FILE_RESTRUCTURE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include "OperationSceneFileRestructure.h"
#include "WuQDialogModal.h"

class QCheckBox;
class QComboBox;
class QLineEdit;

namespace caret {

    class SceneFileRestructureDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        SceneFileRestructureDialog(const AString& specFileName,
                               QWidget* parent = 0);
        
        virtual ~SceneFileRestructureDialog();
        
        SceneFileRestructureDialog(const SceneFileRestructureDialog&) = delete;

        SceneFileRestructureDialog& operator=(const SceneFileRestructureDialog&) = delete;
        
        virtual void okButtonClicked() override;
        
        bool isSuccess() const;
        
    private slots:
        void sceneFilePushButtonClicked();
        
        void outputDirectoryPushButtonClicked();
        

        // ADD_NEW_METHODS_HERE

    private:
        void initializeWithSceneFile(const AString& sceneFileName);
        
        QLineEdit* m_sceneFileNameLineEdit;
        
        QLineEdit* m_outputDirectoryLineEdit;
        
        QComboBox* m_overwriteFilesComboBox;
        
        QCheckBox* m_previewCheckBox;
        
        QCheckBox* m_skipMissingFilesCheckBox;
        
        QCheckBox* m_testScenesCheckBox;
        
        bool m_successFlag = false;
        
        static AString s_previousSceneFileName;
        
        static AString s_previousOutputDirectory;
        
        static OperationSceneFileRestructure::OverwriteFilesMode s_previousOvewriteFilesMode;
        
        static bool s_previousPreviewFlag;
        
        static bool s_previousSkipMissingFilessFlag;
        
        static bool s_previousTestScenesFlag;
        

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_FILE_RESTRUCTURE_DIALOG_DECLARE__
    AString SceneFileRestructureDialog::s_previousSceneFileName;
    
    AString SceneFileRestructureDialog::s_previousOutputDirectory;
    
    OperationSceneFileRestructure::OverwriteFilesMode SceneFileRestructureDialog::s_previousOvewriteFilesMode = OperationSceneFileRestructure::OverwriteFilesMode::OVERWRITE_ERROR;

    bool SceneFileRestructureDialog::s_previousPreviewFlag = false;
    
    bool SceneFileRestructureDialog::s_previousSkipMissingFilessFlag = false;
    
    bool SceneFileRestructureDialog::s_previousTestScenesFlag = false;
    
#endif // __SCENE_FILE_RESTRUCTURE_DIALOG_DECLARE__

} // namespace
#endif  //__SCENE_FILE_RESTRUCTURE_DIALOG_H__

#ifndef __SCENE_FILE_INFORMATION_DIALOG_H__
#define __SCENE_FILE_INFORMATION_DIALOG_H__

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

#include "WuQDialogNonModal.h"

class QComboBox;
class QLineEdit;
class QTextEdit;
class QTreeView;

namespace caret {

    class SceneDataFileTreeItemModel;
    class SceneFile;
    
    class SceneFileInformationDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        SceneFileInformationDialog(const SceneFile* sceneFile,
                                   QWidget* parent);
        
        virtual ~SceneFileInformationDialog();
        
        SceneFileInformationDialog(const SceneFileInformationDialog&) = delete;

        SceneFileInformationDialog& operator=(const SceneFileInformationDialog&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void modeComboBoxActivated(int);
        
    private:
        void displayFiles(int modeInteger);
        
        const SceneFile* m_sceneFile;
        
        QComboBox* m_modeComboBox;
        
        QTextEdit* m_textEdit;
        
        QTreeView* m_sceneFileHierarchyTreeView;
        
        std::unique_ptr<SceneDataFileTreeItemModel> m_sceneFileHierarchyTreeModel;
        
        QLineEdit* m_basePathLineEdit;
        
        QLineEdit* m_sceneFileNameLineEdit;
        
        QLineEdit* m_sceneFilePathLineEdit;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __SCENE_FILE_INFORMATION_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SCENE_FILE_INFORMATION_DIALOG_DECLARE__

} // namespace
#endif  //__SCENE_FILE_INFORMATION_DIALOG_H__

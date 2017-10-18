#ifndef __ZIP_SCENE_FILE_DIALOG_H__
#define __ZIP_SCENE_FILE_DIALOG_H__

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


#include "WuQDialogModal.h"

class QLabel;
class QLineEdit;
class QPushButton;
class QRegularExpressionValidator;

namespace caret {

    class SceneFile;
    class SceneBasePathWidget;
    
    class ZipSceneFileDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        ZipSceneFileDialog(SceneFile* sceneFile,
                           QWidget* parent);
        
        virtual ~ZipSceneFileDialog();
        


        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
    
    private slots:
        void chooseZipFileButtonClicked();
        
        void validateData();
        
    private:
        enum class LabelName {
            EXTRACT_DIRECTORY,
            ZIP_FILE
        };
        
        ZipSceneFileDialog(const ZipSceneFileDialog&);

        ZipSceneFileDialog& operator=(const ZipSceneFileDialog&);
        
        QRegularExpressionValidator* createValidator(const LabelName labelName);
        
        void setLabelText(const LabelName labelName);
        
        SceneFile* m_sceneFile;
        
        QLabel* m_zipFileNameLabel;
        
        QLineEdit* m_zipFileNameLineEdit;
        
        QLabel* m_extractDirectoryLabel;
        
        QLineEdit* m_extractDirectoryNameLineEdit;
        
        SceneBasePathWidget* m_basePathWidget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ZIP_SCENE_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZIP_SCENE_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__ZIP_SCENE_FILE_DIALOG_H__

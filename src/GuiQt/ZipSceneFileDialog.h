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

class QLineEdit;



namespace caret {

    class SceneFile;
    
    class ZipSceneFileDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        ZipSceneFileDialog(const SceneFile* sceneFile,
                           QWidget* parent);
        
        virtual ~ZipSceneFileDialog();
        


        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
    
    private slots:
        void chooseZipFileButtonClicked();
        
    private:
        ZipSceneFileDialog(const ZipSceneFileDialog&);

        ZipSceneFileDialog& operator=(const ZipSceneFileDialog&);
        
        const SceneFile* m_sceneFile;
        
        QLineEdit* m_zipFileNameLineEdit;
        
        QLineEdit* m_extractDirectoryNameLineEdit;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ZIP_SCENE_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZIP_SCENE_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__ZIP_SCENE_FILE_DIALOG_H__

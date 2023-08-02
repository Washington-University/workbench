#ifndef __ANNOTATION_MENU_FILE_SELECTION_H__
#define __ANNOTATION_MENU_FILE_SELECTION_H__

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


#include <QMenu>

#include "AString.h"
#include "UserInputModeEnum.h"

namespace caret {

    class AnnotationFile;
    
    class AnnotationMenuFileSelection : public QMenu {
        
        Q_OBJECT

    public:
        AnnotationMenuFileSelection(const UserInputModeEnum::Enum userInputMode,
                                    QWidget* parent = 0);
        
        virtual ~AnnotationMenuFileSelection();

        AnnotationFile* getSelectedAnnotationFile();

        AString getSelectedNameForToolButton();
        
        // ADD_NEW_METHODS_HERE

    signals:
        void menuItemSelected();
        
    private slots:
        void updateMenuContents();
        
        void menuActionSelected(QAction* action);
        
    private:
        enum {
            ACTION_ID_SCENE           = -2,
            ACTION_ID_NEW_DISK_FILE   = -1,
            ACTION_ID_FIRST_DISK_FILE = 0
        };
        
        enum class FileMode {
            INVALID,
            ANNOTATIONS,
            SAMPLES
        };
        
        AnnotationMenuFileSelection(const AnnotationMenuFileSelection&);

        AnnotationMenuFileSelection& operator=(const AnnotationMenuFileSelection&);
        
        void chooseDiskFile();
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        FileMode m_fileMode = FileMode::INVALID;
        
        std::vector<AnnotationFile*> m_annotationDiskFiles;
        
        AnnotationFile* m_selectedAnnotationFile;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_MENU_FILE_SELECTION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_MENU_FILE_SELECTION_DECLARE__

} // namespace
#endif  //__ANNOTATION_MENU_FILE_SELECTION_H__

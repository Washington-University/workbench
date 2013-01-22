#ifndef __MANAGE_LOADED_FILES_DIALOG__H_
#define __MANAGE_LOADED_FILES_DIALOG__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include "WuQDialogModal.h"
#include "DataFileException.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QToolButton;

namespace caret {

    class Brain;
    class CaretDataFile;
    class CaretMappableDataFile;
    class ManageFileRow;
    class WuQWidgetObjectGroup;
    
    class ManageLoadedFilesDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        ManageLoadedFilesDialog(QWidget* parent,
                                Brain* brain,
                                const bool isQuittingWorkbench);
        
        virtual ~ManageLoadedFilesDialog();

    protected:
        virtual ModalDialogUserButtonResult userButtonPressed(QPushButton* userPushButton);        
        
    private:
        ManageLoadedFilesDialog(const ManageLoadedFilesDialog&);

        ManageLoadedFilesDialog& operator=(const ManageLoadedFilesDialog&);
        
        enum Column {
            COLUMN_SAVE_CHECKBOX,
            COLUMN_DISPLAYED,
            COLUMN_MODIFIED,
            COLUMN_STRUCTURE,
            COLUMN_FILE_TYPE,
            COLUMN_METADATA,
            COLUMN_REMOVE_BUTTON,
            COLUMN_REMOVE_MAP_BUTTON,
            COLUMN_FILE_NAME_BUTTON,
            COLUMN_FILE_NAME,
            COLUMN_LAST   // MUST BE LAST
        };
        
        void updateUserInterfaceAndGraphics();
        
        QPushButton* saveCheckedFilesPushButton;
        
        QCheckBox* addSavedFilesToSpecFileCheckBox;
        
        std::vector<ManageFileRow*> fileRows;
        
        Brain* brain;
        
        bool isQuittingWorkbench;
        
        static bool firstWindowFlag;
        
        friend class ManageFileRow;
        
    };
    
    
    class ManageFileRow : public QObject {
        Q_OBJECT
    private:
        ManageFileRow(ManageLoadedFilesDialog* parentWidget,
                      Brain* brain,
                      CaretDataFile* caretDataFile,
                      const bool caretDataFileDisplayedFlag);
        
        ~ManageFileRow();
        
        ManageLoadedFilesDialog* parentWidget;
        CaretDataFile* caretDataFile;
        CaretMappableDataFile* caretMappableDataFile;
        
        QCheckBox* saveCheckBox;
        QLabel* structureLabel;
        QLabel* fileTypeLabel;
        QLabel* displayedLabel;
        QLabel* modifiedLabel;
        QToolButton* metaDataToolButton;
        QToolButton* removeFileToolButton;
        QToolButton* removeMapToolButton;
        QToolButton* fileNameToolButton;
        QLineEdit* fileNameLineEdit; 
        
        WuQWidgetObjectGroup* widgetGroup;
        
        Brain* brain;
        
        void saveFile(const bool isAddToSpecFile) throw (DataFileException);
        
    private slots:
        void metaDataToolButtonPressed();
        void removeFileToolButtonPressed();
        void removeMapToolButtonPressed();
        void fileNameToolButtonPressed();
        
    private:
        friend class ManageLoadedFilesDialog;
    };
    
#ifdef __MANAGE_LOADED_FILES_DIALOG_DECLARE__
    bool ManageLoadedFilesDialog::firstWindowFlag = true;
#endif // __MANAGE_LOADED_FILES_DIALOG_DECLARE__

} // namespace
#endif  //__MANAGE_LOADED_FILES_DIALOG__H_

#ifndef __CARET_FIVE_FILE_IMPORT_DIALOG_H__
#define __CARET_FIVE_FILE_IMPORT_DIALOG_H__

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

#include "WuQDialogModal.h"

class QCheckBox;
class QComboBox;
class QLineEdit;

namespace caret {

    class CaretFiveFileImportDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        CaretFiveFileImportDialog(QWidget* parent = 0);
        
        virtual ~CaretFiveFileImportDialog();
        
        CaretFiveFileImportDialog(const CaretFiveFileImportDialog&) = delete;

        CaretFiveFileImportDialog& operator=(const CaretFiveFileImportDialog&) = delete;
        

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked() override;
        
        virtual void cancelButtonClicked() override;
        
    private slots:
        void fileSelectionActionTriggered();
        
        void outputDirectorySelectionActionTriggered();
        
    private:
        void saveValuesForNextTime();
        
        QComboBox* m_fileFilterComboBox;
        
        QLineEdit* m_filenameLineEdit;
        
        QLineEdit* m_outputDirectoryLineEdit;
        
        QLineEdit* m_convertedFileNamePrefix;
        
        QCheckBox* m_saveConvertedFilesCheckBox;
        
        static QString s_previousImportFileName;
        
        static QString s_previousImportFileType;
        
        static QString s_previousDirectory;
        
        static QString s_previousFileNamePrefix;
        
        static bool s_previousSaveConvertedFiles;
        
        static bool s_firstTimeFlag;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CARET_FIVE_FILE_IMPORT_DIALOG_DECLARE__
    QString CaretFiveFileImportDialog::s_previousImportFileName;
    
    QString CaretFiveFileImportDialog::s_previousImportFileType;
    
    QString CaretFiveFileImportDialog::s_previousDirectory;
    
    QString CaretFiveFileImportDialog::s_previousFileNamePrefix;
    
    bool CaretFiveFileImportDialog::s_previousSaveConvertedFiles;
    
    bool CaretFiveFileImportDialog::s_firstTimeFlag = true;
#endif // __CARET_FIVE_FILE_IMPORT_DIALOG_DECLARE__

} // namespace
#endif  //__CARET_FIVE_FILE_IMPORT_DIALOG_H__

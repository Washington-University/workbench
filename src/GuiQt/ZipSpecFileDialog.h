#ifndef __ZIP_SPEC_FILE_DIALOG_H__
#define __ZIP_SPEC_FILE_DIALOG_H__

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
class QLineEdit;

namespace caret {

    class ZipSpecFileDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        ZipSpecFileDialog(const AString& specFileName,
                          QWidget* parent);
        
        virtual ~ZipSpecFileDialog();
        
        ZipSpecFileDialog(const ZipSpecFileDialog&) = delete;

        ZipSpecFileDialog& operator=(const ZipSpecFileDialog&) = delete;
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void specFileNamePushButtonClicked();
        
        void zipFileNamePushButtonClicked();
        
        void baseDirectoryNamePushButtonClicked();
        
    protected:
        void initializeWithSpecFile(const AString& specFileName);
        
        virtual void okButtonClicked() override;
        
    private:
        QLineEdit* m_specFileNameLineEdit;
        
        QLineEdit* m_extractDirectoryLineEdit;
        
        QLineEdit* m_zipFileNameLineEdit;
        
        QCheckBox* m_baseDirectoryCheckBox;
        
        QLineEdit* m_baseDirectoryLineEdit;
        
        QCheckBox* m_skipMissingFilesCheckBox;
        
        AString m_directory;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ZIP_SPEC_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ZIP_SPEC_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__ZIP_SPEC_FILE_DIALOG_H__

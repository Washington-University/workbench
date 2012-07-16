#ifndef __CARET_FILE_REMOTE_DIALOG__H_
#define __CARET_FILE_REMOTE_DIALOG__H_

/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
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

#include "DataFileTypeEnum.h"
#include "WuQDialogModal.h"

class QComboBox;
class QLineEdit;

namespace caret {

    class CaretFileRemoteDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        CaretFileRemoteDialog(QWidget* parent = 0);
        
        virtual ~CaretFileRemoteDialog();
        
    protected:
        void okButtonClicked();
        
    private:
        CaretFileRemoteDialog(const CaretFileRemoteDialog&);

        CaretFileRemoteDialog& operator=(const CaretFileRemoteDialog&);
        
        QComboBox* m_fileTypeComboBox;
        QLineEdit* m_urlLineEdit;
        QLineEdit* m_usernameLineEdit;
        QLineEdit* m_passwordLineEdit;
        
        static DataFileTypeEnum::Enum previousNetworkDataFileType;
        static AString previousNetworkFileName;
        static AString previousNetworkUsername;
        static AString previousNetworkPassword;
    };
    
#ifdef __CARET_FILE_REMOTE_DIALOG_DECLARE__
    DataFileTypeEnum::Enum CaretFileRemoteDialog::previousNetworkDataFileType = DataFileTypeEnum::UNKNOWN;
    AString CaretFileRemoteDialog::previousNetworkFileName = "";
    AString CaretFileRemoteDialog::previousNetworkUsername = "";
    AString CaretFileRemoteDialog::previousNetworkPassword = "";
#endif // __CARET_FILE_REMOTE_DIALOG_DECLARE__

} // namespace
#endif  //__CARET_FILE_REMOTE_DIALOG__H_

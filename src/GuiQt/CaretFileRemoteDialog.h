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
class QRadioButton;

namespace caret {
    class EnumComboBoxTemplate;
    class WuQWidgetObjectGroup;

    class CaretFileRemoteDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        CaretFileRemoteDialog(QWidget* parent = 0);
        
        virtual ~CaretFileRemoteDialog();
        
    protected:
        void okButtonClicked();
        
    private slots:
        void locationSourceRadioButtonClicked(QAbstractButton* button);
        
        void selectCustomRadioButton();
        
        void selectStandardRadioButton();
        
    private:
        CaretFileRemoteDialog(const CaretFileRemoteDialog&);

        CaretFileRemoteDialog& operator=(const CaretFileRemoteDialog&);
        
        class StandardData {
        public:
            StandardData(const AString& userFriendlyName,
                         const AString locationUrl,
                         const DataFileTypeEnum::Enum dataFileType)
            : m_userFriendlyName(userFriendlyName),
            m_locationUrl(locationUrl),
            m_dataFileType(dataFileType) { }
            
            AString m_userFriendlyName;
            AString m_locationUrl;
            DataFileTypeEnum::Enum m_dataFileType;
        };
        
        class PreviousSelections {
        public:
            PreviousSelections() {
                m_customURL = "http://";
                m_customDataFileType = DataFileTypeEnum::CONNECTIVITY_DENSE;
                m_standardFileComboBoxIndex = 0;
                m_username = "wbuser";
                m_password = "hcpWb0512";
            }
            
            AString m_customURL;
            DataFileTypeEnum::Enum m_customDataFileType;
            int m_standardFileComboBoxIndex;
            AString m_username;
            AString m_password;
            AString m_radioButtonText;
        };
        
        QWidget* createLocationWidget();
        
        QWidget* createLoginWidget();
        
        void createAndLoadStandardData();
        
        QRadioButton* m_locationCustomRadioButton;
        QRadioButton* m_locationStandardRadioButton;
        
        QComboBox* m_standardFileComboBox;
        EnumComboBoxTemplate* m_customUrlFileTypeComboBox;
        QLineEdit* m_customUrlLineEdit;
        QLineEdit* m_usernameLineEdit;
        QLineEdit* m_passwordLineEdit;
        
        WuQWidgetObjectGroup* m_customWidgetGroup;
        WuQWidgetObjectGroup* m_standardWidgetGroup;
        
        std::vector<StandardData> m_standardData;
        
        static PreviousSelections s_previousSelections;
    };
    
#ifdef __CARET_FILE_REMOTE_DIALOG_DECLARE__
    CaretFileRemoteDialog::PreviousSelections CaretFileRemoteDialog::s_previousSelections;
#endif // __CARET_FILE_REMOTE_DIALOG_DECLARE__

} // namespace
#endif  //__CARET_FILE_REMOTE_DIALOG__H_

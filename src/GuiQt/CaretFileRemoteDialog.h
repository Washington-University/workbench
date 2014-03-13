#ifndef __CARET_FILE_REMOTE_DIALOG__H_
#define __CARET_FILE_REMOTE_DIALOG__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "DataFileTypeEnum.h"
#include "WuQDialogModal.h"

class QCheckBox;
class QComboBox;
class QLineEdit;
class QRadioButton;

namespace caret {
    class EnumComboBoxTemplate;
    class UsernamePasswordWidget;
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
                m_firstTime = true;
            }
            
            AString m_customURL;
            DataFileTypeEnum::Enum m_customDataFileType;
            int m_standardFileComboBoxIndex;
            AString m_radioButtonText;
            bool m_firstTime;
        };
        
        QWidget* createLocationWidget();
        
        void createAndLoadStandardData();
        
        UsernamePasswordWidget* m_usernamePasswordWidget;
        
        QRadioButton* m_locationCustomRadioButton;
        QRadioButton* m_locationStandardRadioButton;
        
        QComboBox* m_standardFileComboBox;
        EnumComboBoxTemplate* m_customUrlFileTypeComboBox;
        QLineEdit* m_customUrlLineEdit;
        
        QCheckBox* m_savePasswordToPreferencesCheckBox;
        
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

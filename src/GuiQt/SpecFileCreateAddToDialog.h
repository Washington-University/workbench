#ifndef __SPEC_FILE_CREATE_ADD_TO_DIALOG__H_
#define __SPEC_FILE_CREATE_ADD_TO_DIALOG__H_

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

#include "WuQDialogModal.h"

class QLineEdit;
class QPushButton;

namespace caret {
    class Brain;
    
    class SpecFileCreateAddToDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        enum FileOpenSaveMode {
            MODE_OPEN,
            MODE_SAVE
        };
        
        SpecFileCreateAddToDialog(Brain* brain,
                                  const FileOpenSaveMode mode,
                                  QWidget* parent = 0);
        
        virtual ~SpecFileCreateAddToDialog();
        
        bool isAddToSpecFileSelected() const;
        
    private slots:
        void fileButtonClicked();
        
    protected:
        void okButtonClicked();
        
        virtual ModalDialogUserButtonResult userButtonPressed(QPushButton* userPushButton);        
        
    private:
        SpecFileCreateAddToDialog(const SpecFileCreateAddToDialog&);

        SpecFileCreateAddToDialog& operator=(const SpecFileCreateAddToDialog&);
        
        Brain* m_brain;
        
        AString m_specFileName;
        
        QLineEdit* m_specFileNameLineEdit;
        
        QPushButton* m_skipPushButton;
        
        bool m_isSpecFileValid;
        
        bool m_addFilesToSpecFileFlag;
    };
    
#ifdef __SPEC_FILE_CREATE_ADD_TO_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __SPEC_FILE_CREATE_ADD_TO_DIALOG_DECLARE__

} // namespace
#endif  //__SPEC_FILE_CREATE_ADD_TO_DIALOG__H_

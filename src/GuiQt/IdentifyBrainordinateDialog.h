#ifndef __IDENTIFY_BRAINORDINATE_DIALOG_H__
#define __IDENTIFY_BRAINORDINATE_DIALOG_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
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

#include "StructureEnum.h"
#include "WuQDialogModal.h"

class QComboBox;
class QRadioButton;
class QSpinBox;

namespace caret {

    class CaretMappableDataFile;
    class StructureEnumComboBox;
    
    class IdentifyBrainordinateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        IdentifyBrainordinateDialog(QWidget* parent);
        
        virtual ~IdentifyBrainordinateDialog();
        
    protected:
        virtual void okButtonClicked();
        
    private:
        IdentifyBrainordinateDialog(const IdentifyBrainordinateDialog&);

        IdentifyBrainordinateDialog& operator=(const IdentifyBrainordinateDialog&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        enum Mode {
            MODE_NONE,
            MODE_CIFTI_ROW,
            MODE_SURFACE_VERTEX
        };
        
        // ADD_NEW_MEMBERS_HERE
        
        QRadioButton* m_vertexRadioButton;
        
        StructureEnumComboBox* m_vertexStructureComboBox;
        
        QSpinBox* m_vertexIndexSpinBox;
        
        QRadioButton* m_ciftiFileRadioButton;
        
        QComboBox* m_ciftiFileComboBox;

        QSpinBox* m_ciftiFileRowIndexSpinBox;
        
        static StructureEnum::Enum s_lastSelectedStructure;
        
        static int32_t s_lastSelectedVertexIndex;
        
        static CaretMappableDataFile* s_lastSelectedCaretMappableDataFile;
        
        static int64_t s_lastSelectedCiftiRowIndex;
        
        static Mode s_lastMode;
        
    };
    
#ifdef __IDENTIFY_BRAINORDINATE_DIALOG_DECLARE__
    StructureEnum::Enum IdentifyBrainordinateDialog::s_lastSelectedStructure;
    
    int32_t IdentifyBrainordinateDialog::s_lastSelectedVertexIndex = 0;
    
    CaretMappableDataFile* IdentifyBrainordinateDialog::s_lastSelectedCaretMappableDataFile = NULL;
    
    int64_t IdentifyBrainordinateDialog::s_lastSelectedCiftiRowIndex = -1;
    
    IdentifyBrainordinateDialog::Mode IdentifyBrainordinateDialog::s_lastMode = IdentifyBrainordinateDialog::MODE_NONE;
    
#endif // __IDENTIFY_BRAINORDINATE_DIALOG_DECLARE__

} // namespace
#endif  //__IDENTIFY_BRAINORDINATE_DIALOG_H__

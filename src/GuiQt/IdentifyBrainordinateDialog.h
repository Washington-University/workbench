#ifndef __IDENTIFY_BRAINORDINATE_DIALOG_H__
#define __IDENTIFY_BRAINORDINATE_DIALOG_H__

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

#ifndef __PALETTE_EDITOR_DIALOG__H_
#define __PALETTE_EDITOR_DIALOG__H_

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


#include "WuQDialogNonModal.h"

class QComboBox;

namespace caret {
    
    class CaretMappableDataFile;
    class PaletteColorMapping;
    class WuQWidgetObjectGroup;
    
    class PaletteEditorDialog : public WuQDialogNonModal {
        Q_OBJECT
        
    public:
        PaletteEditorDialog(QWidget* parent);
        
        void updatePaletteEditor(CaretMappableDataFile* caretMappableDataFile,
                                 const int32_t mapIndex);
                    
        virtual ~PaletteEditorDialog();

    protected:
        virtual void applyButtonPressed();
        
    private:
        PaletteEditorDialog(const PaletteEditorDialog&);

        PaletteEditorDialog& operator=(const PaletteEditorDialog&);
        
    private:
        PaletteColorMapping* paletteColorMapping;
        
        QComboBox* paletteNameComboBox;
        
        WuQWidgetObjectGroup* widgetGroup;
    };
    
#ifdef __PALETTE_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__PALETTE_EDITOR_DIALOG__H_

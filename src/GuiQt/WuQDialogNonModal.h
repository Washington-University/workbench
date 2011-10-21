#ifndef __WU_Q_DIALOG_NON_MODAL__H_
#define __WU_Q_DIALOG_NON_MODAL__H_

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


#include "WuQDialog.h"

namespace caret {

    class WuQDialogNonModal : public WuQDialog {
        
    public:
        WuQDialogNonModal(QWidget* parent = 0,
                          Qt::WindowFlags f = 0);
        
        virtual ~WuQDialogNonModal();
        
    private:
        WuQDialogNonModal(const WuQDialogNonModal&);

        WuQDialogNonModal& operator=(const WuQDialogNonModal&);
        
    public:
    private:
    };
    
#ifdef __WU_Q_DIALOG_NON_MODAL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_DIALOG_NON_MODAL_DECLARE__

} // namespace
#endif  //__WU_Q_DIALOG_NON_MODAL__H_

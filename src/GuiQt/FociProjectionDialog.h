#ifndef __FOCI_PROJECTION_DIALOG__H_
#define __FOCI_PROJECTION_DIALOG__H_

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


#include "WuQDialogModal.h"

class QCheckBox;
class QDoubleSpinBox;

namespace caret {

    class FociFile;
    class SurfaceSelectionViewController;
    
    class FociProjectionDialog : public WuQDialogModal {
        Q_OBJECT
        
    public:
        FociProjectionDialog(QWidget* parent);
        
        virtual ~FociProjectionDialog();
        
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private:
        FociProjectionDialog(const FociProjectionDialog&);
        
        FociProjectionDialog& operator=(const FociProjectionDialog&);

        QWidget* createSurfaceSelectionWidget();
        
        QWidget* createFociFileSelectionWidget();
        
        QWidget* createOptionsWidget();
        
        QString m_objectNamePrefix;
        
        QCheckBox* m_leftSurfaceCheckBox;
        
        SurfaceSelectionViewController* m_leftSurfaceViewController;
        
        QCheckBox* m_rightSurfaceCheckBox;
        
        SurfaceSelectionViewController* m_rightSurfaceViewController;
        
        QCheckBox* m_cerebellumSurfaceCheckBox;
        
        SurfaceSelectionViewController* m_cerebellumSurfaceViewController;
        
        QCheckBox* m_projectAboveSurfaceCheckBox;
        
        QDoubleSpinBox* m_projectAboveSurfaceSpinBox;
        
        std::vector<FociFile*> m_fociFiles;
        
        std::vector<QCheckBox*> m_fociFileCheckBoxes;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __FOCI_PROJECTION_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FOCI_PROJECTION_DIALOG_DECLARE__

} // namespace
#endif  //__FOCI_PROJECTION_DIALOG__H_

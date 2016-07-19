#ifndef __IMAGE_EXPORT_TO_NIFTI_DIALOG_H__
#define __IMAGE_EXPORT_TO_NIFTI_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

class QLineEdit;

namespace caret {

    class EnumComboBoxTemplate;
    class ImageFile;
    
    class ImageExportToNiftiDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        ImageExportToNiftiDialog(QWidget* parent,
                                 const int32_t tabIndex,
                                 ImageFile* imageFile);
        
        virtual ~ImageExportToNiftiDialog();
        

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private:
        ImageExportToNiftiDialog(const ImageExportToNiftiDialog&);

        ImageExportToNiftiDialog& operator=(const ImageExportToNiftiDialog&);
        
        // ADD_NEW_MEMBERS_HERE

        const int32_t m_tabIndex;
        
        ImageFile* m_imageFile;
        
        QLineEdit* m_volumeFileNameLineEdit;
        
        EnumComboBoxTemplate* m_sliceViewPlaneComboBox;
    };
    
#ifdef __IMAGE_EXPORT_TO_NIFTI_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IMAGE_EXPORT_TO_NIFTI_DIALOG_DECLARE__

} // namespace
#endif  //__IMAGE_EXPORT_TO_NIFTI_DIALOG_H__

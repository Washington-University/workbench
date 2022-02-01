#ifndef __CZI_IMAGE_EXPORT_DIALOG_H__
#define __CZI_IMAGE_EXPORT_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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



#include <memory>

#include "WuQDialogModal.h"

class QCheckBox;
class QLabel;
class QLineEdit;
class QSpinBox;

namespace caret {

    class CziImageFile;

    class CziImageExportDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        CziImageExportDialog(CziImageFile* cziImageFile,
                             QWidget* parent = 0);
        
        virtual ~CziImageExportDialog();
        
        CziImageExportDialog(const CziImageExportDialog&) = delete;

        CziImageExportDialog& operator=(const CziImageExportDialog&) = delete;
        

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked() override;

    private slots:
        void fileSelectionButtonClicked();
        
        void updateExportImageDimensionsLabel();
        
    private:
        
        CziImageFile* m_cziImageFile;
        
        QLineEdit* m_filenameLineEdit;
        
        QCheckBox* m_alphaCheckBox;
        
        QSpinBox* m_maximumWidthHeightSpinBox;
        
        QCheckBox* m_maximumWidthHeightCheckBox;
        
        QLabel* m_cziImageDimensionsLabel;
        
        QLabel* m_exportImageDimensionsLabel;
        
        static AString s_lastFileName;
        
        static bool s_lastAlphaSelectedFlag;
        
        static int s_lastImageDimensionValue;
        
        static bool s_lastImageDimensionSelectedFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CZI_IMAGE_EXPORT_DIALOG_DECLARE__

AString CziImageExportDialog::s_lastFileName;

bool CziImageExportDialog::s_lastAlphaSelectedFlag = false;

int CziImageExportDialog::s_lastImageDimensionValue = 4096;

bool CziImageExportDialog::s_lastImageDimensionSelectedFlag = false;

#endif // __CZI_IMAGE_EXPORT_DIALOG_DECLARE__

} // namespace
#endif  //__CZI_IMAGE_EXPORT_DIALOG_H__

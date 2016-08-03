#ifndef __IMAGE_FILE_CONVERT_TO_VOLUME_FILE_DIALOG_H__
#define __IMAGE_FILE_CONVERT_TO_VOLUME_FILE_DIALOG_H__

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

class QComboBox;
class QDoubleSpinBox;
class QLineEdit;
class QSpinBox;

namespace caret {

    class Brain;
    class BrowserTabContent;
    class ControlPoint3D;
    class EnumComboBoxTemplate;
    class ImageFile;
    class Matrix4x4;
    
    class ImageFileConvertToVolumeFileDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        ImageFileConvertToVolumeFileDialog(QWidget* parent,
                                 const int32_t tabIndex,
                                 ImageFile* imageFile);
        
        virtual ~ImageFileConvertToVolumeFileDialog();
        

        // ADD_NEW_METHODS_HERE

    protected:
        virtual void okButtonClicked();
        
    private:
        ImageFileConvertToVolumeFileDialog(const ImageFileConvertToVolumeFileDialog&);

        ImageFileConvertToVolumeFileDialog& operator=(const ImageFileConvertToVolumeFileDialog&);
        
        QWidget* createVolumeSelectionWidget();
        
        QWidget* createControlPointWidget();
        
        void loadAllControlPoints();
        
        void loadControlPoint(const int32_t index,
                              const ControlPoint3D& cp);
        
        // ADD_NEW_MEMBERS_HERE

        const int32_t m_tabIndex;
        
        ImageFile* m_imageFile;
        
        QLineEdit* m_volumeFileNameLineEdit;
        
        EnumComboBoxTemplate* m_sliceViewPlaneComboBox;
        
        QComboBox* m_colorConversionComboBox;
        
        std::vector<QSpinBox*> m_sourceXSpinBox;
        
        std::vector<QSpinBox*> m_sourceYSpinBox;
        
        std::vector<QDoubleSpinBox*> m_targetXSpinBox;
        
        std::vector<QDoubleSpinBox*> m_targetYSpinBox;
        
        std::vector<QDoubleSpinBox*> m_targetZSpinBox;
        
        
    };
    
#ifdef __IMAGE_FILE_CONVERT_TO_VOLUME_FILE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __IMAGE_FILE_CONVERT_TO_VOLUME_FILE_DIALOG_DECLARE__

} // namespace
#endif  //__IMAGE_FILE_CONVERT_TO_VOLUME_FILE_DIALOG_H__

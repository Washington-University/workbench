#ifndef __VOLUME_FILE_CREATE_DIALOG_H__
#define __VOLUME_FILE_CREATE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "VolumeFile.h"
#include "WuQDialogModal.h"

class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QPushButton;
class QSpinBox;

namespace caret {

    class VolumeMappableInterface;
    
    class VolumeFileCreateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        VolumeFileCreateDialog(const VolumeMappableInterface* underlayVolume,
                               QWidget* parent);
        
        virtual ~VolumeFileCreateDialog();
        
        VolumeFile* getVolumeFile();
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void newFileNamePushButtonClicked();
        
        void loadVolumeParametersFromFilePushButtonClicked();
        
        void numberOfMapsSpinBoxValueChanged(int);
        
    protected:
        virtual void okButtonClicked();
        
    private:
        VolumeFileCreateDialog(const VolumeFileCreateDialog&);

        VolumeFileCreateDialog& operator=(const VolumeFileCreateDialog&);
        
        struct PreviousVolumeSettings {
            std::vector<int64_t> m_dimensions;
            std::vector<std::vector<float> > m_indexToSpace;
            SubvolumeAttributes::VolumeType m_volumeType;
        };
        
        QWidget* createNewVolumeFileWidget();
        
        QWidget* addMapToVolumeFileWidget();
        
        QLineEdit* m_newFileNameLineEdit;
        
        QComboBox* m_newFileTypeComboBox;
        
        QSpinBox* m_newFileNumberOfMapsSpinBox;
        
        std::vector<QLabel*> m_mapNameLabels;
        
        std::vector<QLineEdit*> m_mapNameLineEdits;
        
        QSpinBox* m_newDimXSpinBox;
        
        QSpinBox* m_newDimYSpinBox;
        
        QSpinBox* m_newDimZSpinBox;
        
        QDoubleSpinBox* m_newSpacingXSpinBox;
        
        QDoubleSpinBox* m_newSpacingYSpinBox;
        
        QDoubleSpinBox* m_newSpacingZSpinBox;
        
        QDoubleSpinBox* m_newOriginXSpinBox;
        
        QDoubleSpinBox* m_newOriginYSpinBox;

        QDoubleSpinBox* m_newOriginZSpinBox;
        
        QPushButton* m_paramFromFilePushButton;
        
        VolumeFile* m_volumeFile;

        static int32_t s_maximumNumberOfMaps;
        
        static int32_t s_fileNameCounter;
        
        static PreviousVolumeSettings s_previousVolumeSettings;

        static bool s_previousVolumeSettingsValid;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_FILE_CREATE_DIALOG_DECLARE__
    int32_t VolumeFileCreateDialog::s_maximumNumberOfMaps = 5;
    int32_t VolumeFileCreateDialog::s_fileNameCounter = 1;
    VolumeFileCreateDialog::PreviousVolumeSettings VolumeFileCreateDialog::s_previousVolumeSettings;
    bool VolumeFileCreateDialog::s_previousVolumeSettingsValid = false;
#endif // __VOLUME_FILE_CREATE_DIALOG_DECLARE__

} // namespace
#endif  //__VOLUME_FILE_CREATE_DIALOG_H__

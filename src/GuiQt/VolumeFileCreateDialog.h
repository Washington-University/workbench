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
        
        void linkActivated(const QString& link);
        
        void resamplePushButtonClicked();
        
        void updateVoxelEdgeLabels();
        
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
        
        QSpinBox* m_dimXSpinBox;
        
        QSpinBox* m_dimYSpinBox;
        
        QSpinBox* m_dimZSpinBox;
        
        QDoubleSpinBox* m_spacingXSpinBox;
        
        QDoubleSpinBox* m_spacingYSpinBox;
        
        QDoubleSpinBox* m_spacingZSpinBox;
        
        QDoubleSpinBox* m_originXSpinBox;
        
        QDoubleSpinBox* m_originYSpinBox;

        QDoubleSpinBox* m_originZSpinBox;
        
        QLabel* m_xFirstVoxelEdgeLabel;
        
        QLabel* m_xLastVoxelEdgeLabel;
        
        QLabel* m_yFirstVoxelEdgeLabel;
        
        QLabel* m_yLastVoxelEdgeLabel;
        
        QLabel* m_zFirstVoxelEdgeLabel;
        
        QLabel* m_zLastVoxelEdgeLabel;

        QPushButton* m_paramFromFilePushButton;
        
        QPushButton* m_resamplePushButton;
        
        VolumeFile* m_volumeFile;

        bool m_blockVoxelEdgeLabelUpdateFlag = true;
        
        static int32_t s_maximumNumberOfMaps;
        
        static int32_t s_fileNameCounter;
        
        static PreviousVolumeSettings s_previousVolumeSettings;

        static bool s_previousVolumeSettingsValid;
        
        /* If changed, s_spacingSingleStep = pow(10.0, -s_spacingDecimals) */
        static constexpr double  s_spacingSingleStep = 0.1;
        static constexpr int32_t s_spacingDecimals = 2;
        
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

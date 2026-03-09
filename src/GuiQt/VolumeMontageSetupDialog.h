#ifndef __VOLUME_MONTAGE_SETUP_DIALOG_H__
#define __VOLUME_MONTAGE_SETUP_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include "FunctionResult.h"
#include "VolumeSliceViewPlaneEnum.h"
#include "Vector3D.h"
#include "WuQDialogModal.h"

class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QLabel;
class QSpinBox;
class QStackedLayout;

namespace caret {
    class BrowserTabContent;
    class CaretDataFile;
    class CaretDataFileSelectionComboBox;
    class EnumComboBoxTemplate;
    class HistologySlicesFile;
    class VolumeFile;
    
    class VolumeMontageSetupDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        VolumeMontageSetupDialog(const int32_t browserWindowIndex,
                                 BrowserTabContent* browserTabContent,
                                 VolumeFile* underlayVolumeFile,
                                 QWidget* parent = 0);
        
        virtual ~VolumeMontageSetupDialog();
        
        VolumeMontageSetupDialog(const VolumeMontageSetupDialog&) = delete;

        VolumeMontageSetupDialog& operator=(const VolumeMontageSetupDialog&) = delete;
        
        virtual void okButtonClicked() override;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void histologyFileSelected(CaretDataFile* caretDataFile);
        
        void histologyStartSliceNumberSpinBoxValueChanged(int value);
        
        void histologyEndSliceNumberSpinBoxValueChanged(int value);
        
        void histologyStartSliceNameComboBoxActivated(int index);
        
        void histologyEndSliceNameComboBoxActivated(int index);
        
        void volumeFileSelected(CaretDataFile* caretDataFile);
        
        void volumeStartSliceIndexSpinBoxValueChanged(int value);
        
        void volumeEndSliceIndexSpinBoxValueChanged(int value);
        
        void histologyAlignToHistologyFileAxisCheckBoxClicked(bool checked);
        
        void montageInputRowsValueChanged(int value);
        
        void montageInputColumnsValueChanged(int value);
        
        void montageExtentFileTypeComboBoxActivated(int index);
        
    protected:
        virtual DialogUserButtonResult userButtonPressed(QPushButton* userPushButton) override;
        
    private:
        FunctionResultValue<Vector3D> histologySliceNumberToCoordinate(const int32_t sliceIndex) const;
        
        FunctionResultValue<Vector3D> volumeSliceIndexToCoordinate(const int32_t sliceIndex) const;
        
        FunctionResultValue<int32_t> volumeCoordinateToSliceIndex(const Vector3D& xyz) const;
        
        QWidget* createVolumeSliceInputWidget();
        
        QWidget* createHistologyWidget();
        
        QWidget* createMontageOutputWidget();
        
        void montageInputRowOrColumnValueChanged();
        
        void updateVolumeSliceIndicesToHistologyCoordinates();
        
        FunctionResultValue<std::pair<VolumeSliceViewPlaneEnum::Enum,bool>> histologyOrientationToVolumeSliceViewPlane() const;
        
        FunctionResultValue<Vector3D> getHistologySlicesNormalVector() const;
        
        void updateMontageOutputWidget();
        
        void updateVolumeSliceCoordinates();

        bool applyMontageSettings();
        
        const int32_t m_browserWindowIndex;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        VolumeFile* m_underlayVolumeFile;
        
        VolumeSliceViewPlaneEnum::Enum m_underlaySliceViewPlane;
        
        CaretDataFileSelectionComboBox* m_histologyFileSelectionComboBox = NULL;
        
        QComboBox* m_histologyStartSliceNameComboBox = NULL;
        
        QComboBox* m_histologyEndSliceNameComboBox = NULL;
        
        QSpinBox* m_histologyStartSliceNumberSpinBox = NULL;
        
        QSpinBox* m_histologyEndSliceNumberSpinBox = NULL;
        
        QLabel* m_histologyStartSliceCoordinateLabel = NULL;
        
        QLabel* m_histologyEndSliceCoordinateLabel = NULL;
        
        CaretDataFileSelectionComboBox* m_volumeFileSelectionComboBox = NULL;
        
        QWidget* m_montageExtentVolumeFileWidget = NULL;
        
        QSpinBox* m_volumeStartSliceIndexSpinBox = NULL;
        
        QSpinBox* m_volumeEndSliceIndexSpinBox = NULL;
        
        QLabel* m_volumeStartSliceCoordinateLabel = NULL;
        
        QLabel* m_volumeEndSliceCoordinateLabel = NULL;
        
        QWidget* m_montageExtentHistologyFileWidget = NULL;
        
        QCheckBox* m_histologyAlignToFileAxisCheckBox;
        
        QSpinBox* m_montageInputRowsSpinBox = NULL;
        
        QSpinBox* m_montageInputColumnsSpinBox = NULL;
        
        QLabel* m_montageInputNumberOfSlicesLabel = NULL;

        QLabel* m_montageOutputRangeLabel = NULL;
        
        QLabel* m_montageOutputSliceStepLabel = NULL;

        QLabel* m_montageOutputStepDistanceLabel = NULL;
                
        QDoubleSpinBox* m_montageOutputParagittalSliceIndexSpinBox = NULL;
        
        QDoubleSpinBox* m_montageOutputCoronalSliceIndexSpinBox = NULL;
        
        QDoubleSpinBox* m_montageOutputAxialSliceIndexSpinBox = NULL;
        
        QDoubleSpinBox* m_montageOutputParagittalSliceCoordinateSpinBox = NULL;
        
        QDoubleSpinBox* m_montageOutputCoronalSliceCoordinateSpinBox = NULL;
        
        QDoubleSpinBox* m_montageOutputAxialSliceCoordinateSpinBox = NULL;
        
        QLabel* m_montageOutputSliceViewPlaneLabel = NULL;
        
        QLabel* m_montageOutputRotationAnglesLabel = NULL;
        
        QComboBox* m_montageExtentFileTypeComboBox;
        
        int32_t m_montageExtentTypeVolumeFileIndex = -1;
        
        int32_t m_montageExtentTypeHistologyFileIndex = -1;
        
        QStackedLayout* m_volumeSliceRangeSourceLayout;
        
        bool m_dialogCreationInProgressFlag = true;
        
        VolumeFile* m_montageUnderlayVolumeFile = NULL;
        
        QPushButton* m_applyPushButton;
        
        
        VolumeSliceViewPlaneEnum::Enum m_outputSlicePlane = VolumeSliceViewPlaneEnum::ALL;
        float m_outputParasagittalSliceCoordinate;
        float m_outputCoronalSliceCoordinate;
        float m_outputAxialSliceCoordinate;
        float m_outputMontageStep;
        int32_t m_outputMontageRows;
        int32_t m_outputMontageColumns;
        Vector3D m_outputMprRotationAngles;

        
        // ADD_NEW_MEMBERS_HERE

        static constexpr int s_minimumSpinBoxWidth = 100;
    };
    
#ifdef __VOLUME_MONTAGE_SETUP_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_MONTAGE_SETUP_DIALOG_DECLARE__

} // namespace
#endif  //__VOLUME_MONTAGE_SETUP_DIALOG_H__

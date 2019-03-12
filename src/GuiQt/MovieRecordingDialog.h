#ifndef __MOVIE_RECORDING_DIALOG_H__
#define __MOVIE_RECORDING_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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


#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QAbstractButton;
class QCheckBox;
class QLabel;
class QPushButton;
class QRadioButton;
class QSpinBox;
class QToolButton;

namespace caret {

    class BrainBrowserWindowComboBox;
    class EnumComboBoxTemplate;
    
    class MovieRecordingDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        MovieRecordingDialog(QWidget* parent);
        
        virtual ~MovieRecordingDialog();
        
        MovieRecordingDialog(const MovieRecordingDialog&) = delete;

        MovieRecordingDialog& operator=(const MovieRecordingDialog&) = delete;
        
        void setBrowserWindowIndex(const int32_t browserWindowIndex);
        
        void updateDialog();
        
        void restorePositionAndSize();

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void movieRecorderVideoResolutionTypeEnumComboBoxItemActivated();
        
        void movieRecorderCaptureRegionTypeComboBoxActivated();
        
        void recordingModeRadioButtonClicked(QAbstractButton* button);
        
        void customWidthSpinBoxValueChanged(int width);
        
        void customHeightSpinBoxValueChanged(int width);
        
        void frameRateSpinBoxValueChanged(int frameRate);
        
        void removeTemporaryImagesCheckBoxClicked(bool checked);
        
        void windowIndexSelected(const int32_t windowIndex);
        
        void fileNameButtonClicked();
        
        void createMoviePushButtonClicked();
        
        void resetPushButtonClicked();
        
        void manualCaptureToolButtonClicked();
        
        void manualCaptureSecondsSpinBoxValueChanged(int seconds);
        
    protected:
        virtual void closeEvent(QCloseEvent* event) override;
        
    private:
        void updateFileNameLabel();
        
        void updateFrameCountLabel();
        
        void updateCustomWidthHeightSpinBoxes();
        
        void updateManualRecordingOptions();
        
        QWidget* createMainWidget();
        
        QWidget* createSettingsWidget();
        
        QRadioButton* m_recordingAutomaticRadioButton;
        
        QRadioButton* m_recordingManualRadioButton;
        
        QToolButton* m_manualCaptureToolButton;
        
        QSpinBox* m_manualCaptureSecondsSpinBox;
        
        EnumComboBoxTemplate* m_movieRecorderVideoResolutionTypeEnumComboBox;
        
        QSpinBox* m_customWidthSpinBox;
        
        QSpinBox* m_customHeightSpinBox;
        
        QSpinBox* m_frameRateSpinBox;
        
        QCheckBox* m_removeTemporaryImagesAfterMovieCreationCheckBox;
        
        QPushButton* m_filenamePushButton;
        
        QPushButton* m_createMoviePushButton;
        
        QPushButton* m_resetPushButton;
        
        QLabel* m_filenameLabel;
        
        QLabel* m_frameCountNumberLabel;
        
        QLabel* m_lengthLabel;
        
        BrainBrowserWindowComboBox* m_windowComboBox;
        
        EnumComboBoxTemplate* m_movieRecorderCaptureRegionTypeComboBox;
        
        static QByteArray s_previousDialogGeometry;
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __MOVIE_RECORDING_DIALOG_DECLARE__
    QByteArray MovieRecordingDialog::s_previousDialogGeometry;
#endif // __MOVIE_RECORDING_DIALOG_DECLARE__

} // namespace
#endif  //__MOVIE_RECORDING_DIALOG_H__

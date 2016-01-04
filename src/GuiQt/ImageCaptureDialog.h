#ifndef __IMAGE_CAPTURE_DIALOG__H_
#define __IMAGE_CAPTURE_DIALOG__H_

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

#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QCheckBox;
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;


namespace caret {
    
    class BrainBrowserWindow;
    class EnumComboBoxTemplate;
    class ImageCaptureSettings;
    class WuQWidgetObjectGroup;
    
    class ImageCaptureDialog : public WuQDialogNonModal, public EventListenerInterface {
        Q_OBJECT
        
    public:
        ImageCaptureDialog(BrainBrowserWindow* parent);
        
        virtual ~ImageCaptureDialog();

        void setBrowserWindowIndex(const int32_t browserWindowIndex);
        
        void updateDialog();
        
        void receiveEvent(Event* event);
        
    protected:
        virtual void applyButtonClicked();
        
    private slots:
        void selectImagePushButtonPressed();
        
        void updateBrowserWindowWidthAndHeightLabel();
        
        void imageResolutionUnitsEnumComboBoxItemActivated();
        
        void imageSizeUnitsEnumComboBoxItemActivated();
        
        void pixelWidthValueChanged(int);
        
        void pixelHeightValueChanged(int);
        
        void imageFileNameValueChanged();
        
        void imageWidthValueChanged(double);
        
        void imageHeightValueChanged(double);
        
        void imageResolutionValueChanged(double);
        
        void scaleProportionallyCheckBoxClicked(bool);
        
        void imageCroppingCheckBoxClicked(bool);
        
        void imageCroppingMarginValueChanged(int);
        
        void copyImageToClipboardCheckBoxClicked(bool);
        
        void saveImageToFileCheckBoxClicked(bool);
        
        void sizeRadioButtonClicked(QAbstractButton* button);
        
    private:
        ImageCaptureDialog(const ImageCaptureDialog&);

        ImageCaptureDialog& operator=(const ImageCaptureDialog&);
        
        QWidget* createImageSourceSection();
        QWidget* createImageOptionsSection();
        QWidget* createImageDimensionsSection();
        QWidget* createImageDestinationSection();
        
        void updateDimensionsSection();
        void updateImageOptionsSection();
        void updateDestinationSection();
        
        bool getSelectedWindowCoordsWidthAndHeight(int32_t& xOut,
                                                   int32_t& yOut,
                                                   int32_t& widthOut,
                                                   int32_t& heightOut,
                                                   int32_t& graphicsWidthOut,
                                                   int32_t& graphicsHeightOut,
                                                   float& aspectRatioOut) const;
        
        bool getSelectedWindowWidthAndHeight(int32_t& widthOut,
                                             int32_t& heightOut,
                                             float& aspectRatioOut) const;
        
        //void updateDialogWithImageDimensionsModel();
        
        void updateImageNumberOfBytesLabel();
        
        QCheckBox* m_saveImageToFileCheckBox;
        QCheckBox* m_copyImageToClipboardCheckBox;
        QLineEdit* m_imageFileNameLineEdit;
        
        QRadioButton* m_imageSizeWindowRadioButton;
        QRadioButton* m_imageSizeCustomRadioButton;
        QSpinBox* m_pixelWidthSpinBox;
        QSpinBox* m_pixelHeightSpinBox;
        QDoubleSpinBox* m_imageWidthSpinBox;
        QDoubleSpinBox* m_imageHeightSpinBox;
        QDoubleSpinBox* m_imageResolutionSpinBox;
        QCheckBox* m_scaleProportionallyCheckBox;
        
        
        EnumComboBoxTemplate* m_imagePixelsPerSpatialUnitsEnumComboBox;
        EnumComboBoxTemplate* m_imageSpatialUnitsEnumComboBox;

        QLabel* m_imageNumberOfBytesLabel;
        
        QCheckBox* m_imageAutoCropCheckBox;
        QSpinBox*  m_imageAutoCropMarginSpinBox;
        
        QSpinBox* m_windowSelectionSpinBox;
        QCheckBox* m_windowCropToLockAspectRegionCheckBox;
        
        QWidget* m_customDimensionsWidget;
        
        QWidget* m_imageDimensionsWidget;
    };
    
#ifdef __IMAGE_CAPTURE_DIALOG__H__DECLARE__
#endif // __IMAGE_CAPTURE_DIALOG__H__DECLARE__

} // namespace
#endif  //__IMAGE_CAPTURE_DIALOG__H_

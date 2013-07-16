#ifndef __IMAGE_CAPTURE_DIALOG__H_
#define __IMAGE_CAPTURE_DIALOG__H_

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
        virtual void applyButtonPressed();
        
    private slots:
        void selectImagePushButtonPressed();
        
        void updateBrowserWindowWidthAndHeightLabel();
        
        void imageResolutionUnitsEnumComboBoxItemActivated();
        
        void imageSizeUnitsEnumComboBoxItemActivated();
        
    private:
        ImageCaptureDialog(const ImageCaptureDialog&);

        ImageCaptureDialog& operator=(const ImageCaptureDialog&);
        
        QWidget* createImageSourceSection();
        QWidget* createImageOptionsSection();
        QWidget* createImageDimensionsSection();
        QWidget* createImageDestinationSection();
        
        QCheckBox* m_saveImageToFileCheckBox;
        QCheckBox* m_copyImageToClipboardCheckBox;
        QLineEdit* m_imageFileNameLineEdit;
        
        QRadioButton* m_imageSizeWindowRadioButton;
        QRadioButton* m_imageSizeCustomRadioButton;
        QSpinBox* m_customPixelsWidthSpinBox;
        QSpinBox* m_customPixelsHeightSpinBox;
        QDoubleSpinBox* m_customImageUnitsWidthSpinBox;
        QDoubleSpinBox* m_customImageUnitsHeightSpinBox;
        QDoubleSpinBox* m_customResolutionSpinBox;
        QCheckBox* m_customScaleProportionallyCheckBox;
        
        EnumComboBoxTemplate* m_customResolutionUnitsEnumComboBox;
        EnumComboBoxTemplate* m_customSizeUnitsEnumComboBox;
        QCheckBox* m_imageAutoCropCheckBox;
        QSpinBox*  m_imageAutoCropMarginSpinBox;
        
        QSpinBox* m_windowSelectionSpinBox;
    };
    
#ifdef __IMAGE_CAPTURE_DIALOG__H__DECLARE__
#endif // __IMAGE_CAPTURE_DIALOG__H__DECLARE__

} // namespace
#endif  //__IMAGE_CAPTURE_DIALOG__H_

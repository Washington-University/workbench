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


#include "WuQDialogNonModal.h"

class QCheckBox;
class QLineEdit;
class QRadioButton;
class QSpinBox;


namespace caret {
    
    class BrainBrowserWindow;
    class WuQWidgetObjectGroup;
    
    class ImageCaptureDialog : public WuQDialogNonModal {
        Q_OBJECT
        
    public:
        ImageCaptureDialog(BrainBrowserWindow* parent);
        
        virtual ~ImageCaptureDialog();

        void setBrowserWindowIndex(const int32_t browserWindowIndex);
        
        void updateDialog();
        
    protected:
        virtual void applyButtonPressed();
        
    private slots:
        void selectImagePushButtonPressed();
        
    private:
        ImageCaptureDialog(const ImageCaptureDialog&);

        ImageCaptureDialog& operator=(const ImageCaptureDialog&);
        

        QCheckBox* saveImageToFileCheckBox;
        QCheckBox* copyImageToClipboardCheckBox;
        QLineEdit* imageFileNameLineEdit;
        
        QRadioButton* imageSizeWindowRadioButton;
        QRadioButton* imageSizeCustomRadioButton;
        QSpinBox* imageSizeCustomXSpinBox;
        QSpinBox* imageSizeCustomYSpinBox;
        
        QCheckBox* imageAutoCropCheckBox;
        QSpinBox*  imageAutoCropMarginSpinBox;
        
        QSpinBox* windowSelectionSpinBox;
    };
    
#ifdef __IMAGE_CAPTURE_DIALOG__H__DECLARE__
#endif // __IMAGE_CAPTURE_DIALOG__H__DECLARE__

} // namespace
#endif  //__IMAGE_CAPTURE_DIALOG__H_

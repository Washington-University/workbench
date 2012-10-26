#ifndef __MOVIE_DIALOG__H_
#define __MOVIE_DIALOG__H_

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
class QDoubleSpinBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QSpinBox;
class QToolButton;


namespace caret {
    
    class BrainBrowserWindow;
    class WuQWidgetObjectGroup;
    
    class MovieDialog : public WuQDialogNonModal {
        Q_OBJECT
        
    public:
        MovieDialog(BrainBrowserWindow* parent);
        
        virtual ~MovieDialog();

        void setBrowserWindowIndex(const int32_t browserWindowIndex);
        
        void updateDialog();
        
    protected:
        virtual void applyButtonPressed();
        
    private slots:
        void selectImagePushButtonPressed();
        
    private:
        MovieDialog(const MovieDialog&);

        MovieDialog& operator=(const MovieDialog&);

        QToolButton *recordMovieButton;
        QLabel *repeatFramesLabel;
        QSpinBox *repeatFramesSpinBox;
        QLabel *frameRotateXLabel;
        QDoubleSpinBox *frameRotateXSpinBox;
        QLabel *frameRotateYLabel;
        QDoubleSpinBox *frameRotateYSpinBox;
        QLabel *frameRotateZLabel;
        QDoubleSpinBox *frameRotateZSpinBox;
        QLabel *frameRotateCountLabel;
        QSpinBox *frameRotateCountSpinBox;
        QLabel *frameRotateReverseDirectionLabel;
        QCheckBox *frameRotateReverseDirection;
        QToolButton *animateRotationButton;
        QAction *animateRotationAction;
        

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
    
#ifdef __MOVIE_DIALOG__H__DECLARE__
#endif // __MOVIE_DIALOG__H__DECLARE__

} // namespace
#endif  //__MOVIE_DIALOG__H_

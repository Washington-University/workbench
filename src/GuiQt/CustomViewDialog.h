#ifndef __CUSTOM_VIEW_DIALOG_H__
#define __CUSTOM_VIEW_DIALOG_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "WuQDialogNonModal.h"

class QDoubleSpinBox;
class QPushButton;

namespace caret {

    class BrainBrowserWindow;
    class BrowserTabContent;
    class UserView;
    class WuQListWidget;
    
    class CustomViewDialog : public WuQDialogNonModal {
        
        Q_OBJECT

    public:
        CustomViewDialog(QWidget* parent);
        
        virtual ~CustomViewDialog();
        
        void updateDialog();
        
        void setBrowserWindowIndex(const int32_t browserWindowIndex);
        
    private:
        CustomViewDialog(const CustomViewDialog&);

        CustomViewDialog& operator=(const CustomViewDialog&);
        
    private slots:
        void transformValueChanged();
        
        void addNewViewPushButtonClicked();
        
        void applyViewPushButtonClicked();
        
        void deleteViewPushButtonClicked();
        
        void replaceViewPushButtonClicked();
        
        void setTransformationPushButtonClicked();
        
        void updateViewPushButtonClicked();
        
        void viewSelected();
        
        void viewWasDropped();
        
    public:

        // ADD_NEW_METHODS_HERE

    private:

        QWidget* createViewsWidget();
        
        QWidget* createTransformsWidget();
        
        UserView* getSelectedUserView();
        
        void selectViewByName(const AString& name);
        
        std::vector<BrainBrowserWindow*> getBrowserWindows();
        
        // ADD_NEW_MEMBERS_HERE

        /// x translate float spin box
        QDoubleSpinBox* m_xTranslateDoubleSpinBox;
        
        /// y translate float spin box
        QDoubleSpinBox* m_yTranslateDoubleSpinBox;
        
        /// z translate float spin box
        QDoubleSpinBox* m_zTranslateDoubleSpinBox;
        
        /// x rotate float spin box
        QDoubleSpinBox* m_xRotateDoubleSpinBox;
        
        /// y rotate float spin box
        QDoubleSpinBox* m_yRotateDoubleSpinBox;
        
        /// z rotate float spin box
        QDoubleSpinBox* m_zRotateDoubleSpinBox;
        
        /// scale float spin box
        QDoubleSpinBox* m_scaleDoubleSpinBox;
        
        QPushButton* m_addNewViewPushButton;
        
        QPushButton* m_deleteViewPushButton;
        
        QPushButton* m_replaceViewPushButton;
        
        QPushButton* m_updateViewPushButton;
        
        QPushButton* m_applyViewPushButton;
        
        QPushButton* m_setTransformationPushButton;
        
        WuQListWidget* m_viewSelectionListWidget;

        std::vector<BrainBrowserWindow*> m_previousAppliedToBrowserWindows;
    };
    
#ifdef __CUSTOM_VIEW_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CUSTOM_VIEW_DIALOG_DECLARE__

} // namespace
#endif  //__CUSTOM_VIEW_DIALOG_H__

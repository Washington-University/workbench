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

#include "EventListenerInterface.h"
#include "WuQDialogNonModal.h"

class QDoubleSpinBox;
class QPushButton;

namespace caret {

    class BrainBrowserWindow;
    class BrainBrowserWindowComboBox;
    class CaretPreferences;
    class ModelTransform;
    class WuQListWidget;
    class WuQWidgetObjectGroup;
    
    class CustomViewDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        CustomViewDialog(QWidget* parent);
        
        virtual ~CustomViewDialog();
        
        void updateDialog();
        
        void updateContent(const int32_t browserWindowIndex);
        
        void receiveEvent(Event* event);
        
    private:
        CustomViewDialog(const CustomViewDialog&);

        CustomViewDialog& operator=(const CustomViewDialog&);
        
    private slots:
        void transformValueChanged();
        
        void newCustomViewPushButtonClicked();
        
        void deleteCustomViewPushButtonClicked();
        
        void browserWindowComboBoxValueChanged(BrainBrowserWindow* browserWindow);
        
        void customViewSelected();
        
        void customViewSelectedAndApplied();
        
        void copyToCustomViewPushButtonClicked();
        
        void copyToTransformPushButtonClicked();
        
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        void focusGained();

    private:

        void loadCustomViewListWidget(const AString& selectedName = "");
        
        void updateGraphicsWindow();
        
        void getTransformationControlValues(double& panX,
                                double& panY,
                                double& rotX,
                                double& rotY,
                                double& rotZ,
                                double& zoom) const;
        
        void setTransformationControlValues(const double panX,
                                const double panY,
                                const double rotX,
                                const double rotY,
                                const double rotZ,
                                const double zoom) const;
        
        // ADD_NEW_MEMBERS_HERE
        
        CaretPreferences* getCaretPreferences();

        //std::vector<AString> getAllCustomViewNames();
        
        QWidget* createCustomViewWidget();
        
        QWidget* createCopyWidget();
        
        QWidget* createTransformsWidget();
        
        //UserView* getSelectedUserView();
        
        AString getSelectedCustomViewName();
        
        void moveTransformToCustomView(ModelTransform& modelTransform);
        
        QWidget* m_copyWidget;
        
        QDoubleSpinBox* m_xPanDoubleSpinBox;
        
        QDoubleSpinBox* m_yPanDoubleSpinBox;
        
        QDoubleSpinBox* m_xRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_yRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_zRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_zoomDoubleSpinBox;

        WuQWidgetObjectGroup* m_transformWidgetGroup;
        
        BrainBrowserWindowComboBox* m_browserWindowComboBox;
        
        QPushButton* m_newCustomViewPushButton;
        
        QPushButton* m_deleteCustomViewPushButton;
        
        WuQListWidget* m_customViewListWidget;
        
        bool m_blockDialogUpdate;
        
    };
    
#ifdef __CUSTOM_VIEW_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CUSTOM_VIEW_DIALOG_DECLARE__

} // namespace
#endif  //__CUSTOM_VIEW_DIALOG_H__

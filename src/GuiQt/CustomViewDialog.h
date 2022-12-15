#ifndef __CUSTOM_VIEW_DIALOG_H__
#define __CUSTOM_VIEW_DIALOG_H__

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
        void zoomValueChanged(double value);
        
        void transformValueChanged();
        
        void newCustomViewPushButtonClicked();
        
        void deleteCustomViewPushButtonClicked();
        
        void browserWindowComboBoxValueChanged(BrainBrowserWindow* browserWindow);
        
        void customViewSelected();
        
        void customViewSelectedAndApplied();
        
        void copyToCustomViewPushButtonClicked();
        
        void copyToTransformPushButtonClicked();
        
        void resetViewToolButtonClicked();
        
    public:

        // ADD_NEW_METHODS_HERE

    protected:
        void focusGained();

    private:

        void loadCustomViewListWidget(const AString& selectedName = "");
        
        void updateGraphicsWindow();
        
        void getTransformationControlValues(double& panX,
                                            double& panY,
                                            double& panZ,
                                            double& rotX,
                                            double& rotY,
                                            double& rotZ,
                                            double& obRotX,
                                            double& obRotY,
                                            double& obRotZ,
                                            double& mprRotX,
                                            double& mprRotY,
                                            double& mprRotZ,
                                            double& flatRotate,
                                            double& zoom,
                                            double& rightFlatX,
                                            double& rightFlatY,
                                            double& rightFlatZoom) const;
        
        void setTransformationControlValues(const double panX,
                                            const double panY,
                                            const double panZ,
                                            const double rotX,
                                            const double rotY,
                                            const double rotZ,
                                            const double obRotX,
                                            const double obRotY,
                                            const double obRotZ,
                                            const double mprRotX,
                                            const double mprRotY,
                                            const double mprRotZ,
                                            const double flatRotate,
                                            const double zoom,
                                            const double rightFlatX,
                                            const double rightFlatY,
                                            const double rightFlatZoom) const;
        
        // ADD_NEW_MEMBERS_HERE
        
        CaretPreferences* getCaretPreferences();

        //std::vector<AString> getAllCustomViewNames();
        
        QWidget* createCustomViewWidget();
        
        QWidget* createCopyWidget();
        
        QWidget* createTransformsWidget();
        
        //UserView* getSelectedUserView();
        
        AString getSelectedCustomViewName();
        
        void moveTransformToCustomView(ModelTransform& modelTransform);
        
        void updateSpinBoxSingleStepValue(QDoubleSpinBox* spinBox,
                                          const double singleStep);
        
        void updateSpinBoxValue(QDoubleSpinBox* spinBox,
                                const double newValue) const;

        int64_t createScaledInt(const double value,
                                const int32_t decimals) const;
        
        QWidget* m_copyWidget;
        
        QDoubleSpinBox* m_xPanDoubleSpinBox;
        
        QDoubleSpinBox* m_yPanDoubleSpinBox;
        
        QDoubleSpinBox* m_zPanDoubleSpinBox;
        
        QDoubleSpinBox* m_xRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_yRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_zRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_xObliqueRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_yObliqueRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_zObliqueRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_xMprRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_yMprRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_zMprRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_flatRotationDoubleSpinBox;
        
        QDoubleSpinBox* m_zoomDoubleSpinBox;

        QDoubleSpinBox* m_xRightFlatMapSpinBox;
        
        QDoubleSpinBox* m_yRightFlatMapSpinBox;
        
        QDoubleSpinBox* m_rightFlatMapZoomFactorSpinBox;
        
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

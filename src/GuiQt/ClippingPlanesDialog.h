#ifndef __CLIPPING_PLANES_DIALOG_H__
#define __CLIPPING_PLANES_DIALOG_H__

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


#include "WuQDialogNonModal.h"

#include "EventListenerInterface.h"

class QCheckBox;
class QDoubleSpinBox;
class QPushButton;

namespace caret {
    class BrainBrowserWindow;
    class BrainBrowserWindowComboBox;
    class WuQWidgetObjectGroup;
    
    class ClippingPlanesDialog : public WuQDialogNonModal, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ClippingPlanesDialog(QWidget* parent);
        
        virtual ~ClippingPlanesDialog();
        

        void updateDialog();
        
        void updateContent(const int32_t browserWindowIndex);
        
        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void browserWindowComboBoxValueChanged(BrainBrowserWindow* browserWindow);
        
        void clippingValueChanged();
        
    protected:
        void focusGained();
        
        virtual NonModalDialogUserButtonResult userButtonPressed(QPushButton* userPushButton);
        
    private:
        void updateGraphicsWindow();
        
        ClippingPlanesDialog(const ClippingPlanesDialog&);

        ClippingPlanesDialog& operator=(const ClippingPlanesDialog&);
        
        QPushButton* m_resetPushButton;
        
        BrainBrowserWindowComboBox* m_browserWindowComboBox;
        
        WuQWidgetObjectGroup* m_clippingWidgetGroup;
        
        QDoubleSpinBox* m_xPanDoubleSpinBox;
        
        QDoubleSpinBox* m_yPanDoubleSpinBox;
        
        QDoubleSpinBox* m_zPanDoubleSpinBox;
        
        QDoubleSpinBox* m_xRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_yRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_zRotateDoubleSpinBox;
        
        QDoubleSpinBox* m_xThicknessDoubleSpinBox;
        
        QDoubleSpinBox* m_yThicknessDoubleSpinBox;
        
        QDoubleSpinBox* m_zThicknessDoubleSpinBox;
        
        QCheckBox* m_displayClippingBoxCheckBox;
        
        bool m_blockDialogUpdate;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CLIPPING_PLANES_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLIPPING_PLANES_DIALOG_DECLARE__

} // namespace
#endif  //__CLIPPING_PLANES_DIALOG_H__

#ifndef __CLIPPING_PLANES_WIDGET_H__
#define __CLIPPING_PLANES_WIDGET_H__

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


#include <QWidget>

#include "EventListenerInterface.h"

class QCheckBox;
class QDoubleSpinBox;

namespace caret {
    class BrowserTabContent;
    class WuQWidgetObjectGroup;
    
    class ClippingPlanesWidget : public QWidget {
        
        Q_OBJECT

    public:
        ClippingPlanesWidget(QWidget* parent = 0);
        
        virtual ~ClippingPlanesWidget();
        
        void updateContent(const int32_t tabIndex);
        
        // ADD_NEW_METHODS_HERE
    private slots:
        void clippingValueChanged();
        
        void resetButtonClicked();
        
    private:
        void updateGraphicsWindow();
        
        ClippingPlanesWidget(const ClippingPlanesWidget&);

        ClippingPlanesWidget& operator=(const ClippingPlanesWidget&);
        
        QWidget* createClippingBoxWidget();
        
        QWidget* createClippingAxesWidget();
        
        QWidget* createClippingDataTypeWidget();
        
        BrowserTabContent* getBrowserTabContent();
        
        int32_t m_tabIndex = -1;
        
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
        
        QCheckBox* m_xClippingEnabledCheckBox;
        
        QCheckBox* m_yClippingEnabledCheckBox;
        
        QCheckBox* m_zClippingEnabledCheckBox;
        
        QCheckBox* m_surfaceClippingEnabledCheckBox;
        
        QCheckBox* m_volumeClippingEnabledCheckBox;
        
        QCheckBox* m_featuresClippingEnabledCheckBox;
        
        QString m_objectNamePrefix;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CLIPPING_PLANES_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CLIPPING_PLANES_WIDGET_DECLARE__

} // namespace
#endif  //__CLIPPING_PLANES_WIDGET_H__

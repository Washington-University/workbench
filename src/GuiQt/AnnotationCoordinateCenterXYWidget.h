#ifndef __ANNOTATION_COORDINATE_CENTER_XY_WIDGET_H__
#define __ANNOTATION_COORDINATE_CENTER_XY_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationWidgetParentEnum.h"
#include "UserInputModeEnum.h"

class QDoubleSpinBox;

namespace caret {

    class Annotation;
    class AnnotationCoordinate;
    
    class AnnotationCoordinateCenterXYWidget: public QWidget {
        
        Q_OBJECT

    public:
        enum WhichCoordinate {
            COORDINATE_ONE,
            COORDINATE_TWO
        };
        
        AnnotationCoordinateCenterXYWidget(const UserInputModeEnum::Enum userInputMode,
                                   const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                   const WhichCoordinate whichCoordinate,
                                   const int32_t browserWindowIndex,
                                   QWidget* parent = 0);
        
        virtual ~AnnotationCoordinateCenterXYWidget();
        

        // ADD_NEW_METHODS_HERE

        void updateContent(std::vector<Annotation*>& selectedAnnotations);
        
    private slots:
        void xValueChanged(double value);
        
        void yValueChanged(double value);
        
    private:
        AnnotationCoordinateCenterXYWidget(const AnnotationCoordinateCenterXYWidget&);

        AnnotationCoordinateCenterXYWidget& operator=(const AnnotationCoordinateCenterXYWidget&);
        
        void processValueChanged(QDoubleSpinBox* spinBox,
                                 const double value);
        
        // ADD_NEW_MEMBERS_HERE

        const UserInputModeEnum::Enum m_userInputMode;
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const WhichCoordinate m_whichCoordinate;
        
        const int32_t m_browserWindowIndex;
        
        QDoubleSpinBox* m_xCoordSpinBox;
        
        QDoubleSpinBox* m_yCoordSpinBox;
        
        std::vector<Annotation*> m_annotations;
        
    };
    
#ifdef __ANNOTATION_COORDINATE_CENTER_XY_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COORDINATE_CENTER_XY_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_COORDINATE_CENTER_XY_WIDGET_H__

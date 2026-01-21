#ifndef __ANNOTATION_COORDINATES_WIDGET_H__
#define __ANNOTATION_COORDINATES_WIDGET_H__

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

#include "AnnotationSurfaceOffsetVectorTypeEnum.h"
#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationWidgetParentEnum.h"
#include "StructureEnum.h"
#include "UserInputModeEnum.h"

class QDoubleSpinBox;
class QLabel;
class QSpinBox;
class QStackedLayout;

namespace caret {

    class Annotation;
    class AnnotationCoordinate;
    class EnumComboBoxTemplate;
    class StructureEnumComboBox;
    
    class AnnotationCoordinatesWidget : public QWidget {
        
        Q_OBJECT

    public:        
        AnnotationCoordinatesWidget(const UserInputModeEnum::Enum userInputMode,
                                   const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                   const int32_t browserWindowIndex,
                                   QWidget* parent = 0);
        
        virtual ~AnnotationCoordinatesWidget();
        

        // ADD_NEW_METHODS_HERE

        void updateContent(Annotation* annotation);
        
    private slots:
        void coordNumberSpinBoxValueChanged(int coordNumber);
        
        void coordinateSpinBoxValueChanged(double value);
        
        void valueChangedCoordinate();
        
        void surfaceVertexOffsetLengthValueChanged(double);
        
        void surfaceVertexOffsetVectorTypeChanged();
        
        void surfaceVertexIndexValueChanged(int);
        
        void surfaceVertexStructureValueChanged(const StructureEnum::Enum structure);
        
    private:
        AnnotationCoordinatesWidget(const AnnotationCoordinatesWidget&);

        AnnotationCoordinatesWidget& operator=(const AnnotationCoordinatesWidget&);
        
        QWidget* createCoordinateWidgetXYZ();
        
        QWidget* createCoordinateWidgetSurface();
        
        AnnotationCoordinate* getSelectedCoordinate();
        
        AnnotationCoordinate* getFirstCoordinate();
        
        QDoubleSpinBox* createCoordinateSpinBox(const QString& axisCharacter,
                                                const int32_t xyzIndex);
        
        void updateCoordinate();
        
        // ADD_NEW_MEMBERS_HERE

        const UserInputModeEnum::Enum m_userInputMode;
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const int32_t m_browserWindowIndex;
        
        QSpinBox* m_coordNumberSpinBox;
        
        QStackedLayout* m_stackedLayout;
        
        QWidget* m_coordSurfaceWidget;
        
        QWidget* m_coordXyzWidget;
        
        StructureEnumComboBox* m_surfaceStructureComboBox;
        
        QSpinBox* m_surfaceNodeIndexSpinBox;
        
        EnumComboBoxTemplate* m_surfaceOffsetVectorTypeComboBox;
        
        QDoubleSpinBox* m_surfaceOffsetLengthSpinBox;
        
        QDoubleSpinBox* m_xCoordSpinBox;
        
        QDoubleSpinBox* m_yCoordSpinBox;
        
        QLabel* m_zCoordLabel;
        
        QDoubleSpinBox* m_zCoordSpinBox;

        Annotation* m_annotation = NULL;
        
        Annotation* m_previousAnnotation = NULL;
        
        AString m_plusButtonToolTipText;
        
    };
    
#ifdef __ANNOTATION_COORDINATES_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COORDINATES_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_COORDINATES_WIDGET_H__

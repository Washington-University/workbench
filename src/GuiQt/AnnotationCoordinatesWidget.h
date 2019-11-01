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

#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationWidgetParentEnum.h"
#include "UserInputModeEnum.h"

class QDoubleSpinBox;
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
        void valueChangedCoordinateOne();
        
        void valueChangedCoordinateTwo();
        
        void surfaceOffsetLengthValueOneChanged(double);
        
        void surfaceOffsetLengthValueTwoChanged(double);
        
        void surfaceOffsetVectorTypeChanged();
        
    private:
        AnnotationCoordinatesWidget(const AnnotationCoordinatesWidget&);

        AnnotationCoordinatesWidget& operator=(const AnnotationCoordinatesWidget&);
        
        AnnotationCoordinate* getCoordinate(const int32_t coordinateIndex);
        
        QDoubleSpinBox* createCoordinateSpinBox(const int32_t coordinateIndex,
                                                const QString& axisCharacter);
        
        void createCoordinateWidgets(const int32_t coordinateIndex);
        
        void updateCoordinate(const int32_t coordinateIndex,
                              const AnnotationCoordinate* coordinate);
        
        void valueChangedCoordinate(const int32_t coordinateIndex);
        
        // ADD_NEW_MEMBERS_HERE

        const UserInputModeEnum::Enum m_userInputMode;
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const int32_t m_browserWindowIndex;
        
        QStackedLayout* m_stackedLayout;
        
        QWidget* m_surfaceWidget;
        
        QWidget* m_coordinateWidget;
        
        StructureEnumComboBox* m_surfaceStructureComboBox;
        
        QSpinBox* m_surfaceNodeIndexSpinBox[2];
        
        EnumComboBoxTemplate* m_surfaceOffsetVectorTypeComboBox;
        
        QDoubleSpinBox* m_surfaceOffsetLengthSpinBox[2];
        
        QDoubleSpinBox* m_xCoordSpinBox[2];
        
        QDoubleSpinBox* m_yCoordSpinBox[2];
        
        QDoubleSpinBox* m_zCoordSpinBox[2];

        Annotation* m_annotation;
        
        AString m_plusButtonToolTipText;
        
    };
    
#ifdef __ANNOTATION_COORDINATES_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COORDINATES_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_COORDINATES_WIDGET_H__

#ifndef __ANNOTATION_COLOR_WIDGET_H__
#define __ANNOTATION_COLOR_WIDGET_H__

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

#include "AnnotationWidgetParentEnum.h"
#include "CaretColorEnum.h"

class QAction;
class QDoubleSpinBox;
class QToolButton;

namespace caret {

    class Annotation;
    class CaretColorEnumMenu;
    class WuQWidgetObjectGroup;
    
    class AnnotationColorWidget : public QWidget {
        
        Q_OBJECT

    public:
        AnnotationColorWidget(const AnnotationWidgetParentEnum::Enum parentWidgetType,
                              const int32_t browserWindowIndex,
                              QWidget* parent = 0);
        
        virtual ~AnnotationColorWidget();
        
        void updateContent(std::vector<Annotation*>& annotations);
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void backgroundColorSelected(const CaretColorEnum::Enum);
        
        void foregroundColorSelected(const CaretColorEnum::Enum);
        
    private slots:
        void foregroundThicknessSpinBoxValueChanged(double value);
        
    private:
        AnnotationColorWidget(const AnnotationColorWidget&);

        AnnotationColorWidget& operator=(const AnnotationColorWidget&);

        void updateBackgroundColorButton();
        
        void updateForegroundColorButton();
        
        void updateForegroundThicknessSpinBox();
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const int32_t m_browserWindowIndex;
        
        QToolButton* m_backgroundToolButton;
        
        QToolButton* m_foregroundToolButton;
        
        QAction* m_foregroundColorAction;
        
        QAction* m_backgroundColorAction;
        
        CaretColorEnumMenu* m_foregroundColorMenu;
        
        CaretColorEnumMenu* m_backgroundColorMenu;
        
        QDoubleSpinBox* m_foregroundThicknessSpinBox;
        
        WuQWidgetObjectGroup* m_backgroundWidgetGroup;
        
        std::vector<Annotation*> m_annotations;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_COLOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_COLOR_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_COLOR_WIDGET_H__

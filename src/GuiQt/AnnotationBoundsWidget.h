#ifndef __ANNOTATION_BOUNDS_WIDGET_H__
#define __ANNOTATION_BOUNDS_WIDGET_H__

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
#include "UserInputModeEnum.h"

class QDoubleSpinBox;

namespace caret {

    class AnnotationBrowserTab;
    
    class AnnotationBoundsWidget : public QWidget {
        
        Q_OBJECT

    public:
        AnnotationBoundsWidget(const UserInputModeEnum::Enum userInputMode,
                                   const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                   const int32_t browserWindowIndex,
                                   QWidget* parent = 0);
        
        virtual ~AnnotationBoundsWidget();
        

        // ADD_NEW_METHODS_HERE

        void updateContent(std::vector<AnnotationBrowserTab*>& annotationBrowserTabs);
        
    private slots:
        void xMinValueChanged(const double value);
        
        void xMaxValueChanged(const double value);
        
        void yMinValueChanged(const double value);
        
        void yMaxValueChanged(const double value);
        
    private:
        AnnotationBoundsWidget(const AnnotationBoundsWidget&);

        AnnotationBoundsWidget& operator=(const AnnotationBoundsWidget&);
        
        QDoubleSpinBox* createSpinBox();
        
        void valueChangedHelper(QDoubleSpinBox* spinBox,
                                float value);
        
        // ADD_NEW_MEMBERS_HERE

        const UserInputModeEnum::Enum m_userInputMode;
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const int32_t m_browserWindowIndex;
        
        QDoubleSpinBox* m_xMinCoordSpinBox;
        
        QDoubleSpinBox* m_xMaxCoordSpinBox;
        
        QDoubleSpinBox* m_yMinCoordSpinBox;
        
        QDoubleSpinBox* m_yMaxCoordSpinBox;
        
        AnnotationBrowserTab* m_annotationBrowserTab;
        
    };
    
#ifdef __ANNOTATION_BOUNDS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_BOUNDS_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_BOUNDS_WIDGET_H__

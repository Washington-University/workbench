#ifndef __ANNOTATION_THICKNESS_WIDGET_H__
#define __ANNOTATION_THICKNESS_WIDGET_H__

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

#include "EventListenerInterface.h"

class QDoubleSpinBox;

namespace caret {
    class Annotation;

    class AnnotationThicknessWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        AnnotationThicknessWidget(const int32_t browserWindowIndex,
                                 QWidget* parent = 0);
        
        virtual ~AnnotationThicknessWidget();
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

        void updateContent(Annotation* annotation);
        
    private slots:
        void lineSizeSpinBoxValueChanged(double value);
        
    private:
        AnnotationThicknessWidget(const AnnotationThicknessWidget&);

        AnnotationThicknessWidget& operator=(const AnnotationThicknessWidget&);
        
        // ADD_NEW_MEMBERS_HERE

        const int32_t m_browserWindowIndex;
        
        QDoubleSpinBox* m_lineSizeSpinBox;
        
        Annotation* m_annotation;
    };
    
#ifdef __ANNOTATION_THICKNESS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_THICKNESS_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_THICKNESS_WIDGET_H__

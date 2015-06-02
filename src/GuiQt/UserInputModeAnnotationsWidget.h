#ifndef __USER_INPUT_MODE_ANNOTATIONS_WIDGET_H__
#define __USER_INPUT_MODE_ANNOTATIONS_WIDGET_H__

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

class QComboBox;

namespace caret {

    class Annotation;
    class AnnotationColorWidget;
    class AnnotationCoordinateWidget;
    class AnnotationFontWidget;
    class AnnotationFormatWidget;
    class AnnotationInsertNewWidget;
    class AnnotationLineArrowTipsWidget;
    class AnnotationRotationWidget;
    class AnnotationTextAlignmentWidget;
    class AnnotationTextEditorWidget;
    class AnnotationTextOrientationWidget;
    class AnnotationWidthHeightWidget;
    class UserInputModeAnnotations;
    
    class UserInputModeAnnotationsWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        UserInputModeAnnotationsWidget(UserInputModeAnnotations* inputModeAnnotations,
                                       const int32_t browserWindowIndex);
        
        virtual ~UserInputModeAnnotationsWidget();
        
        virtual void receiveEvent(Event* event);
        
        void updateWidget();
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void selectCoordinateOneWithMouse();
        
        void selectCoordinateTwoWithMouse();
        
    private:
        UserInputModeAnnotationsWidget(const UserInputModeAnnotationsWidget&);

        UserInputModeAnnotationsWidget& operator=(const UserInputModeAnnotationsWidget&);
        
        QWidget* createInsertMenuToolButton();
        
        QWidget* createTextEditorWidget();
        
        const int32_t m_browserWindowIndex;
        
        UserInputModeAnnotations* m_inputModeAnnotations;
        
        AnnotationCoordinateWidget* m_coordinateOneWidget;
        
        AnnotationCoordinateWidget* m_coordinateTwoWidget;
        
        AnnotationWidthHeightWidget* m_widthHeightWidget;
        
        AnnotationRotationWidget* m_rotationWidget;
        
        AnnotationLineArrowTipsWidget* m_lineArrowTipsWidget;
        
        AnnotationFontWidget* m_fontWidget;
        
        AnnotationColorWidget* m_colorWidget;
        
        AnnotationTextAlignmentWidget* m_textAlignmentWidget;
        
        AnnotationFormatWidget* m_formatWidget;
        
        AnnotationTextEditorWidget* m_textEditorWidget;
        
        AnnotationTextOrientationWidget* m_textOrientationWidget;
        
        AnnotationInsertNewWidget* m_insertDeleteWidget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __USER_INPUT_MODE_ANNOTATIONS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_ANNOTATIONS_WIDGET_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_ANNOTATIONS_WIDGET_H__

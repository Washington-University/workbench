#ifndef __ANNOTATION_POLY_TYPE_DRAW_EDIT_WIDGET_H__
#define __ANNOTATION_POLY_TYPE_DRAW_EDIT_WIDGET_H__

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


#include <stdint.h>

#include <QWidget>

#include "UserInputModeEnum.h"

class QToolButton;

namespace caret {
    class Annotation;
    class UserInputModeAnnotations;

    class AnnotationPolyTypeDrawEditWidget : public QWidget {
        
        Q_OBJECT

    public:
        AnnotationPolyTypeDrawEditWidget(const Qt::Orientation orientation,
                                     UserInputModeAnnotations* userInputModeAnnotations,
                                     const int32_t browserWindowIndex,
                                     QWidget* parent = 0);
        
        virtual ~AnnotationPolyTypeDrawEditWidget();
        
        void updateContent();
        
    private slots:
        void finishActionTriggered();
        
        void cancelActionTriggered();
        
        void eraseLastCoordinateActionTriggered();

        void editVerticesActionTriggered(bool checked);
        
        // ADD_NEW_METHODS_HERE

    private:
        AnnotationPolyTypeDrawEditWidget(const AnnotationPolyTypeDrawEditWidget&);

        AnnotationPolyTypeDrawEditWidget& operator=(const AnnotationPolyTypeDrawEditWidget&);
        
        UserInputModeAnnotations* m_userInputModeAnnotations;
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        const int32_t m_browserWindowIndex;
        
        QToolButton* m_finishToolButton;
        
        QString m_finishToolButtonStyleSheetDisabled;
        
        QString m_finishToolButtonStyleSheetEnabled;
        
        QAction* m_finishAction;
        
        QAction* m_cancelAction;
        
        QAction* m_eraseLastCoordinateAction;
        
        QAction* m_editVerticesAction;
        
        int32_t m_annotationNumberOfCoordinates = 0;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_POLY_TYPE_DRAW_EDIT_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_POLY_TYPE_DRAW_EDIT_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_POLY_TYPE_DRAW_EDIT_WIDGET_H__

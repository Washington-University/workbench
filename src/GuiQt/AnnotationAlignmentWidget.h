#ifndef __ANNOTATION_ALIGNMENT_WIDGET_H__
#define __ANNOTATION_ALIGNMENT_WIDGET_H__

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

#include "AnnotationAlignHorizontalEnum.h"
#include "AnnotationAlignVerticalEnum.h"
#include "EventListenerInterface.h"

class QAction;
class QActionGroup;
class QPixmap;
class QToolButton;

namespace caret {

    
    class AnnotationAlignmentWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        AnnotationAlignmentWidget(QWidget* parent = 0);
        
        virtual ~AnnotationAlignmentWidget();
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void horizontalAlignmentActionSelected(QAction* action);
        
        void verticalAlignmentActionSelected(QAction* action);
        
    private:
        AnnotationAlignmentWidget(const AnnotationAlignmentWidget&);

        AnnotationAlignmentWidget& operator=(const AnnotationAlignmentWidget&);
        
        QToolButton* createHorizontalAlignmentToolButton(const AnnotationAlignHorizontalEnum::Enum horizontalAlignment);
        
        QToolButton* createVerticalAlignmentToolButton(const AnnotationAlignVerticalEnum::Enum verticalAlignment);
        
        QPixmap createHorizontalAlignmentPixmap(const QWidget* widget,
                                        const AnnotationAlignHorizontalEnum::Enum horizontalAlignment);
        
        QPixmap createVerticalAlignmentPixmap(const QWidget* widget,
                                                const AnnotationAlignVerticalEnum::Enum verticalAlignment);
        
        QActionGroup* m_horizontalAlignActionGroup;
        
        QActionGroup* m_verticalAlignActionGroup;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_ALIGNMENT_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_ALIGNMENT_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_ALIGNMENT_WIDGET_H__

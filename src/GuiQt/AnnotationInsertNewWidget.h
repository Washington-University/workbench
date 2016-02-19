#ifndef __ANNOTATION_INSERT_NEW_WIDGET_H__
#define __ANNOTATION_INSERT_NEW_WIDGET_H__

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
#include "AnnotationTypeEnum.h"

class QActionGroup;
class QToolButton;

namespace caret {
    class AnnotationMenuFileSelection;
    
    class AnnotationInsertNewWidget : public QWidget {
        
        Q_OBJECT

    public:
        AnnotationInsertNewWidget(const int32_t browserWindowIndex,
                                  QWidget* parent = 0);
        
        virtual ~AnnotationInsertNewWidget();
        

        // ADD_NEW_METHODS_HERE

        void updateContent();
        
    private slots:
        void shapeBoxActionTriggered();
        
        void shapeLineActionTriggered();
        
        void shapeImageActionTriggered();
        
        void shapeTextActionTriggered();
        
        void shapeOvalActionTriggered();
        
        void itemSelectedFromFileSelectionMenu();
        
    private:
        AnnotationInsertNewWidget(const AnnotationInsertNewWidget&);

        AnnotationInsertNewWidget& operator=(const AnnotationInsertNewWidget&);
        
        QWidget* createShapeToolButton(const AnnotationTypeEnum::Enum annotationType);
        
        QToolButton* createSpaceToolButton(const AnnotationCoordinateSpaceEnum::Enum annotationSpace);
        
        QPixmap createShapePixmap(const QWidget* widget,
                                  const AnnotationTypeEnum::Enum annotationType);
        
        QPixmap createSpacePixmap(const QWidget* widget,
                                  const AnnotationCoordinateSpaceEnum::Enum annotationSpace);
        
        QToolButton* createFileSelectionToolButton();
        
        void createAnnotationWithType(const AnnotationTypeEnum::Enum annotationType);
        
        const int32_t m_browserWindowIndex;
        
        QActionGroup* m_spaceActionGroup;
        
        QAction* m_fileSelectionToolButtonAction;
        
        AnnotationMenuFileSelection* m_fileSelectionMenu;
        
        static AString s_previousImageFileDirectory;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_INSERT_NEW_WIDGET_DECLARE__
    AString AnnotationInsertNewWidget::s_previousImageFileDirectory;
#endif // __ANNOTATION_INSERT_NEW_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_INSERT_NEW_WIDGET_H__

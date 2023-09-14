#ifndef __ANNOTATION_TEXT_ORIENTATION_WIDGET_H__
#define __ANNOTATION_TEXT_ORIENTATION_WIDGET_H__

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

#include "AnnotationTextOrientationEnum.h"
#include "UserInputModeEnum.h"

class QActionGroup;
class QToolButton;

namespace caret {

    class Annotation;
    class AnnotationText;
    
    class AnnotationTextOrientationWidget : public QWidget {
        
        Q_OBJECT

    public:
        AnnotationTextOrientationWidget(const UserInputModeEnum::Enum userInputMode,
                                        const int32_t browserWindowIndex,
                                        QWidget* parent = 0);
        
        virtual ~AnnotationTextOrientationWidget();
        

        // ADD_NEW_METHODS_HERE

        void updateContent(std::vector<AnnotationText*>& annotationTexts);
    
    private slots:
        void orientationActionSelected(QAction* action);

    private:
        AnnotationTextOrientationWidget(const AnnotationTextOrientationWidget&);

        AnnotationTextOrientationWidget& operator=(const AnnotationTextOrientationWidget&);
        
        QToolButton* createOrientationToolButton(const AnnotationTextOrientationEnum::Enum orientation);
        
        QPixmap createHorizontalAlignmentPixmap(const QWidget* widget,
                                                const AnnotationTextOrientationEnum::Enum orientation);
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        const int32_t m_browserWindowIndex;
        
        std::vector<AnnotationText*> m_annotations;
        
        QActionGroup* m_orientationActionGroup;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TEXT_ORIENTATION_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_ORIENTATION_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_ORIENTATION_WIDGET_H__

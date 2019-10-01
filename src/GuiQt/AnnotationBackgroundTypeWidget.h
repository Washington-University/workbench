#ifndef __ANNOTATION_BACKGROUND_TYPE_WIDGET_H__
#define __ANNOTATION_BACKGROUND_TYPE_WIDGET_H__

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

class QSpinBox;

namespace caret {

    class AnnotationBrowserTab;
    class EnumComboBoxTemplate;
    
    class AnnotationBackgroundTypeWidget: public QWidget {
        
        Q_OBJECT

    public:
        enum WhichCoordinate {
            COORDINATE_ONE,
            COORDINATE_TWO
        };
        
        AnnotationBackgroundTypeWidget(const UserInputModeEnum::Enum userInputMode,
                                   const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                   const int32_t browserWindowIndex,
                                   QWidget* parent = 0);
        
        virtual ~AnnotationBackgroundTypeWidget();
        

        // ADD_NEW_METHODS_HERE

        void updateContent(std::vector<AnnotationBrowserTab*>& selectedBrowserTabAnnotations);
        
    private slots:
        void tileTabsLayoutBackgroundTypeEnumComboBoxItemActivated();
        
        void stackingOrderValueChanged(int value);
        
    private:
        AnnotationBackgroundTypeWidget(const AnnotationBackgroundTypeWidget&);

        AnnotationBackgroundTypeWidget& operator=(const AnnotationBackgroundTypeWidget&);
        
        // ADD_NEW_MEMBERS_HERE

        const UserInputModeEnum::Enum m_userInputMode;
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const int32_t m_browserWindowIndex;
        
        std::vector<AnnotationBrowserTab*> m_annotationBrowserTabs;
        
        EnumComboBoxTemplate* m_TileTabsLayoutBackgroundTypeEnumComboBox;
        
        QSpinBox* m_stackingOrderSpinBox;
        
    };
    
#ifdef __ANNOTATION_BACKGROUND_TYPE_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_BACKGROUND_TYPE_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_BACKGROUND_TYPE_WIDGET_H__

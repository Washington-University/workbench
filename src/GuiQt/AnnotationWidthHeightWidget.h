#ifndef __ANNOTATION_WIDTH_HEIGHT_WIDGET_H__
#define __ANNOTATION_WIDTH_HEIGHT_WIDGET_H__

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

#include <vector>
#include <QWidget>

#include "AnnotationWidgetParentEnum.h"
#include "UserInputModeEnum.h"

class QDoubleSpinBox;

namespace caret {

    class AnnotationOneCoordinateShape;
    
    class AnnotationWidthHeightWidget : public QWidget {
        
        Q_OBJECT

    public:
        AnnotationWidthHeightWidget(const UserInputModeEnum::Enum userInputMode,
                                    const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                    const int32_t browserWindowIndex,
                                    const Qt::Orientation orientation,
                                    QWidget* parent = 0);
        
        virtual ~AnnotationWidthHeightWidget();
        

        // ADD_NEW_METHODS_HERE

        void updateContent(std::vector<AnnotationOneCoordinateShape*>& annotations2D);

    private slots:
        void widthValueChanged(double value);
        
        void heightValueChanged(double value);
        
    private:
        AnnotationWidthHeightWidget(const AnnotationWidthHeightWidget&);

        AnnotationWidthHeightWidget& operator=(const AnnotationWidthHeightWidget&);
        
        // ADD_NEW_MEMBERS_HERE

        const UserInputModeEnum::Enum m_userInputMode;
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const int32_t m_browserWindowIndex;
        
        QDoubleSpinBox* m_widthSpinBox;
        
        QDoubleSpinBox* m_heightSpinBox;
        
        std::vector<AnnotationOneCoordinateShape*> m_annotations2D;
        
    };
    
#ifdef __ANNOTATION_WIDTH_HEIGHT_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_WIDTH_HEIGHT_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_WIDTH_HEIGHT_WIDGET_H__

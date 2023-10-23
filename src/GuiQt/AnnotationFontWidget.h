#ifndef __ANNOTATION_FONT_WIDGET_H__
#define __ANNOTATION_FONT_WIDGET_H__

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
#include "CaretColorEnum.h"
#include "EventListenerInterface.h"
#include "UserInputModeEnum.h"

class QToolButton;

namespace caret {

    class Annotation;
    class AnnotationFontAttributesInterface;
    class AnnotationText;
    class CaretColorEnumMenu;
    class EnumComboBoxTemplate;
    class WuQDoubleSpinBox;
    
    class AnnotationFontWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        AnnotationFontWidget(const Qt::Orientation orientation,
                             const AnnotationWidgetParentEnum::Enum parentWidgetType,
                             const UserInputModeEnum::Enum userInputMode,
                             const int32_t browserWindowIndex,
                             QWidget* parent = 0);
        
        virtual ~AnnotationFontWidget();

        void updateContent(std::vector<AnnotationFontAttributesInterface*>& annotations);

        virtual void receiveEvent(Event* event) override;
        
        // ADD_NEW_METHODS_HERE
        
    private slots:
        void textColorSelected(const CaretColorEnum::Enum);
        
        void fontBoldChanged();

        void fontItalicChanged();
        
        void fontNameChanged();
        
        void fontSizeChanged();
        
        void fontUnderlineChanged();
        
    private:
        AnnotationFontWidget(const AnnotationFontWidget&);

        AnnotationFontWidget& operator=(const AnnotationFontWidget&);
        
        void updateFontSizeSpinBox(const float value,
                                   const bool haveMultipleValuesFlag,
                                   const bool tooSmallFontFlag);
        
        void updateTextColorButton();
        
        void updateFontNameControls();
        
        void updateFontSizeControls();
        
        void updateFontStyleControls();
        
        float getSurfaceMontageRowCount() const;
        
        static std::vector<Annotation*> convertToAnnotations(const std::vector<AnnotationFontAttributesInterface*>& fontInterfaces);
        
        const AnnotationWidgetParentEnum::Enum m_parentWidgetType;
        
        const UserInputModeEnum::Enum m_userInputMode;
        
        const int32_t m_browserWindowIndex;
        
        EnumComboBoxTemplate* m_fontNameComboBox;

        WuQDoubleSpinBox* m_fontSizeSpinBox;
        
        QPalette m_fontSizeSpinBoxDefaultPalette;
        
        QPalette m_fontSizeSpinBoxRedTextPalette;
        
        QToolButton* m_textColorToolButton;
        
        QAction* m_textColorAction;
        
        CaretColorEnumMenu* m_textColorMenu;
        
        QAction* m_boldFontAction;
        
        QAction* m_italicFontAction;
        
        QAction* m_underlineFontAction;
        
        /** Contains annotations supporting font name */
        std::vector<AnnotationFontAttributesInterface*> m_annotationsFontName;
        
        /** Contains annotations supporting font style */
        std::vector<AnnotationFontAttributesInterface*> m_annotationsFontSize;
        
        /** Contains annotations supporting font style */
        std::vector<AnnotationFontAttributesInterface*> m_annotationsFontStyle;
        
        /** Contains annotations supporting font color */
        std::vector<AnnotationFontAttributesInterface*> m_annotationsFontColor;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FONT_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_FONT_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_FONT_WIDGET_H__

#ifndef __ANNOTATION_TEXT_EDITOR_WIDGET_H__
#define __ANNOTATION_TEXT_EDITOR_WIDGET_H__

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
#include <QLineEdit>
#include <QWidget>

namespace caret {

    class AnnotationLineEdit;
    class AnnotationText;
    class EnumComboBoxTemplate;
    
    class AnnotationTextEditorWidget : public QWidget {
        
        Q_OBJECT

    public:
        AnnotationTextEditorWidget(const int32_t browserWindowIndex,
                                   QWidget* parent = 0);
        
        virtual ~AnnotationTextEditorWidget();
        

        // ADD_NEW_METHODS_HERE

        void updateContent(std::vector<AnnotationText*>& annotationTexts);

    private slots:
        void annotationTextChanged();
        
        void displayTextEditor();
        
        void textEditorDialogTextChanged(const QString&);
        
        void annotationTextConnectTypeEnumComboBoxItemActivated();
        
    private:
        AnnotationTextEditorWidget(const AnnotationTextEditorWidget&);

        AnnotationTextEditorWidget& operator=(const AnnotationTextEditorWidget&);
        
        void updateLineEditText(const QString& text);
        
        const int32_t m_browserWindowIndex;
        
        AnnotationText* m_annotationText;
        
        AnnotationLineEdit* m_textLineEdit;
        
        EnumComboBoxTemplate* m_annotationTextConnectTypeEnumComboBox;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
    /**
     * A line edit that emits a signal if the user double-clicks
     * in the line edit.
     */
    class AnnotationLineEdit : public QLineEdit {
        Q_OBJECT
        
    public:
        AnnotationLineEdit(QWidget* parent = 0) : QLineEdit(parent) { }
        
        virtual ~AnnotationLineEdit() { }
        
    signals:
        void doubleClickInLineEdit();
        
    protected:
        void mouseDoubleClickEvent(QMouseEvent*) { emit doubleClickInLineEdit(); }
    };
    
#ifdef __ANNOTATION_TEXT_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_EDITOR_WIDGET_H__

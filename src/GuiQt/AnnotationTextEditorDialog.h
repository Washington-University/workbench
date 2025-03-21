#ifndef __ANNOTATION_TEXT_EDITOR_DIALOG_H__
#define __ANNOTATION_TEXT_EDITOR_DIALOG_H__

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


#include <QDialog>

#include "UserInputModeEnum.h"

namespace caret {

    class WuQTextEditWithToolBarWidget;
    
    class AnnotationText;
    
    class AnnotationTextEditorDialog : public QDialog {
        
        Q_OBJECT

    public:
        AnnotationTextEditorDialog(const UserInputModeEnum::Enum userInputMode,
                                   AnnotationText* textAnnotation,
                                   QWidget* parent);
        
        virtual ~AnnotationTextEditorDialog();
        
        virtual void done(int resultCode) override;
        
    private slots:
        void textWasEdited();
        
        // ADD_NEW_METHODS_HERE

    private:        
        AnnotationTextEditorDialog(const AnnotationTextEditorDialog&);

        AnnotationTextEditorDialog& operator=(const AnnotationTextEditorDialog&);
                
        const UserInputModeEnum::Enum m_userInputMode;
        
        AnnotationText* m_textAnnotation;
        
        QString m_uneditedText;
        
        WuQTextEditWithToolBarWidget* m_textEdit;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_TEXT_EDITOR_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_TEXT_EDITOR_DIALOG_DECLARE__

} // namespace
#endif  //__ANNOTATION_TEXT_EDITOR_DIALOG_H__

#ifndef __ANNOTATION_PASTE_DIALOG_H__
#define __ANNOTATION_PASTE_DIALOG_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include "AnnotationCoordinateInformation.h"
#include "WuQDialogModal.h"

class QRadioButton;

namespace caret {

    class Annotation;
    class AnnotationCoordinateInformation;
    class AnnotationFile;
    class AnnotationPastingInformation;
    class MouseEvent;
    
    class AnnotationPasteDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static Annotation* pasteAnnotationOnClipboard(const MouseEvent& mouseEvent);
        
        static Annotation* pasteAnnotationOnClipboardChangeSpace(const MouseEvent& mouseEvent);
        
        virtual ~AnnotationPasteDialog();
        
        Annotation* getAnnotationThatWasCreated();

        // ADD_NEW_METHODS_HERE

    private:
        AnnotationPasteDialog(const MouseEvent& mouseEvent,
                              const AnnotationPastingInformation& annotationPastingInformation,
                              AnnotationFile* annotationFile,
                              const Annotation* annotation,
                              const AString& informationMessage,
                              QWidget* parent = 0);
        
        AnnotationPasteDialog(const AnnotationPasteDialog&);

        AnnotationPasteDialog& operator=(const AnnotationPasteDialog&);
        
        static bool pasteAnnotationInSpace(AnnotationFile* annotationFile,
                                           Annotation* annotation,
                                           const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                           const AnnotationPastingInformation& annotationPastingInformation);

        virtual void okButtonClicked();
        
        static void adjustTextAnnotationFontHeight(const MouseEvent& mouseEvent,
                                                   const AnnotationCoordinateSpaceEnum::Enum previousSpace,
                                                   Annotation* annotation);
        
        const MouseEvent& m_mouseEvent;
        
        const AnnotationPastingInformation& m_annotationPastingInformation;
        
        AnnotationFile* m_annotationFile;
        
        const Annotation* m_annotation;
        
        Annotation* m_annotationThatWasCreated;
        
        std::vector<QRadioButton*> m_spaceRadioButtons;
        
        std::vector<AnnotationCoordinateSpaceEnum::Enum> m_spaceRadioButtonsSpaces;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_PASTE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_PASTE_DIALOG_DECLARE__

} // namespace
#endif  //__ANNOTATION_PASTE_DIALOG_H__

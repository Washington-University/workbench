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



namespace caret {

    class Annotation;
    class AnnotationCoordinateInformation;
    class AnnotationCoordinateSelectionWidget;
    class AnnotationFile;
    class AnnotationOneDimensionalShape;
    class MouseEvent;
    
    class AnnotationPasteDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        static Annotation* pasteAnnotationOnClipboard(const MouseEvent& mouseEvent,
                                                      const int32_t windowIndex);
        
        AnnotationPasteDialog(const MouseEvent& mouseEvent,
                              AnnotationFile* annotationFile,
                              const Annotation* annotation,
                              QWidget* parent = 0);
        
        virtual ~AnnotationPasteDialog();
        
        Annotation* getAnnotationThatWasCreated();

        // ADD_NEW_METHODS_HERE

    private:
        AnnotationPasteDialog(const AnnotationPasteDialog&);

        AnnotationPasteDialog& operator=(const AnnotationPasteDialog&);
        
        static bool pasteOneDimensionalShape(AnnotationOneDimensionalShape* oneDimShape,
                                             AnnotationCoordinateInformation& coordInfo);
        
        virtual void okButtonClicked();
        
        const MouseEvent& m_mouseEvent;
        
        AnnotationFile* m_annotationFile;
        
        const Annotation* m_annotation;
        
        AnnotationCoordinateSelectionWidget* m_coordinateSelectionWidget;
        
        AnnotationCoordinateInformation m_coordInfo;
        
        Annotation* m_annotationThatWasCreated;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_PASTE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_PASTE_DIALOG_DECLARE__

} // namespace
#endif  //__ANNOTATION_PASTE_DIALOG_H__

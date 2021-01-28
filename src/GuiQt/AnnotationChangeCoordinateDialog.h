#ifndef __ANNOTATION_CHANGE_COORDINATE_DIALOG_H__
#define __ANNOTATION_CHANGE_COORDINATE_DIALOG_H__

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

#include <memory>
#include "UserInputModeAnnotations.h"
#include "WuQDialogModal.h"



namespace caret {

    class Annotation;
    class AnnotationCoordinate;
    class AnnotationCoordinateSelectionWidget;
    
    class AnnotationChangeCoordinateDialog : public WuQDialogModal {
        
        Q_OBJECT

    public:
        AnnotationChangeCoordinateDialog(const AnnotationCoordinateInformation& coordInfo,
                                         Annotation* annotation,
                                         AnnotationCoordinate* coordinate,
                                         AnnotationCoordinate* secondCoordinate,
                                         QWidget* parent);
        
        virtual ~AnnotationChangeCoordinateDialog();

        virtual void okButtonClicked();

        // ADD_NEW_METHODS_HERE

    private:
        AnnotationChangeCoordinateDialog(const AnnotationChangeCoordinateDialog&);

        AnnotationChangeCoordinateDialog& operator=(const AnnotationChangeCoordinateDialog&);
        
        QWidget* createCurrentCoordinateWidget();
        
        QWidget* createNewCoordinateWidget();
        
        const AnnotationCoordinateInformation& m_coordInfo;
        
        Annotation* m_annotation;
        
        AnnotationCoordinate* m_coordinate;
        
        AnnotationCoordinate* m_secondCoordinate;
        
        AnnotationCoordinateSelectionWidget* m_coordinateSelectionWidget;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_CHANGE_COORDINATE_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_CHANGE_COORDINATE_DIALOG_DECLARE__

} // namespace
#endif  //__ANNOTATION_CHANGE_COORDINATE_DIALOG_H__

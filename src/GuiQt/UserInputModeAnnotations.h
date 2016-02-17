#ifndef __USER_INPUT_MODE_ANNOTATIONS_H__
#define __USER_INPUT_MODE_ANNOTATIONS_H__

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

#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationSizingHandleTypeEnum.h"
#include "AnnotationSurfaceOffsetVectorTypeEnum.h"
#include "AnnotationTypeEnum.h"
#include "CaretPointer.h"
#include "EventListenerInterface.h"
#include "StructureEnum.h"
#include "UserInputModeView.h"

namespace caret {

    class Annotation;
    class AnnotationCoordinate;
    class AnnotationCoordinateInformation;
    class AnnotationFile;
    class AnnotationOneDimensionalShape;
    class AnnotationTwoDimensionalShape;
    class KeyEvent;
    class MouseEvent;
    class SelectionItemAnnotation;
    class UserInputModeAnnotationsWidget;
    
    class UserInputModeAnnotations : public UserInputModeView, EventListenerInterface {
        
    public:
        /**
         * Annotation mode
         */
        enum Mode {
            /** Mouse creates an annotation by clicking */
            MODE_NEW_WITH_CLICK,
            /** Mouse creates an annotation by draggin from one point to another */
            MODE_NEW_WITH_DRAG,
            /** User selected Paste from Edit Menu, user may need to click mouse to paste the annotation */
            MODE_PASTE,
            /** Mouse selects annotation */
            MODE_SELECT,
            /** Set coordinate one in annotation*/
            MODE_SET_COORDINATE_ONE,
            /** Set coordinate two in annotation*/
            MODE_SET_COORDINATE_TWO
        };
        
        UserInputModeAnnotations(const int32_t windowIndex);
        
        virtual ~UserInputModeAnnotations();
        
        virtual void receiveEvent(Event* event);
        
        virtual void initialize();
        
        virtual void finish();
        
        virtual void update();
        
        Mode getMode() const;
        
        virtual void keyPressEvent(const KeyEvent& /*keyEvent*/);
        
        virtual void mouseLeftDoubleClick(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftClick(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftClickWithShift(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDrag(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithAlt(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithCtrl(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithCtrlShift(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftDragWithShift(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftPress(const MouseEvent& mouseEvent);
        
        virtual void mouseLeftRelease(const MouseEvent& mouseEvent);

        virtual void mouseMove(const MouseEvent& mouseEvent);

        virtual void mouseMoveWithShift(const MouseEvent& mouseEvent);
        
        virtual void showContextMenu(const MouseEvent& mouseEvent,
                                     const QPoint& menuPosition,
                                     BrainOpenGLWidget* openGLWidget);
        
        virtual CursorEnum::Enum getCursor() const;

        virtual AString toString() const;
        
        virtual void processEditMenuItemSelection(const BrainBrowserWindowEditMenuItemEnum::Enum editMenuItem);
        
        virtual void getEnabledEditMenuItems(std::vector<BrainBrowserWindowEditMenuItemEnum::Enum>& enabledEditMenuItemsOut,
                                             AString& redoMenuItemSuffixTextOut,
                                             AString& undoMenuItemSuffixTextOut);
        
        // ADD_NEW_METHODS_HERE

    private:
        class NewMouseDragCreateAnnotation {
        public:
            NewMouseDragCreateAnnotation(AnnotationFile* annotationFile,
                                         const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                         const AnnotationTypeEnum::Enum annotationType,
                                         const MouseEvent& mousePressEvent);
            
            ~NewMouseDragCreateAnnotation();
            
            void update(const int32_t mouseWindowX,
                        const int32_t mouseWindowY);
            
            void setCoordinate(AnnotationCoordinate* coordinate,
                               const int32_t x,
                               const int32_t y);
            
            const Annotation* getAnnotation() const;

        private:
            AnnotationFile* m_annotationFile;
            
            Annotation* m_annotation;
            
            int32_t m_mousePressWindowX;
            
            int32_t m_mousePressWindowY;
            
            int32_t m_windowOriginX;
            
            int32_t m_windowOriginY;
            
            float m_windowWidth;
            
            float m_windowHeight;
        };
        
        class NewAnnotationFileSpaceAndType {
        public:
            NewAnnotationFileSpaceAndType(AnnotationFile* annotationFile,
                                          AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                          AnnotationTypeEnum::Enum annotationType)
            : m_annotationFile(annotationFile),
            m_annotationSpace(annotationSpace),
            m_annotationType(annotationType) { }
            
            AnnotationFile*                     m_annotationFile;
            AnnotationCoordinateSpaceEnum::Enum m_annotationSpace;
            AnnotationTypeEnum::Enum            m_annotationType;
        };
        
        UserInputModeAnnotations(const UserInputModeAnnotations&);

        UserInputModeAnnotations& operator=(const UserInputModeAnnotations&);
        
        void setMode(const Mode mode);
        
        void processModeNewMouseLeftClick(const MouseEvent& mouseEvent);
        
        void processMouseSelectAnnotation(const MouseEvent& mouseEvent,
                                          const bool shiftKeyDownFlag);
        
        void processModeSetCoordinate(const MouseEvent& mouseEvent);
        
        void setAnnotationUnderMouse(const MouseEvent& mouseEvent,
                                     SelectionItemAnnotation* annotationIDIn);
        
        void createNewAnnotationFromMouseDrag(const MouseEvent& mouseEvent);
        
        void userDrawingAnnotationFromMouseDrag(const MouseEvent& mouseEvent);
        
        void selectAnnotation(Annotation* annotation);
        
        Annotation* getSingleSelectedAnnotation();
        
        void cutAnnotation();
        
        void deleteSelectedAnnotations();
        
        void resetAnnotationUnderMouse();
        
        bool isEditMenuValid() const;
        
        void pasteAnnotationFromAnnotationClipboard(const MouseEvent& mouseEvent);
        
//        bool pasteOneDimensionalShape(AnnotationOneDimensionalShape* oneDimShape,
//                                      AnnotationCoordinateInformation& coordInfo);
        
        UserInputModeAnnotationsWidget* m_annotationToolsWidget;
        
        Mode m_mode;
        
        const int32_t m_browserWindowIndex;
        
        Annotation* m_annotationUnderMouse;
        
        AnnotationSizingHandleTypeEnum::Enum m_annotationUnderMouseSizeHandleType;
        
        Annotation* m_annotationBeingDragged;
        
        AnnotationSizingHandleTypeEnum::Enum m_annotationBeingDraggedHandleType;
        
        CaretPointer<NewAnnotationFileSpaceAndType> m_modeNewAnnotationFileSpaceAndType;
        
        CaretPointer<NewMouseDragCreateAnnotation> m_newAnnotationCreatingWithMouseDrag;
        
        bool m_allowMultipleSelectionModeFlag;
        
        // ADD_NEW_MEMBERS_HERE

        /*
         * Some private methods are accessed by this friend class
         */
        friend class UserInputModeAnnotationsContextMenu;
        friend class UserInputModeAnnotationsWidget;
    };
    
#ifdef __USER_INPUT_MODE_ANNOTATIONS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_ANNOTATIONS_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_ANNOTATIONS_H__

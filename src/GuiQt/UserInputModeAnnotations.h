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

#include "AnnotationSizingHandleTypeEnum.h"
#include "AnnotationTypeEnum.h"
#include "EventListenerInterface.h"
#include "StructureEnum.h"
#include "UserInputModeView.h"

namespace caret {

    class Annotation;
    class KeyEvent;
    class MouseEvent;
    class UserInputModeAnnotationsWidget;
    
    class UserInputModeAnnotations : public UserInputModeView, EventListenerInterface {
        
    public:
        /**
         * Annotation mode
         */
        enum Mode {
            /** Mouse creates an annotation */
            MODE_NEW,
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
        
        virtual void mouseMove(const MouseEvent& mouseEvent);

        virtual void showContextMenu(const MouseEvent& mouseEvent,
                                     const QPoint& menuPosition,
                                     BrainOpenGLWidget* openGLWidget);
        
        virtual CursorEnum::Enum getCursor() const;

        virtual AString toString() const;
        
        class CoordinateInformation {
        public:
            CoordinateInformation() {
                reset();
            };
            
            void reset() {
                m_modelXYZValid    = false;
                m_surfaceNodeValid = false;
                m_tabIndex         = -1;
                m_windowIndex      = -1;
                m_modelXYZ[0]  = 0.0;
                m_modelXYZ[1]  = 0.0;
                m_modelXYZ[2]  = 0.0;
                m_tabXYZ[0]    = 0.0;
                m_tabXYZ[1]    = 0.0;
                m_tabXYZ[2]    = 0.0;
                m_windowXYZ[0] = 0.0;
                m_windowXYZ[1] = 0.0;
                m_windowXYZ[2] = 0.0;
            }
            
            double m_modelXYZ[3];
            bool   m_modelXYZValid;
            
            double m_tabXYZ[3];
            int32_t m_tabIndex;
            
            double m_windowXYZ[3];
            int32_t m_windowIndex;
            
            StructureEnum::Enum m_surfaceStructure;
            int32_t m_surfaceNumberOfNodes;
            int32_t m_surfaceNodeIndex;
            float m_surfaceNodeOffset;
            bool m_surfaceNodeValid;
        };
        static void getCoordinatesFromMouseLocation(const MouseEvent& mouseEvent,
                                                    CoordinateInformation& coordInfoOut);
        
        // ADD_NEW_METHODS_HERE

    private:
        UserInputModeAnnotations(const UserInputModeAnnotations&);

        UserInputModeAnnotations& operator=(const UserInputModeAnnotations&);
        
        void setMode(const Mode mode);
        
        void processModeNewMouseLeftClick(const MouseEvent& mouseEvent);
        
        void processMouseSelectAnnotation(const MouseEvent& mouseEvent,
                                          const bool shiftKeyDownFlag);
        
        void processModeSetCoordinate(const MouseEvent& mouseEvent);
        
        UserInputModeAnnotationsWidget* m_annotationToolsWidget;
        
        Mode m_mode;
        
        const int32_t m_browserWindowIndex;
        
        AnnotationTypeEnum::Enum m_modeNewAnnotationType;

        Annotation* m_annotationBeingEdited;
        
        Annotation* m_annotationUnderMouse;
        
        AnnotationSizingHandleTypeEnum::Enum m_annotationUnderMouseSizeHandleType;
        
        // ADD_NEW_MEMBERS_HERE

        /*
         * Some private methods are accessed by this friend class
         */
        friend class UserInputModeAnnotationsWidget;
    };
    
#ifdef __USER_INPUT_MODE_ANNOTATIONS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __USER_INPUT_MODE_ANNOTATIONS_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_ANNOTATIONS_H__

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
#include "MouseEvent.h"
#include "StructureEnum.h"
#include "UserInputModeView.h"
#include "Vector3D.h"

namespace caret {

    class Annotation;
    class AnnotationCoordinate;
    class AnnotationCoordinateInformation;
    class AnnotationFile;
    class AnnotationRedoUndoCommand;
    class AnnotationTwoCoordinateShape;
    class AnnotationOneCoordinateShape;
    class BrowserTabContent;
    class CaretUndoStack;
    class KeyEvent;
    class Plane;
    class SelectionItemAnnotation;
    class UserInputModeAnnotationsWidget;
    
    class UserInputModeAnnotations : public UserInputModeView, EventListenerInterface {
        
    public:
        /**
         * Annotation mode
         */
        enum class Mode {
            /**
             * Add coordinates to new poly type annotation as mouse is clicked or dragged.
             * Note: while drawing, window coordinates are used and when user completes
             * drawing the annotation, coordinates in the selected space are generated.
             */
            MODE_DRAWING_NEW_POLY_TYPE,
            /** Mouse starts drawing a new poly type annoatation */
            MODE_DRAWING_NEW_POLY_TYPE_INITIALIZE,
            /** Mouse updates new annotation as mouse is clicked */
            MODE_DRAWING_NEW_POLY_TYPE_STEREOTAXIC,
            /** Mouse starts a new annotaiton with a series of clicks (used with polyline) */
            MODE_DRAWING_NEW_POLY_TYPE_STEREOTAXIC_INITIALIZE,
            /** As mouse is dragged, the shape is updated */
            MODE_DRAWING_NEW_SIMPLE_SHAPE,
            /** Clicking the mouse gererates a new annotation.  Dragging the mouse starts a new annotation */
            MODE_DRAWING_NEW_SIMPLE_SHAPE_INITIALIZE,
            /** User selected Paste from Edit Menu, user may need to click mouse to paste the annotation */
            MODE_PASTE,
            /** User selected Paste Special from Edit Menu, user may need to click mouse to paste the annotation */
            MODE_PASTE_SPECIAL,
            /** Mouse selects annotation */
            MODE_SELECT
        };
        
        /**
         * Operations for drawing and editing poly-type annotations
         */
        enum class PolyTypeDrawEditOperation {
            /** Mouse adds new coordinate to poly-type */
            ADD_NEW_COORDINATE,
            /** Cancel drawing of new annotation */
            CANCEL_NEW_ANNOTATION,
            /** Erase last coordinate (available while drawing) */
            ERASE_LAST_COORDINATE,
            /** Finish drawing of new annotation */
            FINISH_NEW_ANNOTATION,
            /** Mouse inserts coordinate */
            INSERT_COORDINATE,
            /** Mouse moves one existing coordinate */
            MOVE_ONE_COORDINATE,
            /** Mouse move coordinate and its corrsponding coordinate at other end of polyhedron */
            MOVE_TWO_COORDINATES,
            /** Mouse removes coordinate */
            REMOVE_COORDINATE
        };
        
        UserInputModeAnnotations(const int32_t browserWindowIndex);
        
        virtual ~UserInputModeAnnotations();
        
        virtual void receiveEvent(Event* event);
        
        virtual void initialize();
        
        virtual void finish();
        
        virtual void update();
        
        Mode getMode() const;
        
        void getEnabledPolyTypeDrawEditOperations(std::vector<PolyTypeDrawEditOperation>& operationsOut,
                                                  const Annotation* &selectedAnnotationOut) const;
        
        PolyTypeDrawEditOperation getPolyTypeDrawEditOperation() const;
        
        virtual bool keyPressEvent(const KeyEvent& /*keyEvent*/) override;
        
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
        
        virtual void gestureEvent(const GestureEvent& gestureEvent);
        
        virtual void showContextMenu(const MouseEvent& mouseEvent,
                                     const QPoint& menuPosition,
                                     BrainOpenGLWidget* openGLWidget);
        
        virtual CursorEnum::Enum getCursor() const;

        virtual AString toString() const;
        
        virtual void processEditMenuItemSelection(const BrainBrowserWindowEditMenuItemEnum::Enum editMenuItem);
        
        virtual void getEnabledEditMenuItems(std::vector<BrainBrowserWindowEditMenuItemEnum::Enum>& enabledEditMenuItemsOut,
                                             AString& redoMenuItemSuffixTextOut,
                                             AString& undoMenuItemSuffixTextOut,
                                             AString& pasteTextOut,
                                             AString& pasteSpecialTextOut);
        
        CaretUndoStack* getUndoRedoStack();
        
        // ADD_NEW_METHODS_HERE

    protected:
        UserInputModeAnnotations(const UserInputModeEnum::Enum userInputMode,
                                 const int32_t browserWindowIndex);
        
        virtual void processMouseSelectAnnotation(const MouseEvent& mouseEvent,
                                                  const bool shiftKeyDownFlag,
                                                  const bool singleSelectionModeFlag);
        
        void processDeselectAllAnnotations();
        
        void processSelectAllAnnotations();
        
    private:
        /**
         * Type of poly-type annotation for drawing / editing
         * This is not stored in a variable but is a function of the MODE
         */
        enum class PolyAnnotationType {
            /** Drawing a polyline or polygon annotation (before finishing) */
            ANNOTATION_POLY_DRAWING_NEW,
            /** Editing a polyline or polygon annotation (after finishing) */
            ANNOTATION_POLY_EDITING,
            /** Drawing a new sample polyhedron (before finishing) */
            SAMPLE_POLYHEDRON_DRAWING_NEW,
            /** Editing a sample polyhedron  (after finishing) */
            SAMPLE_POLYHEDRON_EDITING
        };
        
        /**
         * Supports drawing of a new annotation in the space selected by the user
         */
        class NewUserSpaceAnnotation {
        public:
            NewUserSpaceAnnotation(QWidget* parentWidgetForDialogs,
                                   AnnotationFile* annotationFile,
                                   const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                   const AnnotationTypeEnum::Enum annotationType,
                                   const MouseEvent& mousePressEvent,
                                   const UserInputModeEnum::Enum userInputMode,
                                   const int32_t browserWindowIndex);

            ~NewUserSpaceAnnotation();
            
            void addCoordinate(const MouseEvent& mouseEvent);
            
            void insertCoordinate(const int32_t coordinateIndex,
                                  const float normalizedDistanceToNextCoordinate);
            
            void moveCoordinateOneAtIndex(const int32_t coordinateIndex,
                                          const Vector3D& xyz,
                                          const bool startOfDraggingFlag);
            
            void moveCoordinateTwoAtIndex(const int32_t coordinateIndex,
                                          const Vector3D& xyz,
                                          const bool startOfDraggingFlag);
            
            void removeCoordinateAtIndex(const int32_t coordinateIndex);
            
            void eraseLastCoordinate();
            
            bool finishSamplesAnnotation();
            
            bool isValid() const;
            
            AnnotationFile* getAnnotationFile() const { return m_annotationFile; }
            
            Annotation* getAnnotation() const { return m_annotation.get(); }
            
            int32_t getViewportHeight() const { return m_viewportHeight; }
            
            float getSliceThickness() const { return m_sliceThickness; }
            
            CaretUndoStack* getUndoRedoStack() { return m_undoRedoStack.get(); }
            
        private:
            bool getSamplesDrawingCoordinates(const MouseEvent& mouseEvent,
                                              const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                              Vector3D& firstSliceCoordOut,
                                              Vector3D& lastSliceCoordOut,
                                              Plane& firstPlaneOut,
                                              Plane& lastPlaneOut);

            void moveCoordinateAtIndex(const int32_t coordinateIndex,
                                       const Vector3D& xyz,
                                       const bool moveTwoFlag,
                                       const bool startOfDraggingFlag);

            void applyCommand(AnnotationRedoUndoCommand* command);
            
            QWidget* m_parentWidgetForDialogs;
            
            AnnotationFile* m_annotationFile = NULL;
            
            std::unique_ptr<Annotation> m_annotation;
            
            int32_t m_viewportHeight = 0;
            
            const UserInputModeEnum::Enum m_userInputMode;
            
            const int32_t m_browserWindowIndex = -1;
            
            int32_t m_browserTabIndex = -1;
            
            float m_sliceThickness = 1.0;
            
            bool m_validFlag = false;
            
            std::unique_ptr<CaretUndoStack> m_undoRedoStack;
        };
        
        /**
         * Supports drawing of a new annotation, initially in window space, and then converted
         * to space selected by user when finished.  If vertices of annotation are not in user's
         * selected space, then the user is given a choice of coordinates in valid spaces
         */
        class NewMouseDragCreateAnnotation {
        public:
            class CoordInfo {
            public:
                CoordInfo() { }
                
                CoordInfo(const Vector3D& xyz,
                          const MouseEvent& mouseEvent)
                : m_xyz(xyz) {
                    m_mouseEvent.reset(new MouseEvent(mouseEvent));
                }
                
                Vector3D m_xyz;
                std::unique_ptr<MouseEvent> m_mouseEvent;
            };
            
            NewMouseDragCreateAnnotation(AnnotationFile* annotationFile,
                                         const AnnotationCoordinateSpaceEnum::Enum annotationSpace,
                                         const AnnotationTypeEnum::Enum annotationType,
                                         const MouseEvent& mousePressEvent,
                                         const int32_t drawingViewportHeight);
            
            ~NewMouseDragCreateAnnotation();
            
            void update(const MouseEvent& mouseEvent,
                        const int32_t mouseWindowX,
                        const int32_t mouseWindowY);
            
            void setCoordinate(AnnotationCoordinate* coordinate,
                               const int32_t x,
                               const int32_t y);
            
            Annotation* getAnnotation();

            const Annotation* getAnnotation() const;
            
            AnnotationFile* getAnnotationFile();
            
            const AnnotationFile* getAnnotationFile() const;
            
            std::vector<Vector3D> getDrawingCoordinates() const;
            
            const MouseEvent* getLastMouseEvent() const;
            
            int32_t getDrawingViewportHeight() const;
            
            void insertCoordinateAtIndex(const MouseEvent& mouseEvent,
                                         const int32_t coordinateIndex);
            
            void moveCoordinateAtIndex(const MouseEvent& mouseEvent,
                                         const int32_t coordinateIndex);
            
            void removeCoordinateAtIndex(const int32_t coordinateIndex);
            
            void eraseLastCoordinate();
            
            CaretUndoStack* getUndoRedoStack() { return m_undoRedoStack.get(); }
            
        private:
            const int32_t m_drawingViewportHeight;
            
            AnnotationFile* m_annotationFile;
            
            Annotation* m_annotation;
            
            int32_t m_mousePressWindowX;
            
            int32_t m_mousePressWindowY;
            
            int32_t m_windowOriginX;
            
            int32_t m_windowOriginY;
            
            float m_windowWidth;
            
            float m_windowHeight;
            
            std::vector<CoordInfo> m_drawingCoordinateAndMouseEvents;
            
            std::unique_ptr<CaretUndoStack> m_undoRedoStack;
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
        
        PolyAnnotationType getPolyAnnotationTypeFromMode(const Mode mode) const;
        
        void setPolyTypeDrawEditOperation(const PolyTypeDrawEditOperation drawEditMode);
        
        void createNewAnnotationAtMouseLeftClick(const MouseEvent& mouseEvent);
        
        void setAnnotationUnderMouse(const MouseEvent& mouseEvent,
                                     SelectionItemAnnotation* annotationIDIn);
        
        void finishCreatingNewAnnotationDrawnByUser(const MouseEvent& mouseEvent);
        
        void userDrawingAnnotationFromMouseDrag(const MouseEvent& mouseEvent);
        
        void initializeUserDrawingNewAnnotation(const MouseEvent& mouseEvent);

        void initializeUserDrawingNewPolyTypeAnnotation(const MouseEvent& mouseEvent);
        
        void initializeUserDrawingNewPolyTypeStereotaxicAnnotation(const MouseEvent& mouseEvent);
        
        void addCooordinateToNewPolyTypeStereotaxicAnnotation(const MouseEvent& mouseEvent);
        
        void removeCooordinateFromNewPolyTypeStereotaxicAnnotation();
        
        void insertCooordinateIntoNewPolyTypeStereotaxicAnnotation();
        
        void moveOneCooordinateInNewPolyTypeStereotaxicAnnotation(const MouseEvent& mouseEvent);
        
        void moveTwoCooordinatesInNewPolyTypeStereotaxicAnnotation(const MouseEvent& mouseEvent);
        
        void finishNewPolyTypeStereotaxicAnnotation();
        
        void selectAnnotation(Annotation* annotation);
        
        Annotation* getSingleSelectedAnnotation() const;
        
        void cutAnnotation();
        
        virtual void deleteSelectedAnnotations();
        
        void resetAnnotationUnderMouse();
        
        bool isEditMenuExceptRedoUndoValid() const;
        
        bool isEditMenuRedoUndoValid() const;
        
        void pasteAnnotationFromAnnotationClipboard(const MouseEvent& mouseEvent);
        
        void pasteAnnotationFromAnnotationClipboardAndChangeSpace(const MouseEvent& mouseEvent);
        
        void resetAnnotationBeingCreated();

        void deselectAnnotationsForEditingInAnnotationManager();
        
        void groupAnnotationsAfterPasting(std::vector<Annotation*>& pastedAnnotations);
        
        bool getHistologyStep(BrowserTabContent* browserTabContent,
                              float& stepXOut,
                              float& stepYOut);

        bool getMediaStep(BrowserTabContent* browserTabContent,
                          float& stepXOut,
                          float& stepYOut);
        
        bool isDrawingNewSample() const;
        
        bool isOnePolyTypeAnnotationSelected(const std::vector<Annotation*>& annotations) const;
        
        void addCoordinateToAnnotation(const MouseEvent& mouseEvent,
                                       Annotation* annotation);
        
        Annotation* getSelectedPolyTypeAnnotation() const;
        
        bool mouseEventToStereotaxicCoordinate(const MouseEvent& mouseEvent,
                                               Vector3D& xyzOut) const;
        
        UserInputModeAnnotationsWidget* m_annotationToolsWidget;
        
        Mode m_mode;
        
        /** Edit Samples Mode - Used when the polyhedron is being drawn and before finishing */
        mutable PolyTypeDrawEditOperation m_polyhedronDrawingNewOperation = PolyTypeDrawEditOperation::ADD_NEW_COORDINATE;
        
        /** Edit Samples Mode - Used when the polyhedron is being editited anytime after finishing */
        mutable PolyTypeDrawEditOperation m_polyhedronEditingOperation = PolyTypeDrawEditOperation::MOVE_ONE_COORDINATE;
        
        /** Annotation - Polyline, polygon DRAWING NEW  before finishing*/
        mutable PolyTypeDrawEditOperation m_annotationPolyTypeDrawingNewOperation = PolyTypeDrawEditOperation::ADD_NEW_COORDINATE;
        
        /** Annotation - Polyline, polygon EDITING anytime after finishing*/
        mutable PolyTypeDrawEditOperation m_annotationPolyTypeEditingOperation = PolyTypeDrawEditOperation::MOVE_ONE_COORDINATE;
        
        Annotation* m_annotationUnderMouse;
        
        AnnotationSizingHandleTypeEnum::Enum m_annotationUnderMouseSizeHandleType;
        
        int32_t m_annotationUnderMousePolyLineCoordinateIndex;
        
        /**
         * Normalized (0 to 1) distance from point on line where mouse was clicked from the selected
         * coordinate index (m_annotationUnderMousePolyLineCoordinateIndex) to the next coordinate index
         */
        float m_annotationUnderMousePolyLineNormalizedDistance = 0.0;
        
        AnnotationSizingHandleTypeEnum::Enum m_annotationBeingDraggedHandleType;
        
        CaretPointer<NewAnnotationFileSpaceAndType> m_modeNewAnnotationFileSpaceAndType;
        
        std::unique_ptr<NewMouseDragCreateAnnotation> m_newAnnotationCreatingWithMouseDrag;
                
        std::unique_ptr<NewUserSpaceAnnotation> m_newUserSpaceAnnotationBeingCreated;
        
        std::vector<Vector3D> m_lastSelectedAnnotationWindowCoordinates;

        bool m_allowMultipleSelectionModeFlag;
        
        /*
         * Set when anntoation selection changes
         * Reset in mouseRelease
         */
        bool m_mouseReleasedAnnotationsChangedFlag = false;
        
        /*
         * Set when anntoation selection changes
         * Reset in mouseClick
         */
        bool m_mouseClickAnnotationsChangedFlag = false;
        
        /**
         * Used so that getUndoRedoStack() always returns a valid undo stack
         */
        std::unique_ptr<CaretUndoStack> m_dummyUndoRedoStack;
        
        // ADD_NEW_MEMBERS_HERE

        static constexpr bool s_allowInsertionIntoPolyTypesFlag = false;

        /*
         * Some private methods are accessed by this friend class
         */
        friend class AnnotationPolyTypeDrawEditWidget;
        friend class UserInputModeAnnotationsContextMenu;
        friend class UserInputModeAnnotationsWidget;
        friend class UserInputModeTileTabsLayout;
        friend class UserInputModeTileTabsLayoutContextMenu;
    };
    
#ifdef __USER_INPUT_MODE_ANNOTATIONS_DECLARE__
#endif // __USER_INPUT_MODE_ANNOTATIONS_DECLARE__

} // namespace
#endif  //__USER_INPUT_MODE_ANNOTATIONS_H__

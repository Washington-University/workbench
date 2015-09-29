#ifndef __ANNOTATION_UNDO_COMMAND_H__
#define __ANNOTATION_UNDO_COMMAND_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#include "AnnotationCoordinate.h"
#include "AnnotationRedoUndoCommandModeEnum.h"
#include "AnnotationText.h"
#include "CaretColorEnum.h"
#include "CaretUndoCommand.h"



namespace caret {

    class Annotation;
    
    class AnnotationRedoUndoCommand : public CaretUndoCommand {
        
    public:
        AnnotationRedoUndoCommand();
        
        virtual ~AnnotationRedoUndoCommand();
        
        virtual void redo();
        
        virtual void undo();
        
        int32_t count() const;
        
        virtual bool mergeWith(const CaretUndoCommand* command);
        
        void setModeCoordinateOne(const AnnotationCoordinate& coordinate,
                                  const std::vector<Annotation*>& annotations);
        
        void setModeCoordinateOneAndTwo(const AnnotationCoordinate& coordinateOne,
                                        const AnnotationCoordinate& coordinateTwo,
                                        const std::vector<Annotation*> annotations);
        
        void setModeCoordinateTwo(const AnnotationCoordinate& coordinate,
                                  const std::vector<Annotation*>& annotations);
        
        void setModeLineArrowStart(const bool newStatus,
                                   const std::vector<Annotation*>& annotations);
        
        void setModeLineArrowEnd(const bool newStatus,
                                   const std::vector<Annotation*>& annotations);
        
        void setModeLineWidthForeground(const float newLineWidth,
                                        const std::vector<Annotation*>& annotations);
        
        void setModeColorBackground(const CaretColorEnum::Enum color,
                                    const float customColor[4],
                                    const std::vector<Annotation*>& annotations);
        
        void setModeColorForeground(const CaretColorEnum::Enum color,
                                    const float customColor[4],
                                    const std::vector<Annotation*>& annotations);
        
        void setModeDeleteAnnotations(const std::vector<Annotation*>& annotations);
        
        void setModeLocationAndSize(const std::vector<Annotation*>& annotationsBeforeMoveAndResize,
                                    const std::vector<Annotation*>& annotationsAfterMoveAndResize);
        
        void setModeRotationAngle(const float newRotationAngle,
                                  const std::vector<Annotation*>& annotations);
        
        void setModeTextAlignmentHorizontal(const AnnotationTextAlignHorizontalEnum::Enum newHorizontalAlignment,
                                            const std::vector<Annotation*>& annotations);
        
        void setModeTextAlignmentVertical(const AnnotationTextAlignVerticalEnum::Enum newVerticalAlignment,
                                            const std::vector<Annotation*>& annotations);
        
        void setModeTextCharacters(const AString& text,
                                   const std::vector<Annotation*>& annotations);
    
        void setModeTextConnectToBrainordinate(const AnnotationTextConnectTypeEnum::Enum newConnectType,
                                               const std::vector<Annotation*>& annotations);
        
        void setModeTextFontBold(const bool newStatus,
                                 const std::vector<Annotation*>& annotations);
        
        void setModeTextFontItalic(const bool newStatus,
                                   const std::vector<Annotation*>& annotations);
        
        void setModeTextFontName(const AnnotationTextFontNameEnum::Enum newFontName,
                                   const std::vector<Annotation*>& annotations);
        
        void setModeTextFontPointSize(const AnnotationTextFontPointSizeEnum::Enum newFontPointSize,
                                   const std::vector<Annotation*>& annotations);
        
        void setModeTextFontPercentSize(const float newFontPercentSize,
                                        const std::vector<Annotation*>& annotations);
        
        void setModeTextFontUnderline(const bool newStatus,
                                      const std::vector<Annotation*>& annotations);
        
        void setModeTextOrientation(const AnnotationTextOrientationEnum::Enum newTextOrientation,
                                    const std::vector<Annotation*>& annotations);
        
        void setModeTwoDimHeight(const float newHeight,
                                 const std::vector<Annotation*>& annotations);
        
        void setModeTwoDimWidth(const float newWidth,
                                const std::vector<Annotation*>& annotations);
        
        // ADD_NEW_METHODS_HERE

    private:
        /**
         * The annotation memento contains copies of the
         * annotation before and after its modification.
         */
        class AnnotationMemento {
        public:
            AnnotationMemento(Annotation* annotation,
                              Annotation* redoAnnotation,
                              Annotation* undoAnnotation) {
                m_annotation     = annotation;
                m_redoAnnotation = redoAnnotation;
                m_undoAnnotation = undoAnnotation;
            }
            
            ~AnnotationMemento() {
                if (m_redoAnnotation != NULL) {
                    delete m_redoAnnotation;
                }
                if (m_undoAnnotation != NULL) {
                    delete m_undoAnnotation;
                }
            }

            bool operator=(const AnnotationMemento& am) const {
                return (m_annotation < am.m_annotation);
            }
            
            Annotation* m_annotation;
            
            Annotation* m_undoAnnotation;
            
            Annotation* m_redoAnnotation;
            
        };
        
        AnnotationRedoUndoCommand(const AnnotationRedoUndoCommand&);

        AnnotationRedoUndoCommand& operator=(const AnnotationRedoUndoCommand&);

        void applyRedoOrUndo(Annotation* annotation,
                             const Annotation* annotationValue) const;

        void sortAnnotationMementos() const;
        
        static bool equalAnnotationMemento(const AnnotationMemento* am1,
                                           const AnnotationMemento* am2);

        static bool lessThanAnnotationMemento(const AnnotationMemento* am1,
                                              const AnnotationMemento* am2);
        
        AnnotationRedoUndoCommandModeEnum::Enum m_mode;
        
        mutable std::vector<AnnotationMemento*> m_annotationMementos;
        
        mutable bool m_sortedFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_UNDO_COMMAND_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_UNDO_COMMAND_DECLARE__

} // namespace
#endif  //__ANNOTATION_UNDO_COMMAND_H__

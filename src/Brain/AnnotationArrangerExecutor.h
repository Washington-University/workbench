#ifndef __ANNOTATION_ARRANGER_EXECUTOR_H__
#define __ANNOTATION_ARRANGER_EXECUTOR_H__

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

#include <map>

#include "BoundingBox.h"
#include "CaretObject.h"



namespace caret {

    class Annotation;
    class AnnotationArrangerInputs;
    class AnnotationManager;
    
    class AnnotationArrangerExecutor : public CaretObject {
        
    public:
        AnnotationArrangerExecutor();
        
        virtual ~AnnotationArrangerExecutor();
        
        bool alignAnnotations(AnnotationManager* annotationManager,
                              const AnnotationArrangerInputs& arrangerInputs,
                              AString& errorMessageOut);
        

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        class AnnotationInfo {
        public:
            AnnotationInfo(Annotation* annotation,
                           const int32_t viewport[4],
                           const BoundingBox windowBoundingBox,
                           float viewportPixelOneXY[2],
                           float viewportPixelTwoXY[2]);
            
            void print() const;
            
            Annotation* m_annotation;
            
            int32_t m_viewport[4];
            
            float m_viewportPixelOneXY[2];
            
            BoundingBox m_windowBoundingBox;
            
            float m_windowPixelOneXY[2];
            
            float m_windowPixelTwoXY[2];
        };
        
        AnnotationArrangerExecutor(const AnnotationArrangerExecutor&);

        AnnotationArrangerExecutor& operator=(const AnnotationArrangerExecutor&);
        
        void alignAnnotationsPrivate(const AnnotationArrangerInputs& arrangerInputs);
        
        void alignAnnotationToValue(const AnnotationArrangerInputs& arrangerInputs,
                                    const float alignToWindowCoordinateValue,
                                    AnnotationInfo& annotationInfo,
                                    std::vector<Annotation*>& annotationsBeforeMoving,
                                    std::vector<Annotation*>& annotationsAfterMoving);
        
        void getAnnotationsForArranging(const AnnotationArrangerInputs& arrangerInputs,
                                        std::vector<Annotation*>& annotationsOut) const;
        
        void getTabViewport(const int32_t tabIndex,
                            int32_t tabViewportOut[4]);
        
        void initializeForArranging(const AnnotationArrangerInputs& arrangerInputs);
        
        void setupAnnotationInfo(const AnnotationArrangerInputs& arrangerInputs,
                                 std::vector<Annotation*>& annotations);
        
        void printAnnotationInfo(const QString& title);
        
        AnnotationManager* m_annotationManager;
        
        int32_t m_windowViewport[4];
        
        struct ViewportArray {
            int32_t m_viewport[4];
        };
        
        std::map<int32_t, ViewportArray> m_tabViewports;
        
        BoundingBox m_allAnnotationsBoundingBox;
        
        std::vector<AnnotationInfo> m_annotationInfo;
        
        bool m_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_ARRANGER_EXECUTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_ARRANGER_EXECUTOR_DECLARE__

} // namespace
#endif  //__ANNOTATION_ARRANGER_EXECUTOR_H__

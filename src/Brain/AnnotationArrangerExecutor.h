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
        AnnotationArrangerExecutor(const AnnotationArrangerExecutor&);

        AnnotationArrangerExecutor& operator=(const AnnotationArrangerExecutor&);
        
        bool getAlignmentToValues(const AnnotationArrangerInputs& arrangerInputs,
                                  std::vector<Annotation*>& annotations,
                                  const int32_t windowViewport[4],
                                  std::vector<float>& alignToValuesOut,
                                  AString& errorMessageOut);
        
        AnnotationManager* m_annotationManager;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_ARRANGER_EXECUTOR_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_ARRANGER_EXECUTOR_DECLARE__

} // namespace
#endif  //__ANNOTATION_ARRANGER_EXECUTOR_H__

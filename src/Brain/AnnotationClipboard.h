#ifndef __ANNOTATION_CLIPBOARD_H__
#define __ANNOTATION_CLIPBOARD_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#include "CaretObject.h"
#include "Vector3D.h"

namespace caret {

    class Brain;
    class Annotation;
    class AnnotationFile;
    
    class AnnotationClipboard : public CaretObject {
        
    public:
        AnnotationClipboard(Brain* brain);
        
        virtual ~AnnotationClipboard();
        
        AnnotationClipboard(const AnnotationClipboard&) = delete;

        AnnotationClipboard& operator=(const AnnotationClipboard&) = delete;

        void clear();
        
        const Annotation* getAnnotation() const;
        
        Annotation* getCopyOfAnnotation() const;
        
        AnnotationFile* getAnnotationFile() const;
        
        const Vector3D& getMouseWindowCoordinates() const;
        
        const std::vector<Vector3D>& getAnnotationWindowCoordinates() const;
        
        bool isAnnotationValid() const;
        
        void set(AnnotationFile* annotationFile,
                 const Annotation* annotation,
                 std::vector<Vector3D>& annotationWindowCoordinates,
                 Vector3D& mouseWindowCoordinates);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        Brain* m_brain = NULL;
        
        std::unique_ptr<Annotation> m_annotation;
        
        /** Points to a file so NEVER delete this */
        mutable AnnotationFile* m_annotationFile = NULL;
        
        std::vector<Vector3D> m_annotationWindowCoordinates;
        
        Vector3D m_mouseWindowCoordinates;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_CLIPBOARD_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_CLIPBOARD_DECLARE__

} // namespace
#endif  //__ANNOTATION_CLIPBOARD_H__

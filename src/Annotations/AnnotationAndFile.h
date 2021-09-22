#ifndef __ANNOTATION_AND_FILE_H__
#define __ANNOTATION_AND_FILE_H__

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

#include "AnnotationGroupKey.h"
#include "CaretObject.h"


namespace caret {

    class Annotation;
    class AnnotationFile;
    
    class AnnotationAndFile : public CaretObject {
        
    public:
//        AnnotationAndFile(Annotation* annotation,
//                          AnnotationFile* file);
        
        AnnotationAndFile(Annotation* annotation,
                          AnnotationFile* file,
                          const AnnotationGroupKey& groupKey);
        
        virtual ~AnnotationAndFile();
        
        AnnotationAndFile(const AnnotationAndFile& obj);

        AnnotationAndFile& operator=(const AnnotationAndFile& obj);
        
        bool operator==(const AnnotationAndFile& obj) const;
        
        /** @return The annotation */
        inline Annotation* getAnnotation() const { return m_annotation; }
        
        /** @return The annotation file containing the annotation */
        inline AnnotationFile* getFile() const { return m_file; }

        /** @return The annotation group key */
        inline const AnnotationGroupKey getGroupKey() const { return m_groupKey; }
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperAnnotationAndFile(const AnnotationAndFile& obj);

        Annotation* m_annotation;
        
        AnnotationFile* m_file;

        AnnotationGroupKey m_groupKey;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_AND_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_AND_FILE_DECLARE__

} // namespace
#endif  //__ANNOTATION_AND_FILE_H__

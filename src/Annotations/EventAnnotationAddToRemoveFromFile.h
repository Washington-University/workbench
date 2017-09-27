#ifndef __EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE_H__
#define __EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE_H__

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


#include "Event.h"



namespace caret {

    class Annotation;
    class AnnotationFile;
    
    class EventAnnotationAddToRemoveFromFile : public Event {
        
    public:
        enum Mode {
            MODE_CREATE,
            MODE_CUT,
            MODE_DELETE,
            MODE_DUPLICATE,
            MODE_PASTE,
            MODE_UNCREATE,
            MODE_UNCUT,
            MODE_UNDELETE,
            MODE_UNDUPLICATE,
            MODE_UNPASTE
        };
        
        EventAnnotationAddToRemoveFromFile(const Mode mode,
                                            AnnotationFile* annotationFile,
                                            Annotation* annotation);
        
        virtual ~EventAnnotationAddToRemoveFromFile();
        
        Mode getMode() const;
        
        AnnotationFile* getAnnotationFile() const;
        
        Annotation* getAnnotation() const;

        bool isSuccessful() const;
        
        void setSuccessful(const bool status);
        
        // ADD_NEW_METHODS_HERE

    private:
        EventAnnotationAddToRemoveFromFile(const EventAnnotationAddToRemoveFromFile&);

        EventAnnotationAddToRemoveFromFile& operator=(const EventAnnotationAddToRemoveFromFile&);
        
        const Mode m_mode;
        
        AnnotationFile* m_annotationFile;
        
        Annotation* m_annotation;
        
        bool m_successFlag;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE_H_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE_H_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_ADD_TO_REMOVE_FROM_FILE_H__

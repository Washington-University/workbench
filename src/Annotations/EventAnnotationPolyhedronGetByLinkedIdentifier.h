#ifndef __EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER_H__
#define __EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#include "AnnotationPolyhedronTypeEnum.h"
#include "Event.h"



namespace caret {
    class AnnotationPolyhedron;
    class AnnotationFile;

    class EventAnnotationPolyhedronGetByLinkedIdentifier : public Event {
        
    public:
        EventAnnotationPolyhedronGetByLinkedIdentifier(const AnnotationFile* annotationFile,
                                                       const AnnotationPolyhedronTypeEnum::Enum polyhedronType,
                                                       const AString& linkedIdentifier);
        
        virtual ~EventAnnotationPolyhedronGetByLinkedIdentifier();
        
        const AnnotationFile* getAnnotationFile() const;
        
        AnnotationPolyhedronTypeEnum::Enum getPolyhedronType() const;
        
        AString getLinkedIdentifier() const;
        
        AnnotationPolyhedron* getPolyhedron() const;
        
        void setPolyhedron(AnnotationPolyhedron* polyhedron);
        
        EventAnnotationPolyhedronGetByLinkedIdentifier(const EventAnnotationPolyhedronGetByLinkedIdentifier&) = delete;

        EventAnnotationPolyhedronGetByLinkedIdentifier& operator=(const EventAnnotationPolyhedronGetByLinkedIdentifier&) = delete;
        

        // ADD_NEW_METHODS_HERE

    private:
        const AnnotationFile* m_annotationFile;
        
        const AnnotationPolyhedronTypeEnum::Enum m_polyhedronType;
        
        const AString m_linkedIdentifier;
        
        AnnotationPolyhedron* m_annotationPolyhedron = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER_DECLARE__

} // namespace
#endif  //__EVENT_ANNOTATION_POLYHEDRON_GET_BY_LINKED_IDENTIFIER_H__

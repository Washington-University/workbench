#ifndef __ANNOTATION_FILE_H__
#define __ANNOTATION_FILE_H__

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


#include "CaretDataFile.h"
#include "CaretPointer.h"
#include "EventListenerInterface.h"


namespace caret {

    class Annotation;
    class SceneClassAssistant;
    
    class AnnotationFile : public CaretDataFile, public EventListenerInterface {
        
    public:
        AnnotationFile();
        
        virtual ~AnnotationFile();
        
        AnnotationFile(const AnnotationFile& obj);

        AnnotationFile& operator=(const AnnotationFile& obj);

        bool isEmpty() const;

        virtual void clear();
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;

        void addAnnotation(Annotation* annotation);

        const std::vector<Annotation*>& getAllAnnotations() const;
        
        int32_t getNumberOfAnnotations() const;
        
        Annotation* getAnnotation(const int32_t index);
        
        const Annotation* getAnnotation(const int32_t index) const;
        
        bool removeAnnotation(const Annotation* annotation);
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        void setAllAnnotationsSelected(const bool selectedStatus);
        
        virtual void receiveEvent(Event* event);

        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString& filename);
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
        // ADD_NEW_METHODS_HERE
        
          
          
          
          
    protected: 
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationFile(const AnnotationFile& obj);

        void clearPrivate();
        
        SceneClassAssistant* m_sceneAssistant;

        CaretPointer<GiftiMetaData> m_metadata;
        
        std::vector<Annotation*> m_annotations;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __ANNOTATION_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_FILE_DECLARE__

} // namespace
#endif  //__ANNOTATION_FILE_H__

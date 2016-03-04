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

#include <set>

#include <QSharedPointer>

#include "AnnotationCoordinateSpaceEnum.h"
#include "CaretDataFile.h"
#include "CaretPointer.h"
#include "DataFileContentCopyMoveInterface.h"
#include "EventListenerInterface.h"


namespace caret {

    class Annotation;
    class AnnotationGroup;
    class SceneClassAssistant;
    
    class AnnotationFile
    : public CaretDataFile,
    public EventListenerInterface,
    public DataFileContentCopyMoveInterface {
        
    public:
        /**
         * Sub-type of the annotation file
         */
        enum AnnotationFileSubType {
            /** 
             * Normal annotation file which saves its annotation
             * to a file
             */
            ANNOTATION_FILE_SAVE_TO_FILE,
            
            /**
             * Special variant of annotation file that restores and saves the
             * annotations with the scene methods.  This is used by the 
             * "Brain" for the scene annotation file that is only saved
             * to scenes and never saved to a file.
             */
            ANNOTATION_FILE_SAVE_TO_SCENE
        };
        
        AnnotationFile();
        
        AnnotationFile(const AnnotationFileSubType fileSubType);
        
        virtual ~AnnotationFile();
        
        AnnotationFile(const AnnotationFile& obj);

        AnnotationFile& operator=(const AnnotationFile& obj);

        bool isEmpty() const;

        virtual void clear();
        
        virtual StructureEnum::Enum getStructure() const;
        
        virtual void setStructure(const StructureEnum::Enum structure);
        
        virtual GiftiMetaData* getFileMetaData();
        
        virtual const GiftiMetaData* getFileMetaData() const;

        void getAllAnnotations(std::vector<Annotation*>& annotationsOut) const;
        
        void getAllAnnotationGroups(std::vector<AnnotationGroup*>& annotationGroupsOut) const;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        void setAllAnnotationsSelected(const int32_t windowIndex,
                                       const bool selectedStatus);
        
        virtual void receiveEvent(Event* event);

        virtual void readFile(const AString& filename);
        
        virtual void writeFile(const AString& filename);
        
        virtual bool isModified() const;
        
        virtual void clearModified();
        
        virtual DataFile* getAsDataFile();
        
        virtual void appendContentFromDataFile(const DataFileContentCopyMoveInterface* dataFileCopyMoveInterface);
        
        virtual DataFileContentCopyMoveInterface* newInstanceOfDataFile() const;
        

        // ADD_NEW_METHODS_HERE
        
          
          
          
          
    protected: 
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                             SceneClass* sceneClass);

        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                                  const SceneClass* sceneClass);

    private:
        void copyHelperAnnotationFile(const AnnotationFile& obj);

        void clearPrivate();
        
        void createExampleAnnotations();
        
        void initializeAnnotationFile();
        
        void addAnnotationPrivate(Annotation* annotation);
        
        void addAnnotationDuringFileReading(Annotation* annotation);
        
        bool restoreAnnotation(Annotation* annotation);
        
        bool restoreAnnotationAddIfNotFound(Annotation* annotation);
        
        bool removeAnnotation(Annotation* annotation);
        
        int32_t generateUniqueKey();
        
        void updateUniqueKeysAfterReadingFile();
        
        AnnotationGroup* getSpaceAnnotationGroup(const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                                                 const int32_t tabOrWindowIndex);
        
        const AnnotationFileSubType m_fileSubType;
        
        SceneClassAssistant* m_sceneAssistant;

        CaretPointer<GiftiMetaData> m_metadata;
        
        int32_t m_uniqueKeyGenerator;
        
        std::vector<QSharedPointer<AnnotationGroup> > m_annotationGroups;
        
        /**
         * Contains annotation that have been delete/removed so that
         * they can be 'undeleted' or 're-pasted'.
         */
        std::set<QSharedPointer<Annotation> > m_removedAnnotations;
                
        typedef std::vector<QSharedPointer<AnnotationGroup> >::iterator AnnotationGroupIterator;
        
        typedef std::vector<QSharedPointer<AnnotationGroup> >::const_iterator AnnotationGroupConstIterator;
        
        // ADD_NEW_MEMBERS_HERE

        friend class AnnotationFileXmlReader;
    };
    
#ifdef __ANNOTATION_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_FILE_DECLARE__

} // namespace
#endif  //__ANNOTATION_FILE_H__

#ifndef __ANNOTATION_GROUP_H__
#define __ANNOTATION_GROUP_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include <QSharedPointer>

#include "AnnotationCoordinateSpaceEnum.h"
#include "AnnotationGroupKey.h"
#include "CaretObjectTracksModification.h"
#include "DisplayGroupAndTabItemInterface.h"
#include "SceneableInterface.h"


namespace caret {
    class Annotation;
    class DisplayGroupAndTabItemHelper;
    class SceneClassAssistant;

    class AnnotationGroup : public CaretObjectTracksModification, public DisplayGroupAndTabItemInterface, public SceneableInterface {
        
    public:
        AnnotationGroup(AnnotationFile* annotationFile,
                        const AnnotationGroupTypeEnum::Enum groupType,
                        const int32_t uniqueKey,
                        const AnnotationCoordinateSpaceEnum::Enum coordinateSpace,
                        const int32_t tabOrWindowIndex);
        
        virtual ~AnnotationGroup();

        bool isEmpty() const;
        
        int32_t getUniqueKey() const;
        
        AString getName() const;
        
        AnnotationGroupKey getAnnotationGroupKey() const;
        
        AnnotationFile* getAnnotationFile() const;
        
        AnnotationGroupTypeEnum::Enum getGroupType() const;
        
        AnnotationCoordinateSpaceEnum::Enum getCoordinateSpace() const;
        
        int32_t getTabOrWindowIndex() const;
        
        int32_t getNumberOfAnnotations() const;
        
        Annotation* getAnnotation(const int32_t index);
        
        const Annotation* getAnnotation(const int32_t index) const;
        
        void getAllAnnotations(std::vector<Annotation*>& annotationsOut) const;

        void setAllAnnotationsSelectedForEditing(const int32_t windowIndex,
                                       const bool selectedStatus);
        
        bool isModified() const;
        
        void clearModified();
        
        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);

        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);

          
          
        virtual int32_t getNumberOfItemChildren() const;
        
        virtual DisplayGroupAndTabItemInterface* getItemChild(const int32_t index) const;
        
        virtual std::vector<DisplayGroupAndTabItemInterface*> getItemChildren() const;
        
        virtual DisplayGroupAndTabItemInterface* getItemParent() const;
        
        virtual void setItemParent(DisplayGroupAndTabItemInterface* itemParent);
        
        virtual AString getItemName() const;
        
        virtual void getItemIconColorsRGBA(float backgroundRgbaOut[4],
                                           float outlineRgbaOut[4],
                                           float textRgbaOut[4]) const;
        
        virtual bool isItemExpandable() const;
        
        virtual bool isItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex) const;
        
        virtual void setItemExpanded(const DisplayGroupEnum::Enum displayGroup,
                                     const int32_t tabIndex,
                                     const bool status);
        
        virtual TriStateSelectionStatusEnum::Enum getItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                                    const int32_t tabIndex) const;
        
        virtual void setItemDisplaySelected(const DisplayGroupEnum::Enum displayGroup,
                                     const int32_t tabIndex,
                                     const TriStateSelectionStatusEnum::Enum status);
        
          
          
// If there will be sub-classes of this class that need to save
// and restore data from scenes, these pure virtual methods can
// be uncommented to force their implementation by sub-classes.
//    protected: 
//        virtual void saveSubClassDataToScene(const SceneAttributes* sceneAttributes,
//                                             SceneClass* sceneClass) = 0;
//
//        virtual void restoreSubClassDataFromScene(const SceneAttributes* sceneAttributes,
//                                                  const SceneClass* sceneClass) = 0;

    private:
        AnnotationGroup(const AnnotationGroup& obj);
        
        AnnotationGroup& operator=(const AnnotationGroup& obj);
        
        void copyHelperAnnotationGroup(const AnnotationGroup& obj);

        void addAnnotationPrivate(Annotation* annotation);
        
        void addAnnotationPrivateSharedPointer(QSharedPointer<Annotation>& annotation);
        
        void assignGroupKeyToAnnotation(Annotation* annotation);
        
        bool validateAddedAnnotation(const Annotation* annotation);
        
        bool removeAnnotation(Annotation* annotation,
                              QSharedPointer<Annotation>& removedAnnotationOut);
        
        void removeAllAnnotations(std::vector<QSharedPointer<Annotation> >& allRemovedAnnotationsOut);
        
        int32_t getMaximumUniqueKey() const;
        
        void initializeInstance();
        
        SceneClassAssistant* m_sceneAssistant;

        DisplayGroupAndTabItemHelper* m_displayGroupAndTabItemHelper;
        
        AnnotationGroupKey m_groupKey;
        
        AnnotationCoordinateSpaceEnum::Enum m_coordinateSpace;
        
        int32_t m_tabOrWindowIndex;
        
        mutable AString m_name;
        
        std::vector<QSharedPointer<Annotation> > m_annotations;
        
        typedef std::vector<QSharedPointer<Annotation> >::iterator AnnotationIterator;
        
        typedef std::vector<QSharedPointer<Annotation> >::const_iterator AnnotationConstIterator;
        
        // ADD_NEW_MEMBERS_HERE

        friend class AnnotationFile;
        friend class AnnotationFileXmlReader;
        friend class AnnotationFileXmlWriter;
    };
    
#ifdef __ANNOTATION_GROUP_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __ANNOTATION_GROUP_DECLARE__

} // namespace
#endif  //__ANNOTATION_GROUP_H__

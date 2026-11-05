#ifndef __NEUROGLANCER_ANNOTATION_FILE_H__
#define __NEUROGLANCER_ANNOTATION_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#include "CaretDataFile.h"



namespace caret {

    class ComboBoxItemInterface;
    class FeatureLabelModel;
    class NeuroglancerAnnotation;
    class NeuroglancerPropertyInfo;
    
    class NeuroglancerAnnotationFile : public CaretDataFile {
        
    public:
        NeuroglancerAnnotationFile();
        
        virtual ~NeuroglancerAnnotationFile();
        
        NeuroglancerAnnotationFile(const NeuroglancerAnnotationFile&) = delete;

        NeuroglancerAnnotationFile& operator=(const NeuroglancerAnnotationFile&) = delete;
        
        const NeuroglancerAnnotationFile* castToNeuroglancerAnnotationFile() const;
        
        NeuroglancerAnnotationFile* castToNeuroglancerAnnotationFile();
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const override;
        
        GiftiMetaData* getFileMetaData();
        
        const GiftiMetaData* getFileMetaData() const;
        
        StructureEnum::Enum getStructure() const;
        
        void setStructure(const StructureEnum::Enum structure);
        
        void addAnnotation(NeuroglancerAnnotation* annotation);
        
        int32_t getNumberOfAnnotations() const;
        
        const NeuroglancerAnnotation* getAnnotation(const int32_t index) const;
        
        const NeuroglancerAnnotation* getAnnotationWithUniqueID(const uint64_t uniqueID) const;
        
        void addPropertyInfo(NeuroglancerPropertyInfo* propertyInfo);
        
        std::vector<FeatureLabelModel*> getLabelModels() const;
        
        NeuroglancerPropertyInfo* getSelectedColorProperty();
        
        const NeuroglancerPropertyInfo* getSelectedColorProperty() const;

        void setSelectedColorProperty(NeuroglancerPropertyInfo* colorProperty);
        
        std::vector<ComboBoxItemInterface*> getColorPropertiesForComboBox() const;
        
        const std::vector<const NeuroglancerPropertyInfo*> getAllPropertyInfos() const;
        
        void readFile(const AString& filename);
        
        void writeFile(const AString& filename);
        
        void clear();
        
        bool isEmpty() const;

        virtual bool isModified() const;
        
        virtual void clearModified();
        
        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE
        
        void validateSelectedColorProperty() const;
        
        void updateColoring();
        
        std::unique_ptr<GiftiMetaData> m_fileMetaData;

        std::vector<std::unique_ptr<NeuroglancerPropertyInfo>> m_propertyInfos;
        
        std::vector<std::unique_ptr<NeuroglancerAnnotation>> m_annotations;
        
        mutable NeuroglancerPropertyInfo* m_selectedColorProperty = NULL;
        
        mutable bool m_annotationColoringValidFlag = false;
        
        friend class NeuroglancerAnnotationFileImporter;
    };
    
#ifdef __NEUROGLANCER_ANNOTATION_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NEUROGLANCER_ANNOTATION_FILE_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_ANNOTATION_FILE_H__

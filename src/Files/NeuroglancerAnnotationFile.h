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
#include "Matrix4x4.h"


namespace caret {

    class ComboBoxItemInterface;
    class FeatureLabelModel;
    class NeuroglancerAnnotation;
    class NeuroglancerPropertyInfo;
    class SceneClassAssistant;
    
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
        
        NeuroglancerPropertyInfo* getPropertyWithName(const AString& name);
        
        const NeuroglancerPropertyInfo* getPropertyAtIndex(const int32_t propertyIndex) const;
        
        Matrix4x4 getIjkToXyzTransformationMatrix();
        
        const Matrix4x4 getIjkToXyzTransformationMatrix() const;
        
        void setIjkToXyzTransformationMatrix(const Matrix4x4& matrix);
        
        void setIjkToXyzTransformationMatrixComponent(const int32_t row,
                                                      const int32_t column,
                                                      const float value);

        bool isLeftNegxPosteriorPoszInferiorPosYOrientation() const;

        void setLeftNegxPosteriorPoszInferiorPosYOrientation(const bool status);
        
        void readFile(const AString& filename);
        
        void writeFile(const AString& filename);
        
        void clear();
        
        bool isEmpty() const;

        virtual bool isModified() const;
        
        virtual void clearModified();
        
        void updateDisplayedAnnotations();
        
        void updateAnnotationsXYZ();
        
        // ADD_NEW_METHODS_HERE

    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
    
    private:
        // ADD_NEW_MEMBERS_HERE
        
        void initializeIjkToXyzTransformationMatrix();
        
        void validateSelectedColorProperty() const;
        
        void updateColoring();
        
        void printDebugAnnotations();
        
        std::unique_ptr<GiftiMetaData> m_fileMetaData;

        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;
        
        std::vector<std::unique_ptr<NeuroglancerPropertyInfo>> m_propertyInfos;
        
        std::vector<std::unique_ptr<NeuroglancerAnnotation>> m_annotations;
        
        Matrix4x4 m_ijkToXyzTransformationMatrix;
        
        bool m_leftNegxPosteriorPoszInferiorPosYOrientationFlag = true;
        
        mutable NeuroglancerPropertyInfo* m_selectedColorProperty = NULL;
        
        mutable bool m_annotationColoringValidFlag = false;
        
        friend class NeuroglancerAnnotationFileImporter;
    };
    
#ifdef __NEUROGLANCER_ANNOTATION_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NEUROGLANCER_ANNOTATION_FILE_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_ANNOTATION_FILE_H__

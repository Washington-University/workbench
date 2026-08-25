#ifndef __FEATURE_FILE_H__
#define __FEATURE_FILE_H__

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

#include "CaretDataFileSelectionModel.h"
#include "EventListenerInterface.h"
#include "FeatureItemTypeEnum.h"
#include "FunctionResult.h"

class QFile;
class QStandardItem;

namespace caret {
    class FeatureItem;
    class FeatureItemModel;
    class FeatureLabelModel;
    class FileInformation;
    class NeuroglancerAnnotationFileImporter;
    class SceneClassAssistant;

    class FeatureFile : public CaretDataFile, public EventListenerInterface {
        
    public:
        FeatureFile();
        
        virtual ~FeatureFile();
        
        FeatureFile(const FeatureFile&) = delete;

        FeatureFile& operator=(const FeatureFile&) = delete;
        
        FunctionResult addFeature(QList<QStandardItem*>& featureAndProperties);
        
        virtual FeatureFile* castToFeatureFile() override;
        virtual const FeatureFile* castToFeatureFile() const override;

        Vector3D featureIJKtoXYZ(const FeatureItem* featureItem,
                                 const int32_t coordinateIndex) const;
        
        virtual void receiveEvent(Event* event) override;

        virtual bool isEmpty() const override;
        
        virtual StructureEnum::Enum getStructure() const override;
        
        virtual void setStructure(const StructureEnum::Enum structure) override;
        
        virtual GiftiMetaData* getFileMetaData() override;
        
        virtual const GiftiMetaData* getFileMetaData() const override;
        
        virtual bool supportsFileMetaData() const override;

        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const override;
        
        virtual bool supportsWriting() const override;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        FeatureItemModel* getFeatureItemModel();
        
        const FeatureItemModel* getFeatureItemModel() const;
        
        CaretDataFileSelectionModel* getVolumeFileSelectionModel();
        
        const CaretDataFileSelectionModel* getVolumeFileSelectionModel() const;
        
        int32_t getNumberOfLabelModels() const;
        
        FeatureLabelModel* getLabelModel(const int32_t index);

        const FeatureLabelModel* getLabelModel(const int32_t index) const;
        
        // ADD_NEW_METHODS_HERE
        
    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);

    private:
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::unique_ptr<GiftiMetaData> m_fileMetaData;
        
        std::unique_ptr<FeatureItemModel> m_featureModel;
                
        std::unique_ptr<CaretDataFileSelectionModel> m_volumeFileSelectionModel;
        
        std::vector<std::unique_ptr<FeatureLabelModel>> m_labelModels;
        
        std::unique_ptr<NeuroglancerAnnotationFileImporter> m_neuroglancerAnnotationFileImporter;
        
        bool m_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class NeuroglancerAnnotationFileImporter;

    };
    
#ifdef __FEATURE_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FEATURE_FILE_DECLARE__

} // namespace
#endif  //__FEATURE_FILE_H__

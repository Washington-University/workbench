#ifndef __NEUROGLANCER_PINS_FILE_H__
#define __NEUROGLANCER_PINS_FILE_H__

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

#include <QJsonArray>

#include "CaretDataFile.h"

#include "EventListenerInterface.h"
#include "NeuroglancerAnnotationTypeEnum.h"

class QJsonObject;
class QJsonValue;

namespace caret {
    class NeuroglancerAnnotation;
    class FileInformation;
    class SceneClassAssistant;

    class NeuroglancerAnnotationsFile : public CaretDataFile, public EventListenerInterface {
        
    public:
        enum class AnnotationFileType {
            AXIS_ALIGNED_BOUNDING_BOX,
            ELLIPSOID,
            LINE,
            POINT,
            POLYLINE
        };
        
        enum class Axis {
            X,
            Y,
            Z
        };
        
        enum class Unit {
            MILLIMETERS,
            SECONDS,
            UNITLESS
        };
        
        enum class DataType {
            INVALID,
            RGB,
            RGBA,
            UINT8,
            INT8,
            UINT16,
            INT16,
            UINT32,
            INT32,
            FLOAT32
        };
        
        class Dimension {
        public:
            Axis m_axis  = Axis::X;
            Unit m_units = Unit::UNITLESS;
            float m_unitScale = 1.0; /* converts to meters from mm, etc. */
            float m_resolution = 1.0;
            bool m_valid = false;
        };
                
        enum class PropertyType {
            INVALID,
            ENUMS,
            FLOAT,
            INTEGER,
            RGB,
            RGBA,
            UNSIGNED_INTEGER
        };
        
        class Property {
        public:
            DataType m_propertyType = DataType::INVALID;
            AString m_description;
            AString m_id;
            std::map<int32_t, AString> m_enumValueLabel;
            int64_t m_fileOffset = -1;
        };
        
        struct SpatialGrid
        {
            QString m_path;
            std::vector<int32_t> m_chunkSize;
            std::vector<int32_t> m_gridShape;
            int32_t m_limit = 0;
        };
        
        NeuroglancerAnnotationsFile();
        
        virtual ~NeuroglancerAnnotationsFile();
        
        NeuroglancerAnnotationsFile(const NeuroglancerAnnotationsFile&) = delete;

        NeuroglancerAnnotationsFile& operator=(const NeuroglancerAnnotationsFile&) = delete;
        
        int32_t getNumberOfAnnotations() const;
        
        NeuroglancerAnnotation* getAnnotation(const int32_t index);
        
        const NeuroglancerAnnotation* getAnnotation(const int32_t index) const;
        
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

        // ADD_NEW_METHODS_HERE
        
    protected:
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);

    private:
        void readNeuroglancerFile(const AString& filename);

        void readNeuroglancerJson(const FileInformation& fileInfo,
                                      const QJsonObject& topObject);
  
        void readDimensions(const QJsonObject &dimsObj);
        
        void readProperties(const QJsonArray &propsArr);
        
        std::vector<SpatialGrid> readSpatial(const QJsonArray &spatialArr);
        
        std::vector<float> readFloatArray(const QJsonArray &array);
        
        std::vector<int32_t> readIntArray(const QJsonArray &array);
        
        void readNeuroglancerAnnotationFiles();
        
        static AString dimensionToString(const Dimension& dimension);
        
        static AString dataTypeToString(const DataType& dataType);
        
        static AString annotationFileTypeToString(const AnnotationFileType& annotationFileType);
        
        std::unique_ptr<SceneClassAssistant> m_sceneAssistant;

        std::unique_ptr<GiftiMetaData> m_fileMetaData;
        
        AString m_infoFileDirectoryName;
        
        AString m_byIdDirectoryName;
        
        NeuroglancerAnnotationTypeEnum::Enum m_annotationType = NeuroglancerAnnotationTypeEnum::POINT;
        
        Dimension m_xDimension;
        
        Dimension m_yDimension;
        
        Dimension m_zDimension;
        
        std::vector<std::unique_ptr<NeuroglancerAnnotation>> m_annotations;
        
        /**
         * Origin (lower bound) of the grid
         */
        std::vector<float> m_upperBound;
        
        /**
         * Upper bound of grid
         */
        std::vector<float> m_lowerBound;
        
        std::vector<Property> m_properties;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __NEUROGLANCER_PINS_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NEUROGLANCER_PINS_FILE_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_PINS_FILE_H__

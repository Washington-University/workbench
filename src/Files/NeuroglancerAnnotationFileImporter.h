#ifndef __NEUROGLANCER_ANNOTATION_FILE_IMPORTER_H__
#define __NEUROGLANCER_ANNOTATION_FILE_IMPORTER_H__

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

#include "DataFile.h"

#include "FeatureItemTypeEnum.h"

class QFile;
class QJsonObject;
class QJsonValue;
class QStandardItem;

namespace caret {
    class FeatureFile;
    class FeatureItem;
    class FeatureItemModel;
    class FeatureLabelModel;
    class FileInformation;

    class NeuroglancerAnnotationFileImporter : public DataFile {
        
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
                
        class Dimension {
        public:
            Axis m_axis  = Axis::X;
            Unit m_units = Unit::UNITLESS;
            float m_unitScale = 1.0; /* converts to meters from mm, etc. */
            float m_resolution = 1.0;
            bool m_valid = false;
        };
                
        struct SpatialGrid
        {
            QString m_path;
            std::vector<int32_t> m_chunkSize;
            std::vector<int32_t> m_gridShape;
            int32_t m_limit = 0;
        };
        
        class Sharding {
        public:
            AString m_type;
            AString m_dataEncoding;
            AString m_hash;
            int32_t m_minishardBits = -1;
            AString m_minishardIndexEncoding;
            int32_t m_preshiftBits = -1;
            int32_t m_shardBits = -1;
            bool m_validFlag = false;
            
            void print(const AString& name) const;
        };
        
        class Relationship {
        public:
            AString m_id;
            
            AString m_directoryName;
            
            Sharding m_sharding;
        };
        
        class ChunkInfo {
        public:
            ChunkInfo() { }
            ChunkInfo(uint64_t cid, uint64_t offset, uint64_t csize)
            : m_id(cid), m_offset(offset), m_size(csize) { }
            uint64_t m_id = -1;
            uint64_t m_relativeOffset = -1;
            uint64_t m_offset = -1;
            uint64_t m_size = -1;
        };
        
        enum class NeuroglancerDataType {
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
        
        class NeuroglancerProperty {
        public:
            NeuroglancerDataType m_propertyType = NeuroglancerDataType::INVALID;
            AString m_description;
            AString m_id;
            std::map<int32_t, AString> m_enumValueLabel;
            int64_t m_fileOffset = -1;
            FeatureLabelModel* m_labelModel = NULL;
        };

        
        NeuroglancerAnnotationFileImporter(FeatureFile* featureFile);
        
        virtual ~NeuroglancerAnnotationFileImporter();
        
        NeuroglancerAnnotationFileImporter(const NeuroglancerAnnotationFileImporter&) = delete;

        NeuroglancerAnnotationFileImporter& operator=(const NeuroglancerAnnotationFileImporter&) = delete;
        
        virtual bool isEmpty() const override;
        
        void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation) const;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;

        // ADD_NEW_METHODS_HERE
        
    protected:

    private:
        enum class ShardingDataType {
            ANNOTATION,
            RELATIONSHIP
        };
        
        /**
         * Contains name and feature with properties that are added to
         * a model.  The model could be be a table or a tree.
         */
        class NewAnnoationInformation {
        public:
            NewAnnoationInformation(const uint64_t uniqueID,
                                    const QList<QStandardItem*>& featureAndPropertiesList)
            : m_uniqueID(uniqueID),
            m_featureAndPropertiesList(featureAndPropertiesList) { }
            
            const uint64_t m_uniqueID;
            const QList<QStandardItem*> m_featureAndPropertiesList;
        };
        

        static AString neuroglancerDataTypeToString(const NeuroglancerDataType& dataType);
        
        void readNeuroglancerInfoFile(const AString& filename);

        void parseNeuroglancerInfoFileJson(const FileInformation& fileInfo,
                                           const QJsonObject& topObject);
  
        void parseDimensionsObject(const QJsonObject &dimsObj);
        
        void parsePropertiesArray(const QJsonArray &propsArr);
        
        void parseByIdObject(const QJsonObject& byIdObject);
        
        Sharding parseShardingObject(const QJsonObject shardingObject);
        
        void parseRelationshipsArray(const QJsonArray& relationshipsArray);
        
        std::vector<SpatialGrid> parseSpatialArray(const QJsonArray &spatialArray);
        
        std::vector<float> parseFloatArray(const QJsonArray &array);
        
        std::vector<int32_t> parseIntArray(const QJsonArray &array);
        
        void readNeuroglancerAnnotationFiles();
        
        void readNeuroglancerAnnotationShardedFiles(const Sharding& shardingInfo,
                                                    const ShardingDataType shardingDataType,
                                                    const AString& shardingDataDirectoryName);
        
        void readEncodedMinishardIndex(QFile& file,
                                       const ShardingDataType shardingDataType,
                                       const AString& dataEncoding,
                                       const uint64_t shardIndexEnd,
                                       const AString& minishardIndexEncoding,
                                       const uint64_t minishardOffset,
                                       const uint64_t minishardLength);

        void processMinishardIndex(QFile& file,
                                   const ShardingDataType shardingDataType,
                                   const uint64_t shardIndexEnd,
                                   const QByteArray& minishardIndexData,
                                   const AString& dataEncoding,
                                   std::vector<ChunkInfo>& miniShardChunkInfoOut);
        
        void readChunksFromShardFile(QFile& file,
                                     const ShardingDataType shardingDataType,
                                     const AString& dataEncoding,
                                     const std::vector<ChunkInfo>& miniShardChunkInfo);
        
        static AString dimensionToString(const Dimension& dimension);
        
        static AString annotationFileTypeToString(const AnnotationFileType& annotationFileType);
        
        void addShardingToDataFileInformation(DataFileContentInformation& dataFileInformation,
                                              const AString& shardingName,
                                              const Sharding& sharding) const;
        
        NewAnnoationInformation readAnnotationFromDataStream(QFile* file,
                                                             const QByteArray& dataBytes,
                                                             QDataStream& dataStream,
                                                             const uint64_t annotationID,
                                                             const bool readRelationshipDataAfterAnnotationFlag);
        
        void readRelationshipsFromDataStream(const QByteArray& data,
                                             QDataStream& dataStream,
                                             const AString& relationshipID);
        
        bool decompressData(const QByteArray& compressedDataIn,
                            QByteArray& uncompressedDataOut,
                            const AString encodingName);

        FeatureFile* m_featureFile = NULL;
        
        AString m_infoFileDirectoryName;
        
        AString m_byIdDirectoryName;
        
        Sharding m_byIdSharding;
        
        FeatureItemTypeEnum::Enum m_annotationType = FeatureItemTypeEnum::POINT;
        
        Dimension m_xDimension;
        
        Dimension m_yDimension;
        
        Dimension m_zDimension;
        
        std::vector<Relationship> m_relationships;
        
        std::vector<NeuroglancerProperty> m_properties;
        

        /**
         * Origin (lower bound) of the grid
         */
        std::vector<float> m_upperBound;
        
        /**
         * Upper bound of grid
         */
        std::vector<float> m_lowerBound;
        
        bool m_debugFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __NEUROGLANCER_ANNOTATION_FILE_IMPORTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __NEUROGLANCER_ANNOTATION_FILE_IMPORTER_DECLARE__

} // namespace
#endif  //__NEUROGLANCER_ANNOTATION_FILE_IMPORTER_H__

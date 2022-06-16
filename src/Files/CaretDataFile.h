#ifndef __CARET_DATA_FILE__H_
#define __CARET_DATA_FILE__H_

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include "DataFile.h"
#include "DataFileTypeEnum.h"
#include "SceneableInterface.h"
#include "StructureEnum.h"

namespace caret {

    class CaretMappableDataFile;
    class CziImageFile;
    class FileIdentificationAttributes;
    class ImageFile;
    class MediaFile;
    class GiftiMetaData;
    
    class CaretDataFile : public DataFile, public SceneableInterface {
        
    public:
        CaretDataFile(const DataFileTypeEnum::Enum dataFileType);
        
        virtual ~CaretDataFile();
        
        virtual bool isSingleStructure() const;
        
        /**
         * @return The structure for this file.
         */
        virtual StructureEnum::Enum getStructure() const = 0;
        
        /**
         * Set the structure for this file.
         * @param structure
         *   New structure for this file.
         */
        virtual void setStructure(const StructureEnum::Enum structure) = 0;
        
        DataFileTypeEnum::Enum getDataFileType() const;
        
        /**
         * @return Get access to the file's metadata.
         */
        virtual GiftiMetaData* getFileMetaData() = 0;
        
        /**
         * @return Get access to unmodifiable file's metadata.
         */
        virtual const GiftiMetaData* getFileMetaData() const = 0;
        
        virtual bool supportsFileMetaData() const;
        
        virtual AString getFileNameNoExtension() const;
        
        virtual AString getFileNameNoPathNoExtension() const;
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        virtual bool supportsWriting() const;
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                              const SceneClass* sceneClass);
        
        static void setFileReadingUsernameAndPassword(const AString& username,
                                                      const AString& password);
        
        static AString getFileReadingUsername();
        
        static AString getFileReadingPassword();
        
        virtual CziImageFile* castToCziImageFile();
        virtual const CziImageFile* castToCziImageFile() const;
        
        virtual ImageFile* castToImageFile();
        virtual const ImageFile* castToImageFile() const;
        
        virtual MediaFile* castToMediaFile();
        virtual const MediaFile* castToMediaFile() const;
        
        virtual CaretMappableDataFile* castToCaretMappableDataFile();
        virtual const CaretMappableDataFile* castToCaretMappableDataFile() const;

        FileIdentificationAttributes* getFileIdentificationAttributes();
        
        const FileIdentificationAttributes* getFileIdentificationAttributes() const;
        
    protected:
        CaretDataFile(const CaretDataFile& cdf);

        CaretDataFile& operator=(const CaretDataFile& cdf);
        
        void setDataFileType(const DataFileTypeEnum::Enum dataFileType);
        
        virtual void saveFileDataToScene(const SceneAttributes* sceneAttributes,
                                         SceneClass* sceneClass);
        
        virtual void restoreFileDataFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass);
        
    private:
        void copyDataCaretDataFile(const CaretDataFile& cdf);
        
        DataFileTypeEnum::Enum m_dataFileType;
        
        std::unique_ptr<FileIdentificationAttributes> m_fileIdentificationAttributes;
        
        /** A counter that is used when creating default file names */
        static int64_t s_defaultFileNameCounter;
        
        static AString s_fileReadingUsername;
        static AString s_fileReadingPassword;
    };
    
#ifdef __CARET_DATA_FILE_DECLARE__
    int64_t CaretDataFile::s_defaultFileNameCounter = 1;
    AString CaretDataFile::s_fileReadingUsername = "";
    AString CaretDataFile::s_fileReadingPassword = "";
#endif // __CARET_DATA_FILE_DECLARE__

} // namespace
#endif  //__CARET_DATA_FILE__H_

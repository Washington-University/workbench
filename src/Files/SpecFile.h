#ifndef __SPEC_FILE_H__
#define __SPEC_FILE_H__

/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 


#include <AString.h>

#include "DataFile.h"
#include "DataFileTypeEnum.h"
#include "StructureEnum.h"

namespace caret {
    
    class GiftiMetaData;
    class SpecFileDataFile;
    class SpecFileDataFileTypeGroup;
    
    class SpecFile : public DataFile {
        
    public:
        SpecFile();
        
        virtual ~SpecFile();
        
        SpecFile(const SpecFile&);
        
        SpecFile& operator=(const SpecFile&);
                
    public:
        virtual void clear();
        
        virtual bool isEmpty() const;
        
        void addDataFile(const DataFileTypeEnum::Enum dataFileType,
                         const StructureEnum::Enum structure,
                         const AString& filename) throw (DataFileException);
        
        void addDataFile(const AString& dataFileTypeName,
                         const AString& structureName,
                         const AString& filename) throw (DataFileException);
        
        int32_t getNumberOfFiles() const;
        
        int32_t getNumberOfFilesSelected() const;
        
        virtual void readFile(const AString& filename) throw (DataFileException);
        
        virtual void writeFile(const AString& filename) throw (DataFileException);
        
        virtual AString toString() const;
        
        AString getText() const;
        
        int32_t getNumberOfDataFileTypeGroups() const;
        
        SpecFileDataFileTypeGroup* getDataFileTypeGroup(const int32_t dataFileTypeGroupIndex);
        
        SpecFileDataFileTypeGroup* getDataFileTypeGroup(const DataFileTypeEnum::Enum dataFileType);
        
        void getAllConnectivityFileTypes(std::vector<SpecFileDataFile*>& connectivityDataFiles);
        
        void setAllFilesSelected(bool selectionStatus);
        
        GiftiMetaData* getMetaData() { return this->metadata; }
        
        static float getFileVersion();
        
        static AString getFileVersionAsString();

        bool hasBeenEdited() const;
        
        /** XML Tag for SpecFile element */
        static const AString XML_TAG_SPEC_FILE;
        
        /** XML Tag for DataFile element */
        static const AString XML_TAG_DATA_FILE;
        
        /** XML Tag for Structure attribute */
        static const AString XML_ATTRIBUTE_STRUCTURE;
        
        /** XML Tag for DataFileType attribute */
        static const AString XML_ATTRIBUTE_DATA_FILE_TYPE;
        
        /** XML Tag for Version attribute */
        static const AString XML_ATTRIBUTE_VERSION;
        
        /** Version of this SpecFile */
        static const float specFileVersion;
        
    private:
        void copyHelperSpecFile(const SpecFile& sf);
        
        void initializeSpecFile();
        
        void clearData();
        
        void removeFilesTaggedForRemoval();
        
        std::vector<SpecFileDataFileTypeGroup*> dataFileTypeGroups;
        
        GiftiMetaData* metadata;
    };
    
#ifdef __SPEC_FILE_DEFINE__
    const AString SpecFile::XML_TAG_SPEC_FILE            = "CaretSpecFile";
    const AString SpecFile::XML_TAG_DATA_FILE            = "DataFile";
    const AString SpecFile::XML_ATTRIBUTE_STRUCTURE      = "Structure";
    const AString SpecFile::XML_ATTRIBUTE_DATA_FILE_TYPE = "DataFileType";
    const AString SpecFile::XML_ATTRIBUTE_VERSION        = "Version";
    
    const float SpecFile::specFileVersion = 1.0;
#endif // __SPEC_FILE_DEFINE__
} // namespace


#endif // __SPEC_FILE_H__

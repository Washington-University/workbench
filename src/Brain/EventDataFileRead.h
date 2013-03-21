#ifndef __EVENT_DATA_FILE_READ_H__
#define __EVENT_DATA_FILE_READ_H__

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

#include "DataFileTypeEnum.h"
#include "Event.h"
#include "StructureEnum.h"

namespace caret {

    class Brain;
    class CaretDataFile;
    
    /**
     * Event for reading one or more data files.
     */
    class EventDataFileRead : public Event {
        
    public:
        EventDataFileRead(Brain* loadIntoBrain);
        
        virtual ~EventDataFileRead();
        
        void addDataFile(const DataFileTypeEnum::Enum dataFileType,
                         const AString& dataFileName);
        
        void addDataFile(const StructureEnum::Enum structure,
                         const DataFileTypeEnum::Enum dataFileType,
                         const AString& dataFileName);
        
        int32_t getNumberOfDataFilesToRead() const;
        
        AString getDataFileName(const int32_t dataFileIndex) const;
        
        DataFileTypeEnum::Enum getDataFileType(const int32_t dataFileIndex) const;
        
        Brain* getLoadIntoBrain();
        
        StructureEnum::Enum getStructure(const int32_t dataFileIndex) const;
        
        AString getFileErrorMessage(const int32_t dataFileIndex) const;
        
        void setFileErrorMessage(const int32_t dataFileIndex,
                                 const AString& errorMessage);
        
        bool isFileError(const int32_t dataFileIndex) const;
        
        bool isFileErrorInvalidStructure(const int32_t dataFileIndex) const;
        
        void setFileErrorInvalidStructure(const int32_t dataFileIndex,
                                      const bool status);
        
        AString getUsername() const;
        
        AString getPassword() const;
        
        void setUsernameAndPassword(const AString& username,
                                    const AString& password);
        
        bool isFileToBeMarkedModified(const int32_t dataFileIndex) const;
        
        CaretDataFile* getDataFileRead(const int32_t dataFileIndex);
        
        void setDataFileRead(const int32_t dataFileIndex,
                         CaretDataFile* caretDataFile);
        
    private:
        class FileData {
        public:
            FileData(const StructureEnum::Enum structure,
                     const DataFileTypeEnum::Enum dataFileType,
                     const AString& dataFileName,
                     const bool markFileAsModified)
            : m_structure(structure),
            m_dataFileType(dataFileType),
            m_dataFileName(dataFileName),
            m_markFileAsModified(markFileAsModified) {
                m_invalidStructureError = false;
                m_caretDataFileThatWasRead = NULL;
            }
            
            ~FileData() { }
            
            StructureEnum::Enum m_structure;
            DataFileTypeEnum::Enum m_dataFileType;
            AString m_dataFileName;
            AString m_errorMessage;
            CaretDataFile* m_caretDataFileThatWasRead;
            bool m_markFileAsModified;
            bool m_invalidStructureError;
        };
        
        std::vector<FileData> m_dataFiles;
        
        EventDataFileRead(const EventDataFileRead&);
        
        EventDataFileRead& operator=(const EventDataFileRead&);
        
        Brain* loadIntoBrain;
        
        AString username;
        
        AString password;
    };

} // namespace

#endif // __EVENT_DATA_FILE_READ_H__

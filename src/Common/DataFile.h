#ifndef __DATAFILE_H__
#define __DATAFILE_H__

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

#include <QDateTime>

#include <AString.h>

#include "CaretObject.h"
#include "DataFileInterface.h"

namespace caret {

    class DataFileContentInformation;
    
    /**
     * Abstract Data File.
     */
    class DataFile : public CaretObject, public DataFileInterface {
        
    protected:
        DataFile();
        
        virtual ~DataFile();

        DataFile(const DataFile& s);

        DataFile& operator=(const DataFile&);
        
        void setFileNameProtected(const AString& filename);
        
    public:
        virtual AString getFileName() const;
        
        virtual AString getFilePath() const;
        
        virtual AString getFileNameNoPath() const;
        
        virtual void setFileName(const AString& filename);
        
        virtual void addToDataFileContentInformation(DataFileContentInformation& dataFileInformation);
        
        virtual void setPreferOnDiskReading(const bool&) { }
        
        /**
         * Read the data file.
         *
         * @param filename
         *    Name of the data file.
         * @throws DataFileException
         *    If the file was not successfully read.
         */
        virtual void readFile(const AString& filename) = 0;
        
        /**
         * Write the data file.
         *
         * @param filename
         *    Name of the data file.
         * @throws DataFileException
         *    If the file was not successfully written.
         */
        virtual void writeFile(const AString& filename) = 0;
        
        virtual void setModified();

        virtual void clearModified();

        virtual bool isModified() const;

        virtual void clear();
        
        static bool isFileOnNetwork(const AString& filename);
        
        void checkFileReadability(const AString& filename);
        
        void checkFileWritability(const AString& filename);
        
        bool exists() const;
        
        void addFileReadWarning(const AString& warning);
        
        AString getFileReadWarnings() const;
        
        bool isModifiedSinceTimeOfLastReadOrWrite() const;
        
        virtual std::vector<AString> getChildDataFilePathNames() const;

    private:
        void copyHelperDataFile(const DataFile& df);
        
        void initializeMembersDataFile();
        
        void setTimeOfLastReadOrWrite();
        
        QDateTime getLastModifiedTime() const;
        
        /** name of data file */
        AString m_filename;
        
        /** warnings set when file was read */
        mutable AString m_fileReadWarnings;
        
        /** modification status */
        bool m_modifiedFlag;
        
        QDateTime m_timeOfLastReadOrWrite;
    };
    
} // namespace

#endif // __DATAFILE_H__

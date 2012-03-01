#ifndef __FILE_INFORMATION__H_
#define __FILE_INFORMATION__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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


#include "CaretObject.h"

#include <QFileInfo>

namespace caret {

    
    class FileInformation : public CaretObject {
        
    public:
        FileInformation(const AString& file);
        
        FileInformation(const AString& path,
                        const AString& file);
        
        virtual ~FileInformation();
        
        /**
         * @return true if it exists, else false.
         */
        bool exists() const { return m_fileInfo.exists(); }
        
        /**
         * @return true if it is file, else false.
         */
        bool isFile() const { return m_fileInfo.isFile(); }
        
        /**
         * @return true if it is directory, else false.
         */
        bool isDirectory() const { return m_fileInfo.isDir(); }
        
        /**
         * @return true if it is symbolic link, else false.
         */
        bool isSymbolicLink() const { return m_fileInfo.isSymLink(); }
        
        /**
         * @return true if it is readable, else false.
         */
        bool isReadable() const { return m_fileInfo.isReadable(); }
        
        /**
         * @return true if it is writable, else false.
         */
        bool isWritable() const { return m_fileInfo.isWritable(); }
        
        /**
         * @return true if it is absolute path, else false.
         */
        bool isAbsolute() const { return m_fileInfo.isAbsolute(); }
        
        /**
         * @return true if it is relative path, else false.
         */
        bool isRelative() const { return m_fileInfo.isRelative(); }
        
        /**
         * @return true if it is hidden, else false.
         */
        bool isHidden() const { return m_fileInfo.isHidden(); }
        
        /**
         * @return Size of the file in bytes.
         */
        int64_t size() const { return m_fileInfo.size(); }
        
        /**
         * @return Name of the file excluding any path.
         */
        AString getFileName() const { return m_fileInfo.fileName(); }
        
        /**
         * @return The file's path excluding the file's name.
         */
        AString getPathName() const { return m_fileInfo.path(); }
        
        /**
         * @return The full path to the file. 
         */
        AString getFilePath() const { return m_fileInfo.absoluteFilePath(); }
        
        /**
         * @return The file name's extension.
         */
        AString getFileExtension() const { return m_fileInfo.suffix(); }
        
        bool remove();
        
    private:
        FileInformation(const FileInformation&);

        FileInformation& operator=(const FileInformation&);
        
    public:
        virtual AString toString() const;
        
    private:
        QFileInfo m_fileInfo;
    };
    
#ifdef __FILE_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FILE_INFORMATION_DECLARE__

} // namespace
#endif  //__FILE_INFORMATION__H_

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

namespace caret {

    
    /// Obtains information about a path (file, directory, etc).
    class FileInformation : public CaretObject {
        
    public:
        FileInformation(const AString& pathname);
        
        virtual ~FileInformation();
        
        /**
         * Does the path exist?
         * @return true if it exists, else false.
         */
        bool exists() const { return this->pathExists; }
        
        /**
         * Is the path a file?
         * @return true if it is file, else false.
         */
        bool isFile() const { return this->pathIsFile; }
        
        /**
         * Is the path a directory?
         * @return true if it is directory, else false.
         */
        bool isDirectory() const { return this->pathIsDirectory; }
        
        /**
         * Is the path a symbolic link?
         * @return true if it is symbolic link, else false.
         */
        bool isSymbolicLink() const { return this->pathIsSymbolicLink; }
        
        /**
         * Get the size of the file.
         * @return size of the file.
         */
        int64_t size() const { return this->fileSize; }
        
    private:
        FileInformation(const FileInformation&);

        FileInformation& operator=(const FileInformation&);
        
    public:
        virtual AString toString() const;
        
    private:
        AString pathname;
        
        bool pathExists;
        
        int64_t fileSize;
        
        bool pathIsDirectory;
        
        bool pathIsFile;
        
        bool pathIsSymbolicLink;
    };
    
#ifdef __FILE_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FILE_INFORMATION_DECLARE__

} // namespace
#endif  //__FILE_INFORMATION__H_

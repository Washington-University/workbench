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
#include <QUrl>

namespace caret {

    
    class FileInformation : public CaretObject {
        
    public:
        FileInformation(const AString& file);
        
        FileInformation(const AString& path,
                        const AString& file);
        
        virtual ~FileInformation();
        
        bool isLocalFile() const;
        
        bool isRemoteFile() const;
        
        bool exists() const;
        
        bool isFile() const;
        
        bool isDirectory() const;
        
        bool isSymbolicLink() const;
        
        bool isReadable() const;
        
        bool isWritable() const;
        
        bool isAbsolute() const;
        
        bool isRelative() const;
        
        bool isHidden() const;
        
        int64_t size() const;
        
        AString getFileName() const;
        
        AString getPathName() const;
        
        AString getFilePath() const;
        
        AString getCanonicalFilePath() const;
        
        AString getCanonicalPath() const;
        
        AString getFileExtension() const;
        
        AString getAbsolutePath() const;
        
        bool remove();
        
    private:
        FileInformation(const FileInformation&);

        FileInformation& operator=(const FileInformation&);
        
    public:
        virtual AString toString() const;
        
    private:
        QFileInfo m_fileInfo;
        
        QUrl m_urlInfo;
        
        bool m_isRemoteFile;
        
        bool m_isLocalFile;
    };
    
#ifdef __FILE_INFORMATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __FILE_INFORMATION_DECLARE__

} // namespace
#endif  //__FILE_INFORMATION__H_

#ifndef __FILE_INFORMATION__H_
#define __FILE_INFORMATION__H_

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


#include "CaretObject.h"
#include "DataFileTypeEnum.h"

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
        
        AString getAsLocalAbsoluteFilePath(const AString& currentDirectory,
                                           const DataFileTypeEnum::Enum dataFileType) const;
        
        AString getFileNameFollowedByPathNameForGUI() const;
        
        AString getFileName() const;
        
        AString getFileNameNoExtension() const;
        
        AString getPathName() const;
        
        AString getAbsoluteFilePath() const;
        
        AString getCanonicalFilePath() const;
        
        AString getCanonicalPath() const;
        
        AString getFileExtension() const;
        
        AString getAbsolutePath() const;
        
        AString getLastDirectory() const;
        
        QDateTime getLastModified() const;
        
        void getFileComponents(AString& absolutePathOut,
                               AString& fileNameWithoutExtensionOut,
                               AString& extensionWithoutDotOut) const;
        
        static AString assembleFileComponents(const AString& pathName,
                                              const AString& fileNameWithoutExtension,
                                              const AString& extensionWithoutDot);
        
        static AString assembleFileComponents(const AString& pathName,
                                              const AString& fileNameWithExtension);
        
        static AString createTemporaryFilePathName(const AString& fileNameWithExtension);
        
        static AString replaceExtension(const AString& fileName,
                                        const AString& newExtension);
        
        bool remove();
        
        void getRemoteUrlUsernameAndPassword(AString& urlOut,
                                             AString& usernameOut,
                                             AString& passwordOut) const;
        
        static AString fileSizeToStandardUnits(const int64_t numberOfBytes);
        
        static AString cleanPath(const AString& path);
        
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

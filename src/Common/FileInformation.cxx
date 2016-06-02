
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

#include <QDir>

#define __FILE_INFORMATION_DECLARE__
#include "FileInformation.h"
#undef __FILE_INFORMATION_DECLARE__

#include "CaretLogger.h"
#include "DataFile.h"
#include "DataFileTypeEnum.h"

using namespace caret;

/**
 * \class caret::FileInformation
 * \brief Information about a file path.
 * \ingroup Common
 *
 * Provides information about a path (file, directory, etc).  Support for
 * remote files is provided.  Some of the methods are not appropriate for
 * remote files and the method's documentation indicates any limitations.
 */


/**
 * Constructor.
 * @param file
 *    Name of path for which information is obtained.
 */
FileInformation::FileInformation(const AString& file)
: CaretObject()
{
    m_isLocalFile  = false;
    m_isRemoteFile = false;
    
    if (DataFile::isFileOnNetwork(file)) {
        m_urlInfo.setUrl(file);
        m_isRemoteFile = true;
    }
    else {
        m_fileInfo.setFile(file);
        m_isLocalFile = true;
    }
}

/**
 * Constructor.
 * @param path
 *    Directory containing the file.
 * @param file
 *    Name of path for which information is obtained.
 */
FileInformation::FileInformation(const AString& path,
                                 const AString& file) : CaretObject()
{
    m_isLocalFile  = false;
    m_isRemoteFile = false;
    
    if (DataFile::isFileOnNetwork(file)
        || DataFile::isFileOnNetwork(path)) {
        AString pathCopy = path;
        if (pathCopy.endsWith("/") == false) {
            /*
             * With adding a trailing slash:
             *   path: http://brainvis.wustl.edu/john/workbench
             *   file: ParcellationPilot_AverageT1w.nii.gz
             *   CORRECT: http://brainvis.wustl.edu/john/workbench/ParcellationPilot_AverageT1w.nii.gz
             *
             * Without adding the trailing slash "workbench" is chopped off
             *   INCORRECT: http://brainvis.wustl.edu/john/ParcellationPilot_AverageT1w.nii.gz
             */
            pathCopy += "/";
        }
        QUrl baseUrl(pathCopy);
        QUrl relativeUrl(file);
        m_urlInfo = baseUrl.resolved(relativeUrl);
        m_isRemoteFile = true;
        
        CaretLogFine("Path: "
                       + path
                       + "\n   File: "
                       + file
                       + "\n   Becomes     "
                       + m_urlInfo.toString());
    }
    else {
        m_fileInfo.setFile(path,
                           file);
        
        /*
         * Clean up path to remove any ".." (up a directory level).
         * Note that canonicalFilePath() will return an empty string
         * if the path does not point to a valid file.
         * 
         * TSC: NEVER USE CANONICAL FOR THIS!  canonical resolves symlinks, which is NOT DESIRED unless explicitly asked for (by calling a function including the word "canonical").
         * 
         */
        if (getAbsoluteFilePath().contains("..")) {
            AString cleanedPath = QDir::cleanPath(m_fileInfo.absolutePath());
            if (!cleanedPath.endsWith('/')) cleanedPath += "/";//because "/" and "/usr" are both possible results
            cleanedPath += m_fileInfo.fileName();
            if (cleanedPath.isEmpty() == false) {
                m_fileInfo.setFile(cleanedPath);
            }
        }
        
        m_isLocalFile = true;
    }
}

/**
 * Destructor.
 */
FileInformation::~FileInformation()
{
    
}

/**
 * @return True if the file is a local file, else false.
 */
bool
FileInformation::isLocalFile() const
{
    return m_isLocalFile;
}

/**
 * @return True if the file is a remote file, else false.
 */
bool
FileInformation::isRemoteFile() const
{
    return m_isRemoteFile;
}

/**
 * @return Absolute path including the name of the file.
 *
 * some logic that seems to be missing from QFileInfo: if absolute, 
 * return path() + file() rather than using system call.
 *
 * Note: A remote file returns the original, full URL.
 */
AString FileInformation::getAbsoluteFilePath() const
{
    if (m_isRemoteFile) {
        return m_urlInfo.toString();
    }
    
    if (m_fileInfo.isAbsolute()) {
        return m_fileInfo.filePath();
    }
    else {
        return m_fileInfo.absoluteFilePath();
    }
}

/**
 * Removes the file.
 * Remove files cannot be removed.
 *
 * @return
 *    true if file deleted successfully.
 */
bool 
FileInformation::remove()
{
    if (m_isRemoteFile) {
        CaretLogSevere("Deleting remote file is not allowed: "
                       + m_urlInfo.toString());
        return false;
    }
    
    bool result = false;
    if (m_fileInfo.exists()) {
        result = QFile::remove(m_fileInfo.absoluteFilePath());
    }
    return result;
}

/**
 * @return true if it exists, else false.
 *
 * A remote file always returns true.
 */
bool
FileInformation::exists() const
{
    if (m_isRemoteFile) {
        return true;
    }
    
    return m_fileInfo.exists();
}

/**
 * @return true if it is file, else false.
 *
 * A remote file always returns true.
 */
bool
FileInformation::isFile() const
{
    if (m_isRemoteFile) {
        return true;
    }
    
    return m_fileInfo.isFile();
}

/**
 * @return true if it is directory, else false.
 *
 * A remote file always returns false.
 */
bool
FileInformation::isDirectory() const
{
    if (m_isRemoteFile) {
        return false;
    }
    
    return m_fileInfo.isDir();
}

/**
 * @return true if it is symbolic link, else false.0
 *
 * A remote file always returns false.
 */
bool
FileInformation::isSymbolicLink() const
{
    if (m_isRemoteFile) {
        return false;
    }
    
    return m_fileInfo.isSymLink();
}

/**
 * @return true if it is readable, else false.
 *
 * A remote file always returns true.
 */
bool
FileInformation::isReadable() const
{
    if (m_isRemoteFile) {
        return true;
    }
    
    return m_fileInfo.isReadable();
}

/**
 * @return true if it is writable, else false.
 *
 * A remote file always returns false.
 */
bool
FileInformation::isWritable() const
{
    if (m_isRemoteFile) {
        return false;
    }
    
    return m_fileInfo.isWritable();
}

/**
 * @return true if it is absolute path, else false.
 *
 * A remote file always returns true.
 */
bool
FileInformation::isAbsolute() const
{
    if (m_isRemoteFile) {
        return true;
    }
    
    return m_fileInfo.isAbsolute();
}

/**
 * @return true if it is relative path, else false (remote file is never relative)
 *
 * A remote file always returns false.
 */
bool
FileInformation::isRelative() const
{
    if (m_isRemoteFile) {
        return false;
    }
    
    return m_fileInfo.isRelative();
}

/**
 * @return true if it is hidden, else false.
 *
 * A remote file always returns false.
 */
bool
FileInformation::isHidden() const
{
    if (m_isRemoteFile) {
        return false;
    }
    
    return m_fileInfo.isHidden();
}

/**
 * @return Size of the file in bytes.
 *
 * A remote file always returns 0.
 */
int64_t
FileInformation::size() const
{
    if (m_isRemoteFile) {
        return 0;
    }
    
    return m_fileInfo.size();
}

/**
 * @return name of file followed by path in parenthesis.
 *
 * For example:  /usr/local/file.txt
 *     returns:  file.txt (/usr/local)
 */
AString
FileInformation::getFileNameFollowedByPathNameForGUI() const
{
    AString name = getFileName();
    const AString pn = getPathName();
    if ( ! pn.isEmpty()) {
        name += (" (" + pn + ")");
    }
    
    return name;
}


/**
 * @return Name of the file excluding any path.
 *
 * A remote file always anything after the last slash (/).  If there is
 * no slash, an emtpy string is returned.
 */
AString
FileInformation::getFileName() const
{
    if (m_isRemoteFile) {
        QString name = m_urlInfo.toString();
        const int indx = name.lastIndexOf('/');
        if ((indx >= 0) && (indx < name.length())) {
            name = name.mid(indx + 1);
            return name;
        }
        else {
            return "";
        }
    }
    
    return m_fileInfo.fileName();
}

/**
 * @return Name of the file excluding any path and WITHOUT any extension.
 *
 * A remote file always anything after the last slash (/).  If there is
 * no slash, an emtpy string is returned.
 */
AString
FileInformation::getFileNameNoExtension() const
{
    AString name = getFileName();
    const AString ext = getFileExtension();
    if ( ! ext.isEmpty()) {
        const int32_t extStartIndex = name.indexOf(ext);
        if (extStartIndex > 0) {
            name = name.left(extStartIndex - 1);
        }
    }
    
    return name;
}

/**
 * @return The file's path excluding the file's name.
 *
 * A remote file always everything before the last slash (/).  If there is
 * no slash, the URL is returned.
 */
AString
FileInformation::getPathName() const
{
    if (m_isRemoteFile) {
        QString path = m_urlInfo.toString();
        const int indx = path.lastIndexOf('/');
        if (indx >= 1) {
            path = path.left(indx);
            return path;
        }
        else {
            return path;
        }
    }
    
    return m_fileInfo.path();
}

/**
 * @return The full path to the file including the file name, resolving 
 * any symlinks or ".." or "." components.  This should give exactly one 
 * string per file, no matter how many ways to get to a file there are 
 * (except for hardlinks).
 *
 * Note: A remote file returns the original, full URL.
 */
AString
FileInformation::getCanonicalFilePath() const
{
    if (m_isRemoteFile) {
        return m_urlInfo.toString();
    }
    
    return m_fileInfo.canonicalFilePath();
}

/**
 * @return The full path to the file (excluding the file name), 
 * resolving any symlinks or ".." or "." components.
 *
 * For a remote file, this returns getPathName().
 */
AString
FileInformation::getCanonicalPath() const
{
    if (m_isRemoteFile) {
        return getPathName();
    }
    
    return m_fileInfo.canonicalPath();
}

/**
 * @return The file name's extension.
 *
 * Many Workbench files have filename extensions that include a 
 * dot (nii.gz, pconn.nii, etc) AND users sometimes include dots
 * in the names of the files (Glasser_PilotIII.L.20k_fs_LR.shape.gii)
 * so FileInfo::suffix and FileInfo::completeSuffix methods will
 * not provide the correct file extension.
 *
 * This method will compare the end of the file's name to every Workbench
 * file extension.  If there is a match, this extension is returned.
 * Otherwise FileInfo::suffix is called and that will return anything
 * after but not including the last dot.
 */
AString
FileInformation::getFileExtension() const
{
    const std::vector<AString> workbenchExtensions = DataFileTypeEnum::getFilesExtensionsForEveryFile();

    for (std::vector<AString>::const_iterator extIter = workbenchExtensions.begin();
         extIter != workbenchExtensions.end();
         extIter++) {
        const AString extension = *extIter;
        if ( ! extension.isEmpty()) {
            if (getFileName().endsWith(extension)) {
                return extension;
            }
        }
    }
    
//    if (m_isRemoteFile) {
//        AString ext = getFileName();
//        const int indx = ext.lastIndexOf('.');
//        if ((indx >= 0) && (indx < ext.length())) {
//            ext = ext.mid(indx + 1);
//            return ext;
//        }
//        else {
//            return "";
//        }
//    }
    
    return m_fileInfo.suffix();
}

/**
 * Get the components for a filename.
 *
 * Example: /Volumes/myelin1/caret7_gui_design/data/HCP_demo/areas.border
 * Returns
 *   absolutePathOut => /Volumes/myelin1/caret7_gui_design/data/HCP_demo
 *   fileNameWithoutExtensionOut => areas
 *   extensionWithoutDotOut => border
 *
 * @param absolutePathOut
 *    Absolute path of file file.  Could be empty if this instance
 *    was created using a filename without a path.
 * @param fileNameWithoutExtensionOut
 *    Name of the file without path and without extention.
 * @param extensionWithoutDotOut
 *    Extension without the dot.  Could be empty if filename does 
 *    not have an extension.
 */
void
FileInformation::getFileComponents(AString& absolutePathOut,
                                   AString& fileNameWithoutExtensionOut,
                                   AString& extensionWithoutDotOut) const
{
    absolutePathOut             = getAbsolutePath();
    fileNameWithoutExtensionOut = getFileNameNoExtension();
    extensionWithoutDotOut      = getFileExtension();
}

/**
 * Assemble the file components into a file path and name.
 *
 * @param pathName
 *     Path for file (may be absolute, relative, or empty).
 * @param fileNameWithoutExtension
 *     Name of file without extension.
 * @param extensionWithoutDot
 *     The file extension without the leading dot.
 */
AString
FileInformation::assembleFileComponents(const AString& pathName,
                                        const AString& fileNameWithoutExtension,
                                        const AString& extensionWithoutDot)
{
    AString name;
    if ( ! pathName.isEmpty()) {
        name += (pathName + "/");
    }
    name += fileNameWithoutExtension;
    if ( ! extensionWithoutDot.isEmpty()) {
        name += ("." + extensionWithoutDot);
    }

    return name;
}

/**
 * Convert, if needed, the file information to a local, absolute path.
 *
 * If the file is a remote file, the file name is added to the given current directory.
 *
 * If the file is local but a relative path, the filename is added to the given
 * current directory.
 * 
 * If the file is local but an absolute path, the result of getAbsoluteFilePath()
 * is returned.
 *
 * @param currentDirectory
 *     The current directory used by remote and relative file paths.
 * @param dataFileType
 *     The type of data file.  If the type is not UNKNOWN, and the 
 *     the file name does not end in the proper extension, the extension
 *     is added to the file.
 @ @return
 *     The local absolute file path.
 */
AString
FileInformation::getAsLocalAbsoluteFilePath(const AString& currentDirectory,
                                            const DataFileTypeEnum::Enum dataFileType) const
{
    AString thePath, theName, theExtension;
    getFileComponents(thePath,
                      theName,
                      theExtension);
    
    if (m_isLocalFile) {
        if (m_fileInfo.isRelative()) {
            thePath = currentDirectory;
        }
    }
    else if (m_isRemoteFile) {
        thePath = currentDirectory;
        theName = theName.replace("?", "_");
        theName = theName.replace(":", "_");
        theName = theName.replace("=", "_");
        theName = theName.replace("@", "_");
    }
    
    if (dataFileType != DataFileTypeEnum::UNKNOWN) {
        AString validExtension = "";
        const std::vector<AString> validExtensions = DataFileTypeEnum::getAllFileExtensions(dataFileType);
        for (std::vector<AString>::const_iterator iter = validExtensions.begin();
             iter != validExtensions.end();
             iter++) {
            const AString ext = *iter;
            if (theExtension == ext) {
                validExtension = ext;
                break;
            }
        }

        if (validExtension.isEmpty()) {
            validExtension = DataFileTypeEnum::toFileExtension(dataFileType);
        }
        
        theExtension = validExtension;
    }
    
    const AString nameOut = assembleFileComponents(thePath,
                                                   theName,
                                                   theExtension);
    return nameOut;
}


/**
 * @return The file's absolute path.  It DOES NOT include the file's name.
 * Note: A remote file returns everything up to the last slash.
 */
AString
FileInformation::getAbsolutePath() const
{
    if (m_isRemoteFile) {
        const AString fullName = m_urlInfo.toString();
        const int lastSlashIndex = fullName.lastIndexOf("/");
        if (lastSlashIndex > 0) {
            const AString thePath = fullName.left(lastSlashIndex);
            return thePath;
        }
        return fullName;
    }
    
    return m_fileInfo.absolutePath();
}

AString FileInformation::getLastDirectory() const
{
    QStringList myList = getPathName().split('/', QString::SkipEmptyParts);//QT always uses /, even on windows
    return myList[myList.size() - 1];
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString
FileInformation::toString() const
{
    if (m_isRemoteFile) {
        return ("FileInformation for " + m_urlInfo.toString());
    }
    
    return ("FileInformation for " + m_fileInfo.absoluteFilePath());
}

/**
 * For a remote file, strip the username and password from the URL
 * and return the URL (without username and password), the username, 
 * and the password.  If the file is local or does not contain
 * username/password, the equivalent of getFilePath() is the output URL.
 *
 * @param urlOut
 *     The URL with the username and password removed.
 * @param
 *     The username that was in the URL.
 * @param
 *     The password that was in the URL.
 */
void
FileInformation::getRemoteUrlUsernameAndPassword(AString& urlOut,
                                                 AString& usernameOut,
                                                 AString& passwordOut) const
{
    urlOut      = "";
    usernameOut = "";
    passwordOut = "";
    
    if (m_isRemoteFile) {
        urlOut = m_urlInfo.toString(QUrl::RemoveUserInfo);
        usernameOut = m_urlInfo.userName();
        passwordOut = m_urlInfo.password();
        return;
    }
    
    urlOut = getAbsoluteFilePath();
}

/**
 * Convert the number of bytes to a string that includes standard units
 * (ie: Bytes, Kilobytes, Megabytes, Gigabytes, etc.)
 *
 * @param numberOfBytes
 *    The number of bytes.
 * @return
 *    String with the size in standard units.
 */
AString
FileInformation::fileSizeToStandardUnits(const int64_t numberOfBytes)
{
    double bytes = numberOfBytes;
    short index = 0;
    static const char *labels[9] = {" Bytes", " Kilobytes", " Megabytes", " Gigabytes", " Terabytes", " Petabytes", " Exabytes", " Zettabytes", " Yottabytes"};
    while (index < 8 && bytes > 1000.0f)
    {
        ++index;
        bytes = bytes / 1000.0f;//using 1024 would make it Kibibytes, etc
    }
    AString sizeString = AString::number(bytes, 'f', 2) + labels[index];//2 digits after decimal point
    return sizeString;
}



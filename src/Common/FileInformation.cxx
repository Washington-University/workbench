
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
         */
        if (getAbsoluteFilePath().contains("..")) {
            const AString cleanedPath = m_fileInfo.canonicalFilePath();
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
 * For a remote file, the extension is anything after the last
 * "." in the file's name.  If there is no "." an empty string
 * is returned.
 */
AString
FileInformation::getFileExtension() const
{
    if (m_isRemoteFile) {
        AString ext = getFileName();
        const int indx = ext.lastIndexOf('.');
        if ((indx >= 0) && (indx < ext.length())) {
            ext = ext.mid(indx + 1);
            return ext;
        }
        else {
            return "";
        }
    }
    
    return m_fileInfo.suffix();
}

/**
 * @return The file's absolute path.
 * Note: A remote file returns the original, full URL.
 */
AString
FileInformation::getAbsolutePath() const
{
    if (m_isRemoteFile) {
        return m_urlInfo.toString();
    }
    
    return m_fileInfo.absolutePath();
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



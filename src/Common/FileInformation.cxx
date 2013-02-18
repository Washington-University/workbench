
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

#include <QDir>
#include <QFileInfo>

#define __FILE_INFORMATION_DECLARE__
#include "FileInformation.h"
#undef __FILE_INFORMATION_DECLARE__

#include "CaretLogger.h"
#include "DataFile.h"

using namespace caret;

/**
 * \class caret::FileInformation
 *
 * \brief Information about a file path.
 *
 * Provides information about a path (file, directory, etc).
 */


/**
 * Constructor.
 * @param file
 *    Name of path for which information is obtained.
 */
FileInformation::FileInformation(const AString& file)
: CaretObject(), m_fileInfo(file)
{
    if (DataFile::isFileOnNetwork(file)) {
        CaretLogSevere("PROGRAM ERROR: file is on network which does not function in FileInformation");
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
                                 const AString& file) : CaretObject(), m_fileInfo(path, file)
{
    if (DataFile::isFileOnNetwork(path)) {
        CaretLogSevere("PROGRAM ERROR: file is on network which does not function in FileInformation");
    }
    
    /*
     * Clean up path to remove any ".." (up a directory level).
     * Note that canonicalFilePath() will return an empty string
     * if the path does not point to a valid file.
     */
    if (getFilePath().contains("..")) {
        const AString cleanedPath = m_fileInfo.canonicalFilePath();
        if (cleanedPath.isEmpty() == false) {
            m_fileInfo.setFile(cleanedPath);
        }
    }
}

/**
 * Destructor.
 */
FileInformation::~FileInformation()
{
    
}

//some logic that seems to be missing from QFileInfo: if absolute, return path() + file() rather than using system call
AString FileInformation::getFilePath() const
{
    if (m_fileInfo.isAbsolute())
    {
        return m_fileInfo.filePath();
    } else {
        return m_fileInfo.absoluteFilePath();
    }
}

/**
 * Removes the file.
 * @return
 *    true if file deleted successfully.
 */
bool 
FileInformation::remove()
{   bool result = false;
    if (m_fileInfo.exists()) {
        result = QFile::remove(m_fileInfo.absoluteFilePath());
    }
    return result;
}


/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
FileInformation::toString() const
{
    return ("FileInformation for " + m_fileInfo.absoluteFilePath());
}


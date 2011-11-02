
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

using namespace caret;

/**
 * \class FileInformation
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
: CaretObject()
{
    QFileInfo fileInfo(file);
    this->initialize(fileInfo);
/*
    this->file = file;
    
    this->fileSize = 0;
    
    this->pathExists = false;
    this->pathIsAbsolute = false;
    this->pathIsDirectory = false;
    this->pathIsFile = false;
    this->pathIsHidden = false;
    this->pathIsReadable = false;
    this->pathIsRelative = false;
    this->pathIsSymbolicLink = false;
    this->pathIsWritable = false;

    this->fileName = "";
    this->pathName = "";
    
    QFileInfo fileInfo(this->file);    
    this->pathExists = fileInfo.exists();
    if (this->pathExists) {
        this->fileSize = fileInfo.size();
        
        this->pathIsAbsolute = fileInfo.isAbsolute();
        this->pathIsDirectory = fileInfo.isDir();
        this->pathIsFile      = fileInfo.isFile();
        this->pathIsHidden    = fileInfo.isHidden();
        this->pathIsReadable = fileInfo.isReadable();
        this->pathIsRelative = fileInfo.isRelative();
        this->pathIsSymbolicLink = fileInfo.isSymLink();
        this->pathIsWritable = fileInfo.isWritable();
        
        this->fileName = fileInfo.fileName();
        this->pathName = fileInfo.path();
    }
 */
}

/**
 * Constructor.
 * @param path
 *    Directory containing the file.
 * @param file
 *    Name of path for which information is obtained.
 */
FileInformation::FileInformation(const AString& path,
                                 const AString& file)
{
    QDir dir(path);
    QFileInfo fileInfo(dir, file);
    
    this->initialize(fileInfo);
}

/**
 * Destructor.
 */
FileInformation::~FileInformation()
{
    
}

void 
FileInformation::initialize(const QFileInfo& fileInfo)
{
    //this->file = file;
    
    this->fileSize = 0;
    
    this->pathExists = false;
    this->pathIsAbsolute = false;
    this->pathIsDirectory = false;
    this->pathIsFile = false;
    this->pathIsHidden = false;
    this->pathIsReadable = false;
    this->pathIsRelative = false;
    this->pathIsSymbolicLink = false;
    this->pathIsWritable = false;
    
    this->fileName = "";
    this->pathName = "";
    this->filePath = "";
    
    this->pathExists = fileInfo.exists();
    if (this->pathExists) {
        this->fileSize = fileInfo.size();
    }

    this->pathIsAbsolute = fileInfo.isAbsolute();
    this->pathIsDirectory = fileInfo.isDir();
    this->pathIsFile      = fileInfo.isFile();
    this->pathIsHidden    = fileInfo.isHidden();
    this->pathIsReadable = fileInfo.isReadable();
    this->pathIsRelative = fileInfo.isRelative();
    this->pathIsSymbolicLink = fileInfo.isSymLink();
    this->pathIsWritable = fileInfo.isWritable();
    
    this->fileName = fileInfo.fileName();
    this->pathName = fileInfo.path();
    
    this->filePath = fileInfo.absoluteFilePath();
}

/**
 * Removes the file.
 * @return
 *    true if file deleted successfully.
 */
bool 
FileInformation::remove()
{   bool result = false;
    if (this->pathExists) {
        result = QFile::remove(this->filePath);
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
    return ("FileInformation for " + this->filePath);
}



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

#include <QFile>
#include <QTextStream>

#define __FILE_ADAPTER_DECLARE__
#include "FileAdapter.h"
#undef __FILE_ADAPTER_DECLARE__

#include "CaretAssert.h"
#include "FileInformation.h"

using namespace caret;


    
/**
 * \class caret::FileAdapter 
 * \brief Simplifies opening of files and streams.
 *
 * When an open method is called, the file is opened
 * in the appropriate mode, a stream is created, and
 * the stream is returned.  When the destructor is
 * called it will take care of cleaning up any resources
 * and closing the file (if it has not already been 
 * done by calling close().  
 * 
 * If an instance of this class is created in a 
 * try/catch block, create an instance of the class
 * statically (not NEW) so that if an exception is
 * thrown, the instance of this class will go out
 * of scope which results in the destructor being 
 * called which cleans up resources and closes the
 * file.
 */

/**
 * Constructor.
 */
FileAdapter::FileAdapter() {
    m_file = NULL;
    m_textStream = NULL;
}

/**
 *
 */
FileAdapter::~FileAdapter() {
    close();
}

/**
 * Open a file with the given name, create a QTextStream for
 * the file, and return the QTextStream.
 *
 * @filename
 *    Name of file.
 * @errorMessagesOut
 *    Contains information about errors if any occur.
 * @return
 *    A pointer to the QTextStream that was created.  DO NOT
 * destroy it or else disaster will likely occur.  If this 
 * returned value is NULL, it indicates that an error has 
 * occurred and its description will be in the errorMessageOut
 * parameter.
 */
QTextStream* 
FileAdapter::openQTextStreamForWritingFile(const AString& filename,
                                           AString& errorMessageOut) {
    errorMessageOut = "";
    
    if (m_file != NULL) {
        errorMessageOut = ("A file named "
                           + m_file->fileName()
                           + " is currently open with this FileAdapter");
        return NULL;
    }
    
    if (m_file != NULL) {
        errorMessageOut = "This file is already open and has not been closed.";
        return NULL;
    }
    
    if (filename.isEmpty()) {
        errorMessageOut = "Filename contains no characters.";
        return NULL;
    }
    
    FileInformation fileInfo(filename);
    if (fileInfo.exists()) {
        if (fileInfo.isWritable() == false) {
            errorMessageOut = (filename
                               + " exists but does not have writable permission.");
            return NULL;
        }
    }
    
    m_file = new QFile(filename);
    if (m_file->open(QFile::WriteOnly) == false) {
        errorMessageOut = ("Unable to open "
                           + filename
                           + " for writing: "
                           + m_file->errorString());
        delete m_file;
        m_file = NULL;
        return NULL;
    }
    
    m_textStream = new QTextStream(m_file);
    
    return m_textStream;
}

/**
 * If any streams are valid, they are deleted.
 * If the file is valid, its is flushed, closed
 * and deleted.
 */
void 
FileAdapter::close() {
    if (m_textStream != NULL) {
        m_textStream->flush();
        delete m_textStream;
        m_textStream = NULL;
    }
    
    if (m_file != NULL) {
        m_file->flush();
        m_file->close();
        delete m_file;
        m_file = NULL;
    }
}


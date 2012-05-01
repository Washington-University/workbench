
/*LICENSE_START*/
/*
 * Copyright 2012 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
                               + " exists and does not have writable permission.");
            return NULL;
        }
    }
    
    m_file = new QFile(filename);
    if (m_file->open(QFile::WriteOnly) == false) {
        errorMessageOut = ("Unable to open "
                           + filename
                           + " for writing.");
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
        m_file = NULL;
    }
}


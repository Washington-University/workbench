/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
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

#include <fstream>

#include "CaretLogger.h"
#include "TextFile.h"

using namespace caret;

/**
 * Constructor.
 */
TextFile::TextFile()
: DataFile()
{
    this->text = ""; 
}

/**
 * Destructor.
 */
TextFile::~TextFile()
{
    this->text = "";
}

/**
 * Clear the contents of this file.
 */
void 
TextFile::clear()
{
    DataFile::clear();
    this->text = ""; 
}

/**
 * Is this file empty?
 *
 * @return true if file is empty, else false.
 */
bool 
TextFile::isEmpty() const
{
    return this->text.isEmpty();
}

/**
 * Read the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error reading the file.
 */
void 
TextFile::readFile(const AString& filename) throw (DataFileException)
{
    CaretLogSevere("WARNING: TextFile::readFile() has not been tested");
    this->clearModified();
    
    char* name = filename.toCharArray();
    std::ifstream inputStream(name, std::ifstream::in);
    delete[] name;
    
    const int BUFFER_LENGTH = 4096;
    char buffer[BUFFER_LENGTH];
    
    bool done = false;
    while (done == false) {
        inputStream.read(buffer, BUFFER_LENGTH);
        int64_t numRead = inputStream.gcount();
        
        this->text.append(AString::fromAscii(buffer, numRead));
        
        if (inputStream.eof()) {
            done = true;
        }
    }
    inputStream.close();
    
    this->setFileName(filename);
    
    this->clearModified();
}

/**
 * Write the file.
 *
 * @param filename
 *    Name of file to read.
 *
 * @throws DataFileException
 *    If there is an error writing the file.
 */
void 
TextFile::writeFile(const AString& filename) throw (DataFileException)
{
    char* name = filename.toCharArray();
    std::ofstream outputStream(name);
    delete[] name;
    if (!outputStream) {
        throw DataFileException("Unable to open " + filename + " for writing.");
    }
    
    const int64_t textLength = this->text.length();
    
    outputStream.write(this->text, textLength);

    outputStream.close();
    
    this->setFileName(filename);
    
    this->clearModified();
}

/**
 * Get information about this file's contents.
 * @return
 *    Information about the file's contents.
 */
AString 
TextFile::toString() const
{
    return "TextFile";
}

/**
 * Get the file's text.
 *
 * @return  The file's text.
 */
AString 
TextFile::getText() const
{
    return this->text;
}

/**
 * Replace the file's text.
 *
 * @param text
 *   Replaces text in the file.
 */
void 
TextFile::replaceText(const AString& text)
{
    if (text != this->text) {
        this->text = text;
        this->setModified();
    }
}

/**
 * Add to the file's text.
 *
 * @param text
 *   Text added.
 */
void 
TextFile::addText(const AString& text)
{
    this->text += text;
    this->setModified();
}

/**
 * Add to the file's text and then add
 * a newline character.
 *
 * @param text
 *   Text added.
 */
void 
TextFile::addLine(const AString& text)
{
    this->text += text;
    this->text += "\n";
    this->setModified();
}




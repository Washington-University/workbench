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
#include <QRegularExpression>
#include <QTextStream>

#include "CaretLogger.h"
#include "DataFileException.h"
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
TextFile::readFile(const AString& filename)
{
    clear();
    checkFileReadability(filename);
    
    QFile file(filename);
    if (file.open(QFile::ReadOnly) == false) {
        throw DataFileException(filename,
                                "Unable to open for reading.");
    }

    QTextStream textStream(&file);
    this->text = textStream.readAll();
    
    file.close();
    
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
TextFile::writeFile(const AString& filename)
{
    checkFileWritability(filename);
    
    QFile file(filename);
    if (file.open(QFile::WriteOnly) == false) {
        throw DataFileException(filename,
                                "Unable to open for writing.");
    }
    
    QTextStream textStream(&file);
    textStream << this->text;
    
    file.close();
    
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
 * @return A vector with each element containing one line of text from the file.  Any newlines are removed
 */
std::vector<AString>
TextFile::getTextLines() const
{
    /*
     * Split lines with return and new line characters
     * Cannot split with whitespace since files names may contain spaces
     */
#if QT_VERSION >= 0x060000
    QStringList linesList(this->text.split(QRegularExpression("[\\n|\\r]"),
                                           Qt::SkipEmptyParts));
#else
    QStringList linesList(this->text.split(QRegularExpression("[\\n|\\r]"),
                                           QString::SkipEmptyParts));
#endif
    std::vector<AString> lines;
    QStringListIterator iter(linesList);
    while (iter.hasNext()) {
        AString s(iter.next().trimmed()); /* could have empty lines */
        if ( ! s.isEmpty()) {
            lines.push_back(s);
        }
    }
    
    return lines;
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




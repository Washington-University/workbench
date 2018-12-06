
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __WU_Q_MACRO_FILE_DECLARE__
#include "WuQMacroFile.h"
#undef __WU_Q_MACRO_FILE_DECLARE__

#include <QFile>
#include <QFileInfo>
#include <QTextStream>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "WuQMacro.h"
#include "WuQMacroGroup.h"
#include "WuQMacroGroupXmlReader.h"
#include "WuQMacroGroupXmlWriter.h"

using namespace caret;


    
/**
 * \class caret::WuQMacroFile 
 * \brief File for storing macros
 * \ingroup Common
 */

/**
 * Constructor.
 */
WuQMacroFile::WuQMacroFile()
: DataFile()
{
    m_macroGroup.reset(new WuQMacroGroup("File"));
}

/**
 * Destructor.
 */
WuQMacroFile::~WuQMacroFile()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
WuQMacroFile::WuQMacroFile(const WuQMacroFile& obj)
: DataFile(obj)
{
    this->copyHelperWuQMacroFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
WuQMacroFile&
WuQMacroFile::operator=(const WuQMacroFile& obj)
{
    if (this != &obj) {
        DataFile::operator=(obj);
        this->copyHelperWuQMacroFile(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
WuQMacroFile::copyHelperWuQMacroFile(const WuQMacroFile& obj)
{
    *m_macroGroup = *obj.m_macroGroup;
    
    setMacroGroupName(getFileName());
}

/**
 * @return True if this file is empty (has no macros)
 */
bool
WuQMacroFile::isEmpty() const
{
    return (m_macroGroup->getNumberOfMacros() == 0);
}

/**
 * @return Macro group in this file
 */
WuQMacroGroup*
WuQMacroFile::getMacroGroup()
{
    return m_macroGroup.get();
}

/**
 * @return Macro group in this file (const method)
 */
const WuQMacroGroup*
WuQMacroFile::getMacroGroup() const
{
    return m_macroGroup.get();
}


/**
 * @return File filter for macro file in a QFileDialog
 */
QString
WuQMacroFile::getFileDialogFilter()
{
    QString f("Macros (*"
              + getFileExtension()
              + ")");
    return f;
}

/**
 * @return Extension for a macro file
 */
QString
WuQMacroFile::getFileExtension()
{
    return ".wb_macro";
}

/**
 * Set the status to unmodified.
 */
void
WuQMacroFile::clearModified()
{
    DataFile::clearModified();
    m_macroGroup->clearModified();
}

/**
 * Is the object modified?
 * @return true if modified, else false.
 */
bool
WuQMacroFile::isModified() const
{
    if (DataFile::isModified()) {
        return true;
    }
    if (m_macroGroup->isModified()) {
        return true;
    }
    return false;
}

/**
 * Clear the contents of the file.
 */
void
WuQMacroFile::clear()
{
    m_macroGroup->clear();
}

/**
 * Append macros in the given group to this file
 *
 * @param macroGroup
 *     The macro group whose macros are appended to this macro group
 */
void
WuQMacroFile::appendMacroGroup(const WuQMacroGroup* macroGroup)
{
    const int32_t numMacros = macroGroup->getNumberOfMacros();
    for (int32_t i = 0; i < numMacros; i++) {
        const WuQMacro* macro = macroGroup->getMacroAtIndex(i);
        CaretAssert(macro);
        addMacro(new WuQMacro(*macro));
    }
    setModified();
}

/**
 * Add a macro to this file.
 * This file will take ownership of the macro.
 *
 * @param macro
 *     Macro to add to file
 */
void
WuQMacroFile::addMacro(WuQMacro* macro)
{
    CaretAssert(macro);
    m_macroGroup->addMacro(macro);
}

/**
 * Set the macro group's name to the name of the file
 * without the extension
 */
void
WuQMacroFile::setMacroGroupName(const QString& filename)
{
    if (filename.isEmpty()) {
        return;
    }
    
    QFileInfo fileInfo(filename);
    QString macroGroupName(fileInfo.fileName());
    const int extIndex = macroGroupName.indexOf(getFileExtension());
    if (extIndex > 0) {
        macroGroupName.resize(extIndex);
    }
    m_macroGroup->setName(macroGroupName);
}


/**
 * Read the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully read.
 */
void
WuQMacroFile::readFile(const AString& filename)
{
    if (filename.isEmpty()) {
        throw DataFileException("Filename is empty");
    }
    
    QFile file(filename);
    if (file.open(QFile::ReadOnly)) {
        setMacroGroupName(filename);
        
        QTextStream textStream(&file);
        const QString fileContentString = textStream.readAll();
        
        WuQMacroGroupXmlReader reader;
        reader.readFromString(fileContentString,
                              m_macroGroup.get());
        file.close();
        if (reader.hasError()) {
            throw DataFileException(reader.getErrorMessage());
        }
        setFileName(filename);
        clearModified();
        
        if (reader.hasWarnings()) {
            CaretLogWarning("When reading "
                            + filename
                            + ": "
                            + reader.getWarningMessage());
        }
    }
    else {
        throw DataFileException("Unable to open file for writing: "
                                + filename);
    }
}

/**
 * Write the data file.
 *
 * @param filename
 *    Name of the data file.
 * @throws DataFileException
 *    If the file was not successfully written.
 */
void
WuQMacroFile::writeFile(const AString& filename)
{
    if (filename.isEmpty()) {
        throw DataFileException("Filename is empty");
    }
    
    QFile file(filename);
    if (file.open(QFile::WriteOnly)) {
        setMacroGroupName(filename);
        
        /*
         * Place the macro group into a string
         */
        QString fileContentString;
        WuQMacroGroupXmlWriter writer;
        writer.writeToString(m_macroGroup.get(),
                             fileContentString);
        
        /*
         * Write string containing macros and close file
         */
        QTextStream textStream(&file);
        textStream << fileContentString;
        file.close();
        
        setFileName(filename);
        clearModified();
    }
    else {
        throw DataFileException("Unable to open file for writing: "
                                + filename);
    }
    
    
}



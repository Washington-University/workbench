
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

#include <array>
#include <memory>

#include <QStringList>
#include <QTextStream>

#define __FOCI_FILE_DECLARE__
#include "FociFile.h"
#undef __FOCI_FILE_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileContentInformation.h"
#include "DataFileEditorItem.h"
#include "DataFileEditorModel.h"
#include "DataFileException.h"
#include "GroupAndNameHierarchyModel.h"
#include "FileAdapter.h"
#include "FociFileSaxReader.h"
#include "Focus.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiMetaData.h"
#include "SurfaceProjectedItem.h"
#include "Vector3D.h"
#include "XmlAttributes.h"
#include "XmlSaxParser.h"
#include "XmlWriter.h"

using namespace caret;


    
/**
 * \class caret::FociFile 
 * \brief A foci (plural of focus) file.
 */

/**
 * Constructor.
 */
FociFile::FociFile()
: CaretDataFile(DataFileTypeEnum::FOCI),
GroupAndNameHierarchyUserInterface()
{
    initializeFociFile();
}

/**
 * Destructor.
 */
FociFile::~FociFile()
{
    delete m_nameColorTable;
    delete m_classColorTable;
    delete m_metadata;
    
    for (std::vector<Focus*>::iterator iter = m_foci.begin();
         iter != m_foci.end();
         iter++) {
        delete *iter;
    }
    m_foci.clear();
    
    delete m_classNameHierarchy;
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
FociFile::FociFile(const FociFile& obj)
: CaretDataFile(obj),
GroupAndNameHierarchyUserInterface(obj)
{
    initializeFociFile();
    copyHelperFociFile(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
FociFile&
FociFile::operator=(const FociFile& obj)
{
    if (this != &obj) {
        clear();
        CaretDataFile::operator=(obj);
        copyHelperFociFile(obj);
    }
    return *this;    
}

void 
FociFile::initializeFociFile()
{
    m_classColorTable = new GiftiLabelTable();
    m_nameColorTable  = new GiftiLabelTable();
    m_classNameHierarchy = new GroupAndNameHierarchyModel(this);
    m_metadata = new GiftiMetaData();
    m_forceUpdateOfGroupAndNameHierarchy = true;
}


/**
 * Helps with copying an object of this type.
 * @param ff
 *    Object that is copied.
 */
void 
FociFile::copyHelperFociFile(const FociFile& ff)
{
    *m_classColorTable = *ff.m_classColorTable;
    *m_nameColorTable  = *ff.m_nameColorTable;
    if (m_classNameHierarchy != NULL) {
        delete m_classNameHierarchy;
    }
    m_classNameHierarchy = new GroupAndNameHierarchyModel(this);
    
    *m_metadata = *ff.m_metadata;
    
    const int32_t numFoci = getNumberOfFoci();
    for (int32_t i = 0; i < numFoci; i++) {
        m_foci.push_back(new Focus(*ff.getFocus(i)));
    }
    
    m_forceUpdateOfGroupAndNameHierarchy = true;
    
    setModified();
}

/**
 * @return Is this foci file empty (contains zero focuss)?
 */
bool 
FociFile::isEmpty() const
{
    return m_foci.empty();
}

/**
 * @return The structure for this file.
 */
StructureEnum::Enum 
FociFile::getStructure() const
{
    return StructureEnum::ALL;
}

/**
 * Set the structure for this file.
 * @param structure
 *   New structure for this file.
 */
void 
FociFile::setStructure(const StructureEnum::Enum /*structure*/)
{
    // does nothing since focuss apply to all structures
}

/**
 * @return Get access to the file's metadata.
 */
GiftiMetaData* 
FociFile::getFileMetaData()
{
    return m_metadata;
}

/**
 * @return Get access to unmodifiable file's metadata.
 */
const GiftiMetaData* 
FociFile::getFileMetaData() const
{
    return m_metadata;
}

/**
 * Clear the focus file.
 */
void 
FociFile::clear()
{
    CaretDataFile::clear();
    m_metadata->clear();
    clearFoci();
}

/**
 * Clear foci only
 */
void
FociFile::clearFoci()
{
    m_classNameHierarchy->clear();
    m_forceUpdateOfGroupAndNameHierarchy = true;
    m_classColorTable->clear();
    m_nameColorTable->clear();
    const int32_t numFoci = getNumberOfFoci();
    for (int32_t i = 0; i < numFoci; i++) {
        delete m_foci[i];
    }
    m_foci.clear();
}

/**
 * @return the number of foci.
 */
int32_t 
FociFile::getNumberOfFoci() const
{
    return m_foci.size();
}

/**
 * Get the focus at the given index.
 * @param indx
 *   Index of the focus.
 * @return
 *   focus at the given index.
 */
Focus* 
FociFile::getFocus(const int32_t indx)
{
    CaretAssertVectorIndex(m_foci, indx);
    return m_foci[indx];
}

/**
 * Get the focus at the given index.
 * @param indx
 *   Index of the focus.
 * @return
 *   focus at the given index.
 */
const Focus* 
FociFile::getFocus(const int32_t indx) const
{
    CaretAssertVectorIndex(m_foci, indx);
    return m_foci[indx];
}

/**
 * Add a focus.  NOTE: This focus file
 * takes ownership of the 'focus' and 
 * will handle deleting it.  After calling 
 * this method, the caller must never
 * do anything with the focus that was passed
 * to this method.
 *
 * @param focus
 *    Focus added to this focus file.
 */
void 
FociFile::addFocus(Focus* focus)
{
    m_foci.push_back(focus);
    const AString name = focus->getName();
    if ( ! name.isEmpty()) {
        const int32_t nameColorKey = m_nameColorTable->getLabelKeyFromName(name);
        if (nameColorKey < 0) {
            m_nameColorTable->addLabel(name, 0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    AString className = focus->getClassName();
    if ( ! className.isEmpty()) {
        const int32_t classColorKey = m_classColorTable->getLabelKeyFromName(className);
        if (classColorKey < 0) {
            m_classColorTable->addLabel(className, 0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    m_forceUpdateOfGroupAndNameHierarchy = true;
    setModified();
}

/**
 * Add a focus and if th focus' name or class colors are not in the respecitive
 * color tables, add them to the color table.
 *
 * NOTE: This focus file
 * takes ownership of the 'focus' and
 * will handle deleting it.  After calling
 * this method, the caller must never
 * do anything with the focus that was passed
 * to this method.
 *
 * @param focus
 *    Focus added to this focus file.
 */
void
FociFile::addFocusUseColorsFromFocus(Focus* focus)
{
    const AString name = focus->getName();
    if ( ! name.isEmpty()) {
        const int32_t nameColorKey = m_nameColorTable->getLabelKeyFromName(name);
        if (nameColorKey < 0) {
            if (focus->isNameRgbaValid()) {
                float rgba[4];
                focus->getNameRgba(rgba);
                m_nameColorTable->addLabel(name, rgba[0], rgba[1], rgba[2], 1.0f);
            }
            else {
                m_nameColorTable->addLabel(name, 0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        else if (focus->isNameRgbaValid()) {
            const GiftiLabel* label(m_nameColorTable->getLabel(nameColorKey));
            if (label != NULL) {
                std::array<float, 4> focusRGBA;
                focus->getNameRgba(focusRGBA.data());
                std::array<float, 4> rgba;
                label->getColor(rgba.data());
                if (focusRGBA != rgba) {
                    AString msg("Adding focus with name color ("
                                + AString::fromNumbers(focusRGBA.data(), 4)
                                + ") different than existing name color ("
                                + AString::fromNumbers(rgba.data(), 4)
                                + ") in foci file.");
                    CaretLogWarning(msg);
                }
            }
        }
    }
    AString className = focus->getClassName();
    if ( ! className.isEmpty()) {
        const int32_t classColorKey = m_classColorTable->getLabelKeyFromName(className);
        if (classColorKey < 0) {
            if (focus->isClassRgbaValid()) {
                float rgba[4];
                focus->getClassRgba(rgba);
                m_classColorTable->addLabel(className, rgba[0], rgba[1], rgba[2], 1.0f);
            }
            else {
                m_classColorTable->addLabel(className, 0.0f, 0.0f, 0.0f, 1.0f);
            }
        }
        else if (focus->isClassRgbaValid()) {
            const GiftiLabel* label(m_classColorTable->getLabel(classColorKey));
            if (label != NULL) {
                std::array<float, 4> focusRGBA;
                focus->getClassRgba(focusRGBA.data());
                std::array<float, 4> rgba;
                label->getColor(rgba.data());
                if (focusRGBA != rgba) {
                    AString msg("Adding focus with class color ("
                                + AString::fromNumbers(focusRGBA.data(), 4)
                                + ") different than existing class color ("
                                + AString::fromNumbers(rgba.data(), 4)
                                + ") in foci file.");
                    CaretLogWarning(msg);
                }
            }
        }
    }

    focus->setNameRgbaInvalid();
    focus->setClassRgbaInvalid();
    m_foci.push_back(focus);
    
    m_forceUpdateOfGroupAndNameHierarchy = true;
    setModified();
}


/**
 * Remove the focus at the given index.
 * @param indx
 *   Index of focus for removal.
 */
void 
FociFile::removeFocus(const int32_t indx)
{
    CaretAssertVectorIndex(m_foci, indx);
    Focus* focus = getFocus(indx);
    m_foci.erase(m_foci.begin() + indx);
    delete focus;
    m_forceUpdateOfGroupAndNameHierarchy = true;
    setModified();
}

/**
 * Remove the focus.
 * @param focus
 *   Focus that will be removed and DELETED.
 */
void 
FociFile::removeFocus(Focus* focus)
{
    const int32_t numFoci = getNumberOfFoci();
    for (int32_t i = 0;i < numFoci; i++) {
        if (m_foci[i] == focus) {
            removeFocus(i);
            return;
        }
    }
    CaretLogWarning("Attempting to delete focus not in focus file with name: "
                    + focus->getName());
}

/**
 * @return The class and name hierarchy.
 */
GroupAndNameHierarchyModel* 
FociFile::getGroupAndNameHierarchyModel()
{
    m_classNameHierarchy->update(this,
                                     m_forceUpdateOfGroupAndNameHierarchy);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    
    return m_classNameHierarchy;
}

/**
 * @return  The class color table.
 */
GiftiLabelTable* 
FociFile::getClassColorTable()
{
    return m_classColorTable;
}

/**
 * @return  The class color table.
 */
const GiftiLabelTable* 
FociFile::getClassColorTable() const
{
    return m_classColorTable;
}

/**
 * @return  The name color table.
 */
GiftiLabelTable*
FociFile::getNameColorTable()
{
    return m_nameColorTable;
}

/**
 * @return  The name color table.
 */
const GiftiLabelTable*
FociFile::getNameColorTable() const
{
    return m_nameColorTable;
}

/**
 * Version 1 foci files contained one color table for both names
 * and classes.  Newer versions of the foci file keep them in
 * separate tables.
 *
 * @param oldColorTable
 *    Old color table that is split into name and class color tables.
 */
void
FociFile::createNameAndClassColorTables(const GiftiLabelTable* oldColorTable)
{
    CaretAssert(oldColorTable);
    
    m_classColorTable->clear();
    m_nameColorTable->clear();
    
    std::set<QString> nameSet;
    std::set<QString> classSet;
    
    const int numFoci = getNumberOfFoci();
    for (int32_t i = 0; i < numFoci; i++) {
        const Focus* focus = getFocus(i);
        nameSet.insert(focus->getName());
        classSet.insert(focus->getClassName());
    }
    
    /*
     * Create colors for only the "best matching" color.
     */
    for (std::set<QString>::iterator iter = nameSet.begin();
         iter != nameSet.end();
         iter++) {
        const AString colorName = *iter;
        const GiftiLabel* oldLabel = oldColorTable->getLabelBestMatching(colorName);
        if (oldLabel != NULL) {
            const AString bestMatchingName = oldLabel->getName();
            const int32_t labelKey = m_nameColorTable->getLabelKeyFromName(bestMatchingName);
            if (labelKey < 0) {
                float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
                oldLabel->getColor(rgba);
                m_nameColorTable->addLabel(bestMatchingName,
                                           rgba[0],
                                           rgba[1],
                                           rgba[2],
                                           rgba[3]);
            }
        }
    }
    
    /*
     * Create a color for each class name using the best matching color
     */
    for (std::set<QString>::iterator iter = classSet.begin();
         iter != classSet.end();
         iter++) {
        const AString colorName = *iter;
        const GiftiLabel* label = oldColorTable->getLabelBestMatching(colorName);
        float rgba[4] = { 0.0, 0.0, 0.0, 1.0 };
        if (label != NULL) {
            label->getColor(rgba);
        }
        m_classColorTable->addLabel(colorName,
                                   rgba[0],
                                   rgba[1],
                                   rgba[2],
                                   rgba[3]);
    }
}

/**
 * @return A string list containing all foci names
 * sorted in alphabetical order.
 */
QStringList
FociFile::getAllFociNamesSorted() const
{
    std::set<QString> nameSet;
    
    const int32_t numFoci = getNumberOfFoci();
    for (int32_t i = 0;i < numFoci; i++) {
        nameSet.insert(m_foci[i]->getName());
    }

    QStringList sl;
    for (std::set<QString>::iterator iter = nameSet.begin();
         iter != nameSet.end();
         iter++) {
        sl += *iter;
    }
    
    return sl;
}

/**
 * @return The version of the file as a number.
 */
int32_t
FociFile::getFileVersion()
{
    return FociFile::s_fociFileVersion;
}

/**
 * @return The version of the file as a string.
 */
AString 
FociFile::getFileVersionAsString()
{
    return AString::number(FociFile::getFileVersion());
}

/**
 * Invalidate all assigned colors.
 */
void
FociFile::invalidateAllAssignedColors()
{
    const int32_t numFoci = getNumberOfFoci();
    for (int32_t i = 0;i < numFoci; i++) {
        m_foci[i]->setNameRgbaInvalid();
        m_foci[i]->setClassRgbaInvalid();
    }
    m_forceUpdateOfGroupAndNameHierarchy = true;
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
FociFile::readFile(const AString& filename)
{
    clear();
    
    checkFileReadability(filename);
    
    FociFileSaxReader saxReader(this);
    std::unique_ptr<XmlSaxParser> parser(XmlSaxParser::createXmlParser());
    try {
        parser->parseFile(filename, &saxReader);
    }
    catch (const XmlSaxParserException& e) {
        clear();
        setFileName("");
        
        int lineNum = e.getLineNumber();
        int colNum  = e.getColumnNumber();
        
        AString msg = "Parse Error while reading:";
        
        if ((lineNum >= 0) && (colNum >= 0)) {
            msg += (" line/col ("
                    + AString::number(e.getLineNumber())
                    + "/"
                    + AString::number(e.getColumnNumber())
                    + ")");
        }
        
        msg += (": " + e.whatString());
        
        DataFileException dfe(filename,
                              msg);
        CaretLogThrowing(dfe);
        throw dfe;
    }
    
    setFileName(filename);
    
    m_classNameHierarchy->update(this,
                                     true);
    m_forceUpdateOfGroupAndNameHierarchy = false;
    m_classNameHierarchy->setAllSelected(true);
    
    CaretLogFiner("CLASS/NAME Table for : "
                  + getFileNameNoPath()
                  + "\n"
                  + m_classNameHierarchy->toString());
    
    clearModified();
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
FociFile::writeFile(const AString& filename)
{
    if (!(filename.endsWith(".foci") || filename.endsWith(".wb_foci")))
    {
        CaretLogWarning("foci file '" + filename + "'should be saved ending in .foci");
    }
    checkFileWritability(filename);
    
    setFileName(filename);
    
    try {
        //
        // Format the version string so that it ends with at most one zero
        //
        const AString versionString = FociFile::getFileVersionAsString();
        
        //
        // Open the file
        //
        FileAdapter file;
        AString errorMessage;
        QTextStream* textStream = file.openQTextStreamForWritingFile(getFileName(),
                                                                     errorMessage);
        if (textStream == NULL) {
            throw DataFileException(getFileName(),
                                    errorMessage);
        }
        
        //
        // Create the xml writer
        //
        XmlWriter xmlWriter(*textStream);
        
        //
        // Write header info
        //
        xmlWriter.writeStartDocument("1.0");
        
        //
        // Write GIFTI root element
        //
        XmlAttributes attributes;
        
        //attributes.addAttribute("xmlns:xsi",
        //                        "http://www.w3.org/2001/XMLSchema-instance");
        //attributes.addAttribute("xsi:noNamespaceSchemaLocation",
        //                        "http://brainvis.wustl.edu/caret6/xml_schemas/GIFTI_Caret.xsd");
        attributes.addAttribute(FociFile::XML_ATTRIBUTE_VERSION,
                                versionString);
        xmlWriter.writeStartElement(FociFile::XML_TAG_FOCI_FILE,
                                    attributes);
        
        //
        // Write Metadata
        //
        if (m_metadata != NULL) {
            m_metadata->writeAsXML(xmlWriter);
        }
        
        //
        // Write the class color table
        //
        xmlWriter.writeStartElement(XML_TAG_CLASS_COLOR_TABLE);
        m_classColorTable->writeAsXML(xmlWriter);
        xmlWriter.writeEndElement();
        
        //
        // Write the name color table
        //
        xmlWriter.writeStartElement(XML_TAG_NAME_COLOR_TABLE);
        m_nameColorTable->writeAsXML(xmlWriter);
        xmlWriter.writeEndElement();
        
        //
        // Write foci
        //
        const int32_t numFoci = getNumberOfFoci();
        for (int32_t i = 0; i < numFoci; i++) {
            m_foci[i]->writeAsXML(xmlWriter, i);
        }
        
        xmlWriter.writeEndElement();
        xmlWriter.writeEndDocument();
        
        file.close();
        
        clearModified();
    }
    catch (const GiftiException& e) {
        throw DataFileException(e);
    }
    catch (const XmlException& e) {
        throw DataFileException(e);
    }
}

/**
 * @return Is this foci file modified?
 */
bool 
FociFile::isModified() const
{
    if (CaretDataFile::isModified()) {
        return true;
    }
    if (m_metadata->isModified()) {
        return true;
    }
    if (m_classColorTable->isModified()) {
        return true;
    }
    if (m_nameColorTable->isModified()) {
        return true;
    }
    
    /* 
     * Note, these members do not affect modification status:
     * classNameHierarchy 
     */
    
    const int32_t numFoci = getNumberOfFoci();
    for (int32_t i = 0; i < numFoci; i++) {
        if (m_foci[i]->isModified()) {
            return true;
        }
    }
    
    return false;
}

/**
 * Clear the modification status of this foci file.
 */
void 
FociFile::clearModified()
{
    CaretDataFile::clearModified();
    
    m_metadata->clearModified();
    
    m_classColorTable->clearModified();
    m_nameColorTable->clearModified();
    
    const int32_t numFoci = getNumberOfFoci();
    for (int32_t i = 0; i < numFoci; i++) {
        m_foci[i]->clearModified();
    }
}


/**
 * Add information about the file to the data file information.
 *
 * @param dataFileInformation
 *    Consolidates information about a data file.
 */
void
FociFile::addToDataFileContentInformation(DataFileContentInformation& dataFileInformation)
{
    CaretDataFile::addToDataFileContentInformation(dataFileInformation);
    
    QStringList fociNames = getAllFociNamesSorted();
    const int32_t numNames = fociNames.size();
    if (numNames > 0) {
        AString namesListText = "FOCI NAMES";
        for (int32_t i = 0; i < numNames; i++) {
            namesListText.appendWithNewLine("    "
                                             + fociNames.at(i));
            
        }
        
        dataFileInformation.addText(namesListText);
    }
}

/**
 * Called when a group and name hierarchy item has attribute/status changed
 */
void
FociFile::groupAndNameHierarchyItemStatusChanged()
{
    
}

/**
 * @return Names (unique) of all foci
 */
std::vector<AString>
FociFile::getAllFociNames() const
{
    std::set<AString> names;
    for (const Focus* focus : m_foci) {
        if ( ! focus->getName().isEmpty()) {
            names.insert(focus->getName());
        }
    }
    
    std::vector<AString> namesOut(names.begin(), names.end());
    return namesOut;
}

/**
 * Names (unique) of all classes
 */
std::vector<AString>
FociFile::getAllFociClasses() const
{
    std::set<AString> names;
    for (const Focus* focus : m_foci) {
        if ( ! focus->getClassName().isEmpty()) {
            names.insert(focus->getClassName());
        }
    }
    
    std::vector<AString> namesOut(names.begin(), names.end());
    return namesOut;
}

/**
 * @return Names of all foci that use the given class name
 * @param className
 *    Name of class
 */
std::vector<AString>
FociFile::getAllFociNamesThatUseClass(const AString& className)
{
    std::set<AString> names;
    for (const Focus* focus : m_foci) {
        if (focus->getClassName() == className) {
            names.insert(focus->getName());
        }
    }
    
    std::vector<AString> namesOut(names.begin(), names.end());
    return namesOut;
}

/**
 * @return Classes for all foci with the given name (usually just one)
 * @param focusName
 *    Name of the focus
 */
std::vector<AString>
FociFile::getAllClassesForFociWithName(const AString& focusName) const
{
    std::set<AString> names;
    for (const Focus* focus : m_foci) {
        if (focusName == focus->getName()) {
            if ( ! focus->getClassName().isEmpty()) {
                names.insert(focus->getClassName());
            }
        }
    }
    
    std::vector<AString> namesOut(names.begin(), names.end());
    return namesOut;
}

/**
 * Export the content of a foci file to a DataFileEditorModel
 * @return The DataFileEditorModel containing foci data.
 * Caller takes ownership of returned model.
 * @param modelContent
 *    Describes content of the model
 */
FunctionResultValue<DataFileEditorModel*>
FociFile::exportToDataFileEditorModel(const DataFileEditorColumnContent& modelContent) const
{
    const int32_t numFoci(getNumberOfFoci());
    if (numFoci <= 0) {
        return FunctionResultValue<DataFileEditorModel*>(NULL,
                                                         ("There are no foci to export from "
                                                          + getFileNameNoPath()),
                                                         false);
    }
    
    const int32_t numColumns(modelContent.getNumberOfColumns());
    if (numColumns <= 0) {
        return FunctionResultValue<DataFileEditorModel*>(NULL,
                                                         "Model content is empty",
                                                         false);
    }

    DataFileEditorModel* dataFileEditorModel(new DataFileEditorModel());
    /*
     * Setup column titles and default sorting
     */
    dataFileEditorModel->setNumberOfColumnsAndColumnTitles(modelContent);

    const GiftiLabelTable* classColorTable(getClassColorTable());
    const GiftiLabelTable* nameColorTable(getNameColorTable());
    
    for (int32_t i = 0; i < numFoci; i++) {
        const Focus* focus(getFocus(i));
        
        /*
         * For Focus Name
         */
        float nameRGBA[4] { 0.0, 0.0, 0.0, 1.0 };
        const GiftiLabel* nameLabel(nameColorTable->getLabelBestMatching(focus->getName()));
        if (nameLabel != NULL) {
            nameLabel->getColor(nameRGBA);
        }
        
        /*
         * For Focus Class
         */
        float classRGBA[4] { 0.0, 0.0, 0.0, 1.0 };
        const GiftiLabel* classLabel(classColorTable->getLabelBestMatching(focus->getClassName()));
        if (classLabel != NULL) {
            classLabel->getColor(classRGBA);
        }
        
        /*
         * For Focus XYZ
         */
        AString xyzText;
        const int32_t numProj(focus->getNumberOfProjections());
        for (int32_t i = 0; i < numProj; i++) {
            const SurfaceProjectedItem* spi = focus->getProjection(i);
            CaretAssert(spi);
            Vector3D xyz;
            spi->getStereotaxicXYZ(xyz);
            xyzText = xyz.toString();
            break;
        }
        
        /*
         * All items in row represent the same focus
         */
        std::shared_ptr<Focus> focusShared(new Focus(*focus));
        
        /*
         * Set colors for focus so that they are available
         * when and if the focus is copied
         */
        focusShared->setNameRgba(nameRGBA);
        focusShared->setClassRgba(classRGBA);
        
        /*
         * Create a row and add it to model
         */
        QList<QStandardItem*> rowItems;
        
        float emptyRGBA[4] { 0.0, 0.0, 0.0, 0.0 };
        for (int32_t iCol = 0; iCol < numColumns; iCol++) {
            switch (modelContent.getColumnDataType(iCol)) {
                case DataFileEditorItemTypeEnum::CLASS_NAME:
                    rowItems.push_back(new DataFileEditorItem(DataFileEditorItemTypeEnum::CLASS_NAME,
                                                              focusShared,
                                                              focus->getClassName(),
                                                              (focus->getClassName() + focus->getName()),
                                                              classRGBA));
                    break;
                case DataFileEditorItemTypeEnum::COORDINATES:
                    rowItems.push_back(new DataFileEditorItem(DataFileEditorItemTypeEnum::COORDINATES,
                                                              focusShared,
                                                              xyzText,
                                                              xyzText,
                                                              emptyRGBA));
                    break;
                case DataFileEditorItemTypeEnum::GROUP_NAMED:
                    CaretAssert(0);
                    break;
                case DataFileEditorItemTypeEnum::IDENTIFIER:
                    rowItems.push_back(new DataFileEditorItem(DataFileEditorItemTypeEnum::IDENTIFIER,
                                                              focusShared,
                                                              focus->getFocusID(),
                                                              focus->getFocusID(),
                                                              nameRGBA));
                    break;
                case DataFileEditorItemTypeEnum::NAME:
                    rowItems.push_back(new DataFileEditorItem(DataFileEditorItemTypeEnum::NAME,
                                                              focusShared,
                                                              focus->getName(),
                                                              (focus->getName() + focus->getClassName()),
                                                              nameRGBA));
                    break;
            }
        }

        dataFileEditorModel->appendRow(rowItems);
    }

    return  FunctionResultValue<DataFileEditorModel*>(dataFileEditorModel,
                                                      "",
                                                      true);
}


/**
 * Import foci data from the given DataFileEditorModel
 * Replaces content of this instance.
 * @param dataFileEditorModel
 *    Model that contains foci data
 * @return
 *    Function result indicating success or failure
 */
FunctionResult
FociFile::importFromDataFileEditorModel(const DataFileEditorModel& dataFileEditorModel)
{
    AString errorMessage;
    std::vector<const Focus*> newFoci;
    
    const int32_t numRows(dataFileEditorModel.rowCount());
    for (int32_t iRow = 0; iRow < numRows; iRow++) {
        const int32_t column(0);
        const DataFileEditorItem* item(dataFileEditorModel.getDataFileItemAtRowColumn(iRow, column));
        if (item != NULL) {
            const Focus* focus(item->getFocus());
            if (focus != NULL) {
                newFoci.push_back(focus);
            }
            else {
                errorMessage.appendWithNewLine("PROGRAM ERROR: Focus missing at row=" + AString::number(iRow));
            }
        }
        else {
            errorMessage.appendWithNewLine("PROGRAM ERROR: Invalid item at row=" + AString::number(iRow));
        }
    }
    
    if ( ! errorMessage.isEmpty()) {
        return FunctionResult::error(errorMessage);
    }
    
    /*
     * Remove all foci
     */
    clearFoci();
    
    /*
     * Add foci from data file editor model
     */
    for (const Focus* focus : newFoci) {
        addFocusUseColorsFromFocus(new Focus(*focus));
    }
    
    return FunctionResult::ok();
}

/**
 * Get the color for the given name or class name
 * @param samplesColorMode
 *    Indicates to find name or class
 * @param focusNameOrClassName
 *    The focus name or class name
 */
FunctionResultValue<std::array<uint8_t, 4>>
FociFile::getNameOrClassColor(const SamplesColorModeEnum::Enum samplesColorMode,
                              const AString& focusNameOrClassName) const
{
    std::array<uint8_t, 4> rgba;
    rgba.fill(0);
    
    AString errorMessage;
    
    if (focusNameOrClassName.isEmpty()) {
        errorMessage = "Focus or class name is empty";
    }
    else {
        GiftiLabel* gl(NULL);
        switch (samplesColorMode) {
            case SamplesColorModeEnum::SAMPLE:
                break;
            case SamplesColorModeEnum::FOCUS_ONE_NAME:
            case SamplesColorModeEnum::FOCUS_TWO_NAME:
                gl = m_nameColorTable->getLabel(focusNameOrClassName);
                break;
            case SamplesColorModeEnum::FOCUS_ONE_CLASS:
            case SamplesColorModeEnum::FOCUS_TWO_CLASS:
                gl = m_classColorTable->getLabel(focusNameOrClassName);
                break;
        }
        
        if (gl != NULL) {
            std::array<float, 4> rgbaFloat;
            gl->getColor(rgbaFloat.data());
            for (int32_t i = 0; i < 4; i++) {
                rgba[i] = static_cast<uint8_t>(rgbaFloat[i] * 255.0);
            }
        }
        else {
            errorMessage = ("Color not found for " + focusNameOrClassName);
        }
    }
    
    return FunctionResultValue<std::array<uint8_t, 4>>(rgba,
                                                       errorMessage,
                                                       ( errorMessage.isEmpty()));
}


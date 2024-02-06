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

#include <algorithm>

#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include "CaretAssert.h"
#include "GiftiMetaData.h"
#include "GiftiMetaDataXmlElements.h"
#include "GiftiXmlElements.h"
#include "HtmlTableBuilder.h"
#include "XmlWriter.h"

using namespace caret;

/**
 * Constructor.
 *
 */
GiftiMetaData::GiftiMetaData()
    : CaretObject(), TracksModificationInterface()
{
    this->initializeMembersGiftiMetaData();
}

/**
 * Destructor
 */
GiftiMetaData::~GiftiMetaData()
{
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
GiftiMetaData::GiftiMetaData(const GiftiMetaData& o)
    : CaretObject(o), TracksModificationInterface()
{
    this->initializeMembersGiftiMetaData();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
GiftiMetaData&
GiftiMetaData::operator=(const GiftiMetaData& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

bool GiftiMetaData::operator==(const GiftiMetaData& rhs) const
{
    return (metadata == rhs.metadata);
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
GiftiMetaData::copyHelper(const GiftiMetaData& o)
{
    /*
     * Preserve this instance's Unique ID, but only if it already has one.
     */
    if (exists(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID))
    {
        const AString uid = this->getUniqueID();
        this->metadata = o.metadata;
        this->set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID,
                uid);
        this->clearModified();
    } else {
        this->metadata = o.metadata;
    }
}

void
GiftiMetaData::initializeMembersGiftiMetaData()
{
    this->modifiedFlag = false;
}

/**
 * Get the Unique ID.  If there is not a unique ID, one is created.
 * @return String containing unique ID.
 *
 */
AString
GiftiMetaData::getUniqueID() const
{
    AString uid;
    
    MetaDataConstIterator iter = this->metadata.find(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID);
    if (iter != this->metadata.end()) {
        uid = iter->second;
    }
    else {
        uid = SystemUtilities::createUniqueID();
        this->metadata.insert(std::make_pair(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID,
                                             uid));
    }
    
    return uid;
}

/**
 * Reset the unique identifier.
 * Do not call this method unless you really need to such 
 * as in the caret when multiple data arrays have the 
 * same unique identifier.
 */
void 
GiftiMetaData::resetUniqueIdentifier()
{
    this->set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID, 
              SystemUtilities::createUniqueID());
}

/**
 * Remove the UniqueID from this metadata.
 *
 *
void
GiftiMetaData::removeUniqueID()
{
    this->remove(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID);
}
*/

/**
 * Clear the metadata.
 *
 */
void
GiftiMetaData::clear(bool keepUUID)
{
    if (keepUUID && exists(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID))
    {
    /*
     * Preserve this instance's Unique ID.
     */
        const AString uid = this->getUniqueID();
        this->metadata.clear();
        this->set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID,
                uid);
        this->clearModified();
    } else {
        metadata.clear();
    }
}

/**
 * Append the metadata to this metadata.  A comment is always appended.
 * Other metadata are added only if the name is not in "this" metadata.
 *
 * @param smd   Metadata that is to be appended to "this".
 *
 */
void
GiftiMetaData::append(const GiftiMetaData& smd)
{
    for (MetaDataConstIterator iter = smd.metadata.begin();
         iter != smd.metadata.end();
         iter++) {
        if (iter->first != GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID) {
            this->set(iter->first, iter->second);
        }
    }
    this->setModified();
}

/**
 * Clears this metadata and then copies all metadata from "smd"
 * with the exception of the Unique ID.
 *
 * @param smd   Metadata that is to be copied to "this".
 *
 */
void
GiftiMetaData::replace(const GiftiMetaData& smd)
{
    /*
     * Preserve UniqueID.
     */
    const AString uid = this->getUniqueID();
    this->metadata = smd.metadata;
    this->set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID,
              uid);
    this->setModified();
}

/**
 * Sets metadata.  If a metadata entry named "name" already
 * exists, it is replaced.  
 *
 * @param name   Name of metadata entry.
 * @param value  Value for metadata entry.
 *
 */
void
GiftiMetaData::set(const AString& name,
                   const AString& value)
{
    MetaDataIterator namePos = this->metadata.find(name);
    if (namePos != this->metadata.end()) {
        if (namePos->second != value) {
            namePos->second = value;
            this->setModified();
        }
    }
    else {
        this->metadata.insert(std::make_pair(name, value));
        this->setModified();
    }
}

/**
 * Set metadata with an integer value.
 * @param name - name of metadata.
 * @param value - value of metadata.
 *
 */
void
GiftiMetaData::setInt(
                   const AString& name,
                   const int32_t value)
{
    AString s = AString::number(value);
    this->set(name, s);
}

/**
 * Set metadata with an float value.
 * @param name - name of metadata.
 * @param value - value of metadata.
 *
 */
void
GiftiMetaData::setFloat(
                   const AString& name,
                   const float value)
{
    AString s = AString::number(value);
    this->set(name, s);
}

/**
 * Replace ALL of the metadata with the data in the given map.
 *
 * @param map
 *     New metadata that replaces all existing metadata.
 */
void
GiftiMetaData::replaceWithMap(const std::map<AString, AString>& map)
{
    /*
     * Save UniqueID if it has one and use it if no unique ID in given map.
     */
    AString uid;
    if (exists(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID))
    {
        uid = this->getUniqueID();
    }
    
    this->metadata = map;
    
    /*
     * If metadata was not in given map, restore the Unique ID if we had one.
     */
    if (this->metadata.find(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID) == this->metadata.end() &&
        uid != "") {
        this->set(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID,
                  uid);
    }
    
    this->setModified();
}

/**
 * @return ALL of the metadata in map.
 */
std::map<AString, AString>
GiftiMetaData::getAsMap() const
{
    return this->metadata;
}


/**
 * Remove a metadata entry.
 *
 * @param name   Name of metadata entry that is to be removed.
 *
 */
void
GiftiMetaData::remove(const AString& name)
{
    this->metadata.erase(name);
}

/**
 * See if a metadata entry "name" exists.
 *
 * @param  name  Name of metadata entry.
 * @return   Returns true if the metadata entry "name" exists, else false.
 *
 */
bool
GiftiMetaData::exists(const AString& name) const
{
    if (this->metadata.find(name) != this->metadata.end()) {
        return true;
    }
    return false;
}

/**
 * Get a value for metadata entry.  
 *
 * @param  name  Name of metadata entry.
 * @return       The value of the metadata entry "name".  If the
 *               metadata entry "name" does not exist an empty
 *               string is returned.
 *
 */
AString
GiftiMetaData::get(const AString& name) const
{
    if (this->metadata.empty()) {
        return "";
    }
    
    MetaDataConstIterator iter = this->metadata.find(name);
    if (iter != this->metadata.end()) {
        return iter->second;
    }
    return "";
}

/**
 * Get the metadata as an integer value.  If the metadata does not exist
 * of its string representation is not a number, zero is returned.
 * @param name - name of metadata.
 * @return  Integer value associated with the metadata.
 *
 */
int32_t
GiftiMetaData::getInt(const AString& name) const
{
    AString s = this->get(name);
    if (s.length() > 0) {
        int32_t i = s.toInt();
        return i;
    }
    return 0;
}

/**
 * Get the metadata as an float value.  If the metadata does not exist
 * of its string representation is not a number, zero is returned.
 * @param name - name of metadata.
 * @return  Float value associated with the metadata.
 *
 */
float
GiftiMetaData::getFloat(const AString& name) const
{
    AString s = this->get(name);
    if (s.length() > 0) {
        float f = s.toFloat();
        return f;
    }
    return 0.0f;
}

/**
 * Get names of all metadata.
 *
 * @return List of all metadata names.
 *
 */
std::vector<AString>
GiftiMetaData::getAllMetaDataNames() const
{
    std::vector<AString> names;
    
    for (MetaDataConstIterator iter = this->metadata.begin();
         iter != this->metadata.end();
         iter++) {
        names.push_back(iter->first);   
    }
    return names;
}

/**
 * Update metanames from caret5.
 *
 */
void
GiftiMetaData::updateFromCaret5Names()
{
}

/**
 * Replace a metadata name.
 * @param oldName - old name of metadata.
 * @param newName - new name of metadata.
 *
 */
void
GiftiMetaData::replaceName(const AString& oldName,
                           const AString& newName)
{
    MetaDataIterator iter = this->metadata.find(oldName);
    if (iter != this->metadata.end()) {
        AString value = iter->second;
        this->remove(oldName);
        this->set(newName, value);
        this->setModified();
    }
}

/**
 * Replace names of metadata elements using the given map where
 * the 'key' is the old names and the 'value' is the new name.
 * @param newNameMap
 *    Maps old names to new names
 */
void
GiftiMetaData::updateMetaDataNames(const std::map<AString,AString>& newNameMap)
{
    for (const auto& nameValue : newNameMap) {
        replaceName(nameValue.first,
                    nameValue.second);
    }
}

/**
 * Convert to a string.
 *
 * @return  String representation of metadata.
 *
 */
AString
GiftiMetaData::toString() const
{
    AString s = "GiftiMetaData=[";
    
    for (MetaDataConstIterator iter = this->metadata.begin(); iter != this->metadata.end(); iter++) {
        const AString& name = iter->first;
        const AString& value = iter->second;
        s += ("(" + name + "," + value + ")");
    }
    s += "]";
    
    return s;
}

/**
 * Get a nicely formatted string for printing.
 *
 * @param indentation - use as indentation.
 * @return  String containing label information.
 *
 */
AString
GiftiMetaData::toFormattedString(const AString& indentation)
{
    return (indentation + this->toString());
}

/**
 * @return The metadata names and values in an HTML table format
 * @param metaDataName
 *    This metadata names are listed first
 */
AString
GiftiMetaData::toFormattedHtml(const std::vector<AString>& metaDataNames) const
{
    const int32_t numberOfColumns(2);
    HtmlTableBuilder tableBuilder(HtmlTableBuilder::V4_01,
                                  numberOfColumns);
    tableBuilder.addRow("Metadata");

    const std::vector<AString> allNames(getAllMetaDataNames());
    
    for (auto& name : metaDataNames) {
        tableBuilder.addRow(name,
                            get(name));
    }
    
    for (auto& name : allNames) {
        if (std::find(metaDataNames.begin(),
                      metaDataNames.end(),
                      name) == metaDataNames.end()) {
            if (name != GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID) {
                tableBuilder.addRow(name,
                                    get(name));
            }
        }
    }
    return tableBuilder.getAsHtmlTable();
}


/**
 * Write the metadata in GIFTI XML format.
 *
 * @param xmlWriter - output stream
 * @throws GiftiException if an error occurs while writing.
 */
void
GiftiMetaData::writeAsXML(XmlWriter& xmlWriter)
           
{
    try {
        //
        // Write the metadata tag
        //
        xmlWriter.writeStartElement(GiftiXmlElements::TAG_METADATA);
        
        //
        // Write the metadata
        //
        for (MetaDataConstIterator iter = this->metadata.begin();
             iter != this->metadata.end();
             iter++) {
            const AString& key = iter->first;
            const AString& value = iter->second;
            //
            // MD Tag
            //
            xmlWriter.writeStartElement(GiftiXmlElements::TAG_METADATA_ENTRY);
            
            //
            // Name and value
            //
            xmlWriter.writeElementCData(GiftiXmlElements::TAG_METADATA_NAME, key);
            xmlWriter.writeElementCData(GiftiXmlElements::TAG_METADATA_VALUE, value);
            
            //
            // Closing tag
            //
            xmlWriter.writeEndElement();
        }
        
        //
        // Write the closing metadata tag
        //
        xmlWriter.writeEndElement();
    }
    catch (XmlException& e) {
        throw GiftiException(e);
    }
}

void GiftiMetaData::writeCiftiXML1(QXmlStreamWriter& xmlWriter) const
{
    if (metadata.empty()) return;//don't write an empty tag if we have no metadata
    xmlWriter.writeStartElement(GiftiXmlElements::TAG_METADATA);
    for (MetaDataConstIterator iter = metadata.begin(); iter != metadata.end(); ++iter)
    {
        xmlWriter.writeStartElement(GiftiXmlElements::TAG_METADATA_ENTRY);
        xmlWriter.writeTextElement(GiftiXmlElements::TAG_METADATA_NAME, iter->first);
        xmlWriter.writeTextElement(GiftiXmlElements::TAG_METADATA_VALUE, iter->second);
        xmlWriter.writeEndElement();
    }
    xmlWriter.writeEndElement();
}

void GiftiMetaData::writeCiftiXML2(QXmlStreamWriter& xmlWriter) const
{
    writeCiftiXML1(xmlWriter);
}

void GiftiMetaData::writeBorderFileXML3(QXmlStreamWriter& xmlWriter) const
{
    writeCiftiXML1(xmlWriter);
}

void GiftiMetaData::writeSceneFile3(QXmlStreamWriter& xmlWriter) const
{
    writeCiftiXML1(xmlWriter);
}

void GiftiMetaData::readCiftiXML1(QXmlStreamReader& xml)
{
    clear(false);
    while (!xml.atEnd())//don't check the current element's name
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            auto name = xml.name();
            if (name == GiftiXmlElements::TAG_METADATA_ENTRY)
            {
                readEntry(xml);
            } else {
                xml.raiseError("unexpected tag name in " + GiftiXmlElements::TAG_METADATA + ": " + name.toString());
            }
        } else if (xml.isEndElement()) {
            break;
        }
    }
}

void GiftiMetaData::readCiftiXML2(QXmlStreamReader& xml)
{
    readCiftiXML1(xml);
}

void GiftiMetaData::readBorderFileXML1(QXmlStreamReader& xml)
{
    readCiftiXML1(xml);
}

void GiftiMetaData::readBorderFileXML3(QXmlStreamReader& xml)
{
    readCiftiXML1(xml);
}

void GiftiMetaData::readSceneFile3(QXmlStreamReader& xml)
{
    readCiftiXML1(xml);
}

void GiftiMetaData::readEntry(QXmlStreamReader& xml)
{
    AString key, value;
    bool haveKey = false, haveValue = false;
    while (!xml.atEnd())//don't check the current element's name
    {
        xml.readNext();
        if (xml.isStartElement())
        {
            auto name = xml.name();
            if (name == GiftiXmlElements::TAG_METADATA_NAME)
            {
                if (haveKey) throw GiftiException("MD element has multiple Name elements");
                key = xml.readElementText();
                haveKey = true;
            } else if (name == GiftiXmlElements::TAG_METADATA_VALUE) {
                if (haveValue) throw GiftiException("MD element has multiple Value elements");
                value = xml.readElementText();
                haveValue = true;
            } else {
                xml.raiseError("unexpected tag name in " + GiftiXmlElements::TAG_METADATA_ENTRY + ": " + name.toString());
            }
        } else if (xml.isEndElement()) {
            if (haveKey && haveValue)
            {
                if (exists(key))
                {
                    xml.raiseError("key '" + key + "' used more than once in " + GiftiXmlElements::TAG_METADATA);
                } else {
                    set(key, value);
                }
            } else {
                if (haveKey)
                {
                    xml.raiseError(GiftiXmlElements::TAG_METADATA_ENTRY + " element has no " + GiftiXmlElements::TAG_METADATA_VALUE + " element");
                } else {
                    if (haveValue)
                    {
                        xml.raiseError(GiftiXmlElements::TAG_METADATA_ENTRY + " element has no " + GiftiXmlElements::TAG_METADATA_NAME + " element");
                    } else {
                        xml.raiseError(GiftiXmlElements::TAG_METADATA_ENTRY + " element has no " + GiftiXmlElements::TAG_METADATA_NAME +
                                       " or " + GiftiXmlElements::TAG_METADATA_VALUE + " element");
                    }
                }
            }
            break;
        }
    }
    CaretAssert(xml.hasError() || (xml.isEndElement() && xml.name() == QLatin1String("MD")));
}

/**
 * Set this object has been modified.
 *
 */
void
GiftiMetaData::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
GiftiMetaData::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Get the modification status.  Returns true if this object or
 * any of its children have been modified.
 * @return - The modification status.
 *
 */
bool
GiftiMetaData::isModified() const
{
    return this->modifiedFlag;
}

/**
 * Is the metadata empty (no metadata)?
 */
bool
GiftiMetaData::isEmpty() const
{
    return this->metadata.empty();
}

/**
 * @return Number of metadata
 */
int32_t
GiftiMetaData::getNumberOfMetaData() const
{
    return this->metadata.size();
}

/**
 * Validate the the required metadata (values must not be empty)
 * @param requiredMetaDataNames
 *    Names of required metadata
 * @param errorMessageOut
 *    Contains error message if validation fails
 * @return
 *    True if metadata is valid, else false.
 */
bool
GiftiMetaData::validateRequiredMetaData(const std::vector<AString>& requiredMetaDataNames,
                                        AString& errorMessageOut) const
{
    errorMessageOut.clear();
    
    for (const auto& name : requiredMetaDataNames) {
        const AString& value(get(name).trimmed());
        if (value.isEmpty()) {
            errorMessageOut.appendWithNewLine("   " + name);
        }
    }
    
    if ( ! errorMessageOut.isEmpty()) {
        errorMessageOut.insert(0, "Required metadata is invalid:\n");
    }
    
    return errorMessageOut.isEmpty();
}



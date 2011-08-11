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

#include <algorithm>

#include "GiftiMetaData.h"
#include "GiftiMetaDataXmlElements.h"

#include "NiftiUtilities.h"

#include "GiftiXmlElements.h"
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

/**
 * Helps with copy constructor and assignment operator.
 */
void
GiftiMetaData::copyHelper(const GiftiMetaData& o)
{
    this->metadata = o.metadata;
    this->clearModified();
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
    AString uid = "UID";
    MetaDataConstIterator iter = this->metadata.find(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID);
    if (iter == this->metadata.end()) {
        
    }
    return uid;
}

/**
 * Remove the UniqueID from this metadata.
 *
 */
void
GiftiMetaData::removeUniqueID()
{
    this->remove(GiftiMetaDataXmlElements::METADATA_NAME_UNIQUE_ID);
}

/**
 * Clear the metadata.  Note does create a UniqueID.
 *
 */
void
GiftiMetaData::clear()
{
    this->metadata.clear();
    this->clearModified();
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
        this->set(iter->first, iter->second);
    }
    this->setModified();
}

/**
 * Clears this metadata and then copies all metadata from "smd".
 *
 * @param smd   Metadata that is to be copied to "this".
 *
 */
void
GiftiMetaData::replace(const GiftiMetaData& smd)
{
    this->metadata = smd.metadata;
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
GiftiMetaData::set(
                   const AString& name,
                   const AString& value)
{
    this->metadata.insert(std::make_pair(name, value));
    this->setModified();
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
GiftiMetaData::exists(const AString& name)
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
GiftiMetaData::replaceName(
                   const AString& oldName,
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
    return this->toString();
}

/**
 * Write the metadata in GIFTI XML format.
 *
 * @param xmlWriter - output stream
 * @throws GiftiException if an error occurs while writing.
 */
void
GiftiMetaData::writeAsXML(XmlWriter& xmlWriter)
            throw (GiftiException)
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

/**
 * Update the user name and date in the metadata.
 *
 */
void
GiftiMetaData::updateUserNameDate()
{
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


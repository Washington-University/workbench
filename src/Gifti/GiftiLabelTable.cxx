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

#include <sstream>

#include "CaretAssert.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiXmlElements.h"
#include <algorithm>

#include "XmlWriter.h"

using namespace caret;

/**
 * Constructor.
 *
 */
GiftiLabelTable::GiftiLabelTable()
    : CaretObject()
{
    this->initializeMembersGiftiLabelTable();
}

/**
 * Destructor
 */
GiftiLabelTable::~GiftiLabelTable()
{
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != labelsMap.end();
         iter++) {
        delete iter->second;
    }
    this->labelsMap.clear();
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
GiftiLabelTable::GiftiLabelTable(const GiftiLabelTable& glt)
    : CaretObject(glt), TracksModificationInterface()
{
    this->initializeMembersGiftiLabelTable();
    this->copyHelper(glt);
}

/**
 * Assignment operator.
 */
GiftiLabelTable&
GiftiLabelTable::operator=(const GiftiLabelTable& glt)
{
    if (this != &glt) {
        CaretObject::operator=(glt);
        this->copyHelper(glt);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
GiftiLabelTable::copyHelper(const GiftiLabelTable& glt)
{
    this->clear();
    
    for (LABELS_MAP_CONST_ITERATOR iter = glt.labelsMap.begin();
         iter != labelsMap.end();
         iter++) {
        GiftiLabel* gl2 = new GiftiLabel(*iter->second);
        this->addLabel(gl2);
    }
}

void
GiftiLabelTable::initializeMembersGiftiLabelTable()
{
    this->modifiedFlag = false;
}
/**
 * Clear the labelTable.
 *
 */
void
GiftiLabelTable::clear()
{
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != labelsMap.end();
         iter++) {
        delete iter->second;
    }
    this->labelsMap.clear();
    
    GiftiLabel gl(0, "???", 1.0, 1.0, 1.0, 1.0);
    this->addLabel(&gl);
    this->modifiedFlag = false;
}

/**
 * Append a label table to this label table.  Since labels may be
 * duplicated, the map returned that converts the keys of
 * the appended LabelTable to keys for "this" label table.
 *
 * @param lt  Label table that is to be appended.
 *
 * @return  A map where the keys are the keys in the label table
 *    that is passed as a parameter and the values are the keys
 *    into "this" label table.
 *
 */
std::map<int32_t,int32_t>
GiftiLabelTable::append(const GiftiLabelTable& glt)
{
    std::map<int32_t,int32_t> keyConverterMap;
    
    for (LABELS_MAP_CONST_ITERATOR iter = glt.labelsMap.begin();
         iter != glt.labelsMap.end();
         iter++) {
        int32_t key = iter->first;
        GiftiLabel* gl = new GiftiLabel(*iter->second);
        int32_t newKey = this->addLabel(gl);
        
        keyConverterMap.insert(std::make_pair(key, newKey));
    }
    return keyConverterMap;
}

/**
 * Add a label.  If a label with the name exists, its colors
 * are replaced with these color components.
 * @param labelName Name of label.
 * @param red  Red color component ranging 0.0 to 1.0.
 * @param green Green color component ranging 0.0 to 1.0.
 * @param blue Blue color component ranging 0.0 to 1.0.
 * @param alpha Alpha color component ranging 0.0 to 1.0.
 * @return  Index of the existing label, or, if no label 
 * exists with name, index of new label.
 *
 */
int32_t
GiftiLabelTable::addLabel(
                   const AString& labelName,
                   const float red,
                   const float green,
                   const float blue,
                   const float alpha)
{
    const GiftiLabel gl(-1, labelName, red, green, blue, alpha);
    return this->addLabel(&gl);
}

/**
 * Add a label.  If a label with the name exists, its colors
 * are replaced with these color components.
 * @param labelName Name of label.
 * @param red  Red color component ranging 0.0 to 1.0.
 * @param green Green color component ranging 0.0 to 1.0.
 * @param blue Blue color component ranging 0.0 to 1.0.
 * @return  Index of the existing label, or, if no label 
 * exists with name, index of new label.
 *
 */
int32_t
GiftiLabelTable::addLabel(
                   const AString& labelName,
                   const float red,
                   const float green,
                   const float blue)
{
    return this->addLabel(labelName, red, green, blue, 1.0f);
}

/**
 * Add a label.  If a label with the name exists, its colors
 * are replaced with these color components.
 * @param labelName Name of label.
 * @param red  Red color component ranging 0 to 255.
 * @param green Green color component ranging 0 to 255.
 * @param blue Blue color component ranging 0 to 255.
 * @param alpha Alpha color component ranging 0 to 255.
 * @return  Index of the existing label, or, if no label 
 * exists with name, index of new label.
 *
 */
int32_t
GiftiLabelTable::addLabel(
                   const AString& labelName,
                   const int32_t red,
                   const int32_t green,
                   const int32_t blue,
                   const int32_t alpha)
{
    const GiftiLabel gl(-1, labelName, red, green, blue, alpha);
    return this->addLabel(&gl);
}

/**
 * Add a label.  If a label with the name exists, its colors
 * are replaced with these color components.
 * @param labelName Name of label.
 * @param red  Red color component ranging 0 to 255.
 * @param green Green color component ranging 0 to 255.
 * @param blue Blue color component ranging 0 to 255.
 * @return  Index of the existing label, or, if no label 
 * exists with name, index of new label.
 *
 */
int32_t
GiftiLabelTable::addLabel(
                   const AString& labelName,
                   const int32_t red,
                   const int32_t green,
                   const int32_t blue)
{
    return this->addLabel(labelName, red, green, blue, 255);
}

/**
 * Add a label to the label table.  If the label's key is already in
 * the label table, a new key is created.  If a label of the same
 * name already exists, the key of the existing label is returned
 * and its color is overridden.
 * @param glIn - Label to add.
 * @return  Key of the label, possibly different than its original key.
 *
 */
int32_t
GiftiLabelTable::addLabel(const GiftiLabel* glIn)
{
    int32_t key = glIn->getKey();
    if (key < 0) {
        key = this->generateUnusedKey();
        this->labelsMap.insert(std::make_pair(key, new GiftiLabel(*glIn)));
        return key;
    }
    
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        GiftiLabel* gl = iter->second;
        gl->setName(glIn->getName());
        float rgba[4];
        glIn->getColor(rgba);
        gl->setColor(rgba);
        key = iter->first;
    }
    else {
        this->labelsMap.insert(std::make_pair(key, new GiftiLabel(*glIn)));
    }
    return key;
}

/**
 * Generate an unused key.
 * @return An unused key.
 */ 
int32_t 
GiftiLabelTable::generateUnusedKey() const
{
    std::set<int32_t> keys = getKeys();
    
    int32_t newKey = 1;
    bool found = false;
    while (found == false) {
        if (std::find(keys.begin(), keys.end(), newKey) == keys.end()) {
            found = true;
        }
        else {
            newKey++;
        }
    }
    
    return newKey;
}

/**
 * Remove the label with the specified key.
 * @param key - key of label.
 *
 */
void
GiftiLabelTable::deleteLabel(const int32_t key)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        this->labelsMap.erase(iter);
        delete iter->second;
    }
}

/**
 * Remove a label from the label table.
 * This method WILL DELETE the label passed
 * in so the caller should never use the parameter
 * passed after this call.
 * @param label - label to remove.
 *
 */
void
GiftiLabelTable::deleteLabel(const GiftiLabel* label)
{
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        if (iter->second == label) {
            this->labelsMap.erase(iter);
            break;
        }
    }
    delete label;
}

/**
 * Remove unused labels from the label table.  Note that the unassigned
 * label is not removed, even if it is unused.
 *
 * @param usedLabelKeys - Color keys that are in use.
 *
 */
void
GiftiLabelTable::deleteUnusedLabels(const std::set<int32_t>& usedLabelKeys)
{
    LABELS_MAP newMap;
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        int32_t key = iter->first;
        GiftiLabel* gl = iter->second;
        if (usedLabelKeys.find(key) != usedLabelKeys.end()) {
            newMap.insert(std::make_pair(key, gl));
        }
        else {
            delete gl;
        }
    }
    
    this->labelsMap = newMap;
    this->setModified();
}

/**
 * Insert the label using the labels key.
 * @param labelIn - Label to insert (replaces an existing label
 *    with the same key).
 *
 */
void
GiftiLabelTable::insertLabel(const GiftiLabel* labelIn)
{
    GiftiLabel* label = new GiftiLabel(*labelIn);
    int32_t key = label->getKey();
    if (key < 0) {
        key = this->generateUnusedKey();
        label->setKey(key);
    }
    this->labelsMap.insert(std::make_pair(label->getKey(), label));
    this->setModified();
}

/**
 * Get the key of a lable from its name.
 * @param name   Name to search for.
 * @return       Key of Name or -1 if not found.
 *
 */
int32_t
GiftiLabelTable::getLabelKeyFromName(const AString& name) const
{
    LABELS_MAP newMap;
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        int32_t key = iter->first;
        GiftiLabel* gl = iter->second;
        if (gl->getName() == name) {
            return key;
        }
    }
    return -1;
}

/**
 * Get a GIFTI Label from its name.
 * @param labelName - Name of label that is sought.
 * @return  Reference to label with name or null if no matching label.
 *
 */
const GiftiLabel*
GiftiLabelTable::getLabel(const AString& labelName) const
{
    LABELS_MAP newMap;
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        GiftiLabel* gl = iter->second;
        if (gl->getName() == labelName) {
            return gl;
        }
    }
    return NULL;
}

/**
 * Get the label whose name is the longest substring of "name" beginning
 * at the first character.
 *
 * @param name - name for which best matching label is sought.
 * @return Reference to best matching label or null if not found.
 *
 */
const GiftiLabel*
GiftiLabelTable::getLabelBestMatching(const AString& name) const
{
    GiftiLabel* bestMatchingLabel = NULL;
    size_t bestMatchLength = -1;
    
    LABELS_MAP newMap;
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        GiftiLabel* gl = iter->second;
        AString labelName = gl->getName();
        if (name.toStdString().find(labelName.toStdString()) == 0) {
            size_t len = labelName.length();
            if (len > bestMatchLength) {
                bestMatchLength = len;
                bestMatchingLabel = iter->second;
            }
        }
    }
    return NULL;
}

/**
 * Get the GiftiLabel at the specified key.
 *
 * @param  key - Key of GiftiLabel entry.
 * @return       The GiftiLabel at the specified key or null if the
 *    there is not a label at the specified key.
 *
 */
const GiftiLabel*
GiftiLabelTable::getLabel(const int32_t key) const
{
    LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        return iter->second;
    }
    return NULL;
}

/**
 * Get the key for the unassigned label.
 * @return  Index of key for unassigned label.
 *  or -1 if not found.
 *
 */
int32_t
GiftiLabelTable::getUnassignedLabelKey() const
{
    const GiftiLabel* gl = this->getLabel("???");
    if (gl != NULL) {
        return gl->getKey();
    }
    return -1;
}

/**
 * Get the number of labels.  This value is one greater than the last
 * label key.  Note that not every key may have a label.  If there
 * are no labels this returns 0.
 * @return  Number of labels.
 *
 */
int32_t
GiftiLabelTable::getNumberOfLabels() const
{
    return this->labelsMap.size();
}

/**
 * Get the name of the label at the key.  If there is no label at the
 * key an empty string is returned.
 * @param key - key of label.
 * @return  Name of label at inkeydex.
 *
 */
AString
GiftiLabelTable::getLabelName(const int32_t key) const
{
    LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        const AString name = iter->second->getName();
        return name;
    }
    return "";
}

/**
 * Set the name of a label.
 * @param key - key of label.
 * @param name - new name of label.
 *
 */
void
GiftiLabelTable::setLabelName(
                   const int32_t key,
                   const AString& name)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        iter->second->setName(name);
    }
}

/**
 * Set a label.  If a label with the specified key exists,
 * it is replaced.
 * 
 * @param key    Key for label.
 * @param name   Name of label.
 * @param red    Red color component.
 * @param green  Green color component.
 * @param blue   Blue color component.
 * @param alpha  Alpha color component.
 *
 */
void
GiftiLabelTable::setLabel(
                   const int32_t key,
                   const AString& name,
                   const float red,
                   const float green,
                   const float blue,
                   const float alpha)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        GiftiLabel* gl = iter->second;
        gl->setName(name);
        float rgba[4] = { red, green, blue, alpha };
        gl->setColor(rgba);
    }
    else {
        GiftiLabel gl(key, name, red, green, blue, alpha);
        this->addLabel(&gl);
    }
}

/**
 * Set a label.  If a label with the specified key exists,
 * it is replaced.
 * 
 * @param key    Key for label.
 * @param name   Name of label.
 * @param red    Red color component.
 * @param green  Green color component.
 * @param blue   Blue color component.
 * @param alpha  Alpha color component.
 * @param x      The X-coordinate.
 * @param y      The Y-coordinate.
 * @param z      The Z-coordinate.
 *
 */
void
GiftiLabelTable::setLabel(const int32_t key,
                          const AString& name,
                          const float red,
                          const float green,
                          const float blue,
                          const float alpha,
                          const float x,
                          const float y,
                          const float z)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        GiftiLabel* gl = iter->second;
        gl->setName(name);
        float rgba[4] = { red, green, blue, alpha };
        gl->setColor(rgba);
        gl->setX(x);
        gl->setY(y);
        gl->setZ(z);
    }
    else {
        GiftiLabel gl(key, name, red, green, blue, alpha, x, y, z);
        this->addLabel(&gl);
    }
}

/**
 * Get the selection status of the label at the specified key.  If there
 * is no label at the key, false is returned.
 * @param key - key of label
 * @return  selection status of label.
 *
 */
bool
GiftiLabelTable::isLabelSelected(const int32_t key) const
{
    LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        return iter->second->isSelected();
    }
    return false;
}

/**
 * Set the selection status of a label.
 * @param key - key of label.
 * @param sel - new selection status.
 *
 */
void
GiftiLabelTable::setLabelSelected(
                   const int32_t key,
                   const bool sel)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        iter->second->setSelected(sel);
    }
}

/**
 * Set the selection status for all labels.
 * @param newStatus  New selection status.
 *
 */
void
GiftiLabelTable::setSelectionStatusForAllLabels(const bool newStatus)
{
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        GiftiLabel* gl = iter->second;
        gl->setSelected(newStatus);
    }
}

/**
 * Get the alpha color component for a label.  If the key is not a
 * valid label, an alpha of zero is returned.
 * @param key - Key of label.
 * @return  Alpha for label or zero if invalid key.
 *
 */
float
GiftiLabelTable::getLabelAlpha(const int32_t key) const
{
    const GiftiLabel* gl = this->getLabel(key);
    if (gl != NULL) {
        return gl->getAlpha();
    }
    return 0.0;
}

/**
 * Get the color for a label.
 * @param key - key of label.
 * @return Its color components or null if it is an invalid key.
 *
 */
void
GiftiLabelTable::getLabelColor(const int32_t key, float rgbaOut[4]) const
{
    const GiftiLabel* gl = this->getLabel(key);
    if (gl != NULL) {
        gl->getColor(rgbaOut);
    }
}

/**
 * Set the color of a label.
 * @param key - key of label.
 * @param color - new color of label.
 *
 */
void
GiftiLabelTable::setLabelColor(
                   const int32_t key,
                   const float color[])
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        GiftiLabel* gl = iter->second;
        gl->setColor(color);
    }
}

/**
 * Get the label keys sorted by label name.
 *
 * @return  int array containing label keys sorted by name.
 *
 */
int32_t
GiftiLabelTable::getLabelKeysSortedByName() const
{
    CaretAssertMessage(0, "NEEDS TO BE IMPLEMENTED");
    return 0;
}

/**
 * Reset the label counts to zero.
 *
 */
void
GiftiLabelTable::resetLabelCounts()
{
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        if (iter != this->labelsMap.end()) {
            GiftiLabel* gl = iter->second;
            gl->setCount(0);
        }
    }
}

/**
 * Create labels for the keys with generated names and colors.
 * @param newKeys - Keys that need labels.
 *
 */
void
GiftiLabelTable::createLabelsForKeys(const std::set<int32_t>& newKeys)
{
    AString namePrefix = "Name";
    int32_t nameCount = 0;
    int32_t colorCounter = 0;
    for (std::set<int32_t>::iterator iter = newKeys.begin();
         iter != newKeys.end();
         iter++) {
        int32_t key = *iter;
        if (this->getLabel(key) == NULL) {
            bool found = false;
            AString name;
            while (! found) {
                std::stringstream str;
                str << namePrefix.toStdString() << "_" << nameCount;
                nameCount++;
                
                name = AString::fromStdString(str.str());
                if (this->getLabel(name) == NULL) {
                    found = true;
                }
                float red = 0.0f;
                float green = 0.0f;
                float blue = 0.0f;
                float alpha = 1.0f;
                switch (colorCounter) {
                    case 0:
                        red = 1.0f;
                        break;
                    case 1:
                        red  = 1.0f;
                        blue = 0.5f;
                        break;
                    case 2:
                        red  = 1.0f;
                        blue = 1.0f;
                        break;
                    case 3:
                        red   = 1.0f;
                        green = 0.5f;
                        break;
                    case 4:
                        red   = 1.0f;
                        green = 0.5f;
                        blue  = 0.5f;
                        break;
                    case 5:
                        red   = 1.0f;
                        green = 0.5f;
                        blue  = 1.0f;
                        break;
                    case 6:
                        blue = 0.5f;
                        break;
                    case 7:
                        blue = 1.0f;
                        break;
                    case 8:
                        red = 0.5f;
                        break;
                    case 9:
                        red  = 0.5f;
                        blue = 0.5f;
                        break;
                    case 10:
                        red  = 0.5f;
                        blue = 1.0f;
                        break;
                    case 11:
                        red   = 0.5f;
                        green = 0.5f;
                        break;
                    case 12:
                        red   = 0.5f;
                        green = 0.5f;
                        blue  = 0.5f;
                        break;
                    case 13:
                        red   = 0.5f;
                        green = 0.5f;
                        blue  = 1.0f;
                        break;
                    case 14:
                        red   = 0.5f;
                        green = 1.0f;
                        break;
                    case 15:
                        red   = 0.5f;
                        green = 1.0f;
                        blue  = 0.5f;
                        break;
                    case 16:
                        red   = 0.5f;
                        green = 1.0f;
                        blue  = 1.0f;
                        colorCounter = 0;  // start over
                        break;
                }
                colorCounter++;
                
                GiftiLabel* gl = new GiftiLabel(key, name, red, green, blue, alpha);
                this->addLabel(gl);
            }
        }
    }
}

/**
 * Write the metadata in GIFTI XML format.
 *
 * @param xmlWriter - output stream
 * @throws GiftiException if an error occurs while writing.
 *
 */
void
GiftiLabelTable::writeAsXML(XmlWriter& xmlWriter) throw (GiftiException)
{
    try {
        //
        // Write the label tag
        //
        xmlWriter.writeStartElement(GiftiXmlElements::TAG_LABEL_TABLE);
        
        //
        // Write the labels
        //
        std::set<int32_t> keys = this->getKeys();
        for (std::set<int32_t>::const_iterator iter = keys.begin();
             iter != keys.end();
             iter++) {
            int key = *iter;
            const GiftiLabel* label = this->getLabel(key);
            
            if (label != NULL) {
                XmlAttributes attributes;
                attributes.addAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_KEY,
                                        key);
                
                float* rgba = label->getColor();
                attributes.addAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_RED,
                                        rgba[0]);
                attributes.addAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_GREEN,
                                        rgba[1]);
                attributes.addAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_BLUE,
                                        rgba[2]);
                attributes.addAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_ALPHA,
                                        rgba[3]);
                
                xmlWriter.writeElementCData(GiftiXmlElements::TAG_LABEL,
                                            attributes, label->getName());
            }
        }
        
        //
        // Write the closing label tag
        //
        xmlWriter.writeEndElement();
    }
    catch (XmlException& e) {
        throw GiftiException(e);
    }
}


/**
 * Convert to a string.
 *
 * @return  String representation of labelTable.
 *
 */
AString
GiftiLabelTable::toString() const
{
    AString s = "GiftiLabelTable=[";
    
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        s += "key=";
        s += AString::number(iter->first);
        s += iter->second->toString();
        s += ",";
        
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
GiftiLabelTable::toFormattedString(const AString& indentation)
{
    return this->toString();
}

/**
 * Read the label table from XML DOM structures.
 * @param rootNode - the LabelTable node.
 * @throws GiftiException if an error occurs.
 *
 *
void
GiftiLabelTable::readFromXMLDOM(const Node* rootNode)
            throw (GiftiException)
{
}
*/

/**
 * Read a LabelTable from a String.  The beginning and ending tags must
 * be the label table tag GiftiXmlElements.TAG_LABEL_TABLE.
 * @param s - string containing the label table.
 * @throws GiftiException  If there is an error processing the table.
 *
 */
void
GiftiLabelTable::readFromXmlString(const AString& s)
            throw (GiftiException)
{
}

/**
 * Set this object has been modified.
 *
 */
void
GiftiLabelTable::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
GiftiLabelTable::clearModified()
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
GiftiLabelTable::isModified() const
{
    if (this->modifiedFlag) return true;
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        if (iter->second->isModified()) return true;
    }
    return false;
}

/**
 * Get an iterator that steps thought the label keys in
 *    ascending order.
 * @return  An iterator for stepping through the keys in
 *    ascending order.
 *
 *
Iterator<int32_t>
GiftiLabelTable::getKeysIterator() const
{
    return this->
}
*/

/**
 * Get the valid keys of the labels in ascending order.
 * @return  A Set containing the valid keys of the label in
 *    ascending order.
 *
 */
std::set<int32_t>
GiftiLabelTable::getKeys() const
{
    std::set<int32_t> keys;
    for (std::map<int32_t,GiftiLabel*>::const_iterator iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        keys.insert(iter->first);
    }
    return keys;
}


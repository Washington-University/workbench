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

#include <algorithm>
#include <sstream>

#include "AStringNaturalComparison.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "GiftiXmlElements.h"
#include "StringTableModel.h"
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
    clear();//actually adds the 0: ??? label
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
         iter != glt.labelsMap.end();
         iter++) {
        GiftiLabel* myLabel = this->getLabel(iter->second->getKey());
        if (myLabel != NULL)
        {
            *myLabel = *(iter->second);
        } else {
            addLabel(iter->second);
        }
    }
}

void
GiftiLabelTable::initializeMembersGiftiLabelTable()
{
    this->modifiedFlag = false;
    
    m_tableModelColumnCount = 0;
    m_tableModelColumnIndexKey         = m_tableModelColumnCount++;
    m_tableModelColumnIndexName        = m_tableModelColumnCount++;
    m_tableModelColumnIndexColorSwatch = m_tableModelColumnCount++;
    m_tableModelColumnIndexRed         = m_tableModelColumnCount++;
    m_tableModelColumnIndexGreen       = m_tableModelColumnCount++;
    m_tableModelColumnIndexBlue        = m_tableModelColumnCount++;
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
    
    GiftiLabel gl(0, "???", 1.0, 1.0, 1.0, 0.0);
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
GiftiLabelTable::append(const GiftiLabelTable& glt, const bool errorOnLabelConflict)
{
    std::map<int32_t,int32_t> keyConverterMap;
    
    for (LABELS_MAP_CONST_ITERATOR iter = glt.labelsMap.begin();
         iter != glt.labelsMap.end();
         iter++) {
        int32_t key = iter->first;
        int32_t newKey = this->addLabel(iter->second);

        if (newKey != key)
        {
            if (errorOnLabelConflict)
            {
                throw CaretException("conflicting key value " + AString::number(key) + " for label '" + iter->second->getName() + "', please fix this and any additional conflicts with a label-modify-keys command or an option in this command, if available");
            } else {
                CaretLogWarning("conflicting key value for label '" + iter->second->getName() + "' reassigned to key " + AString::number(newKey));
            }
        }
        
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
    const GiftiLabel gl(GiftiLabel::getInvalidLabelKey(), labelName, red, green, blue, alpha);
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
    const GiftiLabel gl(GiftiLabel::getInvalidLabelKey(), labelName, red, green, blue, alpha);
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
    /*
     * First see if a label with the same name already exists
     */
    int32_t key = this->getLabelKeyFromName(glIn->getName());
    
    /*
     * If no label with the name exists, get the key
     * (which may be invalid) from the input label,
     * and check that nothing uses that key
     */
    if (key == GiftiLabel::getInvalidLabelKey()) {
        int32_t tempkey = glIn->getKey();
        LABELS_MAP_ITERATOR iter = this->labelsMap.find(tempkey);
        if (iter == labelsMap.end())
        {
            key = tempkey;
        }
    }
    
    /*
     * Still need a key, find an unused key
     */
    if (key == GiftiLabel::getInvalidLabelKey()) {
        key = this->generateUnusedKey();
        
        GiftiLabel* gl = new GiftiLabel(*glIn);
        gl->setKey(key);
        this->labelsMap.insert(std::make_pair(key, gl));
        return key;
    }
    
    if (key == 0)
    {
        issueLabelKeyZeroWarning(glIn->getName());
//        if (glIn->getName() != "???") {
//            CaretLogWarning("Label 0 overridden!");
//        }
    }
    
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        /*
         * Update existing label
         */
        GiftiLabel* gl = iter->second;
        gl->setName(glIn->getName());
        float rgba[4];
        glIn->getColor(rgba);
        gl->setColor(rgba);
        key = iter->first;
    }
    else {
        /*
         * Insert a new label
         */
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
    const int32_t numKeys = labelsMap.size();
    LABELS_MAP::const_reverse_iterator rbegin = labelsMap.rbegin();//reverse begin is largest key
    if (numKeys > 0 && rbegin->first > 0)//there is at least one positive key
    {
        if (rbegin->first < numKeys)
        {
            CaretAssert(labelsMap.find(rbegin->first + 1) == labelsMap.end());
            return rbegin->first + 1;//keys are compact unless negatives exist, in which case consider it "compact enough" if positive holes equal number of negative keys
        } else {
            LABELS_MAP::const_iterator begin = labelsMap.begin();
            if (begin->first == 1 && rbegin->first == numKeys)
            {
                CaretAssert(labelsMap.find(rbegin->first + 1) == labelsMap.end());
                return rbegin->first + 1;//keys are compact but missing 0, do not return 0, so return next
            } else {//there aren't enough negatives to make up for the missing, search for a hole in the positives
                LABELS_MAP::const_iterator iter = labelsMap.upper_bound(0);//start with first positive
                int32_t curVal = 0;//if it isn't one, we can stop early
                while (iter != labelsMap.end() && iter->first == curVal + 1)//it should NEVER hit end(), due to above checks, but if it did, it would return rbegin->first + 1
                {
                    curVal = iter->first;
                    ++iter;
                }
                CaretAssert(labelsMap.find(curVal + 1) == labelsMap.end());
                return curVal + 1;
            }
        }
    } else {
        CaretAssert(labelsMap.find(1) == labelsMap.end());
        return 1;//otherwise, no keys exist or all keys are non-positive, return 1
    }
   /*int32_t numKeys = labelsMap.size();
   LABELS_MAP_CONST_ITERATOR myend = labelsMap.end();
   if (labelsMap.upper_bound(numKeys - 1) == myend)
   {//returns a valid iterator only if there is no strictly greater key - zero key is assumed to exist, being the ??? special palette, no negatives exist
      return numKeys;//keys are therefore compact, return the next one
   }
   if (labelsMap.find(0) == myend && labelsMap.upper_bound(numKeys) == myend)
   {//similar check, but in case label 0 doesn't exist (but don't override it)
      return numKeys + 1;
   }
   std::vector<int> scratch;
   scratch.resize(numKeys);//guaranteed to have at least one missing spot within this range other than zero, or one of above tests would have returned
   for (int32_t i = 0; i < numKeys; ++i)
   {
      scratch[i] = 0;
   }
   for (LABELS_MAP_CONST_ITERATOR iter = labelsMap.begin(); iter != myend; ++iter)
   {
      if (iter->first >= 0 && iter->first < numKeys)
      {//dont try to mark above the range
         scratch[iter->first] = 1;
      }
   }
   for (int32_t i = 1; i < numKeys; ++i)
   {//NOTE: start at 1! 0 is reserved (sort of)
      if (scratch[i] == 0)
      {
         return i;
      }
   }
   CaretAssertMessage(false, "generateUnusedKey() failed for unknown reasons");
   return 0;//should never happen//*/
    /*std::set<int32_t> keys = getKeys();
    
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
    
    return newKey;//*/
}

/**
 * Remove the label with the specified key.
 * @param key - key of label.
 *
 */
void
GiftiLabelTable::deleteLabel(const int32_t key)
{
   if (key == 0)
   {//key 0 is reserved (sort of)
      CaretLogWarning("Label 0 DELETED!");
   }
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        GiftiLabel* gl = iter->second;
        this->labelsMap.erase(iter);
        delete gl;
        
        setModified();
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
   if (label->getKey() == 0)
   {//key 0 is reserved (sort of)
      CaretLogWarning("Label 0 DELETED!");
   }
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        if (iter->second == label) {
            this->labelsMap.erase(iter);
            setModified();
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
    int32_t unassignedKey = getUnassignedLabelKey();
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        int32_t key = iter->first;
        GiftiLabel* gl = iter->second;
        if (key == unassignedKey || usedLabelKeys.find(key) != usedLabelKeys.end()) {//unassigned key gets a free pass
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
    if (key == GiftiLabel::getInvalidLabelKey()) {
        key = this->generateUnusedKey();
        label->setKey(key);
    }
    if (key == 0)
    {//key 0 is reserved (sort of)
        issueLabelKeyZeroWarning(label->getName());
    }
    
    /*
     * Note: A map DOES NOT replace an existing key, so it
     * must be deleted and then added.
     */
    LABELS_MAP_ITERATOR keyPos = this->labelsMap.find(label->getKey());
    if (keyPos != this->labelsMap.end()) {
        GiftiLabel* gl = keyPos->second;
        this->labelsMap.erase(keyPos);
        delete gl;
    }
        
    this->labelsMap.insert(std::make_pair(label->getKey(), label));
    this->setModified();
}

/**
 * Get the key of a lable from its name.
 * @param name   Name to search for.
 * @return       Key of Name or GiftiLabel::getInvalidLabelKey() if not found.
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
    return GiftiLabel::getInvalidLabelKey();
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
 * Get a GIFTI Label from its name.
 * @param labelName - Name of label that is sought.
 * @return  Reference to label with name or null if no matching label.
 *
 */
GiftiLabel*
GiftiLabelTable::getLabel(const AString& labelName)
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
    int32_t bestMatchLength = -1;
    
    LABELS_MAP newMap;
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        GiftiLabel* gl = iter->second;
        AString labelName = gl->getName();
        if (name.startsWith(labelName)) {
            const int32_t len = labelName.length();
            if (len > bestMatchLength) {
                bestMatchLength = len;
                bestMatchingLabel = iter->second;
            }
        }
    }
    return bestMatchingLabel;
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
 * Get the GiftiLabel at the specified key.
 *
 * @param  key - Key of GiftiLabel entry.
 * @return       The GiftiLabel at the specified key or null if the
 *    there is not a label at the specified key.
 */
GiftiLabel*
GiftiLabelTable::getLabel(const int32_t key)
{
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        return iter->second;
    }
    return NULL;
}

/**
 * Get the key for the unassigned label.
 * @return  Index of key for unassigned label.
 *          A valid key will always be returned.
 *
 */
int32_t
GiftiLabelTable::getUnassignedLabelKey() const
{
    const GiftiLabel* gl = this->getLabel("???");
    if (gl != NULL) {
        return gl->getKey();
    }

    /*
     * Remove 'constness' from this object so that the 
     * label can be added.
     */
    GiftiLabelTable* glt = (GiftiLabelTable*)this;
    const int32_t key = glt->addLabel("???", 0.0f, 0.0f, 0.0f, 0.0f);
    return key;
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
    if (key == 0)
    {
        if (name != "???") {
            issueLabelKeyZeroWarning(name);
//            CaretLogWarning("Label 0 modified!");
        }
    }
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
    if (key == 0)
    {
        if (name != "???")
        {
            issueLabelKeyZeroWarning(name);
//            CaretLogWarning("Label 0 modified!");
        }
    }
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
    if (key == 0)
    {
        if (name != "???")
        {
            issueLabelKeyZeroWarning(name);
            //CaretLogWarning("Label 0 modified!");
        }
    }
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
    if (key == 0)
    {
        CaretLogFiner("Label 0 color changed");
    }
    LABELS_MAP_ITERATOR iter = this->labelsMap.find(key);
    if (iter != this->labelsMap.end()) {
        GiftiLabel* gl = iter->second;
        gl->setColor(color);
    }
}

/**
 * Get the label keys sorted by label name.
 *
 * @return  Vector containing label keys sorted by name.
 *
 */
std::vector<int32_t>
GiftiLabelTable::getLabelKeysSortedByName() const
{
    /*
     * Use map to sort by name
     * If AStringNaturalComparison crashes, temporarily remove
     * as the third template parameter.
     */
    std::map<AString, int32_t, AStringNaturalComparison> nameToKeyMap;
    
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        nameToKeyMap.insert(std::make_pair(iter->second->getName(),
                                           iter->first));
    }
    
    std::vector<int32_t> keysSortedByName;
    for (std::map<AString, int32_t, AStringNaturalComparison>::iterator iter = nameToKeyMap.begin();
         iter != nameToKeyMap.end();
         iter++) {
        keysSortedByName.push_back(iter->second);
    }
    
    return keysSortedByName;
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
 * @return True if this label table contains a label with the
 * name of the medial wall, else false.
 */
bool
GiftiLabelTable::hasMedialWallLabel() const
{
    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        const GiftiLabel* label = iter->second;
        CaretAssert(label);
        if (label->isMedialWallName()) {
            return true;
        }
    }
    
    return false;
}

///**
// * @return Are there any labels that have an invalid group/name
// * hierarchy settings.  This can be caused by changing the name
// * of a label or its color.
// */
//bool
//GiftiLabelTable::hasLabelsWithInvalidGroupNameHierarchy() const
//{
//    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
//         iter != this->labelsMap.end();
//         iter++) {
//        if (iter != this->labelsMap.end()) {
//            GiftiLabel* gl = iter->second;
//            if (gl->getGroupNameSelectionItem() == NULL) {
//                return true;
//                break;
//            }
//        }
//    }
//    
//    return false;
//}

/**
 * Remove labels that have the 'count' attribute
 * set to zero.
 * Note the ??? label is not removed.
 */
void 
GiftiLabelTable::removeLabelsWithZeroCounts()
{
    
    const int32_t unknownKey = getUnassignedLabelKey();
    
    /**
     * First, iterate through the map to find
     * labels that have the 'count' attribute
     * set to zero.  Delete the label and save 
     * the key since one cannot erase the map
     * element without confusing the iterator
     * and causing a crash.
     */
    std::vector<int32_t> unusedkeys;
    for (LABELS_MAP_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        const GiftiLabel* gl = iter->second;
        if (gl->getCount() <= 0) {
            /*
             * Get key and save it.
             */
            const int32_t key = iter->first;
            if (key != unknownKey) {
                unusedkeys.push_back(key);
                
                /*
                 * Delete the label.
                 */
                delete gl;
                iter->second = NULL;
            }
        }
    }

    /*
     * Now, remove all of the elements in the
     * map for the keys that were found in the
     * previous loop.
     */
    bool isLabelRemoved = false;
    for (std::vector<int32_t>::iterator iter = unusedkeys.begin();
         iter != unusedkeys.end();
         iter++) {
        const int32_t key = *iter;
        this->labelsMap.erase(key);
        isLabelRemoved = true;
    }
    
    if (isLabelRemoved) {
        this->setModified();
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
GiftiLabelTable::writeAsXML(XmlWriter& xmlWriter)
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
                
                float rgba[4];
                label->getColor(rgba);
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

void
GiftiLabelTable::writeAsXML(QXmlStreamWriter& xmlWriter) const
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
                xmlWriter.writeStartElement(GiftiXmlElements::TAG_LABEL);
                xmlWriter.writeAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_KEY,
                                        AString::number(key));
                
                float rgba[4];
                label->getColor(rgba);
                xmlWriter.writeAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_RED,
                                        AString::number(rgba[0]));
                xmlWriter.writeAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_GREEN,
                                        AString::number(rgba[1]));
                xmlWriter.writeAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_BLUE,
                                        AString::number(rgba[2]));
                xmlWriter.writeAttribute(GiftiXmlElements::ATTRIBUTE_LABEL_ALPHA,
                                        AString::number(rgba[3]));
                xmlWriter.writeCharacters(label->getName());
                xmlWriter.writeEndElement();
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
GiftiLabelTable::toFormattedString(const AString& indentation) const
{
    std::set<int32_t> allKeys = getKeys();
    const int32_t numberOfKeys = allKeys.size();
    
    if (numberOfKeys <= 0) {
        return "Empty";
    }
    
    int32_t columnCounter = 0;
    const int32_t COL_INDENT = columnCounter++;
    const int32_t COL_KEY    = columnCounter++;
    const int32_t COL_NAME   = columnCounter++;
    const int32_t COL_RED    = columnCounter++;
    const int32_t COL_GREEN  = columnCounter++;
    const int32_t COL_BLUE   = columnCounter++;
    const int32_t COL_ALPHA  = columnCounter++;
    
    const int32_t numberOfTableRows = numberOfKeys + 1;
    StringTableModel table(numberOfTableRows,
                           columnCounter);
    
    int32_t rowIndex = 0;
    table.setElement(rowIndex,
                     COL_INDENT, // accomplishes indentation
                     indentation);
    table.setElement(rowIndex,
                     COL_KEY,
                     "KEY");
    table.setElement(rowIndex,
                     COL_NAME,
                     "NAME");
    table.setElement(rowIndex,
                     COL_RED,
                     "RED");
    table.setElement(rowIndex,
                     COL_GREEN,
                     "GREEN");
    table.setElement(rowIndex,
                     COL_BLUE,
                     "BLUE");
    table.setElement(rowIndex,
                     COL_ALPHA,
                     "ALPHA");
    
    rowIndex++;
    
    table.setColumnAlignment(COL_KEY, StringTableModel::ALIGN_RIGHT);
    table.setColumnAlignment(COL_NAME, StringTableModel::ALIGN_LEFT);
    table.setColumnAlignment(COL_RED, StringTableModel::ALIGN_RIGHT);
    table.setColumnAlignment(COL_GREEN, StringTableModel::ALIGN_RIGHT);
    table.setColumnAlignment(COL_BLUE, StringTableModel::ALIGN_RIGHT);
    table.setColumnAlignment(COL_ALPHA, StringTableModel::ALIGN_RIGHT);
    
    for (std::set<int32_t>::iterator iter = allKeys.begin();
         iter != allKeys.end();
         iter++) {
        const int32_t key = *iter;
        const GiftiLabel* label = getLabel(key);
        
        CaretAssertArrayIndex("table",
                              numberOfTableRows,
                              rowIndex);
        
        table.setElement(rowIndex, COL_KEY, key);
        if (label != NULL) {
            table.setElement(rowIndex, COL_NAME, label->getName());
            table.setElement(rowIndex, COL_RED, label->getRed());
            table.setElement(rowIndex, COL_GREEN, label->getGreen());
            table.setElement(rowIndex, COL_BLUE, label->getBlue());
            table.setElement(rowIndex, COL_ALPHA, label->getAlpha());
        }
        else {
            table.setElement(rowIndex, COL_NAME, "Label Missing");
        }
        
        rowIndex++;
    }
    
    return table.getInString();
}

/**
 * Read the label table from XML DOM structures.
 * @param rootNode - the LabelTable node.
 * @throws GiftiException if an error occurs.
 *
 *
void
GiftiLabelTable::readFromXMLDOM(const Node* rootNode)
           
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
GiftiLabelTable::readFromXmlString(const AString& /*s*/)
           
{
    CaretAssertMessage(0, "Not implemented yet!");
}

void GiftiLabelTable::readFromQXmlStreamReader(QXmlStreamReader& xml)
{
    clear();
    bool haveUnassigned = false;//because clear() creates the default "???" label
    if (!xml.isStartElement() || xml.name() != GiftiXmlElements::TAG_LABEL_TABLE)
    {//TODO: try to recover instead of erroring?
        xml.raiseError("tried to read GiftiLabelTable when current element is not " + GiftiXmlElements::TAG_LABEL_TABLE);
        return;
    }
    while (xml.readNextStartElement() && !xml.atEnd())
    {
        if (xml.name() != GiftiXmlElements::TAG_LABEL)
        {
            xml.raiseError("unexpected element '" + xml.name().toString() + "' encountered in " + GiftiXmlElements::TAG_LABEL_TABLE);
        }
        int key;
        float rgba[4];
        QXmlStreamAttributes myAttrs = xml.attributes();
        bool ok = false;
        QString temp = myAttrs.value(GiftiXmlElements::ATTRIBUTE_LABEL_KEY).toString();
        key = temp.toInt(&ok);
        if (!ok) xml.raiseError("Key attribute of Label missing or noninteger");
        temp = myAttrs.value(GiftiXmlElements::ATTRIBUTE_LABEL_RED).toString();
        rgba[0] = temp.toFloat(&ok);
        if (!ok) xml.raiseError("Red attribute of Label missing or not a number");
        temp = myAttrs.value(GiftiXmlElements::ATTRIBUTE_LABEL_GREEN).toString();
        rgba[1] = temp.toFloat(&ok);
        if (!ok) xml.raiseError("Green attribute of Label missing or not a number");
        temp = myAttrs.value(GiftiXmlElements::ATTRIBUTE_LABEL_BLUE).toString();
        rgba[2] = temp.toFloat(&ok);
        if (!ok) xml.raiseError("Blue attribute of Label missing or not a number");
        temp = myAttrs.value(GiftiXmlElements::ATTRIBUTE_LABEL_ALPHA).toString();
        if (temp == "")
        {
            rgba[3] = 1.0f;
        } else {
            rgba[3] = temp.toFloat(&ok);
            if (!ok) xml.raiseError("Alpha attribute of Label not a number");
        }
        temp = xml.readElementText();
        if (xml.hasError()) return;
        if ((temp == "unknown" || temp == "Unknown") && rgba[3] == 0.0f)
        {
            if (haveUnassigned)
            {
                CaretLogWarning("found multiple label elements that should be interpreted as unlabeled");
            } else {
                CaretLogFiner("Using '" + temp + "' label as unlabeled key");
                haveUnassigned = true;
                temp = "???";//pretend they are actually our internal unlabeled name
            }
        } else if (temp == "???") {
            if (haveUnassigned)
            {
                CaretLogWarning("found multiple label elements that should be interpreted as unlabeled");
            }
            haveUnassigned = true;
        }
        setLabel(key, temp, rgba[0], rgba[1], rgba[2], rgba[3]);
    }
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

    for (LABELS_MAP_CONST_ITERATOR iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        iter->second->clearModified();
    }
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

void GiftiLabelTable::getKeys(std::vector<int32_t>& keysOut) const
{
    keysOut.reserve(labelsMap.size());
    for (std::map<int32_t,GiftiLabel*>::const_iterator iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        keysOut.push_back(iter->first);
    }
}

/**
 * Get all keys and names.
 * 
 * @parm keysAndNamesOut
 *     Map containing the pairs of corresponding keys and names.
 */
void
GiftiLabelTable::getKeysAndNames(std::map<int32_t, AString>& keysAndNamesOut) const
{
    keysAndNamesOut.clear();
    
    for (std::map<int32_t,GiftiLabel*>::const_iterator iter = this->labelsMap.begin();
         iter != this->labelsMap.end();
         iter++) {
        const GiftiLabel* gl = iter->second;
        keysAndNamesOut.insert(std::make_pair(iter->first,
                                              gl->getName()));
    }
}

/**
 * Change the key of a label from 'currentKey' to 'newKey'.
 * If a label exists with 'newKey', the label with 'newKey' is removed.
 *
 * @param currentKey
 *     Key currently used by the label.
 * @param newKey
 *     New key for the label.
 */
void
GiftiLabelTable::changeLabelKey(const int32_t currentKey,
                                const int32_t newKey)
{
    /*
     * Remove a label that uses 'newKey'.
     */
    if (this->labelsMap.find(newKey) != this->labelsMap.end()) {
        deleteLabel(newKey);
    }
    
    /*
     * Get the label with 'currentKey' and remove it from the map.
     */
    LABELS_MAP_ITERATOR currentLabelIter = this->labelsMap.find(currentKey);
    if (currentLabelIter == this->labelsMap.end()) {
        CaretLogSevere("Attempting to change label key for non-existent label with key="
                       + AString::number(currentKey));
        return;
    }
    GiftiLabel* label = currentLabelIter->second;
    this->labelsMap.erase(currentKey);
    
    /*
     * Change the lable's key from 'currentKey' to 'newKey'
     * and add the label into the label's map.
     */
    label->setKey(newKey);
    this->labelsMap.insert(std::make_pair(newKey,
                                          label));
}


bool GiftiLabelTable::matches(const GiftiLabelTable& rhs, const bool checkColors, const bool checkCoords) const
{
    if (labelsMap.size() != rhs.labelsMap.size()) return false;
    for (LABELS_MAP::const_iterator iter = labelsMap.begin(); iter != labelsMap.end(); ++iter)
    {
        LABELS_MAP::const_iterator riter = rhs.labelsMap.find(iter->first);
        if (riter == rhs.labelsMap.end()) return false;
        if (!iter->second->matches(*(riter->second), checkColors, checkCoords)) return false;
    }
    return true;
}

/**
 * Called when label key zero's name is changed.
 * May result in a logger message is name is not a preferred name
 * for the label with key zero.
 *
 * @param name
 *    New name for label with key zero.
 */
void
GiftiLabelTable::issueLabelKeyZeroWarning(const AString& name) const
{
    if ((name != "???")
        && (name.toLower() != "unknown")) {
        CaretLogFine("Label with key=0 overridden with name \""
                        + name
                        + "\".  This label is typically \"???\" or \"unknown\".");
    }
}

/**
 * Export the content of the GIFTI Label Table to a Caret5 Color File.
 */
void
GiftiLabelTable::exportToCaret5ColorFile(const AString& filename) const
{
    if (filename.isEmpty()) {
        throw GiftiException("Missing filename for export of label table to caret5 color file format.");
    }
    
    QFile file(filename);
    if ( ! file.open(QFile::WriteOnly)) {
        const AString msg = ("Unable to open "
                             + filename
                             + " for export of label table to caret5 color file format.\n"
                             + file.errorString());
        throw GiftiException(msg);
    }
    
    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument("1.0");
    xmlWriter.writeStartElement("Border_Color_File");
    
    xmlWriter.writeStartElement("FileHeader");
    xmlWriter.writeStartElement("Element");
    xmlWriter.writeTextElement("comment",
                               "Exported from Caret7/Workbench");
    xmlWriter.writeEndElement();
    xmlWriter.writeEndElement();
    
    std::set<int32_t> keys = this->getKeys();
    for (std::set<int32_t>::const_iterator iter = keys.begin();
         iter != keys.end();
         iter++) {
        int key = *iter;
        const GiftiLabel* label = this->getLabel(key);
        
        if (label != NULL) {
            xmlWriter.writeStartElement("Color");
            xmlWriter.writeTextElement("name", label->getName());

            const int32_t red   = static_cast<int32_t>(label->getRed() * 255.0);
            const int32_t green = static_cast<int32_t>(label->getGreen() * 255.0);
            const int32_t blue  = static_cast<int32_t>(label->getBlue() * 255.0);
            const int32_t alpha = static_cast<int32_t>(label->getAlpha() * 255.0);
            
            xmlWriter.writeTextElement("red", AString::number(red));
            xmlWriter.writeTextElement("green", AString::number(green));
            xmlWriter.writeTextElement("blue", AString::number(blue));
            xmlWriter.writeTextElement("alpha", AString::number(alpha));
            
            xmlWriter.writeTextElement("pointSize", "1.5");
            xmlWriter.writeTextElement("lineSize", "1.0");
            xmlWriter.writeTextElement("symbol", "POINT");
            xmlWriter.writeTextElement("sumscolorid", "");
            xmlWriter.writeEndElement();
        }
    }
    
    xmlWriter.writeEndElement();
    xmlWriter.writeEndDocument();
    
    file.close();
}



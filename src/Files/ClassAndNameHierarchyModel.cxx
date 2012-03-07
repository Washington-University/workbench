
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

#define __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__
#include "ClassAndNameHierarchyModel.h"
#undef __CLASS_AND_NAME_HIERARCHY_MODEL_DECLARE__

#include "Border.h"
#include "BorderFile.h"
#include "CaretAssert.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"

using namespace caret;


    
/**
 * \class caret::ClassAndNameHierarchySelection 
 * \brief Maintains a 'class' and 'name' hierarchy for selection.
 *
 * Each class maps to one or more names.  Identical names
 * may be children of more than one class.  However, each
 * class holds its child names independently from the children
 * of all other classes.  
 *
 * Note: the contains used in this class and GiftiLabelTable
 * use 'maps'.  A map is typically constructed using a 
 * balanced tree so retrieval can be fast.  However, adding
 * or removing items may be slow due to tree rebalancing.  
 * As a result, unused classes and children names are only 
 * removed when the entire instance is cleared (via clear())
 * or by calling removeUnusedNamesAndClasses().  
 *
 * Each class or name is stored using a GiftiLabel that
 * supports a 'count'.  If the for a class or name is zero,
 * that indicates that the item is unused.
 */

/**
 * Constructor.
 */
ClassAndNameHierarchyModel::ClassAndNameHierarchyModel()
: CaretObject()
{
    this->classLabelTable = new GiftiLabelTable();
    this->clear();
    this->selectionStatus = true;
}

/**
 * Destructor.
 */
ClassAndNameHierarchyModel::~ClassAndNameHierarchyModel()
{
    this->clearPrivate(true);
    delete this->classLabelTable;
}

/**
 * Clear the class/name hierarchy.
 */
void 
ClassAndNameHierarchyModel::clear()
{
    this->clearPrivate(false);
}

/**
 * Clear the class/name hierarchy.
 * @param isDestruction
 *    true if in the process of descontructing this instance.
 */
void 
ClassAndNameHierarchyModel::clearPrivate(const bool isDestruction)
{
    this->classLabelTable->clear();
    
    for (std::map<int32_t, GiftiLabelTable*>::iterator iter = this->classKeyToChildNamesMap.begin();
         iter != this->classKeyToChildNamesMap.end();
         iter++) {
        GiftiLabelTable* glt = iter->second;
        CaretAssert(glt);
        delete glt;
    }
    this->classKeyToChildNamesMap.clear();
    
    if (isDestruction == false) {
        /*
         * A label table always has an entry for an unknown label
         * so add name tables to any labels that are present.
         */
        const std::set<int32_t> existingKeys = this->classLabelTable->getKeys();
        for (std::set<int32_t>::const_iterator iter = existingKeys.begin();
             iter != existingKeys.end();
             iter++) {
            const int32_t classKey = *iter;
            GiftiLabelTable* nameLabelTable = new GiftiLabelTable();
            this->classKeyToChildNamesMap.insert(std::make_pair(classKey, nameLabelTable));
        }
    }
    
    /*
     * Note: These members are not cleared.
     */
    //this->name = "";
}

/**
 * @return The class table.
 */
GiftiLabelTable* 
ClassAndNameHierarchyModel::getClassLabelTable()
{
    return this->classLabelTable;
}

/**
 * @return The class table.
 */
const GiftiLabelTable* 
ClassAndNameHierarchyModel::getClassLabelTable() const
{
    return this->classLabelTable;
}

/**
 * Get the names table for a class with the given key.
 * @param classKey
 *    Key for class.
 * @return LabelTable containing names for class.
 */
GiftiLabelTable* 
ClassAndNameHierarchyModel::getNameLabelTableForClass(const int32_t classKey)
{
    std::map<int32_t, GiftiLabelTable*>::iterator iter = this->classKeyToChildNamesMap.find(classKey);
    if (iter != this->classKeyToChildNamesMap.end()) {
        return iter->second;
    }
    CaretAssertMessage(0, ("Label Table for class with key "
                           + AString::number(classKey)
                           + " is missing."));
    return NULL;
}

/**
 * Get the names table for a class with the given key.
 * @param classKey
 *    Key for class.
 * @return LabelTable containing names for class.
 */
const GiftiLabelTable* 
ClassAndNameHierarchyModel::getNameLabelTableForClass(const int32_t classKey) const
{
    const std::map<int32_t, GiftiLabelTable*>::const_iterator iter = this->classKeyToChildNamesMap.find(classKey);
    if (iter != this->classKeyToChildNamesMap.end()) {
        return iter->second;
    }
    CaretAssertMessage(0, ("Label Table for class with key "
                           + AString::number(classKey)
                           + " is missing."));
    return NULL;
}

/**
 * Set the selected status for EVERYTHING.
 * @param status
 *    The selection status.
 */
void 
ClassAndNameHierarchyModel::setAllSelected(const bool status)
{
    this->classLabelTable->setSelectionStatusForAllLabels(status);
    for (std::map<int32_t, GiftiLabelTable*>::iterator iter = this->classKeyToChildNamesMap.begin();
         iter != this->classKeyToChildNamesMap.end();
         iter++) {
        GiftiLabelTable* glt = iter->second;
        CaretAssert(glt);
        glt->setSelectionStatusForAllLabels(status);
    }
}

/**
 * Update this class hierarchy with the border names
 * and classes.
 *
 * NOTE: Both the classes and names are stored 
 * in a GIFTI label table which use a map that 
 * maps an integer key to a string name.  Because
 * insertion/removal are slow, do not clear the
 * LabelTable when an update is needed.  Instead,
 * use the count attribute that is associated with
 * each label.  A count greater than zero indicates
 * that the class or names is valid.  Plus, color
 * components are associated with classes and
 * thus they must not be deleted.
 */
void 
ClassAndNameHierarchyModel::update(BorderFile* borderFile)
{
    bool needToGenerateKeys = false;
    
    this->name = borderFile->getFileNameNoPath();
    
    const int32_t numBorders = borderFile->getNumberOfBorders();
    for (int32_t i = 0; i < numBorders; i++) {
        const Border* border = borderFile->getBorder(i);
        if (border->isSelectionClassOrNameModified()) {
            needToGenerateKeys = true;
        }
    }
    
    if (needToGenerateKeys) {
        /*
         * Key and name for unknown class
         */
        const int32_t unknownClassKey = this->classLabelTable->getUnassignedLabelKey();
        CaretAssert(unknownClassKey >= 0);
        const GiftiLabel* unknownLabel = this->classLabelTable->getLabel(unknownClassKey);
        CaretAssert(unknownLabel);
        const AString unknownClassName = unknownLabel->getName();
        
        /*
         * Reset the counts for all class and children names.
         */
        this->classLabelTable->resetLabelCounts();
        for (std::map<int32_t, GiftiLabelTable*>::iterator iter = this->classKeyToChildNamesMap.begin();
             iter != this->classKeyToChildNamesMap.end();
             iter++) {
            GiftiLabelTable* glt = iter->second;
            CaretAssert(glt);
            glt->resetLabelCounts();
        }
        
        /*
         * Update with all borders.
         */
        for (int32_t i = 0; i < numBorders; i++) {
            Border* border = borderFile->getBorder(i);
            
            /*
             * Get the class.  If it is empty, use the default name.
             */
            AString theClassName = border->getClassName();
            if (theClassName.isEmpty()) {
                theClassName = unknownClassName;
            }
            
            /*
             * If the class name exists, it will have a valid key.  If not
             * create a new label for the class name.
             */
            int32_t classKey = this->classLabelTable->getLabelKeyFromName(theClassName);
            if (classKey < 0) {
                classKey = this->classLabelTable->addLabel(theClassName, 0.0f, 0.0f, 0.0f);
            }
            CaretAssert(classKey >= 0);
            
            GiftiLabel* classLabel = this->classLabelTable->getLabel(classKey);
            CaretAssert(classLabel);
            classLabel->incrementCount(); // indicates usage
            
            /*
             * Look for the label table containing names that are children of the class.
             * If not found, create and add.
             */
            GiftiLabelTable* nameLabelTable = NULL;
            std::map<int32_t, GiftiLabelTable*>::iterator iter = this->classKeyToChildNamesMap.find(classKey);
            if (iter != this->classKeyToChildNamesMap.end()) {
                nameLabelTable = iter->second;
            }
            else {
                nameLabelTable = new GiftiLabelTable();
                this->classKeyToChildNamesMap.insert(std::make_pair(classKey, nameLabelTable));
            }
            CaretAssert(nameLabelTable);

            /*
             * If the name already exists, addLabel will return the name's key.
             * Otherwise, it will return the key of the existing label with the name.
             */
            const int32_t nameKey = nameLabelTable->addLabel(border->getName(),
                                                             0.0f, 0.0f, 0.0f);
            CaretAssert(nameKey >= 0);
            GiftiLabel* nameLabel = nameLabelTable->getLabel(nameKey);
            CaretAssert(nameLabel);
            nameLabel->incrementCount(); // indicates usage
            
            /*
             * Update keys used by the border.
             */
            border->setSelectionClassAndNameKeys(classKey,
                                                 nameKey);
        }
    }
}

/**
 * Remove any unused names and classes.
 * @param borderFile
 *    Border file that contains names and classes.
 */
void 
ClassAndNameHierarchyModel::removeUnusedNamesAndClasses(BorderFile* borderFile)
{
    /*
     * Update with latest data.
     */ 
    this->update(borderFile);
    
    /*
     * Remove unused classes.
     */
    this->classLabelTable->removeLabelsWithZeroCounts();
    const std::set<int32_t> validClassKeys = this->classLabelTable->getKeys();
    
    /*
     * Find keys in class to child names map that
     * are not in the class table.  Also remove
     * names no longer needed.
     */
    std::vector<int32_t> missingClassKeys;
    for (std::map<int32_t, GiftiLabelTable*>::iterator iter = this->classKeyToChildNamesMap.begin();
         iter != this->classKeyToChildNamesMap.end();
         iter++) {
        GiftiLabelTable* glt = iter->second;
        
        /*
         * Remove unused names.
         */
        glt->removeLabelsWithZeroCounts();
        
        /*
         * Determine if key to classes is still valid.
         */
        const int32_t classKey = iter->first;
        if (validClassKeys.find(classKey) == validClassKeys.end()) {
            /*
             * Set key of item to remove.
             */
            missingClassKeys.push_back(classKey);
            
            /*
             * Delete the name label table.
             */
            delete glt;
        }
    }
    
    /*
     * Remove name label tables for classes that no longer exist.
     */
    for (std::vector<int32_t>::iterator iter = missingClassKeys.begin();
         iter != missingClassKeys.end();
         iter++) {
        const int32_t classKey = *iter;
        this->classKeyToChildNamesMap.erase(classKey);
    }
}

/**
 * Is the class valid?  Valid if class has at least one child
 * with a count attribute greater than zero (it is used).
 * @return true if class is valid, else false.
 */
bool 
ClassAndNameHierarchyModel::isClassValid(const int32_t classKey) const
{
    const GiftiLabelTable* nameLabelTable = this->getNameLabelTableForClass(classKey);
    if (nameLabelTable == NULL) {
        return false;
    }
    
    const std::set<int32_t> nameKeys = nameLabelTable->getKeys();
    for (std::set<int32_t>::const_iterator iter = nameKeys.begin();
         iter != nameKeys.end();
         iter++) {
        int32_t key = *iter;
        
        const GiftiLabel* gl = nameLabelTable->getLabel(key);
        CaretAssert(gl);
        if (gl->getCount() > 0) {
            return true;
        }
    }
    
    return false;
}


/**
 * @return A string containing a description
 * of the contents of this class and name
 * hierarchy.
 */
AString 
ClassAndNameHierarchyModel::toString() const
{
    AString text;
    text.reserve(10000);
    
    const std::vector<int32_t> classKeys = this->classLabelTable->getLabelKeysSortedByName();
    for (std::vector<int32_t>::const_iterator iter = classKeys.begin();
         iter != classKeys.end();
         iter++) {
        int32_t key = *iter;
        
        const GiftiLabel* gl = this->classLabelTable->getLabel(key);
        CaretAssert(gl);
        const AString theClass = gl->getName();
        text += ("Class Key/Name/Count for "
                 + this->name
                 + ": " 
                 + AString::number(key)
                 + " "
                 + theClass
                 + " "
                 + AString::number(gl->getCount())
                 + "\n");
        
        const GiftiLabelTable* nameLabelTable = this->getNameLabelTableForClass(key);
        CaretAssert(nameLabelTable);
        const std::vector<int32_t> nameKeys = nameLabelTable->getLabelKeysSortedByName();
        for (std::vector<int32_t>::const_iterator iter = nameKeys.begin();
             iter != nameKeys.end();
             iter++) {
            int32_t key = *iter;
            
            const GiftiLabel* gl = nameLabelTable->getLabel(key);
            CaretAssert(gl);
            const AString theName = gl->getName();
            text += ("   Key/Name/Count: " 
                     + AString::number(key)
                     + " "
                     + theName
                     + " "
                     + AString::number(gl->getCount())
                     + "\n");
        }
        
        text += "\n";
    }
    
    return text;
}

/**
 * @return  Name of this model.
 */
AString 
ClassAndNameHierarchyModel::getName() const
{
    return this->name;
}

/**
 * @return Is this hierarchy selected?
 */
bool 
ClassAndNameHierarchyModel::isSelected() const
{
    return this->selectionStatus;
}

/**
 * Set the selection status of this hierarchy model.
 * @param selectionStatus
 *    New selection status.
 */
void 
ClassAndNameHierarchyModel::setSelected(const bool selectionStatus)
{
    this->selectionStatus = selectionStatus;
}



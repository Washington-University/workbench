
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
#include "FociFile.h"
#include "Focus.h"
#include "GiftiLabelTable.h"
#include "LabelFile.h"

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
 * Note: Two containers are used to hold the data.  One,
 * a vector, maps class keys to classes and a second, a map
 * class names to classes.  
 * use 'maps'.  A map is typically constructed using a 
 * balanced tree so retrieval can be fast.  However, adding
 * or removing items may be slow due to tree rebalancing.  
 * As a result, unused classes and children names are only 
 * removed when the entire instance is cleared (via clear())
 * or by calling removeUnusedNamesAndClasses().  
 *
 * Each class or name supports a 'count'.  If the for a class 
 * or name is zero, that indicates that the item is unused.
 * 
 * Attributes are available for every tab and also a 
 * few 'display groups'.  A number of methods in this class accept 
 * both display group and tab index parameters.  When the display 
 * group is set to 'Tab', the tab index is used meaning that the
 * attribute requeted/sent is for use with a specifc tab.  For an
 * other display group value, the attribute is for a display group
 * and the tab index is ignored.
 */

/**
 * Constructor.
 */
ClassAndNameHierarchyModel::ClassAndNameHierarchyModel()
: CaretObject()
{
    this->clear();
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        this->selectionStatusInDisplayGroup[i] = true;
        this->expandedStatusInDisplayGroup[i] = true;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->selectionStatusInTab[i] = true;
        this->expandedStatusInTab[i] = true;
    }
}

/**
 * Destructor.
 */
ClassAndNameHierarchyModel::~ClassAndNameHierarchyModel()
{
    this->clear();
}

/**
 * Clear the class/name hierarchy.
 */
void 
ClassAndNameHierarchyModel::clear()
{
    /** 
     * While both maps point to NameDisplayGroupSelectors, both maps point
     * to the same selectors so only need to delete them one time.
     */
    for (std::vector<ClassDisplayGroupSelector*>::iterator iter = this->keyToClassNameSelectorVector.begin();
         iter != this->keyToClassNameSelectorVector.end();
         iter++) {
        ClassDisplayGroupSelector* cs = *iter;
        if (cs != NULL) {
            delete cs;
        }
    }
    
    this->keyToClassNameSelectorVector.clear();
    this->classNameToClassSelectorMap.clear();
    
    this->availableClassKeys.clear();        
}

/**
 * Copy the class and names selections from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which selections are copied.
 * @param targetTabIndex
 *    Index of tab to which selections are copied.
 */
void 
ClassAndNameHierarchyModel::copyClassNameAndHierarchy(const int32_t sourceTabIndex,
                                                      const int32_t targetTabIndex)
{
    this->selectionStatusInTab[targetTabIndex] = this->selectionStatusInTab[sourceTabIndex];
    this->expandedStatusInTab[targetTabIndex]  = this->expandedStatusInTab[sourceTabIndex];
}

/**
 * Set the selected status for EVERYTHING.
 * @param status
 *    The selection status.
 */
void 
ClassAndNameHierarchyModel::setAllSelected(const bool status)
{
    const int32_t numberOfClassKeys = static_cast<int32_t>(this->keyToClassNameSelectorVector.size());
    for (int32_t classKey = 0; classKey < numberOfClassKeys; classKey++) {
        ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[classKey];
        if (cs != NULL) {
            cs->setAllSelected(status);
        }
    }
}

/**
 * Set the selection status of this hierarchy model for the display group/tab.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param selectionStatus
 *    New selection status.
 */
void 
ClassAndNameHierarchyModel::setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const bool selectionStatus)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectionStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectionStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                              tabIndex);
        this->selectionStatusInTab[tabIndex] = selectionStatus;
    }
    else {
        this->selectionStatusInDisplayGroup[displayIndex] = selectionStatus;
    }
    
    for (std::vector<ClassDisplayGroupSelector*>::iterator classIterator = keyToClassNameSelectorVector.begin();
         classIterator != keyToClassNameSelectorVector.end();
         classIterator++) {
        ClassDisplayGroupSelector* classSelector = *classIterator;
        classSelector->setAllSelected(displayGroup,
                                      tabIndex,
                                      selectionStatus);
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
 *
 * @param borderFile
 *    The border file from which classes and names are from.
 * @parm forceUpdate
 *    If true, force an update.
 */
void 
ClassAndNameHierarchyModel::update(BorderFile* borderFile,
                                   const bool forceUpdate)
{
    bool needToGenerateKeys = forceUpdate;
    
    this->name = borderFile->getFileNameNoPath();

    const int32_t numBorders = borderFile->getNumberOfBorders();
    if (needToGenerateKeys == false) {
        for (int32_t i = 0; i < numBorders; i++) {
            const Border* border = borderFile->getBorder(i);
            if (border->isSelectionClassOrNameModified()) {
                needToGenerateKeys = true;
            }
        }
    }
    
    if (needToGenerateKeys) {
        /*
         * Names for missing class names or border names.
         */
        const AString missingClassName = "NoClass";
        const AString missingBorderName = "NoName";
        
        /*
         * Reset the counts for all class and children names.
         */
        const int32_t numberOfClassKeys = static_cast<int32_t>(this->keyToClassNameSelectorVector.size());
        for (int32_t classKey = 0; classKey < numberOfClassKeys; classKey++) {
            ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[classKey];
            if (cs != NULL) {
                cs->clearAllNameCounters();
            }
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
                theClassName = missingClassName;
            }
            
            /*
             * Get the name.
             */
            AString name = border->getName();
            if (name.isEmpty()) {
                name = missingBorderName;
            }
            
            /*
             * Adding border class and name will set the class name keys.
             */
            int32_t classKey = -1;
            int32_t nameKey = -1;
            this->addName(theClassName,
                          name,
                          classKey,
                          nameKey);
            
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
    this->update(borderFile,
                 true);
    
    /*
     * Remove unused classes.
     */
    const int32_t numberOfClasses = static_cast<int32_t>(this->keyToClassNameSelectorVector.size());
    for (int32_t classKey = 0; classKey < numberOfClasses; classKey++) {
        ClassDisplayGroupSelector* classSelector = this->keyToClassNameSelectorVector[classKey];
        if (classSelector != NULL) {
            classSelector->removeNamesWithCountersEqualZero();
            if (classSelector->getNumberOfNamesWithCountersGreaterThanZero() <= 0) {
                for (std::map<AString, ClassDisplayGroupSelector*>::iterator iter = this->classNameToClassSelectorMap.begin();
                     iter != this->classNameToClassSelectorMap.end();
                     iter++) {
                    if (classSelector == iter->second) {
                        this->classNameToClassSelectorMap.erase(iter);
                        break;
                    }
                }
                
                this->availableClassKeys.push_front(classKey);
                delete classSelector;
                this->keyToClassNameSelectorVector[classKey] = NULL;
            }
        }
    }
}

/**
 * Update this class hierarchy with the label names
 * and maps (as class).  Classes and names are done
 * differently for LabelFiles.  Use the map index as
 * the class key and the label index as the name key.
 *
 * @param labelFile
 *    The label file from which classes (map) and names are from.
 * @parm forceUpdate
 *    If true, force an update.
 */
void
ClassAndNameHierarchyModel::update(LabelFile* labelFile,
                                   const bool forceUpdate)
{
    bool needToGenerateKeys = forceUpdate;
    
    this->name = labelFile->getFileNameNoPath();

    if (needToGenerateKeys == false) {
        /*
         * Check to see if any class (map) names have changed.
         */
        const int32_t numClasses = static_cast<int32_t>(this->keyToClassNameSelectorVector.size());
        if (labelFile->getNumberOfMaps()
            != numClasses) {
            needToGenerateKeys = true;
        }
        else {
            for (int32_t i = 0; i < numClasses; i++) {
                ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[i];
                if (cs != NULL) {
                    if (labelFile->getMapName(i) != cs->getName()) {
                        needToGenerateKeys = true;
                        break;
                    }
                }
                else {
                    needToGenerateKeys = true;
                    break;
                }
            }
        }
    }
    
    if (needToGenerateKeys) {
        /*
         * Clear everything
         */
        this->clear();
        
        /*
         * Names for missing class names or foci names.
         */
        const AString missingClassName = "NoClass";
        const AString missingName = "NoName";
        
        /*
         * Update with labels from maps
         */
        const int32_t numMaps = labelFile->getNumberOfMaps();
        for (int32_t iMap = 0; iMap < numMaps; iMap++) {
            /*
             * Get the class.  If it is empty, use the default name.
             */
            AString theClassName = labelFile->getMapName(iMap);
            if (theClassName.isEmpty()) {
                theClassName = missingClassName;
            }
            
            /*
             * Create the class
             */
            ClassDisplayGroupSelectorUserKey* classSelector = new ClassDisplayGroupSelectorUserKey(theClassName,
                                                                                                   iMap);
//            const int32_t numClasses = static_cast<int32_t>(this->keyToClassNameSelectorVector.size());
//            CaretAssert(numClasses == iMap);
//            this->keyToClassNameSelectorVector.push_back(classSelector);

            /*
             * Get indices of labels used in this map
             */
            std::vector<int32_t> labelKeys = labelFile->getUniqueLabelKeysUsedInMap(iMap);
            
            const int32_t numLabelKeys = static_cast<int32_t>(labelKeys.size());
            for (int32_t iLabel = 0; iLabel < numLabelKeys; iLabel++) {
                const int32_t labelKey = labelKeys[iLabel];
                AString labelName = labelFile->getLabelTable()->getLabelName(labelKey);
                if (labelName.isEmpty()) {
                    labelName = missingName;
                }
                
                classSelector->addNameWithKey(labelName,
                                              labelKey);
            }
            
            this->addClass(classSelector);
        }
    }
    
    
//    std::cout << "LABELS: " << qPrintable(this->toString()) << std::endl;
}

/**
 * Update this class hierarchy with the foci names
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
 *
 * @param fociFile
 *    The foci file from which classes and names are from.
 * @parm forceUpdate
 *    If true, force an update.
 */
void 
ClassAndNameHierarchyModel::update(FociFile* fociFile,
                                   const bool forceUpdate)
{
    bool needToGenerateKeys = forceUpdate;
    
    this->name = fociFile->getFileNameNoPath();
    
    const int32_t numFoci = fociFile->getNumberOfFoci();
    if (needToGenerateKeys == false) {
        for (int32_t i = 0; i < numFoci; i++) {
            const Focus* focus = fociFile->getFocus(i);
            if (focus->isSelectionClassOrNameModified()) {
                needToGenerateKeys = true;
            }
        }
    }
    
    if (needToGenerateKeys) {
        /*
         * Names for missing class names or foci names.
         */
        const AString missingClassName = "NoClass";
        const AString missingName = "NoName";
        
        /*
         * Reset the counts for all class and children names.
         */
        const int32_t numberOfClassKeys = static_cast<int32_t>(this->keyToClassNameSelectorVector.size());
        for (int32_t classKey = 0; classKey < numberOfClassKeys; classKey++) {
            ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[classKey];
            if (cs != NULL) {
                cs->clearAllNameCounters();
            }
        }
        
        /*
         * Update with all foci.
         */
        for (int32_t i = 0; i < numFoci; i++) {
            Focus* focus = fociFile->getFocus(i);
            
            /*
             * Get the class.  If it is empty, use the default name.
             */
            AString theClassName = focus->getClassName();
            if (theClassName.isEmpty()) {
                theClassName = missingClassName;
            }
            
            /*
             * Get the name.
             */
            AString name = focus->getName();
            if (name.isEmpty()) {
                name = missingName;
            }
            
            /*
             * Adding focus class and name will set the class name keys.
             */
            int32_t classKey = -1;
            int32_t nameKey = -1;
            this->addName(theClassName,
                          name,
                          classKey,
                          nameKey);
            
            /*
             * Update keys used by the focus.
             */
            focus->setSelectionClassAndNameKeys(classKey,
                                                 nameKey);
        }
    }
}

/**
 * Remove any unused names and classes.
 * @param fociFile
 *    Foci file that contains names and classes.
 */
void 
ClassAndNameHierarchyModel::removeUnusedNamesAndClasses(FociFile* fociFile)
{
    /*
     * Update with latest data.
     */ 
    this->update(fociFile,
                 true);
    
    /*
     * Remove unused classes.
     */
    const int32_t numberOfClasses = static_cast<int32_t>(this->keyToClassNameSelectorVector.size());
    for (int32_t classKey = 0; classKey < numberOfClasses; classKey++) {
        ClassDisplayGroupSelector* classSelector = this->keyToClassNameSelectorVector[classKey];
        if (classSelector != NULL) {
            classSelector->removeNamesWithCountersEqualZero();
            if (classSelector->getNumberOfNamesWithCountersGreaterThanZero() <= 0) {
                for (std::map<AString, ClassDisplayGroupSelector*>::iterator iter = this->classNameToClassSelectorMap.begin();
                     iter != this->classNameToClassSelectorMap.end();
                     iter++) {
                    if (classSelector == iter->second) {
                        this->classNameToClassSelectorMap.erase(iter);
                        break;
                    }
                }
                
                this->availableClassKeys.push_front(classKey);
                delete classSelector;
                this->keyToClassNameSelectorVector[classKey] = NULL;
            }
        }
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
    if ((classKey >= 0) 
        && (classKey < static_cast<int32_t>(this->keyToClassNameSelectorVector.size()))) {
        ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[classKey];
        if (cs != NULL) {
            return true;
        }
    }
    
    return false;
}

/**
 * @return  A vector with all class keys.
 */
std::vector<int32_t> 
ClassAndNameHierarchyModel::getAllClassKeysSortedByName() const
{
    std::vector<int32_t> classKeys;
    
    for (std::map<AString, ClassDisplayGroupSelector*>::const_iterator iter = this->classNameToClassSelectorMap.begin();
         iter != this->classNameToClassSelectorMap.end();
         iter++) {
        const ClassDisplayGroupSelector* cs = iter->second;
        if (cs != NULL) {
            classKeys.push_back(cs->getKey());
        }
    }
    
    return classKeys;
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
    
    text += ("Hierarchy Name: "
             + this->name
             + "\n");
    for (std::map<AString, ClassDisplayGroupSelector*>::const_iterator iter = this->classNameToClassSelectorMap.begin();
         iter != this->classNameToClassSelectorMap.end();
         iter++) {
        const ClassDisplayGroupSelector* cs = iter->second;
        if (cs != NULL) {
            const AString className = cs->getName();
            const int32_t classKey = cs->getKey();
            const int32_t classCount = cs->getCounter();
            
            text += ("   Class Key/Count/Name for "
                     + AString::number(classKey)
                     + ", "
                     + AString::number(classCount)
                     + ": "
                     + className
                     + "\n");
            
            const std::vector<int32_t> allNameKeys = cs->getAllNameKeysSortedByName();
            for (std::vector<int32_t>::const_iterator nameIter = allNameKeys.begin();
                 nameIter != allNameKeys.end();
                 nameIter++) {
                const NameDisplayGroupSelector* ns = cs->getNameSelectorWithKey(*nameIter);
                const AString name = ns->getName();
                const int32_t nameKey = ns->getKey();
                const int32_t nameCount = ns->getCounter();
                text += ("      Key/Count/Name: " 
                         + AString::number(nameKey)
                         + " "
                         + AString::number(nameCount)
                         + " "
                         + name
                         + "\n");
            }
        }
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
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 */
bool 
ClassAndNameHierarchyModel::isSelected(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectionStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectionStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                              tabIndex);
        return this->selectionStatusInTab[tabIndex];
    }
    return this->selectionStatusInDisplayGroup[displayIndex];
}

/**
 * Set the selection status of this hierarchy model.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param selectionStatus
 *    New selection status.
 */
void 
ClassAndNameHierarchyModel::setSelected(const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        const bool selectionStatus)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectionStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectionStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                              tabIndex);
        this->selectionStatusInTab[tabIndex] = selectionStatus;
    }
    else {
        this->selectionStatusInDisplayGroup[displayIndex] = selectionStatus;
    }
}

/**
 * Get the class selector for the given class name.
 * @param className
 *    The key of the desired class selector.
 * @return The class selector for the key or NULL if no class
 *         selector with the given key.
 */
ClassAndNameHierarchyModel::ClassDisplayGroupSelector* 
ClassAndNameHierarchyModel::getClassSelectorForClassName(const AString& className)
{
    ClassDisplayGroupSelector* classSelector = NULL;

    std::map<AString, ClassDisplayGroupSelector*>::iterator iter = this->classNameToClassSelectorMap.find(className);
    if (iter != this->classNameToClassSelectorMap.end()) {
        classSelector = iter->second;
    }
    
    return classSelector;
}

/**
 * Get the class selector for the given class name.
 * @param className
 *    The key of the desired class selector.
 * @return The class selector for the key or NULL if no class
 *         selector with the given key.
 */
const ClassAndNameHierarchyModel::ClassDisplayGroupSelector* 
ClassAndNameHierarchyModel::getClassSelectorForClassName(const AString& className) const
{
    ClassDisplayGroupSelector* classSelector = NULL;
    
    std::map<AString, ClassDisplayGroupSelector*>::const_iterator iter = this->classNameToClassSelectorMap.find(className);
    if (iter != this->classNameToClassSelectorMap.end()) {
        classSelector = iter->second;
    }
    
    return classSelector;
}

/**
 * Get the class selector for the given class key.
 * @param classKey
 *    The key of the desired class selector.
 * @return The class selector for the key or NULL if no class
 *         selector with the given key.
 */
ClassAndNameHierarchyModel::ClassDisplayGroupSelector* 
ClassAndNameHierarchyModel::getClassSelectorForClassKey(const int32_t classKey)
{
    ClassDisplayGroupSelector* classSelector = NULL;
    
    if ((classKey >= 0) 
        && (classKey < static_cast<int32_t>(this->keyToClassNameSelectorVector.size()))) {
        classSelector = this->keyToClassNameSelectorVector[classKey];
    }
    
    return classSelector;
}

/**
 * Get the class selector for the given class key.
 * @param classKey
 *    The key of the desired class selector.
 * @return The class selector for the key or NULL if no class
 *         selector with the given key.
 */
const ClassAndNameHierarchyModel::ClassDisplayGroupSelector* 
ClassAndNameHierarchyModel::getClassSelectorForClassKey(const int32_t classKey) const
{
    ClassDisplayGroupSelector* classSelector = NULL;
    
    if ((classKey >= 0) 
        && (classKey < static_cast<int32_t>(this->keyToClassNameSelectorVector.size()))) {
        classSelector = this->keyToClassNameSelectorVector[classKey];
    }
    
    return classSelector;
}

/**
 * Add a name to its parent class.
 * If the class does not exist for the parent class name, the class is created.
 * If the name is not present in the parent class, the name is added to the class.
 * Keys are returned for both the parent class and the name.
 *
 * @param parentClassName
 *    Name of parent class.
 * @param name
 *    Name to add as child of parent class.
 * @param classKeyOut
 *    OUTPUT containing key for class.
 * @param nameKeyOut
 *    OUTPUT containing key for name in parent class.
 */
void 
ClassAndNameHierarchyModel::addName(const AString& parentClassName,
                                    const AString& name,
                                    int32_t& classKeyOut,
                                    int32_t& nameKeyOut)
{
    ClassDisplayGroupSelectorGeneratedKey* classSelector = NULL;
    std::map<AString, ClassDisplayGroupSelector*>::iterator iter = this->classNameToClassSelectorMap.find(parentClassName);
    if (iter != this->classNameToClassSelectorMap.end()) {
        ClassDisplayGroupSelectorGeneratedKey* cdgk = dynamic_cast<ClassDisplayGroupSelectorGeneratedKey*>(iter->second);
        CaretAssert(cdgk);
        classSelector = cdgk;
    }
    else {
        if (this->availableClassKeys.empty()) {
            const int32_t classKey = static_cast<int32_t>(this->keyToClassNameSelectorVector.size());
            classSelector = new ClassDisplayGroupSelectorGeneratedKey(parentClassName,
                                                          classKey);
            this->keyToClassNameSelectorVector.push_back(classSelector);
        }
        else {
            const int32_t classKey = this->availableClassKeys.front();
            this->availableClassKeys.pop_front();
            CaretAssert(this->keyToClassNameSelectorVector[classKey] == NULL);
            classSelector = new ClassDisplayGroupSelectorGeneratedKey(parentClassName, classKey);
            this->keyToClassNameSelectorVector[classKey] = classSelector;
        }
        this->classNameToClassSelectorMap.insert(std::make_pair(parentClassName, classSelector));
    }
    
    CaretAssert(classSelector);
    classKeyOut = classSelector->getKey();
    CaretAssert(classKeyOut >= 0);
    
    nameKeyOut = classSelector->addName(name);
}

/**
 * Add a class using the key contained in the given class group.
 * @param classDisplayGroup
 *    Class display group that is added using its key.
 */
void
ClassAndNameHierarchyModel::addClass(ClassDisplayGroupSelector* classDisplayGroup)
{
    CaretAssert(classDisplayGroup);
    
    const int32_t key = classDisplayGroup->getKey();
    CaretAssert(key >= 0);
    
    const AString className = classDisplayGroup->getName();
    
    const int32_t numberOfClasses = this->keyToClassNameSelectorVector.size();
    
    if (key < numberOfClasses) {
        if (this->keyToClassNameSelectorVector[key] != NULL) {
            delete this->keyToClassNameSelectorVector[key];
            this->keyToClassNameSelectorVector[key] = NULL;
            this->classNameToClassSelectorMap.erase(className);
        }
    }
    else {
        this->keyToClassNameSelectorVector.resize(key + 1, NULL);
    }
    
    this->keyToClassNameSelectorVector[key] = classDisplayGroup;
    this->classNameToClassSelectorMap.insert(std::make_pair(className,
                                                            classDisplayGroup));
}

/**
 * Is a class selected in the given display group?
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param classKey
 *    Key for class for which selection statis is desired.
 * @return
 *    true if class is selected for the given display group.
 *    false if not selected or key is invalid.
 */
bool 
ClassAndNameHierarchyModel::isClassSelected(const DisplayGroupEnum::Enum displayGroup,
                                            const int32_t tabIndex,
                                            const int32_t classKey) const
{
    bool status = false;
    
    if ((classKey >= 0) 
        && (classKey < static_cast<int32_t>(this->keyToClassNameSelectorVector.size()))) {
        ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[classKey];
        if (cs != NULL) {
            status = cs->isSelected(displayGroup, tabIndex);
        }
        else {
            CaretAssertMessage(0, "No class group for class key="
                               + AString::number(classKey));
        }
    }
    
    return status;
}

/**
 * Set a class selected in the given display group
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param classKey
 *    Key for class for which selection status is set.
 * @param selected
 *    New selection status.
 */
void 
ClassAndNameHierarchyModel::setClassSelected(const DisplayGroupEnum::Enum displayGroup,
                                             const int32_t tabIndex,
                                             const int32_t classKey,
                                             const bool selected)
{
    if ((classKey >= 0) 
        && (classKey < static_cast<int32_t>(this->keyToClassNameSelectorVector.size()))) {
        ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[classKey];
        if (cs != NULL) {
            cs->setSelected(displayGroup, tabIndex, selected);
        }
        else {
            CaretAssertMessage(0, "No class group for class key="
                               + AString::number(classKey));
        }
    }
}

/**
 * Is a name selected in the given parent class and display group?
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param parentClassKey
 *    Key for parent class for which name selection status is desired.
 * @param nameKey
 *    Key for name in parent class for which selection status is desired.
 * @return
 *    true if name is selected for the given class and display group.
 *    false if not selected or either class or name key is invalid.
 */
bool 
ClassAndNameHierarchyModel::isNameSelected(const DisplayGroupEnum::Enum displayGroup,
                                           const int32_t tabIndex,
                                           const int32_t parentClassKey,
                                           const int32_t nameKey) const
{
    bool status = false;
    
    if ((parentClassKey >= 0) 
        && (parentClassKey < static_cast<int32_t>(this->keyToClassNameSelectorVector.size()))) {
        ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[parentClassKey];
        if (cs != NULL) {
            NameDisplayGroupSelector* ns = cs->getNameSelectorWithKey(nameKey);
            if (ns != NULL) {
                status = ns->isSelected(displayGroup, tabIndex);
            }
            else {
                CaretAssertMessage(0, "No name group for name key="
                                   + AString::number(nameKey)
                                   + " for class="
                                   + cs->getName());
            }
        }
        else {
            CaretAssertMessage(0, "No class group for class key="
                               + AString::number(parentClassKey));
        }
    }
    
    return status;
}

/**
 * Set a name selected in the given parent class and display group
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param parentClassKey
 *    Key for parent class for which selection status is set.
 * @param nameKey
 *    Key for name for which selection status is set.
 * @param selected
 *    New selection status.
 */
void ClassAndNameHierarchyModel::setNameSelected(const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex,
                                                 const int32_t parentClassKey,
                                                 const int32_t nameKey,
                                                 const bool selected)
{
    if ((parentClassKey >= 0) 
        && (parentClassKey < static_cast<int32_t>(this->keyToClassNameSelectorVector.size()))) {
        ClassDisplayGroupSelector* cs = this->keyToClassNameSelectorVector[parentClassKey];
        if (cs != NULL) {
            NameDisplayGroupSelector* ns = cs->getNameSelectorWithKey(nameKey);
            if (ns != NULL) {
                ns->setSelected(displayGroup, tabIndex, selected);
            }
            else {
                CaretAssertMessage(0, "No name group for name key="
                                   + AString::number(nameKey)
                                   + " for class="
                                   + cs->getName());
            }
        }
        else {
            CaretAssertMessage(0, "No class group for class key="
                               + AString::number(parentClassKey));
        }
    }
    
}

/**
 * @return The expanded status.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 */
bool 
ClassAndNameHierarchyModel::isExpanded(const DisplayGroupEnum::Enum displayGroup,
                                       const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->expandedStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->expandedStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                              tabIndex);
        return this->expandedStatusInTab[tabIndex];
    }
    return this->expandedStatusInDisplayGroup[displayIndex];
}

/**
 * Set the expanded status.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param expanded
 *    New expaned status.
 */
void 
ClassAndNameHierarchyModel::setExpanded(const DisplayGroupEnum::Enum displayGroup,
                                        const int32_t tabIndex,
                                        const bool expanded)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->expandedStatusInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->expandedStatusInTab, 
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, 
                              tabIndex);
        this->expandedStatusInTab[tabIndex] = expanded;
    }
    else {
        this->expandedStatusInDisplayGroup[displayIndex] = expanded;
    }
}

//===================================================================

/**
 * \class caret::ClassAndNameHierarchyModel::NameDisplayGroupSelector
 * \brief Maintains selection of a name in each 'DisplayGroupEnum'.
 */

/**
 * Constructor.
 * @param name
 *    The name.
 * @param key
 *    Key assigned to the name.
 */
ClassAndNameHierarchyModel::NameDisplayGroupSelector::NameDisplayGroupSelector(const AString& name,
                                                                               const int32_t key)
{
    this->name = name;
    this->key  = key;
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        this->selectedInDisplayGroup[i] = true;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->selectedInTab[i] = true;
    }
    this->counter = 0;
}

/**
 * Destructor.
 */
ClassAndNameHierarchyModel::NameDisplayGroupSelector::~NameDisplayGroupSelector()
{
}

/**
 * Copy the selections from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which selections are copied.
 * @param targetTabIndex
 *    Index of tab to which selections are copied.
 */
void 
ClassAndNameHierarchyModel::NameDisplayGroupSelector::copySelections(const int32_t sourceTabIndex,
                                                                     const int32_t targetTabIndex)
{
    this->selectedInTab[targetTabIndex] = this->selectedInTab[sourceTabIndex];
}

/**
 * @return The name.
 */
AString 
ClassAndNameHierarchyModel::NameDisplayGroupSelector::getName() const
{
    return this->name;
}

/**
 * @return The key.
 */
int32_t 
ClassAndNameHierarchyModel::NameDisplayGroupSelector::getKey() const
{
    return this->key;
}

/**
 * Is this name selected for the given display group.
 * @param displayGroup
 *    Display Group for which selection status is requested.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @return
 *    True if selected, else false.
 */
bool 
ClassAndNameHierarchyModel::NameDisplayGroupSelector::isSelected(const DisplayGroupEnum::Enum displayGroup,
                                                                 const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectedInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectedInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return this->selectedInTab[tabIndex];
    }
    
    return this->selectedInDisplayGroup[displayIndex];
}

/**
 * Set name seletion status for the given display group.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param status
 *    New selection status.
 */
void 
ClassAndNameHierarchyModel::NameDisplayGroupSelector::setSelected(const DisplayGroupEnum::Enum displayGroup,
                                                                  const int32_t tabIndex,
                                                                  const bool status)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectedselectedInDisplayGroup, 
                          DisplayGroupEnum::NUMBER_OF_GROUPS, 
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->selectedInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        this->selectedInTab[tabIndex] = status;
    }
    else {
        this->selectedInDisplayGroup[displayIndex] = status;
    }
}

/**
 * Clear the counter.
 */
void 
ClassAndNameHierarchyModel::NameDisplayGroupSelector::clearCounter()
{
    this->counter = 0;
}

/**
 * Increment the counter.
 */
void 
ClassAndNameHierarchyModel::NameDisplayGroupSelector::incrementCounter()
{
    this->counter++;
}

/**
 * @return The value of the counter.
 */
int32_t 
ClassAndNameHierarchyModel::NameDisplayGroupSelector::getCounter() const
{
    return this->counter;
}

//===================================================================

/**
 * \class caret::ClassAndNameHierarchyModel::ClassDisplayGroupSelector
 * \brief Maintains selection of a class and name in each 'DisplayGroupEnum'.
 */

/**
 * Constructor.
 * @param name
 *    The name.
 * @param key
 *    Key assigned to the name.
 */
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::ClassDisplayGroupSelector(const AString& name,
                                                                                 const int32_t key)
: ClassAndNameHierarchyModel::NameDisplayGroupSelector(name,
                                                       key)
{
    this->clearPrivate();
}

/**
 * Destructor.
 */
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::~ClassDisplayGroupSelector()
{
    this->clearPrivate();
}

/**
 * Copy the selections from one tab to another.
 * @param sourceTabIndex
 *    Index of tab from which selections are copied.
 * @param targetTabIndex
 *    Index of tab to which selections are copied.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::copySelections(const int32_t sourceTabIndex,
                                                                      const int32_t targetTabIndex)
{
    NameDisplayGroupSelector::copySelections(sourceTabIndex,
                                             targetTabIndex);
    this->expandedStatusInTab[targetTabIndex] = this->expandedStatusInTab[sourceTabIndex];
    
    for (std::map<AString, NameDisplayGroupSelector*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        NameDisplayGroupSelector* ns = iter->second;
        if (ns != NULL) {
            ns->copySelections(sourceTabIndex,
                               targetTabIndex);
        }
    }
}

/**
 * Clear the contents of this class selector.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::clear()
{
    clearPrivate();
}

/**
 * Clear the contents of this class selector.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::clearPrivate()
{
    for (std::map<AString, NameDisplayGroupSelector*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        NameDisplayGroupSelector* ns = iter->second;
        if (ns != NULL) {
            delete ns;
        }
    }
    this->nameToNameSelectorMap.clear();
    
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        this->expandedStatusInDisplayGroup[i] = false;
    }
    for (int32_t i = 0; i < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; i++) {
        this->expandedStatusInTab[i] = false;
    }
}

/**
 * Set the selection status for this class and all of its child names
 * and for all display groups.
 *
 * @param status
 *    New selection status.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::setAllSelected(const bool status)
{
    for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
        DisplayGroupEnum::Enum group = (DisplayGroupEnum::Enum)i;
        if (group == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
            for (int32_t j = 0; j < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; j++) {
                this->setSelected(group, j, status);
            }
        }
        else {
            this->setSelected(group, -1, status);
        }
    }
    
    for (std::map<AString, NameDisplayGroupSelector*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        NameDisplayGroupSelector* ns = iter->second;
        if (ns != NULL) {
            for (int32_t i = 0; i < DisplayGroupEnum::NUMBER_OF_GROUPS; i++) {
                DisplayGroupEnum::Enum group = (DisplayGroupEnum::Enum)i;
                if (group == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
                    for (int32_t j = 0; j < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; j++) {
                        ns->setSelected(group, j, status);
                    }
                }
                else {
                    ns->setSelected(group, -1, status);
                }
            }
        }
    }
}

/**
 * Set the selection status of this class and its names for the display group/tab.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param selectionStatus
 *    New selection status for class and its names.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::setAllSelected(const DisplayGroupEnum::Enum displayGroup,
                                                                      const int32_t tabIndex,
                                                                      const bool selectionStatus)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->selectionStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    setSelected(displayGroup,
                tabIndex,
                selectionStatus);
    
    for (std::map<AString, NameDisplayGroupSelector*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        NameDisplayGroupSelector* nameSelector = iter->second;
        nameSelector->setSelected(displayGroup,
                                  tabIndex,
                                  selectionStatus);
    }
}

/**
 * Is this class and all of its child names selected?
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @return
 *    true if everything is selected, else false.
 */
bool
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::isAllSelected(const DisplayGroupEnum::Enum displayGroup,
                                                                     const int32_t tabIndex) const
{
    if (isSelected(displayGroup, tabIndex) == false) {
        return false;
    }
    
    for (std::map<AString, NameDisplayGroupSelector*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        const NameDisplayGroupSelector* nameSelector = iter->second;
        if (nameSelector->isSelected(displayGroup, tabIndex) == false) {
            return false;
        }
    }
    
    return true;
}


/**
 * @return All keys for the names.
 */
std::vector<int32_t>
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::getAllNameKeysSortedByName() const
{
    std::vector<int32_t> allKeys;
    
    for (std::map<AString, NameDisplayGroupSelector*>::const_iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        NameDisplayGroupSelector* ns = iter->second;
        allKeys.push_back(ns->getKey());
    }
    
    return allKeys;
}

/**
 * Add a name selector to the map that maps a name to a name selector.
 * @param name
 *     Name of selector.
 * @param nameSelector
 *     The name selector.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::addToNameSelectorMap(const AString& name,
                                                 NameDisplayGroupSelector* nameSelector)
{
    CaretAssert(nameSelector);
    this->nameToNameSelectorMap.insert(std::make_pair(name, nameSelector));
}

/**
 * Remove a name selector from the map that maps names to name selectors.
 * Caller will delete the name selector so, in here, just remove it from
 * the map.
 *
 * @param nameSelector
 *     Name selector that is to be removed.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::removeNameSelector(NameDisplayGroupSelector* nameSelector)
{
    for (std::map<AString, NameDisplayGroupSelector*>::iterator iter = this->nameToNameSelectorMap.begin();
         iter != this->nameToNameSelectorMap.end();
         iter++) {
        if (nameSelector == iter->second) {
            iter->second = NULL;
            this->nameToNameSelectorMap.erase(iter);
            break;
        }
    }
}


/**
 * Get the name selector for the name with the given name.
 * @param name
 *    The name.
 * @return
 *    Name selector for the given name or NULL if there
 *    is no name selector with the given name.
 */
ClassAndNameHierarchyModel::NameDisplayGroupSelector*
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::getNameSelectorWithName(const AString& name)
{
    std::map<AString, NameDisplayGroupSelector*>::iterator iter = this->nameToNameSelectorMap.find(name);
    if (iter != this->nameToNameSelectorMap.end()) {
        NameDisplayGroupSelector* ns = iter->second;
        return ns;
    }
    
    return NULL;
}


/**
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @return The expanded status.
 */
bool
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::isExpanded(const DisplayGroupEnum::Enum displayGroup,
                                                                  const int32_t tabIndex) const
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->expandedStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->expandedStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        return this->expandedStatusInTab[tabIndex];
    }
    return this->expandedStatusInDisplayGroup[displayIndex];
}

/**
 * Set the expanded status.
 * @param displayGroup
 *    Display group selected.
 * @param tabIndex
 *    Index of tab used when displayGroup is DisplayGroupEnum::DISPLAY_GROUP_TAB.
 * @param expanded
 *    New expaned status.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelector::setExpanded(const DisplayGroupEnum::Enum displayGroup,
                                                                   const int32_t tabIndex,
                                                                   const bool expanded)
{
    const int32_t displayIndex = (int32_t)displayGroup;
    CaretAssertArrayIndex(this->expandedStatusInDisplayGroup,
                          DisplayGroupEnum::NUMBER_OF_GROUPS,
                          displayIndex);
    if (displayGroup == DisplayGroupEnum::DISPLAY_GROUP_TAB) {
        CaretAssertArrayIndex(this->expandedStatusInTab,
                              BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS,
                              tabIndex);
        this->expandedStatusInTab[tabIndex] = expanded;
    }
    else {
        this->expandedStatusInDisplayGroup[displayIndex] = expanded;
    }
}

//===================================================================

/**
 * \class caret::ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey
 * \brief Maintains selection of a class and name in each 'DisplayGroupEnum'.
 * The name's "key" is generated by this class and returned when a name is added.
 */

/**
 * Constructor.
 * @param name
 *    The name.
 * @param key
 *    Key assigned to the name.
 */
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::ClassDisplayGroupSelectorGeneratedKey(const AString& name,
                                                                                 const int32_t key)
: ClassAndNameHierarchyModel::ClassDisplayGroupSelector(name,
                                                       key)
{
    this->clear();
}

/**
 * Destructor.
 */
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::~ClassDisplayGroupSelectorGeneratedKey()
{
    this->clear();
}

/**
 * Clear the contents of this class selector.
 */
void 
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::clear()
{
    ClassDisplayGroupSelector::clear();
    
    /* Note: parent class' clear() method deletes all name selectors, so just clear vector */
    this->keyToNameSelectorVector.clear();
    
    this->availableNameKeys.clear();
}

/**
 * Add a name to the hierarchy.  If the name is already used in the selector,
 * its key is returned.  Otherwise, the name is added and a new key is returned.
 *
 * @param name
 *    Name that is added.
 * @return Key for name.
 */
int32_t 
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::addName(const AString& name)
{
    int32_t nameKey = -1;
    
    NameDisplayGroupSelector* nameSelector = this->getNameSelectorWithName(name);
    if (nameSelector != NULL) {
        nameKey = nameSelector->getKey();
    }
    else {
        if (this->availableNameKeys.empty()) {
            nameKey = static_cast<int32_t>(this->keyToNameSelectorVector.size());
            nameSelector = new NameDisplayGroupSelector(name, nameKey);
            this->keyToNameSelectorVector.push_back(nameSelector);
        }
        else {
            nameKey = this->availableNameKeys.front();
            this->availableNameKeys.pop_front();
            CaretAssert(this->keyToNameSelectorVector[nameKey]);
            nameSelector = new NameDisplayGroupSelector(name, nameKey);
            this->keyToNameSelectorVector[nameKey] = nameSelector;
        }
        this->addToNameSelectorMap(name, nameSelector);
    }
    CaretAssert(nameKey >= 0);
    nameSelector->incrementCounter();
    
    return nameKey;
}

/**
 * Get the name selector for the name with the given key.
 * @param nameKey
 *    Key for name.
 * @return
 *    Name selector for the given key or NULL if there
 *    is no name selector with the given key.
 */
ClassAndNameHierarchyModel::NameDisplayGroupSelector* 
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::getNameSelectorWithKey(const int32_t nameKey)
{
    if ((nameKey >= 0) 
        && (nameKey < static_cast<int32_t>(this->keyToNameSelectorVector.size()))) {
        NameDisplayGroupSelector* ns = this->keyToNameSelectorVector[nameKey];
        if (ns != NULL) {
            return ns;
        }
    }
    
    return NULL;
}

/**
 * Get the name selector for the name with the given key.
 * @param nameKey
 *    Key for name.
 * @return
 *    Name selector for the given key or NULL if there
 *    is no name selector with the given key.
 */
const ClassAndNameHierarchyModel::NameDisplayGroupSelector* 
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::getNameSelectorWithKey(const int32_t nameKey) const
{
    if ((nameKey >= 0) 
        && (nameKey < static_cast<int32_t>(this->keyToNameSelectorVector.size()))) {
        NameDisplayGroupSelector* ns = this->keyToNameSelectorVector[nameKey];
        if (ns != NULL) {
            return ns;
        }
    }
    
    return NULL;
}

/**
 * Clear (set to zero) counters for all names in this class.
 */
void 
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::clearAllNameCounters()
{
    for (std::vector<NameDisplayGroupSelector*>::const_iterator iter = this->keyToNameSelectorVector.begin();
         iter != this->keyToNameSelectorVector.end();
         iter++) {
        NameDisplayGroupSelector* ns = *iter;
        if (ns != NULL) {
            ns->clearCounter();
        }
    }
}

int32_t 
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::getNumberOfNamesWithCountersGreaterThanZero() const
{
    int32_t numberOfNamesWithCounterGreaterThanZero = 0;
    
    for (std::vector<NameDisplayGroupSelector*>::const_iterator iter = this->keyToNameSelectorVector.begin();
         iter != this->keyToNameSelectorVector.end();
         iter++) {
        const NameDisplayGroupSelector* ns = *iter;
        if (ns != NULL) {
            if (ns->getCounter() > 0) {
                numberOfNamesWithCounterGreaterThanZero++;
            }
        }
    }
    
    return numberOfNamesWithCounterGreaterThanZero;
}

/**
 * Remove any names that have a count equal to zero.
 */
void 
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey::removeNamesWithCountersEqualZero()
{
    const int32_t numberOfNames = static_cast<int32_t>(this->keyToNameSelectorVector.size());
    /*
     * Loop to find names with counters equal to zero
     */
    for (int32_t i = 0; i < numberOfNames; i++) {
        NameDisplayGroupSelector* ns = this->keyToNameSelectorVector[i];
        if (ns != NULL) {
            if (ns->getCounter() <= 0) {
                /*
                 * Remove name selector from the name to name selector map
                 */
                this->removeNameSelector(ns);
                
                /*
                 * Delete the name selector and add index to available keys
                 */
                delete ns;
                this->keyToNameSelectorVector[i] = NULL;
                this->availableNameKeys.push_front(i);
            }
        }
    }
}

//===================================================================

/**
 * \class caret::ClassAndNameHierarchyModel::ClassDisplayGroupSelectorGeneratedKey
 * \brief Maintains selection of a class and name in each 'DisplayGroupEnum'.
 * The name's "key" is generated by this class and returned when a name is added.
 */

/**
 * Constructor.
 * @param name
 *    The name.
 * @param key
 *    Key assigned to the name.
 */
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::ClassDisplayGroupSelectorUserKey(const AString& name,
                                                                                                         const int32_t key)
: ClassAndNameHierarchyModel::ClassDisplayGroupSelector(name,
                                                        key)
{
    this->clear();
}

/**
 * Destructor.
 */
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::~ClassDisplayGroupSelectorUserKey()
{
    this->clear();
}

/**
 * Clear the contents of this class selector.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::clear()
{
    ClassDisplayGroupSelector::clear();
    
    /* Note: parent class' clear() method deletes all name selectors, so just clear map */
    this->keyToNameSelectorMap.clear();
}

/**
 * Add a name and key to the hierarchy.  Note, a name may be associated
 * with more than one key for this type of selector.
 *
 * @param name
 *    Name that is added.
 * @param nameKey
 *    Key associated with the name.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::addNameWithKey(const AString& name,
                                                                             const int32_t nameKey)
{
    /*
     * Does selector with key exist?
     */
    NameDisplayGroupSelector* nameSelector = this->getNameSelectorWithKey(nameKey);
    if (nameSelector != NULL) {
        const int32_t existingNameKey = nameSelector->getKey();
        
        /*
         * Does key match? If so, do not need to make any changes and exit.
         */
        if (existingNameKey == nameKey) {
            return;
        }
        
        /*
         * Remove the name selector since key exists and replace it
         */
        this->removeNameSelector(nameSelector);
        this->keyToNameSelectorMap.erase(existingNameKey);
        delete nameSelector;
    }
    
    nameSelector = new NameDisplayGroupSelector(name, nameKey);
    this->addToNameSelectorMap(name, nameSelector);
    this->keyToNameSelectorMap.insert(std::make_pair(nameKey, nameSelector));
}

/**
 * Get the name selector for the name with the given key.
 * @param nameKey
 *    Key for name.
 * @return
 *    Name selector for the given key or NULL if there
 *    is no name selector with the given key.
 */
ClassAndNameHierarchyModel::NameDisplayGroupSelector*
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::getNameSelectorWithKey(const int32_t nameKey)
{
    std::map<int32_t, NameDisplayGroupSelector*>::iterator iter = this->keyToNameSelectorMap.find(nameKey);
    if (iter != this->keyToNameSelectorMap.end()) {
        NameDisplayGroupSelector* ns = iter->second;
        return ns;
    }
    
    return NULL;
}

/**
 * Get the name selector for the name with the given key.
 * @param nameKey
 *    Key for name.
 * @return
 *    Name selector for the given key or NULL if there
 *    is no name selector with the given key.
 */
const ClassAndNameHierarchyModel::NameDisplayGroupSelector*
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::getNameSelectorWithKey(const int32_t nameKey) const
{
    std::map<int32_t, NameDisplayGroupSelector*>::const_iterator iter = this->keyToNameSelectorMap.find(nameKey);
    if (iter != this->keyToNameSelectorMap.end()) {
        NameDisplayGroupSelector* ns = iter->second;
        return ns;
    }
    
    return NULL;
}

/**
 * Clear (set to zero) counters for all names in this class.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::clearAllNameCounters()
{
    /* not used */
}

int32_t
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::getNumberOfNamesWithCountersGreaterThanZero() const
{
    /* not used */
    return 1;
}

/**
 * Remove any names that have a count equal to zero.
 */
void
ClassAndNameHierarchyModel::ClassDisplayGroupSelectorUserKey::removeNamesWithCountersEqualZero()
{
    /* not used */
}


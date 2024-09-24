
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __LABEL_SELECTION_ITEM_DECLARE__
#include "LabelSelectionItem.h"
#undef __LABEL_SELECTION_ITEM_DECLARE__

#include "ApplicationInformation.h"
#include "CaretAssert.h"
#include "Cluster.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * \class caret::LabelSelectionItem 
 * \brief Extends QStandardItem for label hierarchy
 * \ingroup Files
 */

/**
 * Constructor for a label
 * @param text
 *    Text displayed in the item
 * @param ontologyID
 *    The ID from the JSON
 * @param labelIndex
 *    Index of the label
 * @param labelRGBA
 *    Color for icon
 */
LabelSelectionItem::LabelSelectionItem(const AString& text,
                                       const AString& ontologyID,
                                       const int32_t labelIndex,
                                       const std::array<uint8_t, 4>& labelRGBA)
: QStandardItem(),
m_itemType(ItemType::ITEM_LABEL),
m_ontologyID(ontologyID),
m_labelIndex(labelIndex)
{
    initializeInstance();
    setText(text);
    
    /*
     * Only create pixmap when GUI is available (QApplication / wb_view) otherwise a crash
     * will occur without a GUI (QCoreApplication / wb_command).
     */
    if (ApplicationInformation::getApplicationType() == ApplicationTypeEnum::APPLICATION_TYPE_GRAPHICAL_USER_INTERFACE) {
        QPixmap iconPixmap(24, 24);
        iconPixmap.fill(QColor(labelRGBA[0],
                               labelRGBA[1],
                               labelRGBA[2],
                               labelRGBA[3]));
        setIcon(iconPixmap);
    }
}

/**
 * Constructor for an item in the hierarchy that is above and not a label
 * @param text
 *    Text displayed in the item
 * @param ontologyID
 *    The ID from the JSON
 */
LabelSelectionItem::LabelSelectionItem(const AString& text,
                                       const AString& ontologyID)
: QStandardItem(),
m_itemType(ItemType::ITEM_HIERARCHY),
m_ontologyID(ontologyID),
m_labelIndex(-1)
{
    initializeInstance();
    setText(text);
}

/**
 * Copy constructor
 */
LabelSelectionItem::LabelSelectionItem(const LabelSelectionItem& other)
: QStandardItem(other),
SceneableInterface(other),
m_itemType(other.m_itemType)
{
    initializeInstance();
    copyHelper(other);
}


/**
 * Destructor.
 */
LabelSelectionItem::~LabelSelectionItem()
{
}

/**
 * Copy constructor
 * @param other
 *    Item that is copied to 'this'
 */
LabelSelectionItem&
LabelSelectionItem::operator=(const LabelSelectionItem& other)
{
    if (this != &other) {
        QStandardItem::operator=(other);
        this->copyHelper(other);
    }
    return *this;
}

/**
 * Helps with copying an instance
 * @param other
 *    Other item that is copied to this
 */
void
LabelSelectionItem::copyHelper(const LabelSelectionItem& other)
{
    setText(other.text());
    m_ontologyID = other.m_ontologyID;
    m_labelIndex = other.m_labelIndex;
    if (m_labelIndex >= 0) {
        setIcon(other.icon());
    }
}

/**
 * Initialize an instance
 */
void
LabelSelectionItem::initializeInstance()
{
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    setCheckable(true);
    appendToToolTip("Right-click for menu to go to label's location");
    if ( ! m_ontologyID.isEmpty()) {
        appendToToolTip("Ontology ID=" + m_ontologyID);
    }
}

/**
 * @return a copy of this item.
 * The item's children are not copied.
 * When subclassing QStandardItem, you can reimplement this function to provide
 * QStandardItemModel with a factory that it can use to create new items on demand.
 */
QStandardItem*
LabelSelectionItem::clone() const
{
    LabelSelectionItem* item(new LabelSelectionItem(*this));
    
    return item;
}

/**
 * @return the type of this item.
 * The type is used to distinguish custom items from the base class.
 * When subclassing QStandardItem, you should reimplement this function and return a new value
 * greater than or equal to UserType.
 */
int
LabelSelectionItem::type() const
{
    return static_cast<int>(m_itemType);
}

AString 
LabelSelectionItem::getOntologyID() const
{
    return m_ontologyID;
}

/**
 * @return Index of the label
 */
int32_t
LabelSelectionItem::getLabelIndex() const
{
    return m_labelIndex;
}

/**
 * Set all children to the given checked status
 * @param checked
 *    New checked status
 */
void
LabelSelectionItem::setAllChildrenChecked(const bool checked)
{
    const Qt::CheckState checkState(checked
                                    ? Qt::Checked
                                    : Qt::Unchecked);
    setCheckedStatusOfAllChildren(this,
                                  checkState);
}

/**
 * Set the checked status of all children
 * @param item
 *    The item
 * @param checkState
 *    The check state
 */
void
LabelSelectionItem::setCheckedStatusOfAllChildren(QStandardItem* item,
                                                  const Qt::CheckState checkState)
{
    CaretAssert(item);
    const int32_t numChildren(item->rowCount());
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* child(item->child(iRow));
        child->setCheckState(checkState);
        setCheckedStatusOfAllChildren(child,
                                      checkState);
    }
}

/**
 * Set the check state for this instance based upon its children's check states
 * There are three checked status for items WITH CHILDREN
 * - Checked: This instance and ALL children are checked
 * - Unchecked: This instance and ALL children and unchecked
 * - PartiallyChecked: There are children that are both checked and unchecked
 * @return
 *    Check state for this instance
 */
Qt::CheckState
LabelSelectionItem::setCheckStateFromChildren()
{
    const int32_t numChildren(rowCount());
    
    /*
     * If no children, use this instance's check state
     */
    if (numChildren == 0) {
        return checkState();
    }
    
    int32_t numChildrenChecked = 0;
    bool partiallyCheckedFlag(false);
    
    /*
     * Examine checked status of children
     */
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* myChild(child(iRow));
        CaretAssert(myChild);
        CaretAssert(myChild != this); /* should never happen and would cause infinite loop */
        
        LabelSelectionItem* labelChild(dynamic_cast<LabelSelectionItem*>(myChild));
        CaretAssert(labelChild);
        const Qt::CheckState childCheckState(labelChild->setCheckStateFromChildren());
        switch (childCheckState) {
            case Qt::Unchecked:
                break;
            case Qt::Checked:
                ++numChildrenChecked;
                break;
            case Qt::PartiallyChecked:
                /*
                 * If a child is partially checked, then this instance will be partially checked
                 */
                partiallyCheckedFlag = true;
                break;
        }
        
        if (partiallyCheckedFlag) {
            break;
        }
    }
    
    Qt::CheckState childrenCheckState(Qt::Unchecked);
    
    if (partiallyCheckedFlag) {
        /*
         * At least one child is partially checked so this instance is partially checked
         */
        childrenCheckState = Qt::PartiallyChecked;
    }
    else if (numChildrenChecked == numChildren) {
        /*
         * All children are checked so this instance is checked
         */
        childrenCheckState = Qt::Checked;
    }
    else if (numChildrenChecked == 0) {
        /*
         * All children and unchecked so this instance is unchecked
         */
        childrenCheckState = Qt::Unchecked;
    }
    else if (numChildrenChecked > 0) {
        /*
         * Have children that are both checked and unchecked so this
         * instance is partially checked
         */
        childrenCheckState = Qt::PartiallyChecked;;
    }
    else {
        /*
         * Should not get here
         */
        CaretAssert(0);
    }

    Qt::CheckState checkStateOut(childrenCheckState);
    
    /*
     * If this item corresponds to a label, its status
     * is INDEPENDENT of its children.
     */
    if (getLabelIndex() >= 0) {
        switch (checkState()) {
            case Qt::Checked:
                checkStateOut = Qt::Checked;
                break;
            case Qt::Unchecked:
                checkStateOut = Qt::Unchecked;
                break;
            case Qt::PartiallyChecked:
                checkStateOut = Qt::Checked;
                break;
        }
    }
    
    /*
     * Set check state of this instance
     */
    setCheckState(checkStateOut);
    
    return checkStateOut;
}

/**
 * Set the all, left, and right clusters that are accumulated
 * from my clusters and clusters from all children.
 */
void
LabelSelectionItem::setMyAndChildrenMergedClusters()
{
    m_myAndChildrenMergedClusters.clear();
    const int32_t numChildren(rowCount());
    if (numChildren <= 0) {
        return;
    }
    
    std::vector<const Cluster*> allClusters;
    
    const int32_t numMyClusters(m_mergedClusters.size());
    for (int32_t i = 0; i < numMyClusters; i++) {
        CaretAssertVectorIndex(m_mergedClusters, i);
        allClusters.push_back(&m_mergedClusters[i]);
    }

    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* myChild(child(iRow));
        CaretAssert(myChild);
        CaretAssert(myChild != this); /* should never happen and would cause infinite loop */
        
        LabelSelectionItem* labelChild(dynamic_cast<LabelSelectionItem*>(myChild));
        CaretAssert(labelChild);
        
        /*
         * Set merged clusters in child and all of its children
         */
        labelChild->setMyAndChildrenMergedClusters();
        
        /*
         * Now use merged clusters from child
         */
        const std::vector<const Cluster*> childClusters(labelChild->getMergedClusterPointers());
        //const std::vector<const Cluster*> childClusters(labelChild->getMyAndChildrenMergedClusterPointers());
        for (const Cluster* c : childClusters) {
            allClusters.push_back(c);
        }
    }
    
    const bool allowNoneTypeFlag(false);
    m_myAndChildrenMergedClusters = Cluster::mergeClustersByClusterType(text(),
                                                                        getLabelIndex(),
                                                                        allClusters,
                                                                        allowNoneTypeFlag);
}

/**
 * @return All descendants and this item that are of the given type
 * @param itemType
 *    Type for matching to QStandardItem::type()
 * @return
 *    Vector containing the matching items
 */
std::vector<LabelSelectionItem*>
LabelSelectionItem::getThisAndAllDescendantsOfType(const LabelSelectionItem::ItemType itemType)
{
    std::vector<LabelSelectionItem*> itemsOut;
    
    if (type() == static_cast<int>(itemType)) {
        itemsOut.push_back(this);
    }
    
    if (hasChildren()) {
        const int32_t numRows(rowCount());
        for (int32_t iRow = 0; iRow < numRows; iRow++) {
            QStandardItem* itemChild(child(iRow));
            LabelSelectionItem* labelChild(dynamic_cast<LabelSelectionItem*>(itemChild));
            CaretAssert(labelChild);
            std::vector<LabelSelectionItem*> childItems(labelChild->getThisAndAllDescendantsOfType(itemType));
            itemsOut.insert(itemsOut.end(),
                            childItems.begin(), childItems.end());
        }
    }
    
    return itemsOut;
}

/**
 * @return This and all descendants
 */
std::vector<LabelSelectionItem*>
LabelSelectionItem::getThisAndAllDescendants()
{
    std::vector<LabelSelectionItem*> itemsOut;
    
    itemsOut.push_back(this);
    
    if (hasChildren()) {
        const int32_t numRows(rowCount());
        for (int32_t iRow = 0; iRow < numRows; iRow++) {
            QStandardItem* itemChild(child(iRow));
            LabelSelectionItem* labelChild(dynamic_cast<LabelSelectionItem*>(itemChild));
            CaretAssert(labelChild);
            std::vector<LabelSelectionItem*> childItems(labelChild->getThisAndAllDescendants());
            itemsOut.insert(itemsOut.end(),
                            childItems.begin(), childItems.end());
        }
    }

    return itemsOut;
}

/**
 * @return A formatted string showing the hierarchy
 * @param indentation
 *    Indentation for the string
 */
AString
LabelSelectionItem::toFormattedString(const AString& indentation) const
{
    AString text;
    
    text.appendWithNewLine(indentation + AString(this->text()));
    
    if (hasChildren()) {
        const AString& childIndentation(indentation + "   ");

        const int32_t numRows(rowCount());
        for (int32_t iRow = 0; iRow < numRows; iRow++) {
            QStandardItem* itemChild(child(iRow));
            LabelSelectionItem* labelChild(dynamic_cast<LabelSelectionItem*>(itemChild));
            CaretAssert(labelChild);
            text.appendWithNewLine(labelChild->toFormattedString(childIndentation));
        }
    }
    
    return text;
}

/**
 * @return Reference to the raw clusters (before any accumulation)
 * May contain multiple, disjoint clusters.
 */
const std::vector<Cluster>&
LabelSelectionItem::getRawClusters() const
{
    return m_rawClusters;
}

/**
 * @return Reference to the merged All, left, and right, clusters (not all may be present)
 */
const std::vector<Cluster>&
LabelSelectionItem::getMergedClusters() const
{
    return m_mergedClusters;
}

/**
 * Pointers to the merged clusters
 */
std::vector<const Cluster*>
LabelSelectionItem::getMergedClusterPointers() const
{
    std::vector<const Cluster*> clustersOut;
    const int32_t numMergedClusters(m_mergedClusters.size());
    for (int32_t i = 0; i < numMergedClusters; i++) {
        CaretAssertVectorIndex(m_mergedClusters, i);
        clustersOut.push_back(&m_mergedClusters[i]);
    }
    return clustersOut;
}

/**
 * @return Reference to the merged All, left, and right, clusters accumulated
 * from my clusters and all children clusters
 */
const std::vector<Cluster>&
LabelSelectionItem::getMyAndChildrenMergedClusters() const
{
    return m_myAndChildrenMergedClusters;
}

/**
 * Pointers to the merged clusters
 */
std::vector<const Cluster*>
LabelSelectionItem::getMyAndChildrenMergedClusterPointers() const
{
    std::vector<const Cluster*> clustersOut;
    const int32_t numMergedClusters(m_myAndChildrenMergedClusters.size());
    for (int32_t i = 0; i < numMergedClusters; i++) {
        CaretAssertVectorIndex(m_myAndChildrenMergedClusters, i);
        clustersOut.push_back(&m_myAndChildrenMergedClusters[i]);
    }
    return clustersOut;
}

/**
 * Set the raw clusters (may contain multiple, disjoint clusters)
 * @param rawClusters
 *    Raw clusters that are copied to this instance
 */
void
LabelSelectionItem::setRawClusters(const std::vector<const Cluster*>& rawClusters)
{
    m_rawClusters.clear();
    for (const auto& c : rawClusters) {
        m_rawClusters.push_back(Cluster(*c));
    }
    
    m_mergedClusters = Cluster::mergeClustersByCogSignOfX(rawClusters);
}

/**
 * Append text to the current tooltip separating with a newline.
 */
void
LabelSelectionItem::appendToToolTip(const QString& text)
{
    if (text.isEmpty()) {
        return;
    }
    QString s(this->toolTip().trimmed());
    if ( ! s.isEmpty()) {
        s.append("\n");
    }
    s.append(text);
    setToolTip(s);
}

/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
LabelSelectionItem::saveToScene(const SceneAttributes* sceneAttributes,
                                 const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "LabelSelectionItem",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);        
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
LabelSelectionItem::restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
}


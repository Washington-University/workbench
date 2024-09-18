
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
 * @param labelIndex
 *    Index of the label
 * @param labelRGBA
 *    Color for icon
 */
LabelSelectionItem::LabelSelectionItem(const AString& text,
                                       const int32_t labelIndex,
                                       const std::array<uint8_t, 4>& labelRGBA)
: QStandardItem(),
m_itemType(ItemType::ITEM_LABEL),
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
 */
LabelSelectionItem::LabelSelectionItem(const AString& text)
: QStandardItem(),
m_itemType(ItemType::ITEM_HIERARCHY),
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
 * @return  A pair with a boolean indicating the validity of the Vector3D containing the COG for this item
 */
std::pair<bool, Vector3D>
LabelSelectionItem::getCenterOfGravityFromChildren() const
{
    return std::make_pair(m_centerOfGravityValidFlag, m_centerOfGravity);
}

/**
 * Set the center of gravity from its children
 * @retrurn A pair with a boolean indicating the validity of the Vector3D containing the COG for this item
 */
LabelSelectionItem::ChildCogInfo
LabelSelectionItem::setCenterOfGravityFromChildren()
{
    const int32_t numChildren(rowCount());

    m_centerOfGravity.fill(0.0);
    m_centerOfGravityValidFlag = false;

    /*
     * WEIGHTED COG of my clusters
     */
    Vector3D myClustersCenterOfGravity(0.0, 0.0, 0.0);
    bool myClustersCenterOfGravityValidFlag(false);
    float myClustersNumberOfBrainordinates(0.0);
    if ( ! m_clusters.empty()) {
        for (const Cluster& cluster : m_clusters) {
            const float numBrainordinates(cluster.getNumberOfBrainordinates());
            CaretAssert(numBrainordinates >= 1.0);
            myClustersCenterOfGravity += (cluster.getCenterOfGravityXYZ()
                                          * numBrainordinates);
            myClustersNumberOfBrainordinates += numBrainordinates;
        }
        CaretAssert(myClustersNumberOfBrainordinates >= 1.0);
        myClustersCenterOfGravity /= static_cast<float>(myClustersNumberOfBrainordinates);
        myClustersCenterOfGravityValidFlag = true;
    }

    /*
     * If no children use COG of this item's clusters
     * and keep self's COG invalid
     */
    if (numChildren == 0) {
        return ChildCogInfo(myClustersCenterOfGravity,
                            myClustersNumberOfBrainordinates,
                            myClustersCenterOfGravityValidFlag);
    }
        
    /*
     * Find COG of children
     */
    float centerOfGravityNumberOfBrainordinates(0.0);
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* myChild(child(iRow));
        CaretAssert(myChild);
        CaretAssert(myChild != this); /* should never happen and would cause infinite loop */
        
        LabelSelectionItem* labelChild(dynamic_cast<LabelSelectionItem*>(myChild));
        CaretAssert(labelChild);
        
        ChildCogInfo childCogInfo(labelChild->setCenterOfGravityFromChildren());
        if (childCogInfo.isValid()) {
            m_centerOfGravity += (childCogInfo.getCenterOfGravity()
                                  * childCogInfo.getNumberOfBrainordinates());
            centerOfGravityNumberOfBrainordinates += childCogInfo.getNumberOfBrainordinates();
        }
    }
    
    /*
     * Add my center of gravity
     */
    if (myClustersCenterOfGravityValidFlag) {
        m_centerOfGravity += (myClustersCenterOfGravity
                              * myClustersNumberOfBrainordinates);
        centerOfGravityNumberOfBrainordinates += myClustersNumberOfBrainordinates;
    }
    
    if (centerOfGravityNumberOfBrainordinates >= 1.0) {
        m_centerOfGravity /= centerOfGravityNumberOfBrainordinates;
        m_centerOfGravityValidFlag = true;
    }
    
    return ChildCogInfo(m_centerOfGravity, 
                        centerOfGravityNumberOfBrainordinates,
                        m_centerOfGravityValidFlag);
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
 * @return Reference to the clusters
 */
const std::vector<const Cluster>&
LabelSelectionItem::getClusters() const
{
    return m_clusters;
}

/**
 * Set the clusters
 * @param clusters
 *    Clusters that are copied to this instance
 */
void
LabelSelectionItem::setClusters(const std::vector<const Cluster*>& clusters)
{
    m_clusters.clear();
    for (const auto& c : clusters) {
        m_clusters.push_back(Cluster(*c));
    }
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


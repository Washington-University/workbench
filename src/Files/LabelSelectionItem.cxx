
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
#include "CaretLogger.h"
#include "Cluster.h"
#include "GiftiLabel.h"
#include "HtmlStringBuilder.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"

using namespace caret;

/**
 * \class caret::LabelSelectionItem 
 * \brief Extends QStandardItem for label hierarchy
 * \ingroup Files
 */

/**
 * Constructor for a for hierarchy item and optional label
 * @param hierarchyItem
 *    The hierarchy item (MUST NOT BE NULL)
 * @param giftiLabel
 *    Optional GIFTI label (may be NULL)
 */
LabelSelectionItem::LabelSelectionItem(const CaretHierarchy::Item* hierarchyItem,
                                       const GiftiLabel* giftiLabel)
: QStandardItem(),
m_itemType((giftiLabel != NULL)
           ? ItemType::ITEM_LABEL
           : ItemType::ITEM_HIERARCHY),
m_labelIndex((giftiLabel != NULL)
             ? giftiLabel->getKey()
             : -1)
{
    initializeInstance();
    setText(hierarchyItem->name);
    
    /*
     * Only create pixmap when GUI is available (QApplication / wb_view) otherwise a crash
     * will occur without a GUI (QCoreApplication / wb_command).
     */
    if (giftiLabel != NULL) {
        if (ApplicationInformation::getApplicationType() == ApplicationTypeEnum::APPLICATION_TYPE_GRAPHICAL_USER_INTERFACE) {
            QPixmap iconPixmap(24, 24);
            const std::array<uint8_t, 4> rgba(getLabelRGBA(giftiLabel));
            iconPixmap.fill(QColor(rgba[0],
                                   rgba[1],
                                   rgba[2],
                                   rgba[3]));
            setIcon(iconPixmap);
        }
    }
    
    m_primaryName         = hierarchyItem->name;
    m_alternativeNamesMap = hierarchyItem->extraInfo.getAllData();
}

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
    
    m_primaryName = text;
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
    m_primaryName = text;
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
    m_labelIndex = other.m_labelIndex;
    if (m_labelIndex >= 0) {
        if (ApplicationInformation::getApplicationType() == ApplicationTypeEnum::APPLICATION_TYPE_GRAPHICAL_USER_INTERFACE) {
            setIcon(other.icon());
        }
    }
    
    m_primaryName         = other.m_primaryName;
    m_alternativeNamesMap = other.m_alternativeNamesMap;
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
 * @return The alternative names map
 */
std::vector<std::pair<AString, AString>>
LabelSelectionItem::getAlternativeNamesMap() const
{
    return m_alternativeNamesMap;
}

/**
 * @return The primary name (default name)
 */
AString
LabelSelectionItem::getPrimaryName() const
{
    return m_primaryName;
}

/**
 * Show the primary name as text
 */
void
LabelSelectionItem::setShowPrimaryName()
{
    setText(m_primaryName);
}

/**
 * Show the given alternative name as text
 * @param alternativeName
 *    Name to display
 */
void
LabelSelectionItem::setShowAlternativeName(const AString& alternativeName)
{
    for (auto& nameMap : m_alternativeNamesMap) {
        if (nameMap.first == alternativeName) {
            setText(nameMap.second);
            break;
        }
    }
    
    /*
     * Use the primary name if the alternative name is empty
     */
    if (text().isEmpty()) {
        setShowPrimaryName();
    }
}

/**
 * @return The RGBA color for the label as four bytes.  If the label is NULL,
 * white it returned.
 * @param label
 *    The GIFTI label
 */
std::array<uint8_t, 4>
LabelSelectionItem::getLabelRGBA(const GiftiLabel* label)
{
    std::array<uint8_t, 4> rgba { 255, 255, 255, 255 };
    if (label == NULL) {
        return rgba;
    }
    
    const std::array<float, 4> rgbaFloat {
        label->getRed(),
        label->getGreen(),
        label->getBlue(),
        label->getAlpha()
    };
    
    for (int i = 0; i < 4; i++) {
        int32_t c(static_cast<int32_t>(rgbaFloat[i] * 255.0));
        if (c > 255) c = 255;
        if (c < 0) c = 0;
        rgba[i] = c;
    }
    
    return rgba;
}

/**
 * @return Information about label for display in GUI
 */
AString
LabelSelectionItem::getTextForInfoDisplay() const
{
    HtmlStringBuilder html;
    html.addBold(text());
    html.addLineBreak();
    
    for (const auto& nmi : m_alternativeNamesMap) {
        const AString name(nmi.first);
        const AString value(nmi.second);
        if (name.isEmpty()
            && value.isEmpty()) {
            /* Both empty */
        }
        else {
            html.addLine(name
                         + ": "
                         + value);
        }
    }
    return html.toStringWithHtmlBodyForToolTip();
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
    m_myAndChildrenCentersOfGravity.reset(new CogSet());
    
    /*
     * May not have centers of gravity if a parent label
     */
    if (m_centersOfGravity) {
        m_myAndChildrenCentersOfGravity->merge(m_centersOfGravity.get());
    }
    
    const int32_t numChildren(rowCount());
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
        const CogSet* childCOG(labelChild->getMyAndChildrenCentersOfGravity());
        CaretAssert(childCOG);
        m_myAndChildrenCentersOfGravity->merge(childCOG);
    }
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

/*
 * @return Reference to centers of gravity for this item
 */
const LabelSelectionItem::CogSet*
LabelSelectionItem::getCentersOfGravity() const
{
    return m_centersOfGravity.get();
}

/*
 * @return Reference to centers of gravity for this item and all children
 */
const LabelSelectionItem::CogSet*
LabelSelectionItem::getMyAndChildrenCentersOfGravity() const
{
    return m_myAndChildrenCentersOfGravity.get();
}

/**
 * Log a warning when there is more that one cluster for a side
 * @param cog
 *    The existing COG
 * @param duplicate
 *    Cluster that wants to replace the COG and is duplicate of it
 * @clusters
 *    Clusters for the COG
 */
void
LabelSelectionItem::multipleClusterWarning(const COG* cog,
                                           const Cluster* duplicate,
                                           const std::vector<const Cluster*>& clusters) const
{
    AString msg("There is more than cluster of the same type (should only be one) type="
                + duplicate->getLocationTypeName());
    msg.appendWithNewLine("   Using: " + cog->toString());
    msg.appendWithNewLine("   Ignoring: " + duplicate->toString());
    msg.appendWithNewLine("   Original clusters: ");
    for (const auto& c : clusters) {
        msg.appendWithNewLine("      " + c->toString());
    }
    CaretLogWarning(msg);
}

/**
 * Set the clusters (may contain multiple, disjoint clusters)
 * @param clusters
 *    Clusters for this item
 */
void
LabelSelectionItem::setClusters(const std::vector<const Cluster*>& clusters)
{
    COG* allCOG(NULL);
    COG* centralCOG(NULL);
    COG* leftCOG(NULL);
    COG* rightCOG(NULL);
    
    bool centralSplitFlag(false);
    
    for (const auto& c : clusters) {
        const AString title(c->getLocationTypeName()
                            + ": "
                            + AString::number(c->getNumberOfBrainordinates())
                            + " Brainordinates; "
                            + "COG "
                            + c->getCenterOfGravityXYZ().toString());
        switch (c->getLocationType()) {
            case Cluster::LocationType::UNKNOWN:
                CaretLogWarning("Cluster is UNKNOWN "
                                + c->toString());
                break;
            case Cluster::LocationType::CENTRAL:
                if (centralCOG != NULL) {
                    multipleClusterWarning(centralCOG, c, clusters);
                }
                else {
                    centralCOG = new COG(title,
                                         c->getCenterOfGravityXYZ(),
                                         c->getNumberOfBrainordinates());
                    centralSplitFlag = c->isSplitClusterFlag();
                }
                break;
            case Cluster::LocationType::LEFT:
                if (leftCOG != NULL) {
                    multipleClusterWarning(leftCOG, c, clusters);
                }
                else {
                    leftCOG = new COG(title,
                                      c->getCenterOfGravityXYZ(),
                                      c->getNumberOfBrainordinates());
                }
                break;
            case Cluster::LocationType::RIGHT:
                if (rightCOG != NULL) {
                    multipleClusterWarning(rightCOG, c, clusters);
                }
                else {
                    rightCOG = new COG(title,
                                       c->getCenterOfGravityXYZ(),
                                       c->getNumberOfBrainordinates());
                }
                break;
        }
    }
    
    /*
     * Create ALL that is combination of the others
     */
    if ((centralCOG != NULL)
        || (leftCOG != NULL)
        || (rightCOG != NULL)) {
        Vector3D zeros;
        int64_t numBrainordinates(0);
        allCOG = new COG("All", zeros, numBrainordinates);
        if (centralCOG != NULL) {
            /*
             * Central is split into right and left clusters.
             * Adding this would add the brainordinates two times.
             */
            if ( ! centralSplitFlag) {
                allCOG->merge(*allCOG);
            }
        }
        if (leftCOG != NULL) {
            allCOG->merge(*leftCOG);
        }
        if (rightCOG != NULL) {
            allCOG->merge(*rightCOG);
        }
        allCOG->updateTitle("All");
    }
    
    m_centersOfGravity.reset(new CogSet(allCOG,
                                               centralCOG,
                                               leftCOG,
                                               rightCOG));
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


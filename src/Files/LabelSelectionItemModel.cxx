
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

#define __LABEL_SELECTION_ITEM_MODEL_DECLARE__
#include "LabelSelectionItemModel.h"
#undef __LABEL_SELECTION_ITEM_MODEL_DECLARE__

#include <set>

#include "CaretAssert.h"
#include "CaretHierarchy.h"
#include "CaretLogger.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "ScenePrimitiveArray.h"

using namespace caret;


    
/**
 * \class caret::LabelSelectionItemModel 
 * \brief Model for label hierarchy
 * \ingroup Files
 */

/**
 * Constructor.
 * @param GiftiLabelTable
 *    GIFTI label tabel from which this hierarchy is build
 * @param displayGroup
 *    The display group
 * @param tabIndex
 *    Index of the tab if displayGroup is TAB
 */
LabelSelectionItemModel::LabelSelectionItemModel(const GiftiLabelTable* giftiLabelTable,
                                                 const DisplayGroupEnum::Enum displayGroup,
                                                 const int32_t tabIndex)
: QStandardItemModel(),
m_displayGroup(displayGroup),
m_tabIndex(tabIndex)
{
    CaretAssert(giftiLabelTable);
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    buildModel(giftiLabelTable);
}

/**
 * Destructor.
 */
LabelSelectionItemModel::~LabelSelectionItemModel()
{
}

/**
 * @return True if this model is valid
 */
bool
LabelSelectionItemModel::isValid() const
{
    return m_validFlag;
}

/**
 * Set the checked status of all items
 * @param checked
 *    The checked status
 */
void
LabelSelectionItemModel::setCheckedStatusOfAllItems(const bool checked)
{
    QStandardItem* rootItem(invisibleRootItem());
    const int32_t numChildren(rootItem->rowCount());
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* childItem(rootItem->child(iRow));
        LabelSelectionItem* labelItem(dynamic_cast<LabelSelectionItem*>(childItem));
        CaretAssert(labelItem);
        labelItem->setAllChildrenChecked(checked);
    }
}

/**
 * Update the checked state of all items
 */
void
LabelSelectionItemModel::updateCheckedStateOfAllItems()
{
    QStandardItem* rootItem(invisibleRootItem());
    const int32_t numChildren(rootItem->rowCount());
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* childItem(rootItem->child(iRow));
        LabelSelectionItem* labelItem(dynamic_cast<LabelSelectionItem*>(childItem));
        CaretAssert(labelItem);
        labelItem->setCheckStateFromChildren();
    }
}

/**
 * @return True if the label with the given key is checked, else false.
 * @param labelKey
 *    The label's key
 */
bool
LabelSelectionItemModel::isLabelChecked(const int32_t labelKey) const
{
    auto iter(m_labelIndexToLabelSelectionItem.find(labelKey));
    if (iter != m_labelIndexToLabelSelectionItem.end()) {
        const LabelSelectionItem* item(iter->second);
        if (item != NULL) {
            return (item->checkState() == Qt::Checked);
        }
    }
    return false;
}

/**
 * Build the tree model from the hierarchy and the label table
 * @param giftiLabelTable
 *    The GIFTI label table
 */
void
LabelSelectionItemModel::buildModel(const GiftiLabelTable* giftiLabelTable)
{
    m_labelIndexToLabelSelectionItem.clear();
    
    const CaretHierarchy& caretHierarchy(giftiLabelTable->getHierarchy());
    if (caretHierarchy.isEmpty()) {
        return;
    }
    const CaretHierarchy::Item& root(caretHierarchy.getInvisibleRoot());
    
    LabelSelectionItem* rootItem(buildTree(&root,
                                           giftiLabelTable));
    invisibleRootItem()->appendRow(rootItem);

    setCheckedStatusOfAllItems(true);
    
    updateCheckedStateOfAllItems();

    m_validFlag = true;
}

/**
 * Build the tree from the items in the hierarchy
 * @param hierarchyItem
 *    Hierarchy item and its children are added
 * @param giftiLabelTable
 *    The GIFTI label table
 */
LabelSelectionItem*
LabelSelectionItemModel::buildTree(const CaretHierarchy::Item* hierarchyItem,
                                   const GiftiLabelTable* giftiLabelTable)
{
    LabelSelectionItem* itemOut(NULL);
    
    CaretAssert(hierarchyItem);
    const int32_t numChildren(hierarchyItem->children.size());
    if (numChildren > 0) {
        itemOut = new LabelSelectionItem(hierarchyItem->name);
        for (int32_t i = 0; i < numChildren; i++) {
            CaretAssertVectorIndex(hierarchyItem->children, i);
            itemOut->appendRow(buildTree(&hierarchyItem->children[i],
                                         giftiLabelTable));
        }
    }
    else {
        AString name(hierarchyItem->name);
        std::array<uint8_t, 4> rgba { 255, 255, 255, 255 };
        int32_t labelIndex(-1);
        const GiftiLabel* label(giftiLabelTable->getLabel(hierarchyItem->name));
        if (label != NULL) {
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
            labelIndex = label->getKey();
        }
        else {
            name.append(" MISSING");
            CaretLogSevere("No label in label table for name from hierarchy: "
                           + hierarchyItem->name);
        }
        
        itemOut = new LabelSelectionItem(name,
                                         labelIndex,
                                         name,
                                         rgba);
        if (labelIndex >= 0) {
            m_labelIndexToLabelSelectionItem[labelIndex] = itemOut;
        }
    }
    
    return itemOut;
}

/**
 * @return All descendants that are of the given type
 * @param itemType
 *    Type for matching to QStandardItem::type()
 * @return
 *    Vector containing the matching items
 */
std::vector<LabelSelectionItem*> 
LabelSelectionItemModel::getAllDescendantsOfType(const LabelSelectionItem::ItemType itemType)
{
    std::vector<LabelSelectionItem*> itemsOut;
    
    QStandardItem* rootItem(invisibleRootItem());
    const int32_t numChildren(rootItem->rowCount());
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* childItem(rootItem->child(iRow));
        LabelSelectionItem* labelItem(dynamic_cast<LabelSelectionItem*>(childItem));
        CaretAssert(labelItem);
        std::vector<LabelSelectionItem*> items(labelItem->getThisAndAllDescendantsOfType(itemType));
        itemsOut.insert(itemsOut.end(),
                        items.begin(), items.end());
    }
    
    return itemsOut;
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
LabelSelectionItemModel::saveToScene(const SceneAttributes* sceneAttributes,
                                     const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "LabelSelectionItemModel",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    /*
     * Save only those labels that are checked
     */
    std::vector<AString> checkedLabelNames;
    const std::vector<LabelSelectionItem*> labelItems(getAllDescendantsOfType(LabelSelectionItem::ItemType::ITEM_LABEL));
    for (const LabelSelectionItem* lsi : labelItems) {
        if (lsi->checkState() == Qt::Checked) {
            checkedLabelNames.push_back(lsi->text());
        }
    }
    
    if ( ! checkedLabelNames.empty()) {
        sceneClass->addStringArray("CheckedLabels",
                                   &checkedLabelNames[0],
                                   checkedLabelNames.size());
    }
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
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
LabelSelectionItemModel::restoreFromScene(const SceneAttributes* sceneAttributes,
                                          const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);    
    
    /*
     * Restore checked status of labels
     */
    bool haveCheckedLabelsFlag(false);
    const ScenePrimitiveArray* checkedLabelsArray(sceneClass->getPrimitiveArray("CheckedLabels"));
    if (checkedLabelsArray != NULL) {
        std::set<AString> checkedLabelNames;
        const int32_t numLabels(checkedLabelsArray->getNumberOfArrayElements());
        for (int32_t i = 0; i < numLabels; i++) {
            checkedLabelNames.insert(checkedLabelsArray->stringValue(i));
        }
        
        if ( ! checkedLabelNames.empty()) {
            haveCheckedLabelsFlag = true;
            
            std::vector<LabelSelectionItem*> labelItems(getAllDescendantsOfType(LabelSelectionItem::ItemType::ITEM_LABEL));
            for (LabelSelectionItem* item : labelItems) {
                if (checkedLabelNames.find(item->text()) != checkedLabelNames.end()) {                    item->setCheckState(Qt::Checked);
                }
                else {
                    item->setCheckState(Qt::Unchecked);
                }
            }
        }
    }
    
    if ( ! haveCheckedLabelsFlag) {
        setCheckedStatusOfAllItems(true);
    }
    
    updateCheckedStateOfAllItems();
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}


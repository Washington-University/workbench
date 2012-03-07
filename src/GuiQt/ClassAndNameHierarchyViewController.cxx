
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

#define __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER_DECLARE__
#include "ClassAndNameHierarchyViewController.h"
#undef __CLASS_AND_NAME_HIERARCHY_VIEW_CONTROLLER_DECLARE__

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QVBoxLayout>

#include "BorderFile.h"
#include "CaretAssert.h"
#include "ClassAndNameHierarchyModel.h"
#include "ClassAndNameHierarchySelectedItem.h"
#include "GiftiLabel.h"
#include "GiftiLabelTable.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::ClassAndNameHierarchyViewController 
 * \brief View controller for ClassAndNameHierarchyModels
 *
 * A view controller for one or more ClassAndNameHierarchyModel
 * instances.
 */

/**
 * Constructor.
 * @param parent
 *    Parent widget.
 */
ClassAndNameHierarchyViewController::ClassAndNameHierarchyViewController(QWidget* parent)
: QWidget(parent)
{
    this->treeWidget = new QTreeWidget();
    this->treeWidget->setColumnCount(1);
    QObject::connect(this->treeWidget, SIGNAL(itemChanged(QTreeWidgetItem*,int)),
                     this, SLOT(treeWidgetItemChanged(QTreeWidgetItem*,int)));
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutMargins(layout, 0, 0);
    layout->addWidget(this->treeWidget);
    
}

/**
 * Destructor.
 */
ClassAndNameHierarchyViewController::~ClassAndNameHierarchyViewController()
{
    this->deleteItemSelectionInfo();
}

/**
 * Called when an item in the border selection tree widget
 * is changed.
 */
void 
ClassAndNameHierarchyViewController::treeWidgetItemChanged(QTreeWidgetItem* item,
                                                          int column)
{
    std::cout << item->text(column) << " was selected. " << std::endl;
    
    void* ptr = item->data(0, Qt::UserRole).value<void*>();
    ClassAndNameHierarchySelectedItem* selectionInfo = (ClassAndNameHierarchySelectedItem*)ptr;
    
    const bool isSelected = (item->checkState(0) == Qt::Checked);
    
    switch (selectionInfo->getItemType()) {
        case ClassAndNameHierarchySelectedItem::ITEM_TYPE_HIERARCHY_MODEL:
        {
            ClassAndNameHierarchyModel* hierarchyModel = selectionInfo->getClassAndNameHierarchyModel();
            CaretAssert(hierarchyModel);
            hierarchyModel->setSelected(isSelected);
        }
            break;
        case ClassAndNameHierarchySelectedItem::ITEM_TYPE_CLASS:
        case ClassAndNameHierarchySelectedItem::ITEM_TYPE_NAME:
        {
            GiftiLabel* giftiLabel = selectionInfo->getDataAsGiftiLabel();
            CaretAssert(giftiLabel);
            giftiLabel->setSelected(isSelected);
        }
            break;
    }
    emit itemSelected(selectionInfo);
}

/**
 * Update with border files.
 * @param borderFiles
 *    The border files.
 */
void 
ClassAndNameHierarchyViewController::updateContents(std::vector<BorderFile*> borderFiles)
{
    std::vector<ClassAndNameHierarchyModel*> classAndNameHierarchyModels;
    std::vector<void*> borderFilePointers;
    for (std::vector<BorderFile*>::iterator iter = borderFiles.begin();
         iter != borderFiles.end();
         iter++) {
        BorderFile* bf = *iter;
        CaretAssert(bf);
        classAndNameHierarchyModels.push_back(bf->getClassAndNameHierarchy());
        borderFilePointers.push_back((void*)bf);
    }
    
    this->updateContents(classAndNameHierarchyModels);
}

void 
ClassAndNameHierarchyViewController::deleteItemSelectionInfo()
{
    for (std::vector<ClassAndNameHierarchySelectedItem*>::iterator itemSelIter = this->itemSelectionInfo.begin();
         itemSelIter != this->itemSelectionInfo.end();
         itemSelIter++) {
        delete *itemSelIter;
    }
    this->itemSelectionInfo.clear();
}


/**
 * Update the content of the view controller.
 * @param classAndNameHierarchyModels
 *    ClassAndNameHierarchyModels instances for display.
 * @param DataType
 *    Type of data for the class/name hierarchy.
 */
void 
ClassAndNameHierarchyViewController::updateContents(std::vector<ClassAndNameHierarchyModel*>& classAndNameHierarchyModels)
{
    /*
     * Copy the models
     */
    this->classAndNameHierarchyModels = classAndNameHierarchyModels;
    
    /*
     * Remove everything from the tree widget
     */
    this->treeWidget->blockSignals(true);
    this->treeWidget->clear();
    this->deleteItemSelectionInfo();
    
    /*
     * Loop through the models.
     */
    const int32_t numberOfModels = static_cast<int32_t>(this->classAndNameHierarchyModels.size());
    for (int32_t iModel = 0; iModel < numberOfModels; iModel++) {
        const ClassAndNameHierarchyModel* classNamesModel = this->classAndNameHierarchyModels[iModel];
        
        QList<QTreeWidgetItem*> classTreeWidgets;
        
        /*
         * Loop through each class
         */
        const GiftiLabelTable* classTable = classNamesModel->getClassLabelTable();
        const std::vector<int32_t> classKeys = classTable->getLabelKeysSortedByName();
        for (std::vector<int32_t>::const_iterator classIter = classKeys.begin();
             classIter != classKeys.end();
             classIter++) {
            const int32_t classKey = *classIter;
            const GiftiLabel* classLabel = classTable->getLabel(classKey);
            CaretAssert(classLabel);
                
            /*
             * Names in the model
             */
            QList<QTreeWidgetItem*> nameTreeWidgets;
            const GiftiLabelTable* namesTable = classNamesModel->getNameLabelTableForClass(classKey);
            const std::vector<int32_t> nameKeys = namesTable->getLabelKeysSortedByName();
            for (std::vector<int32_t>::const_iterator nameIter = nameKeys.begin();
                 nameIter != nameKeys.end();
                 nameIter++) {
                const int32_t nameKey = *nameIter;
                const GiftiLabel* nameLabel = namesTable->getLabel(nameKey);
                CaretAssert(nameLabel);
                if (nameLabel->getCount() > 0) {
                    ClassAndNameHierarchySelectedItem* nameInfo = 
                    new ClassAndNameHierarchySelectedItem(ClassAndNameHierarchySelectedItem::ITEM_TYPE_NAME,
                                                           (void*)nameLabel);
                    QTreeWidgetItem* nameItem = this->createTreeWidgetItem(nameLabel->getName(),
                                                                           nameLabel->isSelected(),
                                                                           nameInfo);
                    nameTreeWidgets.append(nameItem);
                }
            }
            
            /*
             * Only create a class item if it has valid child names
             */
            if (nameTreeWidgets.empty() == false) {                    
                /*
                 * Item for class
                 */
                ClassAndNameHierarchySelectedItem* classInfo = 
                   new ClassAndNameHierarchySelectedItem(ClassAndNameHierarchySelectedItem::ITEM_TYPE_CLASS,
                                                       (void*)classLabel);
                QTreeWidgetItem* classItem = this->createTreeWidgetItem(classLabel->getName(),
                                                                        classLabel->isSelected(),
                                                                        classInfo);
                classItem->addChildren(nameTreeWidgets);
                
                classTreeWidgets.append(classItem);
            }
        }
        
        /*
         * If there are any valid, classes in the model, add a class/name model in the tree widget
         */
        if (classTreeWidgets.empty() == false) {
            ClassAndNameHierarchySelectedItem* modelInfo = 
               new ClassAndNameHierarchySelectedItem(ClassAndNameHierarchySelectedItem::ITEM_TYPE_HIERARCHY_MODEL,
                                                       (void*)classNamesModel);
            QTreeWidgetItem* modelItem = this->createTreeWidgetItem(classNamesModel->getName(), 
                                                                    true, 
                                                                    modelInfo);
            modelItem->addChildren(classTreeWidgets);
            
            this->treeWidget->addTopLevelItem(modelItem);
        }
    }
    
    
    this->treeWidget->expandAll();
    
    this->treeWidget->blockSignals(false);
}

/**
 * Create a tree widget item that is checkable.
 * @param text
 *    Text displayed inh item.
 * @param isSelected
 *    True if item is selected.
 */
QTreeWidgetItem*
ClassAndNameHierarchyViewController::createTreeWidgetItem(const AString& name,
                                                          const bool isSelected,
                                                          ClassAndNameHierarchySelectedItem* selectionInfo)
{
    QTreeWidgetItem* twi = new QTreeWidgetItem();
    twi->setText(0, name);
    twi->setFlags(Qt::ItemIsUserCheckable
                  | Qt::ItemIsEnabled);
    if (isSelected) {
        twi->setCheckState(0, Qt::Checked);
    }
    else {
        twi->setCheckState(0, Qt::Unchecked);
    }
    twi->setData(0, Qt::UserRole, qVariantFromValue((void*)selectionInfo));
    
    this->itemSelectionInfo.push_back(selectionInfo);
    
    return twi;
}


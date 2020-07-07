
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __DYN_CONN_VIEW_CONTROLLER_DECLARE__
#include "DynConnViewController.h"
#undef __DYN_CONN_VIEW_CONTROLLER_DECLARE__

#include <QStandardItem>
#include <QStandardItemModel>
#include <QTreeView>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "EventManager.h"
using namespace caret;


    
/**
 * \class caret::DynConnViewController 
 * \brief Prototype for dynamic connectivity controller
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
DynConnViewController::DynConnViewController()
: QWidget()
{
    std::vector<AString> types { "Correlation", "Covariance", "Fisher Z" };
    
    QStringList labels;
    labels
    << "Mode"
    << "Load"
    << "Layer"
    << "Demean"
    << "Copy   "
    << "Name";
    
    QStandardItemModel* model = new QStandardItemModel(0, 6);
    model->setHorizontalHeaderLabels(labels);
    
    std::vector<QModelIndex> topModelIndices;
    
    const int32_t numFiles = 3;
    for (int32_t i = 0; i < numFiles; i++) {
        QString name("File_"
                     + AString::number(i + 1));
        QStandardItem* topItem = new QStandardItem();
        topItem->setText(name
                         + ".dtseries.nii (/mnt/myelin/caret7_gui_design/data/temp)");
        
        for (auto nt : types) {
            const bool addNamesFlag(false);
            QStandardItem* loadItem = new QStandardItem();
            loadItem->setCheckable(true);
            if (addNamesFlag) {
                loadItem->setText("Load");
            }
            
            QStandardItem* layerItem = new QStandardItem();
            layerItem->setCheckable(true);
            if (addNamesFlag) {
                layerItem->setText("Layer");
            }

            QStandardItem* demeanItem = new QStandardItem();
            demeanItem->setCheckable(true);
            if (addNamesFlag) {
                demeanItem->setText("Demean");
            }

            QStandardItem* copyItem = new QStandardItem();
            copyItem->setText("Copy");
            
            QStandardItem* typeItem = new QStandardItem();
            typeItem->setText(nt);
            
            QStandardItem* fileItem = new QStandardItem();
            fileItem->setText(nt
                              + "_"
                              + name
                              + ".dynconn.nii");
            
            QList<QStandardItem*> items;
            items.push_back(typeItem);
            items.push_back(loadItem);
            items.push_back(layerItem);
            items.push_back(demeanItem);
            items.push_back(copyItem);
            items.push_back(fileItem);
            
            topItem->appendRow(items);
        }
        
        model->appendRow(topItem);
        topModelIndices.push_back(model->indexFromItem(topItem));
    }
    
    m_treeView = new QTreeView();
    m_treeView->setModel(model);
    
    const int32_t numRows(static_cast<int32_t>(topModelIndices.size()));
    for (int32_t i = 0; i < numRows; i++) {
        m_treeView->setFirstColumnSpanned(i, m_treeView->rootIndex(), true);
    }
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_treeView);
/*    EventManager::get()->addEventListener(this, EventTypeEnum::); */

    for (int32_t i = 0; i < (model->columnCount() - 1); i++) {
        m_treeView->resizeColumnToContents(i);
    }
    
}

/**
 * Destructor.
 */
DynConnViewController::~DynConnViewController()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
DynConnViewController::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}


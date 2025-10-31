
/*LICENSE_START*/
/*
 *  Copyright (C) 2025 Washington University School of Medicine
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

#define __LABEL_SELECTION_ITEM_MODEL_PROXY_FILTER_DECLARE__
#include "LabelSelectionItemModelProxyFilter.h"
#undef __LABEL_SELECTION_ITEM_MODEL_PROXY_FILTER_DECLARE__

#include <iostream>

#include "CaretAssert.h"
#include "DisplayPropertiesLabels.h"
#include "EventDisplayPropertiesLabels.h"
#include "EventManager.h"
#include "LabelSelectionItem.h"

using namespace caret;

#include <QAbstractItemModel>
#include <QStandardItem>

    
/**
 * \class caret::LabelSelectionItemModelProxyFilter 
 * \brief Proxy filter to remove disabled items from display
 * \ingroup Files
 */

/**
 * Constructor.
 */
LabelSelectionItemModelProxyFilter::LabelSelectionItemModelProxyFilter(QObject* parent)
: QSortFilterProxyModel(parent),
EventListenerInterface()
{
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_DISPLAY_PROPERTIES_LABELS);
}

/**
 * Destructor.
 */
LabelSelectionItemModelProxyFilter::~LabelSelectionItemModelProxyFilter()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return true if the item in the row indicated by the given source_row and source_parent should be included in the model; otherwise returns false.
 */
bool
LabelSelectionItemModelProxyFilter::filterAcceptsRow(int sourceRow,
                                                     const QModelIndex& sourceParent) const
{
    if ( ! m_showHideStatusValid) {
        /*
         * The show/hide status is not valid so update it
         */
        m_showHideStatusValid = true;
        
        EventDisplayPropertiesLabels eventDPL(EventDisplayPropertiesLabels::Mode::GET);
        EventManager::get()->sendEvent(eventDPL.getPointer());
        const DisplayPropertiesLabels* dpl(eventDPL.getDisplayPropertiesLabels());
        CaretAssert(dpl);
        m_showUnusedLabelsFlag = dpl->isShowUnusedLabelsInHierarchies();
    }
    
    QModelIndex index(sourceModel()->index(sourceRow, 0, sourceParent));
    QStandardItemModel* smi(dynamic_cast<QStandardItemModel*>(sourceModel()));
    CaretAssert(smi);
    QStandardItem* item(smi->itemFromIndex(index));
    CaretAssert(item);
    
    /*
     * Note that unused labels are disabled
     */
    if (item->isEnabled()) {
        LabelSelectionItem* labelItem(dynamic_cast<LabelSelectionItem*>(item));
        if (labelItem != NULL) {
            const LabelSelectionItem::CogSet* allCogSet(labelItem->getMyAndChildrenCentersOfGravity());
            if (allCogSet != NULL) {
                const std::vector<const LabelSelectionItem::COG*>& cogs(allCogSet->getCOGs());
                if (cogs.empty()) {
                    if ( ! m_showBranchesWithoutLabelsFlag) {
                        return false;
                    }
                }
            }
        }
        return true;
    }
    if (m_showUnusedLabelsFlag) {
        return true;
    }
    
    return false;
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
LabelSelectionItemModelProxyFilter::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_DISPLAY_PROPERTIES_LABELS) {
        EventDisplayPropertiesLabels* eventDPL(dynamic_cast<EventDisplayPropertiesLabels*>(event));
        CaretAssert(eventDPL);
        switch (eventDPL->getMode()) {
            case EventDisplayPropertiesLabels::Mode::GET:
                break;
            case EventDisplayPropertiesLabels::Mode::SEND_SHOW_UNUSED_LABELS_CHANGED:
            {
                /*
                 * The show/hide status has changed to update the filter
                 */
                const DisplayPropertiesLabels* dpl(eventDPL->getDisplayPropertiesLabels());
                CaretAssert(dpl);
                m_showUnusedLabelsFlag = dpl->isShowUnusedLabelsInHierarchies();
                m_showBranchesWithoutLabelsFlag = dpl->isShowBranchesWithoutLabelsInHierarchies();
                m_showHideStatusValid  = true;
                
                /*
                 * Method in proxy filter that invalidates the filter
                 * so that the view (tree view) gets updated
                 */
                invalidate();
            }
                break;
        }
    }
}

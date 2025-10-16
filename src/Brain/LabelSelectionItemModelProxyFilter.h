#ifndef __LABEL_SELECTION_ITEM_MODEL_PROXY_FILTER_H__
#define __LABEL_SELECTION_ITEM_MODEL_PROXY_FILTER_H__

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



#include <memory>

#include <QSortFilterProxyModel>

#include "EventListenerInterface.h"

namespace caret {

    class LabelSelectionItemModelProxyFilter : public QSortFilterProxyModel, public EventListenerInterface {
        
        Q_OBJECT

    public:
        LabelSelectionItemModelProxyFilter(QObject* parent);
        
        virtual ~LabelSelectionItemModelProxyFilter();
        
        LabelSelectionItemModelProxyFilter(const LabelSelectionItemModelProxyFilter&) = delete;

        LabelSelectionItemModelProxyFilter& operator=(const LabelSelectionItemModelProxyFilter&) = delete;
        
        virtual void receiveEvent(Event* event) override;

    protected:
        virtual bool filterAcceptsRow(int sourceRow,
                                      const QModelIndex& sourceParent) const override;
        
        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE

        /*
         * If true, show unused labels, else hide them
         */
        mutable bool m_showUnusedLabelsFlag = false;
        
        /**
         * If true, m_showUnusedLabelsFlag is valid;
         * If false, m_showUnusedLabelsFlag needs to be updated
         */
        mutable bool m_showHideStatusValid = false;
        
    };
    
#ifdef __LABEL_SELECTION_ITEM_MODEL_PROXY_FILTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __LABEL_SELECTION_ITEM_MODEL_PROXY_FILTER_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_ITEM_MODEL_PROXY_FILTER_H__

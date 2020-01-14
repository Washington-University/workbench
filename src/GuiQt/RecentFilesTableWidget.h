#ifndef __RECENT_FILES_TABLE_WIDGET_H__
#define __RECENT_FILES_TABLE_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include <QTableWidget>

#include "AString.h"
#include "RecentFileItemSortingKeyEnum.h"
#include "RecentFileItemsFilter.h"

class QLabel;
class QTableWidgetItem;

namespace caret {
    class CaretMappableDataFile;
    class RecentFileItem;
    class RecentFileItemsContainer;
    
    class RecentFilesTableWidget : public QTableWidget {
        
        Q_OBJECT

    public:
        RecentFilesTableWidget();
        
        virtual ~RecentFilesTableWidget();
        
        RecentFilesTableWidget(const RecentFilesTableWidget&) = delete;

        RecentFilesTableWidget& operator=(const RecentFilesTableWidget&) = delete;
        
        void updateContent(RecentFileItemsContainer* recentFileItemsContainer,
                           const RecentFileItemsFilter& itemsFilter);

        RecentFileItem* getSelectedItem();
        
        virtual QSize sizeHint() const override;
        
        // ADD_NEW_METHODS_HERE

    signals:
        void sortingChanged();
        
        void selectedItemChanged(RecentFileItem* item);
        
        void selectedItemDoubleClicked(RecentFileItem* item);
        
    private slots:
        void tableCellClicked(int row, int column);
        
        void tableCellDoubleClicked(int row, int column);
        
        void sortIndicatorClicked(int locicalIndex,
                                  Qt::SortOrder sortOrder);
        
    private:
        enum COLUMNS {
            COLUMN_NAME = 0,
            COLUMN_DATE_TIME = 1,
            COLUMN_FAVORITE = 2,
            COLUMN_SHARE = 3,
            COLUMN_FORGET = 4,
            COLUMN_COUNT = 5
        };

        AString getColumnName(const int32_t) const;
        
        void updateTableDimensions(const int32_t numberOfItems);
        
        void updateRow(const int32_t rowIndex);
        
        std::unique_ptr<QIcon> loadIcon(const AString& iconFileName) const;
        
        void resizeColumnAsNeeded();
        
        void clearSelectedItem();
        
        void sortRecentItems();
        
        void updateHeaderSortingKey();
        
        void showShareMenuForRow(const int32_t rowIndex);
        
        int32_t m_lastNumberRecentItems = 0;
        
        RecentFileItemsContainer* m_recentFileItemsContainer = NULL;
        
        RecentFileItemsFilter m_recentFileItemsFilter;
        
        std::vector<RecentFileItem*> m_recentItems;
        
        std::vector<CaretMappableDataFile*> m_mapFiles;
        
        std::unique_ptr<QIcon> m_favoriteFilledIcon;
        
        std::unique_ptr<QIcon> m_favoriteOutlineIcon;
        
        std::unique_ptr<QIcon> m_shareIcon;
        
        std::unique_ptr<QIcon> m_forgetIcon;
        
        std::unique_ptr<QIcon> m_forgetOnIcon;
        
        RecentFileItemSortingKeyEnum::Enum m_sortingKey = RecentFileItemSortingKeyEnum::DATE_NEWEST;

        static const int32_t s_pixmapSizeXY;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RECENT_FILES_TABLE_WIDGET_DECLARE__
    
    const int32_t RecentFilesTableWidget::s_pixmapSizeXY = 24;
    
#endif // __RECENT_FILES_TABLE_WIDGET_DECLARE__

} // namespace
#endif  //__RECENT_FILES_TABLE_WIDGET_H__

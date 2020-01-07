#ifndef __RECENT_FILE_ITEMS_FILTER_H__
#define __RECENT_FILE_ITEMS_FILTER_H__

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



#include <memory>

#include "CaretObject.h"

class QRegularExpression;

namespace caret {

    class RecentFileItem;
    
    class RecentFileItemsFilter : public CaretObject {
        
    public:
        RecentFileItemsFilter();
        
        virtual ~RecentFileItemsFilter();
        
        RecentFileItemsFilter(const RecentFileItemsFilter& obj);

        RecentFileItemsFilter& operator=(const RecentFileItemsFilter& obj);
        
        bool testItemPassesFilter(const RecentFileItem* recentFileItem) const;
        
        AString getNameMatching() const;
        
        void setNameMatching(const AString& nameMatching);
        
        bool isShowSpecFiles() const;
        
        void setShowSpecFiles(const bool showSpecFiles);
        
        bool isShowSceneFiles() const;
        
        void setShowSceneFiles(const bool showSceneFiles);

        bool isShowDirectories() const;
        
        void setShowDirectories(const bool showDirectories);
        
        bool isFavoritesOnly() const;
        
        void setFavoritesOnly(const bool favoritesOnly);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperRecentFileItemsFilter(const RecentFileItemsFilter& obj);

        /** name matching (glob)*/
        AString m_nameMatching;
        
        /** Show spec files*/
        bool m_showSpecFiles = false;
        
        /** Show scene files*/
        bool m_showSceneFiles = false;

        /** show directories*/
        bool m_showDirectories = false;;
        
        /** show only favorites*/
        bool m_favoritesOnly = false;
        
        mutable std::unique_ptr<QRegularExpression> m_regularExpression;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RECENT_FILE_ITEMS_FILTER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __RECENT_FILE_ITEMS_FILTER_DECLARE__

} // namespace
#endif  //__RECENT_FILE_ITEMS_FILTER_H__

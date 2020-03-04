#ifndef __RECENT_FILE_ITEM_H__
#define __RECENT_FILE_ITEM_H__

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

#include <QDateTime>

#include <memory>

#include "CaretObjectTracksModification.h"
#include "RecentFileItemTypeEnum.h"

class QXmlStreamWriter;

namespace caret {

    class RecentFileItem : public CaretObjectTracksModification {
        
    public:
        RecentFileItem(const RecentFileItemTypeEnum::Enum fileItemType,
                       const AString& pathAndFileName);
        
        virtual ~RecentFileItem();
        
        RecentFileItem(const RecentFileItem& obj);

        RecentFileItem& operator=(const RecentFileItem& obj);
        
        bool operator<(const RecentFileItem& obj) const;

        bool operator==(const RecentFileItem& obj) const;
        
        RecentFileItemTypeEnum::Enum getFileItemType() const;
        
        AString getLastAccessByWorkbenchDateTimeAsString() const;
        
        void setLastAccessByWorkbenchDateTimeFromString(const AString& dateTimeString);
        
        QDateTime getLastAccessByWorkbenchDateTime() const;
        
        void setLastAccessByWorkbenchDateTime(const QDateTime& dateTime);

        void setLastAccessByWorkbenchDateTimeToCurrentDateTime();
        
        QDateTime getLastModifiedDateTime() const;
        
        AString getLastModifiedDateTimeAsString() const;
        
        AString getPathAndFileName() const;
        
        AString getPathName() const;
        
        AString getFileName() const;
        
        AString getComment() const;
        
        void setComment(const AString& text);
        
        bool isFavorite() const;
        
        void setFavorite(const bool status);
        
        bool isNotFound() const;
        
        bool isForget() const;
        
        void setForget(const bool status);

        // ADD_NEW_METHODS_HERE

        virtual AString toString() const;
        
    private:
        void copyHelperRecentFileItem(const RecentFileItem& obj);

        RecentFileItemTypeEnum::Enum m_fileItemType;
        
        QDateTime m_lastAccessDateTime;
        
        QDateTime m_lastModifiedDateTime;
        
        AString m_pathAndFileName;
        
        AString m_fileName;
        
        AString m_pathName;
        
        AString m_comment;
        
        bool m_favoriteFlag = false;
        
        bool m_notFoundFlag = false;
        
        bool m_forgetFlag = false;
        
        static const Qt::DateFormat s_qtStringDateFormat = Qt::TextDate;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __RECENT_FILE_ITEM_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __RECENT_FILE_ITEM_DECLARE__

} // namespace
#endif  //__RECENT_FILE_ITEM_H__

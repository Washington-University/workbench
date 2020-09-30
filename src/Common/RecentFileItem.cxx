
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

#define __RECENT_FILE_ITEM_DECLARE__
#include "RecentFileItem.h"
#undef __RECENT_FILE_ITEM_DECLARE__

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventManager.h"
#include "EventRecentFilesSystemAccessMode.h"
#include "FileInformation.h"

using namespace caret;


    
/**
 * \class caret::RecentFileItem 
 * \brief Contains data for one recent file
 * \ingroup Common
 */

/**
 * Constructor.
 * @param fileItemType
 *  Type of file
 * @param pathAndFileName
 *  Path and name of a file
 */
RecentFileItem::RecentFileItem(const RecentFileItemTypeEnum::Enum fileItemType,
                               const AString& pathAndFileName)
: CaretObjectTracksModification(),
m_fileItemType(fileItemType),
m_pathAndFileName(pathAndFileName)
{
    bool useFileSystemFlag(false);
    EventRecentFilesSystemAccessMode modeEvent;
    EventManager::get()->sendEvent(modeEvent.getPointer());
    switch (modeEvent.getMode()) {
        case RecentFilesSystemAccessModeEnum::OFF:
            break;
        case RecentFilesSystemAccessModeEnum::ON:
            useFileSystemFlag = true;
            break;
    }
    
    if (useFileSystemFlag) {
        FileInformation fileInfo(pathAndFileName);
        m_pathAndFileName = fileInfo.getAbsoluteFilePath();
        switch (m_fileItemType) {
            case RecentFileItemTypeEnum::DIRECTORY:
                m_pathName = m_pathAndFileName; /* directory !!! */
                break;
            case RecentFileItemTypeEnum::SCENE_FILE:
            case RecentFileItemTypeEnum::SPEC_FILE:
                m_pathName = fileInfo.getAbsolutePath();
                break;
        }

        m_fileName = fileInfo.getFileName();
        m_notFoundFlag = ( ! fileInfo.exists());
        m_lastModifiedDateTime = fileInfo.getLastModified();
    }
    else {
        m_pathAndFileName = pathAndFileName;
        switch (m_fileItemType) {
            case RecentFileItemTypeEnum::DIRECTORY:
                m_pathName = m_pathAndFileName; /* directory !!! */
                break;
            case RecentFileItemTypeEnum::SCENE_FILE:
            case RecentFileItemTypeEnum::SPEC_FILE:
            {
                const int lastSlash = m_pathAndFileName.lastIndexOf('/');
                m_pathName = "";
                if (lastSlash >= 0) {
                    m_pathName = m_pathAndFileName.left(lastSlash);
                }
            }
                break;
        }
        
        const int lastSlash = m_pathAndFileName.lastIndexOf('/');
        if (lastSlash >= 0) {
            m_fileName = m_pathAndFileName.mid(lastSlash);
        }
        m_notFoundFlag = false;
        m_lastModifiedDateTime = QDateTime();
    }
    
    m_forgetFlag   = false;
    m_favoriteFlag = false;
    m_lastAccessDateTime = QDateTime(); /* invalid date/time */
}

/**
 * Destructor.
 */
RecentFileItem::~RecentFileItem()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
RecentFileItem::RecentFileItem(const RecentFileItem& obj)
: CaretObjectTracksModification(obj)
{
    this->copyHelperRecentFileItem(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
RecentFileItem&
RecentFileItem::operator=(const RecentFileItem& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperRecentFileItem(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
RecentFileItem::copyHelperRecentFileItem(const RecentFileItem& obj)
{
    m_fileItemType       = obj.m_fileItemType;
    m_lastAccessDateTime = obj.m_lastAccessDateTime;
    m_pathAndFileName    = obj.m_pathAndFileName;
    m_pathName           = obj.m_pathName;
    m_fileName           = obj.m_fileName;
    m_comment            = obj.m_comment;
    m_forgetFlag         = obj.m_forgetFlag;
    m_favoriteFlag       = obj.m_favoriteFlag;
    m_notFoundFlag       = obj.m_notFoundFlag;
    m_lastModifiedDateTime = obj.m_lastModifiedDateTime;
}

/**
 * Less than operator that compares by path and name
 * @param obj
 *    Instance compared to this for less than comparison.
 * @return 
 *    True if this instance is less than 'obj' instance.
 */
bool
RecentFileItem::operator<(const RecentFileItem& obj) const
{
    if (this == &obj) {
        return false;
    }

    return (m_pathAndFileName < obj.m_pathAndFileName);
}

/**
 * Equality operator that compares by path and name
 * @param obj
 *    Instance compared to this for equality.
 * @return
 *    True if this instance is less than 'obj' instance.
 */
bool
RecentFileItem::operator==(const RecentFileItem& obj) const
{
    if (this == &obj) {
        return true;
    }
    
    return (m_pathAndFileName == obj.m_pathAndFileName);
}

/**
 * @return The type of item
 */
RecentFileItemTypeEnum::Enum
RecentFileItem::getFileItemType() const
{
    return m_fileItemType;
}

/**
 * @return Time last accessed by wb_view
 */
QDateTime
RecentFileItem::getLastAccessByWorkbenchDateTime() const
{
    return m_lastAccessDateTime;
}

/**
 * @return Time last accessed by wb_view
 */
AString
RecentFileItem::getLastAccessByWorkbenchDateTimeAsString() const
{
    AString s(m_lastAccessDateTime.toString(s_qtStringDateFormat));
    return s;
}

/**
 * Set last accessed by wb_view time from a string
 * @param dateTimeString
 * Date/Time that must be in a valid date/time format
 */
void
RecentFileItem::setLastAccessByWorkbenchDateTimeFromString(const AString& dateTimeString)
{
    if (dateTimeString.isEmpty()) {
        m_lastAccessDateTime = QDateTime();
        return;
    }
    
    m_lastAccessDateTime = QDateTime::fromString(dateTimeString,
                                                 s_qtStringDateFormat);
    if (m_lastAccessDateTime.isNull()) {
        CaretLogWarning("Invalid date/time string \""
                        + dateTimeString
                        + "\"");
    }
}

/**
 * Set last accessed time by wb_view
 * @param dateTime
 * Last time accessed
 */
void
RecentFileItem::setLastAccessByWorkbenchDateTime(const QDateTime& dateTime)
{
    m_lastAccessDateTime = dateTime;
    setModified();
}

/**
 * Set last accessed by wb_view time to the current date/time
 */
void
RecentFileItem::setLastAccessByWorkbenchDateTimeToCurrentDateTime()
{
    setLastAccessByWorkbenchDateTime(QDateTime::currentDateTime());
}

/**
 * @return Last time item was modiifed as reported by the operating system
 */
QDateTime
RecentFileItem::getLastModifiedDateTime() const
{
    return m_lastModifiedDateTime;
}

/**
 * @return Last time item was modiifed as a string as reported by the operating system
 */
AString
RecentFileItem::getLastModifiedDateTimeAsString() const
{
    AString s(m_lastModifiedDateTime.toString(s_qtStringDateFormat));
    return s;
}

/**
 * @return The path and file name
 */
AString
RecentFileItem::getPathAndFileName() const
{
    return m_pathAndFileName;
}

/**
 * @return The path name
 */
AString
RecentFileItem::getPathName() const
{
    return m_pathName;
}

/**
 * @return The file name
 */
AString
RecentFileItem::getFileName() const
{
    return m_fileName;
}

/**
 * @return User comment
 */
AString
RecentFileItem::getComment() const
{
    return m_comment;
}

/**
 * Set the user's comment
 * @param text
 *     New text for user's comment
 */
void
RecentFileItem::setComment(const AString& text)
{
    m_comment = text;
    setModified();
}

/**
 * @return True if this is a favorite
 */
bool
RecentFileItem::isFavorite() const
{
    return m_favoriteFlag;
}

/**
 * Set the favorite status
 * @param status
 * New status
 */
void
RecentFileItem::setFavorite(const bool status)
{
    m_favoriteFlag = status;
    setModified();
}

/**
 * @return True if this item is not found
 */
bool
RecentFileItem::isNotFound() const
{
    return m_notFoundFlag;
}

/**
 * @return True if this item is to forgtten
 */
bool
RecentFileItem::isForget() const
{
    return m_forgetFlag;
}

/**
 * Set the forget status
 * @param status
 * New status
 */
void
RecentFileItem::setForget(const bool status)
{
    m_forgetFlag = status;
    setModified();
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
RecentFileItem::toString() const
{
    return "RecentFileItem";
}


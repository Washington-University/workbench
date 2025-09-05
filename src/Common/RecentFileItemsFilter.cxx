
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

#define __RECENT_FILE_ITEMS_FILTER_DECLARE__
#include "RecentFileItemsFilter.h"
#undef __RECENT_FILE_ITEMS_FILTER_DECLARE__

#include <QRegularExpression>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "RecentFileItem.h"

using namespace caret;

/**
 * \class caret::RecentFileItemsFilter 
 * \brief Filters recent file items
 * \ingroup Common
 */

/**
 * Constructor with all items off (filter will reject all items).
 */
RecentFileItemsFilter::RecentFileItemsFilter()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
RecentFileItemsFilter::~RecentFileItemsFilter()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
RecentFileItemsFilter::RecentFileItemsFilter(const RecentFileItemsFilter& obj)
: CaretObject(obj)
{
    this->copyHelperRecentFileItemsFilter(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return 
 *    Reference to this object.
 */
RecentFileItemsFilter&
RecentFileItemsFilter::operator=(const RecentFileItemsFilter& obj)
{
    if (this != &obj) {
        CaretObject::operator=(obj);
        this->copyHelperRecentFileItemsFilter(obj);
    }
    return *this;    
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void 
RecentFileItemsFilter::copyHelperRecentFileItemsFilter(const RecentFileItemsFilter& obj)
{
    m_favoritesOnly   = obj.m_favoritesOnly;
    m_nameMatching    = obj.m_nameMatching;
    m_listDirectories = obj.m_listDirectories;
    m_listSceneFiles  = obj.m_listSceneFiles;
    m_listSpecFiles   = obj.m_listSpecFiles;
    m_showFilePaths   = obj.m_showFilePaths;
}

/**
 * @return True if the items passes the filters selections, else false.
 * @param recentFileItem
 *   Item that is tested to pass this filter's selections
 */
bool
RecentFileItemsFilter::testItemPassesFilter(const RecentFileItem* recentFileItem) const
{
    if (recentFileItem == NULL) {
        return false;
    }
    
    if (m_favoritesOnly) {
        if ( ! recentFileItem->isFavorite()) {
            return false;
        }
    }
    
    switch (recentFileItem->getFileItemType()) {
        case RecentFileItemTypeEnum::DIRECTORY:
            if ( ! m_listDirectories) {
                return false;
            }
            break;
        case RecentFileItemTypeEnum::EXAMPLE_SCENE:
            if ( ! m_listSceneFiles) {
                return false;
            }
            break;
        case RecentFileItemTypeEnum::SCENE_FILE:
            if ( ! m_listSceneFiles) {
                return false;
            }
            break;
        case RecentFileItemTypeEnum::SCENE_IN_SCENE_FILE:
            if ( ! m_listSceneFiles) {
                return false;
            }
            break;
        case RecentFileItemTypeEnum::SPEC_FILE:
            if ( ! m_listSpecFiles) {
                return false;
            }
            break;
    }
    
    if ( ! m_nameMatching.isEmpty()) {
        /*
         * NOTE: QRegularExpression::wildcardToRegularExpression() added in Qt 5.12
         */
        if ( ! m_regularExpression) {
            QString reText(m_nameMatching);
            /* convert glob matches to regular expression text */
            reText.replace("*", ".*");
            reText.replace("?", ".?");
            m_regularExpression.reset(new QRegularExpression(reText));
            m_regularExpression->setPatternOptions(QRegularExpression::CaseInsensitiveOption);
            if ( ! m_regularExpression->isValid()) {
                CaretLogFine("Regular expression failure for RecentFileItem: "
                               "Name matching \""
                               + m_nameMatching
                               + "\" converted to regular expression \""
                               + reText
                               + "\" error message \""
                               + m_regularExpression->errorString()
                               + "\"");
            }
        }
        
        if (m_regularExpression != NULL) {
            if (m_regularExpression->isValid()) {
                if ( ! m_regularExpression->match(recentFileItem->getPathAndFileName()).hasMatch()) {
                    return false;
                }
            }
        }
    }
    
    /*
     * Passed all filtering
     */
    return true;
}

/**
 * @return Tooltip for the matching line edit in the GUI
 */
AString
RecentFileItemsFilter::getMatchingLineEditToolTip()
{
    AString text;
    
    text = ("<html><body>"
            "Enter text for case-insensitive wildcard (GLOB) matching:"
            "<ul>"
            "<li>c  Any character represenents iteslf (c matches c)"
            "<li>?  Matches any single character"
            "<li>*  Matches zero or more of any character"
            "<li>[abc]  Matches one character in the brackets"
            "<li>[a-c]  Matches one character from the range in the brackets"
            "<li>[!abc]  Matches one character NOT in the brackets"
            "<li>[!a-c]  Matches one character NOT from the range in the brackets"
            "</ul>"
            "</body></html>");
    return text;
}

/**
 * @return name matching (glob)
 */
AString
RecentFileItemsFilter::getNameMatching() const
{
    return m_nameMatching;
}

/**
 * Set name matching (glob)
 *
 * @param nameMatching
 *    New value for name matching (glob)
 */
void
RecentFileItemsFilter::setNameMatching(const AString& nameMatching)
{
    m_nameMatching = nameMatching;
    m_regularExpression.reset();
}

/**
 * @return List spec files
 */
bool
RecentFileItemsFilter::isListSpecFiles() const
{
    return m_listSpecFiles;
}

/**
 * Set List spec files
 *
 * @param listSpecFiles
 *    New value for List spec files
 */
void
RecentFileItemsFilter::setListSpecFiles(const bool listSpecFiles)
{
    m_listSpecFiles = listSpecFiles;
}

/**
 * @return List scene files
 */
bool
RecentFileItemsFilter::isListSceneFiles() const
{
    return m_listSceneFiles;
}

/**
 * Set List scene files
 *
 * @param listSceneFiles
 *    New value for List scene files
 */
void
RecentFileItemsFilter::setListSceneFiles(const bool listSceneFiles)
{
    m_listSceneFiles = listSceneFiles;
}

/**
 * @return list directories
 */
bool
RecentFileItemsFilter::isListDirectories() const
{
    return m_listDirectories;
}
/**
* Set list directories
*
* @param listDirectories
*    New value for list directories
*/
void
RecentFileItemsFilter::setListDirectories(const bool listDirectories)
{
    m_listDirectories = listDirectories;
}

/**
* @return Show file paths
*/
bool
RecentFileItemsFilter::isShowFilePaths() const
{
    return m_showFilePaths;
}

/**
* Set show file paths
*
* @param showFilePaths
*    New value for show paths
*/
void
RecentFileItemsFilter::setShowFilePaths(const bool showFilePaths)
{
    m_showFilePaths = showFilePaths;
}

/**
 * @return show only favorites
 */
bool
RecentFileItemsFilter::isFavoritesOnly() const
{
    return m_favoritesOnly;
}

/**
 * Set show only favorites
 *
 * @param favoritesOnly
 *    New value for show only favorites
 */
void
RecentFileItemsFilter::setFavoritesOnly(const bool favoritesOnly)
{
    m_favoritesOnly = favoritesOnly;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
RecentFileItemsFilter::toString() const
{
    return "RecentFileItemsFilter";
}


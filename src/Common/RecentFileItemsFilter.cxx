
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

#include <QRegExp>
#include <QRegularExpression>

#include "CaretAssert.h"
#include "CaretLogger.h"
#include "RecentFileItem.h"

using namespace caret;

#define _MATCH_WITH_Q_REG_EXP_
    
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
    m_showDirectories = obj.m_showDirectories;
    m_showSceneFiles  = obj.m_showSceneFiles;
    m_showSpecFiles   = obj.m_showSpecFiles;
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
            if ( ! m_showDirectories) {
                return false;
            }
            break;
        case RecentFileItemTypeEnum::SCENE_FILE:
            if ( ! m_showSceneFiles) {
                return false;
            }
            break;
        case RecentFileItemTypeEnum::SPEC_FILE:
            if ( ! m_showSpecFiles) {
                return false;
            }
            break;
    }
    
    if ( ! m_nameMatching.isEmpty()) {
#ifdef _MATCH_WITH_Q_REG_EXP_
        if ( ! m_regExp) {
            m_regExp.reset(new QRegExp(m_nameMatching));
            m_regExp->setPatternSyntax(QRegExp::Wildcard);
            if ( ! m_regExp->isValid()) {
                CaretLogFine("Regular expression failure for RecentFileItem: "
                               "Name matching \""
                               + m_nameMatching
                               + "\" error message \""
                               + m_regExp->errorString()
                               + "\"");
            }
        }
        if (m_regExp) {
            if (m_regExp->isValid()) {
                if ( ! m_regExp->exactMatch(recentFileItem->getPathAndFileName())) {
                    return false;
                }
            }
        }
#else /* _MATCH_WITH_Q_REG_EXP_ */
        /*
         * NOTE: QRegularExpression::wildcardToRegularExpression() added in Qt 5.12
         */
        if ( ! m_regularExpression) {
            const QString reText(QRegularExpression::wildcardToRegularExpression(m_nameMatching));
            m_regularExpression.reset(new QRegularExpression(reText));
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
#endif /* _MATCH_WITH_Q_REG_EXP_ */
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
    
#ifdef _MATCH_WITH_Q_REG_EXP_
    text = ("<html><body>"
            "Enter text for wildcard (GLOB) matching:"
            "<ul>"
            "<li>c  Any character represenents iteslf (c matches c)"
            "<li>?  Matches any single character"
            "<li>*  Matches zero or more of any character"
            "<li>[abc]  Matches one character in the brackets"
            "<li>[a-c]  Matches one character from the range in the brackets"
            "</ul>"
            "</body></html>");
#else /* _MATCH_WITH_Q_REG_EXP_ */
    text = ("<html><body>"
            "Enter text for wildcard (GLOB) matching:"
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
#endif /* _MATCH_WITH_Q_REG_EXP_ */
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
    m_regExp.reset();
}

/**
 * @return Show spec files
 */
bool
RecentFileItemsFilter::isShowSpecFiles() const
{
    return m_showSpecFiles;
}

/**
 * Set show spec files
 *
 * @param showSpecFiles
 *    New value for Show spec files
 */
void
RecentFileItemsFilter::setShowSpecFiles(const bool showSpecFiles)
{
    m_showSpecFiles = showSpecFiles;
}

/**
 * @return Show scene files
 */
bool
RecentFileItemsFilter::isShowSceneFiles() const
{
    return m_showSceneFiles;
}

/**
 * Set show scene files
 *
 * @param showSceneFiles
 *    New value for Show scene files
 */
void
RecentFileItemsFilter::setShowSceneFiles(const bool showSceneFiles)
{
    m_showSceneFiles = showSceneFiles;
}

/**
 * @return show directories
 */
bool
RecentFileItemsFilter::isShowDirectories() const
{
    return m_showDirectories;
}

/**
 * Set show directories
 *
 * @param showDirectories
 *    New value for show directories
 */
void
RecentFileItemsFilter::setShowDirectories(const bool showDirectories)
{
    m_showDirectories = showDirectories;
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


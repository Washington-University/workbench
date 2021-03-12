
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

#define __HTML_TABLE_BUILDER_DECLARE__
#include "HtmlTableBuilder.h"
#undef __HTML_TABLE_BUILDER_DECLARE__

#include <algorithm>

#include "CaretAssert.h"

using namespace caret;


    
/**
 * \class caret::HtmlTableBuilder 
 * \brief Helps create an HTML table
 * \ingroup Common
 */

/**
 * Constructor.
 *  @param htmlVersion
 *    Version of HTML
 *  @param numberOfColumns
 *    Number of columns in the table
 */
HtmlTableBuilder::HtmlTableBuilder(const HtmlVersion htmlVersion,
                                   const int32_t numberOfColumns)
: CaretObject(),
m_htmlVersion(htmlVersion),
m_numberOfColumns(numberOfColumns)
{
    
}

/**
 * Destructor.
 */
HtmlTableBuilder::~HtmlTableBuilder()
{
}


/**
 * Set the title of the table using bold text
 * @param title
 * Title displayed spanning all rows at top of table
 */
void
HtmlTableBuilder::setTitleBold(const QString& title)
{
    m_title      = title;
    m_titleStyle = TitleStyle::BOLD;
}

/**
 * Set the title of the table using plain text
 * @param title
 * Title displayed spanning all rows at top of table
 */
void
HtmlTableBuilder::setTitlePlain(const QString& title)
{
    m_title      = title;
    m_titleStyle = TitleStyle::PLAIN;
}

/**
 * Add data as  header columns into a table row
 *
 * @param textColOne
 *   Data for first column
 * @param textColTwo
 *   Data for second column
 * @param textColThree
 *   Data for third column
 * @param textColFour
 *   Data for fourth column
 * @param textColFive
 *   Data for fifth column
 */
void
HtmlTableBuilder::addHeaderRow(const QString& textColOne,
                               const QString& textColTwo,
                               const QString& textColThree,
                               const QString& textColFour,
                               const QString& textColFive)
{
    addRowPrivate("th",
                  textColOne,
                  textColTwo,
                  textColThree,
                  textColFour,
                  textColFive);
}

/**
 * Add data as columns into a table row
 *
 * @param textColOne
 *   Data for first column
 * @param textColTwo
 *   Data for second column
 * @param textColThree
 *   Data for third column
 * @param textColFour
 *   Data for fourth column
 * @param textColFive
 *   Data for fifth column
 */
void
HtmlTableBuilder::addRow(const QString& textColOne,
                         const QString& textColTwo,
                         const QString& textColThree,
                         const QString& textColFour,
                         const QString& textColFive)
{
    addRowPrivate("td",
                  textColOne,
                  textColTwo,
                  textColThree,
                  textColFour,
                  textColFive);
}

/**
 * Add data as columns into a table row
 *
 * @param rowElementTag
 *   The tag for the row element
 * @param textColOne
 *   Data for first column
 * @param textColTwo
 *   Data for second column
 * @param textColThree
 *   Data for third column
 * @param textColFour
 *   Data for fourth column
 * @param textColFive
 *   Data for fifth column
 */
void
HtmlTableBuilder::addRowPrivate(const QString& rowElementTag,
                                const QString& textColOne,
                                const QString& textColTwo,
                                const QString& textColThree,
                                const QString& textColFour,
                                const QString& textColFive)
{
    QString tr("<tr>");
    
    addTableDataElementToRow(tr,
                             rowElementTag,
                             textColOne);
    
    if (m_numberOfColumns >= 2) {
        addTableDataElementToRow(tr,
                                 rowElementTag,
                                 textColTwo);
    }
    
    if (m_numberOfColumns >= 3) {
        addTableDataElementToRow(tr,
                                 rowElementTag,
                                 textColThree);
    }
    
    if (m_numberOfColumns >= 4) {
        addTableDataElementToRow(tr,
                                 rowElementTag,
                                 textColFour);
    }
    
    if (m_numberOfColumns >= 5) {
        addTableDataElementToRow(tr,
                                 rowElementTag,
                                 textColFive);
    }
    
    tr.append("</tr>");
    
    m_tableRows.push_back(tr);
}


/**
 * Add table data element to a row
 * @param tableRow
 *   The table row
 * @param rowElementTag
 *   The tag for the row element
 * @param columnData
 *   Data to add to row
 */
void
HtmlTableBuilder::addTableDataElementToRow(QString& tableRow,
                                           const QString& rowElementTag,
                                           const QString& columnData)
{
    tableRow.append("    <" + rowElementTag + " align=\"left\">"
                    + columnData
                    + "</" + rowElementTag + ">\n");
    
    m_modifiedSinceLastUpdatedFlag = true;
}

/**
 * @return The table in HTML table format
 */
QString
HtmlTableBuilder::getAsHtmlTable() const
{
    QString html;
    
    switch (m_htmlVersion) {
        case V4_01:
            html = getAsHtmlTableV401();
            break;
    }
    
    return html;
}

/**
 * @return The table in HTML 4.01 table format
 */
QString
HtmlTableBuilder::getAsHtmlTableV401() const
{
    if (m_tableRows.empty()) {
        return "";
    }
    
    /*
     * Cache the HTML and only update when a data has been added
     */
    if (m_modifiedSinceLastUpdatedFlag) {
        m_modifiedSinceLastUpdatedFlag = false;
        m_tableText.clear();
        
        m_tableText.append("<table> <tbody>\n");

        if ( ! m_title.isEmpty()) {
            QString tdTH;
            switch (m_titleStyle) {
                case BOLD:
                    tdTH = "th";
                    break;
                case PLAIN:
                    tdTH = "td";
                    break;
            }
            const QString s("<tr><"
                            + tdTH
                            + " colspan=\"" + AString::number(m_numberOfColumns) + "\" align=\"left\">"
                            + m_title
                            + "</"
                            + tdTH
                            + "></tr>\n");
            m_tableText.append(s);
        }
        
        for (const auto& tr : m_tableRows) {
            m_tableText.append(tr);
        }
        
        m_tableText.append("</tbody></table>\n");
        m_tableText.append("<p>\n");
    }
        
    return m_tableText;
}


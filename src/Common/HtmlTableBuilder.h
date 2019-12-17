#ifndef __HTML_TABLE_BUILDER_H__
#define __HTML_TABLE_BUILDER_H__

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
#include <vector>

#include "CaretObject.h"

namespace caret {
    class HtmlTableBuilder : public CaretObject {
        
    public:
        /*
         * Version of HTML
         */
        enum HtmlVersion {
            /** Version 4.01 */
            V4_01
        };
        
        HtmlTableBuilder(const HtmlVersion htmlVersion,
                         const int32_t numberOfColumns);
        
        virtual ~HtmlTableBuilder();
        
        HtmlTableBuilder(const HtmlTableBuilder&) = delete;

        HtmlTableBuilder& operator=(const HtmlTableBuilder&) = delete;
        
        void setTitleBold(const QString& title);
        
        void setTitlePlain(const QString& title);
        
        void addHeaderRow(const QString& textColOne,
                          const QString& textColTwo   = "",
                          const QString& textColThree = "",
                          const QString& textColFour  = "",
                          const QString& textColFive  = "");

        void addRow(const QString& textColOne,
                    const QString& textColTwo   = "",
                    const QString& textColThree = "",
                    const QString& textColFour  = "",
                    const QString& textColFive  = "");
        
        QString getAsHtmlTable() const;
        
       // ADD_NEW_METHODS_HERE

    private:
        enum TitleStyle {
            BOLD,
            PLAIN
        };
        
        void addTableDataElementToRow(QString& tableRow,
                                      const QString& rowElementTag,
                                      const QString& columnData);
        
        QString getAsHtmlTableV401() const;
        
        void addRowPrivate(const QString& rowElementTag,
                           const QString& textColOne,
                           const QString& textColTwo   = "",
                           const QString& textColThree = "",
                           const QString& textColFour  = "",
                           const QString& textColFive  = "");
        
        const HtmlVersion m_htmlVersion;
        
        const int32_t m_numberOfColumns;
        
        QString m_title;
        
        TitleStyle m_titleStyle = TitleStyle::PLAIN;
        
        std::vector<QString> m_tableRows;
        
        mutable QString m_tableText;
        
        mutable bool m_modifiedSinceLastUpdatedFlag = false;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __HTML_TABLE_BUILDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HTML_TABLE_BUILDER_DECLARE__

} // namespace
#endif  //__HTML_TABLE_BUILDER_H__


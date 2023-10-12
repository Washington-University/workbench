#ifndef __COMMA_SEPARATED_VALUES_FILE_H__
#define __COMMA_SEPARATED_VALUES_FILE_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#include "DataFile.h"

class QStandardItemModel;
class QStringListModel;
class QxtCsvModel;

namespace caret {

    class CommaSeparatedValuesFile : public DataFile {
        
    public:
        CommaSeparatedValuesFile();
        
        virtual ~CommaSeparatedValuesFile();
        
        CommaSeparatedValuesFile(const CommaSeparatedValuesFile&) = delete;

        CommaSeparatedValuesFile& operator=(const CommaSeparatedValuesFile&) = delete;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;
        
        virtual bool isEmpty() const override;

        QxtCsvModel* getCsvModel() const;
        
        void printCsvModelContent(QxtCsvModel* csvModel);
        
        AString stripDoubleQuotes(const QString& text) const;
        
        // ADD_NEW_METHODS_HERE

    private:
        
        std::unique_ptr<QxtCsvModel> m_csvModel;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __COMMA_SEPARATED_VALUES_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __COMMA_SEPARATED_VALUES_FILE_DECLARE__

} // namespace
#endif  //__COMMA_SEPARATED_VALUES_FILE_H__

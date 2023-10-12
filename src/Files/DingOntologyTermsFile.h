#ifndef __DING_ONTOLOGY_TERMS_FILE_H__
#define __DING_ONTOLOGY_TERMS_FILE_H__

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


#include <cstdint>
#include <memory>

#include "CommaSeparatedValuesFile.h"

class QStandardItem;
class QStandardItemModel;
class QxtCsvModel;

namespace caret {

    class DingOntologyTermsFile : public CommaSeparatedValuesFile {
        
    public:
        static AString getDingOntologyResourcesFileName();
        
        DingOntologyTermsFile();
        
        virtual ~DingOntologyTermsFile();
        
        DingOntologyTermsFile(const DingOntologyTermsFile&) = delete;

        DingOntologyTermsFile& operator=(const DingOntologyTermsFile&) = delete;
        
        virtual void readFile(const AString& filename) override;
        
        virtual void writeFile(const AString& filename) override;
        
        virtual bool isEmpty() const override;

        QStandardItemModel* getTableModel() const;
        
        QStandardItemModel* getTreeModel() const;
        
        QString getAbbreviatedName(const QStandardItem* item) const;
        
        QString getDescriptiveName(const QStandardItem* item) const;

        int32_t getAbbreviatedNameItemRole() const;
        
        /** @return Title for the abbreviated name data */
        static QString getAbbreviatedNameTitle() { return "Abbreviation"; }

        /** @return Title for the descriptive name data */
        static QString getDescriptiveNameTitle() { return "Descriptive"; }
        
        // ADD_NEW_METHODS_HERE

    private:
        
        void createModels(QxtCsvModel* csvModel);
        
        QStandardItem* createStandardItem(const QString& shortName,
                                          const QString& longName) const;
        
        std::unique_ptr<QStandardItemModel> m_tableModel;
        
        std::unique_ptr<QStandardItemModel> m_treeModel;
        
        int32_t m_abbreviatedNameItemRole = -1;
        
        int32_t m_descriptiveNameItemRole = -1;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __DING_ONTOLOGY_TERMS_FILE_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __DING_ONTOLOGY_TERMS_FILE_DECLARE__

} // namespace
#endif  //__DING_ONTOLOGY_TERMS_FILE_H__

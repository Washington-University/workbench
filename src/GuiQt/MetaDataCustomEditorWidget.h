#ifndef __META_DATA_CUSTOM_EDITOR_WIDGET_H__
#define __META_DATA_CUSTOM_EDITOR_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <map>
#include <set>

#include <QWidget>
#include "AString.h"

class QLabel;
class QLineEdit;
class QTextEdit;

namespace caret {

    class GiftiMetaData;
    
    class MetaDataCustomEditorWidget : public QWidget {
        
        Q_OBJECT

    public:
        MetaDataCustomEditorWidget(const std::vector<AString>& metaDataNames,
                                   GiftiMetaData* metaData,
                                   QWidget* parent = 0);
        
        virtual ~MetaDataCustomEditorWidget();
        
        bool validateAndSaveRequiredMetaData(const std::vector<AString>& requiredMetaDataNames,
                                             AString& errorMessageOut);

        void saveMetaData();
        
    private:
        MetaDataCustomEditorWidget(const MetaDataCustomEditorWidget&);

        MetaDataCustomEditorWidget& operator=(const MetaDataCustomEditorWidget&);
        
        enum {
            COLUMN_DELETE = 0,
            COLUMN_NAME   = 1,
            COLUMN_VALUE  = 2
        };

        class MetaDataWidgetRow {
        public:
            MetaDataWidgetRow(const AString& metaDataName,
                              GiftiMetaData* metaData);
            
            ~MetaDataWidgetRow();
            
            void saveToMetaData();
            
            const AString m_metaDataName;
            
            GiftiMetaData* m_metaData;
            
            QLabel* m_nameLabel;
            
            QLineEdit* m_valueLineEdit;
        };

        GiftiMetaData* m_metaData = NULL;
        
        std::vector<MetaDataWidgetRow*> m_metaDataWidgetRows;

        QTextEdit* m_commentTextEditor = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __META_DATA_CUSTOM_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __META_DATA_CUSTOM_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__META_DATA_CUSTOM_EDITOR_WIDGET_H__

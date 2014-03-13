#ifndef __META_DATA_EDITOR_WIDGET_H__
#define __META_DATA_EDITOR_WIDGET_H__

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

class QAction;
class QGridLayout;
class QLineEdit;
class QPushButton;
class QScrollArea;
class QSignalMapper;
class QToolButton;

namespace caret {

    class GiftiMetaData;
    class WuQDataEntryDialog;
    class WuQWidgetObjectGroup;
    
    class MetaDataEditorWidget : public QWidget {
        
        Q_OBJECT

    public:
        MetaDataEditorWidget(QWidget* parent);
        
        virtual ~MetaDataEditorWidget();
        
        void loadMetaData(GiftiMetaData* metaData);
        
        bool isMetaDataModified();
        
        AString saveMetaData();
        
    private slots:
        void newPushButtonClicked();
        
        void deleteActionTriggered(int indx);
        
        void validateNewName(WuQDataEntryDialog* dataEntryDialog);
        
    private:
        MetaDataEditorWidget(const MetaDataEditorWidget&);

        MetaDataEditorWidget& operator=(const MetaDataEditorWidget&);
        
        enum {
            COLUMN_DELETE = 0,
            COLUMN_NAME   = 1,
            COLUMN_VALUE  = 2
        };

        class MetaDataWidgetRow {
        public:
            MetaDataWidgetRow(QWidget* parent,
                              QSignalMapper* deleteActionSignalMapper,
                              const int32_t indx);
            
            ~MetaDataWidgetRow();
            
            QToolButton* m_deleteToolButton;
            
            QLineEdit* m_nameLineEdit;
            
            QLineEdit* m_valueLineEdit;
            
            WuQWidgetObjectGroup* m_widgetGroup;
        };

        void displayNamesAndValues();
        
        void readNamesAndValues();
        
        bool getNamesInDialog(std::set<AString>& namesOut,
                              std::set<AString>* duplicateNamesOut,
                                           bool* haveEmptyNamesOut);

        std::vector<std::pair<AString, AString> > m_namesAndValues;
        
        std::map<AString, AString> m_unmodifiedNamesAndValues;
        
        /** Metadata that is being edited */
        GiftiMetaData* m_metaDataBeingEdited;
        
        QGridLayout* m_metaGridLayout;
        
        std::vector<MetaDataWidgetRow*> m_metaDataWidgetRows;
        
        QSignalMapper* m_deleteActionSignalMapper;
        
        QPushButton* m_newPushButton;
        
        QLineEdit* m_newNameDialogLineEdit;
        
        QScrollArea* m_metaDataNameValueScrollArea;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __META_DATA_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __META_DATA_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__META_DATA_EDITOR_WIDGET_H__

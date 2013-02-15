#ifndef __META_DATA_EDITOR_WIDGET_H__
#define __META_DATA_EDITOR_WIDGET_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
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
        
        QGridLayout* m_metaDataWidgetLayout;
        
        std::vector<MetaDataWidgetRow*> m_metaDataWidgetRows;
        
        QSignalMapper* m_deleteActionSignalMapper;
        
        QPushButton* m_newPushButton;
        
        QLineEdit* m_newNameDialogLineEdit;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __META_DATA_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __META_DATA_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__META_DATA_EDITOR_WIDGET_H__

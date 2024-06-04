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
#include <memory>
#include <set>

#include <QDate>
#include <QWidget>
#include "AString.h"

class QComboBox;
class QDateEdit;
class QGridLayout;
class QLabel;
class QLineEdit;
class QTextEdit;
class QToolButton;

namespace caret {

    class GiftiMetaData;
    class GiftiMetaDataValidatorInterface;
    
    class MetaDataCustomEditorWidget : public QWidget {
        
        Q_OBJECT

    public:
        /**
         * Mode of editing
         */
        enum Mode {
            /**
             * Editing a new sample's metadata
             */
            NEW_SAMPLE_EDITING,
            /**
             * Normal metadata editing
             */
            NORMAL_EDITING
        };
        
        MetaDataCustomEditorWidget(const Mode mode,
                                   const std::vector<AString>& metaDataNames,
                                   const std::vector<AString>& requiredMetaDataNames,
                                   GiftiMetaData* userMetaData,
                                   QWidget* parent = 0);
        
        virtual ~MetaDataCustomEditorWidget();
        
        bool validateAndSaveRequiredMetaData(const std::vector<AString>& requiredMetaDataNames,
                                             AString& errorMessageOut);

        bool validateAndSaveRequiredMetaData(GiftiMetaDataValidatorInterface* metaDataValidator,
                                             const AString& errorMessageSuffix);
        
        bool isMetaDataModified() const;
        
        void saveMetaData();
        
        void calledByMetaDataWidgetRowWhenValueChanges(const AString& metaDataName);
        
        void reloadAllMetaDataWidgetRows();
        
    private:
        class MetaDataWidgetRow {
        public:
            MetaDataWidgetRow(MetaDataCustomEditorWidget* editorWidget,
                              QGridLayout* gridLayout,
                              const int32_t gridLayoutRow,
                              const int32_t gridLayoutNameColumn,
                              const int32_t gridLayoutValueColumn,
                              const int32_t gridLayoutButtonColumn,
                              const AString& metaDataName,
                              GiftiMetaData* metaData,
                              const bool requiredMetaDataFlag);
            
            ~MetaDataWidgetRow();
            
            void saveToMetaData();
            
            void saveAfterDataChangedInGUI();
            
            void toolButtonClicked();
            
            void updateValueWidget();
            
            void updateCompositeMetaDataValueWidget();
            
            QString getAsText() const;
            
            void setSavingEnabled(const bool enabled);
            
            MetaDataCustomEditorWidget* m_editorWidget = NULL;
            
            const AString m_metaDataName;
            
            GiftiMetaData* m_metaData;

            QComboBox* m_valueComboBox = NULL;
            
            QDateEdit* m_valueDateEdit = NULL;
            
            QLineEdit* m_valueLineEdit = NULL;
            
            QToolButton* m_toolButton = NULL;
            
            bool m_compositeMetaDataItemFlag = false;
            
            bool m_savingEnabled = true;
        };
        
        MetaDataCustomEditorWidget(const MetaDataCustomEditorWidget&);

        MetaDataCustomEditorWidget& operator=(const MetaDataCustomEditorWidget&);
        
        MetaDataWidgetRow* getMetaDataWidgetRow(const QString& metaDataName);
        
        void updateValueInMetaDataWidgetRow(const QString& metaDataName);
        
        void metaDataButtonClicked(const AString& metaDataName,
                                   QWidget* parentDialogWidget);
        
        void processLabelForIdDescription(const AString& labelText,
                                          AString& idOut,
                                          AString& descriptionOut) const;
        
        const Mode m_mode;
        
        GiftiMetaData* m_userMetaData = NULL;
        
        /**
         * This is a copy of the user's metadata.  It is a copy so that the user can
         * edit it but then choose to discard the changes.
         */
        std::unique_ptr<GiftiMetaData> m_editorMetaData;
        
        std::vector<MetaDataWidgetRow*> m_metaDataWidgetRows;

        AString m_commentMetaDataName;
        
        QTextEdit* m_commentTextEditor = NULL;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __META_DATA_CUSTOM_EDITOR_WIDGET_DECLARE__
#endif // __META_DATA_CUSTOM_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__META_DATA_CUSTOM_EDITOR_WIDGET_H__

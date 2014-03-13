#ifndef __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_H__
#define __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_H__

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

#include <stdint.h>

#include "AString.h"
#include "WuQWidget.h"

class QComboBox;
class QModelIndex;

namespace caret {
    class GiftiLabel;
    class GiftiLabelTable;

    class GiftiLabelTableSelectionComboBox : public WuQWidget {
        
        Q_OBJECT

    public:
        GiftiLabelTableSelectionComboBox(QObject* parent);
        
        virtual ~GiftiLabelTableSelectionComboBox();
        
        void updateContent(GiftiLabelTable* giftiLabelTable);
        
        virtual QWidget* getWidget();
        
        const GiftiLabel* getSelectedLabel() const;
        
        GiftiLabel* getSelectedLabel();
        
        void setSelectedLabel(const GiftiLabel* label);
        
        int32_t getSelectedLabelKey() const;
        
        void setSelectedLabelKey(const int32_t key);
        
        QString getSelectedLabelName() const;
        
        void setSelectedLabelName(const QString& labelName);
        
        void setUnassignedLabelTextOverride(const AString& text);
        
    signals:
        /**
         * This signal is sent when the user chooses a label in the combobox. 
         * The item's label is passed. Note that this signal is sent even when
         * the choice is not changed. If you need to know when the choice 
         * actually changes, use signal labelChanged().  If the selected
         * label is invalid, NULL is passed.
         */
        void labelActivated(GiftiLabel*);
        
        /**
         * This signal is sent when the user chooses a label in the combobox.
         * The item's key is passed. Note that this signal is sent even when
         * the choice is not changed. If you need to know when the choice
         * actually changes, use signal labelKeyChanged().  If the selected
         * label is invalid, GiftiLabel::getInvalidLabelKey() is passed.
         */
        void labelKeyActivated(const int32_t key);
        
        /**
         * This signal is sent whenever the label in the combobox 
         * changes either through user interaction or programmatically.
         * If the selected label is invalid, NULL is passed.
         */
        void labelChanged(GiftiLabel*);
        
        /**
         * This signal is sent whenever the label in the combobox
         * changes either through user interaction or programmatically.
         * If the selected label is invalid, GiftiLabel::getInvalidLabelKey() is passed.
         */
        void labelKeyChanged(const int32_t key);
        
        // ADD_NEW_METHODS_HERE
        
    private slots:
        void currentIndexChanged(int indx);
        
        void itemActivated(int indx);
        
        void addEditButtonClicked();
        
        void rowsWereInserted(const QModelIndex& parent,
                          int start,
                          int end);
    private:
        GiftiLabelTableSelectionComboBox(const GiftiLabelTableSelectionComboBox&);
        
        GiftiLabelTableSelectionComboBox& operator=(const GiftiLabelTableSelectionComboBox&);
        
        GiftiLabelTable* m_giftiLabelTable;
        
        QComboBox* m_comboBox;
        
        bool m_ignoreInsertedRowsFlag;
        
        AString m_unassignedLabelTextOverride;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_DECLARE__

} // namespace
#endif  //__GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_H__

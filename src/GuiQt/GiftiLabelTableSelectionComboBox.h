#ifndef __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_H__
#define __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_H__

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

#include <stdint.h>

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
         * label is invalid, -1 is passed.
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
         * If the selected label is invalid, -1 is passed.
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
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_DECLARE__

} // namespace
#endif  //__GIFTI_LABEL_TABLE_SELECTION_COMBO_BOX_H__

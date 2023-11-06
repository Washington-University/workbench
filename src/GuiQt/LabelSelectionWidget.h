#ifndef __LABEL_SELECTION_WIDGET_H__
#define __LABEL_SELECTION_WIDGET_H__

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

#include <QWidget>

#include "AString.h"

namespace caret {

    class CaretMappableDataFileAndMapSelectorObject;
    class GiftiLabelTableSelectionComboBox;
    
    class LabelSelectionWidget : public QWidget {
        
        Q_OBJECT

    public:
        LabelSelectionWidget(const QString& saveRestoreStateName = "",
                             QWidget* parent = 0);
        
        virtual ~LabelSelectionWidget();
        
        LabelSelectionWidget(const LabelSelectionWidget&) = delete;

        LabelSelectionWidget& operator=(const LabelSelectionWidget&) = delete;
        
        AString getSelectedLabel() const;

        // ADD_NEW_METHODS_HERE

    private slots:
        void labelFileOrMapSelectionChanged();
        
    private:
        const QString s_saveRestoreStateName;
        
        void updateWidget();
        
        void restoreSelections(const QString& selectionName);
        
        void saveSelections(const QString& selectionName);
        
        CaretMappableDataFileAndMapSelectorObject* m_fileSelector = NULL;
        
        GiftiLabelTableSelectionComboBox* m_fileLabelComboBox = NULL;
        
        static const QString s_stateSeparator;
        
        static std::map<QString, QString> s_previousStates;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __LABEL_SELECTION_WIDGET_DECLARE__
    const QString LabelSelectionWidget::s_stateSeparator = "!@#$%^XXX";
    std::map<QString, QString> LabelSelectionWidget::s_previousStates;
#endif // __LABEL_SELECTION_WIDGET_DECLARE__

} // namespace
#endif  //__LABEL_SELECTION_WIDGET_H__

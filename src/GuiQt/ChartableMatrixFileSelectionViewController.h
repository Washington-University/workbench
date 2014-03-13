#ifndef __CHARTABLE_MATRIX_FILE_SELECTION_VIEW_CONTROLLER_H__
#define __CHARTABLE_MATRIX_FILE_SELECTION_VIEW_CONTROLLER_H__

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
#include <vector>

#include "EventListenerInterface.h"

#include "WuQWidget.h"

class QAbstractButton;
class QButtonGroup;
class QRadioButton;
class QVBoxLayout;

namespace caret {

    class ChartableMatrixFileSelectionModel;
    
    class ChartableMatrixFileSelectionViewController : public WuQWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartableMatrixFileSelectionViewController(const int32_t browserWindowIndex,
                                                   QWidget* parent);
        
        virtual ~ChartableMatrixFileSelectionViewController();

        virtual void receiveEvent(Event* event);
        
        QWidget* getWidget();
        
        void updateViewController();
        
        void updateViewController(ChartableMatrixFileSelectionModel* selectionModel);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void radioButtonClicked(QAbstractButton* radioButton);
        
    private:
        ChartableMatrixFileSelectionViewController(const ChartableMatrixFileSelectionViewController&);

        ChartableMatrixFileSelectionViewController& operator=(const ChartableMatrixFileSelectionViewController&);
        
        int32_t m_browserWindowIndex;
        
        ChartableMatrixFileSelectionModel* m_selectionModel;
        
        std::vector<QRadioButton*> m_radioButtons;
        
        QWidget* m_radioButtonWidget;
        
        QVBoxLayout* m_radioButtonLayout;
        
        QButtonGroup* m_radioButtonGroup;
        
        static const QString FILE_POINTER_PROPERTY_NAME;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHARTABLE_MATRIX_FILE_SELECTION_VIEW_CONTROLLER_DECLARE__
    const QString ChartableMatrixFileSelectionViewController::FILE_POINTER_PROPERTY_NAME = "filePointer";
#endif // __CHARTABLE_MATRIX_FILE_SELECTION_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHARTABLE_MATRIX_FILE_SELECTION_VIEW_CONTROLLER_H__

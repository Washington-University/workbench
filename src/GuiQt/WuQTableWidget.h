#ifndef __WU_Q_TABLE_WIDGET_H__
#define __WU_Q_TABLE_WIDGET_H__

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

#include <QSize>

#include "AString.h"
#include "WuQWidget.h"

class QSignalMapper;
class QTableWidget;
class QTableWidgetItem;

namespace caret {

    class WuQImageLabel;
    class WuQTableWidgetModel;
    class WuQTableWidgetModelColumnCheckBox;
    class WuQTableWidgetModelColumnContent;
    class WuQTableWidgetModelColumnColorSwatch;
    class WuQTableWidgetModelColumnDoubleSpinBox;
    class WuQTableWidgetModelColumnIcon;
    class WuQTableWidgetModelColumnIntegerSpinBox;
    class WuQTableWidgetModelColumnText;
    
    class WuQTableWidget : public WuQWidget {
        
        Q_OBJECT

    public:
        WuQTableWidget(QObject* parent);
        
        virtual ~WuQTableWidget();
        
        virtual QWidget* getWidget();
        
        void setModel(WuQTableWidgetModel* tableModel);
        
    private slots:
        void horizontalHeaderSelectedForSorting(int);
        
        void widgetSignalMapperSelected(int);
        
        void filesTableWidgetCellChanged(int,int);
        
        void filesTableWidgetCellClicked(int,int);
        
    private:
        WuQTableWidget(const WuQTableWidget&);

        WuQTableWidget& operator=(const WuQTableWidget&);
        
        void setNumberOfDisplayedRows(const int32_t numberOfRowsRequested);
        
        void updateRow(const int32_t rowIndex);
        
        QTableWidgetItem* createTextItem(const AString& text);
        
        QSize estimateTableWidgetSize();
        
        void setHorizontalHeaderTextItem(const int32_t columnIndex,
                                                         const QString& text);
        
        void insertTextItem(const WuQTableWidgetModelColumnText* columnInfo,
                                         const int32_t rowIndex,
                                         const int32_t columnIndex);
        
        
        void insertColorSwatch(const WuQTableWidgetModelColumnColorSwatch* columnInfo,
                               const int32_t rowIndex,
                               const int32_t columnIndex);
        
        void insertCheckableItem(const WuQTableWidgetModelColumnCheckBox* columnInfo,
                                 const int32_t rowIndex,
                                 const int32_t columnIndex);
        
        void insertIconItem(const WuQTableWidgetModelColumnIcon* columnInfo,
                                  const int32_t rowIndex,
                                  const int32_t columnIndex);
        
        void insertIntegerSpinBoxItem(const WuQTableWidgetModelColumnIntegerSpinBox* columnInfo,
                                      const int32_t rowIndex,
                               const int32_t columnIndex);
        
        void insertDoubleSpinBoxItem(const WuQTableWidgetModelColumnDoubleSpinBox* columnInfo,
                                     const int32_t rowIndex,
                               const int32_t columnIndex);
        
        QTableWidgetItem* getTableWidgetItem(const int32_t rowIndex,
                                             const int32_t columnIndex);
        
        void setTableWidgetItem(const int32_t rowIndex,
                                const int32_t columnIndex,
                                QTableWidgetItem* item);
        
        void connectToWidgetSignalMapper(const int32_t rowIndex,
                                         const int32_t columnIndex,
                                         QObject* object,
                                         const char* objectSignal);
        
        int32_t widgetSignalMapperIndexFromRowAndColumn(const int32_t rowIndex,
                                                  const int32_t columnIndex) const;
        
        void widgetSignalMapperIndexToRowAndColumn(const int32_t widgetSignalMapperIndex,
                                                   int32_t& rowIndexOut,
                                             int32_t& columnIndexOut) const;
        
        void setItemTextAlignment(const WuQTableWidgetModelColumnContent* columnInfo,
                                  QTableWidgetItem* item);
        
        int32_t getNumberOfModelRows() const;
        
        int32_t getNumberOfModelColumns() const;
        
        // ADD_NEW_METHODS_HERE

        QTableWidget* m_tableWidget;
        
        WuQTableWidgetModel* m_tableModel;
        
        int32_t m_numberOfRows;
        
        int32_t m_numberOfColumns;
        
        QSignalMapper* m_widgetSignalMapper;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WU_Q_TABLE_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TABLE_WIDGET_DECLARE__

} // namespace
#endif  //__WU_Q_TABLE_WIDGET_H__

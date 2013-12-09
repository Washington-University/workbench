#ifndef __WU_Q_TABLE_WIDGET_MODEL_H__
#define __WU_Q_TABLE_WIDGET_MODEL_H__

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
#include <vector>

#include <QObject>

#include "AString.h"

class QIcon;

namespace caret {
    class WuQTableWidgetModelColumnContent {
    public:
        enum ColumnAlignment {
            COLUMN_ALIGN_LEFT,
            COLUMN_ALIGN_CENTER,
            COLUMN_ALIGN_RIGHT
        };
        
        enum ColumnDataType {
            COLUMN_DATA_CHECK_BOX,
            COLUMN_DATA_COLOR_SWATCH,
            COLUMN_DATA_DOUBLE_SPIN_BOX,
            COLUMN_DATA_ICON,
            COLUMN_DATA_INTEGER_SPIN_BOX,
            COLUMN_DATA_TEXT
        };
        
    protected:
        WuQTableWidgetModelColumnContent(const AString& columnTitle,
                                         const ColumnAlignment columnAlignment,
                                  const ColumnDataType columnDataType);
        
    public:
        virtual ~WuQTableWidgetModelColumnContent();
        
        AString getColumnTitle() const;
        
        ColumnAlignment getColumnAlignment() const;
        
        ColumnDataType getColumnDataType() const;
        
    protected:
        virtual void setNumberOfRows(const int32_t numberOfRows) = 0;
        
        AString m_columnTitle;
        
        ColumnAlignment m_columnAlignment;
        
        ColumnDataType m_columnDataType;

        friend class WuQTableWidgetModel;
    };
    
    /* ========================================================== */
    
    class WuQTableWidgetModelColumnCheckBox : public WuQTableWidgetModelColumnContent {
    public:
        WuQTableWidgetModelColumnCheckBox(const AString& columnTitle,
                                          const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment);
        
        virtual ~WuQTableWidgetModelColumnCheckBox();

    protected:
        virtual void setNumberOfRows(const int32_t numberOfRows);
        
        std::vector<bool> m_selectionStatus;
        
        friend class WuQTableWidgetModel;
    };
    
    /* ========================================================== */
    
    class WuQTableWidgetModelColumnColorSwatch : public WuQTableWidgetModelColumnContent {
    public:
        WuQTableWidgetModelColumnColorSwatch(const AString& columnTitle,
                                             const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment);
        
        virtual ~WuQTableWidgetModelColumnColorSwatch();
        
    protected:
        virtual void setNumberOfRows(const int32_t numberOfRows);
        
        std::vector<float> m_rgba;
        
        friend class WuQTableWidgetModel;
    };
    
    /* ========================================================== */
    
    class WuQTableWidgetModelColumnDoubleSpinBox : public WuQTableWidgetModelColumnContent {
    public:
        WuQTableWidgetModelColumnDoubleSpinBox(const AString& columnTitle,
                                               const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment,
                                               const double minimumValue,
                                               const double maximumValue,
                                               const double stepSize,
                                               const int32_t digitsRightOfDecimalPoint);
        
        virtual ~WuQTableWidgetModelColumnDoubleSpinBox();
        
        double getMinimumValue() const;
        
        double getMaximumValue() const;
        
        double getStepSize() const;
        
        double getDigitsRightOfDecimalPoint() const;
        
        
    protected:
        virtual void setNumberOfRows(const int32_t numberOfRows);
        
        double m_minimumValue;
        double m_maximumValue;
        double m_stepSize;
        int32_t m_digitsRightOfDecimalPoint;
        
        std::vector<double> m_values;
        
        friend class WuQTableWidgetModel;
    };
    
    /* ========================================================== */
    
    class WuQTableWidgetModelColumnIcon : public WuQTableWidgetModelColumnContent {
    public:
        WuQTableWidgetModelColumnIcon(const AString& columnTitle,
                                      const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment,
                                      QIcon* icon);
        
        virtual ~WuQTableWidgetModelColumnIcon();
        
        const QIcon* getIcon() const;
        
        void setIcon(const QIcon* icon);
        
    protected:
        virtual void setNumberOfRows(const int32_t numberOfRows);
        
        QIcon* m_icon;
        
        friend class WuQTableWidgetModel;
    };
    
    /* ========================================================== */
    
    class WuQTableWidgetModelColumnIntegerSpinBox : public WuQTableWidgetModelColumnContent {
    public:
        WuQTableWidgetModelColumnIntegerSpinBox(const AString& columnTitle,
                                                const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment,
                                               const int32_t minimumValue,
                                               const int32_t maximumValue,
                                               const int32_t stepSize);
        
        virtual ~WuQTableWidgetModelColumnIntegerSpinBox();
        
        int32_t getMinimumValue() const;
        
        int32_t getMaximumValue() const;
        
        int32_t getStepSize() const;
        
        
    protected:
        virtual void setNumberOfRows(const int32_t numberOfRows);

        std::vector<int32_t> m_values;
        
        int32_t m_minimumValue;
        int32_t m_maximumValue;
        int32_t m_stepSize;
        
        friend class WuQTableWidgetModel;
    };
    
    /* ========================================================== */
    
    class WuQTableWidgetModelColumnText : public WuQTableWidgetModelColumnContent {
    public:
        enum TextEditable {
            TEXT_EDITABLE_NO,
            TEXT_EDITABLE_YES
        };
        
        WuQTableWidgetModelColumnText(const AString& columnTitle,
                                      const WuQTableWidgetModelColumnContent::ColumnAlignment columnAlignment,
                                      const TextEditable textEditable);
        
        virtual ~WuQTableWidgetModelColumnText();
        
        TextEditable getTextEditable() const;
        
    protected:
        const TextEditable m_textEditable;
        
        virtual void setNumberOfRows(const int32_t numberOfRows);
        
        std::vector<AString> m_values;
        
        friend class WuQTableWidgetModel;
    };
    
    /* ========================================================== */
    
    class WuQTableWidgetModel : public QObject {
        
        Q_OBJECT

    public:
        WuQTableWidgetModel(const std::vector<WuQTableWidgetModelColumnContent*>& columnContentInfo,
                            const int32_t numberOfRows);
        
        virtual ~WuQTableWidgetModel();
        
        int32_t getNumberOfRows() const;
        
        int32_t getNumberOfColumns() const;
        
        const WuQTableWidgetModelColumnContent* getColumnContentInfo(const int32_t columnIndex) const;
        
        bool isChecked(const int32_t rowIndex,
                       const int32_t columnIndex) const;
        
        double getDouble(const int32_t rowIndex,
                         const int32_t columnIndex) const;
        
        int32_t getInteger(const int32_t rowIndex,
                         const int32_t columnIndex) const;
        
        AString getText(const int32_t rowIndex,
                          const int32_t columnIndex) const;
        
        void getColorSwatch(const int32_t rowIndex,
                            const int32_t columnIndex,
                            float rgbaOut[4]) const;
        
        void setColorSwatch(const int32_t rowIndex,
                            const int32_t columnIndex,
                            const float rgba[4]);
        
        void setChecked(const int32_t rowIndex,
                        const int32_t columnIndex,
                        const bool checkBoxSelected);
        
        void setDouble(const int32_t rowIndex,
                         const int32_t columnIndex,
                         const double doubleValue);
        
        void setInteger(const int32_t rowIndex,
                           const int32_t columnIndex,
                        const int32_t integerValue);
        
        void setText(const int32_t rowIndex,
                          const int32_t columnIndex,
                       const AString& textValue);
        
        const QIcon* getIcon(const int32_t rowIndex,
                       const int32_t columnIndex) const;
        
    private:
        WuQTableWidgetModel(const WuQTableWidgetModel&);

        WuQTableWidgetModel& operator=(const WuQTableWidgetModel&);
        
    public:

        // ADD_NEW_METHODS_HERE

    private:
        // ADD_NEW_MEMBERS_HERE
        
        std::vector<WuQTableWidgetModelColumnContent*> m_columnContentInfo;
        
        int32_t m_numberOfRows;
        
        int32_t m_numberOfColumns;
    };
    
    
#ifdef __WU_Q_TABLE_WIDGET_MODEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WU_Q_TABLE_WIDGET_MODEL_DECLARE__

} // namespace
#endif  //__WU_Q_TABLE_WIDGET_MODEL_H__

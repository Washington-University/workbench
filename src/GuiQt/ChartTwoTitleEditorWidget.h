#ifndef __CHART_TWO_TITLE_EDITOR_WIDGET_H__
#define __CHART_TWO_TITLE_EDITOR_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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


#include <QWidget>

class QLineEdit;

namespace caret {

    class BrowserTabContent;
    class ChartTwoOverlaySet;
    class WuQDoubleSpinBox;
    
    class ChartTwoTitleEditorWidget : public QWidget {
        Q_OBJECT
        
    public:
        ChartTwoTitleEditorWidget(QWidget* parent,
                                  const QString& parentObjectName);
        
        virtual ~ChartTwoTitleEditorWidget();
     
        virtual void updateControls(ChartTwoOverlaySet* chartTwoOverlaySet);
        
        // ADD_NEW_METHODS_HERE

    private slots:
        void textLineEditChanged(const QString& text);
        
        void sizeSpinBoxValueChanged(double);
        
    private:
        ChartTwoTitleEditorWidget(const ChartTwoTitleEditorWidget&);

        ChartTwoTitleEditorWidget& operator=(const ChartTwoTitleEditorWidget&);
        
        void updateGraphics();
        
        QLineEdit* m_textLineEdit;
        
        WuQDoubleSpinBox* m_titleSizeSpinBox;
        
        WuQDoubleSpinBox* m_paddingSizeSpinBox;

        ChartTwoOverlaySet* m_chartOverlaySet;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_TITLE_EDITOR_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_TITLE_EDITOR_WIDGET_DECLARE__

} // namespace
#endif  //__CHART_TWO_TITLE_EDITOR_WIDGET_H__

#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ATTRIBUTES_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ATTRIBUTES_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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


#include "BrainBrowserWindowToolBarComponent.h"
#include "EventListenerInterface.h"

class QCheckBox;
class QDoubleSpinBox;
class QStackedWidget;

namespace caret {

    class CartesianChartTwoAttributesWidget;
    class ChartTwoMatrixDisplayProperties;
    class EnumComboBoxTemplate;
    class MatrixChartTwoAttributesWidget;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBarChartTwoAttributes : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarChartTwoAttributes(BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarChartTwoAttributes();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
        // ADD_NEW_METHODS_HERE

    private:
        BrainBrowserWindowToolBarChartTwoAttributes(const BrainBrowserWindowToolBarChartTwoAttributes&);

        BrainBrowserWindowToolBarChartTwoAttributes& operator=(const BrainBrowserWindowToolBarChartTwoAttributes&);
        
        //ChartModelCartesian* getCartesianChart();
        
        ChartTwoMatrixDisplayProperties* getChartableTwoMatrixDisplayProperties();
        
        void updateGraphics();
        
        CartesianChartTwoAttributesWidget* m_cartesianChartAttributesWidget;
        
        MatrixChartTwoAttributesWidget* m_matrixChartTwoAttributesWidget;
        
        QStackedWidget* m_stackedWidget;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class CartesianChartTwoAttributesWidget;
        friend class MatrixChartTwoAttributesWidget;
    };
    
    /*
     * While this should be a private class in the class above
     * Qt's meta-object compiler (moc) does not allow it to be.
     */
    class CartesianChartTwoAttributesWidget : public QWidget {
        Q_OBJECT
    
    public:
        CartesianChartTwoAttributesWidget(BrainBrowserWindowToolBarChartTwoAttributes* brainBrowserWindowToolBarChartAttributes);
        
        ~CartesianChartTwoAttributesWidget();
        
        virtual void updateContent();
        
    private slots:
        void cartesianLineWidthValueChanged(double value);

    private:
        BrainBrowserWindowToolBarChartTwoAttributes* m_brainBrowserWindowToolBarChartAttributes;
        
        QDoubleSpinBox* m_cartesianLineWidthDoubleSpinBox;
        
    };
    
    /*
     * While this should be a private class in the class above
     * Qt's meta-object compiler (moc) does not allow it to be.
     */
    class MatrixChartTwoAttributesWidget : public QWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        MatrixChartTwoAttributesWidget(BrainBrowserWindowToolBarChartTwoAttributes* brainBrowserWindowToolBarChartAttributes);
        
        ~MatrixChartTwoAttributesWidget();
        
        virtual void receiveEvent(Event* event);
        
        virtual void updateContent();
        
    private slots:
        void valueChanged();
        
    private:
        BrainBrowserWindowToolBarChartTwoAttributes* m_brainBrowserWindowToolBarChartAttributes;
        
        QDoubleSpinBox* m_cellWidthPercentageSpinBox;
        
        QDoubleSpinBox* m_cellHeightPercentageSpinBox;

        QCheckBox* m_highlightSelectionCheckBox;
        
        QCheckBox* m_displayGridLinesCheckBox;
        
        WuQWidgetObjectGroup* m_manualWidgetsGroup;
        
    };
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ATTRIBUTES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ATTRIBUTES_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_TWO_ATTRIBUTES_H__

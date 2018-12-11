#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_H__

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

    class CartesianChartAttributesWidget;
    class ChartMatrixDisplayProperties;
    class ChartModelCartesian;
    class EnumComboBoxTemplate;
    class MatrixChartAttributesWidget;
    class WuQWidgetObjectGroup;
    
    class BrainBrowserWindowToolBarChartAttributes : public BrainBrowserWindowToolBarComponent {
        Q_OBJECT
        
    public:
        BrainBrowserWindowToolBarChartAttributes(BrainBrowserWindowToolBar* parentToolBar,
                                                 const QString& parentObjectName);
        
        virtual ~BrainBrowserWindowToolBarChartAttributes();
        
        virtual void updateContent(BrowserTabContent* browserTabContent);
        
        // ADD_NEW_METHODS_HERE

    private:
        BrainBrowserWindowToolBarChartAttributes(const BrainBrowserWindowToolBarChartAttributes&);

        BrainBrowserWindowToolBarChartAttributes& operator=(const BrainBrowserWindowToolBarChartAttributes&);
        
        ChartModelCartesian* getCartesianChart();
        
        ChartMatrixDisplayProperties* getChartableMatrixDisplayProperties();
        
        void updateGraphics();
        
        CartesianChartAttributesWidget* m_cartesianChartAttributesWidget;
        
        MatrixChartAttributesWidget* m_matrixChartAttributesWidget;
        
        QStackedWidget* m_stackedWidget;
        
        // ADD_NEW_MEMBERS_HERE
        
        friend class CartesianChartAttributesWidget;
        friend class MatrixChartAttributesWidget;
    };
    
    /*
     * While this should be a private class in the class above
     * Qt's meta-object compiler (moc) does not allow it to be.
     */
    class CartesianChartAttributesWidget : public QWidget {
        Q_OBJECT
    
    public:
        CartesianChartAttributesWidget(BrainBrowserWindowToolBarChartAttributes* brainBrowserWindowToolBarChartAttributes,
                                       const QString& parentObjectName);
        
        ~CartesianChartAttributesWidget();
        
        virtual void updateContent();
        
    private slots:
        void cartesianLineWidthValueChanged(double value);

    private:
        BrainBrowserWindowToolBarChartAttributes* m_brainBrowserWindowToolBarChartAttributes;
        
        QDoubleSpinBox* m_cartesianLineWidthDoubleSpinBox;
        
    };
    
    /*
     * While this should be a private class in the class above
     * Qt's meta-object compiler (moc) does not allow it to be.
     */
    class MatrixChartAttributesWidget : public QWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        MatrixChartAttributesWidget(BrainBrowserWindowToolBarChartAttributes* brainBrowserWindowToolBarChartAttributes,
                                    const QString& parentObjectName);
        
        ~MatrixChartAttributesWidget();
        
        virtual void receiveEvent(Event* event);
        
        virtual void updateContent();
        
    private slots:
        void cellWidthSpinBoxValueChanged(double value);
        
        void cellHeightSpinBoxValueChanged(double value);
        
        void highlightSelectionCheckBoxClicked(bool checked);
        
        void displayGridLinesCheckBoxClicked(bool checked);
        
        void resetButtonClicked();
        
    private:
        BrainBrowserWindowToolBarChartAttributes* m_brainBrowserWindowToolBarChartAttributes;
        
        QDoubleSpinBox* m_cellWidthSpinBox;
        
        QDoubleSpinBox* m_cellHeightSpinBox;

        WuQWidgetObjectGroup* m_manualWidgetsGroup;
        
        QCheckBox* m_highlightSelectionCheckBox;
        
        QCheckBox* m_displayGridLinesCheckBox;
        
    };
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_CHART_ATTRIBUTES_H__

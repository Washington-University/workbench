#ifndef __CHART_TWO_LINE_LAYER_NORMALIZATION_WIDGET_H__
#define __CHART_TWO_LINE_LAYER_NORMALIZATION_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

class QDoubleSpinBox;
class QLabel;

namespace caret {

    class ChartTwoOverlay;

    class ChartTwoLineLayerNormalizationWidget : public QWidget {
        
        Q_OBJECT

    public:
        ChartTwoLineLayerNormalizationWidget();
        
        virtual ~ChartTwoLineLayerNormalizationWidget();
        
        ChartTwoLineLayerNormalizationWidget(const ChartTwoLineLayerNormalizationWidget&) = delete;

        ChartTwoLineLayerNormalizationWidget& operator=(const ChartTwoLineLayerNormalizationWidget&) = delete;
        
        void updateContent(ChartTwoOverlay* chartTwoOverlay);
        
        // ADD_NEW_METHODS_HERE
        
    private slots:
        void valueChanged();
        
    private:
        ChartTwoOverlay* m_chartTwoOverlay = NULL;
        
        QDoubleSpinBox* m_demeanSpinBox;
        
        QLabel* m_meanDevLabel;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_LINE_LAYER_NORMALIZATION_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_LINE_LAYER_NORMALIZATION_WIDGET_DECLARE__

} // namespace
#endif  //__CHART_TWO_LINE_LAYER_NORMALIZATION_WIDGET_H__

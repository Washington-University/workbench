#ifndef __CHART_TWO_OVERLAY_SET_VIEW_CONTROLLER_H__
#define __CHART_TWO_OVERLAY_SET_VIEW_CONTROLLER_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2016 Washington University School of Medicine
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

#include "EventListenerInterface.h"

class QLabel;
class QScrollArea;

namespace caret {
    class ChartTwoOverlaySet;
    class ChartTwoOverlayViewController;
    class WuQGridLayoutGroup;
    
    class ChartTwoOverlaySetViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        ChartTwoOverlaySetViewController(const Qt::Orientation orientation,
                                         const int32_t browserWindowIndex,
                                         const QString& parentObjectName,
                                         QWidget* parent = 0);
        
        virtual ~ChartTwoOverlaySetViewController();
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void processAddOverlayAbove(const int32_t overlayIndex);
        
        void processAddOverlayBelow(const int32_t overlayIndex);
        
        void processRemoveOverlay(const int32_t overlayIndex);
        
        void processMoveOverlayDown(const int32_t overlayIndex);
        
        void processMoveOverlayUp(const int32_t overlayIndex);
        
    private:
        ChartTwoOverlaySetViewController(const ChartTwoOverlaySetViewController&);

        ChartTwoOverlaySetViewController& operator=(const ChartTwoOverlaySetViewController&);
        
        ChartTwoOverlaySet* getChartTwoOverlaySet();
        
        void updateViewController();
        
        void updateColoringAndGraphics();
        
        const int32_t m_browserWindowIndex;
        
        std::vector<ChartTwoOverlayViewController*> m_chartOverlayViewControllers;

        std::vector<WuQGridLayoutGroup*> m_chartOverlayGridLayoutGroups;

        QLabel* m_allMapsLabel = NULL;
        
        QLabel* m_loadLabel = NULL;
        
        QLabel* m_opacityLabel = NULL;
        
        QLabel* m_lineWidthLabel = NULL;
        
        QLabel* m_pointLabel = NULL;
        
        QScrollArea* m_scrollArea;
        
        QLabel* m_mapRowOrColumnIndexLabel = NULL;
        
        QLabel* m_mapRowOrColumnNameLabel = NULL;

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __CHART_TWO_OVERLAY_SET_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_OVERLAY_SET_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_TWO_OVERLAY_SET_VIEW_CONTROLLER_H__

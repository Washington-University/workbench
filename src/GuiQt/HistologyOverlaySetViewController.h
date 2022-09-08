#ifndef __HISTOLOGY_OVERLAY_SET_VIEW_CONTROLLER__H_
#define __HISTOLOGY_OVERLAY_SET_VIEW_CONTROLLER__H_

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
#include <QWidget>

#include "EventListenerInterface.h"

class QScrollArea;
class QSpinBox;

namespace caret {

    class HistologyOverlaySet;
    class HistologyOverlayViewController;
    
    class HistologyOverlaySetViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        HistologyOverlaySetViewController(const Qt::Orientation orientation,
                                          const int32_t browserWindowIndex,
                                          const QString& parentObjectName,
                                          QWidget* parent = 0);

        virtual ~HistologyOverlaySetViewController();
        
        void receiveEvent(Event* event);
        
    private slots:
        void processAddOverlayAbove(const int32_t overlayIndex);
        
        void processAddOverlayBelow(const int32_t overlayIndex);
        
        void processRemoveOverlay(const int32_t overlayIndex);
        
        void processMoveOverlayDown(const int32_t overlayIndex);
        
        void processMoveOverlayUp(const int32_t overlayIndex);
        
    private:
        HistologyOverlaySetViewController(const HistologyOverlaySetViewController&);

        HistologyOverlaySetViewController& operator=(const HistologyOverlaySetViewController&);

        HistologyOverlaySet* getOverlaySet();
        
        void updateViewController();
        
        void updateColoringAndGraphics();
        
        std::vector<HistologyOverlayViewController*> m_overlayViewControllers;
        
        int32_t m_browserWindowIndex;
        
        QScrollArea* m_scrollArea;
    };
    
#ifdef __HISTOLOGY_OVERLAY_SET_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __HISTOLOGY_OVERLAY_SET_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__HISTOLOGY_OVERLAY_SET_VIEW_CONTROLLER__H_

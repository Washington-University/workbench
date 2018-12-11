#ifndef __OVERLAY_SET_VIEW_CONTROLLER__H_
#define __OVERLAY_SET_VIEW_CONTROLLER__H_

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

    class OverlaySet;
    class OverlayViewController;
    
    class OverlaySetViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        OverlaySetViewController(const Qt::Orientation orientation,
                                 const int32_t browserWindowIndex,
                                 const QString& parentObjectName,
                                 QWidget* parent = 0);
        
        virtual ~OverlaySetViewController();
        
        void receiveEvent(Event* event);
        
    private slots:
        void processAddOverlayAbove(const int32_t overlayIndex);
        
        void processAddOverlayBelow(const int32_t overlayIndex);
        
        void processRemoveOverlay(const int32_t overlayIndex);
        
        void processMoveOverlayDown(const int32_t overlayIndex);
        
        void processMoveOverlayUp(const int32_t overlayIndex);
        
    private:
        OverlaySetViewController(const OverlaySetViewController&);

        OverlaySetViewController& operator=(const OverlaySetViewController&);

        OverlaySet* getOverlaySet();
        
        void updateViewController();
        
        void updateColoringAndGraphics();
        
        std::vector<OverlayViewController*> overlayViewControllers;
        
        int32_t browserWindowIndex;
        
        QScrollArea* scrollArea;
    };
    
#ifdef __OVERLAY_SET_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OVERLAY_SET_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__OVERLAY_SET_VIEW_CONTROLLER__H_

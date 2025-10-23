#ifndef __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER__H_
#define __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER__H_

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

#include "BrowserTabContent.h"
#include "EventListenerInterface.h"

class QSpinBox;

namespace caret {

    class VolumeSurfaceOutlineSetModel;
    class VolumeSurfaceOutlineViewController;
    
    class VolumeSurfaceOutlineSetViewController : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        VolumeSurfaceOutlineSetViewController(const Qt::Orientation orientation,
                                              const int32_t browserWindowIndex,
                                              const QString& parentObjectNamePrefix,
                                              const QString& descriptivePrefix,
                                              QWidget* parent = 0);
        
        virtual ~VolumeSurfaceOutlineSetViewController();
        
        void receiveEvent(Event* event);
        
    private slots:
        void outlineCountSpinBoxValueChanged(int);
        
        void copyOutlinesActionTriggered();
        
    private:
        VolumeSurfaceOutlineSetViewController(const VolumeSurfaceOutlineSetViewController&);

        VolumeSurfaceOutlineSetViewController& operator=(const VolumeSurfaceOutlineSetViewController&);
 
        BrowserTabContent* getBrowserTabContent();
        
        VolumeSurfaceOutlineSetModel* getOutlineSet();
        
        void updateViewController();
        
        QSpinBox* outlineCountSpinBox;

        int32_t browserWindowIndex;
        
        std::vector<VolumeSurfaceOutlineViewController*> outlineViewControllers;
    };
    
#ifdef __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__VOLUME_SURFACE_OUTLINE_SET_VIEW_CONTROLLER__H_

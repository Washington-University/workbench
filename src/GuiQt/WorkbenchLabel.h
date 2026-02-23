#ifndef __WORKBENCH_LABEL_H__
#define __WORKBENCH_LABEL_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#include <QLabel>

#include "EventListenerInterface.h"
#include "GuiDarkLightColorSchemeModeEnum.h"
#include "WorkbenchIconTypeEnum.h"

namespace caret {

    class WorkbenchLabel : public QLabel, public EventListenerInterface {
        
        Q_OBJECT

    public:
        WorkbenchLabel(const WorkbenchIconTypeEnum::Enum iconType,
                       QWidget* parent = 0);
        
        virtual ~WorkbenchLabel();
        
        WorkbenchLabel(const WorkbenchLabel&) = delete;

        WorkbenchLabel& operator=(const WorkbenchLabel&) = delete;
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private:
        void updateForDarkLightColorScheme(const GuiDarkLightColorSchemeModeEnum::Enum darkLightColorSchemeMode);
        
        GuiDarkLightColorSchemeModeEnum::Enum getCurrentDarkLightColorSchemeMode() const;
        
        const WorkbenchIconTypeEnum::Enum m_iconType;
        
        QPixmap m_lightPixmap;
        
        QPixmap m_darkPixmap;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __WORKBENCH_LABEL_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __WORKBENCH_LABEL_DECLARE__

} // namespace
#endif  //__WORKBENCH_LABEL_H__

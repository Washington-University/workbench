#ifndef __VOLUME_OBLIQUE_OPTIONS_WIDGET_H__
#define __VOLUME_OBLIQUE_OPTIONS_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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
#include <vector>

#include <QWidget>

#include "EventListenerInterface.h"
#include "VolumeSliceInterpolationEdgeEffectsMaskingEnum.h"

class QAbstractButton;
class QRadioButton;

namespace caret {

    class BrowserTabContent;
    
    class VolumeObliqueOptionsWidget : public QWidget, public EventListenerInterface {
        
        Q_OBJECT

    public:
        VolumeObliqueOptionsWidget(const QString& objectNamePrefix,
                                   QWidget* parent = 0);
        
        virtual ~VolumeObliqueOptionsWidget();
        
        VolumeObliqueOptionsWidget(const VolumeObliqueOptionsWidget&) = delete;

        VolumeObliqueOptionsWidget& operator=(const VolumeObliqueOptionsWidget&) = delete;
        
        void updateContent(const int32_t tabIndex);
        

        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    private slots:
        void maskingButtonClicked(QAbstractButton*);
        
        
    private:
        BrowserTabContent* getBrowserTabContent();
        
        void updateGraphicsWindow();
        
        const QString m_objectNamePrefix;
        
        int32_t m_tabIndex = -1;
        
        std::vector<QRadioButton*> m_maskingRadioButtons;
        
        std::vector<VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum> m_maskingEnums;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __VOLUME_OBLIQUE_OPTIONS_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __VOLUME_OBLIQUE_OPTIONS_WIDGET_DECLARE__

} // namespace
#endif  //__VOLUME_OBLIQUE_OPTIONS_WIDGET_H__

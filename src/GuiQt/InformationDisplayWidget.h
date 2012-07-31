#ifndef __INFORMATION_DISPLAY_WIDGET__H_
#define __INFORMATION_DISPLAY_WIDGET__H_

/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <set>

#include <QWidget>
#include "EventListenerInterface.h"
#include "SceneableInterface.h"
class QAction;

namespace caret {

    class EventInformationTextDisplay;
    class HyperLinkTextBrowser;
    
    class InformationDisplayWidget : public QWidget, public EventListenerInterface, public SceneableInterface {
        
        Q_OBJECT

    public:
        InformationDisplayWidget(QWidget* parent = 0);
        
        virtual ~InformationDisplayWidget();
        
        void receiveEvent(Event* event);
        
        void processTextEvent(EventInformationTextDisplay* informationEvent);
        
        void updateInformationDisplayWidget();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private slots:
        
        void clearInformationText();
        
        void removeIdSymbols();
        
        void contralateralIdentificationToggled(bool);
        
        void volumeSliceIdentificationToggled(bool);
        
        void showPropertiesDialog();
        
    private:
        InformationDisplayWidget(const InformationDisplayWidget&);

        InformationDisplayWidget& operator=(const InformationDisplayWidget&);
        
        void updateAllInformationDisplayWidgets();
        
        HyperLinkTextBrowser* m_informationTextBrowser;
        
        static std::set<InformationDisplayWidget*> s_allInformationDisplayWidgets;
        
        QAction* m_contralateralIdentificationAction;
        
        QAction* m_volumeSliceIdentificationAction;
        
        QString m_informationText;
    };
    
#ifdef __INFORMATION_DISPLAY_WIDGET_DECLARE__
    std::set<InformationDisplayWidget*> InformationDisplayWidget::s_allInformationDisplayWidgets;
#endif // __INFORMATION_DISPLAY_WIDGET_DECLARE__

} // namespace
#endif  //__INFORMATION_DISPLAY_WIDGET__H_

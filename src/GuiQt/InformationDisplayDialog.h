#ifndef __INFORMATION_DISPLAY_DIALOG__H_
#define __INFORMATION_DISPLAY_DIALOG__H_

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

#include "EventListenerInterface.h"
#include "SceneableInterface.h"
#include "WuQDialogNonModal.h"

namespace caret {

    class BrainBrowserWindow;
    class HyperLinkTextBrowser;
    class InformationDisplayPropertiesDialog;
    
    class InformationDisplayDialog : public WuQDialogNonModal, public EventListenerInterface, public SceneableInterface {
        Q_OBJECT
        
    public:
        InformationDisplayDialog(BrainBrowserWindow* parent);
        
        virtual ~InformationDisplayDialog();
        
        void receiveEvent(Event* event);
        
        virtual void updateDialog();
        
        virtual SceneClass* saveToScene(const SceneAttributes* sceneAttributes,
                                        const AString& instanceName);
        
        virtual void restoreFromScene(const SceneAttributes* sceneAttributes,
                                      const SceneClass* sceneClass);
    private slots:
        
        void clearInformationText();
        
        void removeIdSymbols();
        
        void contralateralIdentificationToggled(bool);
        
        //void volumeSliceIdentificationToggled(bool);
        
        void showPropertiesDialog();
        
    private:
        InformationDisplayDialog(const InformationDisplayDialog&);

        InformationDisplayDialog& operator=(const InformationDisplayDialog&);

        HyperLinkTextBrowser* m_informationTextBrowser;
        
        QAction* m_contralateralIdentificationAction;
        
        QString m_informationText;
        
        InformationDisplayPropertiesDialog* m_propertiesDialog;
    };
    
#ifdef __INFORMATION_DISPLAY_DIALOG_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __INFORMATION_DISPLAY_DIALOG_DECLARE__

} // namespace
#endif  //__INFORMATION_DISPLAY_DIALOG__H_

#ifndef __BRAIN_BROWSER_WINDOW_TOOL_BAR_ORIENTATION_H__
#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_ORIENTATION_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#include "BrainBrowserWindowToolBarComponent.h"

class QIcon;
class QStackedLayout;
class QToolButton;

namespace caret {

    class BrowserTabContent;
    class CaretUndoStack;
    
    class BrainBrowserWindowToolBarOrientation : public BrainBrowserWindowToolBarComponent {

        Q_OBJECT        

    public:
        BrainBrowserWindowToolBarOrientation(const QString& parentObjectName,
                                             BrainBrowserWindowToolBar* parentToolBar);
        
        virtual ~BrainBrowserWindowToolBarOrientation();
        
        BrainBrowserWindowToolBarOrientation(const BrainBrowserWindowToolBarOrientation&) = delete;

        BrainBrowserWindowToolBarOrientation& operator=(const BrainBrowserWindowToolBarOrientation&) = delete;
        
        virtual void updateContent(BrowserTabContent* browserTabContent) override;
        

        // ADD_NEW_METHODS_HERE

    private slots:
        void orientationLeftOrLateralToolButtonTriggered(bool checked);
        void orientationRightOrMedialToolButtonTriggered(bool checked);
        void orientationAnteriorToolButtonTriggered(bool checked);
        void orientationPosteriorToolButtonTriggered(bool checked);
        void orientationDorsalToolButtonTriggered(bool checked);
        void orientationVentralToolButtonTriggered(bool checked);
        void orientationResetToolButtonTriggered(bool checked);
        
        void orientationLateralMedialToolButtonTriggered(bool checked);
        void orientationDorsalVentralToolButtonTriggered(bool checked);
        void orientationAnteriorPosteriorToolButtonTriggered(bool checked);
        
        void redoActionTriggered();
        void undoActionTriggered();
        void selectRegionActionTriggered();
        
    private:
        CaretUndoStack* getUndoStack();
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        QAction* orientationLateralMedialToolButtonAction;
        QAction* orientationDorsalVentralToolButtonAction;
        QAction* orientationAnteriorPosteriorToolButtonAction;
        
        QToolButton* orientationLateralMedialToolButton;
        QToolButton* orientationDorsalVentralToolButton;
        QToolButton* orientationAnteriorPosteriorToolButton;
        
        QAction* orientationLeftOrLateralToolButtonAction;
        QAction* orientationRightOrMedialToolButtonAction;
        QAction* orientationAnteriorToolButtonAction;
        QAction* orientationPosteriorToolButtonAction;
        QAction* orientationDorsalToolButtonAction;
        QAction* orientationVentralToolButtonAction;
        
        QToolButton* orientationLeftOrLateralToolButton;
        QToolButton* orientationRightOrMedialToolButton;
        QToolButton* orientationAnteriorToolButton;
        QToolButton* orientationPosteriorToolButton;
        QToolButton* orientationDorsalToolButton;
        QToolButton* orientationVentralToolButton;
        
        QAction* orientationResetToolButtonAction;
        QToolButton* orientationCustomViewSelectToolButton;
        
        QIcon* viewOrientationLeftIcon;
        QIcon* viewOrientationRightIcon;
        QIcon* viewOrientationAnteriorIcon;
        QIcon* viewOrientationPosteriorIcon;
        QIcon* viewOrientationDorsalIcon;
        QIcon* viewOrientationVentralIcon;
        QIcon* viewOrientationLeftLateralIcon;
        QIcon* viewOrientationLeftMedialIcon;
        QIcon* viewOrientationRightLateralIcon;
        QIcon* viewOrientationRightMedialIcon;

        QAction* m_redoAction;
        QAction* m_undoAction;
        QAction* m_selectRegionAction;
        
        QWidget* m_emptyViewWidget;
        QWidget* m_singleViewWidget;
        QWidget* m_dualViewWidget;
        QStackedLayout* m_singleDualViewLayout;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __BRAIN_BROWSER_WINDOW_TOOL_BAR_ORIENTATION_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __BRAIN_BROWSER_WINDOW_TOOL_BAR_ORIENTATION_DECLARE__

} // namespace
#endif  //__BRAIN_BROWSER_WINDOW_TOOL_BAR_ORIENTATION_H__

#ifndef __OVERLAY_VIEW_CONTROLLER__H_
#define __OVERLAY_VIEW_CONTROLLER__H_

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

#include <QWidget>

class QAction;
class QCheckBox;
class QComboBox;
class QDoubleSpinBox;
class QGridLayout;
class QMenu;
class QSpinBox;
class QToolButton;

namespace caret {

    class AString;
    class MapYokingGroupComboBox;
    class Overlay;
    class WuQGridLayoutGroup;
    
    class OverlayViewController : public QObject {
        
        Q_OBJECT

    public:
        OverlayViewController(const Qt::Orientation orientation,
                              QGridLayout* gridLayout,
                              const int32_t browserWindowIndex,
                              const int32_t overlayIndex,
                              QObject* parent);
        
        virtual ~OverlayViewController();
        
        void setVisible(bool visible);
        
        void updateViewController(Overlay* overlay);
        
    signals:
        void requestAddOverlayAbove(const int32_t overlayIndex);
        
        void requestAddOverlayBelow(const int32_t overlayIndex);
        
        void requestRemoveOverlay(const int32_t overlayIndex);
        
        void requestMoveOverlayUp(const int32_t overlayIndex);
        
        void requestMoveOverlayDown(const int32_t overlayIndex);
        
    private slots:
        void fileComboBoxSelected(int);
        
        void mapNameComboBoxSelected(int);
        
        void mapIndexSpinBoxValueChanged(int);
        
        void enabledCheckBoxClicked(bool);
        
        void colorBarActionTriggered(bool);
        
        void settingsActionTriggered();
                
        void opacityDoubleSpinBoxValueChanged(double value);
        
        void yokingGroupActivated();
        
        void menuAddOverlayAboveTriggered();
        
        void menuAddOverlayBelowTriggered();
        
        void menuRemoveOverlayTriggered();
        
        void menuMoveOverlayDownTriggered();
        
        void menuMoveOverlayUpTriggered();        
        
        void menuReloadFileTriggered();
        
        void menuCopyFileNameToClipBoard();
        
        void menuCopyMapNameToClipBoard();
        
        void menuConstructionAboutToShow();
        
    private:
        OverlayViewController(const OverlayViewController&);

        OverlayViewController& operator=(const OverlayViewController&);

        void updateUserInterfaceAndGraphicsWindow();
        
        void updateUserInterface();
        
        void updateGraphicsWindow();
        
        QMenu* createConstructionMenu(QWidget* parent,
                                      const AString& menuActionNamePrefix);
        
        void validateYokingSelection();
        
        void updateOverlaySettingsEditor();
        
        const int32_t browserWindowIndex;
        
        const int32_t m_overlayIndex;
        
        Overlay* overlay;
        
        QCheckBox* enabledCheckBox;
        
        QComboBox* fileComboBox;
        
        QComboBox* mapNameComboBox;
        
        QSpinBox* m_mapIndexSpinBox;
        
        QDoubleSpinBox* opacityDoubleSpinBox;
        
        QToolButton* m_constructionToolButton;
        
        QAction* constructionAction;

        QToolButton* m_colorBarToolButton;
        //QAction* colorBarAction;
        
        QAction* settingsAction;
        
        MapYokingGroupComboBox* m_mapYokingGroupComboBox;
        
        QAction* m_constructionReloadFileAction;
        
        QAction* m_copyPathAndFileNameToClipboardAction;
        
        WuQGridLayoutGroup* gridLayoutGroup;
        
    friend class OverlaySetViewController;
        
    };
    
#ifdef __OVERLAY_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __OVERLAY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__OVERLAY_VIEW_CONTROLLER__H_

#ifndef __CHART_OVERLAY_VIEW_CONTROLLER_H__
#define __CHART_OVERLAY_VIEW_CONTROLLER_H__

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


#include <QObject>


class QAction;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QMenu;
class QSpinBox;
class QToolButton;

namespace caret {
    class ChartOverlay;
    class MapYokingGroupComboBox;
    class WuQGridLayoutGroup;

    class ChartOverlayViewController : public QObject {
        
        Q_OBJECT

    public:
        ChartOverlayViewController(const Qt::Orientation orientation,
                                   const int32_t browserWindowIndex,
                                   const int32_t chartOverlayIndex,
                                   QObject* parent);
        
        virtual ~ChartOverlayViewController();

        void updateViewController(ChartOverlay* overlay);
        
        // ADD_NEW_METHODS_HERE
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
        
        void yokingGroupActivated();
        
        void historyActionTriggered(bool);
        
        void allMapsCheckBoxClicked(bool);
        
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
        ChartOverlayViewController(const ChartOverlayViewController&);

        ChartOverlayViewController& operator=(const ChartOverlayViewController&);
        
        void updateUserInterfaceAndGraphicsWindow();
        
        void updateUserInterface();
        
        void updateGraphicsWindow();
        
        QMenu* createConstructionMenu(QWidget* parent);
        
        void validateYokingSelection();
        
        void updateOverlaySettingsEditor();
        
        QPixmap createHistoryPixmap(QWidget* widget);
        
        const int32_t m_browserWindowIndex;
        
        const int32_t m_chartOverlayIndex;
        
        ChartOverlay* m_chartOverlay;
        
        QCheckBox* m_enabledCheckBox;
        
        QToolButton* m_settingsToolButton;
        
        QAction* m_settingsAction;
        
        QToolButton* m_colorBarToolButton;
        
        QAction* m_colorBarAction;
        
        QAction* m_constructionAction;
        
        QToolButton* m_historyToolButton;
        
        QAction* m_historyAction;
        
        QComboBox* m_mapFileComboBox;
        
        MapYokingGroupComboBox* m_mapYokingGroupComboBox;

        QCheckBox* m_allMapsCheckBox;
        
        QSpinBox* m_mapIndexSpinBox;
        
        QComboBox* m_mapNameComboBox;
        
        QToolButton* m_constructionToolButton;
        
        QAction* m_constructionReloadFileAction;
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ChartOverlaySetViewController;
    };
    
#ifdef __CHART_OVERLAY_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_OVERLAY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_OVERLAY_VIEW_CONTROLLER_H__

#ifndef __CHART_TWO_OVERLAY_VIEW_CONTROLLER_H__
#define __CHART_TWO_OVERLAY_VIEW_CONTROLLER_H__

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
#include <QPixmap>

#include "ChartAxisLocationEnum.h"
#include "ChartTwoMatrixTriangularViewingModeEnum.h"

class QAction;
class QCheckBox;
class QComboBox;
class QGridLayout;
class QMenu;
class QSpinBox;
class QToolButton;

namespace caret {
    class CaretColor;
    class CaretColorToolButton;
    class ChartTwoOverlay;
    class MapYokingGroupComboBox;
    class WuQDoubleSpinBox;
    class WuQGridLayoutGroup;

    class ChartTwoOverlayViewController : public QObject {
        
        Q_OBJECT

    public:
        ChartTwoOverlayViewController(const Qt::Orientation orientation,
                                      const int32_t browserWindowIndex,
                                      const int32_t chartOverlayIndex,
                                      const QString& parentObjectName,
                                      QObject* parent);
        
        virtual ~ChartTwoOverlayViewController();

        void updateViewController(ChartTwoOverlay* overlay);
        
        // ADD_NEW_METHODS_HERE
    signals:
        void requestAddOverlayAbove(const int32_t overlayIndex);
        
        void requestAddOverlayBelow(const int32_t overlayIndex);
        
        void requestRemoveOverlay(const int32_t overlayIndex);
        
        void requestMoveOverlayUp(const int32_t overlayIndex);
        
        void requestMoveOverlayDown(const int32_t overlayIndex);
        
    private slots:
        void fileComboBoxSelected(int);
        
        void mapRowOrColumnNameComboBoxSelected(int);
        
        void mapRowOrColumnIndexSpinBoxValueChanged(int);
        
        void enabledCheckBoxClicked(bool);
        
        void lineSeriesLoadingEnabledCheckBoxClicked(bool);
        
        void colorBarActionTriggered(bool);
        
        void settingsActionTriggered();
        
        void yokingGroupActivated();
        
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

        void menuConstructionPreColorAllFiles();
        
        void menuMatrixTriangularViewModeTriggered(QAction* action);
        
        void menuAxisLocationTriggered(QAction* action);
        
        void lineLayerColorSelected(const CaretColor& caretColor);
        
        void lineLayerLineWidthChanged(const float lineWidth);
        
    private:
        ChartTwoOverlayViewController(const ChartTwoOverlayViewController&);

        ChartTwoOverlayViewController& operator=(const ChartTwoOverlayViewController&);
        
        void updateUserInterfaceAndGraphicsWindow();
        
        void updateUserInterface();
        
        void updateGraphicsWindow();
        
        QMenu* createConstructionMenu(QWidget* parent,
                                      const QString& parentObjectName,
                                      const QString& descriptivePrefix);
        
        QMenu* createMatrixTriangularViewModeMenu(QWidget* widget,
                                                  const QString& parentObjectName,
                                                  const QString& descriptivePrefix);
        
        QMenu* createAxisLocationMenu(QWidget* widget,
                                      const QString& parentObjectName,
                                      const QString& descriptivePrefix);
        
        void validateYokingSelection();
        
        void updateOverlaySettingsEditor();
        
        QPixmap createMatrixTriangularViewModePixmap(QWidget* widget,
                                                     const ChartTwoMatrixTriangularViewingModeEnum::Enum matrixViewMode);

        QPixmap createAxisLocationPixmap(QWidget* widget,
                                         const ChartAxisLocationEnum::Enum axisLocation);
        
        void updateMatrixTriangularViewModeAction(const ChartTwoMatrixTriangularViewingModeEnum::Enum matrixViewMode);
        
        void updateAxisLocationAction(const ChartAxisLocationEnum::Enum axisLocation);
        
        const int32_t m_browserWindowIndex;
        
        const int32_t m_chartOverlayIndex;
        
        ChartTwoOverlay* m_chartOverlay;
        
        QCheckBox* m_enabledCheckBox;
        
        QCheckBox* m_lineSeriesLoadingEnabledCheckBox;
        
        QToolButton* m_settingsToolButton;
        
        QAction* m_settingsAction;
        
        QToolButton* m_colorBarToolButton;
        
        QAction* m_colorBarAction;
        
        QAction* m_constructionAction;
        
        QToolButton* m_matrixTriangularViewModeToolButton;
        
        QAction* m_matrixTriangularViewModeAction;
        
        QAction* m_axisLocationAction;
        
        QToolButton* m_axisLocationToolButton;
        
        CaretColorToolButton* m_lineLayerColorToolButton;
        
        std::vector<std::tuple<ChartTwoMatrixTriangularViewingModeEnum::Enum, QAction*,QPixmap>> m_matrixViewMenuData;

        std::vector<std::tuple<ChartAxisLocationEnum::Enum, QAction*,QPixmap>> m_axisLocationMenuData;
        
        QComboBox* m_mapFileComboBox;
        
        MapYokingGroupComboBox* m_mapRowOrColumnYokingGroupComboBox;

        QCheckBox* m_allMapsCheckBox;
        
        QSpinBox* m_mapRowOrColumnIndexSpinBox;
        
        QComboBox* m_mapRowOrColumnNameComboBox;
        
        QToolButton* m_constructionToolButton;
        
        QAction* m_constructionReloadFileAction;
        
        WuQDoubleSpinBox* m_lineLayerWidthSpinBox = NULL;

        // ADD_NEW_MEMBERS_HERE

        friend class ChartTwoOverlaySetViewController;
    };
    
#ifdef __CHART_TWO_OVERLAY_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_OVERLAY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_TWO_OVERLAY_VIEW_CONTROLLER_H__

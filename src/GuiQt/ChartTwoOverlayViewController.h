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
    class ChartTwoLineLayerNormalizationWidget;
    class ChartTwoOverlay;
    class EnumComboBoxTemplate;
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
        
        void matrixOpacityValueChanged(double value);
        
        void constructionToolButtonClicked();
        
        void lineLayerColorSelected(const CaretColor& caretColor);
        
        void lineLayerLineWidthChanged(const float lineWidth);
        
        void lineLayerToolTipOffsetToolButtonClicked();
        
        void lineLayerActiveModeEnumComboBoxItemActivated();
        
        void lineLayerNormalizationMenuAboutToHide();
        
        void lineLayerNormalizationMenuAboutToShow();
        
        void lineLayerNormalizationToolButtonClicked();
        
        void selectedPointIndexSpinBoxValueChanged(int index);
        
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
        
        void validateYokingSelection();
        
        void updateOverlaySettingsEditor();
        
        QPixmap createMatrixTriangularViewModePixmap(QWidget* widget,
                                                     const ChartTwoMatrixTriangularViewingModeEnum::Enum matrixViewMode);

        QPixmap createCardinalDirectionPixmap(QWidget* widget);
        
        void updateMatrixTriangularViewModeAction(const ChartTwoMatrixTriangularViewingModeEnum::Enum matrixViewMode);
        
        void updateLineLayerToolTipOffsetToolButton();
        
        QPixmap createNormalizationPixmap(QWidget *widget);
        
        const int32_t m_browserWindowIndex;
        
        const int32_t m_chartOverlayIndex;
        
        ChartTwoOverlay* m_chartOverlay;
        
        const AString m_parentObjectName;
        
        QCheckBox* m_enabledCheckBox;
        
        QCheckBox* m_lineSeriesLoadingEnabledCheckBox;
        
        QToolButton* m_settingsToolButton;
        
        QAction* m_settingsAction;
        
        QToolButton* m_colorBarToolButton;
        
        QAction* m_colorBarAction;
        
        QMenu* m_constructionMenu;
        
        QToolButton* m_matrixTriangularViewModeToolButton;
        
        QAction* m_matrixTriangularViewModeAction;
        
        CaretColorToolButton* m_lineLayerColorToolButton;
        
        QToolButton* m_lineLayerToolTipOffsetToolButton;
        
        bool m_useIconInLineLayerToolTipOffsetButtonFlag = false;
        
        QToolButton* m_lineLayerNormalizationToolButton;
        
        ChartTwoLineLayerNormalizationWidget* m_lineLayerNormalizationWidget;
        
        QMenu* m_lineLayerNormalizationMenu;
        
        std::vector<std::tuple<ChartTwoMatrixTriangularViewingModeEnum::Enum, QAction*,QPixmap>> m_matrixViewMenuData;

        QComboBox* m_mapFileComboBox;
        
        MapYokingGroupComboBox* m_mapRowOrColumnYokingGroupComboBox;

        QCheckBox* m_allMapsCheckBox;
        
        QSpinBox* m_mapRowOrColumnIndexSpinBox;
        
        QComboBox* m_mapRowOrColumnNameComboBox;
        
        QToolButton* m_constructionToolButton;
        
        QAction* m_constructionReloadFileAction;
        
        WuQDoubleSpinBox* m_lineLayerWidthSpinBox = NULL;

        EnumComboBoxTemplate* m_lineLayerActiveComboBox = NULL;
        
        WuQDoubleSpinBox* m_matrixOpacitySpinBox = NULL;
        
        QSpinBox* m_selectedPointIndexSpinBox = NULL;
        
        // ADD_NEW_MEMBERS_HERE

        friend class ChartTwoOverlaySetViewController;
    };
    
#ifdef __CHART_TWO_OVERLAY_VIEW_CONTROLLER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __CHART_TWO_OVERLAY_VIEW_CONTROLLER_DECLARE__

} // namespace
#endif  //__CHART_TWO_OVERLAY_VIEW_CONTROLLER_H__

#ifndef __TILE_TABS_MANUAL_TAB_GEOMETRY_WIDGET_H__
#define __TILE_TABS_MANUAL_TAB_GEOMETRY_WIDGET_H__

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

#include <QObject>

class QCheckBox;
class QGridLayout;
class QLabel;
class QDoubleSpinBox;
class QSpinBox;

namespace caret {

    class BrowserTabContent;
    class EnumComboBoxTemplate;
    class TileTabsConfigurationDialog;
    class WuQGridLayoutGroup;
    
    class TileTabsManualTabGeometryWidget : public QObject {
        
        Q_OBJECT

    public:
        TileTabsManualTabGeometryWidget(TileTabsConfigurationDialog* tileTabsConfigurationDialog,
                                        const int32_t index,
                                        QGridLayout* gridLayout,
                                        QObject* parent);
        
        virtual ~TileTabsManualTabGeometryWidget();
        
        TileTabsManualTabGeometryWidget(const TileTabsManualTabGeometryWidget&) = delete;

        TileTabsManualTabGeometryWidget& operator=(const TileTabsManualTabGeometryWidget&) = delete;
        
        void updateContent(BrowserTabContent* browserTabContent);

        static QString getStackOrderToolTipText();
        
        // ADD_NEW_METHODS_HERE

    signals:
        void itemChanged();

    private slots:
        void showCheckBoxClicked(bool status);
        
        void xMinSpinBoxValueChanged(double value);
        
        void xMaxSpinBoxValueChanged(double value);
        
        void yMinSpinBoxValueChanged(double value);
        
        void yMaxSpinBoxValueChanged(double value);
        
        void stackingOrderValueChanged(int value);
        
        void tileTabsLayoutBackgroundTypeEnumComboBoxItemActivated();
        
    private:
        QDoubleSpinBox* createPercentSpinBox(const QString& toolTip,
                                             const bool minValueFlag);
        
        void updatePercentSpinBox(QDoubleSpinBox* spinBox,
                                  const double value);
        
        TileTabsConfigurationDialog* m_tileTabsConfigurationDialog;
        
        /** index of this item; NOT the tab index */
        const int32_t m_index;
        
        BrowserTabContent* m_browserTabContent = NULL;
        
        QCheckBox* m_showTabCheckBox;
        
        QLabel* m_tabNumberLabel;
        
        QDoubleSpinBox* m_xMinSpinBox;
        
        QDoubleSpinBox* m_xMaxSpinBox;
        
        QDoubleSpinBox* m_yMinSpinBox;
        
        QDoubleSpinBox* m_yMaxSpinBox;
        
        QSpinBox* m_stackingOrderSpinBox;
        
        EnumComboBoxTemplate* m_TileTabsLayoutBackgroundTypeEnumComboBox;
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __TILE_TABS_MANUAL_TAB_GEOMETRY_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TABS_MANUAL_TAB_GEOMETRY_WIDGET_DECLARE__

} // namespace
#endif  //__TILE_TABS_MANUAL_TAB_GEOMETRY_WIDGET_H__

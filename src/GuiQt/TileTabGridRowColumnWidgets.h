#ifndef __TILE_TAB_GRID_ROW_COLUMN_WIDGETS_H__
#define __TILE_TAB_GRID_ROW_COLUMN_WIDGETS_H__

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

#include "EventTileTabsGridConfigurationModification.h"

class QAction;
class QDoubleSpinBox;
class QGridLayout;
class QLabel;
class QMenu;
class QToolButton;

namespace caret {

    class EnumComboBoxTemplate;
    class TileTabsConfigurationDialog;
    class TileTabsGridRowColumnElement;
    class WuQGridLayoutGroup;
    
    /**
     * Contains widgets for one row or column of stretching.
     */
    class TileTabGridRowColumnWidgets : public QObject {
        Q_OBJECT
        
    public:
        TileTabGridRowColumnWidgets(TileTabsConfigurationDialog* tileTabsConfigurationDialog,
                              const EventTileTabsGridConfigurationModification::RowColumnType rowColumnType,
                              const int32_t index,
                              QGridLayout* gridLayout,
                              QObject* parent);
        
        virtual ~TileTabGridRowColumnWidgets();
        
        void updateContent(TileTabsGridRowColumnElement* element);
        
    signals:
        void itemChanged();
        
        void modificationRequested(EventTileTabsGridConfigurationModification& modification);
        
        private slots:
        void constructionMenuAboutToShow();
        
        void constructionMenuTriggered(QAction*);
        
        void contentTypeActivated();
        
        void stretchTypeActivated();
        
        void stretchValueChanged(double);
        
    private:
        QMenu* createConstructionMenu(QToolButton* toolButton);
        
        TileTabsConfigurationDialog* m_tileTabsConfigurationDialog;
        const EventTileTabsGridConfigurationModification::RowColumnType m_rowColumnType;
        const int32_t m_index;
        TileTabsGridRowColumnElement* m_element;
        
        QLabel* m_indexLabel;
        QAction* m_constructionAction;
        QToolButton* m_constructionToolButton;
        EnumComboBoxTemplate* m_contentTypeComboBox;
        EnumComboBoxTemplate* m_stretchTypeComboBox;
        QDoubleSpinBox* m_stretchValueSpinBox;
        
        QAction* m_menuDeleteAction;
        QAction* m_menuDuplicateAfterAction;
        QAction* m_menuDuplicateBeforeAction;
        QAction* m_insertSpacerAfterAction;
        QAction* m_insertSpacerBeforeAction;
        QAction* m_menuMoveAfterAction;
        QAction* m_menuMoveBeforeAction;
        
        WuQGridLayoutGroup* m_gridLayoutGroup;
        
    };

    
#ifdef __TILE_TAB_GRID_ROW_COLUMN_WIDGETS_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __TILE_TAB_GRID_ROW_COLUMN_WIDGETS_DECLARE__

} // namespace
#endif  //__TILE_TAB_GRID_ROW_COLUMN_WIDGETS_H__

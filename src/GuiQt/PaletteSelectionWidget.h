#ifndef __PALETTE_SELECTION_WIDGET_H__
#define __PALETTE_SELECTION_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#include <QWidget>

#include "PalettePixmapPainter.h"

class QComboBox;
class QListWidget;
class QListWidgetItem;

namespace caret {

    class PaletteGroup;
    class PaletteNew;

    class PaletteSelectionWidget : public QWidget {
        
        Q_OBJECT

    public:
        PaletteSelectionWidget(QWidget* parent= 0);
        
        virtual ~PaletteSelectionWidget();
        
        PaletteSelectionWidget(const PaletteSelectionWidget&) = delete;

        PaletteSelectionWidget& operator=(const PaletteSelectionWidget&) = delete;

        void updateContent();
        
        PaletteGroup* getSelectedPaletteGroup() const;
        
        std::unique_ptr<PaletteNew> getSelectedPalette() const;

        // ADD_NEW_METHODS_HERE

    signals:
        /**
         * Emitted when the user selects a palette
         * @param palette
         *    The palette selected or NULL if no palette selected
         */
        void paletteSelectionChanged();
        
    private slots:
        
        void paletteGroupComboBoxActivated(int index);
        
        void paletteListWidgetActivated(QListWidgetItem* item);
        
    private:
        void createUserPalettes();
        
        void updateGroupSelectionComboBox();
        
        void updatePaletteSelectionListWidget();
        
        PalettePixmapPainter::Mode m_pixmapMode = PalettePixmapPainter::Mode::INTERPOLATE_ON;

        QComboBox* m_paletteGroupComboBox;
        
        QListWidget* m_paletteSelectionListWidget;
        
        std::vector<std::weak_ptr<PaletteGroup>> m_paletteGroups;        

        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_SELECTION_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_SELECTION_WIDGET_DECLARE__

} // namespace
#endif  //__PALETTE_SELECTION_WIDGET_H__

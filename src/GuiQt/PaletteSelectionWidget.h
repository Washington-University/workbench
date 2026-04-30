#ifndef __PALETTE_SELECTION_WIDGET_H__
#define __PALETTE_SELECTION_WIDGET_H__

/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#include <QComboBox>
#include <QWidget>

#include <memory>

#include "EventListenerInterface.h"
#include "PaletteDesignTypeEnum.h"

class QListWidget;
class QListWidgetItem;

namespace caret {
    class PaletteBase;

    class PaletteSelectionWidget : public QWidget, public EventListenerInterface {
        Q_OBJECT
        
    public:
        enum class WidgetType {
            COMBO_BOX,
            LIST_WIDGET
        };
        
        PaletteSelectionWidget(const WidgetType widgetType,
                               const std::vector<PaletteDesignTypeEnum::Enum>& paletteDesignTypes,
                               QWidget* parent = 0);
        
        virtual ~PaletteSelectionWidget();
        
        PaletteSelectionWidget(const PaletteSelectionWidget&) = delete;

        PaletteSelectionWidget& operator=(const PaletteSelectionWidget&) = delete;
        
        const PaletteBase* getSelectedPalette() const;

        AString getSelectedPaletteName() const;
        
        void selectPalette(const PaletteBase* paletteBase);
        
        void updateContent(const bool forceUpdate = false);
        
        // ADD_NEW_METHODS_HERE

        virtual void receiveEvent(Event* event);

    signals:
        void paletteSelected(const PaletteBase* paletteBase);
        
    private slots:
        void comboBoxActivated(int index);
        
        void listWidgetItemClicked(QListWidgetItem* item);
        
    private:
        QPixmap createPixmapForPalette(const PaletteBase* paletteBase);
        
        const WidgetType m_widgetType;
        
        const std::vector<PaletteDesignTypeEnum::Enum> m_paletteDesignTypes;
        
        QComboBox*   m_paletteComboBox;
        
        QListWidget* m_paletteListWidget;
        
        AString m_paletteBeingEditedName;
        
        std::vector<const PaletteBase*> m_previouslyLoadedPalettes;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PALETTE_SELECTION_WIDGET_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PALETTE_SELECTION_WIDGET_DECLARE__

} // namespace
#endif  //__PALETTE_SELECTION_WIDGET_H__

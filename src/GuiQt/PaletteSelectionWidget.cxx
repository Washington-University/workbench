
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

#define __PALETTE_SELECTION_WIDGET_DECLARE__
#include "PaletteSelectionWidget.h"
#undef __PALETTE_SELECTION_WIDGET_DECLARE__

#include <QComboBox>
#include <QGridLayout>
#include <QListWidget>

#include "CaretAssert.h"
#include "EventManager.h"
#include "EventPalettesGetOperations.h"
#include "Palette.h"
#include "PaletteNew.h"
#include "PalettePixmapPainter.h"

using namespace caret;


    
/**
 * \class caret::PaletteSelectionWidget 
 * \brief Widget for selecting palette
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param widgetType
 *    Type of widget used for palette selection
 * @param paletteDesignTypes
 *    Types of palettes for selection
 * @param parent
 *    The parent widget
 */
PaletteSelectionWidget::PaletteSelectionWidget(const WidgetType widgetType,
                                               const std::vector<PaletteDesignTypeEnum::Enum>& paletteDesignTypes,
                                               QWidget* parent)
: QWidget(parent),
m_widgetType(widgetType),
m_paletteDesignTypes(paletteDesignTypes)
{
    
    QWidget* selectionWidget(NULL);
    switch (m_widgetType) {
        case WidgetType::COMBO_BOX:
            m_paletteComboBox = new QComboBox();
            selectionWidget   = m_paletteComboBox;
            QObject::connect(m_paletteComboBox, &QComboBox::activated,
                             this, &PaletteSelectionWidget::comboBoxActivated);
            break;
        case WidgetType::LIST_WIDGET:
            m_paletteListWidget = new QListWidget();
            QObject::connect(m_paletteListWidget, &QListWidget::itemClicked,
                             this, &PaletteSelectionWidget::listWidgetItemClicked);
            selectionWidget     = m_paletteListWidget;
            break;
    }
    CaretAssert(selectionWidget);
    
    QGridLayout* layout(new QGridLayout(this));
    layout->addWidget(selectionWidget, 0, 0);
    
    updateContent();
//    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
PaletteSelectionWidget::~PaletteSelectionWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

const PaletteBase*
PaletteSelectionWidget::getSelectedPalette() const
{
    const PaletteBase* palette(NULL);
    
    switch (m_widgetType) {
        case WidgetType::COMBO_BOX:
        {
            const int32_t index(m_paletteComboBox->currentIndex());
            if (index >= 0) {
                palette = m_paletteComboBox->itemData(index).value<const PaletteBase*>();
            }
        }
            break;
        case WidgetType::LIST_WIDGET:
        {
            const QListWidgetItem* item(m_paletteListWidget->currentItem());
            if (item != NULL) {
                palette = item->data(Qt::UserRole).value<const PaletteBase*>();
            }
        }
            break;
    }

    return palette;
}
/**
 * @return Name of the selected palette or empty if no selection
 */
AString
PaletteSelectionWidget::getSelectedPaletteName() const
{
    AString name;
    
    switch (m_widgetType) {
        case WidgetType::COMBO_BOX:
            CaretAssert(m_paletteComboBox);
            name = m_paletteComboBox->currentText();
            break;
        case WidgetType::LIST_WIDGET:
            CaretAssert(m_paletteListWidget);
            const QList<QListWidgetItem*> previousItemsList(m_paletteListWidget->selectedItems());
            if ( ! previousItemsList.isEmpty()) {
                name = previousItemsList.at(0)->text();
            }
            break;
    }
    
    return name;
}

/**
 * Called when the combo box is activated
 * @param index
 *    Index of item selected
 */
void
PaletteSelectionWidget::comboBoxActivated(int index)
{
    CaretAssert(m_paletteComboBox);
    if ((index >= 0)
        && (index < m_paletteComboBox->count())) {
        const PaletteBase* palette(m_paletteComboBox->itemData(index).value<const PaletteBase*>());
        if (palette != NULL) {
            emit paletteSelected(palette);
        }
    }
}

/**
 * Called when a  palette is selected in the list widget
 * @param item
 *    List widget item containing palette that was selected
 */
void
PaletteSelectionWidget::listWidgetItemClicked(QListWidgetItem* item)
{
//    m_paletteBeingEdited = NULL;
    if (item != NULL) {
        const PaletteBase* palette(item->data(Qt::UserRole).value<const PaletteBase*>());
        if (palette != NULL) {
            emit paletteSelected(palette);
        }
    }
//    
//    loadPaletteIntoEditor();
//    
//    updatePaletteMovementButtons();
}

void
PaletteSelectionWidget::selectPalette(const PaletteBase* paletteBase)
{
    switch (m_widgetType) {
        case WidgetType::COMBO_BOX:
        {
            CaretAssert(m_paletteComboBox);
            QSignalBlocker blocker(m_paletteComboBox);
            for (int32_t i = 0; i < m_paletteComboBox->count(); i++) {
                if (paletteBase == m_paletteComboBox->itemData(i).value<const PaletteBase*>()) {
                    m_paletteComboBox->setCurrentIndex(i);
                    break;
                }
            }
        }
            break;
        case WidgetType::LIST_WIDGET:
        {
            QSignalBlocker blocker(m_paletteListWidget);
            CaretAssert(m_paletteListWidget);
            for (int32_t i = 0; i < m_paletteListWidget->count(); i++) {
                if (paletteBase == m_paletteListWidget->item(i)->data(Qt::UserRole).value<const PaletteBase*>()) {
                    m_paletteListWidget->setCurrentRow(i);
                    break;
                }
            }
        }
            break;
    }
}

/**
 * Update the content of the dialog
 * @param forceUpdate
 *   If update only if the palette pointers have changed.
 */
void
PaletteSelectionWidget::updateContent(const bool forceUpdate)
{
    const std::vector<const PaletteBase*> palettes(EventPalettesGetOperations::getAllPaletteTypesSortedByName());
    if ( ! forceUpdate) {
        /*
         * Any changes?
         */
        if (palettes == m_previouslyLoadedPalettes) {
            return;
        }
    }
    
    m_paletteBeingEditedName.clear();

    switch (m_widgetType) {
        case WidgetType::COMBO_BOX:
            CaretAssert(m_paletteComboBox);
            m_paletteBeingEditedName = m_paletteComboBox->currentText();
            m_paletteComboBox->clear();
            break;
        case WidgetType::LIST_WIDGET:
            CaretAssert(m_paletteListWidget);
            const QList<QListWidgetItem*> previousItemsList(m_paletteListWidget->selectedItems());
            if ( ! previousItemsList.isEmpty()) {
                m_paletteBeingEditedName = previousItemsList.at(0)->text();
            }
            m_paletteListWidget->clear();
            break;
    }
    
    QListWidgetItem* selectedListWidgetItem(NULL);

    bool firstValidPixmapFlag(true);
    for (const PaletteBase* p : palettes) {
        CaretAssert(p);
        if (std::find(m_paletteDesignTypes.begin(),
                      m_paletteDesignTypes.end(),
                      p->getPaletteDesignType()) != m_paletteDesignTypes.end()) {
            const QPixmap pixmap(createPixmapForPalette(p));
            
            AString paletteName(p->getName());
            switch (p->getPaletteDesignType()) {
                case PaletteDesignTypeEnum::PALETTE:
                    break;
                case PaletteDesignTypeEnum::PALETTE_NEW:
                    /* Causes problems getting selected palette: paletteName.append(" (User)");*/
                    break;
            }
            
            const QVariant paletteKey(QVariant::fromValue(p));
            switch (m_widgetType) {
                case WidgetType::COMBO_BOX:
                {
                    if (pixmap.isNull()) {
                        m_paletteComboBox->addItem(paletteName,
                                                   paletteKey);
                    }
                    else {
                        if (firstValidPixmapFlag) {
                            firstValidPixmapFlag = false;
                            m_paletteComboBox->setIconSize(pixmap.size());
                        }
                        m_paletteComboBox->addItem(pixmap,
                                                   paletteName,
                                                   paletteKey);
                    }
                }
                    break;
                case WidgetType::LIST_WIDGET:
                {
                    QListWidgetItem* item = new QListWidgetItem();
                    item->setText(paletteName);
                    if ( ! pixmap.isNull()) {
                        if (firstValidPixmapFlag) {
                            firstValidPixmapFlag = false;
                            m_paletteListWidget->setIconSize(pixmap.size());
                        }
                        item->setIcon(pixmap);
                    }
                    item->setData(Qt::UserRole, paletteKey);
                    m_paletteListWidget->addItem(item);
                    
                    if (p->getName() == m_paletteBeingEditedName) {
                        selectedListWidgetItem = item;
                    }
                }
                    break;
            }
        }
    }
    
    switch (m_widgetType) {
        case WidgetType::COMBO_BOX:
            if ( ! m_paletteBeingEditedName.isEmpty()) {
                m_paletteComboBox->setCurrentText(m_paletteBeingEditedName);
            }
            break;
        case WidgetType::LIST_WIDGET:
        {
            if (selectedListWidgetItem == NULL) {
                if (m_paletteListWidget->count() > 0) {
                    const int32_t row(0);
                    selectedListWidgetItem = m_paletteListWidget->item(row);
                }
            }
            if (selectedListWidgetItem != NULL) {
                m_paletteListWidget->setCurrentItem(selectedListWidgetItem);
                m_paletteListWidget->scrollToItem(selectedListWidgetItem,
                                                  QAbstractItemView::EnsureVisible);
            }
        }
            break;
    }

    
    m_previouslyLoadedPalettes = palettes;
}

/**
 * Create a pixmap showing the palette's color
 * @param paletteBase
 *   The palette
 * @return The pixmap.  Check ".isNull()" to test for failure.
 */
QPixmap
PaletteSelectionWidget::createPixmapForPalette(const PaletteBase* paletteBase)
{
    QPixmap pixmap;
    
    const Palette* origPalette(paletteBase->castToPalette());
    const PaletteNew* newPalette(paletteBase->castToPaletteNew());
    if (origPalette != NULL) {
        PalettePixmapPainter palettePainter(origPalette,
                                            PalettePixmapPainter::Mode::INTERPOLATE_ON);
        pixmap = palettePainter.getPixmap();
    }
    else if (newPalette != NULL) {
        PalettePixmapPainter palettePainter(newPalette,
                                            PalettePixmapPainter::Mode::INTERPOLATE_ON);
        pixmap = palettePainter.getPixmap();
    }

    return pixmap;
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
PaletteSelectionWidget::receiveEvent(Event* /*event*/)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}


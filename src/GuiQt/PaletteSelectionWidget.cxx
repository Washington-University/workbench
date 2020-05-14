
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

#define __PALETTE_SELECTION_WIDGET_DECLARE__
#include "PaletteSelectionWidget.h"
#undef __PALETTE_SELECTION_WIDGET_DECLARE__

#include <QComboBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QVBoxLayout>


#include "CaretAssert.h"
#include "EventManager.h"
#include "EventPaletteGroupsGet.h"
#include "PaletteCreateNewDialog.h"
#include "PaletteGroup.h"
#include "PaletteNew.h"
#include "PalettePixmapPainter.h"

using namespace caret;


/**
 * \class caret::PaletteSelectionWidget 
 * \brief Widget for selection of a palette source and palette from the source
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 * The parent widget
 */
PaletteSelectionWidget::PaletteSelectionWidget(QWidget* parent)
: QWidget(parent)
{
    createUserPalettes();
    
    m_paletteGroupComboBox = new QComboBox();
    QObject::connect(m_paletteGroupComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &PaletteSelectionWidget::paletteGroupComboBoxActivated);
//    m_paletteSourceComboBox->addItem("User Palettes");
    
    m_paletteSelectionListWidget = new QListWidget();
    QObject::connect(m_paletteSelectionListWidget, &QListWidget::itemActivated,
                     this, &PaletteSelectionWidget::paletteListWidgetActivated);
    
//    QSize iconSize(80, 18);
//
//    for (auto& pal : m_userPalettes) {
//        PalettePixmapPainter palettePainter(pal.get(),
//                                            iconSize,
//                                            m_pixmapMode);
//        QPixmap pixmap = palettePainter.getPixmap();
//        const QString name = pal->getName();
//        if (pixmap.isNull()) {
//
//            m_paletteSelectionListWidget->addItem(name);
//        }
//        else {
//
//            m_paletteSelectionListWidget->addItem(new QListWidgetItem(pixmap,
//                                                                          name));
//        }
//    }
//    m_paletteSelectionListWidget->setIconSize(iconSize);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_paletteGroupComboBox);
    layout->addWidget(m_paletteSelectionListWidget);
    
    if (m_paletteSelectionListWidget->count() > 0) {
        m_paletteSelectionListWidget->setCurrentRow(0);
    }
    
    updateContent();
}

/**
 * Destructor.
 */
PaletteSelectionWidget::~PaletteSelectionWidget()
{
}

/**
 * Update the content of the selection widget
 */
void
PaletteSelectionWidget::updateContent()
{
    updateGroupSelectionComboBox();
    
    updatePaletteSelectionListWidget();
}

/**
 * Update the group selection combo box
 */
void
PaletteSelectionWidget::updateGroupSelectionComboBox()
{
    /*
     * Find the selected group
     */
    PaletteGroup* selectedGroup(getSelectedPaletteGroup());
    
    m_paletteGroups.clear();
    int32_t defaultIndex(0);
    
    /*
     * Get the valid groups
     */
    EventPaletteGroupsGet paletteEvent;
    EventManager::get()->sendEvent(paletteEvent.getPointer());
    
    /*
     * Load the group combo box
     */
    m_paletteGroupComboBox->clear();
    std::vector<std::weak_ptr<PaletteGroup>> groups = paletteEvent.getPaletteGroups();
    for (auto groupPtr : groups) {
        std::shared_ptr<PaletteGroup> sharedPtr = groupPtr.lock();
        if (sharedPtr) {
            PaletteGroup* pg = sharedPtr.get();
            CaretAssert(pg);
            if (pg == selectedGroup) {
                defaultIndex = m_paletteGroupComboBox->count();
            }
            m_paletteGroupComboBox->addItem(pg->getGroupName());
            m_paletteGroups.push_back(groupPtr);
        }
    }
    
    if (defaultIndex < m_paletteGroupComboBox->count()) {
        m_paletteGroupComboBox->setCurrentIndex(defaultIndex);
    }
}

/**
 * Update the palette selection list widget
 */
void
PaletteSelectionWidget::updatePaletteSelectionListWidget()
{
    /*
     * Get name of previously selected palette
     */
    QString selectedPaletteName;
    QListWidgetItem* selectedItem = m_paletteSelectionListWidget->currentItem();
    if (selectedItem != NULL) {
        selectedPaletteName = selectedItem->text();
    }
    
    m_paletteSelectionListWidget->clear();
    
    const PaletteGroup* paletteGroup = getSelectedPaletteGroup();
    if (paletteGroup != NULL) {
        std::vector<PaletteNew> palettes;
        paletteGroup->getPalettes(palettes);
        
        int32_t defaultIndex(0);
        for (auto p : palettes) {
            const QString paletteName = p.getName();
            if (selectedPaletteName == paletteName) {
                defaultIndex = m_paletteSelectionListWidget->count();
            }
//            m_paletteSelectionListWidget->addItem(paletteName);
            
            QSize iconSize(80, 18);
            
            PalettePixmapPainter palettePainter(&p,
                                                iconSize,
                                                PalettePixmapPainter::Mode::INTERPOLATE_ON);
            QPixmap pixmap = palettePainter.getPixmap();
            const QString name = p.getName();
            if (pixmap.isNull()) {
                
                m_paletteSelectionListWidget->addItem(name);
            }
            else {
                
                m_paletteSelectionListWidget->addItem(new QListWidgetItem(pixmap,
                                                                          name));
            }
            m_paletteSelectionListWidget->setIconSize(iconSize);
        }
        
        if (defaultIndex < m_paletteSelectionListWidget->count()) {
            m_paletteSelectionListWidget->setCurrentRow(defaultIndex);
        }
    }
}

/**
 * @return Pointer to the selected palette group
 */
PaletteGroup*
PaletteSelectionWidget::getSelectedPaletteGroup() const
{
    PaletteGroup* paletteGroup(NULL);
    
    const int32_t groupIndex = m_paletteGroupComboBox->currentIndex();
    if ((groupIndex >= 0)
        && (groupIndex < m_paletteGroupComboBox->count())) {
        std::shared_ptr<PaletteGroup> sharedPtr = m_paletteGroups[groupIndex].lock();
        if (sharedPtr) {
            paletteGroup = sharedPtr.get();
        }
    }

    
    return paletteGroup;
}

/**
 * @return Palette selected or NULL if no palette selected
 */
std::unique_ptr<PaletteNew>
PaletteSelectionWidget::getSelectedPalette() const
{
    PaletteGroup* selectedGroup = getSelectedPaletteGroup();
    if (selectedGroup == NULL) {
        return NULL;
    }
    
    QString selectedPaletteName;
    QListWidgetItem* selectedItem = m_paletteSelectionListWidget->currentItem();
    if (selectedItem != NULL) {
        selectedPaletteName = selectedItem->text();
    }
    
    std::unique_ptr<PaletteNew> palette = selectedGroup->getPaletteWithName(selectedPaletteName);
    
//    const int32_t paletteIndex = m_paletteSelectionListWidget->currentRow();
//    if ((paletteIndex >= 0)
//        && (paletteIndex < m_paletteSelectionListWidget->count())) {
//        CaretAssertVectorIndex(m_userPalettes, paletteIndex);
//        paletteOut = m_userPalettes[paletteIndex].get();
//    }
    
    return palette;
}

void
PaletteSelectionWidget::paletteGroupComboBoxActivated(int /*index*/)
{
    updatePaletteSelectionListWidget();
    emit paletteSelectionChanged();
}

/**
 * Called when a user palette is selected
 * @param item
 *     Item selected by the user
 */
void
PaletteSelectionWidget::paletteListWidgetActivated(QListWidgetItem* /*item*/)
{
    emit paletteSelectionChanged();
}

/**
 * Create example palettes for testing
 */
void
PaletteSelectionWidget::createUserPalettes()
{
//    m_userPalettes.clear();
//
//    std::unique_ptr<PaletteNew> pal55(PaletteCreateNewDialog::createPaletteNew("Pal55", 5, 5));
//    std::unique_ptr<PaletteNew> pal34(PaletteCreateNewDialog::createPaletteNew("Pal34", 3, 4));
//
//    m_userPalettes.push_back(std::move(pal55));
//    m_userPalettes.push_back(std::move(pal34));
}


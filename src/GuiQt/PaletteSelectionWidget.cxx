
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
#include "PaletteCreateNewDialog.h"
#include "PaletteNew.h"

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
 * @param pixmapMode
 * Pixmap mode for painting color bars from palettes
 */
PaletteSelectionWidget::PaletteSelectionWidget(const PalettePixmapPainter::Mode pixmapMode,
                                               QWidget* parent)
: QWidget(parent),
m_pixmapMode(pixmapMode)
{
    createUserPalettes();
    
    m_paletteSourceComboBox = new QComboBox();
    m_paletteSourceComboBox->addItem("User Palettes");
    
    m_userPaletteSelectionListWidget = new QListWidget();
    QObject::connect(m_userPaletteSelectionListWidget, &QListWidget::itemActivated,
                     this, &PaletteSelectionWidget::userPaletteListWidgetActivated);
    
    QSize iconSize(80, 18);
    
    for (auto& pal : m_userPalettes) {
        PalettePixmapPainter palettePainter(pal.get(),
                                            iconSize,
                                            m_pixmapMode);
        QPixmap pixmap = palettePainter.getPixmap();
        const QString name = pal->getName();
        if (pixmap.isNull()) {

            m_userPaletteSelectionListWidget->addItem(name);
        }
        else {
            
            m_userPaletteSelectionListWidget->addItem(new QListWidgetItem(pixmap,
                                                                          name));
        }
    }
    m_userPaletteSelectionListWidget->setIconSize(iconSize);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_paletteSourceComboBox);
    layout->addWidget(m_userPaletteSelectionListWidget);
    
    if (m_userPaletteSelectionListWidget->count() > 0) {
        m_userPaletteSelectionListWidget->setCurrentRow(0);
    }
}

/**
 * Destructor.
 */
PaletteSelectionWidget::~PaletteSelectionWidget()
{
}

/**
 * @return Palette selected or NULL if no palette selected
 */
const PaletteNew*
PaletteSelectionWidget::getSelectedPalette() const
{
    PaletteNew* paletteOut(NULL);
    
    const int32_t paletteIndex = m_userPaletteSelectionListWidget->currentRow();
    if ((paletteIndex >= 0)
        && (paletteIndex < m_userPaletteSelectionListWidget->count())) {
        CaretAssertVectorIndex(m_userPalettes, paletteIndex);
        paletteOut = m_userPalettes[paletteIndex].get();
    }
    
    return paletteOut;
}


/**
 * Called when a user palette is selected
 * @param item
 *     Item selected by the user
 */
void
PaletteSelectionWidget::userPaletteListWidgetActivated(QListWidgetItem* /*item*/)
{
    const PaletteNew* palette = getSelectedPalette();
    emit paletteSelected(palette);
}

/**
 * Create example palettes for testing
 */
void
PaletteSelectionWidget::createUserPalettes()
{
    m_userPalettes.clear();
    
    std::unique_ptr<PaletteNew> pal55(PaletteCreateNewDialog::createPaletteNew("Pal55", 5, 5));
    std::unique_ptr<PaletteNew> pal34(PaletteCreateNewDialog::createPaletteNew("Pal34", 3, 4));
    
    m_userPalettes.push_back(std::move(pal55));
    m_userPalettes.push_back(std::move(pal34));
}


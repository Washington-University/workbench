
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __MAP_SETTINGS_COLOR_BAR_PALETTE_OPTIONS_WIDGET_DECLARE__
#include "MapSettingsColorBarPaletteOptionsWidget.h"
#undef __MAP_SETTINGS_COLOR_BAR_PALETTE_OPTIONS_WIDGET_DECLARE__

#include <QAction>
#include <QCheckBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "CopyPaletteColorMappingToFilesDialog.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;



/**
 * \class caret::MapSettingsColorBarPaletteOptionsWidget
 * \brief Widget for apply to all maps checkbox and apply to files button for color bar and palette
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *    The parent widget.
 */
MapSettingsColorBarPaletteOptionsWidget::MapSettingsColorBarPaletteOptionsWidget(QWidget* parent)
: QGroupBox("Color Bar and Palette Settings",
            parent)
{
    m_applyToAllMapsCheckBox = new QCheckBox("Apply to All Maps");
    QObject::connect(m_applyToAllMapsCheckBox, &QCheckBox::clicked,
                     this, &MapSettingsColorBarPaletteOptionsWidget::applyToAllMapsCheckBoxClicked);
    m_applyToAllMapsCheckBox->setToolTip("When checked, these color bar and palette settings\n"
                                         "are applied to all maps in the file.\n"
                                         "When disabled, the file uses one palette for all maps.");
    
    m_applyToAllFilesAction = new QAction(this);
    m_applyToAllFilesAction->setText("Apply to Files...");
    m_applyToAllFilesAction->setToolTip("Displays a dialog that allows selection of data\n"
                                        "files to which the palette settings are applied");
    QObject::connect(m_applyToAllFilesAction, &QAction::triggered,
                     this, &MapSettingsColorBarPaletteOptionsWidget::applyToAllFilesToolButtonClicked);
    
    QToolButton* applyToFilesToolButton = new QToolButton();
    applyToFilesToolButton->setDefaultAction(m_applyToAllFilesAction);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_applyToAllMapsCheckBox);
    layout->addWidget(applyToFilesToolButton);
    setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                              QSizePolicy::Fixed));
}

/**
 * Destructor.
 */
MapSettingsColorBarPaletteOptionsWidget::~MapSettingsColorBarPaletteOptionsWidget()
{
}

/**
 * Update contents for editing a map settings for data in a caret
 * mappable data file.
 *
 * @param caretMappableDataFile
 *    Data file containing palette that is edited.
 * @param mapIndex
 *    Index of map for palette that is edited.
 */
void
MapSettingsColorBarPaletteOptionsWidget::updateEditor(CaretMappableDataFile* caretMappableDataFile,
                                                      const int32_t mapIndex)
{
    m_mapFile = caretMappableDataFile;
    m_mapFileIndex = mapIndex;
    
    if ((m_mapFile != NULL)
        && (m_mapFileIndex >= 0)) {
  
        if (m_mapFile->isOnePaletteUsedForAllMaps()) {
            m_applyToAllMapsCheckBox->setEnabled(false);
            m_applyToAllMapsCheckBox->setChecked(false);
        }
        else {
            m_applyToAllMapsCheckBox->setEnabled(true);
            m_applyToAllMapsCheckBox->setChecked(m_mapFile->isApplyPaletteColorMappingToAllMaps());
        }

        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
}

/**
 * Called by users of this widget to apply the selected options.
 */
void
MapSettingsColorBarPaletteOptionsWidget::applyOptions()
{
    if (m_mapFile != NULL) {
        if (m_mapFile->isApplyPaletteColorMappingToAllMaps()) {
            m_mapFile->applyPaletteColorMappingToAllMaps(m_mapFileIndex);
            m_mapFile->updateScalarColoringForAllMaps();
        }
    }

    updateColoringAndGraphics();
}

/**
 * Called when the "Apply to All Maps" checkbox is clicked
 *
 * @param checked
 *     New status of button.
 */
void
MapSettingsColorBarPaletteOptionsWidget::applyToAllMapsCheckBoxClicked(bool checked)
{
    if (m_mapFile != NULL) {
        m_mapFile->setApplyPaletteColorMappingToAllMaps(checked);
    }
    
    applyOptions();
}

/**
 * Called when the Apply to Files tool button is clicked
 */
void
MapSettingsColorBarPaletteOptionsWidget::applyToAllFilesToolButtonClicked()
{
    if (m_mapFile != NULL) {
        AString errorMessage;
        if ( ! CopyPaletteColorMappingToFilesDialog::run(m_mapFile,
                                                         m_mapFile->getMapPaletteColorMapping(m_mapFileIndex),
                                                         this,
                                                         errorMessage)) {
            if ( ! errorMessage.isEmpty()) {
                WuQMessageBox::errorOk(this, errorMessage);
            }
        }
        
        updateColoringAndGraphics();
    }
}

/**
 * Update coloring and graphics
 */
void
MapSettingsColorBarPaletteOptionsWidget::updateColoringAndGraphics()
{
    if (m_applyToAllMapsCheckBox->isChecked()) {
        m_mapFile->updateScalarColoringForAllMaps();
    }
    else {
        m_mapFile->updateScalarColoringForMap(m_mapFileIndex);
    }
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}



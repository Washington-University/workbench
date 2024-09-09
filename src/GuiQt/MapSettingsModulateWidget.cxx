
/*LICENSE_START*/
/*
 *  Copyright (C) 2024 Washington University School of Medicine
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

#define __MAP_SETTINGS_MODULATE_WIDGET_DECLARE__
#include "MapSettingsModulateWidget.h"
#undef __MAP_SETTINGS_MODULATE_WIDGET_DECLARE__

#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "CaretMappableDataFileAndMapSelectorObject.h"
#include "DataFileColorModulateSelector.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"
#include "WuQtUtilities.h"
using namespace caret;


    
/**
 * \class caret::MapSettingsModulateWidget 
 * \brief Widget for selection of modulate color file in map settings dialog
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    The parent widget
 */
MapSettingsModulateWidget::MapSettingsModulateWidget(QWidget* parent)
: QWidget(parent)
{
    m_enabledCheckBox = new QCheckBox("Enable Color Modulation");
    QObject::connect(m_enabledCheckBox, &QCheckBox::clicked,
                     this, &MapSettingsModulateWidget::modulateEnabledCheckBoxClicked);
    
    m_modulateMapFileIndexSelector = new CaretMappableDataFileAndMapSelectorObject(CaretMappableDataFileAndMapSelectorObject::OPTION_SHOW_MAP_INDEX_SPIN_BOX,
                                                                                   this);
    QObject::connect(m_modulateMapFileIndexSelector, &CaretMappableDataFileAndMapSelectorObject::selectionWasPerformed,
                     this, &MapSettingsModulateWidget::modulateMapFileIndexSelectorChanged);
    QWidget* modulateFileComboBox(0);
    QWidget* modulateMapIndexSpinBox(0);
    QWidget* modulateMapNameComboBox(0);
    m_modulateMapFileIndexSelector->getWidgetsForAddingToLayout(modulateFileComboBox,
                                                                modulateMapIndexSpinBox,
                                                                modulateMapNameComboBox);
    
    const AString ttText(" for modulation.  This modulation file is used to 'modulate' the coloring.  "
                         "Values in the modulation file should range [0.0, 1.0] and the modulation files "
                         "available for selection are the same dimensions as the file to which "
                         "modulation is applied.  The RGB values of the file being displayed are "
                         "multiplied by the corresponding (same dimension index) value from the "
                         "modulation file.");
    WuQtUtilities::setWordWrappedToolTip(modulateFileComboBox,
                                         "Select file" + ttText);
    WuQtUtilities::setWordWrappedToolTip(modulateMapIndexSpinBox,
                                         "Select map" + ttText);
    WuQtUtilities::setWordWrappedToolTip(modulateMapNameComboBox,
                                         "Select map" + ttText);
    
    QGridLayout* modulateFileLayout = new QGridLayout(this);
    QLabel* modulateFileLabel = new QLabel("File");
    QLabel* modulateMapLabel  = new QLabel("Map");
    modulateFileLayout->setColumnStretch(0, 0);
    modulateFileLayout->setColumnStretch(1, 0);
    modulateFileLayout->setColumnStretch(2, 100);
    int32_t rowIndex(0);
    modulateFileLayout->addWidget(m_enabledCheckBox, rowIndex, 0, 1, 3, Qt::AlignLeft);
    ++rowIndex;
    modulateFileLayout->addWidget(modulateFileLabel, rowIndex, 0);
    modulateFileLayout->addWidget(modulateFileComboBox, rowIndex, 1, 1, 2);
    ++rowIndex;
    modulateFileLayout->addWidget(modulateMapLabel, rowIndex, 0);
    modulateFileLayout->addWidget(modulateMapIndexSpinBox, rowIndex, 1);
    modulateFileLayout->addWidget(modulateMapNameComboBox, rowIndex, 2);
    ++rowIndex;
    
    modulateFileLayout->setRowStretch(100, 100);
    modulateFileLayout->setColumnStretch(100, 100);
    
    setSizePolicy(sizePolicy().horizontalPolicy(),
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsModulateWidget::~MapSettingsModulateWidget()
{
}

/**
 * Update the widgets
 * @param caretMappableDataFile
 *    File that is in editor
 * @param mapIndex
 *    Index of map selected in file
 */
void
MapSettingsModulateWidget::updateEditor(CaretMappableDataFile* caretMappableDataFile,
                                        const int32_t mapIndex)
{
    m_caretMappableDataFile = caretMappableDataFile;
    m_mapIndex              = mapIndex;
    
    DataFileColorModulateSelector* selector(getSelector());
    if (selector != NULL) {
        m_modulateMapFileIndexSelector->updateFileAndMapSelector(selector->getSelectionModel());
        m_enabledCheckBox->setChecked(selector->isEnabled());
        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
}

/**
 * Gets called when the threshold map file/index is changed.
 */
void
MapSettingsModulateWidget::modulateMapFileIndexSelectorChanged()
{
    DataFileColorModulateSelector* selector(getSelector());
    if (selector != NULL) {
        m_caretMappableDataFile->updateScalarColoringForMap(m_mapIndex);
        EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
}

/**
 * Gets called when modulate checkbox is checked/unchecked
 * @param checked
 *    New checked status
 */
void
MapSettingsModulateWidget::modulateEnabledCheckBoxClicked(bool checked)
{
    DataFileColorModulateSelector* selector(getSelector());
    if (selector != NULL) {
        selector->setEnabled(checked);
    }
    modulateMapFileIndexSelectorChanged();
}

/**
 * @return The active color modulate selector or NULL if not available
 */
DataFileColorModulateSelector*
MapSettingsModulateWidget::getSelector()
{
    if (m_caretMappableDataFile != NULL) {
        if ((m_mapIndex >= 0)
            && m_mapIndex < m_caretMappableDataFile->getNumberOfMaps()) {
            DataFileColorModulateSelector* selector(m_caretMappableDataFile->getMapColorModulateFileSelector(m_mapIndex));
            return selector;
        }
    }
    return NULL;
}
    

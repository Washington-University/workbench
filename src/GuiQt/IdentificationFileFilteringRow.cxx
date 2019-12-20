
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

#define __IDENTIFICATION_FILE_FILTERING_ROW_DECLARE__
#include "IdentificationFileFilteringRow.h"
#undef __IDENTIFICATION_FILE_FILTERING_ROW_DECLARE__

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>

#include "CaretAssert.h"
#include "CaretMappableDataFile.h"
#include "EnumComboBoxTemplate.h"
#include "FileIdentificationAttributes.h"
#include "FileIdentificationMapSelectionEnum.h"
#include "WuQValueChangedSignalWatcher.h"
#include "WuQWidgetObjectGroup.h"

using namespace caret;


    
/**
 * \class caret::IdentificationFileFilteringRow 
 * \brief Contains a row for file filtering selections
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param gridLayout
 * Layout for widgets
 */
IdentificationFileFilteringRow::IdentificationFileFilteringRow(QGridLayout *gridLayout)
: QObject()
{
    m_enabledCheckBox = new QCheckBox();
    
    m_displayedLabel  = new QLabel("          ");
    m_displayedLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    m_fileNameLabel = new QLabel();
    
    m_mapModeComboBox = new EnumComboBoxTemplate(this);
    m_mapModeComboBox->setup<FileIdentificationMapSelectionEnum, FileIdentificationMapSelectionEnum::Enum>();
    
    m_mapNameComboBox = new QComboBox();
    
    m_signalWatcher = new WuQValueChangedSignalWatcher(this);
    QObject::connect(m_signalWatcher, &WuQValueChangedSignalWatcher::valueChanged,
                     this, &IdentificationFileFilteringRow::widgetValueChanged);
    m_signalWatcher->addObject(m_enabledCheckBox);
    m_signalWatcher->addObject(m_mapModeComboBox->getComboBox());
    m_signalWatcher->addObject(m_mapNameComboBox);

    m_objectGroup = new WuQWidgetObjectGroup(this);
    m_objectGroup->add(m_enabledCheckBox);
    m_objectGroup->add(m_displayedLabel);
    m_objectGroup->add(m_fileNameLabel);
    m_objectGroup->add(m_mapModeComboBox->getWidget());
    m_objectGroup->add(m_mapNameComboBox);
    
    const int row = gridLayout->rowCount();
    gridLayout->addWidget(m_enabledCheckBox,
                          row, COLUMN_ENABLED_CHECKBOX);
    gridLayout->addWidget(m_displayedLabel,
                          row, COLUMN_DISPLAYED_LABEL);
    gridLayout->addWidget(m_fileNameLabel,
                          row, COLUMN_FILE_NAME_LABEL);
    gridLayout->addWidget(m_mapModeComboBox->getWidget(),
                          row, COLUMN_MAP_MODE_COMBO_BOX);
    gridLayout->addWidget(m_mapNameComboBox,
                          row, COLUMN_MAP_NAME_COMBO_BOX);
}

/**
 * Destructor.
 */
IdentificationFileFilteringRow::~IdentificationFileFilteringRow()
{
}

/**
 * Add column titles to the given layout
 * @param gridLayout
 *     Layout receiving tiltles
 */
void
IdentificationFileFilteringRow::addGridLayoutColumnTitles(QGridLayout* gridLayout)
{
    const int row(0);
    gridLayout->addWidget(new QLabel("Show"),
                          row, COLUMN_ENABLED_CHECKBOX);
    gridLayout->addWidget(new QLabel("Displayed"),
                          row, COLUMN_DISPLAYED_LABEL);
    gridLayout->addWidget(new QLabel("File Name"),
                          row, COLUMN_FILE_NAME_LABEL);
    gridLayout->addWidget(new QLabel("Map Mode"),
                          row, COLUMN_MAP_MODE_COMBO_BOX);
    gridLayout->addWidget(new QLabel("Selected Map Name"),
                          row, COLUMN_MAP_NAME_COMBO_BOX);
}


/**
 * Update with the given map file
 * @param mapFile
 * The map file
 */
void
IdentificationFileFilteringRow::updateContent(CaretMappableDataFile* mapFile,
                                              const bool fileDisplayedFlag)
{
    m_mapFile = mapFile;
    
    m_objectGroup->setVisible(m_mapFile != NULL);
    
    if (m_mapFile != NULL) {
        const FileIdentificationAttributes* atts = m_mapFile->getFileIdentificationAttributes();
        CaretAssert(atts);
        
        m_enabledCheckBox->setChecked(atts->isEnabled());
        if (fileDisplayedFlag) {
            m_displayedLabel->setText("YES");
        }
        else {
            m_displayedLabel->setText("   ");
        }
        m_fileNameLabel->setText(m_mapFile->getFileNameNoPath());
        m_mapModeComboBox->setSelectedItem<FileIdentificationMapSelectionEnum, FileIdentificationMapSelectionEnum::Enum>(atts->getMapSelectionMode());
        
        m_mapNameComboBox->clear();
        const int32_t numMaps = m_mapFile->getNumberOfMaps();
        for (int32_t i = 0; i < numMaps; i++) {
            m_mapNameComboBox->addItem(m_mapFile->getMapName(i));
        }
        m_mapNameComboBox->setCurrentIndex(atts->getMapIndex());
    }
}

/**
 * Called when a wldget is changed by the user
 */
void
IdentificationFileFilteringRow::widgetValueChanged()
{
    if (m_mapFile != NULL) {
        FileIdentificationAttributes* atts = m_mapFile->getFileIdentificationAttributes();
        atts->setEnabled(m_enabledCheckBox->isChecked());
        atts->setMapSelectionMode(m_mapModeComboBox->getSelectedItem<FileIdentificationMapSelectionEnum, FileIdentificationMapSelectionEnum::Enum>());
        atts->setMapIndex(m_mapNameComboBox->currentIndex());
    }
}


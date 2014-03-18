
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __MAP_SETTINGS_PARCELS_WIDGET_DECLARE__
#include "MapSettingsParcelsWidget.h"
#undef __MAP_SETTINGS_PARCELS_WIDGET_DECLARE__

#include <QGridLayout>
#include <QLabel>

#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "CiftiMappableConnectivityMatrixDataFile.h"
#include "CiftiParcelColoringModeEnum.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventSurfaceColoringInvalidate.h"

using namespace caret;

/**
 * \class caret::MapSettingsParcelsWidget 
 * \brief Widget for setting parcel coloring properties.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
MapSettingsParcelsWidget::MapSettingsParcelsWidget(QWidget* parent)
: QWidget(parent)
{
    m_ciftiMatrixFile = NULL;
    
    QLabel* colorModeLabel = new QLabel("Color Mode");
    m_parcelColoringModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_parcelColoringModeEnumComboBox->setup<CiftiParcelColoringModeEnum,CiftiParcelColoringModeEnum::Enum>();
    QObject::connect(m_parcelColoringModeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(ciftiParcelColoringModeEnumComboBoxItemActivated()));
    
    QLabel* colorLabel = new QLabel("Color");
    m_parcelColorEnumComboBox = new CaretColorEnumComboBox(this);
    QObject::connect(m_parcelColorEnumComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(parcelColorSelected(const CaretColorEnum::Enum)));
    
    QWidget* gridWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(gridWidget);
    gridLayout->addWidget(colorModeLabel,
                      0, 0);
    gridLayout->addWidget(m_parcelColoringModeEnumComboBox->getWidget(),
                      0, 1);
    gridLayout->addWidget(colorLabel,
                      1, 0);
    gridLayout->addWidget(m_parcelColorEnumComboBox->getWidget(),
                      1, 1);
    gridWidget->setFixedSize(gridWidget->sizeHint());
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(gridWidget, 0, Qt::AlignLeft);
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
MapSettingsParcelsWidget::~MapSettingsParcelsWidget()
{
}

/**
 * Update the editor with the given CIFTI matrix file.
 */
void
MapSettingsParcelsWidget::updateEditor(CiftiMappableConnectivityMatrixDataFile* ciftiMatrixFile)
{
    CaretAssert(ciftiMatrixFile);
    m_ciftiMatrixFile = ciftiMatrixFile;
    m_parcelColoringModeEnumComboBox->setSelectedItem<CiftiParcelColoringModeEnum,CiftiParcelColoringModeEnum::Enum>(m_ciftiMatrixFile->getSelectedParcelColoringMode());
    m_parcelColorEnumComboBox->setSelectedColor(m_ciftiMatrixFile->getSelectedParcelColor());
}

/**
 * Update the widget.
 */
void
MapSettingsParcelsWidget::updateWidget()
{
    updateEditor(m_ciftiMatrixFile);
}

/**
 * Called when parcel color combo box is changed.
 */
void
MapSettingsParcelsWidget::parcelColorSelected(const CaretColorEnum::Enum color)
{
    m_ciftiMatrixFile->setSelectedParcelColor(color);
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when parcel coloring mode combo box is changed.
 */
void
MapSettingsParcelsWidget::ciftiParcelColoringModeEnumComboBoxItemActivated()
{
    const CiftiParcelColoringModeEnum::Enum colorMode = m_parcelColoringModeEnumComboBox->getSelectedItem<CiftiParcelColoringModeEnum,CiftiParcelColoringModeEnum::Enum>();
    m_ciftiMatrixFile->setSelectedParcelColoringMode(colorMode);
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

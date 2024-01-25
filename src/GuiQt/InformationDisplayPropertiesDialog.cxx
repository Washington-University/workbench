
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

#define __INFORMATION_DISPLAY_OPTIONS_DIALOG_DECLARE__
#include "InformationDisplayPropertiesDialog.h"
#undef __INFORMATION_DISPLAY_OPTIONS_DIALOG_DECLARE__

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QLabel>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "IdentificationManager.h"
#include "WuQFactory.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::InformationDisplayPropertiesDialog 
 * \brief Options for information dialog display.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
InformationDisplayPropertiesDialog::InformationDisplayPropertiesDialog(QWidget* parent,
                                                                 Qt::WindowFlags f)
: WuQDialogNonModal("Information Properties",
                    parent,
                    f)
{
    const int WIDGET_WIDTH = 100;

    QLabel* surfaceLabel = new QLabel("Show Surface ID Symbols");
    m_surfaceIdentificationSymbolComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_surfaceIdentificationSymbolComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(informationPropertyChanged()));
    
    QLabel* volumeLabel = new QLabel("Show Volume ID Symbols");
    m_volumeIdentificationSymbolComboBox = new WuQTrueFalseComboBox("On", "Off", this);
    QObject::connect(m_volumeIdentificationSymbolComboBox, SIGNAL(statusChanged(bool)),
                     this, SLOT(informationPropertyChanged()));
    
    QLabel* idColorLabel = new QLabel("ID Symbol Color: ");
    m_idColorComboBox = new CaretColorEnumComboBox(this);
    m_idColorComboBox->getWidget()->setFixedWidth(WIDGET_WIDTH);
    QObject::connect(m_idColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(informationPropertyChanged()));
    
    QLabel* idContralateralLabel = new QLabel("ID Contralateral Symbol Color: ");
    m_idContralateralColorComboBox = new CaretColorEnumComboBox(this);
    m_idContralateralColorComboBox->getWidget()->setFixedWidth(WIDGET_WIDTH);
    QObject::connect(m_idContralateralColorComboBox, SIGNAL(colorSelected(const CaretColorEnum::Enum)),
                     this, SLOT(informationPropertyChanged()));
    
    QLabel* symbolSizeLabel = new QLabel("Symbol Diameter: ");
    m_symbolSizeSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.1,
                                                                                         10000.0,
                                                                                         0.1, 1,
                                                                                         this,
                                                                                         SLOT(informationPropertyChanged()));
    m_symbolSizeSpinBox->setFixedWidth(WIDGET_WIDTH);
    m_symbolSizeSpinBox->setSuffix("mm");
    QObject::connect(m_symbolSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(informationPropertyChanged()));
    
    
    
    QLabel* mostRecentSymbolSizeLabel = new QLabel("Most Recent ID Symbol Diameter: ");
    m_mostRecentSymbolSizeSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(0.1,
                                                                                         10000.0,
                                                                                         0.1, 1,
                                                                                         this,
                                                                                         SLOT(informationPropertyChanged()));
    m_mostRecentSymbolSizeSpinBox->setFixedWidth(WIDGET_WIDTH);
    m_mostRecentSymbolSizeSpinBox->setSuffix("mm");
    QObject::connect(m_mostRecentSymbolSizeSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(informationPropertyChanged()));
    
    const int COLUMN_LABEL  = 0;
    const int COLUMN_WIDGET = 1;
    QWidget* widget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(widget);
    int row = 0;
    gridLayout->addWidget(surfaceLabel, row, COLUMN_LABEL);
    gridLayout->addWidget(m_surfaceIdentificationSymbolComboBox->getWidget(), row, COLUMN_WIDGET);
    row++;
    gridLayout->addWidget(volumeLabel, row, COLUMN_LABEL);
    gridLayout->addWidget(m_volumeIdentificationSymbolComboBox->getWidget(), row, COLUMN_WIDGET);
    row++;

    
    gridLayout->addWidget(idColorLabel, row, COLUMN_LABEL);
    gridLayout->addWidget(m_idColorComboBox->getWidget(), row, COLUMN_WIDGET);
    row++;
    gridLayout->addWidget(idContralateralLabel, row, COLUMN_LABEL);
    gridLayout->addWidget(m_idContralateralColorComboBox->getWidget(), row, COLUMN_WIDGET);
    row++;
    gridLayout->addWidget(symbolSizeLabel, row, COLUMN_LABEL);
    gridLayout->addWidget(m_symbolSizeSpinBox, row, COLUMN_WIDGET);
    row++;
    gridLayout->addWidget(mostRecentSymbolSizeLabel, row, COLUMN_LABEL);
    gridLayout->addWidget(m_mostRecentSymbolSizeSpinBox, row, COLUMN_WIDGET);
    row++;
    gridLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, COLUMN_LABEL, 1, 2);
    row++;
    
    setCentralWidget(widget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    setApplyButtonText("");
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    updateDialog();
    
    setSaveWindowPositionForNextTime(true);
}

/**
 * Destructor.
 */
InformationDisplayPropertiesDialog::~InformationDisplayPropertiesDialog()
{
}

/** 
 * update its content 
 */
void
InformationDisplayPropertiesDialog::updateDialog()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();

    m_surfaceIdentificationSymbolComboBox->setStatus(info->isShowSurfaceIdentificationSymbols());
    m_volumeIdentificationSymbolComboBox->setStatus(info->isShowVolumeIdentificationSymbols());
    m_idColorComboBox->setSelectedColor(info->getIdentificationSymbolColor());
    m_idContralateralColorComboBox->setSelectedColor(info->getIdentificationContralateralSymbolColor());
    m_symbolSizeSpinBox->blockSignals(true);
    m_symbolSizeSpinBox->setValue(info->getIdentificationSymbolSize());
    m_symbolSizeSpinBox->blockSignals(false);
    m_mostRecentSymbolSizeSpinBox->blockSignals(true);
    m_mostRecentSymbolSizeSpinBox->setValue(info->getMostRecentIdentificationSymbolSize());
    m_mostRecentSymbolSizeSpinBox->blockSignals(false);
}

/**
 * Gets called when a property changes.
 */
void
InformationDisplayPropertiesDialog::informationPropertyChanged()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    
    info->setShowSurfaceIdentificationSymbols(m_surfaceIdentificationSymbolComboBox->isTrue());
    info->setShowVolumeIdentificationSymbols(m_volumeIdentificationSymbolComboBox->isTrue());
    info->setIdentificationSymbolColor(m_idColorComboBox->getSelectedColor());
    info->setIdentificationContralateralSymbolColor(m_idContralateralColorComboBox->getSelectedColor());
    info->setIdentificationSymbolSize(m_symbolSizeSpinBox->value());
    info->setMostRecentIdentificationSymbolSize(m_mostRecentSymbolSizeSpinBox->value());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}



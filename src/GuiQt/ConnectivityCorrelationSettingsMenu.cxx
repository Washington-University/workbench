
/*LICENSE_START*/
/*
 *  Copyright (C) 2023 Washington University School of Medicine
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

#define __CONNECTIVITY_CORRELATION_SETTINGS_MENU_DECLARE__
#include "ConnectivityCorrelationSettingsMenu.h"
#undef __CONNECTIVITY_CORRELATION_SETTINGS_MENU_DECLARE__

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>
#include <QWidgetAction>

#include "CaretAssert.h"
#include "ConnectivityCorrelationSettings.h"

using namespace caret;

/**
 * \class caret::ConnectivityCorrelationSettingsMenu 
 * \brief Menu for ConnectivityCorrelationSettings
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *   The parent widget
 */
ConnectivityCorrelationSettingsMenu::ConnectivityCorrelationSettingsMenu(ConnectivityCorrelationSettings* settings,
                                                                         QWidget* parent)
: QMenu(parent),
m_settings(settings)
{
    CaretAssert(m_settings);
    
    m_modeCorrelationRadioButton = new QRadioButton("Correlation");
    
    m_modeCovarianceRadioButton = new QRadioButton("Covariance");
    
    QButtonGroup* modeButtonGroup(new QButtonGroup(this));
    modeButtonGroup->addButton(m_modeCorrelationRadioButton);
    modeButtonGroup->addButton(m_modeCovarianceRadioButton);
    modeButtonGroup->setExclusive(true);
    QObject::connect(modeButtonGroup, static_cast<void(QButtonGroup::*)(QAbstractButton *)>(&QButtonGroup::buttonClicked),
                     this, &ConnectivityCorrelationSettingsMenu::modeButtonClicked);
    
    switch (settings->getMode()) {
        case ConnectivityCorrelationModeEnum::CORRELATION:
            m_modeCorrelationRadioButton->setChecked(true);
            break;
        case ConnectivityCorrelationModeEnum::COVARIANCE:
            m_modeCovarianceRadioButton->setChecked(true);
            break;
    }
    
    m_optionFisherZCheckBox = new QCheckBox("Fisher-Z");
    m_optionFisherZCheckBox->setChecked(settings->isCorrelationFisherZEnabled());
    QObject::connect(m_optionFisherZCheckBox, &QCheckBox::clicked,
                     this, &ConnectivityCorrelationSettingsMenu::optionFisherZCheckBoxClicked);
    
    m_optionNoDemeanCheckBox = new QCheckBox("No Demean");
    m_optionNoDemeanCheckBox->setChecked(settings->isCorrelationNoDemeanEnabled());
    QObject::connect(m_optionNoDemeanCheckBox, &QCheckBox::clicked,
                     this, &ConnectivityCorrelationSettingsMenu::optionNoDemeanCheckBoxClicked);

    QGroupBox* modeGroupBox(new QGroupBox("Mode"));
    QVBoxLayout* modeGroupBoxLayout(new QVBoxLayout(modeGroupBox));
    modeGroupBoxLayout->addWidget(m_modeCorrelationRadioButton);
    modeGroupBoxLayout->addWidget(m_modeCovarianceRadioButton);
    
    QGroupBox* optionsGroupBox(new QGroupBox("Correlation Options"));
    QVBoxLayout* optionsGroupBoxLayout(new QVBoxLayout(optionsGroupBox));
    optionsGroupBoxLayout->addWidget(m_optionFisherZCheckBox);
    optionsGroupBoxLayout->addWidget(m_optionNoDemeanCheckBox);
    
    QWidget* optionsWidget(new QWidget());
    QVBoxLayout* optionsLayout(new QVBoxLayout(optionsWidget));
    optionsLayout->addWidget(modeGroupBox);
    optionsLayout->addWidget(optionsGroupBox);
    
    QWidgetAction* widgetAction(new QWidgetAction(this));
    widgetAction->setDefaultWidget(optionsWidget);
    
    addAction(widgetAction);

    updateMenu();
}

/**
 * Destructor.
 */
ConnectivityCorrelationSettingsMenu::~ConnectivityCorrelationSettingsMenu()
{
}

/**
 * Called when a mode button is selected
 * @param button
 *    Button that was clicked
 */
void
ConnectivityCorrelationSettingsMenu::modeButtonClicked(QAbstractButton *button)
{
    ConnectivityCorrelationModeEnum::Enum mode = ConnectivityCorrelationModeEnum::CORRELATION;
    if (button == m_modeCorrelationRadioButton) {
        mode = ConnectivityCorrelationModeEnum::CORRELATION;
    }
    else if (button == m_modeCovarianceRadioButton) {
        mode = ConnectivityCorrelationModeEnum::COVARIANCE;
    }
    m_settings->setMode(mode);
    updateMenu();
}

/**
 * Called when
 * @param checked
 *    Checked status
 */
void
ConnectivityCorrelationSettingsMenu::optionFisherZCheckBoxClicked(bool checked)
{
    m_settings->setCorrelationFisherZEnabled(checked);
}

/**
 * Called when
 * @param checked
 *    Checked status
 */
void
ConnectivityCorrelationSettingsMenu::optionNoDemeanCheckBoxClicked(bool checked)
{
    m_settings->setCorrelationNoDemeanEnabled(checked);
}

/**
 * Update the menu
 */
void
ConnectivityCorrelationSettingsMenu::updateMenu()
{
    bool enableOptionsFlag(false);
    switch (m_settings->getMode()) {
        case ConnectivityCorrelationModeEnum::CORRELATION:
            enableOptionsFlag = true;
            break;
        case ConnectivityCorrelationModeEnum::COVARIANCE:
            break;
    }
    m_optionFisherZCheckBox->setEnabled(enableOptionsFlag);
    m_optionNoDemeanCheckBox->setEnabled(enableOptionsFlag);
}


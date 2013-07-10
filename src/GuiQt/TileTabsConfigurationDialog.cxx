
/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/

#include <QComboBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>


#define __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
#include "TileTabsConfigurationDialog.h"
#undef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "SessionManager.h"
#include "TileTabsConfiguration.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::TileTabsConfigurationDialog 
 * \brief Edit and create configurations for tile tabs viewing.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
TileTabsConfigurationDialog::TileTabsConfigurationDialog(QWidget* parent)
: WuQDialogNonModal("Tile Tabs Configuration",
                    parent)
{
    
    m_caretPreferences = SessionManager::get()->getCaretPreferences();
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->setSpacing(0);
    dialogLayout->addWidget(createConfigurationSelectionWidget());
    dialogLayout->addWidget(createEditConfigurationWidget(),
                            100,
                            Qt::AlignHCenter);
    
    setCentralWidget(dialogWidget);
    
    disableAutoDefaultForAllPushButtons();

    setApplyButtonText("");
    //    EventManager::get()->addEventListener(this, EventTypeEnum::);
    
}

/**
 * Destructor.
 */
TileTabsConfigurationDialog::~TileTabsConfigurationDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return The configuration selection widget.
 */
QWidget*
TileTabsConfigurationDialog::createConfigurationSelectionWidget()
{
    QLabel* configurationLabel = new QLabel("Configuration");
    m_configurationSelectionComboBox = WuQFactory::newComboBoxSignalInt(this,
                                                                        SLOT(configurationComboBoxItemSelected(int)));
    
    QHBoxLayout* selectionLayout = new QHBoxLayout();
    selectionLayout->addWidget(configurationLabel, 0);
    selectionLayout->addWidget(m_configurationSelectionComboBox, 100);
    
    m_newConfigurationPushButton = new QPushButton("New...");
    QObject::connect(m_newConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(newConfigurationButtonClicked()));
    
    m_deleteConfigurationPushButton = new QPushButton("Delete...");
    QObject::connect(m_deleteConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(deleteConfigurationButtonClicked()));
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(m_newConfigurationPushButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_deleteConfigurationPushButton);
    
    QGroupBox* configurationWidget = new QGroupBox("Configuration Selection");
    QVBoxLayout* configurationLayout = new QVBoxLayout(configurationWidget);
    configurationLayout->setMargin(0);
    configurationLayout->addLayout(selectionLayout);
    configurationLayout->addLayout(buttonsLayout);
//    configurationWidget->setSizePolicy(QSizePolicy::Fixed,
//                                       QSizePolicy::Fixed);
    
    return configurationWidget;
}

/**
 * @return The edit configuration widget.
 */
QWidget*
TileTabsConfigurationDialog::createEditConfigurationWidget()
{
    QLabel* rowsLabel = new QLabel("Number of Rows");
    QLabel* columnsLabel = new QLabel("Number of Columns");
    
    m_numberOfRowsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                          100,
                                                                          1,
                                                                          this,
                                                                          SLOT(numberOfRowsOrColumnsChanged()));

    m_numberOfColumnsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                          100,
                                                                          1,
                                                                          this,
                                                                          SLOT(numberOfRowsOrColumnsChanged()));
    QWidget* numberOfWidget = new QWidget();
    QGridLayout* numberOfGridLayout = new QGridLayout(numberOfWidget);
    numberOfGridLayout->addWidget(rowsLabel,
                                  0, 0);
    numberOfGridLayout->addWidget(m_numberOfRowsSpinBox,
                                  0, 1);
    numberOfGridLayout->addWidget(columnsLabel,
                                  1, 0);
    numberOfGridLayout->addWidget(m_numberOfColumnsSpinBox,
                                  1, 1);
    numberOfWidget->setSizePolicy(QSizePolicy::Fixed,
                                  QSizePolicy::Fixed);

    
    QLabel* stretchFactorLabel = new QLabel("Stretch Factors");
    QLabel* indexLabel = new QLabel("Index");
    QLabel* rowLabel = new QLabel("Row");
    QLabel* columnLabel = new QLabel("Column");
    
    QWidget* stretchFactorWidget = new QWidget();
    QGridLayout* stretchFactorGridLayout = new QGridLayout(stretchFactorWidget);
    int row = 0;
    stretchFactorGridLayout->addWidget(stretchFactorLabel,
                                       row, 0,
                                       1, 3,
                                       Qt::AlignHCenter);
    row++;
    stretchFactorGridLayout->addWidget(indexLabel,
                                       row, GRID_LAYOUT_COLUMN_INDEX_FOR_LABELS);
    stretchFactorGridLayout->addWidget(rowLabel,
                          row, GRID_LAYOUT_COLUMN_INDEX_FOR_ROW_CONTROLS);
    stretchFactorGridLayout->addWidget(columnLabel,
                          row, GRID_LAYOUT_COLUMN_INDEX_FOR_COLUMN_CONTROLS);
    row++;
    
    
    QGroupBox* widget = new QGroupBox("Edit Configuration");
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->setMargin(0);
    widgetLayout->addWidget(numberOfWidget);
    widgetLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    widgetLayout->addWidget(stretchFactorWidget);
    widgetLayout->addStretch();
    
    return widget;
}

/**
 * Update the content of the dialog.
 */
void
TileTabsConfigurationDialog::updateDialog()
{
    m_caretPreferences->readTileTabsConfigurations();
    
    const AString selectedTileTabsName = m_configurationSelectionComboBox->currentText();
    int defaultIndex = m_configurationSelectionComboBox->currentIndex();
    
    m_configurationSelectionComboBox->blockSignals(true);
    m_configurationSelectionComboBox->clear();
    
    std::vector<const TileTabsConfiguration*> configurations = m_caretPreferences->getTileTabsConfigurationsSortedByName();
    const int32_t numConfig = static_cast<int32_t>(configurations.size());
    for (int32_t i = 0; i < numConfig; i++) {
        const TileTabsConfiguration* ttc = configurations[i];
        
        /*
         * Second element is user data which contains the Unique ID
         */
        m_configurationSelectionComboBox->addItem(ttc->getName(),
                                                  QVariant(ttc->getUniqueIdentifier()));
        
        if (ttc->getName() == selectedTileTabsName) {
            defaultIndex = i;
        }
    }

    const int32_t numItemsInComboBox = m_configurationSelectionComboBox->count();
    if (defaultIndex >= numItemsInComboBox) {
        defaultIndex = numItemsInComboBox - 1;
    }
    if (defaultIndex < 0) {
        defaultIndex = 0;
    }
    if (defaultIndex < m_configurationSelectionComboBox->count()) {
        m_configurationSelectionComboBox->setCurrentIndex(defaultIndex);
        configurationComboBoxItemSelected(defaultIndex);
    }
    
    m_configurationSelectionComboBox->blockSignals(false);
}

/**
 * Select the tile tabs configuration with the given name.
 */
void
TileTabsConfigurationDialog::selectTileTabConfigurationByUniqueID(const AString& uniqueID)
{
    const int32_t numItems = m_configurationSelectionComboBox->count();
    for (int32_t i = 0; i < numItems; i++) {
        const AString itemID = m_configurationSelectionComboBox->itemData(i,
                                                                          Qt::UserRole).toString();
        if (itemID == uniqueID) {
            m_configurationSelectionComboBox->setCurrentIndex(i);
            configurationComboBoxItemSelected(i);
            break;
        }
    }
}

/**
 * Called when a configuration is selected from the combo box.
 *
 * @param indx
 *    Index of item selected.
 */
void
TileTabsConfigurationDialog::configurationComboBoxItemSelected(int indx)
{
    if ((indx >= 0)
        && (indx < m_configurationSelectionComboBox->count())) {
        const AString itemID = m_configurationSelectionComboBox->itemData(indx,
                                                                          Qt::UserRole).toString();
        TileTabsConfiguration* ttc = m_caretPreferences->getTileTabsConfigurationByUniqueIdentifier(itemID);
        if (ttc != NULL) {
            m_numberOfRowsSpinBox->blockSignals(true);
            m_numberOfRowsSpinBox->setValue(ttc->getNumberOfRows());
            m_numberOfRowsSpinBox->blockSignals(false);
            
            m_numberOfColumnsSpinBox->blockSignals(true);
            m_numberOfColumnsSpinBox->setValue(ttc->getNumberOfColumns());
            m_numberOfColumnsSpinBox->blockSignals(false);
        }
    }
}

/**
 * Called when new configuration button is clicked.
 */
void
TileTabsConfigurationDialog::newConfigurationButtonClicked()
{
    AString newTileTabsName;
    
    AString configurationUniqueID;
    
    bool exitLoop = false;
    while (exitLoop == false) {
        /*
         * Popup dialog to get name for new configuration
         */
        WuQDataEntryDialog ded("New Tile Tabs Configuration",
                               m_newConfigurationPushButton);
        
        QLineEdit* nameLineEdit = ded.addLineEditWidget("View Name");
        nameLineEdit->setText(newTileTabsName);
        if (ded.exec() == WuQDataEntryDialog::Accepted) {
            /*
             * Make sure name is not empty
             */
            newTileTabsName = nameLineEdit->text().trimmed();
            if (newTileTabsName.isEmpty()) {
                WuQMessageBox::errorOk(m_newConfigurationPushButton,
                                       "Enter a name");
            }
            else {
                /*
                 * See if a configuration with the user entered name already exists
                 */
                TileTabsConfiguration* ttc = m_caretPreferences->getTileTabsConfigurationByName(newTileTabsName);
                if (ttc != NULL) {
                    const QString msg = ("Configuration named \""
                                         + newTileTabsName
                                         + "\" already exits.  Rename it?");
                    if (WuQMessageBox::warningYesNo(m_newConfigurationPushButton,
                                                    msg)) {
                        ttc->setName(newTileTabsName);
                        configurationUniqueID = ttc->getUniqueIdentifier();
                        exitLoop = true;
                    }
                }
                else {
                    /*
                     * Create a new configuration with the name
                     * entered by the user.
                     */
                    TileTabsConfiguration* configuration = new TileTabsConfiguration();
                    configuration->setName(newTileTabsName);
                    configurationUniqueID = configuration->getUniqueIdentifier();
                    m_caretPreferences->addTileTabsConfiguration(configuration);
                    exitLoop = true;
                }
            }
        }
        else {
            /*
             * User pressed cancel button.
             */
            exitLoop = true;
        }
    }
    
    if (configurationUniqueID.isEmpty() == false) {
        updateDialog();
        selectTileTabConfigurationByUniqueID(configurationUniqueID);
    }
}

/**
 * Called when delete configuration button is clicked.
 */
void
TileTabsConfigurationDialog::deleteConfigurationButtonClicked()
{
    TileTabsConfiguration* configuration = getSelectedTileTabsConfiguration();
    if (configuration != NULL) {
        const QString msg = ("Delete configuration named \""
                             + configuration->getName()
                             + "\" ?");
        if (WuQMessageBox::warningYesNo(m_newConfigurationPushButton,
                                        msg)) {
            m_caretPreferences->removeTileTabsConfigurationByUniqueIdentifier(configuration->getUniqueIdentifier());
            updateDialog();
        }
    }
}

/**
 * @return A pointer to the selected tile tabs configuration of NULL if 
 * no configuration is available.
 */
TileTabsConfiguration*
TileTabsConfigurationDialog::getSelectedTileTabsConfiguration()
{
    const AString uniqueID = getSelectedTileTabsConfigurationUniqueID();
    TileTabsConfiguration* configuration = m_caretPreferences->getTileTabsConfigurationByUniqueIdentifier(uniqueID);
    return configuration;
}

/**
 * @return The unique identifier of the selected tile tabs configuration an
 * empty string if no configuration is available.
 */
AString
TileTabsConfigurationDialog::getSelectedTileTabsConfigurationUniqueID()
{
    AString uniqueID;
    
    const int32_t indx = m_configurationSelectionComboBox->currentIndex();
    if ((indx >= 0)
        && (indx < m_configurationSelectionComboBox->count())) {
        uniqueID = m_configurationSelectionComboBox->itemData(indx,
                                                              Qt::UserRole).toString();
    }
    
    return uniqueID;
}


/**
 * Called when the number of rows or columns changes.
 */
void
TileTabsConfigurationDialog::numberOfRowsOrColumnsChanged()
{
    TileTabsConfiguration* configuration = getSelectedTileTabsConfiguration();
    if (configuration != NULL) {
        configuration->setNumberOfRows(m_numberOfRowsSpinBox->value());
        configuration->setNumberOfColumns(m_numberOfColumnsSpinBox->value());
        m_caretPreferences->writeTileTabsConfigurations();
    }
}

/**
 * Called when a configuration stretch factor value is changed.
 */
void
TileTabsConfigurationDialog::configurationStretchFactorWasChanged()
{
    
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
TileTabsConfigurationDialog::receiveEvent(Event* event)
{
//    if (event->getEventType() == EventTypeEnum::) {
//        <EVENT_CLASS_NAME*> eventName = dynamic_cast<EVENT_CLASS_NAME*>(event);
//        CaretAssert(eventName);
//
//        event->setEventProcessed();
//    }
}



/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QVBoxLayout>


#define __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
#include "TileTabsConfigurationDialog.h"
#undef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowComboBox.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "SessionManager.h"
#include "TileTabsConfiguration.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQListWidget.h"
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
 *
 * @param parentBrainBrowserWindow
 *     Parent window.
 */
TileTabsConfigurationDialog::TileTabsConfigurationDialog(BrainBrowserWindow* parentBrainBrowserWindow)
: WuQDialogNonModal("Tile Tabs Configuration",
                    parentBrainBrowserWindow)
{
    m_brainBrowserWindow = parentBrainBrowserWindow;
    m_blockReadConfigurationsFromPreferences = false;
    m_caretPreferences = SessionManager::get()->getCaretPreferences();
    
    QWidget* dialogWidget = new QWidget();
    QHBoxLayout* configurationLayout = new QHBoxLayout(dialogWidget);
    configurationLayout->setSpacing(0);
    configurationLayout->addWidget(createActiveConfigurationWidget(),
                                   0);
    configurationLayout->addWidget(createCopyLoadPushButtonsWidget(),
                                   0,
                                   Qt::AlignTop);
    configurationLayout->addWidget(createUserConfigurationSelectionWidget(),
                                   0,
                                   Qt::AlignTop);
    
    setCentralWidget(dialogWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    disableAutoDefaultForAllPushButtons();

    setApplyButtonText("");
    
    updateDialogWithSelectedTileTabsFromWindow(parentBrainBrowserWindow);
}

/**
 * Destructor.
 */
TileTabsConfigurationDialog::~TileTabsConfigurationDialog()
{
}

/**
 * Gets called when the dialog gains focus.
 */
void
TileTabsConfigurationDialog::focusGained()
{
    updateDialog();
}

/**
 * @return Create and return the copy and load buttons widget.
 */
QWidget*
TileTabsConfigurationDialog::createCopyLoadPushButtonsWidget()
{
    QPushButton* copyPushButton = new QPushButton("Copy -->");
    WuQtUtilities::setWordWrappedToolTip(copyPushButton,
                                         "Copy the Rows, Columns, and Stretch Factors into the selected Configuration.");
    QObject::connect(copyPushButton, SIGNAL(clicked()),
                     this, SLOT(copyPushButtonClicked()));
    
    QPushButton* loadPushButton = new QPushButton("<-- Load");
    WuQtUtilities::setWordWrappedToolTip(loadPushButton,
                                         "Load the Configuration into the Rows, Columns, and Stretch Factors.");
    QObject::connect(loadPushButton, SIGNAL(clicked()),
                     this, SLOT(loadPushButtonClicked()));
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addSpacing(50);
    layout->addWidget(copyPushButton);
    layout->addSpacing(50);
    layout->addWidget(loadPushButton);
    layout->addStretch();
    
    return widget;
}

/**
 * Called when Copy pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::copyPushButtonClicked()
{
    
}

/**
 * Called when Load pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::loadPushButtonClicked()
{
    
}

/**
 * @return The configuration selection widget.
 */
QWidget*
TileTabsConfigurationDialog::createUserConfigurationSelectionWidget()
{
    m_configurationSelectionListWidget = new WuQListWidget();
    m_configurationSelectionListWidget->setSelectionMode(QListWidget::SingleSelection);
    QObject::connect(m_configurationSelectionListWidget, &WuQListWidget::itemClicked,
                     this, &TileTabsConfigurationDialog::configurationListItemSelected);
    
    QHBoxLayout* selectionLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(selectionLayout,
                                    0);
    selectionLayout->addWidget(m_configurationSelectionListWidget, 100);
    
    m_newConfigurationPushButton = new QPushButton("New...");
    QObject::connect(m_newConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(newConfigurationButtonClicked()));
    
    m_renameConfigurationPushButton = new QPushButton("Rename...");
    QObject::connect(m_renameConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(renameConfigurationButtonClicked()));
    
    m_deleteConfigurationPushButton = new QPushButton("Delete...");
    QObject::connect(m_deleteConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(deleteConfigurationButtonClicked()));
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_newConfigurationPushButton);
    buttonsLayout->addWidget(m_renameConfigurationPushButton);
    buttonsLayout->addWidget(m_deleteConfigurationPushButton);
    buttonsLayout->addStretch();
    
    QGroupBox* configurationWidget = new QGroupBox("User Configurations");
    QVBoxLayout* configurationLayout = new QVBoxLayout(configurationWidget);
    configurationLayout->addWidget(m_configurationSelectionListWidget,
                                   100,
                                   Qt::AlignHCenter);
    configurationLayout->addLayout(buttonsLayout,
                                   0);
    
    return configurationWidget;
}

/**
 * @return The active configuration widget.
 */
QWidget*
TileTabsConfigurationDialog::createActiveConfigurationWidget()
{
    /*
     * Window number
     */
    QLabel* windowLabel = new QLabel("Workbench Window");
    m_browserWindowComboBox = new BrainBrowserWindowComboBox(BrainBrowserWindowComboBox::STYLE_NUMBER,
                                                             this);
    m_browserWindowComboBox->getWidget()->setFixedWidth(60);
    QObject::connect(m_browserWindowComboBox, SIGNAL(browserWindowSelected(BrainBrowserWindow*)),
                     this, SLOT(browserWindowComboBoxValueChanged(BrainBrowserWindow*)));
    
    
    const int32_t maximumNumberOfRows = TileTabsConfiguration::getMaximumNumberOfRows();
    const int32_t maximumNumberOfColumns = TileTabsConfiguration::getMaximumNumberOfColumns();
    
    m_automaticConfigurationCheckBox = new QCheckBox("Automatic Configuration");
    QObject::connect(m_automaticConfigurationCheckBox, &QCheckBox::clicked,
                     this, &TileTabsConfigurationDialog::automaticConfigurationCheckBoxClicked);
    
    QLabel* rowsLabel = new QLabel("Number of Rows");
    m_numberOfRowsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                          maximumNumberOfRows,
                                                                          1,
                                                                          this,
                                                                          SLOT(numberOfRowsOrColumnsChanged()));

    QLabel* columnsLabel = new QLabel("Columns");
    m_numberOfColumnsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                          maximumNumberOfColumns,
                                                                          1,
                                                                          this,
                                                                          SLOT(numberOfRowsOrColumnsChanged()));
    
    
    QHBoxLayout* numberOfLayout = new QHBoxLayout();
    numberOfLayout->setContentsMargins(0, 0, 0, 0);
    numberOfLayout->setSpacing(4);
    numberOfLayout->addWidget(rowsLabel);
    numberOfLayout->addWidget(m_numberOfRowsSpinBox);
    numberOfLayout->addSpacing(4);
    numberOfLayout->addWidget(columnsLabel);
    numberOfLayout->addWidget(m_numberOfColumnsSpinBox);
    numberOfLayout->addStretch();

    QHBoxLayout* windowLayout = new QHBoxLayout();
    windowLayout->setContentsMargins(0, 0, 0, 0);
    windowLayout->addWidget(windowLabel);
    windowLayout->addWidget(m_browserWindowComboBox->getWidget());
    windowLayout->addStretch();

    QVBoxLayout* topLayout = new QVBoxLayout();
    topLayout->setContentsMargins(0, 0, 0, 0);
    topLayout->addLayout(windowLayout);
    topLayout->addWidget(m_automaticConfigurationCheckBox, 0, Qt::AlignLeft);
    
    const float stretchFactorMinimumValue = 0.1;
    const float stretchFactorMaximumValue = 10000000.0;
    const float stretchFactorStep = 0.1;
    const float stretchFactorDigitsRightOfDecimal = 2;
    
    const int32_t spinBoxWidth = 80;
    
    QWidget* rowStretchFactorWidget = new QWidget();
    QGridLayout* rowStretchFactorLayout = new QGridLayout(rowStretchFactorWidget);
    rowStretchFactorLayout->setContentsMargins(0, 0, 0, 0);
    rowStretchFactorLayout->setSpacing(2);
    {
        rowStretchFactorLayout->addWidget(new QLabel("Rows"), 0, 0, 1, 2, Qt::AlignHCenter);
        
        for (int32_t i = 0; i < maximumNumberOfRows; i++) {
            AString labelSpace = ((i >= 10) ? "" : "  ");
            if (i < maximumNumberOfRows) {
                QLabel* numberLabel = new QLabel(labelSpace + AString::number(i + 1));
                m_rowStretchFactorIndexLabels.push_back(numberLabel);

                QDoubleSpinBox* spinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(stretchFactorMinimumValue,
                                                                                                            stretchFactorMaximumValue,
                                                                                                            stretchFactorStep,
                                                                                                            stretchFactorDigitsRightOfDecimal,
                                                                                                            this,
                                                                                                            SLOT(configurationStretchFactorWasChanged()));
                spinBox->setFixedWidth(spinBoxWidth);
                m_rowStretchFactorSpinBoxes.push_back(spinBox);
                
                const int layoutRow = rowStretchFactorLayout->rowCount();
                rowStretchFactorLayout->addWidget(numberLabel, layoutRow, 0, Qt::AlignRight);
                rowStretchFactorLayout->addWidget(spinBox, layoutRow, 1);
            }
        }
        
        rowStretchFactorLayout->setRowStretch(maximumNumberOfRows, 100);
    }
    
    QWidget* columnStretchFactorWidget = new QWidget();
    QGridLayout* columnStretchFactorLayout = new QGridLayout(columnStretchFactorWidget);
    columnStretchFactorLayout->setContentsMargins(0, 0, 0, 0);
    columnStretchFactorLayout->setSpacing(2);
    {
        columnStretchFactorLayout->addWidget(new QLabel("Columns"), 0, 0, 1, 2, Qt::AlignHCenter);
        
        for (int32_t i = 0; i < maximumNumberOfColumns; i++) {
            AString labelSpace = ((i >= 10) ? "" : "  ");
            if (i < maximumNumberOfColumns) {
                QLabel* numberLabel = new QLabel(labelSpace + AString::number(i + 1));
                m_columnStretchFactorIndexLabels.push_back(numberLabel);
                
                QDoubleSpinBox* spinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(stretchFactorMinimumValue,
                                                                                                            stretchFactorMaximumValue,
                                                                                                            stretchFactorStep,
                                                                                                            stretchFactorDigitsRightOfDecimal,
                                                                                                            this,
                                                                                                            SLOT(configurationStretchFactorWasChanged()));
                spinBox->setFixedWidth(spinBoxWidth);
                m_columnStretchFactorSpinBoxes.push_back(spinBox);
                
                const int layoutRow = columnStretchFactorLayout->rowCount();
                columnStretchFactorLayout->addWidget(numberLabel, layoutRow, 0, Qt::AlignRight);
                columnStretchFactorLayout->addWidget(spinBox, layoutRow, 1);
            }
        }
        
        columnStretchFactorLayout->setRowStretch(maximumNumberOfColumns, 100);
    }
    
    QLabel* stretchFactorLabel = new QLabel("Stretch Factors");
    
    m_stretchFactorWidget = new QWidget();
    m_stretchFactorWidget->setSizePolicy(m_stretchFactorWidget->sizePolicy().horizontalPolicy(),
                                         QSizePolicy::Maximum);
    QHBoxLayout* stretchFactorLayout = new QHBoxLayout(m_stretchFactorWidget);
    stretchFactorLayout->addWidget(rowStretchFactorWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
    stretchFactorLayout->addWidget(WuQtUtilities::createVerticalLineWidget(), 0);
    stretchFactorLayout->addWidget(columnStretchFactorWidget, 0, Qt::AlignHCenter | Qt::AlignTop);
    
    m_stretchFactorScrollArea = new QScrollArea();
    m_stretchFactorScrollArea->setWidget(m_stretchFactorWidget);
    m_stretchFactorScrollArea->setWidgetResizable(false);
    m_stretchFactorScrollArea->setMinimumWidth(m_stretchFactorWidget->sizeHint().width() + 10);
    
    QGroupBox* widget = new QGroupBox("Active Configuration");
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->addLayout(topLayout,
                            0);
    widgetLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                            0);
    widgetLayout->addLayout(numberOfLayout);
    widgetLayout->addWidget(stretchFactorLabel,
                            0,
                            Qt::AlignHCenter);
    widgetLayout->addWidget(m_stretchFactorScrollArea,
                            0);
    widgetLayout->addStretch();
    
    return widget;
}

/**
 * Called when window number combo box value changed.
 */
void
TileTabsConfigurationDialog::browserWindowComboBoxValueChanged(BrainBrowserWindow* browserWindow)
{
    int32_t windowIndex = -1;
    if (browserWindow != NULL) {
        windowIndex = browserWindow->getBrowserWindowIndex();
    }
    
    updateDialogWithSelectedTileTabsFromWindow(GuiManager::get()->getBrowserWindowByWindowIndex(windowIndex));
}

/**
 * Called when automatic configuration checkbox is clicked
 *
 * @param checked
 *     New checked status of checkbox.
 */
void
TileTabsConfigurationDialog::automaticConfigurationCheckBoxClicked(bool checked)
{
    
}



/**
 * Update the content of the dialog.  If tile tabs is selected in the given
 * window, the dialog will be initialized with the tile tabs configuration
 * selected in the window.
 *
 * @param brainBrowserWindow
 *    Browser window from which dialog was selected.
 */
void
TileTabsConfigurationDialog::updateDialogWithSelectedTileTabsFromWindow(BrainBrowserWindow* brainBrowserWindow)
{
    CaretAssert(brainBrowserWindow);
    m_brainBrowserWindow = brainBrowserWindow;
    
//    readConfigurationsFromPreferences();
//    
//    if (m_brainBrowserWindow->isTileTabsSelected()) {
//        TileTabsConfiguration* configuration = m_brainBrowserWindow->getSelectedTileTabsConfiguration();
//        if (configuration != NULL) {
//            selectTileTabConfigurationByUniqueID(configuration->getUniqueIdentifier());
//        }
//    }
    
    updateDialog();
}

/**
 * Update the tile tabs configuration in the brain browser window if
 * the browser window has tile tabs enabled.
 */
void
TileTabsConfigurationDialog::updateBrowserWindowsTileTabsConfigurationSelection()
{
    if (m_brainBrowserWindow != NULL) {
        if (m_brainBrowserWindow->isTileTabsSelected()) {
            m_brainBrowserWindow->setSelectedTileTabsConfiguration(getSelectedTileTabsConfiguration());
            updateGraphicsWindows();
        }
    }
}

/**
 * Read the configurations from the preferences.
 */
void
TileTabsConfigurationDialog::readConfigurationsFromPreferences()
{
    if (m_blockReadConfigurationsFromPreferences) {
        return;
    }
    
    m_caretPreferences->readTileTabsConfigurations();
}

/**
 * Update the content of the dialog.
 */
void
TileTabsConfigurationDialog::updateDialog()
{
    m_browserWindowComboBox->updateComboBox();
    m_browserWindowComboBox->setBrowserWindow(m_brainBrowserWindow);
    /* if window was invalid, this updates to valid window */
    m_brainBrowserWindow = m_browserWindowComboBox->getSelectedBrowserWindow();
    CaretAssert(m_brainBrowserWindow);
    if (m_brainBrowserWindow == NULL) {
        close();
    }
    
    readConfigurationsFromPreferences();
    
    const AString selectedUniqueID = getSelectedTileTabsConfigurationUniqueID();
    int defaultIndex = m_configurationSelectionListWidget->currentRow();
    
    QSignalBlocker blocker(m_configurationSelectionListWidget);
    m_configurationSelectionListWidget->clear();
    
    std::vector<const TileTabsConfiguration*> configurations = m_caretPreferences->getTileTabsConfigurationsSortedByName();
    const int32_t numConfig = static_cast<int32_t>(configurations.size());
    for (int32_t i = 0; i < numConfig; i++) {
        const TileTabsConfiguration* configuration = configurations[i];
        
        /*
         * Second element is user data which contains the Unique ID
         */
        QListWidgetItem* item = new QListWidgetItem(configuration->getName());
        item->setData(Qt::UserRole,
                      QVariant(configuration->getUniqueIdentifier()));
        m_configurationSelectionListWidget->addItem(item);
        
        if (configuration->getUniqueIdentifier() == selectedUniqueID) {
            defaultIndex = i;
        }
    }

    const int32_t numItemsInComboBox = m_configurationSelectionListWidget->count();
    if (defaultIndex >= numItemsInComboBox) {
        defaultIndex = numItemsInComboBox - 1;
    }
    if (defaultIndex < 0) {
        defaultIndex = 0;
    }
    if (defaultIndex < m_configurationSelectionListWidget->count()) {
        m_configurationSelectionListWidget->setCurrentRow(defaultIndex);
        selectConfigurationFromComboBoxIndex(defaultIndex);
    }
    
    updateStretchFactors();
}

/**
 * Update the stretch factors.
 */
void
TileTabsConfigurationDialog::updateStretchFactors()
{
    int32_t numValidRows = 0;
    int32_t numValidColumns = 0;
    
    const TileTabsConfiguration* configuration = getSelectedTileTabsConfiguration();
    if (configuration != NULL) {
        numValidRows = configuration->getNumberOfRows();
        numValidColumns = configuration->getNumberOfColumns();
    }
    
    CaretAssert(m_columnStretchFactorIndexLabels.size() == m_columnStretchFactorSpinBoxes.size());
    const int32_t numColSpinBoxes = static_cast<int32_t>(m_columnStretchFactorSpinBoxes.size());
    for (int32_t i = 0; i < numColSpinBoxes; i++) {
        CaretAssertVectorIndex(m_columnStretchFactorSpinBoxes, i);
        CaretAssertVectorIndex(m_columnStretchFactorIndexLabels, i);
        QDoubleSpinBox* sb = m_columnStretchFactorSpinBoxes[i];
        QLabel* label = m_columnStretchFactorIndexLabels[i];
        if (i < numValidColumns) {
            QSignalBlocker blocker(sb);
            sb->setValue(configuration->getColumnStretchFactor(i));
        }
        label->setVisible(i < numValidColumns);
        sb->setVisible(i < numValidColumns);
    }

    CaretAssert(m_rowStretchFactorIndexLabels.size() == m_rowStretchFactorSpinBoxes.size());
    const int32_t numRowSpinBoxes = static_cast<int32_t>(m_rowStretchFactorSpinBoxes.size());
    for (int32_t i = 0; i < numRowSpinBoxes; i++) {
        CaretAssertVectorIndex(m_rowStretchFactorIndexLabels, i);
        CaretAssertVectorIndex(m_rowStretchFactorSpinBoxes, i);
        QDoubleSpinBox* sb = m_rowStretchFactorSpinBoxes[i];
        QLabel* label = m_rowStretchFactorIndexLabels[i];
        if (i < numValidRows) {
            QSignalBlocker blocker(sb);
            sb->setValue(configuration->getRowStretchFactor(i));
        }
        label->setVisible(i < numValidRows);
        sb->setVisible(i < numValidRows);
    }
    
    m_stretchFactorWidget->setFixedSize(m_stretchFactorWidget->sizeHint());
}

/**
 * Select the tile tabs configuration with the given name.
 */
void
TileTabsConfigurationDialog::selectTileTabConfigurationByUniqueID(const AString& uniqueID)
{
    const int32_t numItems = m_configurationSelectionListWidget->count();
    for (int32_t i = 0; i < numItems; i++) {
        QListWidgetItem* item = m_configurationSelectionListWidget->item(i);
        const AString itemID = item->data(Qt::UserRole).toString();
        if (itemID == uniqueID) {
            QSignalBlocker blocker(m_configurationSelectionListWidget);
            m_configurationSelectionListWidget->setCurrentItem(item);
            selectConfigurationFromComboBoxIndex(i);
            break;
        }
    }
}

/**
 * Called when a configuration is selected from the list widget
 *
 * @param indx
 *    Index of item selected.
 */
void
TileTabsConfigurationDialog::configurationListItemSelected(QListWidgetItem* item)
{
    const int32_t indx = m_configurationSelectionListWidget->row(item);
    selectConfigurationFromComboBoxIndex(indx);
    updateBrowserWindowsTileTabsConfigurationSelection();
}

/**
 * Select the configuration at the given index from the configuration combo box.
 *
 * @param indx
 *    Index of item for selection.
 */
void
TileTabsConfigurationDialog::selectConfigurationFromComboBoxIndex(int indx)
{
    if ((indx >= 0)
        && (indx < m_configurationSelectionListWidget->count())) {
        QListWidgetItem* item = m_configurationSelectionListWidget->item(indx);
        const AString itemID = item->data(Qt::UserRole).toString();
        TileTabsConfiguration* configuration = m_caretPreferences->getTileTabsConfigurationByUniqueIdentifier(itemID);
        if (configuration != NULL) {
            m_numberOfRowsSpinBox->blockSignals(true);
            m_numberOfRowsSpinBox->setValue(configuration->getNumberOfRows());
            m_numberOfRowsSpinBox->blockSignals(false);
            
            m_numberOfColumnsSpinBox->blockSignals(true);
            m_numberOfColumnsSpinBox->setValue(configuration->getNumberOfColumns());
            m_numberOfColumnsSpinBox->blockSignals(false);
        }
    }
    
    updateStretchFactors();
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
                TileTabsConfiguration* configuration = m_caretPreferences->getTileTabsConfigurationByName(newTileTabsName);
                if (configuration != NULL) {
                    const QString msg = ("Configuration named \""
                                         + newTileTabsName
                                         + "\" already exits.  Rename it?");
                    if (WuQMessageBox::warningYesNo(m_newConfigurationPushButton,
                                                    msg)) {
                        configuration->setName(newTileTabsName);
                        configurationUniqueID = configuration->getUniqueIdentifier();
                        exitLoop = true;
                    }
                }
                else {
                    /*
                     * New configuration is copy of selected configuration (if available)
                     */
                    const TileTabsConfiguration* selectedConfiguration = getSelectedTileTabsConfiguration();
                    TileTabsConfiguration* configuration = ((selectedConfiguration != NULL)
                                                            ? selectedConfiguration->newCopyWithNewUniqueIdentifier()
                                                            : new TileTabsConfiguration());
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
        updateBrowserWindowsTileTabsConfigurationSelection();
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
        const AString uniqueID = configuration->getUniqueIdentifier();
        const QString msg = ("Delete configuration named \""
                             + configuration->getName()
                             + "\" ?");
        if (WuQMessageBox::warningYesNo(m_newConfigurationPushButton,
                                        msg)) {
            m_caretPreferences->removeTileTabsConfigurationByUniqueIdentifier(uniqueID);
            updateDialog();
            updateBrowserWindowsTileTabsConfigurationSelection();
        }
    }
}

/**
 * Called when rename configuration button is clicked.
 */
void
TileTabsConfigurationDialog::renameConfigurationButtonClicked()
{
    TileTabsConfiguration* configuration = getSelectedTileTabsConfiguration();
    if (configuration != NULL) {
        m_blockReadConfigurationsFromPreferences = true;
        
        bool ok = false;
        const AString oldName = configuration->getName();
        const AString newName = QInputDialog::getText(m_deleteConfigurationPushButton,
                                                      "Rename Configuration",
                                                      "Name",
                                                      QLineEdit::Normal,
                                                      oldName,
                                                      &ok);
        if (ok
            && (newName.isEmpty() == false)) {
            configuration->setName(newName);
            m_caretPreferences->writeTileTabsConfigurations();
            m_blockReadConfigurationsFromPreferences = false;
            updateDialog();
        }
        else {
            m_blockReadConfigurationsFromPreferences = false;
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
    
    const int32_t indx = m_configurationSelectionListWidget->currentRow();
    if ((indx >= 0)
        && (indx < m_configurationSelectionListWidget->count())) {
        QListWidgetItem* item = m_configurationSelectionListWidget->item(indx);
        uniqueID = item->data(Qt::UserRole).toString();
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
        
        updateStretchFactors();

        updateGraphicsWindows();
    }
}

/**
 * Called when a configuration stretch factor value is changed.
 */
void
TileTabsConfigurationDialog::configurationStretchFactorWasChanged()
{
    TileTabsConfiguration* configuration = getSelectedTileTabsConfiguration();
    if (configuration == NULL) {
        return;
    }
    
    const int32_t numColSpinBoxes = static_cast<int32_t>(m_columnStretchFactorSpinBoxes.size());
    for (int32_t i = 0; i < numColSpinBoxes; i++) {
        if (m_columnStretchFactorSpinBoxes[i]->isEnabled()) {
            configuration->setColumnStretchFactor(i,
                                                  m_columnStretchFactorSpinBoxes[i]->value());
        }
    }
    
    const int32_t numRowSpinBoxes = static_cast<int32_t>(m_rowStretchFactorSpinBoxes.size());
    for (int32_t i = 0; i < numRowSpinBoxes; i++) {
        if (m_rowStretchFactorSpinBoxes[i]->isEnabled()) {
            configuration->setRowStretchFactor(i,
                                               m_rowStretchFactorSpinBoxes[i]->value());
        }
    }
    
    m_caretPreferences->writeTileTabsConfigurations();
    
    updateGraphicsWindows();
}


/**
 * Update the graphics in any windows that have tile tabs enabled to the
 * selected tile tabs configuration in this dialog.
 */
void
TileTabsConfigurationDialog::updateGraphicsWindows()
{
    std::vector<BrainBrowserWindow*> allBrowserWindows = GuiManager::get()->getAllOpenBrainBrowserWindows();
    
    for (std::vector<BrainBrowserWindow*>::iterator iter = allBrowserWindows.begin();
         iter != allBrowserWindows.end();
         iter++) {
        BrainBrowserWindow* bbw = *iter;
        if (bbw->isTileTabsSelected()) {
            EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(bbw->getBrowserWindowIndex()).getPointer());
        }
    }
}



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
#include <QSpinBox>
#include <QVBoxLayout>


#define __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
#include "TileTabsConfigurationDialog.h"
#undef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowComboBox.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
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
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
}

/**
 * Destructor.
 */
TileTabsConfigurationDialog::~TileTabsConfigurationDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
TileTabsConfigurationDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN) {
        auto redrawEvent = dynamic_cast<EventBrowserWindowGraphicsRedrawn*>(event);
        CaretAssert(redrawEvent);
        
        BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
        CaretAssert(browserWindowContent);
        
        if (redrawEvent->getBrowserWindowIndex() == browserWindowContent->getWindowIndex()) {
            if (browserWindowContent->isTileTabsEnabled()) {
                if (browserWindowContent->isTileTabsAutomaticConfigurationEnabled()) {
                    updateStretchFactors();
                }
            }
        }
    }
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
    m_copyPushButton = new QPushButton("Copy -->");
    WuQtUtilities::setWordWrappedToolTip(m_copyPushButton,
                                         "Copy the Rows, Columns, and Stretch Factors into the selected User Configuration.");
    QObject::connect(m_copyPushButton, SIGNAL(clicked()),
                     this, SLOT(copyToUserConfigurationPushButtonClicked()));
    
    m_loadPushButton = new QPushButton("<-- Load");
    WuQtUtilities::setWordWrappedToolTip(m_loadPushButton,
                                         "Load the User Configuration into the Rows, Columns, and Stretch Factors.");
    QObject::connect(m_loadPushButton, SIGNAL(clicked()),
                     this, SLOT(loadIntoActiveConfigurationPushButtonClicked()));
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addSpacing(50);
    layout->addWidget(m_copyPushButton);
    layout->addSpacing(50);
    layout->addWidget(m_loadPushButton);
    layout->addStretch();
    
    return widget;
}

/**
 * Called when Copy to user configuration pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::copyToUserConfigurationPushButtonClicked()
{
    const TileTabsConfiguration* activeConfiguration = getActiveTileTabsConfiguration();
    CaretAssert(activeConfiguration);

    TileTabsConfiguration* userConfiguration = getSelectedUserTileTabsConfiguration();
    if (userConfiguration == NULL) {
        WuQMessageBox::errorOk(this,
                               "There are no user configurations");
        return;
    }
    
    userConfiguration->copy(*activeConfiguration);

    m_caretPreferences->writeTileTabsConfigurations();
}

/**
 * Called when Load pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::loadIntoActiveConfigurationPushButtonClicked()
{
    if (m_automaticConfigurationCheckBox->isChecked()) {
        return;
    }
    
    TileTabsConfiguration* activeConfiguration = getActiveTileTabsConfiguration();
    CaretAssert(activeConfiguration);
    
    const TileTabsConfiguration* userConfiguration = getSelectedUserTileTabsConfiguration();
    if (userConfiguration == NULL) {
        WuQMessageBox::errorOk(this,
                               "There are no user configurations");
        return;
    }
    
    activeConfiguration->copy(*userConfiguration);
    
    updateStretchFactors();
    updateGraphicsWindow();
}

/**
 * @return The browser window content for the selected window index.
 */
BrowserWindowContent*
TileTabsConfigurationDialog::getBrowserWindowContent()
{
    m_browserWindowComboBox->updateComboBox();
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    CaretAssert(bbw);
    BrowserWindowContent* bwc = bbw->getBrowerWindowContent();
    CaretAssert(bwc);
    
    return bwc;
}


/**
 * @return The configuration selection widget.
 */
QWidget*
TileTabsConfigurationDialog::createUserConfigurationSelectionWidget()
{
    m_userConfigurationSelectionListWidget = new WuQListWidget();
    m_userConfigurationSelectionListWidget->setSelectionMode(QListWidget::SingleSelection);
    
    QHBoxLayout* selectionLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(selectionLayout,
                                    0);
    selectionLayout->addWidget(m_userConfigurationSelectionListWidget, 100);
    
    const AString newToolTip = WuQtUtilities::createWordWrappedToolTipText("Create new User Configuration by entering a name.\n"
                                                                           "It will contain rows/columns/factors from the Active Configuration");
    m_newConfigurationPushButton = new QPushButton("New...");
    m_newConfigurationPushButton->setToolTip(newToolTip);
    QObject::connect(m_newConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(newUserConfigurationButtonClicked()));
    
    m_renameConfigurationPushButton = new QPushButton("Rename...");
    QObject::connect(m_renameConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(renameUserConfigurationButtonClicked()));
    
    m_deleteConfigurationPushButton = new QPushButton("Delete...");
    QObject::connect(m_deleteConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(deleteUserConfigurationButtonClicked()));
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_newConfigurationPushButton);
    buttonsLayout->addWidget(m_renameConfigurationPushButton);
    buttonsLayout->addWidget(m_deleteConfigurationPushButton);
    buttonsLayout->addStretch();
    
    QGroupBox* configurationWidget = new QGroupBox("User Configurations");
    QVBoxLayout* configurationLayout = new QVBoxLayout(configurationWidget);
    configurationLayout->addWidget(m_userConfigurationSelectionListWidget,
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
                                                                          SLOT(configurationNumberOfRowsOrColumnsChanged()));

    QLabel* columnsLabel = new QLabel("Columns");
    m_numberOfColumnsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                          maximumNumberOfColumns,
                                                                          1,
                                                                          this,
                                                                          SLOT(configurationNumberOfRowsOrColumnsChanged()));
    
    
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
    
    m_rowColumnFactorWidget = new QWidget();
    QVBoxLayout* rowColumnFactorLayout = new QVBoxLayout(m_rowColumnFactorWidget);
    rowColumnFactorLayout->setContentsMargins(0, 0, 0, 0);
    rowColumnFactorLayout->addLayout(numberOfLayout);
    rowColumnFactorLayout->addWidget(stretchFactorLabel,
                            0,
                            Qt::AlignHCenter);
    rowColumnFactorLayout->addWidget(m_stretchFactorScrollArea,
                            0);
    
    QGroupBox* widget = new QGroupBox("Active Configuration");
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->addLayout(topLayout,
                            0);
    widgetLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                            0);
    widgetLayout->addWidget(m_rowColumnFactorWidget);
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
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    browserWindowContent->setTileTabsAutomaticConfigurationEnabled(checked);
    updateStretchFactors();
    updateGraphicsWindow();
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
    
    m_browserWindowComboBox->updateComboBox();
    m_browserWindowComboBox->setBrowserWindow(brainBrowserWindow);
    
    updateDialog();
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
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    m_automaticConfigurationCheckBox->setChecked(browserWindowContent->isTileTabsAutomaticConfigurationEnabled());
    
    readConfigurationsFromPreferences();
    
    int defaultIndex = m_userConfigurationSelectionListWidget->currentRow();
    
    QSignalBlocker blocker(m_userConfigurationSelectionListWidget);
    m_userConfigurationSelectionListWidget->clear();
    
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
        m_userConfigurationSelectionListWidget->addItem(item);
    }

    const int32_t numItemsInComboBox = m_userConfigurationSelectionListWidget->count();
    if (defaultIndex >= numItemsInComboBox) {
        defaultIndex = numItemsInComboBox - 1;
    }
    if (defaultIndex < 0) {
        defaultIndex = 0;
    }
    if (defaultIndex < m_userConfigurationSelectionListWidget->count()) {
        m_userConfigurationSelectionListWidget->setCurrentRow(defaultIndex);
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
    
    const TileTabsConfiguration* configuration = getActiveTileTabsConfiguration();
    if (configuration != NULL) {
        numValidRows = configuration->getNumberOfRows();
        numValidColumns = configuration->getNumberOfColumns();
    }
    
    QSignalBlocker rowNumBlocker(m_numberOfRowsSpinBox);
    m_numberOfRowsSpinBox->setValue(numValidRows);
    
    QSignalBlocker colNumBlocker(m_numberOfColumnsSpinBox);
    m_numberOfColumnsSpinBox->setValue(numValidColumns);
    
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
    
    const bool editableFlag = ( ! m_automaticConfigurationCheckBox->isChecked());
    
    m_rowColumnFactorWidget->setEnabled(editableFlag);
    m_loadPushButton->setEnabled(editableFlag);
}

/**
 * Select the tile tabs configuration with the given name.
 */
void
TileTabsConfigurationDialog::selectTileTabConfigurationByUniqueID(const AString& uniqueID)
{
    const int32_t numItems = m_userConfigurationSelectionListWidget->count();
    for (int32_t i = 0; i < numItems; i++) {
        QListWidgetItem* item = m_userConfigurationSelectionListWidget->item(i);
        const AString itemID = item->data(Qt::UserRole).toString();
        if (itemID == uniqueID) {
            QSignalBlocker blocker(m_userConfigurationSelectionListWidget);
            m_userConfigurationSelectionListWidget->setCurrentItem(item);
            break;
        }
    }
}

/**
 * Called when new user configuration button is clicked.
 */
void
TileTabsConfigurationDialog::newUserConfigurationButtonClicked()
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
                    const TileTabsConfiguration* selectedConfiguration = getActiveTileTabsConfiguration();
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
    
    if ( ! configurationUniqueID.isEmpty()) {
        updateDialog();
        selectTileTabConfigurationByUniqueID(configurationUniqueID);
    }
}

/**
 * Called when delete user configuration button is clicked.
 */
void
TileTabsConfigurationDialog::deleteUserConfigurationButtonClicked()
{
    TileTabsConfiguration* configuration = getSelectedUserTileTabsConfiguration();
    if (configuration != NULL) {
        const AString uniqueID = configuration->getUniqueIdentifier();
        const QString msg = ("Delete configuration named \""
                             + configuration->getName()
                             + "\" ?");
        if (WuQMessageBox::warningYesNo(m_newConfigurationPushButton,
                                        msg)) {
            m_caretPreferences->removeTileTabsConfigurationByUniqueIdentifier(uniqueID);
            updateDialog();
        }
    }
}

/**
 * Called when rename user configuration button is clicked.
 */
void
TileTabsConfigurationDialog::renameUserConfigurationButtonClicked()
{
    TileTabsConfiguration* configuration = getSelectedUserTileTabsConfiguration();
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
TileTabsConfigurationDialog::getActiveTileTabsConfiguration()
{
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    return browserWindowContent->getTileTabsConfiguration();
}

/**
 * @return The selected user tile tabs configuration (will be
 *         NULL if there are no user configurations).
 */
TileTabsConfiguration*
TileTabsConfigurationDialog::getSelectedUserTileTabsConfiguration()
{
    TileTabsConfiguration* configuration = NULL;
    
    const int32_t indx = m_userConfigurationSelectionListWidget->currentRow();
    if ((indx >= 0)
        && (indx < m_userConfigurationSelectionListWidget->count())) {
        QListWidgetItem* item = m_userConfigurationSelectionListWidget->item(indx);
        const AString itemID = item->data(Qt::UserRole).toString();
        configuration = m_caretPreferences->getTileTabsConfigurationByUniqueIdentifier(itemID);
    }
    
    return configuration;
}

/**
 * Called when the number of rows or columns changes.
 */
void
TileTabsConfigurationDialog::configurationNumberOfRowsOrColumnsChanged()
{
    TileTabsConfiguration* configuration = getActiveTileTabsConfiguration();
    if (configuration != NULL) {
        configuration->setNumberOfRows(m_numberOfRowsSpinBox->value());
        configuration->setNumberOfColumns(m_numberOfColumnsSpinBox->value());
        m_caretPreferences->writeTileTabsConfigurations();
        
        updateStretchFactors();

        updateGraphicsWindow();
    }
}

/**
 * Called when a configuration stretch factor value is changed.
 */
void
TileTabsConfigurationDialog::configurationStretchFactorWasChanged()
{
    TileTabsConfiguration* configuration = getActiveTileTabsConfiguration();
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
    
    
    updateGraphicsWindow();
}


/**
 * Update the graphics for the selected window.
 */
void
TileTabsConfigurationDialog::updateGraphicsWindow()
{
    const BrowserWindowContent* bwc = getBrowserWindowContent();
    if (bwc->isTileTabsEnabled()) {
        const int32_t windowIndex = bwc->getWindowIndex();
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(windowIndex).getPointer());
    }
}


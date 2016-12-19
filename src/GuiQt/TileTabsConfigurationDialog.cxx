
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

#include <QComboBox>
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
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "GuiManager.h"
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
    m_blockReadConfigurationsFromPreferences = false;
    m_caretPreferences = SessionManager::get()->getCaretPreferences();
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->setSpacing(0);
    dialogLayout->addWidget(createConfigurationSelectionWidget(),
                            0,
                            Qt::AlignHCenter);
    dialogLayout->addWidget(createEditConfigurationWidget(),
                            100,
                            Qt::AlignHCenter);
    
    setCentralWidget(dialogWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    
    disableAutoDefaultForAllPushButtons();

    setApplyButtonText("");
    //    EventManager::get()->addEventListener(this, EventTypeEnum::);
    
    updateDialog();
    
    /*
     * The content region of a scroll area is often too large vertically
     * so adjust the size of the dialog which will cause the scroll area
     * to approximately fit its content.
     */
    WuQDialog::adjustSizeOfDialogWithScrollArea(this,
                                                m_stretchFactorScrollArea);
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
 * @return The configuration selection widget.
 */
QWidget*
TileTabsConfigurationDialog::createConfigurationSelectionWidget()
{
    QLabel* configurationLabel = new QLabel("Configuration");
    m_configurationSelectionComboBox = WuQFactory::newComboBoxSignalInt(this,
                                                                        SLOT(configurationComboBoxItemSelected(int)));
    
    QHBoxLayout* selectionLayout = new QHBoxLayout();
    WuQtUtilities::setLayoutMargins(selectionLayout,
                                    0);
    selectionLayout->addWidget(configurationLabel, 0);
    selectionLayout->addWidget(m_configurationSelectionComboBox, 100);
    
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
    buttonsLayout->addWidget(m_newConfigurationPushButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_renameConfigurationPushButton);
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(m_deleteConfigurationPushButton);
    
    QGroupBox* configurationWidget = new QGroupBox("Configuration Selection");
    QVBoxLayout* configurationLayout = new QVBoxLayout(configurationWidget);
    configurationLayout->addLayout(selectionLayout);
    configurationLayout->addLayout(buttonsLayout);
    
    return configurationWidget;
}

/**
 * @return The edit configuration widget.
 */
QWidget*
TileTabsConfigurationDialog::createEditConfigurationWidget()
{
    const int32_t maximuNumberOfRows = TileTabsConfiguration::getMaximumNumberOfRows();
    const int32_t maximumNumberOfColumns = TileTabsConfiguration::getMaximumNumberOfColumns();
    
    QLabel* rowsLabel = new QLabel("Number of Rows");
    QLabel* columnsLabel = new QLabel("Number of Columns");
    
    m_numberOfRowsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                          maximuNumberOfRows,
                                                                          1,
                                                                          this,
                                                                          SLOT(numberOfRowsOrColumnsChanged()));

    m_numberOfColumnsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                          maximumNumberOfColumns,
                                                                          1,
                                                                          this,
                                                                          SLOT(numberOfRowsOrColumnsChanged()));
    QWidget* numberOfWidget = new QWidget();
    QGridLayout* numberOfGridLayout = new QGridLayout(numberOfWidget);
    WuQtUtilities::setLayoutMargins(numberOfGridLayout,
                                    0);
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
    
    m_stretchFactorWidget = new QWidget();
    QGridLayout* stretchFactorGridLayout = new QGridLayout(m_stretchFactorWidget);
    WuQtUtilities::setLayoutMargins(stretchFactorGridLayout,
                                    2);
    stretchFactorGridLayout->setSizeConstraint(QLayout::SetFixedSize);
    int row = 0;
    stretchFactorGridLayout->addWidget(indexLabel,
                                       row, GRID_LAYOUT_COLUMN_INDEX_FOR_LABELS,
                                       Qt::AlignHCenter);
    stretchFactorGridLayout->addWidget(rowLabel,
                          row, GRID_LAYOUT_COLUMN_INDEX_FOR_ROW_CONTROLS,
                                       Qt::AlignHCenter);
    stretchFactorGridLayout->addWidget(columnLabel,
                          row, GRID_LAYOUT_COLUMN_INDEX_FOR_COLUMN_CONTROLS,
                                       Qt::AlignHCenter);
    row++;
    
    const float stretchMinimumValue = 0.1;
    const float stretchMaximumValue = 10000000.0;
    const float stretchStep = 0.1;
    const float stretchDigitsRightOfDecimal = 2;
    
    const int32_t spinBoxWidth = 80;
    const int32_t maxItems = std::max(maximuNumberOfRows,
                                      maximumNumberOfColumns);
    for (int i = 0; i < maxItems; i++) {
        QLabel* indexLabel = new QLabel(AString::number(i + 1));
        m_stretchFactorIndexLabels.push_back(indexLabel);
        stretchFactorGridLayout->addWidget(indexLabel,
                                           row, GRID_LAYOUT_COLUMN_INDEX_FOR_LABELS,
                                           Qt::AlignHCenter);
        
        if (i < maximuNumberOfRows) {
            QDoubleSpinBox* rowSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(stretchMinimumValue,
                                                                                                        stretchMaximumValue,
                                                                                                        stretchStep,
                                                                                                        stretchDigitsRightOfDecimal,
                                                                                                        this,
                                                                                                        SLOT(configurationStretchFactorWasChanged()));
            rowSpinBox->setFixedWidth(spinBoxWidth);
            m_rowStretchFactorSpinBoxes.push_back(rowSpinBox);
            stretchFactorGridLayout->addWidget(rowSpinBox,
                                               row, GRID_LAYOUT_COLUMN_INDEX_FOR_ROW_CONTROLS);
        }
        
        if (i < maximumNumberOfColumns) {
            QDoubleSpinBox* colSpinBox = WuQFactory::newDoubleSpinBoxWithMinMaxStepDecimalsSignalDouble(stretchMinimumValue,
                                                                                                        stretchMaximumValue,
                                                                                                        stretchStep,
                                                                                                        stretchDigitsRightOfDecimal,
                                                                                                        this,
                                                                                                        SLOT(configurationStretchFactorWasChanged()));
            colSpinBox->setFixedWidth(spinBoxWidth);
            m_columnStretchFactorSpinBoxes.push_back(colSpinBox);
            stretchFactorGridLayout->addWidget(colSpinBox,
                                               row, GRID_LAYOUT_COLUMN_INDEX_FOR_COLUMN_CONTROLS);
        }
        
        row++;
    }
    
    m_stretchFactorScrollArea = new QScrollArea();
    m_stretchFactorScrollArea->setWidget(m_stretchFactorWidget);
    m_stretchFactorScrollArea->setWidgetResizable(true);
    
    QGroupBox* widget = new QGroupBox("Edit Configuration");
    QVBoxLayout* widgetLayout = new QVBoxLayout(widget);
    widgetLayout->addWidget(numberOfWidget,
                            0);
    widgetLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                            0);
    widgetLayout->addWidget(stretchFactorLabel,
                            0,
                            Qt::AlignHCenter);
    widgetLayout->addWidget(m_stretchFactorScrollArea,
                            100);
    
    return widget;
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
    
    readConfigurationsFromPreferences();
    
    if (m_brainBrowserWindow->isTileTabsSelected()) {
        TileTabsConfiguration* configuration = m_brainBrowserWindow->getSelectedTileTabsConfiguration();
        if (configuration != NULL) {
            selectTileTabConfigurationByUniqueID(configuration->getUniqueIdentifier());
        }
    }
    
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
    readConfigurationsFromPreferences();
    
    const AString selectedUniqueID = getSelectedTileTabsConfigurationUniqueID();
    int defaultIndex = m_configurationSelectionComboBox->currentIndex();
    
    m_configurationSelectionComboBox->blockSignals(true);
    m_configurationSelectionComboBox->clear();
    
    std::vector<const TileTabsConfiguration*> configurations = m_caretPreferences->getTileTabsConfigurationsSortedByName();
    const int32_t numConfig = static_cast<int32_t>(configurations.size());
    for (int32_t i = 0; i < numConfig; i++) {
        const TileTabsConfiguration* configuration = configurations[i];
        
        /*
         * Second element is user data which contains the Unique ID
         */
        m_configurationSelectionComboBox->addItem(configuration->getName(),
                                                  QVariant(configuration->getUniqueIdentifier()));
        
        if (configuration->getUniqueIdentifier() == selectedUniqueID) {
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
        selectConfigurationFromComboBoxIndex(defaultIndex);
    }
    
    m_configurationSelectionComboBox->blockSignals(false);
    
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
    
    const int32_t numColSpinBoxes = static_cast<int32_t>(m_columnStretchFactorSpinBoxes.size());
    for (int32_t i = 0; i < numColSpinBoxes; i++) {
        QDoubleSpinBox* sb = m_columnStretchFactorSpinBoxes[i];
        if (i < numValidColumns) {
            sb->setVisible(true);
            sb->blockSignals(true);
            sb->setValue(configuration->getColumnStretchFactor(i));
            sb->blockSignals(false);
        }
        else {
            sb->setVisible(false);
        }
    }

    const int32_t numRowSpinBoxes = static_cast<int32_t>(m_rowStretchFactorSpinBoxes.size());
    for (int32_t i = 0; i < numRowSpinBoxes; i++) {
        QDoubleSpinBox* sb = m_rowStretchFactorSpinBoxes[i];
        if (i < numValidRows) {
            sb->setVisible(true);
            sb->blockSignals(true);
            sb->setValue(configuration->getRowStretchFactor(i));
            sb->blockSignals(false);
        }
        else {
            sb->setVisible(false);
        }
    }
    
    const int32_t numIndexLabels = static_cast<int32_t>(m_stretchFactorIndexLabels.size());
    const int32_t numValidLabels = std::max(numValidRows, numValidColumns);
    for (int32_t i = 0; i < numIndexLabels; i++) {
        if (i < numValidLabels) {
            m_stretchFactorIndexLabels[i]->setVisible(true);
        }
        else {
            m_stretchFactorIndexLabels[i]->setVisible(false);
        }
    }
    
    m_stretchFactorWidget->setFixedSize(m_stretchFactorWidget->sizeHint());
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
            selectConfigurationFromComboBoxIndex(i);
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
        && (indx < m_configurationSelectionComboBox->count())) {
        const AString itemID = m_configurationSelectionComboBox->itemData(indx,
                                                                          Qt::UserRole).toString();
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
//            if (bbw->getSelectedTileTabsConfiguration() == getSelectedTileTabsConfiguration()) {
                EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(bbw->getBrowserWindowIndex()).getPointer());
//            }
        }
    }
}


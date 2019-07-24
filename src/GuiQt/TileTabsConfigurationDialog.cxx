
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

#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>


#define __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
#include "TileTabsConfigurationDialog.h"
#undef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowComboBox.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventHelpViewerDisplay.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "SessionManager.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "TileTabsLayoutManualConfiguration.h"
#include "TileTabsGridRowColumnElement.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQGridLayoutGroup.h"
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
: WuQDialogNonModal("Tile Tabs Configurations",
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
                                   100,
                                   Qt::AlignTop);
    
    setApplyButtonText("");
    setStandardButtonText(QDialogButtonBox::Help,
                          "Help");
    
    updateDialogWithSelectedTileTabsFromWindow(parentBrainBrowserWindow);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    setCentralWidget(dialogWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    resize(750,
           500);
    
    disableAutoDefaultForAllPushButtons();
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
                    updateDialog();
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
    m_addPushButton     = new QPushButton("Add -->");
    m_addPushButton->setAutoDefault(false);
    WuQtUtilities::setWordWrappedToolTip(m_addPushButton,
                                         "Add a new User Configuration containing the Rows, Columns, and Stretch Factors "
                                         "with those from the Custom Configuration");
    QObject::connect(m_addPushButton, SIGNAL(clicked()),
                     this, SLOT(addUserConfigurationPushButtonClicked()));

    m_replacePushButton = new QPushButton("Replace -->");
    m_replacePushButton->setAutoDefault(false);
    WuQtUtilities::setWordWrappedToolTip(m_replacePushButton,
                                         "Replace the Rows, Columns, and Stretch Factors in the User Configuration "
                                         "with those from the Custom Configuration");
    QObject::connect(m_replacePushButton, SIGNAL(clicked()),
                     this, SLOT(replaceUserConfigurationPushButtonClicked()));
    
    m_loadPushButton = new QPushButton("<-- Load");
    m_loadPushButton->setAutoDefault(false);
    WuQtUtilities::setWordWrappedToolTip(m_loadPushButton,
                                         "Load the User Configuration's Rows, Columns, and Stretch Factors into "
                                         "the Custom Configuration");
    QObject::connect(m_loadPushButton, SIGNAL(clicked()),
                     this, SLOT(loadIntoActiveConfigurationPushButtonClicked()));
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addSpacing(50);
    layout->addWidget(m_addPushButton);
    layout->addSpacing(10);
    layout->addWidget(m_replacePushButton);
    layout->addSpacing(50);
    layout->addWidget(m_loadPushButton);
    layout->addStretch();
    
    return widget;
}

/**
 * @return Name for new tile tabs configuration
 * If empty, user cancelled.
 */
AString
TileTabsConfigurationDialog::getNewConfigurationName(QWidget* dialogParent)
{
    AString newTileTabsName;
    
    bool exitLoop = false;
    while (exitLoop == false) {
        /*
         * Popup dialog to get name for new configuration
         */
        WuQDataEntryDialog ded("New Tile Tabs Configuration",
                               dialogParent);
        
        QLineEdit* nameLineEdit = ded.addLineEditWidget("Configuration Name");
        nameLineEdit->setText(newTileTabsName);
        if (ded.exec() == WuQDataEntryDialog::Accepted) {
            /*
             * Make sure name is not empty
             */
            newTileTabsName = nameLineEdit->text().trimmed();
            if (newTileTabsName.isEmpty()) {
                WuQMessageBox::errorOk(dialogParent,
                                       "Empty name is invalid.");
            }
            else {
                /*
                 * See if a configuration with the user entered name already exists
                 */
                const TileTabsLayoutBaseConfiguration* configuration = m_caretPreferences->getTileTabsUserConfigurationByName(newTileTabsName);
                if (configuration != NULL) {
                    const QString msg = ("Configuration named \""
                                         + newTileTabsName
                                         + "\" already exits.  Choose a different name.");
                    WuQMessageBox::errorOk(dialogParent, msg);
                }
                else {
                    /*
                     * Have new name
                     */
                    exitLoop = true;
                }
            }
        }
        else {
            /*
             * User pressed cancel button.
             */
            exitLoop = true;
            newTileTabsName = "";
        }
    }

    return newTileTabsName;
}

/**
 * Called when Replace to user configuration pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::addUserConfigurationPushButtonClicked()
{
    const AString newConfigName = getNewConfigurationName(m_addPushButton);
    if (newConfigName.isEmpty()) {
        return;
    }
    
    m_blockReadConfigurationsFromPreferences = true;
    
    const TileTabsLayoutGridConfiguration* activeConfiguration = getCustomTileTabsGridConfiguration();
    CaretAssert(activeConfiguration);

    TileTabsLayoutGridConfiguration* configCopy = new TileTabsLayoutGridConfiguration(*activeConfiguration);
    CaretAssert(configCopy);
    configCopy->setName(newConfigName);
    m_caretPreferences->addTileTabsUserConfiguration(configCopy);
    
    m_blockReadConfigurationsFromPreferences = false;
    updateDialog();
}

/**
 * Called when Replace to user configuration pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::replaceUserConfigurationPushButtonClicked()
{
    const TileTabsLayoutGridConfiguration* activeConfiguration = getCustomTileTabsGridConfiguration();
    CaretAssert(activeConfiguration);

    const AString tileTabsUniqueID = getSelectedUserTileTabsConfigurationUniqueIdentifier();
    if (tileTabsUniqueID.isEmpty()) {
            WuQMessageBox::errorOk(this,
                                   "There are no user configurations, use Add button.");
            return;
    }
    else {
        const AString msg("Do you want to replace the configuration?");
        if ( ! WuQMessageBox::warningOkCancel(m_replacePushButton,
                                              msg)) {
            m_blockReadConfigurationsFromPreferences = false;
            return;
        }
    }
    
    m_blockReadConfigurationsFromPreferences = true;
    
    AString errorMessage;
    if ( ! m_caretPreferences->replaceTileTabsUserConfiguration(tileTabsUniqueID,
                                                                activeConfiguration,
                                                                errorMessage)) {
        WuQMessageBox::errorOk(m_replacePushButton,
                               errorMessage);
    }

    m_blockReadConfigurationsFromPreferences = false;
    updateDialog();
}

/**
 * Called when Load pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::loadIntoActiveConfigurationPushButtonClicked()
{
    if (m_automaticConfigurationRadioButton->isChecked()) {
        return;
    }
    
    const AString userConfigID = getSelectedUserTileTabsConfigurationUniqueIdentifier();
    if (userConfigID.isEmpty()) {
        WuQMessageBox::errorOk(this, "No user configuration is selected.");
        return;
    }
    
    std::unique_ptr<TileTabsLayoutBaseConfiguration> userConfiguration = m_caretPreferences->getCopyOfTileTabsUserConfigurationByUniqueIdentifier(userConfigID);
    if ( ! userConfiguration) {
        WuQMessageBox::errorOk(this,
                               ("User configuration with UniqueID="
                                + userConfigID
                                + " was not found"));
        return;
    }
    
    switch (userConfiguration->getLayoutType()) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
        {
            TileTabsLayoutGridConfiguration* activeGridConfiguration = getCustomTileTabsGridConfiguration();
            CaretAssert(activeGridConfiguration);
            
            const TileTabsLayoutGridConfiguration* copyConfig = userConfiguration->castToGridConfiguration();
            CaretAssert(copyConfig);
            
            activeGridConfiguration->copy(*copyConfig);
        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            CaretAssertToDoFatal();
            break;
    }
    
    updateStretchFactors();
    updateGraphicsWindow();
}

/**
 * @return The browser window selected window index.
 */
BrainBrowserWindow*
TileTabsConfigurationDialog::getBrowserWindow()
{
    m_browserWindowComboBox->updateComboBox();
    /*
     * This can be NULL when wb_view is closing.
     */
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    return bbw;
}

/**
 * @return The browser window content for the selected window index.
 * May be NULL when no tabs are open.
 */
BrowserWindowContent*
TileTabsConfigurationDialog::getBrowserWindowContent()
{
    BrowserWindowContent* bwc(NULL);
    BrainBrowserWindow* bbw = getBrowserWindow();
    if (bbw != NULL) {
        bwc = bbw->getBrowerWindowContent();
    }
    
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
                                                                           "It will contain rows/columns/factors from the Custom Configuration");

    m_renameConfigurationPushButton = new QPushButton("Rename...");
    m_renameConfigurationPushButton->setToolTip("Rename the selected User Configuration");
    m_renameConfigurationPushButton->setAutoDefault(false);
    QObject::connect(m_renameConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(renameUserConfigurationButtonClicked()));
    
    m_deleteConfigurationPushButton = new QPushButton("Delete...");
    m_deleteConfigurationPushButton->setToolTip("Delete the selected User Configuration");
    m_deleteConfigurationPushButton->setAutoDefault(false);
    QObject::connect(m_deleteConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(deleteUserConfigurationButtonClicked()));
    
    QGridLayout* buttonsLayout = new QGridLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addWidget(m_renameConfigurationPushButton, 0, 1, Qt::AlignHCenter);
    buttonsLayout->addWidget(m_deleteConfigurationPushButton, 1, 1, Qt::AlignHCenter);
    
    QGroupBox* configurationWidget = new QGroupBox("User Configurations");
    QVBoxLayout* configurationLayout = new QVBoxLayout(configurationWidget);
    configurationLayout->addWidget(m_userConfigurationSelectionListWidget,
                                   100);
    configurationLayout->addLayout(buttonsLayout,
                                   0);
    
    return configurationWidget;
}

/**
 * @return Instance of workbench window widget.
 */
QWidget*
TileTabsConfigurationDialog::createWorkbenchWindowWidget()
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
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(windowLabel);
    layout->addWidget(m_browserWindowComboBox->getWidget());
    layout->addStretch();
    
    return widget;
}

/**
 * @return The rows/columns stretch layout
 */
QWidget*
TileTabsConfigurationDialog::createCustomOptionsWidget()
{
    const QString toolTip("<html>"
                          "Removes any space between rows and columns in the tile tabs configuration.  "
                          "Some scenes created in previous versions of wb_view may not appear correctly "
                          "due to changes in layout of the tabs.  Enabling this option may fix the "
                          "problem.  In addition, if the Lock Aspect option is selected prior to "
                          "to enabling tile tabs, this option may improve the layout."
                          "</html>");
    m_centeringCorrectionCheckBox = new QCheckBox("Centering Correction");
    m_centeringCorrectionCheckBox->setToolTip(toolTip);
    QObject::connect(m_centeringCorrectionCheckBox, &QCheckBox::clicked,
                     this, &TileTabsConfigurationDialog::centeringCorrectionCheckBoxClicked);
    
    QGroupBox* groupBox = new QGroupBox("Options");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(m_centeringCorrectionCheckBox);
    
    return groupBox;
}

/**
 * Called when user checks/unchecks the centering correction checkbox
 *
 * @bool checked
 *     New checked status
 */
void
TileTabsConfigurationDialog::centeringCorrectionCheckBoxClicked(bool checked)
{
    TileTabsLayoutGridConfiguration* config = getCustomTileTabsGridConfiguration();
    if (config != NULL) {
        config->setCenteringCorrectionEnabled(checked);
        updateGraphicsWindow();
    }
}

/**
 * Update the custom options
 */
void
TileTabsConfigurationDialog::updateCustomOptionsWidget()
{
    const TileTabsLayoutGridConfiguration* config = getCustomTileTabsGridConfiguration();
    if (config != NULL) {
        m_centeringCorrectionCheckBox->setChecked(config->isCenteringCorrectionEnabled());
    }
}

/**
 * @return The rows/columns stretch layout
 */
QWidget*
TileTabsConfigurationDialog::createRowColumnStretchWidget()
{
    QGroupBox* rowGroupBox = new QGroupBox("Rows");
    rowGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_rowElementsGridLayout = new QGridLayout(rowGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(m_rowElementsGridLayout, 4, 2);
    
    QGroupBox* columnsGroupBox = new QGroupBox("Columns");
    m_columnElementsGridLayout = new QGridLayout(columnsGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(m_columnElementsGridLayout, 2, 2);
 
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(rowGroupBox);
    layout->addSpacing(6);
    layout->addWidget(columnsGroupBox);
    layout->addStretch();

    return widget;
}

/**
 * update the row and column stretching widgets.
 * 
 * @param configuration
 *     Current custom configuration.
 */
void
TileTabsConfigurationDialog::updateRowColumnStretchWidgets(TileTabsLayoutGridConfiguration* configuration)
{
    /*
     * Update rows
     */
    {
        const int32_t numRows  = configuration->getNumberOfRows();
        int32_t numRowElements = static_cast<int32_t>(m_rowElements.size());
        
        /**
         * Add elements as needed.
         */
        const int32_t numToAdd = numRows - numRowElements;
        for (int32_t iRow = 0; iRow < numToAdd; iRow++) {
            addRowColumnStretchWidget(EventTileTabsGridConfigurationModification::RowColumnType::ROW,
                                      m_rowElementsGridLayout,
                                      m_rowElements);
        }
        
        /*
         * Update widgets with element content
         */
        numRowElements = static_cast<int32_t>(m_rowElements.size());
        for (int32_t iRow = 0; iRow < numRowElements; iRow++) {
            TileTabsGridRowColumnElement* element(NULL);
            if (iRow < numRows) {
                element = configuration->getRow(iRow);
            }
            
            CaretAssertVectorIndex(m_rowElements, iRow);
            m_rowElements[iRow]->updateContent(element);
        }
        
        m_rowElementsGridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    }
    
    /*
     * Update columns
     */
    {
        const int32_t numColumns  = configuration->getNumberOfColumns();
        int32_t numColumnElements = static_cast<int32_t>(m_columnElements.size());
        
        /**
         * Add elements as needed.
         */
        const int32_t numToAdd = numColumns - numColumnElements;
        for (int32_t iColumn = 0; iColumn < numToAdd; iColumn++) {
            addRowColumnStretchWidget(EventTileTabsGridConfigurationModification::RowColumnType::COLUMN,
                                      m_columnElementsGridLayout,
                                      m_columnElements);
        }
        
        /*
         * Update widgets with element content
         */
        numColumnElements = static_cast<int32_t>(m_columnElements.size());
        for (int32_t iColumn = 0; iColumn < numColumnElements; iColumn++) {
            TileTabsGridRowColumnElement* element(NULL);
            if (iColumn < numColumns) {
                element = configuration->getColumn(iColumn);
            }
            
            CaretAssertVectorIndex(m_columnElements, iColumn);
            m_columnElements[iColumn]->updateContent(element);
        }

        m_columnElementsGridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    }
    
    updateCustomOptionsWidget();
}

/**
 * Add a row/column stretch widget.
 *
 * @param rowColumnType
 *     The row or column type.
 * @param gridLayout
 *     Grid layout for widgets.
 * @param elementVector
 *     Container for row/column elements.
 */
void
TileTabsConfigurationDialog::addRowColumnStretchWidget(const EventTileTabsGridConfigurationModification::RowColumnType rowColumnType,
                                                       QGridLayout* gridLayout,
                                                       std::vector<TileTabElementWidgets*>& elementVector)
{
    const int32_t index = static_cast<int32_t>(elementVector.size());
    if (index == 0) {
        int32_t columnIndex(0);
        int32_t row = gridLayout->rowCount();
        gridLayout->addWidget(new QLabel("Index"), row, columnIndex++, Qt::AlignRight);
        gridLayout->addWidget(new QLabel(" "), row, columnIndex++);
        gridLayout->addWidget(new QLabel("Content"), row, columnIndex++, Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Type"), row, columnIndex++, Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Stretch"), row, columnIndex++, Qt::AlignHCenter);
        for (int32_t i = 0; i < columnIndex; i++) {
            gridLayout->setColumnStretch(i, 0);
        }
    }
    
    TileTabElementWidgets* elementWidget = new TileTabElementWidgets(this,
                                                                     rowColumnType,
                                                                     index,
                                                                     gridLayout,
                                                                     this);
    QObject::connect(elementWidget, &TileTabElementWidgets::itemChanged,
                     this, &TileTabsConfigurationDialog::configurationStretchFactorWasChanged);
    QObject::connect(elementWidget, &TileTabElementWidgets::modificationRequested,
                     this, &TileTabsConfigurationDialog::tileTabsModificationRequested);

    elementVector.push_back(elementWidget);
}


/**
 * @return The active configuration widget.
 */
QWidget*
TileTabsConfigurationDialog::createActiveConfigurationWidget()
{
    const AString autoToolTip("Workbench adjusts the number of rows and columns so "
                              "that all tabs are displayed");
    m_automaticConfigurationRadioButton = new QRadioButton("Automatic Configuration");
    m_automaticConfigurationRadioButton->setToolTip(WuQtUtilities::createWordWrappedToolTipText(autoToolTip));
    
    const AString customToolTip("User sets the number of row, columns, and stretch factors");
    m_customConfigurationRadioButton = new QRadioButton("Custom Configuration");
    m_customConfigurationRadioButton->setToolTip(WuQtUtilities::createWordWrappedToolTipText(customToolTip));
    
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(m_automaticConfigurationRadioButton);
    buttonGroup->addButton(m_customConfigurationRadioButton);
    QObject::connect(buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
                     this, &TileTabsConfigurationDialog::automaticCustomButtonClicked);
    
    QLabel* rowsLabel = new QLabel("Rows");
    m_numberOfRowsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                          s_maximumRowsColumns,
                                                                          1,
                                                                          this,
                                                                          SLOT(configurationNumberOfRowsOrColumnsChanged()));
    m_numberOfRowsSpinBox->setToolTip("Number of rows for the tab configuration");
    
    QLabel* columnsLabel = new QLabel("Columns");
    m_numberOfColumnsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                             s_maximumRowsColumns,
                                                                             1,
                                                                             this,
                                                                             SLOT(configurationNumberOfRowsOrColumnsChanged()));
    m_numberOfColumnsSpinBox->setToolTip("Number of columns for the tab configuration");
    
    m_customConfigurationWidget = createRowColumnStretchWidget();
    m_customConfigurationWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    m_customOptionsWidget = createCustomOptionsWidget();
    m_customOptionsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);


    QScrollArea* stretchFactorScrollArea = new QScrollArea();
    stretchFactorScrollArea->setWidget(m_customConfigurationWidget);
    stretchFactorScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    stretchFactorScrollArea->setWidgetResizable(true);
    
    QGroupBox* widget = new QGroupBox("Tile Tabs Configuration in Workbench Window");
    QGridLayout* widgetLayout = new QGridLayout(widget);
    widgetLayout->setColumnStretch(0, 0);
    widgetLayout->setColumnStretch(1, 0);
    widgetLayout->setColumnStretch(2, 0);
    widgetLayout->setColumnStretch(3, 0);
    widgetLayout->setColumnStretch(4, 0);
    widgetLayout->setColumnStretch(5, 100);
    widgetLayout->setColumnMinimumWidth(0, 20);
    widgetLayout->addWidget(createWorkbenchWindowWidget(),
                            0, 0, 1, 6,
                            Qt::AlignLeft);
    widgetLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(),
                            1, 0, 1, 6);
    widgetLayout->addWidget(m_automaticConfigurationRadioButton,
                            2, 0, 1, 6,
                            Qt::AlignLeft);
    widgetLayout->addWidget(m_customConfigurationRadioButton,
                            3, 0, 1, 1,
                            Qt::AlignLeft);
    widgetLayout->addWidget(rowsLabel,
                            3, 1, 1, 1);
    widgetLayout->addWidget(m_numberOfRowsSpinBox,
                            3, 2, 1, 1);
    widgetLayout->addWidget(columnsLabel,
                            3, 3, 1, 1);
    widgetLayout->addWidget(m_numberOfColumnsSpinBox,
                            3, 4, 1, 1);
    widgetLayout->addWidget(stretchFactorScrollArea,
                            4, 0, 1, 6);
    widgetLayout->addWidget(m_customOptionsWidget,
                            5, 0, 1, 6, Qt::AlignLeft);
    
    widget->setFixedWidth(widget->sizeHint().width());
    
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
 * Called when automatic/custom configuration radiobutton is clicked
 *
 * @param button
 *     New checked status of checkbox.
 */
void
TileTabsConfigurationDialog::automaticCustomButtonClicked(QAbstractButton* button)
{
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    if (button == m_automaticConfigurationRadioButton) {
        browserWindowContent->setTileTabsConfigurationMode(TileTabsGridModeEnum::AUTOMATIC);
    }
    else if (button == m_customConfigurationRadioButton) {
        browserWindowContent->setTileTabsConfigurationMode(TileTabsGridModeEnum::CUSTOM);
    }
    else {
        CaretAssert(0);
    }
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
    
    m_caretPreferences->readTileTabsUserConfigurations();
}

/**
 * Update the content of the dialog.
 */
void
TileTabsConfigurationDialog::updateDialog()
{
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    if (browserWindowContent == NULL) {
        return;
    }
    
    switch (browserWindowContent->getTileTabsConfigurationMode()) {
        case TileTabsGridModeEnum::AUTOMATIC:
            m_automaticConfigurationRadioButton->setChecked(true);
            break;
        case TileTabsGridModeEnum::CUSTOM:
            m_customConfigurationRadioButton->setChecked(true);
            break;
    }
    
    readConfigurationsFromPreferences();
    
    int defaultIndex = m_userConfigurationSelectionListWidget->currentRow();
    
    QSignalBlocker blocker(m_userConfigurationSelectionListWidget);
    m_userConfigurationSelectionListWidget->clear();
    
    std::vector<std::pair<AString, AString>> nameUniqueIDs =
    m_caretPreferences->getTileTabsUserConfigurationsNamesAndUniqueIdentifiers();
    
    for (const auto nameID : nameUniqueIDs) {
        /*
         * Second element is user data which contains the Unique ID
         */
        QListWidgetItem* item = new QListWidgetItem(nameID.first);
        item->setData(Qt::UserRole,
                      QVariant(nameID.second));
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
    BrainBrowserWindow* browserWindow = getBrowserWindow();
    m_automaticConfigurationRadioButton->setText(browserWindow->getTileTabsConfigurationLabelText(TileTabsGridModeEnum::AUTOMATIC,
                                                                                              true));
    m_customConfigurationRadioButton->setText(browserWindow->getTileTabsConfigurationLabelText(TileTabsGridModeEnum::CUSTOM,
                                                                                           false));
    const TileTabsLayoutGridConfiguration* configuration = getCustomTileTabsGridConfiguration();
    if (configuration != NULL) {
        updateRowColumnStretchWidgets(const_cast<TileTabsLayoutGridConfiguration*>(configuration));
        QSignalBlocker rowBlocker(m_numberOfRowsSpinBox);
        m_numberOfRowsSpinBox->setValue(configuration->getNumberOfRows());
        QSignalBlocker columnBlocker(m_numberOfColumnsSpinBox);
        m_numberOfColumnsSpinBox->setValue(configuration->getNumberOfColumns());
    }
    
    const bool editableFlag = ( ! m_automaticConfigurationRadioButton->isChecked());
    
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
 * Called when delete user configuration button is clicked.
 */
void
TileTabsConfigurationDialog::deleteUserConfigurationButtonClicked()
{
    const QString uniqueID = getSelectedUserTileTabsConfigurationUniqueIdentifier();
    const QString msg = ("Delete selected configuration ?");
    if (WuQMessageBox::warningYesNo(m_deleteConfigurationPushButton,
                                    msg)) {
        AString errorMessage;
        if ( ! m_caretPreferences->removeTileTabsUserConfigurationByUniqueIdentifier(uniqueID,
                                                                                     errorMessage)) {
            WuQMessageBox::errorOk(m_deleteConfigurationPushButton,
                                   errorMessage);
        }
        updateDialog();
    }
}

/**
 * Called when rename user configuration button is clicked.
 */
void
TileTabsConfigurationDialog::renameUserConfigurationButtonClicked()
{
    AString oldName;
    AString uniqueID;
    if (getSelectedUserConfigurationNameAndUniqueID(oldName,
                                                    uniqueID)) {
        bool ok = false;
        const AString newName = QInputDialog::getText(m_deleteConfigurationPushButton,
                                                      "Rename Configuration",
                                                      "Name",
                                                      QLineEdit::Normal,
                                                      oldName,
                                                      &ok);
        if (ok
            && ( ! newName.isEmpty())) {
            m_blockReadConfigurationsFromPreferences = true;
            AString errorMessage;
            if (m_caretPreferences->renameTileTabsUserConfiguration(uniqueID,
                                                                    newName,
                                                                    errorMessage)) {
            }
            else {
                WuQMessageBox::errorOk(m_renameConfigurationPushButton,
                                       errorMessage);
            }
            m_blockReadConfigurationsFromPreferences = false;
            updateDialog();
        }

    }
}

/**
 * @return A pointer to the custom tile tabs configuration.
 */
TileTabsLayoutGridConfiguration*
TileTabsConfigurationDialog::getCustomTileTabsGridConfiguration()
{
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    TileTabsLayoutGridConfiguration* configuration = browserWindowContent->getCustomGridTileTabsConfiguration();
    CaretAssert(configuration);
    return configuration;
}

/**
 * Get the name and unique identifier for the selected user configuration
 *
 * @param nameOut
 *     Output with name
 * @param uniqueIDOut
 *     Output with Unique ID
 * @return
 *     True if the output data is valid or false if no configuration is selected
 */
bool
TileTabsConfigurationDialog::getSelectedUserConfigurationNameAndUniqueID(AString& nameOut,
                                                                         AString& uniqueIDOut) const
{
    const int32_t indx = m_userConfigurationSelectionListWidget->currentRow();
    if ((indx >= 0)
        && (indx < m_userConfigurationSelectionListWidget->count())) {
        QListWidgetItem* item = m_userConfigurationSelectionListWidget->item(indx);
        nameOut = item->text();
        uniqueIDOut = item->data(Qt::UserRole).toString();
        
        return true;
    }
    
    nameOut.clear();
    uniqueIDOut.clear();
    
    return false;
}

/**
 * @return The Unique Identifier of the selected user tile tabs configuration.
 * Empty if no configuration is selected.
 */
AString
TileTabsConfigurationDialog::getSelectedUserTileTabsConfigurationUniqueIdentifier()
{
    AString uniqueID;
    
    const int32_t indx = m_userConfigurationSelectionListWidget->currentRow();
    if ((indx >= 0)
        && (indx < m_userConfigurationSelectionListWidget->count())) {
        QListWidgetItem* item = m_userConfigurationSelectionListWidget->item(indx);
        uniqueID = item->data(Qt::UserRole).toString();
    }
    
    return uniqueID;
}

/**
 * Called when the number of rows or columns changes.
 */
void
TileTabsConfigurationDialog::configurationNumberOfRowsOrColumnsChanged()
{
    TileTabsLayoutGridConfiguration* configuration = getCustomTileTabsGridConfiguration();
    if (configuration != NULL) {
        configuration->setNumberOfRows(m_numberOfRowsSpinBox->value());
        configuration->setNumberOfColumns(m_numberOfColumnsSpinBox->value());
        
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
    TileTabsLayoutGridConfiguration* configuration = getCustomTileTabsGridConfiguration();
    if (configuration == NULL) {
        return;
    }
    
    updateStretchFactors();
    updateGraphicsWindow();
}

/**
 * Called when a tile tabs configuration modification is requested
 *
 * @param modification
 *     Modification that is requested.
 */
void
TileTabsConfigurationDialog::tileTabsModificationRequested(EventTileTabsGridConfigurationModification& modification)
{
    TileTabsLayoutGridConfiguration* configuration = getCustomTileTabsGridConfiguration();
    if (configuration != NULL) {
        
        modification.setWindowIndex(m_browserWindowComboBox->getSelectedBrowserWindowIndex());
        
        EventManager::get()->sendEvent(modification.getPointer());
        
        updateStretchFactors();
        
        updateGraphicsWindow();
    }
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
        EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
        EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(windowIndex).getPointer());
    }
}

/**
 * Called when help button is clicked.
 */
void
TileTabsConfigurationDialog::helpButtonClicked()
{
    EventHelpViewerDisplay helpViewerEvent(getBrowserWindow(),
                                           "Tile_Tabs_Configuration");
    EventManager::get()->sendEvent(helpViewerEvent.getPointer());
}


/**
 * Constructor.
 *
 * @param tileTabsConfigurationDialog
 *    The tile tabs configuration dialog.
 * @param rowColumnType
 *    'Row' or 'Column'
 * @param index
 *    Index of the row/column
 * @param gridLayout
 *    Gridlayout for widgets
 * @param parent
 *    Parent QObject
 */
TileTabElementWidgets::TileTabElementWidgets(TileTabsConfigurationDialog* tileTabsConfigurationDialog,
                                             const EventTileTabsGridConfigurationModification::RowColumnType rowColumnType,
                                             const int32_t index,
                                             QGridLayout* gridLayout,
                                             QObject* parent)
: QObject(parent),
m_tileTabsConfigurationDialog(tileTabsConfigurationDialog),
m_rowColumnType(rowColumnType),
m_index(index),
m_element(NULL)
{
    m_indexLabel = new QLabel(QString::number(m_index + 1));
    m_indexLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    const AString rowColText((rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::ROW)
                             ? "Row"
                             : "Column");
    const AString contructionToolTip(WuQtUtilities::createWordWrappedToolTipText("Delete, Duplicate, or Move "
                                                                                 + rowColText));
    const AString contentToolTip(WuQtUtilities::createWordWrappedToolTipText("Content of the "
                                                                          + rowColText
                                                                             + ": Spacer (empty space for Annotations) "
                                                                             "or Tabs (Browser Tabs)"));
    const AString typeToolTip(WuQtUtilities::createWordWrappedToolTipText("Type of Stretching: Percent or Weight"));
    const AString stretchToolTip(WuQtUtilities::createWordWrappedToolTipText("Value of Stretching Percentage [0, 100] or Stretching Weight"));
    
    /*
     * Construction Tool Button
     */
    QIcon constructionIcon;
    const bool constructionIconValid = WuQtUtilities::loadIcon(":/LayersPanel/construction.png",
                                                               constructionIcon);
    m_constructionAction = WuQtUtilities::createAction("M",
                                                       "Add/Move/Remove",
                                                       this);
    if (constructionIconValid) {
        m_constructionAction->setIcon(constructionIcon);
    }
    m_constructionToolButton = new QToolButton();
    QMenu* constructionMenu = createConstructionMenu(m_constructionToolButton);
    QObject::connect(constructionMenu, &QMenu::aboutToShow,
                     this, &TileTabElementWidgets::constructionMenuAboutToShow);
    QObject::connect(constructionMenu, &QMenu::triggered,
                     this, &TileTabElementWidgets::constructionMenuTriggered);
    m_constructionAction->setMenu(constructionMenu);
    m_constructionToolButton->setDefaultAction(m_constructionAction);
    m_constructionToolButton->setPopupMode(QToolButton::InstantPopup);
    m_constructionToolButton->setFixedWidth(m_constructionToolButton->sizeHint().width());
    m_constructionToolButton->setToolTip(contructionToolTip);
    
    /*
     * Content type combo box
     */
    m_contentTypeComboBox = new EnumComboBoxTemplate(this);
    m_contentTypeComboBox->setup<TileTabsGridRowColumnContentTypeEnum, TileTabsGridRowColumnContentTypeEnum::Enum>();
    QObject::connect(m_contentTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &TileTabElementWidgets::contentTypeActivated);
    m_contentTypeComboBox->getComboBox()->setFixedWidth(m_contentTypeComboBox->getComboBox()->sizeHint().width());
    m_contentTypeComboBox->getComboBox()->setToolTip(contentToolTip);
    
    /*
     * Stretch type combo box
     */
    m_stretchTypeComboBox = new EnumComboBoxTemplate(this);
    m_stretchTypeComboBox->setup<TileTabsGridRowColumnStretchTypeEnum, TileTabsGridRowColumnStretchTypeEnum::Enum>();
    QObject::connect(m_stretchTypeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &TileTabElementWidgets::stretchTypeActivated);
    m_stretchTypeComboBox->getComboBox()->setFixedWidth(m_stretchTypeComboBox->getComboBox()->sizeHint().width());
    m_stretchTypeComboBox->getComboBox()->setToolTip(typeToolTip);
    
    /*
     * Stretch value spin box
     */
    m_stretchValueSpinBox = new QDoubleSpinBox();
    m_stretchValueSpinBox->setKeyboardTracking(false);
    m_stretchValueSpinBox->setRange(0.0, 1000.0);
    m_stretchValueSpinBox->setDecimals(2);
    m_stretchValueSpinBox->setSingleStep(0.1);
    QObject::connect(m_stretchValueSpinBox, static_cast<void (QDoubleSpinBox::*)(double)>(&QDoubleSpinBox::valueChanged),
                     this, &TileTabElementWidgets::stretchValueChanged);
    m_stretchValueSpinBox->setFixedWidth(m_stretchValueSpinBox->sizeHint().width());
    m_stretchValueSpinBox->setToolTip(stretchToolTip);
    
    m_gridLayoutGroup = new WuQGridLayoutGroup(gridLayout);
    const int32_t rowIndex(gridLayout->rowCount());
    int32_t columnIndex(0);
    m_gridLayoutGroup->addWidget(m_indexLabel, rowIndex, columnIndex++, Qt::AlignRight);
    m_gridLayoutGroup->addWidget(m_constructionToolButton, rowIndex, columnIndex++);
    m_gridLayoutGroup->addWidget(m_contentTypeComboBox->getWidget(), rowIndex, columnIndex++);
    m_gridLayoutGroup->addWidget(m_stretchTypeComboBox->getWidget(), rowIndex, columnIndex++);
    m_gridLayoutGroup->addWidget(m_stretchValueSpinBox, rowIndex, columnIndex++);
}

/**
 * Destructor.
 */
TileTabElementWidgets::~TileTabElementWidgets()
{
    
}

/**
 * @return The construction menu.
 *
 * @param toolButton
 *    The parent toolbutton.
 */
QMenu*
TileTabElementWidgets::createConstructionMenu(QToolButton* toolButton)
{
    const AString deleteText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                             ? "Delete this Column"
                             : "Delete this Row");
    
    const AString duplicateAfterText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                             ? "Duplicate this Column to Right"
                             : "Duplicate this Row Below");
    
    const AString duplicateBeforeText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                             ? "Duplicate this Column to Left"
                             : "Duplicate this Row Above");

    const AString insertSpacerAfterText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                                         ? "Insert Spacer Column to Right"
                                         : "Insert Spacer Row Below");
    const AString insertSpacerBeforeText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                                         ? "Insert Spacer Column to Left"
                                         : "Insert Spacer Row Above");
    const AString moveAfterText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                             ? "Move this Column to Right"
                             : "Move this Row Down");
    
    const AString moveBeforeText((m_rowColumnType == EventTileTabsGridConfigurationModification::RowColumnType::COLUMN)
                             ? "Move this Column to Left"
                             : "Move this Row Up");
    
    m_menuDeleteAction = new QAction(deleteText);
    m_menuDeleteAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::DELETE_IT));
    
    m_menuDuplicateAfterAction = new QAction(duplicateAfterText);
    m_menuDuplicateAfterAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::DUPLICATE_AFTER));
    
    m_menuDuplicateBeforeAction = new QAction(duplicateBeforeText);
    m_menuDuplicateBeforeAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::DUPLICATE_BEFORE));
    
    m_insertSpacerAfterAction = new QAction(insertSpacerAfterText);
    m_insertSpacerAfterAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_AFTER));

    m_insertSpacerBeforeAction = new QAction(insertSpacerBeforeText);
    m_insertSpacerBeforeAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_BEFORE));
    
    m_menuMoveAfterAction = new QAction(moveAfterText);
    m_menuMoveAfterAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::MOVE_AFTER));
    
    m_menuMoveBeforeAction = new QAction(moveBeforeText);
    m_menuMoveBeforeAction->setData(static_cast<int>(EventTileTabsGridConfigurationModification::Operation::MOVE_BEFORE));
    
    QMenu* menu = new QMenu(toolButton);
    menu->addAction(m_menuDuplicateBeforeAction);
    menu->addAction(m_menuDuplicateAfterAction);
    menu->addSeparator();
    menu->addAction(m_insertSpacerBeforeAction);
    menu->addAction(m_insertSpacerAfterAction);
    menu->addSeparator();
    menu->addAction(m_menuMoveBeforeAction);
    menu->addAction(m_menuMoveAfterAction);
    menu->addSeparator();
    menu->addAction(m_menuDeleteAction);
    
    return menu;
}

/**
 * Update with the given row/column element.
 */
void
TileTabElementWidgets::updateContent(TileTabsGridRowColumnElement* element)
{
    m_element = element;
    const bool showFlag(m_element != NULL);
    
    if (showFlag) {
        m_contentTypeComboBox->setSelectedItem<TileTabsGridRowColumnContentTypeEnum, TileTabsGridRowColumnContentTypeEnum::Enum>(element->getContentType());
        m_stretchTypeComboBox->setSelectedItem<TileTabsGridRowColumnStretchTypeEnum, TileTabsGridRowColumnStretchTypeEnum::Enum>(element->getStretchType());
        QSignalBlocker valueBlocker(m_stretchValueSpinBox);
        switch (m_element->getStretchType()) {
            case TileTabsGridRowColumnStretchTypeEnum::PERCENT:
                m_stretchValueSpinBox->setRange(0.0, 100.0);
                m_stretchValueSpinBox->setSingleStep(1.0);
                m_stretchValueSpinBox->setValue(m_element->getPercentStretch());
                m_stretchValueSpinBox->setSuffix("%");
                break;
            case TileTabsGridRowColumnStretchTypeEnum::WEIGHT:
                m_stretchValueSpinBox->setRange(0.0, 1000.0);
                m_stretchValueSpinBox->setSingleStep(0.1);
                m_stretchValueSpinBox->setValue(m_element->getWeightStretch());
                m_stretchValueSpinBox->setSuffix("");
                break;
        }
    }
    
    m_gridLayoutGroup->setVisible(showFlag);
}

/**
 * Called when an item is selected from the construction menu
 *
 * @param action
 *     Action that was selected.
 */
void
TileTabElementWidgets::constructionMenuTriggered(QAction* action)
{
    if (action != NULL) {
        const EventTileTabsGridConfigurationModification::Operation operation
            = static_cast<EventTileTabsGridConfigurationModification::Operation>(action->data().toInt());
        
        /*
         * This switch is here so that it will cause a compilation error
         * if the operations are changed.
         */
        switch (operation) {
            case EventTileTabsGridConfigurationModification::Operation::DELETE_IT:
                break;
            case EventTileTabsGridConfigurationModification::Operation::DUPLICATE_AFTER:
                break;
            case EventTileTabsGridConfigurationModification::Operation::DUPLICATE_BEFORE:
                break;
            case EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_BEFORE:
                break;
            case EventTileTabsGridConfigurationModification::Operation::INSERT_SPACER_AFTER:
                break;
            case EventTileTabsGridConfigurationModification::Operation::MOVE_AFTER:
                break;
            case EventTileTabsGridConfigurationModification::Operation::MOVE_BEFORE:
                break;
        }
        
        EventTileTabsGridConfigurationModification modification(m_tileTabsConfigurationDialog->getCustomTileTabsGridConfiguration(),
                                                                m_index,
                                                                m_rowColumnType,
                                                                operation);
        emit modificationRequested(modification);
    }
    
}

/**
 * Called when construction menu is about to show.
 */
void
TileTabElementWidgets::constructionMenuAboutToShow()
{
    const TileTabsLayoutGridConfiguration* config = m_tileTabsConfigurationDialog->getCustomTileTabsGridConfiguration();
    if (config != NULL) {
        int32_t numItems(-1);
        switch (m_rowColumnType) {
            case EventTileTabsGridConfigurationModification::RowColumnType::COLUMN:
                numItems = config->getNumberOfColumns();
                break;
            case EventTileTabsGridConfigurationModification::RowColumnType::ROW:
                numItems = config->getNumberOfRows();
                break;
        }
        
        m_menuDeleteAction->setEnabled(numItems > 1);
        m_menuDuplicateAfterAction->setEnabled(numItems >= 1);
        m_menuDuplicateBeforeAction->setEnabled(numItems >= 1);
        m_menuMoveAfterAction->setEnabled((numItems > 1)
                                          && (m_index < (numItems - 1)));
        m_menuMoveBeforeAction->setEnabled((numItems > 1)
                                           && (m_index > 0));
    }
}


/**
 * Called when content type combo box changed.
 */
void
TileTabElementWidgets::contentTypeActivated()
{
    if (m_element != NULL) {
        m_element->setContentType(m_contentTypeComboBox->getSelectedItem<TileTabsGridRowColumnContentTypeEnum, TileTabsGridRowColumnContentTypeEnum::Enum>());
        emit itemChanged();
    }
}

/**
 * Called when stretch type combo box changed.
 */
void
TileTabElementWidgets::stretchTypeActivated()
{
    if (m_element != NULL) {
        m_element->setStretchType(m_stretchTypeComboBox->getSelectedItem<TileTabsGridRowColumnStretchTypeEnum, TileTabsGridRowColumnStretchTypeEnum::Enum>());
        emit itemChanged();
    }
}

/**
 * Called when stretch value changed.
 */
void
TileTabElementWidgets::stretchValueChanged(double)
{
    if (m_element != NULL) {
        switch (m_element->getStretchType()) {
            case TileTabsGridRowColumnStretchTypeEnum::PERCENT:
                m_element->setPercentStretch(m_stretchValueSpinBox->value());
                break;
            case TileTabsGridRowColumnStretchTypeEnum::WEIGHT:
                m_element->setWeightStretch(m_stretchValueSpinBox->value());
                break;
        }
        emit itemChanged();
    }
}


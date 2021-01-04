
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

#include <algorithm>

#include <QButtonGroup>
#include <QCheckBox>
#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QToolButton>
#include <QVBoxLayout>


#define __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__
#include "TileTabsConfigurationDialog.h"
#undef __TILE_TABS_CONFIGURATION_DIALOG_DECLARE__

#include "AnnotationBrowserTab.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowComboBox.h"
#include "BrainOpenGLViewportContent.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserTabDeleteInToolBar.h"
#include "EventBrowserTabNewInGUI.h"
#include "EventBrowserWindowGraphicsRedrawn.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventHelpViewerDisplay.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "MathFunctions.h"
#include "SessionManager.h"
#include "TileTabsBrowserTabGeometry.h"
#include "TileTabsLayoutGridConfiguration.h"
#include "TileTabsLayoutManualConfiguration.h"
#include "TileTabsManualTabGeometryWidget.h"
#include "TileTabsGridRowColumnElement.h"
#include "TileTabGridRowColumnWidgets.h"
#include "WuQDataEntryDialog.h"
#include "WuQFactory.h"
#include "WuQGridLayoutGroup.h"
#include "WuQDataEntryDialog.h"
#include "WuQListWidget.h"
#include "WuQMessageBox.h"
#include "WuQTextEditorDialog.h"
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
    m_blockReadUserConfigurationsFromPreferences = false;
    m_caretPreferences = SessionManager::get()->getCaretPreferences();
    
    QWidget* workbenchWindowWidget = createWorkbenchWindowWidget();
    
    
    setApplyButtonText("");
    setStandardButtonText(QDialogButtonBox::Help,
                          "Help");
    
    QWidget* dialogWidget = new QWidget();
    QGridLayout* dialogLayout = new QGridLayout(dialogWidget);
    dialogLayout->setColumnStretch(0, 0);
    dialogLayout->setColumnStretch(1, 0);
    dialogLayout->setColumnStretch(2, 100);
    dialogLayout->addWidget(workbenchWindowWidget,
                            0, 0, Qt::AlignLeft);
    dialogLayout->addWidget(createConfigurationTypeWidget(),
                            1, 0);
    dialogLayout->addWidget(createConfigurationSettingsWidget(),
                            2, 0, 1, 2);
    dialogLayout->addWidget(createCopyLoadPushButtonsWidget(),
                            0, 1, 2, 1, Qt::AlignBottom);
    dialogLayout->addWidget(createUserConfigurationSelectionWidget(),
                            0, 2, 3, 1);

    setCentralWidget(dialogWidget,
                     WuQDialog::SCROLL_AREA_NEVER);
    updateDialogWithSelectedTileTabsFromWindow(parentBrainBrowserWindow);
    
    resize(900,
           500);
    
    disableAutoDefaultForAllPushButtons();

    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_GRAPHICS_HAVE_BEEN_REDRAWN);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_BROWSER_WINDOW_MENUS_UPDATE);
    
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
    else if (event->getEventType() == EventTypeEnum::EVENT_BROWSER_WINDOW_MENUS_UPDATE) {
        updateTemplateUserConfigurationPushButtons(getSelectedConfigurationSourceType());
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
    m_addConfigurationPushButton     = new QPushButton("Add -->");
    m_addConfigurationPushButton->setAutoDefault(false);
    WuQtUtilities::setWordWrappedToolTip(m_addConfigurationPushButton,
                                         "Create a new User Configuration containg the Configuration Settings.  "
                                         "A dialog is displayed to enter the name.");
    QObject::connect(m_addConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(addUserConfigurationPushButtonClicked()));

    m_replaceConfigurationPushButton = new QPushButton("Replace -->");
    m_replaceConfigurationPushButton->setAutoDefault(false);
    WuQtUtilities::setWordWrappedToolTip(m_replaceConfigurationPushButton,
                                         "Replace the User Configuration with the Configuration Settings.");
    QObject::connect(m_replaceConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(replaceUserConfigurationPushButtonClicked()));
    
    m_loadConfigurationPushButton = new QPushButton("<-- Load");
    m_loadConfigurationPushButton->setAutoDefault(false);
    WuQtUtilities::setWordWrappedToolTip(m_loadConfigurationPushButton,
                                         "Load the selected User Configuration into the Configuration Type and Settings.");
    QObject::connect(m_loadConfigurationPushButton, SIGNAL(clicked()),
                     this, SLOT(loadIntoActiveConfigurationPushButtonClicked()));

    WuQtUtilities::matchWidgetWidths(m_addConfigurationPushButton,
                                     m_replaceConfigurationPushButton,
                                     m_loadConfigurationPushButton);
    
    QWidget* widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_addConfigurationPushButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_replaceConfigurationPushButton, 0, Qt::AlignHCenter);
    layout->addSpacing(10);
    layout->addWidget(m_loadConfigurationPushButton, 0, Qt::AlignHCenter);
    
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
    const AString newConfigName = getNewConfigurationName(m_addConfigurationPushButton);
    if (newConfigName.isEmpty()) {
        return;
    }
    
    m_blockReadUserConfigurationsFromPreferences = true;
    
    switch (getBrowserWindowContent()->getTileTabsConfigurationMode()) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            CaretAssert(0);
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
        {
            const TileTabsLayoutGridConfiguration* activeConfiguration = getCustomTileTabsGridConfiguration();
            CaretAssert(activeConfiguration);
            
            m_caretPreferences->addTileTabsUserConfiguration(activeConfiguration,
                                                             newConfigName);
        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
        {
            TileTabsLayoutManualConfiguration* manualConfig = createManualConfigurationFromCurrentTabs();
            if (manualConfig != NULL) {
                m_caretPreferences->addTileTabsUserConfiguration(manualConfig,
                                                                 newConfigName);
                delete manualConfig;
            }
            else {
                WuQMessageBox::errorOk(m_addConfigurationPushButton,
                                       "Tile Tabs MUST be enabled.");
            }
        }
            break;
    }
    
    m_blockReadUserConfigurationsFromPreferences = false;
    updateDialog();
}

/**
 * Called when Replace to user configuration pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::replaceUserConfigurationPushButtonClicked()
{
    CaretAssert(getSelectedConfigurationSourceType() == ConfigurationSourceTypeEnum::USER);
    
    const AString tileTabsUniqueID = getSelectedUserTileTabsConfigurationUniqueIdentifier();
    if (tileTabsUniqueID.isEmpty()) {
            WuQMessageBox::errorOk(this,
                                   "There are no user configurations, use Add button.");
            return;
    }
    else {
        const AString msg("Do you want to replace the configuration?");
        if ( ! WuQMessageBox::warningOkCancel(m_replaceConfigurationPushButton,
                                              msg)) {
            m_blockReadUserConfigurationsFromPreferences = false;
            return;
        }
    }
    
    m_blockReadUserConfigurationsFromPreferences = true;
    
    switch (getBrowserWindowContent()->getTileTabsConfigurationMode()) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            CaretAssert(0);
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
        {
            const TileTabsLayoutGridConfiguration* activeConfiguration = getCustomTileTabsGridConfiguration();
            CaretAssert(activeConfiguration);
            
            AString errorMessage;
            if ( ! m_caretPreferences->replaceTileTabsUserConfiguration(tileTabsUniqueID,
                                                                        activeConfiguration,
                                                                        errorMessage)) {
                WuQMessageBox::errorOk(m_replaceConfigurationPushButton,
                                       errorMessage);
            }
        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
        {
            TileTabsLayoutManualConfiguration* manualConfig = createManualConfigurationFromCurrentTabs();
            if (manualConfig != NULL) {
                AString errorMessage;
                if ( ! m_caretPreferences->replaceTileTabsUserConfiguration(tileTabsUniqueID,
                                                                            manualConfig,
                                                                            errorMessage)) {
                    WuQMessageBox::errorOk(m_replaceConfigurationPushButton,
                                           errorMessage);
                }
                delete manualConfig;
            }
            else {
                WuQMessageBox::errorOk(m_addConfigurationPushButton,
                                       "Tile Tabs MUST be enabled.");
            }
        }
            break;
    }

    m_blockReadUserConfigurationsFromPreferences = false;
    updateDialog();
}

/**
 * Called when Load pushbutton is clicked.
 */
void
TileTabsConfigurationDialog::loadIntoActiveConfigurationPushButtonClicked()
{
    std::unique_ptr<TileTabsLayoutBaseConfiguration> configuration;
    switch (getSelectedConfigurationSourceType()) {
        case TEMPLATE:
        {
            configuration = getSelectedTemplateConfiguration();
        }
            break;
        case USER:
        {
            const AString userConfigID = getSelectedUserTileTabsConfigurationUniqueIdentifier();
            if (userConfigID.isEmpty()) {
                WuQMessageBox::errorOk(m_loadConfigurationPushButton, "No user configuration is selected.");
                return;
            }
            
            configuration = m_caretPreferences->getCopyOfTileTabsUserConfigurationByUniqueIdentifier(userConfigID);
            if ( ! configuration) {
                WuQMessageBox::errorOk(m_loadConfigurationPushButton,
                                       ("User configuration with UniqueID="
                                        + userConfigID
                                        + " was not found"));
                return;
            }
        }
            break;
    }
    
    if ( ! configuration) {
        return;
    }
    
    switch (configuration->getLayoutType()) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            CaretAssert(0);
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
        {
            TileTabsLayoutGridConfiguration* customGridConfiguration = getCustomTileTabsGridConfiguration();
            CaretAssert(customGridConfiguration);
            
            const TileTabsLayoutGridConfiguration* userGridConfig = configuration->castToGridConfiguration();
            CaretAssert(userGridConfig);
            
            if ( ! warnIfGridConfigurationTooSmallDialog(userGridConfig)) {
                return;
            }

            customGridConfiguration->copy(*userGridConfig);
            
            getBrowserWindowContent()->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID);
        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
        {
            const TileTabsLayoutManualConfiguration* userManualConfig = configuration->castToManualConfiguration();
            CaretAssert(userManualConfig);
            
            loadIntoManualConfiguration(userManualConfig);
            
            getBrowserWindowContent()->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::MANUAL);
        }
            break;
    }
    
    updateDialog();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    updateGraphicsWindow();
}

/*
 * Load the given configuration into the manual configuration
 *
 * @param configuration
 *     Configuration that is loaded into the manual configuration
 * @return
 *     True if the configuration was loaded or false if user cancelled
 */
bool
TileTabsConfigurationDialog::loadIntoManualConfiguration(const TileTabsLayoutBaseConfiguration* configuration)
{
    CaretAssert(configuration);
    
    std::vector<BrowserTabContent*> allTabContent;
    getBrowserWindow()->getAllTabContent(allTabContent);
    int32_t numBrowserTabs = static_cast<int32_t>(allTabContent.size());

    std::vector<int32_t> tabIndices;
    for (auto tc : allTabContent) {
        CaretAssert(tc);
        tabIndices.push_back(tc->getTabNumber());
    }

    std::unique_ptr<TileTabsLayoutManualConfiguration> manualConfiguration;
    
    switch (configuration->getLayoutType()) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
        {
            const TileTabsLayoutGridConfiguration* gridConfig = configuration->castToGridConfiguration();
            CaretAssert(gridConfig);
            
            manualConfiguration.reset(TileTabsLayoutManualConfiguration::newInstanceFromGridLayout(const_cast<TileTabsLayoutGridConfiguration*>(gridConfig),
                                                                                                   gridConfig->getLayoutType(),
                                                                                                   tabIndices));
        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
        {
            const TileTabsLayoutGridConfiguration* gridConfig = configuration->castToGridConfiguration();
            CaretAssert(gridConfig);
            manualConfiguration.reset(TileTabsLayoutManualConfiguration::newInstanceFromGridLayout(const_cast<TileTabsLayoutGridConfiguration*>(gridConfig),
                                                                                                   gridConfig->getLayoutType(),
                                                                                                   tabIndices));

        }
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
        {
            const TileTabsLayoutManualConfiguration* manConfig = configuration->castToManualConfiguration();
            CaretAssert(manConfig);
            manualConfiguration.reset(manConfig->newCopyWithNewUniqueIdentifier()->castToManualConfiguration());
        }
    }
    
    CaretAssert(manualConfiguration);
    const int32_t numberOfConfigurationTabs = manualConfiguration->getNumberOfTabs();
    if (numberOfConfigurationTabs <= 0) {
        WuQMessageBox::errorOk(m_loadConfigurationPushButton,
                               ("User configuration \""
                                + configuration->getName()
                                + "\" is invalid (does not have any tabs)"));
        return false;
    }
    
    /*
     * Display status of tabs must be set after
     * configuration is copied from User Configurations
     */
    std::set<BrowserTabContent*> tabsWithDisplayStatusOn;
    std::set<BrowserTabContent*> tabsWithDisplayStatusOff;
    
    if (numBrowserTabs == numberOfConfigurationTabs) {
        /* OK */
    }
    else {
        AString msg("<html>"
                    "The Window contains "
                    + AString::number(numBrowserTabs)
                    + " tabs.<br>"
                    "The Layout has space for "
                    + AString::number(numberOfConfigurationTabs)
                    + " tabs"
                    "<p>");
        if (numBrowserTabs < numberOfConfigurationTabs) {
            msg.append("Do you want to add tabs to fill layout?:</html>");
            WuQDataEntryDialog dialog("Load Configuration",
                                      m_loadConfigurationPushButton,
                                      WuQDialogNonModal::SCROLL_AREA_NEVER);
            dialog.setTextAtTop(msg, true);
            QRadioButton* yesRadioButton = dialog.addRadioButton("Yes, add tabs to fill layout");
            QRadioButton* yesDisableDisplayRadioButton = dialog.addRadioButton("Yes, add tabs but disable display of them");
            QRadioButton* noRadioButton = dialog.addRadioButton("No, discard extra spaces in layout");
            yesDisableDisplayRadioButton->setChecked(true);
            if (dialog.exec() == WuQDataEntryDialog::Accepted) {
                bool addTabsFlag(false);
                bool showTabsFlag(false);
                if (yesRadioButton->isChecked()) {
                    addTabsFlag  = true;
                    showTabsFlag = true;
                }
                else if (yesDisableDisplayRadioButton->isChecked()) {
                    addTabsFlag = true;
                }
                else if (noRadioButton->isChecked()) {
                    /* No action needed */
                }
                else {
                    CaretAssert(0);
                }
                
                if (addTabsFlag) {
                    const int32_t numTabsToAdd = numberOfConfigurationTabs - numBrowserTabs;
                    for (int32_t i = 0; i < numTabsToAdd; i++) {
                        EventBrowserTabNewInGUI newTabEvent;
                        EventManager::get()->sendEvent(newTabEvent.getPointer());
                        BrowserTabContent* btc = newTabEvent.getBrowserTab();
                        updateGraphicsWindow();
                        if (btc != NULL) {
                            if (showTabsFlag) {
                                tabsWithDisplayStatusOn.insert(btc);
                            }
                            else {
                                tabsWithDisplayStatusOff.insert(btc);
                            }
                        }
                    }
                }
            }
            else {
                return false;
            }
        }
        else {
            msg.append("Do you want to:</html>");
            
            WuQDataEntryDialog dialog("Load Configuration",
                                      m_loadConfigurationPushButton,
                                      WuQDialogNonModal::SCROLL_AREA_NEVER);
            dialog.setTextAtTop(msg, true);
            QRadioButton* closeTabsRadioButton = dialog.addRadioButton("Close extra tabs");
            QRadioButton* expandLayoutRadioButton = dialog.addRadioButton("Expand layout to include tabs");
            expandLayoutRadioButton->setChecked(true);
            if (dialog.exec() == WuQDataEntryDialog::Accepted) {
                if (closeTabsRadioButton->isChecked()) {
                    const int32_t numTabsToDelete = numBrowserTabs - numberOfConfigurationTabs;
                    int32_t deleteIndex = numBrowserTabs - 1;
                    for (int32_t i = 0; i < numTabsToDelete; i++) {
                        CaretAssertVectorIndex(allTabContent, deleteIndex);
                        EventBrowserTabDeleteInToolBar deleteTabEvent(allTabContent[deleteIndex],
                                                                      allTabContent[deleteIndex]->getTabNumber());
                        EventManager::get()->sendEvent(deleteTabEvent.getPointer());
                        updateGraphicsWindow();
                        deleteIndex--;
                    }
                }
                else if (expandLayoutRadioButton->isChecked()) {
                    /* Position and size from within tab will be used */
                }
                else {
                    CaretAssert(0);
                }
            }
            else {
                return false;
            }
        }
    }
    
    /*
     * Get the tabs again since the number of tabs may have changed
     */
    allTabContent.clear();
    getBrowserWindow()->getAllTabContent(allTabContent);
    numBrowserTabs = static_cast<int32_t>(allTabContent.size());
    
    std::vector<const BrainOpenGLViewportContent*> tabViewports;
    getBrowserWindow()->getAllBrainOpenGLViewportContent(tabViewports);
    const int32_t numViewportContent = static_cast<int32_t>(tabViewports.size());
    
    for (int32_t i = 0; i < numBrowserTabs; i++) {
        CaretAssertVectorIndex(allTabContent, i);
        AnnotationBrowserTab* browserTabAnnotation = allTabContent[i]->getManualLayoutBrowserTabAnnotation();
        CaretAssert(browserTabAnnotation);
        
        if (i < numberOfConfigurationTabs) {
            
            const TileTabsBrowserTabGeometry* configGeometry =  manualConfiguration->getTabInfo(i);
            CaretAssert(configGeometry);
            
            browserTabAnnotation->setFromTileTabsGeometry(configGeometry);
            
            BrowserTabContent* btc = browserTabAnnotation->getBrowserTabContent();
            if (tabsWithDisplayStatusOn.find(btc) != tabsWithDisplayStatusOn.end()) {
                browserTabAnnotation->setBrowserTabDisplayed(true);
            }
            else if (tabsWithDisplayStatusOff.find(btc) != tabsWithDisplayStatusOff.end()) {
                browserTabAnnotation->setBrowserTabDisplayed(false);
            }
        }
        else {
            if (i < numViewportContent) {
                /*
                 * Since no geometry available from the user configuration,
                 * keep this tab in its current window position
                 */
                const BrainOpenGLViewportContent* tabViewportContent = getViewportContentForTab(allTabContent[i]->getTabNumber());
                if (tabViewportContent != NULL) {
                    int32_t tabViewport[4];
                    tabViewportContent->getTabViewportBeforeApplyingMargins(tabViewport);
                    int32_t windowViewport[4];
                    tabViewportContent->getWindowViewport(windowViewport);
                    const float tabX(tabViewport[0]);
                    const float tabY(tabViewport[1]);
                    const float tabWidth(tabViewport[2]);
                    const float tabHeight(tabViewport[3]);
                    const float windowWidth(windowViewport[2]);
                    const float windowHeight(windowViewport[3]);
                    tabViewportContent->getWindowViewport(windowViewport);
                    const float minX((tabX / windowWidth) * 100.0);
                    const float maxX(((tabX + tabWidth) / windowWidth) * 100.0);
                    const float minY((tabY / windowHeight) * 100.0);
                    const float maxY(((tabY + tabHeight) / windowHeight) * 100.0);
                    
                    browserTabAnnotation->setBounds2D(minX, maxX, minY, maxY);
                }
                else {
                    CaretLogWarning("Unable to find viewport content for tab number "
                                    + AString::number(allTabContent[i]->getTabNumber()));
                }
            }
        }
    }
    
    getBrowserWindowContent()->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::MANUAL);
    getBrowserWindowContent()->setWindowAnnotationsStackingOrder(manualConfiguration->getWindowAnnotationsStackingOrder());

    updateDialog();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    updateGraphicsWindow();
    
    return true;
}

/**
 * If the given grid configuration contains fewer tabs than the number of tabs in the window,
 * warn the user, with the option to continue.
 *
 * @param gridConfiguration
 *     The grid configuration
 * @return
 *     True if processing should continue (use the configuration), else false indicating cancel.
 */
bool
TileTabsConfigurationDialog::warnIfGridConfigurationTooSmallDialog(const TileTabsLayoutGridConfiguration* gridConfiguration) const
{
    CaretAssert(gridConfiguration);

    std::vector<BrowserTabContent*> allTabContent;
    getBrowserWindow()->getAllTabContent(allTabContent);
    int32_t numBrowserTabs = static_cast<int32_t>(allTabContent.size());
    
    const int32_t numConfigTabs = gridConfiguration->getNumberOfTabs();
    if (numConfigTabs <= 0) {
        WuQMessageBox::errorOk(m_loadConfigurationPushButton,
                               ("User configuration \""
                                + gridConfiguration->getName()
                                + "\" is invalid (does not have any tabs)"));
        return false;
    }
    
    if (gridConfiguration->getLayoutType() == TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID) {
        if (gridConfiguration->isCustomDefaultFlag()) {
            /* custom grid will get updated to show all tabs */
            return true;
        }
    }
    
    if (numConfigTabs < numBrowserTabs) {
        AString msg("<html>"
                    "The Window contains "
                    + AString::number(numBrowserTabs)
                    + " tabs.<p>"
                    "The Grid contains space for "
                    + AString::number(numConfigTabs)
                    + " tabs.<p>"
                    "If you continue, not all tabs will be visible but you can edit the configuration to view all tabs."
                    "</html>");
        return WuQMessageBox::warningOkCancel(m_loadConfigurationPushButton,
                                              msg);
    }

    return true;
}


/**
 * @return the BrainOpenGLViewportContent for the tab with the given index (NULL if not found)
 */
const BrainOpenGLViewportContent*
TileTabsConfigurationDialog::getViewportContentForTab(const int32_t tabIndex) const
{
    std::vector<const BrainOpenGLViewportContent*> tabViewports;
    getBrowserWindow()->getAllBrainOpenGLViewportContent(tabViewports);
    
    for (const auto tv : tabViewports) {
        if (tv->getTabIndex() == tabIndex) {
            return tv;
        }
    }
    
    return NULL;
}

/**
 * @return A manual configuration from the geometry of the current tabs.  If Tile Tabs
 * is NOT enabled, NULL is returned.
 */
TileTabsLayoutManualConfiguration*
TileTabsConfigurationDialog::createManualConfigurationFromCurrentTabs() const
{
    const BrainBrowserWindow* window = getBrowserWindow();
    std::vector<BrowserTabContent*> allTabContent;
    window->getAllTabContent(allTabContent);
    
    TileTabsLayoutManualConfiguration* manualConfig = new TileTabsLayoutManualConfiguration();
    for (const auto btc : allTabContent) {
        CaretAssert(btc);
        const AnnotationBrowserTab* browserTabAnnotation = btc->getManualLayoutBrowserTabAnnotation();
        CaretAssert(browserTabAnnotation);
        TileTabsBrowserTabGeometry* geometry = new TileTabsBrowserTabGeometry(browserTabAnnotation->getTabIndex());
        browserTabAnnotation->getTileTabsGeometry(geometry);
        manualConfig->addTabInfo(geometry);
    }

    manualConfig->setWindowAnnotationsStackingOrder(getBrowserWindowContent()->getWindowAnnotationsStackingOrder());
    
    return manualConfig;
}


/**
 * @return The browser window selected window index.
 */
const BrainBrowserWindow*
TileTabsConfigurationDialog::getBrowserWindow() const
{
    m_browserWindowComboBox->updateComboBox();
    /*
     * This can be NULL when wb_view is closing.
     */
    BrainBrowserWindow* bbw = m_browserWindowComboBox->getSelectedBrowserWindow();
    return bbw;
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
 * @return The browser window content for the selected window index.
 * May be NULL when no tabs are open.
 */
const BrowserWindowContent*
TileTabsConfigurationDialog::getBrowserWindowContent() const
{
    const BrowserWindowContent* bwc(NULL);
    const BrainBrowserWindow* bbw = getBrowserWindow();
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
    QObject::connect(m_userConfigurationSelectionListWidget, &QListWidget::itemSelectionChanged,
                     this, &TileTabsConfigurationDialog::userConfigurationSelectionListWidgetItemChanged);

    loadTemplateLayoutConfigurations();
    
    m_templateConfigurationSelectionListWidget = new WuQListWidget();
    m_templateConfigurationSelectionListWidget->setSelectionMode(QListWidget::SingleSelection);
    for (const auto& tc : m_templateLayoutConfigurations) {
        m_templateConfigurationSelectionListWidget->addItem(tc->getName()
                                                            + " ("
                                                            + AString::number(tc->getNumberOfTabs())
                                                            + ")");
    }
    if (m_templateConfigurationSelectionListWidget->count() > 0) {
        m_templateConfigurationSelectionListWidget->setCurrentRow(0);
    }
    QObject::connect(m_templateConfigurationSelectionListWidget, &QListWidget::itemSelectionChanged,
                     this, &TileTabsConfigurationDialog::templateConfigurationSelectionListWidgetItemChanged);

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

    m_showConfigurationXmlPushButton = new QPushButton("Show XML...");
    m_showConfigurationXmlPushButton->setToolTip("Show XML representation of User Configuration");
    m_showConfigurationXmlPushButton->setAutoDefault(false);
    QObject::connect(m_showConfigurationXmlPushButton, &QPushButton::clicked,
                     this, &TileTabsConfigurationDialog::showConfigurationXmlPushButtonClicked);
    
    WuQtUtilities::matchWidgetWidths(m_renameConfigurationPushButton,
                                     m_deleteConfigurationPushButton,
                                     m_showConfigurationXmlPushButton);
    
    QGridLayout* buttonsLayout = new QGridLayout();
    buttonsLayout->setContentsMargins(0, 0, 0, 0);
    buttonsLayout->addWidget(m_renameConfigurationPushButton,  0, 0);
    buttonsLayout->addWidget(m_deleteConfigurationPushButton,  0, 1);
    buttonsLayout->addWidget(m_showConfigurationXmlPushButton, 1, 0, 1, 2, Qt::AlignHCenter);
    
    QLabel* previewTextLabel = new QLabel("Configuration Preview");
    m_configurationImagePreviewLabel = new QLabel();
    m_configurationImagePreviewLabel->setScaledContents(true); /* scale pixmap in label to fill space */
    
    m_configurationSourceTabWidget = new QTabWidget();
    m_configurationSourceTemplateTabIndex = m_configurationSourceTabWidget->addTab(m_templateConfigurationSelectionListWidget,
                                                                         "Template");
    m_configurationSourceUserTabIndex = m_configurationSourceTabWidget->addTab(m_userConfigurationSelectionListWidget,
                                                                         "User");
    m_configurationSourceTabWidget->setCurrentIndex(m_configurationSourceUserTabIndex);
    QObject::connect(m_configurationSourceTabWidget, &QTabWidget::tabBarClicked,
                     this, &TileTabsConfigurationDialog::configurationSourceTabWidgetClicked);
    
    QGroupBox* configurationWidget = new QGroupBox("Configuration Library");
    QVBoxLayout* configurationLayout = new QVBoxLayout(configurationWidget);
    configurationLayout->addWidget(m_configurationSourceTabWidget,
                                   0);
    configurationLayout->addWidget(previewTextLabel);
    configurationLayout->addWidget(m_configurationImagePreviewLabel,
                                   0);
    configurationLayout->addLayout(buttonsLayout,
                                   0);
    
    return configurationWidget;
}

/**
 * Called when configuration source tab bar is clicked by user
 *
 * @param index
 *     Index of item in tab bar
 */
void
TileTabsConfigurationDialog::configurationSourceTabWidgetClicked(int index)
{
    /*
     * Note: This method is called before the tab widget updates
     * its internal's with the new index.  As a result, when
     * the selected tab changes, the index passed to this method
     * will be different than that returned by QTabWidget::currentIndex()
     */
    ConfigurationSourceTypeEnum sourceType = ConfigurationSourceTypeEnum::USER;
    if (index == m_configurationSourceUserTabIndex) {
        sourceType = ConfigurationSourceTypeEnum::USER;
        userConfigurationSelectionListWidgetItemChanged();
    }
    else if (index == m_configurationSourceTemplateTabIndex) {
        sourceType = ConfigurationSourceTypeEnum::TEMPLATE;
        templateConfigurationSelectionListWidgetItemChanged();
    }

    updateTemplateUserConfigurationPushButtons(sourceType);
}

/**
 * @return The selected configuration source type (template or user)
 */
TileTabsConfigurationDialog::ConfigurationSourceTypeEnum
TileTabsConfigurationDialog::getSelectedConfigurationSourceType() const
{
    if (m_configurationSourceUserTabIndex == m_configurationSourceTabWidget->currentIndex()) {
        return ConfigurationSourceTypeEnum::USER;
    }
    else if (m_configurationSourceTemplateTabIndex == m_configurationSourceTabWidget->currentIndex()) {
        return ConfigurationSourceTypeEnum::TEMPLATE;
    }
    
    CaretAssert(0);
    return ConfigurationSourceTypeEnum::USER;
}

/**
 * @return The selected user configuration
 */
std::unique_ptr<TileTabsLayoutBaseConfiguration>
TileTabsConfigurationDialog::getSelectedUserConfiguration() const
{
    std::unique_ptr<TileTabsLayoutBaseConfiguration> config;
    
    AString uuid = getSelectedUserTileTabsConfigurationUniqueIdentifier();
    if ( ! uuid.isEmpty()) {
        config = m_caretPreferences->getCopyOfTileTabsUserConfigurationByUniqueIdentifier(uuid);
    }
    
    return config;
}

/**
 * @return The selected template configuration
 */
std::unique_ptr<TileTabsLayoutBaseConfiguration>
TileTabsConfigurationDialog::getSelectedTemplateConfiguration() const
{
    std::unique_ptr<TileTabsLayoutBaseConfiguration> config;
    
    const int32_t index = m_templateConfigurationSelectionListWidget->currentIndex().row();
    if (index >= 0) {
        CaretAssertVectorIndex(m_templateLayoutConfigurations, index);
        config.reset(m_templateLayoutConfigurations[index]->newCopyWithNewUniqueIdentifier());
    }

    return config;
}

/**
 * Called when a configuration is highlighted and shows an outline of the layout in the
 * configuration preview label
 */
void
TileTabsConfigurationDialog::templateConfigurationSelectionListWidgetItemChanged()
{
    loadConfigurationPreviewLabel(getSelectedTemplateConfiguration().get());
}

/**
 * Called when a configuration is highlighted and shows an outline of the layout in the
 * configuration preview label
 */
void
TileTabsConfigurationDialog::userConfigurationSelectionListWidgetItemChanged()
{
    std::unique_ptr<TileTabsLayoutBaseConfiguration> config = getSelectedUserConfiguration();
    loadConfigurationPreviewLabel(config.get());
}

/**
 * Load the given configuration into the configuration preview label
 *
 * @param configuration
 *     The configuration to show in label
 */
void
TileTabsConfigurationDialog::loadConfigurationPreviewLabel(TileTabsLayoutBaseConfiguration* configuration)
{


    QPixmap pixmap(106, 106);
    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(m_configurationImagePreviewLabel,
                                                                                            pixmap);
    painter->translate(3, 3);

    QPen pen = painter->pen();
    pen.setWidth(1);
    painter->setPen(pen);
    
    if (configuration != NULL) {
        switch (configuration->getLayoutType()) {
            case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
                break;
            case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            {
                TileTabsLayoutGridConfiguration* gridConfig = configuration->castToGridConfiguration();
                CaretAssert(gridConfig);
                const int32_t numModels(gridConfig->getNumberOfRows() * gridConfig->getNumberOfColumns());
                std::vector<int32_t> rowHeights;
                std::vector<int32_t> columnWidths;
                const int32_t windowWidth(100);
                const int32_t windowHeight(100);
                gridConfig->getRowHeightsAndColumnWidthsForWindowSize(windowWidth,
                                                                      windowHeight,
                                                                      numModels,
                                                                      TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID,
                                                                      rowHeights,
                                                                      columnWidths);
                
                const int32_t numRows = static_cast<int32_t>(rowHeights.size());
                const int32_t numCols = static_cast<int32_t>(columnWidths.size());
                int32_t topY(windowHeight);
                for (int32_t iRow = 0; iRow < numRows; iRow++) {
                    int32_t colX(0);
                    CaretAssertVectorIndex(rowHeights, iRow);
                    const int32_t height = rowHeights[iRow];
                    
                    for (int32_t jCol = 0; jCol < numCols; jCol++) {
                        CaretAssertVectorIndex(columnWidths, jCol);
                        const int width = columnWidths[jCol];
                        
                        QPoint bottomLeft(colX, topY - height);
                        QPoint bottomRight(colX + width - 1, topY - height);
                        QPoint topRight(colX + width - 1, topY);
                        QPoint topLeft(colX, topY);
                        painter->drawLine(bottomLeft, bottomRight);
                        painter->drawLine(bottomRight, topRight);
                        painter->drawLine(topRight, topLeft);
                        painter->drawLine(topLeft, bottomLeft);

                        colX += width;
                    }
                    
                    topY -= height;
                    
                }
            }
                break;
            case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            {
                TileTabsLayoutManualConfiguration* manConfig = configuration->castToManualConfiguration();
                CaretAssert(manConfig);
                const int32_t numTabs = manConfig->getNumberOfTabs();
                for (int32_t i = 0; i < numTabs; i++) {
                    TileTabsBrowserTabGeometry* geom = manConfig->getTabInfo(i);
                    CaretAssert(geom);
                    float minX(0.0), maxX(0.0), minY(0.0), maxY(0.0);
                    geom->getBounds(minX, maxX, minY, maxY);
                    
                    QPointF bottomLeft(minX, minY);
                    QPointF bottomRight(maxX, minY);
                    QPointF topRight(maxX, maxY);
                    QPointF topLeft(minX, maxY);
                    painter->drawLine(bottomLeft, bottomRight);
                    painter->drawLine(bottomRight, topRight);
                    painter->drawLine(topRight, topLeft);
                    painter->drawLine(topLeft, bottomLeft);
                }
            }
                break;
        }
    }
    
    m_configurationImagePreviewLabel->setPixmap(pixmap);
    
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
TileTabsConfigurationDialog::createGridCustomOptionsWidget()
{
    const QString toolTip("<html>"
                          "Removes any space between rows and columns in the tile tabs configuration.  "
                          "Some scenes created in previous versions of wb_view may not appear correctly "
                          "due to changes in layout of the tabs.  Enabling this option may fix the "
                          "problem.  In addition, if the Lock Aspect option is selected prior to "
                          "to enabling tile tabs, this option may improve the layout."
                          "</html>");
    m_gridCenteringCorrectionCheckBox = new QCheckBox("Custom Grid Centering Correction");
    m_gridCenteringCorrectionCheckBox->setToolTip(toolTip);
    QObject::connect(m_gridCenteringCorrectionCheckBox, &QCheckBox::clicked,
                     this, &TileTabsConfigurationDialog::centeringCorrectionCheckBoxClicked);
    
    QGroupBox* groupBox = new QGroupBox("Options");
    QVBoxLayout* layout = new QVBoxLayout(groupBox);
    layout->addWidget(m_gridCenteringCorrectionCheckBox);
    
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
        m_gridCenteringCorrectionCheckBox->setChecked(config->isCenteringCorrectionEnabled());
    }
}

/**
 * @return The rows/columns stretch layout
 */
QWidget*
TileTabsConfigurationDialog::createGridRowColumnStretchWidget()
{
    /*
     * Set number of rows
     */
    QLabel* numberOfRowsLabel = new QLabel("Rows");
    m_numberOfGridRowsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                              s_maximumRowsColumns,
                                                                              1,
                                                                              this,
                                                                              SLOT(gridConfigurationNumberOfRowsOrColumnsChanged()));
    m_numberOfGridRowsSpinBox->setToolTip("Number of rows for the tab configuration");
    
    /*
     * Edit content of rows
     */
    QGroupBox* rowGroupBox = new QGroupBox();
    rowGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_gridRowElementsGridLayout = new QGridLayout(rowGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(m_gridRowElementsGridLayout, 4, 2);

    /*
     * Set number of columns
     */
    QLabel* numberOfColumnsLabel = new QLabel("Columns");
    m_numberOfGridColumnsSpinBox = WuQFactory::newSpinBoxWithMinMaxStepSignalInt(1,
                                                                                 s_maximumRowsColumns,
                                                                                 1,
                                                                                 this,
                                                                                 SLOT(gridConfigurationNumberOfRowsOrColumnsChanged()));
    m_numberOfGridColumnsSpinBox->setToolTip("Number of columns for the tab configuration");
 
    QGroupBox* columnsGroupBox = new QGroupBox();
    columnsGroupBox->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    m_gridColumnElementsGridLayout = new QGridLayout(columnsGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(m_gridColumnElementsGridLayout, 2, 2);

    QHBoxLayout* rowsColumnsCountLayout = new QHBoxLayout();
    rowsColumnsCountLayout->setContentsMargins(0, 0, 0, 0);
    rowsColumnsCountLayout->addWidget(numberOfRowsLabel);
    rowsColumnsCountLayout->addWidget(m_numberOfGridRowsSpinBox);
    rowsColumnsCountLayout->addWidget(numberOfColumnsLabel);
    rowsColumnsCountLayout->addWidget(m_numberOfGridColumnsSpinBox);
    rowsColumnsCountLayout->addStretch();
    
    QWidget* customGridOptionsWidget = createGridCustomOptionsWidget();
    customGridOptionsWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setSpacing(4);
    layout->addLayout(rowsColumnsCountLayout, 0);
    layout->addWidget(rowGroupBox, 0, Qt::AlignLeft);
    layout->addWidget(columnsGroupBox, 0, Qt::AlignLeft);
    layout->addWidget(customGridOptionsWidget, 0, Qt::AlignLeft);
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
        int32_t numRowElements = static_cast<int32_t>(m_gridRowElements.size());
        
        /**
         * Add elements as needed.
         */
        const int32_t numToAdd = numRows - numRowElements;
        for (int32_t iRow = 0; iRow < numToAdd; iRow++) {
            addRowColumnStretchWidget(EventTileTabsGridConfigurationModification::RowColumnType::ROW,
                                      m_gridRowElementsGridLayout,
                                      m_gridRowElements);
        }
        
        /*
         * Update widgets with element content
         */
        numRowElements = static_cast<int32_t>(m_gridRowElements.size());
        for (int32_t iRow = 0; iRow < numRowElements; iRow++) {
            TileTabsGridRowColumnElement* element(NULL);
            if (iRow < numRows) {
                element = configuration->getRow(iRow);
            }
            
            CaretAssertVectorIndex(m_gridRowElements, iRow);
            m_gridRowElements[iRow]->updateContent(element);
        }
        
        m_gridRowElementsGridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
    }
    
    /*
     * Update columns
     */
    {
        const int32_t numColumns  = configuration->getNumberOfColumns();
        int32_t numColumnElements = static_cast<int32_t>(m_gridColumnElements.size());
        
        /**
         * Add elements as needed.
         */
        const int32_t numToAdd = numColumns - numColumnElements;
        for (int32_t iColumn = 0; iColumn < numToAdd; iColumn++) {
            addRowColumnStretchWidget(EventTileTabsGridConfigurationModification::RowColumnType::COLUMN,
                                      m_gridColumnElementsGridLayout,
                                      m_gridColumnElements);
        }
        
        /*
         * Update widgets with element content
         */
        numColumnElements = static_cast<int32_t>(m_gridColumnElements.size());
        for (int32_t iColumn = 0; iColumn < numColumnElements; iColumn++) {
            TileTabsGridRowColumnElement* element(NULL);
            if (iColumn < numColumns) {
                element = configuration->getColumn(iColumn);
            }
            
            CaretAssertVectorIndex(m_gridColumnElements, iColumn);
            m_gridColumnElements[iColumn]->updateContent(element);
        }

        m_gridColumnElementsGridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
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
                                                       std::vector<TileTabGridRowColumnWidgets*>& elementVector)
{
    const int32_t index = static_cast<int32_t>(elementVector.size());
    if (index == 0) {
        QString title("Index");
        switch (rowColumnType) {
            case EventTileTabsGridConfigurationModification::RowColumnType::COLUMN:
                title = "Column";
                break;
            case EventTileTabsGridConfigurationModification::RowColumnType::ROW:
                title = "Row";
                break;
        }
        int32_t columnIndex(0);
        int32_t row = gridLayout->rowCount();
        gridLayout->addWidget(new QLabel(title), row, columnIndex++, 1, 2, Qt::AlignHCenter);
        columnIndex++;
        gridLayout->addWidget(new QLabel("Content"), row, columnIndex++, Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Type"), row, columnIndex++, Qt::AlignHCenter);
        gridLayout->addWidget(new QLabel("Stretch"), row, columnIndex++, Qt::AlignHCenter);
        for (int32_t i = 0; i < columnIndex; i++) {
            gridLayout->setColumnStretch(i, 0);
        }
    }
    
    TileTabGridRowColumnWidgets* elementWidget = new TileTabGridRowColumnWidgets(this,
                                                                     rowColumnType,
                                                                     index,
                                                                     gridLayout,
                                                                     this);
    QObject::connect(elementWidget, &TileTabGridRowColumnWidgets::itemChanged,
                     this, &TileTabsConfigurationDialog::gridConfigurationStretchFactorWasChanged);
    QObject::connect(elementWidget, &TileTabGridRowColumnWidgets::modificationRequested,
                     this, &TileTabsConfigurationDialog::tileTabsModificationRequested);

    elementVector.push_back(elementWidget);
}

/**
 * Update the widget for manual geometry editing
 */
void
TileTabsConfigurationDialog::updateManualGeometryEditorWidget()
{
    BrainBrowserWindow* window = getBrowserWindow();
    CaretAssert(window);
    std::vector<BrowserTabContent*> allBrowserTabs;
    window->getAllTabContent(allBrowserTabs);

    const int32_t numTabs = static_cast<int32_t>(allBrowserTabs.size());
    int32_t numWidgets = static_cast<int32_t>(m_manualGeometryEditorWidgets.size());
    
    /*
     * Add elements as needed
     */
    const int32_t numToAdd = numTabs - numWidgets;
    for (int32_t j = 0; j < numToAdd; j++) {
        addManualGeometryWidget(m_manualGeometryGridLayout,
                                m_manualGeometryEditorWidgets);
    }
    
    /*
     * Update with element contents
     */
    numWidgets = static_cast<int32_t>(m_manualGeometryEditorWidgets.size());
    for (int32_t iRow = 0; iRow < numWidgets; iRow++) {
        
        BrowserTabContent* tabContent(NULL);
        if (iRow < numTabs) {
            tabContent = allBrowserTabs[iRow];
        }
        
        m_manualGeometryEditorWidgets[iRow]->updateContent(tabContent);
    }
    
    BrowserWindowContent* bwc = getBrowserWindowContent();
    if (bwc != NULL) {
        QSignalBlocker winAnnBlocker(m_manualConfigurationWindowAnnotationsDepthSpinBox);
        m_manualConfigurationWindowAnnotationsDepthSpinBox->setValue(bwc->getWindowAnnotationsStackingOrder());
    }
    m_manualConfigurationWindowAnnotationsDepthSpinBox->setEnabled(bwc != NULL);
    
    m_manualGeometryGridLayout->setSizeConstraint(QLayout::SetMinAndMaxSize);
}

/**
 * Called when the geomety is changed in a manual configuration
 */
void
TileTabsConfigurationDialog::manualConfigurationGeometryChanged()
{
    updateGraphicsWindow();
}

/**
 * Add a manual geometry widget
 *
 * @param gridLayout
 *     The Qt layout
 * @param widgetsVector
 *     Vector containing the manual editing widgets
 */
void
TileTabsConfigurationDialog::addManualGeometryWidget(QGridLayout* gridLayout,
                                                     std::vector<TileTabsManualTabGeometryWidget*>& widgetsVector)
{
    const int32_t index = static_cast<int32_t>(widgetsVector.size());
    if (index == 0) {
        int32_t columnIndex(0);
        int32_t rowIndex = gridLayout->rowCount();
        
        gridLayout->setHorizontalSpacing(8);
        gridLayout->addWidget(new QLabel("Show"), rowIndex, columnIndex++, Qt::AlignLeft);
        const int32_t nameColumn(columnIndex);
        gridLayout->addWidget(new QLabel("Tab Name"), rowIndex, columnIndex++, Qt::AlignLeft);
        gridLayout->addWidget(new QLabel("Left"), rowIndex, columnIndex++, Qt::AlignLeft);
        gridLayout->addWidget(new QLabel("Right"), rowIndex, columnIndex++, Qt::AlignLeft);
        gridLayout->addWidget(new QLabel("Bottom"), rowIndex, columnIndex++, Qt::AlignLeft);
        gridLayout->addWidget(new QLabel("Top"), rowIndex, columnIndex++, Qt::AlignLeft);
        gridLayout->addWidget(new QLabel("Background"), rowIndex, columnIndex++, Qt::AlignLeft);
        const int32_t zOrderColumn(columnIndex);
        gridLayout->addWidget(new QLabel("Z-Order"), rowIndex, columnIndex++, Qt::AlignLeft);
        
        m_manualConfigurationWindowAnnotationsDepthSpinBox = new QSpinBox();
        m_manualConfigurationWindowAnnotationsDepthSpinBox->setMinimum(-1000);
        m_manualConfigurationWindowAnnotationsDepthSpinBox->setMaximum(1000);
        m_manualConfigurationWindowAnnotationsDepthSpinBox->setSingleStep(1);
        QObject::connect(m_manualConfigurationWindowAnnotationsDepthSpinBox,  QOverload<int>::of(&QSpinBox::valueChanged),
                         this, &TileTabsConfigurationDialog::manualConfigurationWindowAnnotationsDepthSpinBoxValueChanged);
        m_manualConfigurationWindowAnnotationsDepthSpinBox->setToolTip(TileTabsManualTabGeometryWidget::getStackOrderToolTipText());
        
        const int32_t windowRow(1000); /* will be at bottom */
        gridLayout->addWidget(new QLabel("Window Annotations"),
                              windowRow, nameColumn, 1, 5, Qt::AlignLeft);
        gridLayout->addWidget(m_manualConfigurationWindowAnnotationsDepthSpinBox,
                              windowRow, zOrderColumn);
    }
    
    TileTabsManualTabGeometryWidget* geometryWidget = new TileTabsManualTabGeometryWidget(this,
                                                                                          index,
                                                                                          gridLayout,
                                                                                          this);
    QObject::connect(geometryWidget, &TileTabsManualTabGeometryWidget::itemChanged,
                     this, &TileTabsConfigurationDialog::manualConfigurationGeometryChanged);
    
    widgetsVector.push_back(geometryWidget);
}

/**
 * @return New instance of the manual configuration tool button
 */
QToolButton*
TileTabsConfigurationDialog::createManualConfigurationSetToolButton()
{
    m_setManualToAutomaticGridActionText = "Set Bounds of Tabs from Automatic Grid";
    m_setManualToCustomGridActionText    = "Set Bounds of Tabs from Custom Grid";
    m_setManualToGridColumnsActionText   = "Set Bounds of Tabs from Grid...";

    const QString toolTipText("<html>"
                              "Set (replace) the bounds of all tabs using the bounds created by a grid configuration.  After "
                              "choosing one of the selections, the user may edit individual tab bounds as desired."
                              "<ul>"
                              "<li><b>" + m_setManualToAutomaticGridActionText + "</b> - Tab bounds will be identical to the Automatic Grid"
                              "<li><b>" + m_setManualToCustomGridActionText + "</b> - Tab bounds will be identical to the Custom Grid (note that a "
                              "custom grid may not display all tabs but all tabs will be in the manual configuration)"
                              "<li><b>" + m_setManualToGridColumnsActionText + "</b> - Using a dialog, the user is prompted to enter the number of columns in the grid and wb_view will "
                              "set the number of rows so that the grid contains all tabs.  Tabs bounds are then set using the grid"
                              "</ul>"
                              "</html>");
    QToolButton* toolButton = new QToolButton();
    toolButton->setText("Set...");
    toolButton->setToolTip(toolTipText);
    QObject::connect(toolButton, &QToolButton::clicked,
                     this, &TileTabsConfigurationDialog::manualConfigurationSetToolButtonClicked);
    
    return toolButton;
}

/**
 * Called when manual configuration set tool button is clicked
 */
void
TileTabsConfigurationDialog::manualConfigurationSetToolButtonClicked()
{
    QMenu menu(m_manualConfigurationSetButton);
    QAction* setAutomaticGridAction = menu.addAction(m_setManualToAutomaticGridActionText);
    QAction* setCustomGridAction    = menu.addAction(m_setManualToCustomGridActionText);
    QAction* setColumnsAction       = menu.addAction(m_setManualToGridColumnsActionText);

    QAction* selectedAction = menu.exec(m_manualConfigurationSetButton->mapToGlobal(QPoint(0,0)));
    if (selectedAction == setColumnsAction) {
        manualConfigurationSetMenuColumnsItemTriggered();
    }
    else if (selectedAction == setAutomaticGridAction) {
        manualConfigurationSetMenuFromAutomaticItemTriggered();
    }
    else if (selectedAction == setCustomGridAction) {
        manualConfigurationSetMenuFromCustomItemTriggered();
    }
    else if (selectedAction != NULL) {
        CaretAssertMessage(0, "Has a new action been added but not processed?");
    }
    
    updateDialog();
}

/**
 * Called when manual configuration set columns menu item triggered
 */
void
TileTabsConfigurationDialog::manualConfigurationSetMenuColumnsItemTriggered()
{
    const BrainBrowserWindow* window = getBrowserWindow();
    CaretAssert(window);
    
    std::vector<BrowserTabContent*> allTabContent;
    window->getAllTabContent(allTabContent);
    const int32_t numberOfTabs = static_cast<int32_t>(allTabContent.size());
    
    int32_t defaultNumberOfRows(1);
    int32_t defaultNumberOfColumns(1);
    TileTabsLayoutGridConfiguration::getRowsAndColumnsForNumberOfTabs(numberOfTabs,
                                                                      defaultNumberOfRows,
                                                                      defaultNumberOfColumns);

    WuQDataEntryDialog ded("Create Manual Layout from Grid",
                           m_manualConfigurationSetButton);
    QSpinBox* rowsSpinBox    = ded.addSpinBox("Rows",
                                              defaultNumberOfRows);
    rowsSpinBox->setMinimum(1);
    rowsSpinBox->setMaximum(100);
    rowsSpinBox->setSingleStep(1);
    
    QSpinBox* columnsSpinBox = ded.addSpinBox("Columns",
                                              defaultNumberOfColumns);
    columnsSpinBox->setMinimum(1);
    columnsSpinBox->setMaximum(100);
    columnsSpinBox->setSingleStep(1);
    
    const bool wrapTextFlag(false);
    ded.setTextAtTop("Set rows and columns of Grid:",
                     wrapTextFlag);
    if (ded.exec() == WuQDataEntryDialog::Accepted) {
        const int32_t numberOfRows    = rowsSpinBox->value();
        CaretAssert(numberOfRows >= 1);
        const int32_t numberOfColumns = columnsSpinBox->value();
        CaretAssert(numberOfColumns >= 1);
        
        std::unique_ptr<TileTabsLayoutGridConfiguration> gridConfig(TileTabsLayoutGridConfiguration::newInstanceCustomGrid());
        gridConfig->setNumberOfRows(numberOfRows);
        gridConfig->setNumberOfColumns(numberOfColumns);
        
        loadIntoManualConfiguration(gridConfig.get());
    }
}

/**
 * Called when manual configuration set from automatic grid menu item is triggered
 */
void
TileTabsConfigurationDialog::manualConfigurationSetMenuFromAutomaticItemTriggered()
{
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    if (browserWindowContent != NULL) {
        TileTabsLayoutGridConfiguration* gridConfiguration = browserWindowContent->getAutomaticGridTileTabsConfiguration();
        CaretAssert(gridConfiguration);
        loadIntoManualConfiguration(gridConfiguration);
    }
}

/**
 * Called when manual configuration set from custom grid menu item is triggered
 */
void
TileTabsConfigurationDialog::manualConfigurationSetMenuFromCustomItemTriggered()
{
    TileTabsLayoutGridConfiguration* gridConfiguration = getCustomTileTabsGridConfiguration();
    CaretAssert(gridConfiguration);
    if (gridConfiguration->isCustomDefaultFlag()) {
        /*
         * If we are here the user has not yet selected the custom grid
         * and it defaults to the automatic configuration when selected.
         * So, in this case, use the automatic grid.
         */
        manualConfigurationSetMenuFromAutomaticItemTriggered();
    }
    else {
        loadIntoManualConfiguration(gridConfiguration);
    }
}

/**
 * Called when manual configuration window annotation depth changed
 * @param value
 *    New value for window annotation depth
 */
void
TileTabsConfigurationDialog::manualConfigurationWindowAnnotationsDepthSpinBoxValueChanged(int value)
{
    BrowserWindowContent* bwc = getBrowserWindowContent();
    CaretAssert(bwc);
    bwc->setWindowAnnotationsStackingOrder(value);
    updateGraphicsWindow();
}


/**
 * Create the active configuration type widget
 */
QWidget*
TileTabsConfigurationDialog::createConfigurationTypeWidget()
{
    const AString autoToolTip("Workbench adjusts the number of rows and columns so "
                              "that all tabs are displayed");
    m_automaticGridConfigurationRadioButton = new QRadioButton("Automatic Configuration");
    m_automaticGridConfigurationRadioButton->setToolTip(WuQtUtilities::createWordWrappedToolTipText(autoToolTip));
    
    const AString customToolTip("User sets the number of row, columns, and stretch factors");
    m_customGridConfigurationRadioButton = new QRadioButton("Custom Configuration");
    m_customGridConfigurationRadioButton->setToolTip(WuQtUtilities::createWordWrappedToolTipText(customToolTip));
    
    const AString manualToolTip("User sets positions and sizes of all tabs");
    m_manualConfigurationRadioButton = new QRadioButton("Manual");
    m_manualConfigurationRadioButton->setToolTip(WuQtUtilities::createWordWrappedToolTipText(manualToolTip));
    
    QButtonGroup* buttonGroup = new QButtonGroup(this);
    buttonGroup->addButton(m_automaticGridConfigurationRadioButton);
    buttonGroup->addButton(m_customGridConfigurationRadioButton);
    buttonGroup->addButton(m_manualConfigurationRadioButton);
    QObject::connect(buttonGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked),
                     this, &TileTabsConfigurationDialog::automaticCustomButtonClicked);
    
    m_manualConfigurationSetButton = createManualConfigurationSetToolButton();
    
    QGroupBox* layoutTypeGroupBox = new QGroupBox("Active Configuration Type");
    QGridLayout* buttonTypeLayout = new QGridLayout(layoutTypeGroupBox);
    buttonTypeLayout->setColumnStretch(0,   0);
    buttonTypeLayout->setColumnStretch(1,   0);
    buttonTypeLayout->setColumnStretch(2, 100);
    buttonTypeLayout->addWidget(m_automaticGridConfigurationRadioButton,
                                0, 0, 1, 3, Qt::AlignLeft);
    buttonTypeLayout->addWidget(m_customGridConfigurationRadioButton,
                                1, 0, 1, 3, Qt::AlignLeft);
    buttonTypeLayout->addWidget(m_manualConfigurationRadioButton,
                                2, 0, 1, 1);
    buttonTypeLayout->addWidget(m_manualConfigurationSetButton,
                                2, 1, 1, 2, Qt::AlignLeft);
    
    return layoutTypeGroupBox;
}

QWidget*
TileTabsConfigurationDialog::createConfigurationSettingsWidget()
{
    m_customGridConfigurationWidget = createGridRowColumnStretchWidget();
    m_customGridConfigurationWidget->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
    m_manualGeometryWidget = createManualGeometryEditingWidget();
    
    m_editConfigurationStackedWidget = new QStackedWidget();
    m_editConfigurationStackedWidget->addWidget(m_customGridConfigurationWidget);
    m_editConfigurationStackedWidget->addWidget(m_manualGeometryWidget);
    
    QScrollArea* stretchFactorScrollArea = new QScrollArea();
    stretchFactorScrollArea->setWidget(m_editConfigurationStackedWidget);
    stretchFactorScrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    stretchFactorScrollArea->setWidgetResizable(true);
    
    QGroupBox* layoutSettingsGroupBox = new QGroupBox("Active Configuration Settings");
    QVBoxLayout* settingsGroupBoxLayout = new QVBoxLayout(layoutSettingsGroupBox);
    settingsGroupBoxLayout->addWidget(stretchFactorScrollArea);
    
    return layoutSettingsGroupBox;
}

/**
 * @return The rows/columns stretch layout
 */
QWidget*
TileTabsConfigurationDialog::createManualGeometryEditingWidget()
{
    m_manualGeometryGridLayout = new QGridLayout();
    QWidget* widget = new QWidget;
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addLayout(m_manualGeometryGridLayout);
    layout->addStretch();
    WuQtUtilities::setLayoutSpacingAndMargins(m_manualGeometryGridLayout, 4, 2);
    
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
    if (button == m_automaticGridConfigurationRadioButton) {
        browserWindowContent->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID);
    }
    else if (button == m_customGridConfigurationRadioButton) {
        const TileTabsLayoutGridConfiguration* gridConfig = browserWindowContent->getCustomGridTileTabsConfiguration();
        if (gridConfig != NULL) {
            if ( ! warnIfGridConfigurationTooSmallDialog(gridConfig)) {
                return;
            }
        }
        browserWindowContent->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID);
    }
    else if (button == m_manualConfigurationRadioButton) {
        browserWindowContent->setTileTabsConfigurationMode(TileTabsLayoutConfigurationTypeEnum::MANUAL);
    }
    else {
        CaretAssert(0);
    }
    
    updateConfigurationEditingWidget();
    updateGridStretchFactors();
    updateManualGeometryEditorWidget();
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
 * Read the user configurations from the preferences.
 */
void
TileTabsConfigurationDialog::readUserConfigurationsFromPreferences()
{
    if (m_blockReadUserConfigurationsFromPreferences) {
        return;
    }
    
    m_caretPreferences->readTileTabsUserConfigurations();
}

/**
 * Read the configurations from the preferences.
 */
void
TileTabsConfigurationDialog::updateConfigurationEditingWidget()
{
    BrowserWindowContent* browserWindowContent = getBrowserWindowContent();
    if (browserWindowContent == NULL) {
        return;
    }
    
    bool editingEnabledFlag(false);
    switch (browserWindowContent->getTileTabsConfigurationMode()) {
        case TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID:
            m_automaticGridConfigurationRadioButton->setChecked(true);
            m_editConfigurationStackedWidget->setCurrentWidget(m_customGridConfigurationWidget);
            break;
        case TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID:
            m_customGridConfigurationRadioButton->setChecked(true);
            m_editConfigurationStackedWidget->setCurrentWidget(m_customGridConfigurationWidget);
            editingEnabledFlag = true;
            break;
        case TileTabsLayoutConfigurationTypeEnum::MANUAL:
            m_manualConfigurationRadioButton->setChecked(true);
            m_editConfigurationStackedWidget->setCurrentWidget(m_manualGeometryWidget);
            editingEnabledFlag = true;
            break;
    }
    
    m_editConfigurationStackedWidget->setEnabled(editingEnabledFlag);
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
    
    updateConfigurationEditingWidget();
    
    readUserConfigurationsFromPreferences();
    
    updateUserConfigurationListWidget();
    
    updateGridStretchFactors();
    updateManualGeometryEditorWidget();
    
    switch (getSelectedConfigurationSourceType()) {
        case TEMPLATE:
            templateConfigurationSelectionListWidgetItemChanged();
            break;
        case USER:
            userConfigurationSelectionListWidgetItemChanged();
            break;
    }
}

/**
 * Update the user configuration list widget
 */
void
TileTabsConfigurationDialog::updateUserConfigurationListWidget()
{
    int defaultIndex = m_userConfigurationSelectionListWidget->currentRow();
    
    QSignalBlocker blocker(m_userConfigurationSelectionListWidget);
    m_userConfigurationSelectionListWidget->clear();
    
    const bool includeManualConfigurationsFlag(true);
    std::vector<std::pair<AString, AString>> nameUniqueIDs =
    m_caretPreferences->getTileTabsUserConfigurationsNamesAndUniqueIdentifiers(includeManualConfigurationsFlag);
    
    for (const auto& nameID : nameUniqueIDs) {
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
}

/**
 * Update the template configuration list widget
 */
void
TileTabsConfigurationDialog::updateTemplateConfigurationListWidget()
{
    int defaultIndex = m_templateConfigurationSelectionListWidget->currentRow();
    
    QSignalBlocker blocker(m_templateConfigurationSelectionListWidget);
    m_templateConfigurationSelectionListWidget->clear();
    
    const bool includeManualConfigurationsFlag(true);
    std::vector<std::pair<AString, AString>> nameUniqueIDs =
    m_caretPreferences->getTileTabsUserConfigurationsNamesAndUniqueIdentifiers(includeManualConfigurationsFlag);
    
    for (const auto& nameID : nameUniqueIDs) {
        /*
         * Second element is user data which contains the Unique ID
         */
        QListWidgetItem* item = new QListWidgetItem(nameID.first);
        item->setData(Qt::UserRole,
                      QVariant(nameID.second));
        m_templateConfigurationSelectionListWidget->addItem(item);
    }
    
    const int32_t numItemsInComboBox = m_templateConfigurationSelectionListWidget->count();
    if (defaultIndex >= numItemsInComboBox) {
        defaultIndex = numItemsInComboBox - 1;
    }
    if (defaultIndex < 0) {
        defaultIndex = 0;
    }
    if (defaultIndex < m_templateConfigurationSelectionListWidget->count()) {
        m_templateConfigurationSelectionListWidget->setCurrentRow(defaultIndex);
    }
}

/**
 * Update the stretch factors.
 */
void
TileTabsConfigurationDialog::updateGridStretchFactors()
{
    BrainBrowserWindow* browserWindow = getBrowserWindow();
    m_automaticGridConfigurationRadioButton->setText(browserWindow->getTileTabsConfigurationLabelText(TileTabsLayoutConfigurationTypeEnum::AUTOMATIC_GRID,
                                                                                              true));
    m_customGridConfigurationRadioButton->setText(browserWindow->getTileTabsConfigurationLabelText(TileTabsLayoutConfigurationTypeEnum::CUSTOM_GRID,
                                                                                           true));
    const TileTabsLayoutGridConfiguration* configuration = getCustomTileTabsGridConfiguration();
    if (configuration != NULL) {
        updateRowColumnStretchWidgets(const_cast<TileTabsLayoutGridConfiguration*>(configuration));
        QSignalBlocker rowBlocker(m_numberOfGridRowsSpinBox);
        m_numberOfGridRowsSpinBox->setValue(configuration->getNumberOfRows());
        QSignalBlocker columnBlocker(m_numberOfGridColumnsSpinBox);
        m_numberOfGridColumnsSpinBox->setValue(configuration->getNumberOfColumns());
    }
    
    updateTemplateUserConfigurationPushButtons(getSelectedConfigurationSourceType());
}

/**
 * Update the push buttons enabled status
 *
 * @param sourceType
 *     The selected configuration source type.
 */
void
TileTabsConfigurationDialog::updateTemplateUserConfigurationPushButtons(const ConfigurationSourceTypeEnum sourceType)
{
    bool addEnabledFlag(false);
    bool replaceEnabledFlag(false);
    bool deleteRenameEnabledFlag(false);
    bool loadEnabledFlag(false);
    bool showXmlVisibleFlag(m_caretPreferences->isDevelopMenuEnabled());
    bool showXmlEnabledFlag(false);
    
    switch (sourceType) {
        case TEMPLATE:
        {
            const bool haveTemplateConfigurationFlag = (getSelectedTemplateConfiguration() != NULL);
            if (haveTemplateConfigurationFlag) {
                loadEnabledFlag = true;
            }
        }
            break;
        case USER:
        {
            const bool haveUserConfigurationFlag(getSelectedUserConfiguration() != NULL);
            if (m_automaticGridConfigurationRadioButton->isChecked()) {
                /* Nothing */
            }
            else {
                addEnabledFlag = true;
                if (haveUserConfigurationFlag) {
                    replaceEnabledFlag = true;
                }
            }

            if (haveUserConfigurationFlag) {
                deleteRenameEnabledFlag = true;
                loadEnabledFlag         = true;
                showXmlEnabledFlag      = true;
            }
        }
            break;
    }
    
    m_addConfigurationPushButton->setEnabled(addEnabledFlag);
    m_replaceConfigurationPushButton->setEnabled(replaceEnabledFlag);
    m_loadConfigurationPushButton->setEnabled(loadEnabledFlag);
    
    m_renameConfigurationPushButton->setEnabled(deleteRenameEnabledFlag);
    m_deleteConfigurationPushButton->setEnabled(deleteRenameEnabledFlag);
    
    m_showConfigurationXmlPushButton->setVisible(showXmlVisibleFlag);
    m_showConfigurationXmlPushButton->setEnabled(showXmlVisibleFlag
                                                 && showXmlEnabledFlag);

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
    const AString uniqueID(getSelectedUserTileTabsConfigurationUniqueIdentifier());
    if (uniqueID.isEmpty()) {
        WuQMessageBox::errorOk(m_renameConfigurationPushButton,
                               "Selected configuration is missing Unique Identifier (Program Error)");
        return;
    }
    
    std::unique_ptr<TileTabsLayoutBaseConfiguration> config = m_caretPreferences->getCopyOfTileTabsUserConfigurationByUniqueIdentifier(uniqueID);
    if (config) {
        const AString oldName = config->getName();
        bool ok = false;
        const AString newName = QInputDialog::getText(m_renameConfigurationPushButton,
                                                      "Rename Configuration",
                                                      "Name",
                                                      QLineEdit::Normal,
                                                      oldName,
                                                      &ok);
        if (ok
            && ( ! newName.isEmpty())) {
            m_blockReadUserConfigurationsFromPreferences = true;
            AString errorMessage;
            if (m_caretPreferences->renameTileTabsUserConfiguration(uniqueID,
                                                                    newName,
                                                                    errorMessage)) {
            }
            else {
                WuQMessageBox::errorOk(m_renameConfigurationPushButton,
                                       errorMessage);
            }
            m_blockReadUserConfigurationsFromPreferences = false;
            updateDialog();
        }
    }
    else {
        WuQMessageBox::errorOk(m_renameConfigurationPushButton, ("Unable to find configuration with unique ID="
                                                                 + uniqueID));
    }
}

/**
 * Called when show configuration XML button is clicked
 */
void
TileTabsConfigurationDialog::showConfigurationXmlPushButtonClicked()
{
    std::unique_ptr<TileTabsLayoutBaseConfiguration> config = getSelectedUserConfiguration();
    if (config) {
        const QString xml = config->encodeInXML();
        WuQTextEditorDialog::runNonModal("XML",
                                         xml,
                                         WuQTextEditorDialog::TextMode::PLAIN,
                                         WuQTextEditorDialog::WrapMode::NO,
                                         m_showConfigurationXmlPushButton);
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
 * @return The Unique Identifier of the selected user tile tabs configuration.
 * Empty if no configuration is selected.
 */
AString
TileTabsConfigurationDialog::getSelectedUserTileTabsConfigurationUniqueIdentifier() const
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
TileTabsConfigurationDialog::gridConfigurationNumberOfRowsOrColumnsChanged()
{
    TileTabsLayoutGridConfiguration* configuration = getCustomTileTabsGridConfiguration();
    if (configuration != NULL) {
        configuration->setNumberOfRows(m_numberOfGridRowsSpinBox->value());
        configuration->setNumberOfColumns(m_numberOfGridColumnsSpinBox->value());
        
        updateGridStretchFactors();

        updateGraphicsWindow();
    }
}

/**
 * Called when a configuration stretch factor value is changed.
 */
void
TileTabsConfigurationDialog::gridConfigurationStretchFactorWasChanged()
{
    TileTabsLayoutGridConfiguration* configuration = getCustomTileTabsGridConfiguration();
    if (configuration == NULL) {
        return;
    }
    
    updateGridStretchFactors();
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
        
        updateGridStretchFactors();
        
        updateManualGeometryEditorWidget();
        
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
 * Load the template layout configurations
 */
void
TileTabsConfigurationDialog::loadTemplateLayoutConfigurations()
{
    {
        const QString configXML(R""""(<TileTabsManualLayout Name="2 Left, 1 Right" Version="1" UniqueID="{cbbda258-c860-40fe-be46-c87db104d103}">"
                                         "<TabInfo DisplayStatus="true" TabIndex="0" MinX="0.00" MaxX="50.00" MinY="50.00" MaxY="100.00" StackingOrder="1" Background="OPAQUE_BG"/>"
                                         "<TabInfo DisplayStatus="true" TabIndex="1" MinX="50.00" MaxX="100.00" MinY="0.00" MaxY="100.00" StackingOrder="2" Background="OPAQUE_BG"/>"
                                         "<TabInfo DisplayStatus="true" TabIndex="2" MinX="0.00" MaxX="50.00" MinY="0.00" MaxY="50.00" StackingOrder="3" Background="OPAQUE_BG"/>"
                                         "</TileTabsManualLayout>)"""");
        loadTemplateLayoutConfigurationFromXML(configXML);
    }
                                         
     {
         const QString configXML(R""""(<TileTabsManualLayout Name="1 Left, 2 Right" Version="1" UniqueID="{c2c55e5a-07ea-4cc2-8731-52c33532c726}">
                                          <TabInfo DisplayStatus="true" TabIndex="0" MinX="50.00" MaxX="100.00" MinY="50.00" MaxY="100.00" StackingOrder="1" Background="OPAQUE_BG"/>
                                          <TabInfo DisplayStatus="true" TabIndex="1" MinX="0.00" MaxX="50.00" MinY="0.00" MaxY="100.00" StackingOrder="2" Background="OPAQUE_BG"/>
                                          <TabInfo DisplayStatus="true" TabIndex="2" MinX="50.00" MaxX="100.00" MinY="0.00" MaxY="50.00" StackingOrder="3" Background="OPAQUE_BG"/>
                                          </TileTabsManualLayout>)"""");
         loadTemplateLayoutConfigurationFromXML(configXML);
     }
                                         
     {
         const QString configXML(R""""(<TileTabsManualLayout Name="1 Large, 3 Small" Version="1" UniqueID="{e13c96c6-0c06-4876-8983-7ced716a2324}">
                                          <TabInfo DisplayStatus="true" TabIndex="0" MinX="0.00" MaxX="100.00" MinY="31.00" MaxY="100.00" StackingOrder="1" Background="OPAQUE_BG"/>
                                          <TabInfo DisplayStatus="true" TabIndex="1" MinX="0.00" MaxX="33.00" MinY="0.00" MaxY="30.00" StackingOrder="2" Background="OPAQUE_BG"/>
                                          <TabInfo DisplayStatus="true" TabIndex="2" MinX="33.00" MaxX="66.00" MinY="0.00" MaxY="30.00" StackingOrder="3" Background="OPAQUE_BG"/>
                                          <TabInfo DisplayStatus="true" TabIndex="3" MinX="67.00" MaxX="100.00" MinY="0.00" MaxY="30.00" StackingOrder="4" Background="OPAQUE_BG"/>
                                          </TileTabsManualLayout>)"""");
         loadTemplateLayoutConfigurationFromXML(configXML);
     }
                                      
      {
          const QString configXML(R""""(<TileTabsManualLayout Name="Offset Rows" Version="1" UniqueID="{8fae9798-f20b-456e-b8e2-aba1d3cff3d4}">
                                        <TabInfo DisplayStatus="true" TabIndex="0" MinX="0.00" MaxX="40.00" MinY="50.00" MaxY="100.00" StackingOrder="1" Background="OPAQUE_BG"/>
                                        <TabInfo DisplayStatus="true" TabIndex="1" MinX="40.00" MaxX="80.00" MinY="50.00" MaxY="100.00" StackingOrder="2" Background="OPAQUE_BG"/>
                                        <TabInfo DisplayStatus="true" TabIndex="2" MinX="20.00" MaxX="60.00" MinY="0.00" MaxY="50.00" StackingOrder="3" Background="TRANSPARENT_BG"/>
                                        <TabInfo DisplayStatus="true" TabIndex="3" MinX="60.00" MaxX="100.00" MinY="0.00" MaxY="50.00" StackingOrder="4" Background="OPAQUE_BG"/>
                                        </TileTabsManualLayout>)"""");
          loadTemplateLayoutConfigurationFromXML(configXML);
      }
}

/**
 * Load a template configuration from the given XML
 */
void
TileTabsConfigurationDialog::loadTemplateLayoutConfigurationFromXML(const QString& xml)
{
    AString errorMessage;
    TileTabsLayoutBaseConfiguration* config = TileTabsLayoutBaseConfiguration::decodeFromXML(xml,
                                                                                             errorMessage);
    if (config != NULL) {
        std::unique_ptr<TileTabsLayoutBaseConfiguration> configPtr(config);
        m_templateLayoutConfigurations.push_back(std::move(configPtr));
    }
    else {
        CaretLogWarning("Failed to laod template configuration from XML:\n"
                        + xml);
    }
}




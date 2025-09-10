
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

#define __RECENT_FILES_DIALOG_DECLARE__
#include "RecentFilesDialog.h"
#undef __RECENT_FILES_DIALOG_DECLARE__

#include <QAction>
#include <QActionGroup>
#include <QCheckBox>
#include <QDesktopServices>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QLabel>
#include <QLineEdit>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QUrl>

#include "ApplicationInformation.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "DataFileException.h"
#include "GuiManager.h"
#include "RecentFileItem.h"
#include "RecentFileItemsContainer.h"
#include "RecentFileItemsFilter.h"
#include "RecentFilesTableWidget.h"
#include "Scene.h"
#include "SceneFile.h"
#include "SessionManager.h"
#include "UsernamePasswordWidget.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::RecentFilesDialog 
 * \brief Dialog for opening recent files and also functions as splash dialog
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param runMode
 *    Mode for running the dialog
 * @param parent
 *    The parent widget
 */
RecentFilesDialog::RecentFilesDialog(const RunMode runMode,
                                     QWidget* parent)
: QDialog(parent),
m_runMode(runMode)
{
    AString dialogTitle;
    ApplicationInformation appInfo;
    switch (m_runMode) {
        case RunMode::OPEN_RECENT:
            dialogTitle = "Open Recent";
            break;
        case RunMode::SPLASH_SCREEN:
            dialogTitle = (appInfo.getName()
                           + " "
                           + appInfo.getVersion());
            break;
    }
    
    setWindowTitle(dialogTitle);

    QWidget* internetButtonsWidget = createInternetButtonsWidget();
    
    QWidget* fileTypeButtonsWidget = createFileTypesButtonWidget();
    
    QWidget* filesFilteringWidget = createFilesFilteringWidget();
    
    m_recentFilesTableWidget = new RecentFilesTableWidget();
    QObject::connect(m_recentFilesTableWidget, &RecentFilesTableWidget::sortingChanged,
                     this, &RecentFilesDialog::updateFilesTableContent);
    QObject::connect(m_recentFilesTableWidget, &RecentFilesTableWidget::loadSceneOrSpecFileFromItem,
                     this, &RecentFilesDialog::loadSceneOrSpecFileFromItem);
    


    QWidget* dialogButtonWidget = createDialogButtonsWidget();
    
    /*
     * Must be connected after dialog buttons created since it may alter the Open button status
     */
    QObject::connect(m_recentFilesTableWidget, &RecentFilesTableWidget::selectedItemChanged,
                     this, &RecentFilesDialog::tableWidgetItemClicked);
    QObject::connect(m_recentFilesTableWidget, &RecentFilesTableWidget::selectedItemDoubleClicked,
                     this, &RecentFilesDialog::tableWidgetItemDoubleClicked);
    
    QGridLayout* dialogLayout = new QGridLayout(this);
    QMargins layoutMargins = dialogLayout->contentsMargins();
    layoutMargins.setBottom(0);
    layoutMargins.setLeft(0);
    layoutMargins.setTop(0);
    dialogLayout->setContentsMargins(layoutMargins);
    dialogLayout->setHorizontalSpacing(0);
    dialogLayout->setVerticalSpacing(0);
    dialogLayout->setRowStretch(2, 100);
    dialogLayout->setColumnStretch(1, 100);
    dialogLayout->addWidget(internetButtonsWidget, 0, 0, 2, 1);
    dialogLayout->addWidget(filesFilteringWidget, 0, 1);
    dialogLayout->addWidget(fileTypeButtonsWidget, 2, 0);
    dialogLayout->addWidget(m_recentFilesTableWidget, 1, 1, 2, 1);
    dialogLayout->addWidget(dialogButtonWidget, 3, 1);

    m_loadPushButton->setAutoDefault(true);
    m_loadPushButton->setDefault(true);
    
    Brain* brain = GuiManager::get()->getBrain();
    m_currentDirectoryItemsContainer.reset(RecentFileItemsContainer::newInstanceSceneAndSpecFilesInDirectory(brain->getCurrentDirectory()));
    m_modeDirectorySceneSpecFilesAction->setEnabled( ! m_currentDirectoryItemsContainer->isEmpty());
    
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    m_recentFilesItemsContainer.reset(RecentFileItemsContainer::newInstanceRecentSceneAndSpecFiles(preferences,
                                                                                                   RecentFileItemsContainer::WriteIfModifiedType::WRITE_YES));
    m_modeRecentFilesAction->setEnabled( ! m_recentFilesItemsContainer->isEmpty());
    
    m_recentDirectoryItemsContainer.reset(RecentFileItemsContainer::newInstanceRecentDirectories(preferences,
                                                                                                 RecentFileItemsContainer::WriteIfModifiedType::WRITE_YES));
    m_modeRecentDirectoriesAction->setEnabled( ! m_recentDirectoryItemsContainer->isEmpty());

    m_recentScenesItemsContainer.reset(RecentFileItemsContainer::newInstanceRecentScenes(preferences,
                                                                                         RecentFileItemsContainer::WriteIfModifiedType::WRITE_YES));
    m_modeRecentScenesAction->setEnabled( ! m_recentScenesItemsContainer->isEmpty());
    
    m_exampleDataSetsItemsContainer.reset(RecentFileItemsContainer::newInstanceExampleDataSets());
    std::vector<ExampleSceneInfo> exampleSceneInfo;
    SessionManager::get()->getExampleSceneFilesAndSceneNames(exampleSceneInfo);
    m_exampleDataSetsItemsContainer->addSceneFileAndSceneNamesToExamplesContainer(exampleSceneInfo);
    m_modeExampleDataSetsAction->setEnabled( ! m_exampleDataSetsItemsContainer->isEmpty());
    
    /*
     * Favorites is updated when it is selected
     */
    std::vector<RecentFileItemsContainer*> emptyContainers;
    m_favoriteItemsContainer.reset(RecentFileItemsContainer::newInstanceFavorites(emptyContainers));
    
    /*
     * Default to a non-empty container
     */
    RecentFileItemsContainerModeEnum::Enum selectedMode = RecentFileItemsContainerModeEnum::RECENT_FILES;
    if ( ! m_recentFilesItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::RECENT_FILES;
    }
    else if ( ! m_exampleDataSetsItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS;
    }
    else if ( ! m_recentScenesItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::RECENT_SCENES;
    }
    else if ( ! m_recentDirectoryItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES;
    }
    else if ( ! m_currentDirectoryItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES;
    }
    else if ( ! m_favoriteItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::FAVORITES;
    }
    
    updateFavoritesContainer();
    QAction* selectedAction = getActionForMode(selectedMode);
    selectedAction->trigger();
    
    /*
     * Enables/disables Open Button
     */
    tableWidgetItemClicked(m_recentFilesTableWidget->getSelectedItem());
}

/**
 * Destructor.
 */
RecentFilesDialog::~RecentFilesDialog()
{
}

/**
 * Called when a key is pressed
 */
void
RecentFilesDialog::keyPressEvent(QKeyEvent* event)
{
    /*
     * Prevents return key in the name matching line edit
     * from selecting default button and closing dialog
     */
    const int32_t key = event->key();
    if ((key == Qt::Key_Return)
        || (key == Qt::Key_Enter)) {
        event->ignore();
    }
    else {
        QDialog::keyPressEvent(event);
    }
}

/**
 * Run the dialog
 *
 * @param runMode
 *     The run mode (open recent files or splash screen)
 * @param nameOut
 *     Output with name of selected directory or file name
 * @param
 *     Index of scene if result is loading scene from scene file
 * @param parent
 *     Parent for dialog
 * @return
 *     Result enumerated value
 */
RecentFilesDialog::ResultModeEnum
RecentFilesDialog::runDialog(const RunMode runMode,
                             AString& nameOut,
                             int32_t& sceneIndexOut,
                             QWidget* parent)
{
    RecentFilesDialog rfd(runMode,
                          parent);
    rfd.exec();
    
    ResultModeEnum resultMode = rfd.getResultMode();
    nameOut       = rfd.getSelectedDirectoryOrFileName();
    sceneIndexOut = rfd.getSelectedSceneIndex();
    
    return resultMode;
}

/**
 * @return The result mode
 */
RecentFilesDialog::ResultModeEnum
RecentFilesDialog::getResultMode() const
{
    return m_resultMode;
}

/**
 * @return The selected directory or file name
 */
AString
RecentFilesDialog::getSelectedDirectoryOrFileName() const
{
    return m_resultFilePathAndName;
}

/**
 * @return Index of scene if requested loading of scene from scene file
 */
int32_t
RecentFilesDialog::getSelectedSceneIndex() const
{
    return m_resultSceneIndex;
}

/**
 * @return New instance files filtering widget
 */
QWidget*
RecentFilesDialog::createFilesFilteringWidget()
{
    QLabel* listLabel = new QLabel("List: ");
    
    m_listDirectoriesCheckBox = new QCheckBox("Directories");
    m_listDirectoriesCheckBox->setChecked(true);
    QObject::connect(m_listDirectoriesCheckBox, &QCheckBox::clicked,
                     this, &RecentFilesDialog::listDirectoriesCheckBoxClicked);
    m_listDirectoriesCheckBox->setToolTip("<html>Show/hide directories</html>");

    m_listSceneFilesCheckBox = new QCheckBox("Scene");
    m_listSceneFilesCheckBox->setChecked(true);
    QObject::connect(m_listSceneFilesCheckBox, &QCheckBox::clicked,
                     this, &RecentFilesDialog::listSceneFilesCheckBoxClicked);
    m_listSceneFilesCheckBox->setToolTip("<html>Show/hide scene files</html>");

    m_listSpecFilesCheckBox = new QCheckBox("Spec");
    m_listSpecFilesCheckBox->setChecked(true);
    QObject::connect(m_listSpecFilesCheckBox, &QCheckBox::clicked,
                     this, &RecentFilesDialog::listSpecFilesCheckBoxClicked);
    m_listSpecFilesCheckBox->setToolTip("<html>Show/hide spec files</html>");

    QLabel* nameFilterLabel = new QLabel("Name Filter: ");
    m_nameFilterLineEdit = new QLineEdit();
    m_nameFilterLineEdit->setClearButtonEnabled(true);
    m_nameFilterLineEdit->setFixedWidth(250);
    QObject::connect(m_nameFilterLineEdit, &QLineEdit::textEdited,
                     this, &RecentFilesDialog::nameFilterTextEdited);
    m_nameFilterLineEdit->setToolTip(RecentFileItemsFilter::getMatchingLineEditToolTip());
    
    m_showFilePathsCheckBox = new QCheckBox("Show Scene/Spec Paths");
    m_showFilePathsCheckBox->setChecked(true);
    QObject::connect(m_showFilePathsCheckBox, &QCheckBox::clicked,
                     this, &RecentFilesDialog::showFilePathsCheckBoxClicked);
    m_showFilePathsCheckBox->setToolTip("<html>Show/hide paths below the names of scene and spec files</html>");

    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(listLabel);
    layout->addWidget(m_listDirectoriesCheckBox);
    layout->addWidget(m_listSceneFilesCheckBox);
    layout->addWidget(m_listSpecFilesCheckBox);
    layout->addSpacing(25);
    layout->addWidget(nameFilterLabel);
    layout->addWidget(m_nameFilterLineEdit);
    layout->addSpacing(25);
    layout->addWidget(m_showFilePathsCheckBox);
    layout->addStretch();
    
    return widget;
}


/**
 * @return New instance of the dialog buttons
 */
QWidget*
RecentFilesDialog::createDialogButtonsWidget()
{
    bool showTestButtonFlag(false);
    QPushButton* testPushButton(NULL);
    if (showTestButtonFlag) {
        testPushButton = new QPushButton("Test XML Read/Write");
        QObject::connect(testPushButton, &QPushButton::clicked,
                         this, &RecentFilesDialog::testButtonClicked);
    }

    m_loadPushButton = new QPushButton("Load");
    QObject::connect(m_loadPushButton, &QPushButton::clicked,
                     this, &RecentFilesDialog::loadButtonClicked);
    m_loadPushButton->setToolTip("<html><body>"
                                 "Action depends upon type of item selected:"
                                 "<ul>"
                                 "<li> Scene File - Displays a menu listing scene names.  Selecting a scene name "
                                 "loads the scene without having to use the Scene Dialog."
                                 "<li> Spec File  - Loads all files in the spec file without having to use "
                                 "the Open Spec File Dialog."
                                 "</ul>"
                                 "</body></html>");

    m_openPushButton = new QPushButton("Open");
    QObject::connect(m_openPushButton, &QPushButton::clicked,
                     this, &RecentFilesDialog::openButtonClicked);
    m_openPushButton->setToolTip("<html><body>"
                                 "Action depends upon type of item selected:"
                                 "<ul>"
                                 "<li> Directory  - File Dialog is displayed listing contents of directory."
                                 "<li> Scene File - Scene File is opened in the Scene File Dialog for Scene selection."
                                 "<li> Spec File  - Files in Spec File are listed in the Spec File Dialog."
                                 "</ul>"
                                 "</body></html>");

    QPushButton* openOtherPushButton = new QPushButton("Open Other...");
    QObject::connect(openOtherPushButton, &QPushButton::clicked,
                     this, &RecentFilesDialog::openOtherButtonClicked);
    openOtherPushButton->setToolTip("File Dialog is displayed listing contents of current directory "
                                    "(same action as File Menu -> Open File)");

    QPushButton* cancelPushButton = new QPushButton("Cancel");
    QObject::connect(cancelPushButton, &QPushButton::clicked,
                     this, &RecentFilesDialog::cancelButtonClicked);
    switch (m_runMode) {
        case RunMode::OPEN_RECENT:
            cancelPushButton->setToolTip("Closes dialog with no action taken");
            break;
        case RunMode::SPLASH_SCREEN:
            cancelPushButton->setToolTip("Close this dialog and display Workbench Window");
            break;
    }

    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addStretch();
    if (testPushButton != NULL) {
        layout->addWidget(testPushButton);
        layout->addSpacing(50);
    }
    layout->addWidget(openOtherPushButton);
    layout->addWidget(m_openPushButton);
    layout->addWidget(m_loadPushButton);
    layout->addWidget(cancelPushButton);
    
    
    return widget;
}

/**
 * @return New instance of the file type buttons
 */
QWidget*
RecentFilesDialog::createFileTypesButtonWidget()
{
    m_fileTypeModeActionGroup = new QActionGroup(this);
    
    std::vector<RecentFileItemsContainerModeEnum::Enum> modes;
   RecentFileItemsContainerModeEnum::getAllEnums(modes);
    
    std::vector<QToolButton*> toolButtons;
    for (auto m : modes) {
        AString buttonText(RecentFileItemsContainerModeEnum::toGuiButtonName(m));
        if (buttonText.isEmpty()) {
            /* Mode is not valid in dialog */
            continue;
        }
        
        QAction* action = m_fileTypeModeActionGroup->addAction(buttonText);
        action->setData(RecentFileItemsContainerModeEnum::toName(m));
        action->setCheckable(true);
        
        QString toolTipText;
        switch (m) {
            case RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES:
                toolTipText = ("Choose from Scene and Spec Files in the current directory");
                m_modeDirectorySceneSpecFilesAction = action;
                break;
            case RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS:
                toolTipText = ("Choose from example data sets");
                m_modeExampleDataSetsAction = action;
                break;
            case RecentFileItemsContainerModeEnum::FAVORITES:
                toolTipText = ("Choose from Favorites: favorites are created by clicking "
                               "the Favorite Icon (star) in the Favorite column for an item");
                m_modeFavoritesAction = action;
                break;
            case RecentFileItemsContainerModeEnum::OTHER:
                break;
            case RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES:
                toolTipText = ("Choose from directories that have been visitied by "
                               "the user for opening or saving files");
                m_modeRecentDirectoriesAction = action;
                break;
            case RecentFileItemsContainerModeEnum::RECENT_FILES:
                toolTipText = ("Choose from Scene and Spec files recently opened by the user");
                m_modeRecentFilesAction = action;
                break;
            case RecentFileItemsContainerModeEnum::RECENT_SCENES:
                toolTipText = ("Choose from recently displayed scenes");
                m_modeRecentScenesAction = action;
                break;
        }
        WuQtUtilities::setWordWrappedToolTip(action, toolTipText);

        QToolButton* tb = new QToolButton();
        tb->setDefaultAction(action);
        tb->setStyleSheet("font : 14px"); /* larger characters */
        toolButtons.push_back(tb);
        
        /*
         * Disable action as it will get updated later if its corresponding
         * container is valid
         */
        action->setEnabled(false);
    }
        
    CaretAssert(m_modeDirectorySceneSpecFilesAction);
    CaretAssert(m_modeExampleDataSetsAction);
    CaretAssert(m_modeFavoritesAction);
    CaretAssert(m_modeRecentDirectoriesAction);
    CaretAssert(m_modeRecentFilesAction);
    CaretAssert(m_modeRecentScenesAction);

    std::vector<QWidget*> toolButtonWidgets(toolButtons.begin(),
                                            toolButtons.end());
    WuQtUtilities::matchWidgetSizes(toolButtonWidgets);
    
    QObject::connect(m_fileTypeModeActionGroup, &QActionGroup::triggered,
                     this, &RecentFilesDialog::filesModeActionTriggered);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    for (auto tb : toolButtons) {
        layout->addWidget(tb);
    }
    layout->addStretch();

    return widget;
}

/**
 * Called when name filter text is edited by user
 * @param text
 *    Text in the name filter line edit
 */
void
RecentFilesDialog::nameFilterTextEdited(const QString& /*text*/)
{
    updateFilesTableContent();
    m_nameFilterLineEdit->setFocus();
}

/**
 * Called if List Directories checkbox is clicked
 */
void
RecentFilesDialog::listDirectoriesCheckBoxClicked(bool /*checked*/)
{
    updateFilesTableContent();
}

/**
 * Called if List Scene Files checkbox is clicked
 */
void
RecentFilesDialog::listSceneFilesCheckBoxClicked(bool /*checked*/)
{
    updateFilesTableContent();
}

/**
 * Called if List Spec Files checkbox is clicked
 */
void
RecentFilesDialog::listSpecFilesCheckBoxClicked(bool /*checked*/)
{
    updateFilesTableContent();
}

/**
 * Called if Show File Paths checkbox is clicked
 */
void
RecentFilesDialog::showFilePathsCheckBoxClicked(bool /*checked*/)
{
    updateFilesTableContent();
}

/**
 * @return New instance of internet buttons widget
 */
QWidget*
RecentFilesDialog::createInternetButtonsWidget()
{
    QToolButton* hcpToolButton = new QToolButton();
    QIcon hcpImage;
    if (WuQtUtilities::loadIcon(":/RecentFilesDialog/hcp_image.png", hcpImage)) {
        hcpToolButton->setIconSize(QSize(32, 32));
        hcpToolButton->setIcon(hcpImage);
    }
    else {
        hcpToolButton->setText("H");
    }
    hcpToolButton->setToolTip("Visit HCP Website");
    QObject::connect(hcpToolButton, &QToolButton::clicked,
                     this, &RecentFilesDialog::hcpWebsiteButtonClicked);
    
    QToolButton* twitterToolButton = new QToolButton();
    QIcon twitterIcon;
    if (WuQtUtilities::loadIcon(":/RecentFilesDialog/twitter_image.png",
                                twitterIcon)) {
        twitterToolButton->setIconSize(QSize(32, 32));
        twitterToolButton->setIcon(twitterIcon);
    }
    else {
        twitterToolButton->setText("T");
    }
    twitterToolButton->setToolTip("Visit HCP Twitter Feed");
    QObject::connect(twitterToolButton, &QToolButton::clicked,
                     this, &RecentFilesDialog::twitterButtonClicked);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addStretch();
    layout->addWidget(hcpToolButton);
    layout->addWidget(twitterToolButton);
    layout->addStretch();
    
    return widget;
}

/**
 * Called when the HCP website button is clicked
 */
void
RecentFilesDialog::hcpWebsiteButtonClicked()
{
    websiteLinkActivated("http://www.humanconnectome.org");
}

/**
 * Called when the Twitter button is clicked
 */
void
RecentFilesDialog::twitterButtonClicked()
{
    websiteLinkActivated("http://twitter.com/#!/HumanConnectome");
}

/**
 * Called when a label's hyperlink is selected.
 * @param link
 *   The URL.
 */
void
RecentFilesDialog::websiteLinkActivated(const QString& link)
{
    if ( ! link.isEmpty()) {
        QDesktopServices::openUrl(QUrl(link));
    }
}

/**
 * Called when a files mode action is selected
 *
 * @param action
 *    Action that was selected
 */
void
RecentFilesDialog::filesModeActionTriggered(QAction* /*action*/)
{
    updateFilesTableContent();
}

/**
 * Update the files table widget's content
 */
void
RecentFilesDialog::updateFilesTableContent()
{
    RecentFileItemsFilter filter;
    filter.setNameMatching(m_nameFilterLineEdit->text().trimmed());
    
    RecentFileItemsContainer* itemsContainer(NULL);
    
    switch (getSelectedFilesMode()) {
        case RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES:
            itemsContainer = m_currentDirectoryItemsContainer.get();
            filter.setListSceneFiles(m_listSceneFilesCheckBox->isChecked());
            filter.setListSpecFiles(m_listSpecFilesCheckBox->isChecked());
            break;
        case RecentFileItemsContainerModeEnum::EXAMPLE_DATA_SETS:
            itemsContainer = m_exampleDataSetsItemsContainer.get();
            filter.setListDirectories(m_listDirectoriesCheckBox->isChecked());
            filter.setListSceneFiles(m_listSceneFilesCheckBox->isChecked());
            filter.setListSpecFiles(m_listSpecFilesCheckBox->isChecked());
            break;
        case RecentFileItemsContainerModeEnum::FAVORITES:
            updateFavoritesContainer();
            itemsContainer = m_favoriteItemsContainer.get();
            filter.setListDirectories(m_listDirectoriesCheckBox->isChecked());
            filter.setListSceneFiles(m_listSceneFilesCheckBox->isChecked());
            filter.setListSpecFiles(m_listSpecFilesCheckBox->isChecked());
            break;
        case RecentFileItemsContainerModeEnum::OTHER:
            CaretAssertMessage(0, "OTHER not used in dialog");
            break;
        case RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES:
            itemsContainer = m_recentDirectoryItemsContainer.get();
            filter.setListDirectories(true);
            break;
        case RecentFileItemsContainerModeEnum::RECENT_FILES:
            itemsContainer = m_recentFilesItemsContainer.get();
            filter.setListSceneFiles(m_listSceneFilesCheckBox->isChecked());
            filter.setListSpecFiles(m_listSpecFilesCheckBox->isChecked());
            break;
        case RecentFileItemsContainerModeEnum::RECENT_SCENES:
            itemsContainer = m_recentScenesItemsContainer.get();
            filter.setListSceneFiles(true); /* always show scenes */
            break;
    }
    
    filter.setShowFilePaths(m_showFilePathsCheckBox->isChecked());
    
    m_recentFilesTableWidget->updateContent(itemsContainer,
                                            filter);
}

/**
 * Update the contents of the favorites container
 */
void
RecentFilesDialog::updateFavoritesContainer()
{
    std::vector<RecentFileItemsContainer*> containers {
        m_recentFilesItemsContainer.get(),
        m_recentDirectoryItemsContainer.get(),
        m_recentScenesItemsContainer.get()
    };
    m_favoriteItemsContainer.reset(RecentFileItemsContainer::newInstanceFavorites(containers));
    m_modeFavoritesAction->setEnabled( ! m_favoriteItemsContainer->isEmpty());
}

/**
 * @return The selected files mode
 */
RecentFileItemsContainerModeEnum::Enum
RecentFilesDialog::getSelectedFilesMode() const
{
   RecentFileItemsContainerModeEnum::Enum mode =RecentFileItemsContainerModeEnum::RECENT_FILES;
    
    QAction* selectedAction = m_fileTypeModeActionGroup->checkedAction();
    CaretAssert(selectedAction);
    
    const AString modeName = selectedAction->data().toString();
    bool validFlag(false);
    mode = RecentFileItemsContainerModeEnum::fromName(modeName, &validFlag);
    CaretAssert(validFlag);
    
    return mode;
}

/**
 * @return Action for the given mode
 * @param recentFilesMode
 *     The mode
 */
QAction*
RecentFilesDialog::getActionForMode(const RecentFileItemsContainerModeEnum::Enum recentFilesMode) const
{
    const AString modeName(RecentFileItemsContainerModeEnum::toName(recentFilesMode));
    
    QAction* actionOut(NULL);
    
    QListIterator<QAction*> iter(m_fileTypeModeActionGroup->actions());
    while (iter.hasNext()) {
        QAction* action = iter.next();
        const AString actionDataName = action->data().toString();
        if (modeName == actionDataName) {
            actionOut = action;
            break;
        }
    }
    CaretAssert(actionOut);
    
    return actionOut;
}

/**
 * Called when Cancel button is clicked
 */
void
RecentFilesDialog::cancelButtonClicked()
{
    m_resultMode = ResultModeEnum::CANCEL;
    
    reject();
}

/**
 * Called when Load button is clicked
 */
void
RecentFilesDialog::loadButtonClicked()
{
    RecentFileItem* item = m_recentFilesTableWidget->getSelectedItem();
    if (item == NULL) {
        WuQMessageBox::warningOk(m_loadPushButton, "No item selected");
        return;
    }
    
    QPoint centerPoint(m_loadPushButton->width() / 2,
                       m_loadPushButton->height() / 2);
    QPoint point = m_loadPushButton->mapToGlobal(centerPoint);
    const bool showMenuForSpecFileFlag(false);
    loadSceneOrSpecFileFromItem(item,
                                point,
                                showMenuForSpecFileFlag);
}

/**
 * Called when Open button is clicked
 */
void
RecentFilesDialog::openButtonClicked()
{
    RecentFileItem* item = m_recentFilesTableWidget->getSelectedItem();
    if (item == NULL) {
        WuQMessageBox::warningOk(m_openPushButton, "No item selected");
        return;
    }
    
    if (item != NULL) {
        switch (item->getFileItemType()) {
            case RecentFileItemTypeEnum::DIRECTORY:
                m_resultMode = ResultModeEnum::OPEN_DIRECTORY;
                break;
            case RecentFileItemTypeEnum::EXAMPLE_SCENE:
                m_resultMode = ResultModeEnum::LOAD_EXAMPLE_SCENE_IN_SCENE_FILE;
                break;
            case RecentFileItemTypeEnum::SCENE_FILE:
                m_resultMode = ResultModeEnum::OPEN_FILE;
                break;
            case RecentFileItemTypeEnum::SCENE_IN_SCENE_FILE:
                m_resultMode = ResultModeEnum::LOAD_SCENE_FROM_SCENE_FILE;
                break;
            case RecentFileItemTypeEnum::SPEC_FILE:
                m_resultMode = ResultModeEnum::OPEN_FILE;
                break;
        }
        
        m_resultFilePathAndName = item->getPathAndFileName();
    }

    accept();
}

/**
 * Called when Open Other button is clicked
 */
void
RecentFilesDialog::openOtherButtonClicked()
{
    m_resultMode = ResultModeEnum::OPEN_OTHER;
    m_resultFilePathAndName = GuiManager::get()->getBrain()->getCurrentDirectory();
    
    accept();
}

/**
 * Called when an item is selected in the table widget
 * @param item
 * Item that was selected (may be NULL)
 */
void
RecentFilesDialog::tableWidgetItemClicked(RecentFileItem* item)
{
    bool loadValidFlag(false);
    bool openValidFlag(false);
    if (item != NULL) {
        openValidFlag = true;
        
        switch (item->getFileItemType()) {
            case RecentFileItemTypeEnum::DIRECTORY:
                break;
            case RecentFileItemTypeEnum::EXAMPLE_SCENE:
                loadValidFlag = true;
                openValidFlag = false;
                break;
            case RecentFileItemTypeEnum::SCENE_FILE:
                loadValidFlag = true;
                break;
            case RecentFileItemTypeEnum::SCENE_IN_SCENE_FILE:
                loadValidFlag = true;
                openValidFlag = false;
                break;
            case RecentFileItemTypeEnum::SPEC_FILE:
                loadValidFlag = true;
                break;
        }
    }
    
    m_loadPushButton->setEnabled(loadValidFlag);
    m_openPushButton->setEnabled(openValidFlag);
}

/**
 * Called when an item is double-clicked in the table widget
 * @param item
 * Item that was selected (may be NULL)
 */
void
RecentFilesDialog::tableWidgetItemDoubleClicked(RecentFileItem* item)
{
    if (item != NULL) {
        openButtonClicked();
    }
}

/**
 * Test reading/writing
 */
void
RecentFilesDialog::testButtonClicked()
{
    if (m_currentDirectoryItemsContainer != NULL) {
        m_currentDirectoryItemsContainer->testXmlReadingAndWriting();
    }
}

/**
 * Load the given scene or spec file
 * @param resultMode
 * The result mode
 * @param pathAndFileName
 * Path and file name of file
 * @param sceneIndex
 * Index of scene if scene file
 */
void
RecentFilesDialog::loadSceneOrSpecFile(const ResultModeEnum resultMode,
                                       const AString& pathAndFileName,
                                       const int32_t sceneIndex)
{
    m_resultMode = resultMode;
    
    bool validFlag;
    const DataFileTypeEnum::Enum dataFileType = DataFileTypeEnum::fromFileExtension(pathAndFileName, &validFlag);
    switch (dataFileType) {
        case DataFileTypeEnum::SCENE:
            m_resultFilePathAndName = pathAndFileName;
            m_resultSceneIndex      = sceneIndex + 1; /* command line indices start at 1 */
            accept();
            break;
        case DataFileTypeEnum::SPECIFICATION:
            m_resultFilePathAndName = pathAndFileName;
            accept();
            break;
        default:
            CaretAssertMessage(0, ("File type not scene nor spec: "
                                   + DataFileTypeEnum::toName(dataFileType)));
    }
}

/**
 * For the given item load the spec file or pop-up menu with scenes for a scene file
 * @param item
 * Item from which to load files
 * @param globalPostion
 * Location of mouse
 * @param showMenuForSpecFileFlag
 * If true, pop-up a menu to confirm loading a spec file
 */
void
RecentFilesDialog::loadSceneOrSpecFileFromItem(RecentFileItem* item,
                                               const QPoint& globalPosition,
                                               const bool showMenuForSpecFileFlag)
{
    switch (item->getFileItemType()) {
        case RecentFileItemTypeEnum::DIRECTORY:
            break;
        case RecentFileItemTypeEnum::SCENE_FILE:
        {
            /**
             * List scenes and allow user to load a scene bypassing the scene dialog
             */
            SceneFile sceneFile;
            try {
                sceneFile.readFile(item->getPathAndFileName());
                const int32_t numScenes = sceneFile.getNumberOfScenes();
                if (numScenes > 0) {
                    std::vector<QAction*> actions;
                    QMenu menu(this);
                    for (int32_t i = 0; i < numScenes; i++) {
                        actions.push_back(menu.addAction("Load "
                                                         + AString::number(i + 1)
                                                         + " "
                                                         + sceneFile.getSceneAtIndex(i)->getName()));
                    }
                    
                    QAction* selectedAction = menu.exec(globalPosition);
                    for (int32_t i = 0; i < numScenes; i++) {
                        CaretAssertVectorIndex(actions, i);
                        if (selectedAction == actions[i]) {
                            const Scene* scene = sceneFile.getSceneAtIndex(i);
                            CaretAssert(scene);
                            if (scene->hasFilesWithRemotePaths()) {
                                const QString msg("This scene contains files that are on the network.  "
                                                  "If accessing the files requires a username and "
                                                  "password, enter it here.  Otherwise, remove any "
                                                  "text from the username and password fields.");
                                
                                AString username;
                                AString password;
                                if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(m_loadPushButton,
                                                                                           "Username and Password",
                                                                                           msg,
                                                                                           username,
                                                                                           password)) {
                                    CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                                                     password);
                                }
                            }
                            loadSceneOrSpecFile(ResultModeEnum::LOAD_SCENE_FROM_SCENE_FILE,
                                                item->getPathAndFileName(), i);
                        }
                    }
                }
            }
            catch (const DataFileException& dfe) {
                CaretLogWarning(dfe.whatString());
            }
        }
            break;
        case RecentFileItemTypeEnum::EXAMPLE_SCENE:
        case RecentFileItemTypeEnum::SCENE_IN_SCENE_FILE:
        {
            /**
             * List scenes and allow user to load a scene bypassing the scene dialog
             */
            SceneFile sceneFile;
            try {
                sceneFile.readFile(item->getPathAndFileName());
                const Scene* scene(sceneFile.getSceneWithName(item->getSceneName()));
                const int32_t sceneIndex(sceneFile.getSceneIndexFromNumberOrName(item->getSceneName()));
                if ((scene != NULL)
                    && (sceneIndex >= 0)) {
                    if (scene->hasFilesWithRemotePaths()) {
                        const QString msg("This scene contains files that are on the network.  "
                                          "If accessing the files requires a username and "
                                          "password, enter it here.  Otherwise, remove any "
                                          "text from the username and password fields.");
                        
                        AString username;
                        AString password;
                        if (UsernamePasswordWidget::getUserNameAndPasswordInDialog(m_loadPushButton,
                                                                                   "Username and Password",
                                                                                   msg,
                                                                                   username,
                                                                                   password)) {
                            CaretDataFile::setFileReadingUsernameAndPassword(username,
                                                                             password);
                        }
                    }
                    
                    ResultModeEnum resultMode(ResultModeEnum::LOAD_SCENE_FROM_SCENE_FILE);
                    switch (item->getFileItemType()) {
                        case RecentFileItemTypeEnum::DIRECTORY:
                            break;
                        case RecentFileItemTypeEnum::SCENE_FILE:
                            break;
                        case RecentFileItemTypeEnum::EXAMPLE_SCENE:
                            resultMode = ResultModeEnum::LOAD_EXAMPLE_SCENE_IN_SCENE_FILE;
                            break;
                        case RecentFileItemTypeEnum::SCENE_IN_SCENE_FILE:
                            resultMode = ResultModeEnum::LOAD_SCENE_FROM_SCENE_FILE;
                            break;
                        case RecentFileItemTypeEnum::SPEC_FILE:
                            break;
                    }
                    loadSceneOrSpecFile(resultMode,
                                        item->getPathAndFileName(),
                                        sceneIndex);
                }
                else {
                    const AString msg("Scene \""
                                      + item->getSceneName()
                                      + "\" not found in scene file "
                                      + sceneFile.getFileName());
                    WuQMessageBox::critical(this, "ERROR", msg);
                }
            }
            catch (const DataFileException& dfe) {
                CaretLogWarning(dfe.whatString());
            }
        }
            break;
        case RecentFileItemTypeEnum::SPEC_FILE:
        {
            /*
             * Allow user to load all files in a spec file while bypassing the spec file dialogt
             */
            if (showMenuForSpecFileFlag) {
                QMenu menu(this);
                QAction* action = menu.addAction("Load all files from spec file");
                QAction* selectedAction = menu.exec(globalPosition);
                if (action == selectedAction) {
                    loadSceneOrSpecFile(ResultModeEnum::LOAD_FILES_IN_SPEC_FILE,
                                        item->getPathAndFileName(), 0);
                }
            }
            else {
                loadSceneOrSpecFile(ResultModeEnum::LOAD_FILES_IN_SPEC_FILE,
                                    item->getPathAndFileName(), 0);
            }
        }
            break;
    }
}

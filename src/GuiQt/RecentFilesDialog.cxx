
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
#include <QPushButton>
#include <QToolButton>
#include <QUrl>

#include "ApplicationInformation.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "GuiManager.h"
#include "RecentFileItem.h"
#include "RecentFileItemsContainer.h"
#include "RecentFileItemsFilter.h"
#include "RecentFilesTableWidget.h"
#include "SessionManager.h"
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
 * @param dialogTitle
 *    Title for dialog
 * @param parent
 *    The parent widget
 */
RecentFilesDialog::RecentFilesDialog(const AString& dialogTitle,
                                     QWidget* parent)
: QDialog(parent)
{
    setWindowTitle(dialogTitle);

    QWidget* internetButtonsWidget = createInternetButtonsWidget();
    
    QWidget* fileTypeButtonsWidget = createFileTypesButtonWidget();
    
    QWidget* filesFilteringWidget = createFilesFilteringWidget();
    
    m_recentFilesTableWidget = new RecentFilesTableWidget();
    QObject::connect(m_recentFilesTableWidget, &RecentFilesTableWidget::sortingChanged,
                     this, &RecentFilesDialog::updateFilesTableContent);

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
    int row(0);
    dialogLayout->addWidget(internetButtonsWidget, 0, 0, 2, 1);
    dialogLayout->addWidget(filesFilteringWidget, 0, 1);
    row++;
    dialogLayout->addWidget(fileTypeButtonsWidget, 2, 0);
    dialogLayout->addWidget(m_recentFilesTableWidget, 1, 1, 2, 1);
    row++;
    dialogLayout->addWidget(dialogButtonWidget, 3, 1);

    m_openPushButton->setAutoDefault(true);
    m_openPushButton->setDefault(true);
    
    Brain* brain = GuiManager::get()->getBrain();
    m_currentDirectoryItemsContainer.reset(RecentFileItemsContainer::newInstanceSceneAndSpecFilesInDirectory(brain->getCurrentDirectory()));
    
    CaretPreferences* preferences = SessionManager::get()->getCaretPreferences();
    m_recentFilesItemsContainer.reset(RecentFileItemsContainer::newInstanceRecentSceneAndSpecFiles(preferences,
                                                                                                   RecentFileItemsContainer::WriteIfModifiedType::WRITE_YES));
    
    m_recentDirectoryItemsContainer.reset(RecentFileItemsContainer::newInstanceRecentDirectories(preferences,
                                                                                                 RecentFileItemsContainer::WriteIfModifiedType::WRITE_YES));
    
    /*
     * Default to a non-empty container
     */
    RecentFileItemsContainerModeEnum::Enum selectedMode = RecentFileItemsContainerModeEnum::RECENT_FILES;
    if ( ! m_recentFilesItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::RECENT_FILES;
    }
    else if ( ! m_recentDirectoryItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES;
    }
    else if ( ! m_currentDirectoryItemsContainer->isEmpty()) {
        selectedMode = RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES;
    }
    
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
 * @param parent
 *     Parent for dialog
 * @return
 *     Result enumerated value
 */
RecentFilesDialog::ResultModeEnum
RecentFilesDialog::runDialog(const RunMode runMode,
                             AString& nameOut,
                             QWidget* parent)
{
    AString dialogTitle;
    ApplicationInformation appInfo;
    switch (runMode) {
        case RunMode::OPEN_RECENT:
            dialogTitle = "Open Recent";
            break;
        case RunMode::SPLASH_SCREEN:
            dialogTitle = (appInfo.getName()
                           + " "
                           + appInfo.getVersion());
            break;
    }
    
    RecentFilesDialog rfd(dialogTitle,
                          parent);
    rfd.exec();
    
    ResultModeEnum resultMode = rfd.getResultMode();
    nameOut = rfd.getSelectedDirectoryOrFileName();
    
    return resultMode;
}

/**
 *
 */
RecentFilesDialog::ResultModeEnum
RecentFilesDialog::getResultMode()
{
    return m_resultMode;
}

/**
 * @return The selected directory or file name
 */
AString
RecentFilesDialog::getSelectedDirectoryOrFileName()
{
    return m_resultFilePathAndName;
}

/**
 * @return New instance files filtering widget
 */
QWidget*
RecentFilesDialog::createFilesFilteringWidget()
{
    QLabel* showLabel = new QLabel("Show: ");
    m_showSceneFilesCheckBox = new QCheckBox("Scene");
    m_showSceneFilesCheckBox->setChecked(true);
    QObject::connect(m_showSceneFilesCheckBox, &QCheckBox::clicked,
                     this, &RecentFilesDialog::showSceneFilesCheckBoxClicked);
    
    m_showSpecFilesCheckBox = new QCheckBox("Spec");
    m_showSpecFilesCheckBox->setChecked(true);
    QObject::connect(m_showSpecFilesCheckBox, &QCheckBox::clicked,
                     this, &RecentFilesDialog::showSceneFilesCheckBoxClicked);
    
    QLabel* nameFilterLabel = new QLabel("Name Filter: ");
    m_nameFilterLineEdit = new QLineEdit();
    m_nameFilterLineEdit->setClearButtonEnabled(true);
    m_nameFilterLineEdit->setFixedWidth(250);
    QObject::connect(m_nameFilterLineEdit, &QLineEdit::textEdited,
                     this, &RecentFilesDialog::nameFilterTextEdited);
    m_nameFilterLineEdit->setToolTip(RecentFileItemsFilter::getMatchingLineEditToolTip());
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addWidget(showLabel);
    layout->addWidget(m_showSceneFilesCheckBox);
    layout->addWidget(m_showSpecFilesCheckBox);
    layout->addSpacing(25);
    layout->addWidget(nameFilterLabel);
    layout->addWidget(m_nameFilterLineEdit);
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

    m_openPushButton = new QPushButton("Open");
    QObject::connect(m_openPushButton, &QPushButton::clicked,
                     this, &RecentFilesDialog::openButtonClicked);

    QPushButton* openOtherPushButton = new QPushButton("Open Other...");
    QObject::connect(openOtherPushButton, &QPushButton::clicked,
                     this, &RecentFilesDialog::openOtherButtonClicked);

    QPushButton* cancelPushButton = new QPushButton("Cancel");
    QObject::connect(cancelPushButton, &QPushButton::clicked,
                     this, &RecentFilesDialog::cancelButtonClicked);

    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->addStretch();
    if (testPushButton != NULL) {
        layout->addWidget(testPushButton);
        layout->addSpacing(50);
    }
    layout->addWidget(openOtherPushButton);
    layout->addWidget(m_openPushButton);
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
        action->setToolTip("");
        
        QToolButton* tb = new QToolButton();
        tb->setDefaultAction(action);
        toolButtons.push_back(tb);
    }
    
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
 * Called if Show Scene Files checkbox is clicked
 */
void
RecentFilesDialog::showSceneFilesCheckBoxClicked(bool /*checked*/)
{
    updateFilesTableContent();
}

/**
 * Called if Show Spec Files checkbox is clicked
 */
void
RecentFilesDialog::showSpecFilesCheckBoxClicked(bool /*checked*/)
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
RecentFilesDialog::filesModeActionTriggered(QAction* action)
{
    CaretAssert(action);
    
    updateFilesTableContent();
}

/**
 * Update the files table widget's content
 */
void
RecentFilesDialog::updateFilesTableContent()
{
    RecentFileItemsFilter filter;
    RecentFileItemsContainer* itemsContainer(NULL);
    
    switch (getSelectedFilesMode()) {
        case RecentFileItemsContainerModeEnum::DIRECTORY_SCENE_AND_SPEC_FILES:
            itemsContainer = m_currentDirectoryItemsContainer.get();
            filter.setShowSceneFiles(m_showSceneFilesCheckBox->isChecked());
            filter.setShowSpecFiles(m_showSpecFilesCheckBox->isChecked());
            filter.setNameMatching(m_nameFilterLineEdit->text().trimmed());
            break;
        case RecentFileItemsContainerModeEnum::FAVORITES:
            filter.setFavoritesOnly(true);
            break;
        case RecentFileItemsContainerModeEnum::OTHER:
            CaretAssertMessage(0, "OTHER not used in dialog");
            break;
        case RecentFileItemsContainerModeEnum::RECENT_DIRECTORIES:
            itemsContainer = m_recentDirectoryItemsContainer.get();
            filter.setShowDirectories(true);
            filter.setNameMatching(m_nameFilterLineEdit->text().trimmed());
            break;
        case RecentFileItemsContainerModeEnum::RECENT_FILES:
            itemsContainer = m_recentFilesItemsContainer.get();
            filter.setShowSceneFiles(m_showSceneFilesCheckBox->isChecked());
            filter.setShowSpecFiles(m_showSpecFilesCheckBox->isChecked());
            filter.setNameMatching(m_nameFilterLineEdit->text().trimmed());
            break;
    }
    
    m_recentFilesTableWidget->updateContent(itemsContainer,
                                            filter);
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
    mode =RecentFileItemsContainerModeEnum::fromName(modeName, &validFlag);
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
            case RecentFileItemTypeEnum::SCENE_FILE:
                m_resultMode = ResultModeEnum::OPEN_FILE;
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
    m_openPushButton->setEnabled(item != NULL);
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


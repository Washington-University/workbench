
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
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTableWidget>
#include <QToolButton>

#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::RecentFilesDialog 
 * \brief Dialog for opening recent files and also functions as splash dialog
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
RecentFilesDialog::RecentFilesDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Open Recent Files");

    QWidget* fileTypeButtonsWidget = createFileTypesButtonWidget();
    
    QWidget* filesFilteringWidget = createFilesFilteringWidget();
    
    m_filesTableWidget = new QTableWidget();

    QWidget* dialogButtonWidget = createDialogButtonsWidget();
    
    QGridLayout* dialogLayout = new QGridLayout(this);
    QMargins layoutMargins = dialogLayout->contentsMargins();
    layoutMargins.setBottom(0);
    layoutMargins.setLeft(0);
    layoutMargins.setTop(0);
    dialogLayout->setContentsMargins(layoutMargins);
    dialogLayout->setHorizontalSpacing(0);
    dialogLayout->setVerticalSpacing(0);
    dialogLayout->setRowStretch(1, 100);
    dialogLayout->setColumnStretch(1, 100);
    int row(0);
    dialogLayout->addWidget(filesFilteringWidget, row, 1);
    row++;
    dialogLayout->addWidget(fileTypeButtonsWidget, row, 0);
    dialogLayout->addWidget(m_filesTableWidget, row, 1);
    row++;
    dialogLayout->addWidget(dialogButtonWidget, row, 1);

    m_openPushButton->setAutoDefault(true);
    m_openPushButton->setDefault(true);
    
    CaretAssertToDoWarning(); // need to get from preferences
    const RecentFilesModeEnum::Enum selectedMode = RecentFilesModeEnum::RECENT_FILES;
    QAction* selectedAction = getActionForMode(selectedMode);
    selectedAction->trigger();
}

/**
 * Destructor.
 */
RecentFilesDialog::~RecentFilesDialog()
{
}

/**
 * Run the dialog
 *
 * @param nameOut
 *     Output with name of selected directory or file name
 * @param parent
 *     Parent for dialog
 * @return
 *     Result enumerated value
 */
RecentFilesDialog::ResultModeEnum
RecentFilesDialog::runDialog(AString& nameOut,
                             QWidget* parent)
{
    RecentFilesDialog rfd(parent);
    rfd.exec();
    
    ResultModeEnum resultMode = rfd.getResultMode();
    switch (resultMode) {
        case ResultModeEnum::CANCEL:
            std::cout << "RESULT: Cancel" << std::endl;
            break;
        case ResultModeEnum::OPEN_DIRECTORY:
            std::cout << "RESULT: Open Directory" << std::endl;
            break;
        case ResultModeEnum::OPEN_FILE:
            std::cout << "RESULT: Open File" << std::endl;
            break;
        case ResultModeEnum::OPEN_OTHER:
            std::cout << "RESULT: Open Other" << std::endl;
            break;
    }
    
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
    AString nameOut;
    
    return nameOut;
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
    m_nameFilterLineEdit->setFixedWidth(250);
    QObject::connect(m_nameFilterLineEdit, &QLineEdit::textEdited,
                     this, &RecentFilesDialog::nameFilterTextEdited);
    
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
    m_filesModeActionGroup = new QActionGroup(this);
    
    std::vector<RecentFilesModeEnum::Enum> modes;
    RecentFilesModeEnum::getAllEnums(modes);
    
    std::vector<QToolButton*> toolButtons;
    for (auto m : modes) {
        AString modeName(RecentFilesModeEnum::toGuiName(m));
        int32_t spaceIndex = modeName.indexOf(' ');
        if (spaceIndex > 0) {
            /* items with multiple words span to two lines */
            modeName[spaceIndex] = '\n';
        }
        
        QAction* action = m_filesModeActionGroup->addAction(modeName);
        action->setData(RecentFilesModeEnum::toName(m));
        action->setCheckable(true);
        
        QToolButton* tb = new QToolButton();
        tb->setDefaultAction(action);
        toolButtons.push_back(tb);
    }
    
    std::vector<QWidget*> toolButtonWidgets(toolButtons.begin(),
                                            toolButtons.end());
    WuQtUtilities::matchWidgetWidths(toolButtonWidgets);
    
    QObject::connect(m_filesModeActionGroup, &QActionGroup::triggered,
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
RecentFilesDialog::nameFilterTextEdited(const QString& text)
{
    
}

/**
 * Called if Show Scene Files checkbox is clicked
 */
void
RecentFilesDialog::showSceneFilesCheckBoxClicked(bool checked)
{
    
}

/**
 * Called if Show Spec Files checkbox is clicked
 */
void
RecentFilesDialog::showSpecFilesCheckBoxClicked(bool checked)
{
    
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
    
    std::cout << "Action selected: " << action->text() << std::endl;
}

/**
 * @return The selected files mode
 */
RecentFilesModeEnum::Enum
RecentFilesDialog::getSelectedFilesMode() const
{
    RecentFilesModeEnum::Enum mode = RecentFilesModeEnum::RECENT_FILES;
    
    QAction* selectedAction = m_filesModeActionGroup->checkedAction();
    CaretAssert(selectedAction);
    
    const AString modeName = selectedAction->data().toString();
    bool validFlag(false);
    mode = RecentFilesModeEnum::fromName(modeName, &validFlag);
    CaretAssert(validFlag);
    
    return mode;
}

/**
 * @return Action for the given mode
 * @param recentFilesMode
 *     The mode
 */
QAction*
RecentFilesDialog::getActionForMode(const RecentFilesModeEnum::Enum recentFilesMode) const
{
    const AString modeName(RecentFilesModeEnum::toName(recentFilesMode));
    
    QAction* actionOut(NULL);
    
    QListIterator<QAction*> iter(m_filesModeActionGroup->actions());
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
    m_resultMode = ResultModeEnum::OPEN_FILE;
    
    m_resultMode = ResultModeEnum::OPEN_DIRECTORY;
    
    accept();
}

/**
 * Called when Open Other button is clicked
 */
void
RecentFilesDialog::openOtherButtonClicked()
{
    m_resultMode = ResultModeEnum::OPEN_OTHER;
    
    accept();
}


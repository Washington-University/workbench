
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __LOCK_ASPECT_WARNING_DIALOG_DECLARE__
#include "LockAspectWarningDialog.h"
#undef __LOCK_ASPECT_WARNING_DIALOG_DECLARE__

#include <algorithm>

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QCheckBox>
#include <QVBoxLayout>

#include "BestPracticesDialog.h"
#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::LockAspectWarningDialog 
 * \brief Dialog that warns and allows user to lock tab/window aspect.
 * \ingroup GuiQt
 */

/**
 * Run the lock aspect warning dialog and return the result.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parent
 *     The parent widget.
 * @return
 *     Result of user interaction with dialog.
 */
LockAspectWarningDialog::Result
LockAspectWarningDialog::runDialog(const int32_t browserWindowIndex,
                                   QWidget* parent)
{
    const BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
    CaretAssert(bbw);
    const BrowserWindowContent* browserWindowContent = bbw->getBrowerWindowContent();
    CaretAssert(browserWindowContent);
    
    const bool aspectLockedFlag = (browserWindowContent->isWindowAspectLocked()
                                   && browserWindowContent->isAllTabsInWindowAspectRatioLocked());
    if (aspectLockedFlag) {
        return Result::NO_CHANGES;
    }
    
    const BrowserTabContent* selectedTab = bbw->getBrowserTabContent();
    if (selectedTab == NULL) {
        return Result::NO_CHANGES;
    }
    
    if (s_doNotShowAgainStatusFlag) {
        return Result::NO_CHANGES;
    }
    
    LockAspectWarningDialog dialog(parent);
    
    if (dialog.exec() == LockAspectWarningDialog::Accepted) {
        s_doNotShowAgainStatusFlag = dialog.isDoNotShowAgainChecked();
        return dialog.getResult();
    }
    
    return Result::CANCEL;
}

/**
 * Constructor.
 *
 * @param tabMode
 *     The mode for tabs (selected or all)
 * @param tileTabsEnabled
 *     True if tile tabs is enabled.
 * @param browserWindowAspectLocked
 *     True if window aspect is locked.
 * @param tabAspectLockedCount
 *     Count of tabs with aspect locked.
 * @param tabCount
 *     Count of tabs.
 * @param parent
 *     The parent widget.
 */
LockAspectWarningDialog::LockAspectWarningDialog(QWidget* parent)
: QDialog(parent)
{
    setWindowTitle("Warning: Aspect Unlocked");
    
    const QString helpButtonText("More Info");
    
    const AString msg = BestPracticesDialog::getTextForInfoMode(BestPracticesDialog::InfoMode::LOCK_ASPECT_BEST_PRACTICES,
                                                                BestPracticesDialog::TextMode::BRIEF);
//    const AString msg("<html>"
//                      "Aspect is unlocked and it is <b>strongly recommended</b> that aspect is locked and is never unlocked "
//                      "when annotations are present.  Failing to lock aspect or unlocking the aspect may "
//                      "cause annotations to move from their original locations."
//                      "<p>"
//                      "When creating annotations and scenes, follow these best practices:"
//                      "<ol>"
//                      "<li> If desired, enter Tile Tabs for a multi-tab Scene (View Menu->Enter Tile Tabs)"
//                      "<li> Adjust size of window"
//                      "<li> Setup view of the model(s) (pan/rotate/zoom)"
//                      "<li> Lock Aspect Ratio (click Lock Aspect button in right side of Toolbar)"
//                      "<li> Enter Annotations Mode (click Annotate button in Toolbar)"
//                      "<li> Add Annotations"
//                      "<li> Display the Scene Dialog (click Clapboard icon in Toolbar or select Window Menu->Scenes)"
//                      "<li> Add a Scene (click Add button on Scene Dialog)"
//                      "<li> Save the Scene File (click Save button (or Save As if Save is disabled) at top of Scene Dialog)"
//                      "<li> Repeat last few steps as needed"
//                      "</ol>"
//                      "</html>");
    QLabel* warningLabel = new QLabel(msg);
    warningLabel->setWordWrap(true);
    
    QLabel* moreLabel = new QLabel("<html>"
                                   "For a more detailed explanation, click the <b>" + helpButtonText + "</b> button below.  "
                                   "Note: This dialog will close if the <b> " + helpButtonText + " </b>  is clicked."
                                   "</html>");
    QPushButton* helpButton = new QPushButton(helpButtonText + "...");
    QObject::connect(helpButton, &QPushButton::clicked,
                     this, &LockAspectWarningDialog::helpButtonClicked);
    
    QLabel* buttonsLabel = new QLabel("Continue to Annotations Mode?");
    
    QPushButton* lockWindowAndTabButton = new QPushButton("OK - Lock Aspect");
    QObject::connect(lockWindowAndTabButton, &QPushButton::clicked,
                     [=] { this->buttonClicked(Result::LOCK_ASPECT); });
    
    QPushButton* noChangesButton = new QPushButton("OK - Aspect Remains Unlocked");
    QObject::connect(noChangesButton, &QPushButton::clicked,
                     [=] { this->buttonClicked(Result::NO_CHANGES); });
    
    QPushButton* cancelButton = new QPushButton("Cancel");
    QObject::connect(cancelButton, &QPushButton::clicked,
                     [=] { this->buttonClicked(Result::CANCEL); });
    
    m_doNotShowAgainCheckBox = new QCheckBox("Do not show again and user will be\n"
                                              "responsible for aspect locking/unlocking");
    
    QVBoxLayout* buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(buttonsLabel);
    buttonLayout->addWidget(lockWindowAndTabButton);
    buttonLayout->addWidget(noChangesButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    buttonLayout->addWidget(m_doNotShowAgainCheckBox);
    buttonLayout->addStretch();

    QVBoxLayout* helpLayout = new QVBoxLayout();
    helpLayout->addWidget(warningLabel);
    helpLayout->addWidget(moreLabel);
    helpLayout->addWidget(helpButton, 0, Qt::AlignHCenter);
    helpLayout->addStretch();
    
    WuQtUtilities::matchWidgetWidths(lockWindowAndTabButton,
                                     noChangesButton,
                                     cancelButton);
    
    QHBoxLayout* dialogLayout = new QHBoxLayout(this);
    dialogLayout->addLayout(helpLayout);
    dialogLayout->addLayout(buttonLayout);
    
    /*
     * Highlighted button
     */
    QPushButton* defaultPushButton = lockWindowAndTabButton;
    defaultPushButton->setAutoDefault(true);
    defaultPushButton->setDefault(true);
}

/**
 * Destructor.
 */
LockAspectWarningDialog::~LockAspectWarningDialog()
{
}

/**
 * @return The selected lock status
 */
LockAspectWarningDialog::Result
LockAspectWarningDialog::getResult() const
{
    return m_result;
}

/**
 * Called when help button is clicked.
 */
void
LockAspectWarningDialog::helpButtonClicked()
{
    /*
     * Best practices dialog will destroy itself when closed.
     * Use parent widget so that user may close this dialog but allow
     * best practices dialog to remain open.
     */
    BestPracticesDialog* dialog = new BestPracticesDialog(BestPracticesDialog::InfoMode::LOCK_ASPECT_BEST_PRACTICES,
                                                          parentWidget());
    dialog->showDialog();
    
    buttonClicked(Result::CANCEL);
}

/**
 * Called when one of the buttons is clicked.
 *
 * @param buttonClicked
 *     Button that was clicked.
 */
void
LockAspectWarningDialog::buttonClicked(const Result buttonClicked)
{
    m_result = buttonClicked;
    switch (m_result) {
        case Result::LOCK_ASPECT:
            accept();
            break;
        case Result::NO_CHANGES:
            accept();
            break;
        case Result::CANCEL:
            reject();
            break;
    }
}


/**
 * @return True if the "do not show again" checkbox is checked.
 */
bool
LockAspectWarningDialog::isDoNotShowAgainChecked() const
{
    return m_doNotShowAgainCheckBox->isChecked();
}



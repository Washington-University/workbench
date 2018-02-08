
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

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventTabAndWindowLockAspectRatioStatus.h"
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
    const LockAspectWarningDialog::TabMode tabMode = LockAspectWarningDialog::TabMode::SELECTED_TAB;
    
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
    CaretAssert(bbw);
    
    BrowserTabContent* selectedTab = bbw->getBrowserTabContent();
    if (selectedTab == NULL) {
        return Result::NO_CHANGES;
    }
    
    if (s_doNotShowAgainStatusFlag) {
        return Result::NO_CHANGES;
    }
    
    EventTabAndWindowLockAspectRatioStatus lockEvent;
    EventManager::get()->sendEvent(lockEvent.getPointer());
    CaretAssert(lockEvent.getEventProcessCount() > 0);
    
    /*
     * Determine lock status for window and tabs in the window
     */
    const bool tileTabsEnabledFlag    = lockEvent.getWindowStatus(browserWindowIndex)->isTileTabsEnabled();
    const bool windowAspectLockedFlag = lockEvent.getWindowStatus(browserWindowIndex)->isAspectRatioLocked();
    const bool selectedTabLockedFlag = lockEvent.getTabStatus(selectedTab->getTabNumber())->isAspectRatioLocked();
    
    if (windowAspectLockedFlag
        || selectedTabLockedFlag) {
        return Result::NO_CHANGES;
    }

    LockAspectWarningDialog dialog(tabMode,
                                   tileTabsEnabledFlag,
                                   windowAspectLockedFlag,
                                   lockEvent.getWindowStatus(browserWindowIndex)->getTabLockedCount(),
                                   lockEvent.getWindowStatus(browserWindowIndex)->getTabCount(),
                                   parent);
    
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
LockAspectWarningDialog::LockAspectWarningDialog(const TabMode tabMode,
                                                 const bool tileTabsEnabled,
                                                 const bool browserWindowAspectLocked,
                                                 const int32_t tabAspectLockedCount,
                                                 const int32_t tabCount,
                                                 QWidget* parent)
: QDialog(parent),
  m_tabMode(tabMode),
  m_browserWindowAspectLocked(browserWindowAspectLocked),
m_tabAspectLockedCount(tabAspectLockedCount),
m_tabCount(tabCount)
{
    AString tabText;
    switch (m_tabMode) {
        case TabMode::ALL_TABS:
            tabText = "all Tab Aspect Ratios";
            break;
        case TabMode::SELECTED_TAB:
            tabText = "the Selected Tab Aspect Ratio";
            break;
    }
    
    const AString msg("<html>"
                      "Neither the Window Aspect Ratio nor " + tabText + " are locked.  "
                      "Prior to entering Annotations Mode and creating annotations, "
                      "it is recommended that the user adjust the window "
                      "to the desired size, enter Tile Tabs (if desired), "
                      "and then lock the aspect ratio for the Window and All Tabs.  "
                      "<P>"
                      "Tab and Window annotations are displayed "
                      "in <B>percentage coordinates</B> of the tab/window "
                      "width and height (0% is at bottom/left and 100% "
                      "is at top/right).  Locking the aspect ratio ensures that these "
                      "annotations remain in the correct location when the window "
                      "size changes.  "
                      "</html>");
    QLabel* warningLabel = new QLabel(msg);
    warningLabel->setWordWrap(true);
    
    QLabel* buttonsLabel = new QLabel("Continue to Annotations Mode?");
    
    QPushButton* lockWindowAndTabButton = new QPushButton("OK - Lock Window and All Tab Aspects");
    QObject::connect(lockWindowAndTabButton, &QPushButton::clicked,
                     [=] { this->buttonClicked(Result::LOCK_WINDOW_ASPECT_AND_ALL_TAB_ASPECTS); });
    
    QPushButton* noChangesButton = new QPushButton("OK - No Lock Aspect Changes");
    QObject::connect(noChangesButton, &QPushButton::clicked,
                     [=] { this->buttonClicked(Result::NO_CHANGES); });
    
    QPushButton* cancelButton = new QPushButton("Cancel");
    QObject::connect(cancelButton, &QPushButton::clicked,
                     [=] { this->buttonClicked(Result::CANCEL); });
    
    m_doNotShowAgainCheckBox = new QCheckBox("Do not show again and user will be\n"
                                              "responsible for aspect locking/unlocking");
    
    QVBoxLayout* labelLayout = new QVBoxLayout();
    labelLayout->addWidget(warningLabel);
    labelLayout->addStretch();
    
    QBoxLayout* dialogLayout = new QHBoxLayout(this);
    
    WuQtUtilities::matchWidgetWidths(lockWindowAndTabButton,
                                     noChangesButton,
                                     cancelButton);
    
    QBoxLayout* buttonLayout = new QVBoxLayout();
    buttonLayout->addWidget(buttonsLabel);
    buttonLayout->addWidget(lockWindowAndTabButton);
    buttonLayout->addWidget(noChangesButton);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    buttonLayout->addWidget(m_doNotShowAgainCheckBox);
    buttonLayout->addStretch();
    
    dialogLayout->addLayout(labelLayout);
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
 * @return Message with current locked status
 */
QString
LockAspectWarningDialog::getLockedStatusText() const
{
    AString windowMessage("Window Aspect: "
                          + AString((m_browserWindowAspectLocked ? "Locked" : "Unlocked")));
    
    AString tabMessage("Tab Aspect: ");
    if (m_tabAspectLockedCount == 0) {
        tabMessage.append("All Unlocked");
    }
    else if (m_tabAspectLockedCount == m_tabCount) {
        tabMessage.append("All Locked");
    }
    else  {
        tabMessage.append(AString::number(m_tabAspectLockedCount)
                          + " of "
                          + AString::number(m_tabCount)
                          + " Locked");
    }
    
    AString lockedMessage(windowMessage
                          + "\n"
                          + tabMessage);
    
    return lockedMessage;
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
    switch (buttonClicked) {
        case Result::LOCK_WINDOW_ASPECT_AND_ALL_TAB_ASPECTS:
            m_result = Result::LOCK_WINDOW_ASPECT_AND_ALL_TAB_ASPECTS;
            accept();
            break;
        case Result::NO_CHANGES:
            m_result = Result::NO_CHANGES;
            accept();
            break;
        case Result::CANCEL:
            m_result = Result::CANCEL;
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



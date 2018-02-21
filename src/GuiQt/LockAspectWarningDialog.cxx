
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

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
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
        return dialog.getOkResult();
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
: WuQDialogModal("Enter Annotations Mode",
                 parent)
{
    const QString mainInstructions("Do you want to lock the aspect ratio while entering annotations mode?");
    
    const QString supplementalInstructions("<html>"
                                           "Prior to locking the aspect ratio, the user should adjust the size "
                                           "of the window and optionally enable Tile Tabs for a multi-tab view.  "
                                           "If this has not been done, click the <i>Cancel</i> button, make those "
                                           "adjustments, and then click the <i>Toolbar's Annotate Mode</i> button."
                                           "<P>"
                                           "View the <a href=\"Link\">Best Practices Guide</a> (this dialog "
                                           "will close).  This guide explains the importance of aspect "
                                           "locking and documents procedures for successful creation of annotations "
                                           "and scenes."
                                           "</html>");
    
    const QString lockAspectInstructions("Locking the aspect ratio, and never unlocking the aspect "
                                         "ratio, ensures annotations stay in the correct location.");
    const QString leaveUnlockedInstructions("Advanced users may choose to lock and unlock the aspect ratio");
    
    QLabel* mainInstructionsLabel = new QLabel(mainInstructions);
    QFont font = mainInstructionsLabel->font();
    font.setPointSize(font.pointSize() * 1.4);
    font.setBold(true);
    mainInstructionsLabel->setFont(font);
    
    QLabel* supplementalInstructionLabel = new QLabel(supplementalInstructions);
    supplementalInstructionLabel->setWordWrap(true);
    QObject::connect(supplementalInstructionLabel, &QLabel::linkActivated,
                     this, &LockAspectWarningDialog::detailsLabelLinkActivated);
    
    QLabel* lockAspectLabel = new QLabel(lockAspectInstructions);
    lockAspectLabel->setWordWrap(true);
    QLabel* lockAspectRadioButtonLabel = new QLabel("Lock Aspect Ratio (Recommended)");
    m_lockAspectRadioButton = new QRadioButton();
    
    QLabel* leaveUnlockedAspectRadioButtonLabel = new QLabel("Leave Aspect Ratio Unlocked");
    m_leaveUnlockedAspectRadioButton = new QRadioButton();
    QLabel* leaveUnlockedLabel = new QLabel(leaveUnlockedInstructions);
    
    QButtonGroup* buttGroup = new QButtonGroup(this);
    buttGroup->addButton(m_lockAspectRadioButton);
    buttGroup->addButton(m_leaveUnlockedAspectRadioButton);
    m_lockAspectRadioButton->setChecked(true);
    
    m_doNotShowAgainCheckBox = new QCheckBox("Do not show again.  User will not be warned about "
                                             "aspect locking and unlocking.");
    
    /*
     * No text is added to readio buttons since 
     * radio buttons and labels seem to get a different
     * height in a grid layout.
     */
    const int COL_RADIO_EMPTY  = 0;
    const int COL_RADIO_BUTTON = 1;
    const int COL_RADIO_LABEL  = 2;
    const int COL_RADIO_INFO   = 3;
    const int COL_RADIO_STRETCH = 4;
    QGridLayout* buttonGridLayout = new QGridLayout();
    buttonGridLayout->setVerticalSpacing(4);
    buttonGridLayout->setColumnMinimumWidth(COL_RADIO_EMPTY, 20);
    buttonGridLayout->setColumnMinimumWidth(COL_RADIO_LABEL, 20);
    buttonGridLayout->setColumnStretch(COL_RADIO_EMPTY, 0);
    buttonGridLayout->setColumnStretch(COL_RADIO_BUTTON, 0);
    buttonGridLayout->setColumnStretch(COL_RADIO_STRETCH, 100);
    int row = 0;
    buttonGridLayout->addWidget(m_lockAspectRadioButton, row, COL_RADIO_BUTTON);
    buttonGridLayout->addWidget(lockAspectRadioButtonLabel, row, COL_RADIO_LABEL, 1, 2);
    row++;
    buttonGridLayout->addWidget(lockAspectLabel, row, COL_RADIO_INFO);
    row++;
    buttonGridLayout->addWidget(m_leaveUnlockedAspectRadioButton, row, COL_RADIO_BUTTON);
    buttonGridLayout->addWidget(leaveUnlockedAspectRadioButtonLabel, row, COL_RADIO_LABEL, 1, 2);
    row++;
    buttonGridLayout->addWidget(leaveUnlockedLabel, row, COL_RADIO_INFO);
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(mainInstructionsLabel);
    dialogLayout->addLayout(buttonGridLayout);
    dialogLayout->addSpacing(10);
    dialogLayout->addWidget(supplementalInstructionLabel);
    dialogLayout->addSpacing(10);
    dialogLayout->addWidget(m_doNotShowAgainCheckBox);
    
    setCentralWidget(dialogWidget,
                     WuQDialogModal::SCROLL_AREA_NEVER);
    
    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    
}

/**
 * Destructor.
 */
LockAspectWarningDialog::~LockAspectWarningDialog()
{
}

/**
 * @return The result if the user clicked the OK button.
 */
LockAspectWarningDialog::Result
LockAspectWarningDialog::getOkResult() const
{
    if (m_lockAspectRadioButton->isChecked()) {
        return Result::LOCK_ASPECT;
    }
    else if (m_leaveUnlockedAspectRadioButton->isChecked()) {
        return Result::NO_CHANGES;
    }
    else {
        CaretAssert(0);
    }
    
    return m_result;
}

/**
 * Called when link in details label is clicked
 *
 * @param text of the link.
 */
void
LockAspectWarningDialog::detailsLabelLinkActivated(const QString& /*link*/)
{
    /*
     * Best practices dialog will destroy itself when closed.
     * Use parent widget so that user may close this dialog but allow
     * best practices dialog to remain open.
     */
    BestPracticesDialog* dialog = new BestPracticesDialog(BestPracticesDialog::InfoMode::LOCK_ASPECT_BEST_PRACTICES,
                                                          parentWidget());
    dialog->showDialog();
    
    reject();
}


/**
 * @return True if the "do not show again" checkbox is checked.
 */
bool
LockAspectWarningDialog::isDoNotShowAgainChecked() const
{
    return m_doNotShowAgainCheckBox->isChecked();
}



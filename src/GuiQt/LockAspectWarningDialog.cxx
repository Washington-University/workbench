
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

#include "BrainBrowserWindow.h"
#include "BrowserTabContent.h"
#include "BrowserWindowContent.h"
#include "CaretAssert.h"
#include "EventHelpViewerDisplay.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::LockAspectWarningDialog 
 * \brief Dialog that warns and allows user to lock tab/window aspect.
 * \ingroup GuiQt
 */

/**
 * Run the dialog for when the user turns on the Lock Aspect button in the toolbar
 *
 * @param bbw
 *     Parent brain browser window
 * @param numberOfTabsInWindow
 *     Number of tabs in the window
 */
bool
LockAspectWarningDialog::runDialogToolBarLockAspect(BrainBrowserWindow* bbw,
                                                    const int32_t numberOfTabsInWindow)
{
    CaretAssert(bbw);

    /*
     * Show warning if all of these conditions are met:
     *
     * (1) User has NOT set do not show again
     * (2) Tile tabs is NOT selected
     * (3) There is more than one tab in the window
     */
    if (s_doNotShowAgainLockAspectModeStatusFlag) {
        return true;
    }
    if (bbw->isTileTabsSelected()) {
        return true;
    }
    if (numberOfTabsInWindow <= 1) {
        return true;
    }
    
    LockAspectWarningDialog dialog(bbw,
                                   Mode::TOOLBAR_LOCK_ASPECT);
    
    
    const bool acceptedFlag = (dialog.exec() == LockAspectWarningDialog::Accepted);
    s_doNotShowAgainLockAspectModeStatusFlag = dialog.isDoNotShowAgainChecked();
    
    return acceptedFlag;
}


/**
 * Run the lock aspect warning dialog when the user presses the
 * Annotation mode button and return the result.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 * @return
 *     Result of user interaction with dialog.
 */
LockAspectWarningDialog::Result
LockAspectWarningDialog::runDialogEnterAnnotationsMode(const int32_t browserWindowIndex)
{
    BrainBrowserWindow* bbw = GuiManager::get()->getBrowserWindowByWindowIndex(browserWindowIndex);
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
    
    if (s_doNotShowAgainEnterAnnotationsModeStatusFlag) {
        return Result::NO_CHANGES;
    }
    
    LockAspectWarningDialog dialog(bbw,
                                   Mode::ENTER_ANNOTATIONS_MODE);
    
    if (dialog.exec() == LockAspectWarningDialog::Accepted) {
        s_doNotShowAgainEnterAnnotationsModeStatusFlag = dialog.isDoNotShowAgainChecked();
        return dialog.getOkResult();
    }
    
    return Result::CANCEL;
}

/**
 * @return Locking aspect instruction containing importance of window sizing
 * and locking tabs.
 *
 * @parm buttonText
 *     Text of button that user has clicked
 * @param showBestPracticesLink
       If true, include a link to the best practices guide.
 */
QString
LockAspectWarningDialog::getLockingInstructionsText(const QString& buttonText,
                                                    const bool showBestPracticesLink)
{
    QString text("<html>"
                 "Prior to locking the aspect ratio, the user should: "
                 "<ul>"
                 "<li> Adjust the size of the window;"
                 "<li> Optionally enable Tile Tabs for a multi-tab view;  "
                 "If annotating a Tile Tabs view, Tile Tabs "
                 "(View Menu -> Enter Tile Tabs) should ALWAYS be enabled prior to "
                 "locking the aspect ratio.  Failure to do so may cause excess, "
                 "undesirable space around and between the drawing of tab rows."
                 "</ul>"
                 "If this has not been done, click the <i>Cancel</i> button, make those "
                 "adjustments, and then click the <i>" + buttonText + "</i>  button.");
    if (showBestPracticesLink) {
        text.append("<P>"
                    "View the <a href=\"Link\">Best Practices Guide</a> (this dialog "
                    "will close).  This guide explains the importance of aspect "
                    "locking and documents procedures for successful creation of annotations "
                    "and scenes.");
    }
    text.append("</html>");

    return text;
}


/**
 * Constructor.
 *
 * @param mode
 *     The mode for the dialog
 * @param brainBrowserWindow
 *     Parent window for help dialog that is linked from this dialog
 * @param parent
 *     The parent widget.
 */
LockAspectWarningDialog::LockAspectWarningDialog(BrainBrowserWindow* brainBrowserWindow,
                                                 const Mode mode)
: WuQDialogModal("Enter Annotations Mode",
                 brainBrowserWindow),
m_brainBrowserWindow(brainBrowserWindow),
m_mode(mode)
{
    QString buttonName;
    switch (m_mode) {
        case Mode::ENTER_ANNOTATIONS_MODE:
            setWindowTitle("Enter Annotations Mode");
            buttonName = "Toolbar's Annotate Mode";
            break;
        case Mode::TOOLBAR_LOCK_ASPECT:
            setWindowTitle("Lock Aspect Ratio");
            buttonName = "Lock Aspect";
            break;
    }
    
    
    const QString supplementalInstructions(getLockingInstructionsText(buttonName,
                                                                      true));
    QLabel* supplementalInstructionLabel = new QLabel(supplementalInstructions);
    supplementalInstructionLabel->setWordWrap(true);
    QObject::connect(supplementalInstructionLabel, &QLabel::linkActivated,
                     this, &LockAspectWarningDialog::detailsLabelLinkActivated);
    
    m_doNotShowAgainCheckBox = new QCheckBox("Do not show this dialog again.  User will not be warned about "
                                             "aspect locking and unlocking.");

    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    switch (m_mode) {
        case Mode::ENTER_ANNOTATIONS_MODE:
        {
            const QString mainInstructions("Do you want to lock the aspect ratio while entering annotations mode?");
            
            
            const QString lockAspectInstructions("Locking the aspect ratio, and never unlocking the aspect "
                                                 "ratio, ensures annotations stay in the correct location.");
            const QString leaveUnlockedInstructions("Advanced users may choose to lock and unlock the aspect ratio");
            
            QLabel* mainInstructionsLabel = new QLabel(mainInstructions);
            QFont font = mainInstructionsLabel->font();
            font.setPointSize(font.pointSize() * 1.4);
            font.setBold(true);
            mainInstructionsLabel->setFont(font);
            
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

            dialogLayout->addWidget(mainInstructionsLabel);
            dialogLayout->addLayout(buttonGridLayout);
            dialogLayout->addSpacing(10);
        }
            break;
        case Mode::TOOLBAR_LOCK_ASPECT:
            break;
    }

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
    EventHelpViewerDisplay helpViewerEvent(m_brainBrowserWindow,
                                           "Annotation_and_Scenes_Best_Practices");
    EventManager::get()->sendEvent(helpViewerEvent.getPointer());
    
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



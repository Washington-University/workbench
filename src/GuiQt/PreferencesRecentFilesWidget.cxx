
/*LICENSE_START*/
/*
 *  Copyright (C) 2020 Washington University School of Medicine
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

#define __PREFERENCES_RECENT_FILES_WIDGET_DECLARE__
#include "PreferencesRecentFilesWidget.h"
#undef __PREFERENCES_RECENT_FILES_WIDGET_DECLARE__

    
/**
 * \class caret::PreferencesRecentFilesWidget 
 * \brief Widget for recent file properties in preferences
 * \ingroup GuiQt
 */

#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "CaretPreferences.h"
#include "EnumComboBoxTemplate.h"
#include "RecentFilesSystemAccessModeEnum.h"
#include "WuQMessageBox.h"
#include "WuQTrueFalseComboBox.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Constructor.
 */
PreferencesRecentFilesWidget::PreferencesRecentFilesWidget(QWidget* parent)
: QWidget(parent)
{
    /*
     * Show Open Recent Files Dialog at Startup
     */
    QLabel* showRecentFilesDialogLabel = new QLabel("Show Open Recent Files Dialog at Startup");
    m_showOpenRecentFilesDialogAtStartupComboBox = new WuQTrueFalseComboBox("On",
                                                                            "Off",
                                                                            this);
    QObject::connect(m_showOpenRecentFilesDialogAtStartupComboBox, &WuQTrueFalseComboBox::statusChanged,
                     this, &PreferencesRecentFilesWidget::showOpenRecentFilesDialogAtStartupComboBoxActivated);

    /*
     * Files system access mode
     */
    QLabel* fileSystemLabel = new QLabel("File System Access");
    const QString fileSystemToolTip("File system access allows verification of a path's validity "
                                    "and display of last modified time.  Disabling file system "
                                    "access may be useful when there are problems with a file "
                                    "system (usually remote, mounted file systems).");
    m_recentFilesSystemAccessModeEnumComboBox = new EnumComboBoxTemplate(this);
    m_recentFilesSystemAccessModeEnumComboBox->setup<RecentFilesSystemAccessModeEnum,RecentFilesSystemAccessModeEnum::Enum>();
    QObject::connect(m_recentFilesSystemAccessModeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(recentFilesSystemAccessModeEnumComboBoxItemActivated()));
    WuQtUtilities::setWordWrappedToolTip(m_recentFilesSystemAccessModeEnumComboBox->getWidget(),
                                         fileSystemToolTip);
    
    /*
     * Recent files
     */
    QString recentSceneSpecSpinBoxToolTip;
    QString recentSceneSpecClearButtonToolTip;
    getMaximumSpinBoxAndClearButtonToolTips("Scene and Spec Files",
                                            recentSceneSpecSpinBoxToolTip,
                                            recentSceneSpecClearButtonToolTip);
    QLabel* recentFilesLabel = new QLabel("Recent Files Maximum");
    m_numberOfRecentSceneAndSpecFilesSpinBox = new QSpinBox();
    m_numberOfRecentSceneAndSpecFilesSpinBox->setRange(0, 1000);
    QObject::connect(m_numberOfRecentSceneAndSpecFilesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &PreferencesRecentFilesWidget::numberOfRecentSceneAndSpecFilesSpinBoxValueChanged);
    m_numberOfRecentSceneAndSpecFilesSpinBox->setToolTip(recentSceneSpecSpinBoxToolTip);
    QPushButton* clearRecentFilesPushButton = new QPushButton("Clear...");
    QObject::connect(clearRecentFilesPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::clearRecentSceneAndSpecFilesButtonClicked);
    clearRecentFilesPushButton->setToolTip(recentSceneSpecClearButtonToolTip);
    
    /*
     * Recent directories
     */
    QString recentDirectoriesSpinBoxToolTip;
    QString recentDirectoriesClearButtonToolTip;
    getMaximumSpinBoxAndClearButtonToolTips("Directories",
                                            recentDirectoriesSpinBoxToolTip,
                                            recentDirectoriesClearButtonToolTip);
    QLabel* recentDirectoriesLabel = new QLabel("Recent Directories Maximum");
    m_numberOfRecentDirectoriesSpinBox = new QSpinBox();
    m_numberOfRecentDirectoriesSpinBox->setRange(0, 1000);
    QObject::connect(m_numberOfRecentDirectoriesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &PreferencesRecentFilesWidget::numberOfRecentDirectoriesSpinBoxValueChanged);
    m_numberOfRecentDirectoriesSpinBox->setToolTip(recentDirectoriesSpinBoxToolTip);
    QPushButton* clearRecentDirectoriesPushButton = new QPushButton("Clear...");
    QObject::connect(clearRecentDirectoriesPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::clearRecentDirectoriesButtonClicked);
    clearRecentDirectoriesPushButton->setToolTip(recentDirectoriesClearButtonToolTip);

    /*
     * Exclusion Paths
     */
    const QString exclusionPathToolTip("If a Directory, Scene File, or Spec File is in an exclusion path, "
                                       "the directory or file is NOT added to recent directories and files "
                                       "listed in the Open Recent Files Dialog.  This may be useful if the "
                                       "user is working with some form of protected data.");
    m_exclusionPathsListWidget = new QListWidget();
    m_exclusionPathsListWidget->setSelectionMode(QListWidget::ExtendedSelection);
    m_exclusionPathsListWidget->setToolTip("<html>" + exclusionPathToolTip + "</html>");
    QPushButton* addExclusionPathPushButton = new QPushButton("Add...");
    QObject::connect(addExclusionPathPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::addExclusionPathButtonClicked);
    addExclusionPathPushButton->setToolTip("<html>"
                                           "Add an exclusion path.<br>"
                                           + exclusionPathToolTip
                                           + "</html>");
    QPushButton* removeExclusionPathPushButton = new QPushButton("Remove...");
    QObject::connect(removeExclusionPathPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::removeExclusionPathButtonClicked);
    removeExclusionPathPushButton->setToolTip("<html>"
                                           "Remove selected exclusion path(s).<br>"
                                           + exclusionPathToolTip
                                           + "</html>");
    
    /*
     * Layouts
     */
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    int32_t row(0);
    layout->addWidget(showRecentFilesDialogLabel,
                      row, 0, Qt::AlignRight);
    layout->addWidget(m_showOpenRecentFilesDialogAtStartupComboBox->getWidget(),
                           row, 1, 1, 3);
    row++;
    layout->addWidget(fileSystemLabel, row, 0, Qt::AlignRight);
    layout->addWidget(m_recentFilesSystemAccessModeEnumComboBox->getWidget(),
                           row, 1, 1, 3);
    row++;
    layout->addWidget(recentFilesLabel, row, 0, Qt::AlignRight);
    layout->addWidget(m_numberOfRecentSceneAndSpecFilesSpinBox, row, 1);
    layout->addWidget(clearRecentFilesPushButton, row, 2);
    row++;
    layout->addWidget(recentDirectoriesLabel, row, 0, Qt::AlignRight);
    layout->addWidget(m_numberOfRecentDirectoriesSpinBox, row, 1);
    layout->addWidget(clearRecentDirectoriesPushButton, row, 2);
    row++;
    
    QGroupBox* exclusionPathsGroupBox = new QGroupBox("Exclusion Paths");
    QGridLayout* exclusionPathsLayout = new QGridLayout(exclusionPathsGroupBox);
    exclusionPathsLayout->addWidget(m_exclusionPathsListWidget, 0, 0, 1, 2);
    exclusionPathsLayout->addWidget(addExclusionPathPushButton, 1, 0, Qt::AlignHCenter);
    exclusionPathsLayout->addWidget(removeExclusionPathPushButton, 1, 1, Qt::AlignHCenter);
    
    /*
     * Widget layout
     */
    QVBoxLayout* widgetLayout = new QVBoxLayout(this);
    widgetLayout->addWidget(widget);
    widgetLayout->addWidget(exclusionPathsGroupBox);
}

/**
 * Destructor.
 */
PreferencesRecentFilesWidget::~PreferencesRecentFilesWidget()
{
}

/**
 * Get the tool tips for a recent file type's maximum spin box and clear push button.
 * @param typeName
 *    Type name for spin box and button
 * @param spinBoxToolTipOut
 *    Tooltip for spin box
 * @param clearButtonToolTipOut
 *    Tooltip for clear button
 */
void
PreferencesRecentFilesWidget::getMaximumSpinBoxAndClearButtonToolTips(const QString& typeName,
                                                                      QString& spinBoxToolTipOut,
                                                                      QString& clearButtonToolTipOut) const
{
    spinBoxToolTipOut = ("<html>"
                         "Maximum number of " + typeName + " listed in Open Recent "
                         "Files Dialog.  Note that Favorites are not automatically removed so the "
                         "number of " + typeName + " listed in the Open Recent "
                         "Files Dialog may exceed this number."
                         "</html>");
    
    clearButtonToolTipOut = ("<html>"
                             "Clears history of recently opened " + typeName + " listed in the "
                             "Open Recent Files Dialog.  When this button is clicked, the user is "
                             "asked to choose between keeping or removing any Favorites that are in the Recent "
                             + typeName
                             + "."
                             + "</html>");
}


/**
 * Called when file system access mode is selected by user
 */
void
PreferencesRecentFilesWidget::recentFilesSystemAccessModeEnumComboBoxItemActivated()
{
    if (m_preferences != NULL) {
        const auto accessMode = m_recentFilesSystemAccessModeEnumComboBox->getSelectedItem<RecentFilesSystemAccessModeEnum,RecentFilesSystemAccessModeEnum::Enum>();
        m_preferences->setRecentFilesSystemAccessMode(accessMode);
    }
}

/*
 * Update the content in this widget
 * @param caretPreferences
 *    The caret preferences
 */
void
PreferencesRecentFilesWidget::updateContent(CaretPreferences* caretPreferences)
{
    m_preferences = caretPreferences;
    CaretAssert(m_preferences);

    m_showOpenRecentFilesDialogAtStartupComboBox->setStatus(m_preferences->isSplashScreenEnabled());
    
    const auto accessMode = m_preferences->getRecentFilesSystemAccessMode();
    m_recentFilesSystemAccessModeEnumComboBox->setSelectedItem<RecentFilesSystemAccessModeEnum,RecentFilesSystemAccessModeEnum::Enum>(accessMode);
    
    QSignalBlocker filesSpinBlocker(m_numberOfRecentSceneAndSpecFilesSpinBox);
    m_numberOfRecentSceneAndSpecFilesSpinBox->setValue(m_preferences->getRecentMaximumNumberOfSceneAndSpecFiles());
    
    QSignalBlocker directoriesSpinBlocker(m_numberOfRecentDirectoriesSpinBox);
    m_numberOfRecentDirectoriesSpinBox->setValue(m_preferences->getRecentMaximumNumberOfDirectories());
    
    std::set<AString> exclusionPaths;
    m_preferences->readRecentFilesExclusionPaths(exclusionPaths);
    
    m_exclusionPathsListWidget->clear();
    for (auto ep : exclusionPaths) {
        m_exclusionPathsListWidget->addItem(ep);
    }
}

/**
 * Called when number of recent files spin box value changed
 * @param value
 *    New value
 */
void
PreferencesRecentFilesWidget::numberOfRecentSceneAndSpecFilesSpinBoxValueChanged(int value)
{
    if (m_preferences != NULL) {
        m_preferences->setRecentMaximumNumberOfSceneAndSpecFiles(value);
        updateContent(m_preferences);
    }
}

/**
 * Called when number of recent directories spin box value changed
 * @param value
 *    New value
 */
void
PreferencesRecentFilesWidget::numberOfRecentDirectoriesSpinBoxValueChanged(int value)
{
    if (m_preferences != NULL) {
        m_preferences->setRecentMaximumNumberOfDirectories(value);
        updateContent(m_preferences);
    }
}

/**
 * Called when clear recent files button clicked
 */
void
PreferencesRecentFilesWidget::clearRecentSceneAndSpecFilesButtonClicked()
{
    if (m_preferences != NULL) {
        const AString infoText("<html>"
                               "Do you want to include favorites when deleting all recent scene and spec files?<p>  "
                               "   Yes - All recent files, including favorites are removed<br>"
                               "   No  - Recent files are removed, excluding any favorites<br>"
                               "   Cancel - Take no further action"
                               "</html>");
        const auto yesNoCancelResult = WuQMessageBox::warningYesNoCancel(this,
                                                                         "Delete Favorites Too?",
                                                                         infoText);
        switch (yesNoCancelResult) {
            case WuQMessageBox::RESULT_CANCEL:
                break;
            case WuQMessageBox::RESULT_NO:
                m_preferences->clearRecentSceneAndSpecFiles(false);
                break;
            case WuQMessageBox::RESULT_YES:
                m_preferences->clearRecentSceneAndSpecFiles(true);
                break;
        }
        
        updateContent(m_preferences);
    }
}

/**
 * Called when clear recent directories button clicked
 */
void
PreferencesRecentFilesWidget::clearRecentDirectoriesButtonClicked()
{
    if (m_preferences != NULL) {
        const AString infoText("<html>"
                               "Do you want to include favorites when deleting all recent directories?<p>  "
                               "   Yes - All recent directories, including favorites are removed<br>"
                               "   No  - Recent directories are removed, excluding any favorites<br>"
                               "   Cancel - Take no further action"
                               "</html>");
        const auto yesNoCancelResult = WuQMessageBox::warningYesNoCancel(this,
                                                                         "Delete Favorites Too?",
                                                                         infoText);
        switch (yesNoCancelResult) {
            case WuQMessageBox::RESULT_CANCEL:
                break;
            case WuQMessageBox::RESULT_NO:
                m_preferences->clearRecentDirectories(false);
                break;
            case WuQMessageBox::RESULT_YES:
                m_preferences->clearRecentDirectories(true);
                break;
        }
        
        updateContent(m_preferences);
    }

}

/**
 * Called when add exclusion path button clicked
 */
void
PreferencesRecentFilesWidget::addExclusionPathButtonClicked()
{
    if (m_preferences != NULL) {
        const QString directoryName = CaretFileDialog::getExistingDirectoryDialog(this,
                                                                                  "Choose Directory");
        if ( ! directoryName.isEmpty()) {
            m_preferences->addToRecentFilesExclusionPaths(directoryName);
            updateContent(m_preferences);
        }
    }
}

/**
 * Called when remove exclusion path button clicked
 */
void
PreferencesRecentFilesWidget::removeExclusionPathButtonClicked()
{
    if (m_preferences != NULL) {
        std::vector<AString> pathsToRemove;
        const int32_t numItems = m_exclusionPathsListWidget->count();
        for (int32_t row = 0; row < numItems; row++) {
            QListWidgetItem* item = m_exclusionPathsListWidget->item(row);
            if (item != NULL) {
                if (item->isSelected()) {
                    pathsToRemove.push_back(item->text());
                }
            }
        }
        
        if ( ! pathsToRemove.empty()) {
            for (auto& path : pathsToRemove) {
                m_preferences->removeFromRecentFilesExclusionPaths(path);
            }
            updateContent(m_preferences);
        }
    }
}

/**
 * Called when Show Open Recent Files Dialog Combo Box activated
 * @param status
 *    New status
 */
void
PreferencesRecentFilesWidget::showOpenRecentFilesDialogAtStartupComboBoxActivated(bool status)
{
    if (m_preferences != NULL) {
        m_preferences->setSplashScreenEnabled(status);
    }
}

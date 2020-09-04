
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
#include "EnumComboBoxTemplate.h"
#include "RecentFilesSystemAccessModeEnum.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * Constructor.
 */
PreferencesRecentFilesWidget::PreferencesRecentFilesWidget(QWidget* parent)
: QWidget(parent)
{
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
    QLabel* recentFilesLabel = new QLabel("Recent Files Maximum");
    m_numberOfRecentFilesSpinBox = new QSpinBox();
    m_numberOfRecentFilesSpinBox->setRange(0, 1000);
    QObject::connect(m_numberOfRecentFilesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &PreferencesRecentFilesWidget::numberOfRecentFilesSpinBoxValueChanged);
    QPushButton* clearRecentFilesPushButton = new QPushButton("Clear...");
    QObject::connect(clearRecentFilesPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::clearRecentFilesButtonClicked);
    
    /*
     * Recent directories
     */
    QLabel* recentDirectoriesLabel = new QLabel("Recent Directories Maximum");
    m_numberOfRecentDirectoriesSpinBox = new QSpinBox();
    m_numberOfRecentDirectoriesSpinBox->setRange(0, 1000);
    QObject::connect(m_numberOfRecentDirectoriesSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &PreferencesRecentFilesWidget::numberOfRecentDirectoriesSpinBoxValueChanged);
    QPushButton* clearRecentDirectoriesPushButton = new QPushButton("Clear...");
    QObject::connect(clearRecentDirectoriesPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::clearRecentDirectoriesButtonClicked);

    /*
     * Remove invalid paths
     */
    QPushButton* removeInvalidPathsPushButton = new QPushButton("Remove Invalid Paths...");
    QObject::connect(removeInvalidPathsPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::removeInvalidPathsButtonClicked);
    
    /*
     * Exclusion Paths
     */
    m_exclusionPathsListWidget = new QListWidget();
    QPushButton* addExclusionPathPushButton = new QPushButton("Add...");
    QObject::connect(addExclusionPathPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::addExclusionPathButtonClicked);
    QPushButton* removeExclusionPathPushButton = new QPushButton("Remove...");
    QObject::connect(removeExclusionPathPushButton, &QPushButton::clicked,
                     this, &PreferencesRecentFilesWidget::removeExclusionPathButtonClicked);

    
    /*
     * Layouts
     */
    QWidget* filesWidget = new QWidget();
    QGridLayout* filesLayout = new QGridLayout(filesWidget);
    int32_t filesRow(0);
    filesLayout->addWidget(fileSystemLabel, filesRow, 0);
    filesLayout->addWidget(m_recentFilesSystemAccessModeEnumComboBox->getWidget(),
                           filesRow, 1, 1, 3);
    filesRow++;
    filesLayout->addWidget(recentFilesLabel, filesRow, 0);
    filesLayout->addWidget(m_numberOfRecentFilesSpinBox, filesRow, 1);
    filesLayout->addWidget(clearRecentFilesPushButton, filesRow, 2);
    filesRow++;
    filesLayout->addWidget(recentDirectoriesLabel, filesRow, 0);
    filesLayout->addWidget(m_numberOfRecentDirectoriesSpinBox, filesRow, 1);
    filesLayout->addWidget(clearRecentDirectoriesPushButton, filesRow, 2);
    filesRow++;
    
    QGroupBox* exclusionPathsGroupBox = new QGroupBox("Exclusion Paths");
    QGridLayout* exclusionPathsLayout = new QGridLayout(exclusionPathsGroupBox);
    exclusionPathsLayout->addWidget(m_exclusionPathsListWidget, 0, 0, 1, 2);
    exclusionPathsLayout->addWidget(addExclusionPathPushButton, 1, 0, Qt::AlignHCenter);
    exclusionPathsLayout->addWidget(removeExclusionPathPushButton, 1, 1, Qt::AlignHCenter);
    
    /*
     * Widget layout
     */
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(filesWidget);
    layout->addWidget(exclusionPathsGroupBox);
}

/**
 * Destructor.
 */
PreferencesRecentFilesWidget::~PreferencesRecentFilesWidget()
{
}

/**
 * Called when file system access mode is selected by user
 */
void
PreferencesRecentFilesWidget::recentFilesSystemAccessModeEnumComboBoxItemActivated()
{
//    const RecentFilesSystemAccessModeEnum::Enum mode = m_recentFilesSystemAccessModeEnumComboBox->getSelectedItem<RecentFilesSystemAccessModeEnum,RecentFilesSystemAccessModeEnum::Enum>();
}

/*
 * Update the content in this widget
 */
void
PreferencesRecentFilesWidget::updateContent()
{
//    m_recentFilesSystemAccessModeEnumComboBox->setSelectedItem<RecentFilesSystemAccessModeEnum,RecentFilesSystemAccessModeEnum::Enum>(NEW_VALUE);
}

/**
 * Called when number of recent files spin box value changed
 * @param value
 *    New value
 */
void
PreferencesRecentFilesWidget::numberOfRecentFilesSpinBoxValueChanged(int)
{
    
}

/**
 * Called when number of recent directories spin box value changed
 * @param value
 *    New value
 */
void
PreferencesRecentFilesWidget::numberOfRecentDirectoriesSpinBoxValueChanged(int)
{
    
}

/**
 * Called when clear recent files button clicked
 */
void
PreferencesRecentFilesWidget::clearRecentFilesButtonClicked()
{
    
}

/**
 * Called when clear recent directories button clicked
 */
void
PreferencesRecentFilesWidget::clearRecentDirectoriesButtonClicked()
{
    
}

/**
 * Called when remove invalid paths button clicked
 */
void
PreferencesRecentFilesWidget::removeInvalidPathsButtonClicked()
{
    
}

/**
 * Called when add exclusion path button clicked
 */
void
PreferencesRecentFilesWidget::addExclusionPathButtonClicked()
{
    
}

/**
 * Called when remove exclusion path button clicked
 */
void
PreferencesRecentFilesWidget::removeExclusionPathButtonClicked()
{
    
}


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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MPR_DECLARE__
#include "BrainBrowserWindowToolBarVolumeMPR.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_VOLUME_MPR_DECLARE__

#include <QGridLayout>
#include <QLabel>

#include "BrowserTabContent.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "CaretUndoStack.h"
#include "EventManager.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarVolumeMPR
 * \brief Image Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarVolumeMPR::BrainBrowserWindowToolBarVolumeMPR(BrainBrowserWindowToolBar* parentToolBar,
                                                                                   const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QLabel* intensityModeLabel = new QLabel("Intensity");
    m_mprIntensityModeComboBox = new EnumComboBoxTemplate(this);
    m_mprIntensityModeComboBox->getWidget()->setToolTip("Select Intensity Mode");
    m_mprIntensityModeComboBox->setup<VolumeMprIntensityProjectionModeEnum,VolumeMprIntensityProjectionModeEnum::Enum>();
    QObject::connect(m_mprIntensityModeComboBox, SIGNAL(itemActivated()),
                     this, SLOT(mprIntensityComboBoxItemActivated()));
    m_mprIntensityModeComboBox->getWidget()->setObjectName(parentObjectName
                                                           + "mprIntensityComboBox");
    WuQMacroManager::instance()->addMacroSupportToObject(m_mprIntensityModeComboBox->getWidget(),
                                                         "Intensity Mode");


    QGridLayout* layout = new QGridLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(intensityModeLabel, 0, 0);
    layout->addWidget(m_mprIntensityModeComboBox->getWidget(), 0, 1);
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarVolumeMPR::~BrainBrowserWindowToolBarVolumeMPR()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarVolumeMPR::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;

    bool enabledFlag(false);
    if (browserTabContent != NULL) {
        const VolumeMprIntensityProjectionModeEnum::Enum intensityMode(m_browserTabContent->getVolumeMprIntensityProjectionMode());
        m_mprIntensityModeComboBox->setSelectedItem<VolumeMprIntensityProjectionModeEnum,VolumeMprIntensityProjectionModeEnum::Enum>(intensityMode);
        
        switch (m_browserTabContent->getSliceProjectionType()) {
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_NEUROLOGICAL:
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_MPR_RADIOLOGICAL:
                enabledFlag = true;
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_OBLIQUE:
                break;
            case VolumeSliceProjectionTypeEnum::VOLUME_SLICE_PROJECTION_ORTHOGONAL:
                break;
        }
    }
    
    setEnabled(enabledFlag);
}

/**
 * Called when intensity mode combo box is selected
 */
void
BrainBrowserWindowToolBarVolumeMPR::mprIntensityComboBoxItemActivated()
{
    if (m_browserTabContent != NULL) {
        const VolumeMprIntensityProjectionModeEnum::Enum value = m_mprIntensityModeComboBox->getSelectedItem<VolumeMprIntensityProjectionModeEnum,VolumeMprIntensityProjectionModeEnum::Enum>();
        m_browserTabContent->setVolumeMprIntensityProjectionMode(value);
        updateGraphicsWindowAndYokedWindows();
        updateUserInterface();
    }
}

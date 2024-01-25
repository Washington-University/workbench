
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

#define __BRAIN_BROWSER_WINDOW_TOOL_BAR_IMAGE_DECLARE__
#include "BrainBrowserWindowToolBarImage.h"
#undef __BRAIN_BROWSER_WINDOW_TOOL_BAR_IMAGE_DECLARE__

#include <QAction>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "BrainBrowserWindowToolBar.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "ModelMedia.h"
#include "WuQMacroManager.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::BrainBrowserWindowToolBarImage
 * \brief Image Component of Brain Browser Window ToolBar
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parentToolBar
 *    parent toolbar.
 */
BrainBrowserWindowToolBarImage::BrainBrowserWindowToolBarImage(BrainBrowserWindowToolBar* parentToolBar,
                                                                                   const QString& parentObjectName)
: BrainBrowserWindowToolBarComponent(parentToolBar),
m_parentToolBar(parentToolBar)
{
    QLabel* modeLabel(new QLabel("Coord"));
    m_mediaDisplayCoordinateModeEnumComboBox = new EnumComboBoxTemplate(this);
    std::vector<MediaDisplayCoordinateModeEnum::Enum> supportedModes;
    supportedModes.push_back(MediaDisplayCoordinateModeEnum::PIXEL);
    supportedModes.push_back(MediaDisplayCoordinateModeEnum::PLANE);
    m_mediaDisplayCoordinateModeEnumComboBox->setupWithItems<MediaDisplayCoordinateModeEnum,MediaDisplayCoordinateModeEnum::Enum>(supportedModes);
    QObject::connect(m_mediaDisplayCoordinateModeEnumComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &BrainBrowserWindowToolBarImage::mediaDisplayCoordinateModeEnumComboBoxItemActivated);
    m_mediaDisplayCoordinateModeEnumComboBox->getWidget()->setObjectName(parentObjectName
                                                                         + ":mediaDisplayModeComboBox");
    m_mediaDisplayCoordinateModeEnumComboBox->getWidget()->setToolTip("Coordinate Display Mode");
    WuQMacroManager::instance()->addMacroSupportToObject(m_mediaDisplayCoordinateModeEnumComboBox->getWidget(), "Set media coordinate mode for display");
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(modeLabel, 0, Qt::AlignHCenter);
    layout->addWidget(m_mediaDisplayCoordinateModeEnumComboBox->getWidget(), 0, Qt::AlignHCenter);
    layout->addStretch();
}

/**
 * Destructor.
 */
BrainBrowserWindowToolBarImage::~BrainBrowserWindowToolBarImage()
{
}

/**
 * Update content of this tool bar component.
 *
 * @param browserTabContent
 *     Content of the browser tab.
 */
void
BrainBrowserWindowToolBarImage::updateContent(BrowserTabContent* browserTabContent)
{
    m_browserTabContent = browserTabContent;

    if (browserTabContent != NULL) {
        const MediaDisplayCoordinateModeEnum::Enum mediaDisplayMode(browserTabContent->getMediaDisplayCoordinateMode());
        m_mediaDisplayCoordinateModeEnumComboBox->setSelectedItem<MediaDisplayCoordinateModeEnum,MediaDisplayCoordinateModeEnum::Enum>(mediaDisplayMode);
    }
    
    m_mediaDisplayCoordinateModeEnumComboBox->getWidget()->setEnabled(browserTabContent != NULL);
}

/**
 * Called when media coordinate display mode is changed
 */
void
BrainBrowserWindowToolBarImage::mediaDisplayCoordinateModeEnumComboBoxItemActivated()
{
    if (m_browserTabContent != NULL) {
        const MediaDisplayCoordinateModeEnum::Enum mode(m_mediaDisplayCoordinateModeEnumComboBox->getSelectedItem<MediaDisplayCoordinateModeEnum,MediaDisplayCoordinateModeEnum::Enum>());
        m_browserTabContent->setMediaDisplayCoordinateMode(mode);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    }
}

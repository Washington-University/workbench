
/*LICENSE_START*/
/*
 *  Copyright (C) 2021 Washington University School of Medicine
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

#define __MEDIA_OVERLAY_SETTINGS_MENU_DECLARE__
#include "MediaOverlaySettingsMenu.h"
#undef __MEDIA_OVERLAY_SETTINGS_MENU_DECLARE__

#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "CziImageFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "MediaOverlay.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::MediaOverlaySettingsMenu 
 * \brief Menu for setting media overlay settings
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param mediaOverlay
 *    The media overlay
 *    @param parentObjectName
 *    Name of parent for macros
 */
MediaOverlaySettingsMenu::MediaOverlaySettingsMenu(MediaOverlay* mediaOverlay,
                                                   const QString& parentObjectName)
: QMenu(),
m_mediaOverlay(mediaOverlay)
{
    CaretAssert(m_mediaOverlay);
    
    QLabel* pyramidLayerLabel = new QLabel("Pyramid Layer");
    
    std::vector<CziImageResolutionChangeModeEnum::Enum> resModes;
    CziImageResolutionChangeModeEnum::getAllEnums(resModes);
    QString resModeTT("<html><body>"
                      "<ul>");
    for (const auto& rm : resModes) {
        resModeTT.append("<li>"
                         + CziImageResolutionChangeModeEnum::toGuiName(rm)
                         + " - ");
        switch (rm) {
            case CziImageResolutionChangeModeEnum::INVALID:
                resModeTT.append("Invalid");
                break;
            case CziImageResolutionChangeModeEnum::AUTO2:
                resModeTT.append("Workbench allows the CZI library to load the appropriate "
                                 "image resolution when zoomed and panned.");
                break;
            case CziImageResolutionChangeModeEnum::MANUAL2:
                resModeTT.append("User selects level of image resolution");
                break;
        }
    }
    resModeTT.append("</ul>"
                     "</body></html>");
    
    m_cziResolutionChangeModeComboBox = new EnumComboBoxTemplate(this);
    m_cziResolutionChangeModeComboBox->setup<CziImageResolutionChangeModeEnum, CziImageResolutionChangeModeEnum::Enum>();
    QObject::connect(m_cziResolutionChangeModeComboBox, &EnumComboBoxTemplate::itemActivated,
                     this, &MediaOverlaySettingsMenu::resolutionModeComboBoxActivated);
    m_cziResolutionChangeModeComboBox->getWidget()->setToolTip(resModeTT);
    m_cziResolutionChangeModeComboBox->getWidget()->setObjectName(parentObjectName
                                                         + "MediaOverlaySettingsMenu::ResolutionModeComboBox");
    WuQMacroManager::instance()->addMacroSupportToObject(m_cziResolutionChangeModeComboBox->getWidget(),
                                                         "Change CZI Resolution Mode");
    
    const QString pyrTT("Increase/decrease pyramid layer to show higher/lower resolution image.  "
                        "Higher resolution images cover a smaller spatial region.");
    m_cziPyramidLayerIndexSpinBox = new QSpinBox();
    QObject::connect(m_cziPyramidLayerIndexSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &MediaOverlaySettingsMenu::pyramidLayerChanged);
    m_cziPyramidLayerIndexSpinBox->setObjectName(parentObjectName
                                         + ":MediaOverlaySettingsMenu:PyramidLayerSpinBox");
    WuQtUtilities::setWordWrappedToolTip(m_cziPyramidLayerIndexSpinBox, pyrTT);
    WuQMacroManager::instance()->addMacroSupportToObject(m_cziPyramidLayerIndexSpinBox,
                                                         "Select pyramid layer of CZI Image in Tab");
    
    const QString reloadTT("Reload the current pyramid layer.  This may be useful when user pans the "
                           "image so that much of the image is off the screen.");
    m_reloadAction = new QAction(this);
    m_reloadAction->setText("Reload");
    QObject::connect(m_reloadAction, &QAction::triggered,
                     this, &MediaOverlaySettingsMenu::reloadActionTriggered);
    WuQtUtilities::setWordWrappedToolTip(m_reloadAction,
                                         reloadTT);
    m_reloadAction->setObjectName(parentObjectName
                                  + ":MediaOverlaySettingsMenu:ReloadCziImageAction");
    WuQMacroManager::instance()->addMacroSupportToObject(m_reloadAction,
                                                         "Reload CZI Image");
    
    
    QToolButton* reloadToolButton = new QToolButton();
    reloadToolButton->setDefaultAction(m_reloadAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(reloadToolButton);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(pyramidLayerLabel);
    layout->addWidget(m_cziResolutionChangeModeComboBox->getWidget());
    layout->addWidget(m_cziPyramidLayerIndexSpinBox);
    layout->addWidget(reloadToolButton);
    layout->addStretch();
    
    updateContent();
}

/**
 * Destructor.
 */
MediaOverlaySettingsMenu::~MediaOverlaySettingsMenu()
{
}

/**
 * Update content of widget
 */
void
MediaOverlaySettingsMenu::updateContent()
{
    CaretAssert(m_mediaOverlay);
    const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());

    QSignalBlocker indexBlocker(m_cziPyramidLayerIndexSpinBox);
    m_cziPyramidLayerIndexSpinBox->setRange(selectionData.m_cziManualPyramidLayerMinimumValue,
                                            selectionData.m_cziManualPyramidLayerMaximumValue);
    m_cziPyramidLayerIndexSpinBox->setValue(selectionData.m_cziManualPyramidLayerIndex);
    
    m_cziResolutionChangeModeComboBox->setSelectedItem<CziImageResolutionChangeModeEnum, CziImageResolutionChangeModeEnum::Enum>(selectionData.m_cziResolutionChangeMode);
}


/**
 * Called when pyramid layer spin box is changed
 * @param value
 *    New value
 */
void
MediaOverlaySettingsMenu::pyramidLayerChanged(int value)
{
    m_mediaOverlay->setCziPyramidLayerIndex(value);
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Called when reload button is clicked
 */
void
MediaOverlaySettingsMenu::reloadActionTriggered()
{
    CaretAssertToDoFatal();
    
    const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
    if (selectionData.m_selectedCziImageFile != NULL) {
        selectionData.m_selectedCziImageFile->reloadPyramidLayerInTabOverlay(m_mediaOverlay->m_tabIndex,
                                                                             m_mediaOverlay->m_overlayIndex);
        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
        updateContent();
    }
}

/**
 * Called when user changes resolution mode
 */
void
MediaOverlaySettingsMenu::resolutionModeComboBoxActivated()
{
    const CziImageResolutionChangeModeEnum::Enum mode = m_cziResolutionChangeModeComboBox->getSelectedItem<CziImageResolutionChangeModeEnum, CziImageResolutionChangeModeEnum::Enum>();
    m_mediaOverlay->setCziResolutionChangeMode(mode);

    switch (mode) {
        case CziImageResolutionChangeModeEnum::INVALID:
            break;
        case CziImageResolutionChangeModeEnum::AUTO2:
        {
            /*
             * AUTO needs repaint since it may try to load a new image
             * of a different resolution.
             */
            const bool doRepaintFlag(true);
            EventGraphicsUpdateAllWindows graphicsEvent(doRepaintFlag);
            EventManager::get()->sendEvent(graphicsEvent.getPointer());
        }
            break;
        case CziImageResolutionChangeModeEnum::MANUAL2:
            EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
            break;
    }
}

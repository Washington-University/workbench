
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

#include <QCheckBox>
#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>
#include <QWidgetAction>

#include "CaretAssert.h"
#include "CziImageFile.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintNowAllWindows.h"
#include "EventGraphicsPaintSoonAllWindows.h"
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
    
    m_pyramidLayerDimensionsLabel = new QLabel();
    
    QWidget* pyramidLayerWidget(new QWidget());
    QVBoxLayout* pyramidLayerLayout = new QVBoxLayout(pyramidLayerWidget);
    pyramidLayerLayout->addWidget(pyramidLayerLabel);
    pyramidLayerLayout->addWidget(m_cziResolutionChangeModeComboBox->getWidget());
    pyramidLayerLayout->addWidget(m_cziPyramidLayerIndexSpinBox);
    pyramidLayerLayout->addWidget(m_pyramidLayerDimensionsLabel, Qt::AlignLeft);
    pyramidLayerLayout->addWidget(reloadToolButton);

    QLabel* channelLabel = new QLabel("Channel");
    m_selectedChannelSpinBox = new QSpinBox();
    m_selectedChannelSpinBox->setSingleStep(1);
    QObject::connect(m_selectedChannelSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &MediaOverlaySettingsMenu::selectedChannelSpinBoxValueChanged);
    QSignalBlocker channelBlocker(m_selectedChannelSpinBox);
    m_selectedChannelSpinBox->setRange(1, 1000); /* set range and fixed size, sets size of widget */
    m_selectedChannelSpinBox->setFixedSize(m_selectedChannelSpinBox->sizeHint());
    
    m_channelWidget = new QWidget();
    QGridLayout* channelLayout(new QGridLayout(m_channelWidget));
    channelLayout->addWidget(channelLabel, 1, 0);
    channelLayout->addWidget(m_selectedChannelSpinBox, 1, 1, Qt::AlignLeft);
    
    QWidget* widget(new QWidget());
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(pyramidLayerWidget);
    layout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    layout->addWidget(m_channelWidget);
    layout->addStretch();
    
    QWidgetAction* widgetAction(new QWidgetAction(this));
    widgetAction->setDefaultWidget(widget);
    
    addAction(widgetAction);
    
    QObject::connect(this, &QMenu::aboutToShow,
                     this, &MediaOverlaySettingsMenu::updateContent);
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
    
    m_channelWidget->setEnabled(false);
    m_selectedChannelSpinBox->setEnabled(false);
    
    updateDimensionsLabel();
    
    if (selectionData.m_selectedMediaFile != NULL) {
        const MediaFileChannelInfo* channelInfo(selectionData.m_constSelectedMediaFile->getMediaFileChannelInfo());
        CaretAssert(channelInfo);
        if (channelInfo->isChannelsSupported()) {
            QSignalBlocker blocker(m_selectedChannelSpinBox);
            
            if (channelInfo->isSingleChannelSelectionSupported()) {
                m_selectedChannelSpinBox->setRange(0, channelInfo->getNumberOfChannels() - 1);
            }
            
            if (channelInfo->isAllChannelsSelectionSupported()) {
                CaretAssert(MediaOverlay::getAllChannelsSelectedIndexValue() == -1);
                if (channelInfo->isSingleChannelSelectionSupported()) {
                    m_selectedChannelSpinBox->setMinimum(-1); /* Minimum value is used for special value text */
                }
                else {
                    m_selectedChannelSpinBox->setRange(-1, -1);
                }
                m_selectedChannelSpinBox->setSpecialValueText("All");
            }
            
            m_selectedChannelSpinBox->setValue(selectionData.m_selectedChannelIndex);
            m_selectedChannelSpinBox->setEnabled(true);
            m_channelWidget->setEnabled(true);
        }
    }
}

/**
 * Update the dimensions label
 */
void
MediaOverlaySettingsMenu::updateDimensionsLabel()
{
    m_pyramidLayerDimensionsLabel->setText("");

    CaretAssert(m_mediaOverlay);
    const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());

    if (selectionData.m_selectedMediaFile != NULL) {
        std::vector<int64_t> dims;
        if (selectionData.m_selectedMediaFile->getPyrimidLevelDimensions(selectionData.m_cziManualPyramidLayerIndex,
                                                                         dims)) {
            std::vector<AString> dimString;
            for (const auto& d : dims) {
                dimString.push_back(AString::number(d));
            }
            m_pyramidLayerDimensionsLabel->setText("X x Y: ("
                                                   + AString::join(dimString, ", ")
                                                   + ")");
        }
    }
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
    updateDimensionsLabel();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when reload button is clicked
 */
void
MediaOverlaySettingsMenu::reloadActionTriggered()
{
    const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
    if (selectionData.m_selectedMediaFile != NULL) {
        selectionData.m_selectedMediaFile->reloadPyramidLayerInTabOverlay(m_mediaOverlay->m_tabIndex,
                                                                             m_mediaOverlay->m_overlayIndex);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
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
            EventManager::get()->sendEvent(EventGraphicsPaintNowAllWindows().getPointer());
        }
            break;
        case CziImageResolutionChangeModeEnum::MANUAL2:
            EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
            break;
    }
}

/**
 * Called when all channels spin box value changed
 * @param value
 *    New  channel
 */
void
MediaOverlaySettingsMenu::selectedChannelSpinBoxValueChanged(int value)
{
    /*
     * NOTE: Disabling spin box prevents multiple signals from being
     * issued when user clicks once on up or down arrow on MacOS.
     * BUT it causes focus to be lost so need to regain focus.
     */
    m_selectedChannelSpinBox->setEnabled(false);
    m_mediaOverlay->setSelectedChannelIndex(value);
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    m_selectedChannelSpinBox->setEnabled(true);
    m_selectedChannelSpinBox->setFocus();
}

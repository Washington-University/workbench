
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

#define __HISTOLOGY_OVERLAY_SETTINGS_MENU_DECLARE__
#include "HistologyOverlaySettingsMenu.h"
#undef __HISTOLOGY_OVERLAY_SETTINGS_MENU_DECLARE__

#include <QLabel>
#include <QSpinBox>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "HistologyOverlay.h"
#include "WuQMacroManager.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::HistologyOverlaySettingsMenu
 * \brief Menu for setting histology overlay settings
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param histologyOverlay
 *    The histology overlay
 *    @param parentObjectName
 *    Name of parent for macros
 */
HistologyOverlaySettingsMenu::HistologyOverlaySettingsMenu(HistologyOverlay* histologyOverlay,
                                                   const QString& parentObjectName)
: QMenu(),
m_histologyOverlay(histologyOverlay)
{
    CaretAssert(m_histologyOverlay);
        
    const QString reloadTT("Reload the current pyramid layer.  This may be useful when user pans the "
                           "image so that much of the image is off the screen.");
    m_reloadAction = new QAction(this);
    m_reloadAction->setText("Reload");
    QObject::connect(m_reloadAction, &QAction::triggered,
                     this, &HistologyOverlaySettingsMenu::reloadActionTriggered);
    WuQtUtilities::setWordWrappedToolTip(m_reloadAction,
                                         reloadTT);
    m_reloadAction->setObjectName(parentObjectName
                                  + ":HistologyOverlaySettingsMenu:ReloadCziImageAction");
    WuQMacroManager::instance()->addMacroSupportToObject(m_reloadAction,
                                                         "Reload CZI Image");
    
    
    QToolButton* reloadToolButton = new QToolButton();
    reloadToolButton->setDefaultAction(m_reloadAction);
    WuQtUtilities::setToolButtonStyleForQt5Mac(reloadToolButton);
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 5);
    layout->addWidget(reloadToolButton);
    layout->addStretch();
    
    updateContent();
}

/**
 * Destructor.
 */
HistologyOverlaySettingsMenu::~HistologyOverlaySettingsMenu()
{
}

/**
 * Update content of widget
 */
void
HistologyOverlaySettingsMenu::updateContent()
{
    CaretAssert(m_histologyOverlay);
    const HistologyOverlay::SelectionData selectionData(m_histologyOverlay->getSelectionData());

}


/**
 * Called when reload button is clicked
 */
void
HistologyOverlaySettingsMenu::reloadActionTriggered()
{
//    const MediaOverlay::SelectionData selectionData(m_mediaOverlay->getSelectionData());
//    if (selectionData.m_selectedCziImageFile != NULL) {
//        selectionData.m_selectedCziImageFile->reloadPyramidLayerInTabOverlay(m_mediaOverlay->m_tabIndex,
//                                                                             m_mediaOverlay->m_overlayIndex);
//        EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
//        updateContent();
//    }
}


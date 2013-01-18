
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <QBoxLayout>
#include <QCheckBox>
#include <QGroupBox>
#include <QLabel>
#include <QTabWidget>

#define __MAP_SETTINGS_EDITOR_DIALOG_DECLARE__
#include "MapSettingsEditorDialog.h"
#undef __MAP_SETTINGS_EDITOR_DIALOG_DECLARE__

#include "CaretMappableDataFile.h"
#include "MapSettingsOverlayWidget.h"
#include "MapSettingsPaletteColorMappingWidget.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::MapSettingsScalarDataEditorDialog 
 * \brief Dialog for editing scalar data map settings
 *
 * Presents controls for setting palettes, and thresholding used to color
 * scalar data.
 */

/**
 * Constructor for editing a palette selection.
 *
 * @param parent
 *    Parent widget on which this dialog is displayed.
 */
MapSettingsEditorDialog::MapSettingsEditorDialog(QWidget* parent)
: WuQDialogNonModal("Overlay and Map Settings",
                    parent)
{
    /*
     * No context menu, it screws things up
     */
    this->setContextMenuPolicy(Qt::NoContextMenu);
    
    this->setDeleteWhenClosed(false);

    m_caretMappableDataFile = NULL;
    m_mapFileIndex = -1;
    
    /*
     * No apply button
     */
    this->setApplyButtonText("");
    
    m_overlayWidget = new MapSettingsOverlayWidget();
    
    m_paletteColorMappingWidget = new MapSettingsPaletteColorMappingWidget();
    
    QWidget* windowOptionsWidget = this->createWindowOptionsSection();
    
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(m_overlayWidget,
                      "Overlay");
    tabWidget->addTab(m_paletteColorMappingWidget,
                      "Palette");
    
    QWidget* w = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(w);
    this->setLayoutMargins(layout);
    layout->addWidget(tabWidget);
    layout->addWidget(windowOptionsWidget);

    this->setCentralWidget(w);
}

/**
 * Destructor.
 */
MapSettingsEditorDialog::~MapSettingsEditorDialog()
{
}

/**
 * May be called to update the dialog's content.
 *
 * @param overlay
 *    Overlay for the dialog.
 * @param caretMappableDataFile
 *    Mappable file in dialog.
 * @param mapFileIndex
 *    Index of file in dialog.
 */
void 
MapSettingsEditorDialog::updateDialogContent(Overlay* overlay,
                                             CaretMappableDataFile* caretMappableDataFile,
                                             const int32_t mapFileIndex)
{
    m_overlay = overlay;
    m_caretMappableDataFile = caretMappableDataFile;
    m_mapFileIndex = mapFileIndex;
    
    bool isPaletteValid = false;
    
    if (m_caretMappableDataFile != NULL) {
        if (caretMappableDataFile->isMappedWithPalette()) {
            isPaletteValid = true;
            m_paletteColorMappingWidget->updateEditor(m_caretMappableDataFile,
                                                      m_mapFileIndex);
        }
    }
  
    bool isOverlayValid = false;
    if (m_overlay != NULL) {
        m_overlayWidget->updateContent(m_overlay);
        isOverlayValid = true;
    }
    
    m_overlayWidget->setEnabled(isOverlayValid);
    m_paletteColorMappingWidget->setEnabled(isPaletteValid);
}

/**
 * May be called to update the dialog.
 */
void
MapSettingsEditorDialog::updateDialog()
{
    updateDialogContent(m_overlay,
                        m_caretMappableDataFile,
                        m_mapFileIndex);
}


/**
 * Called when close button pressed.
 */ 
void
MapSettingsEditorDialog::closeButtonPressed()
{
    /*
     * Allow this dialog to be reused (checked means DO NOT reuse)
     */
    m_doNotReplaceCheckBox->setCheckState(Qt::Unchecked);
    
    WuQDialogNonModal::closeButtonPressed();
}

/**
 * Set the layout margins.
 * @param layout
 *   Layout for which margins are set.
 */
void 
MapSettingsEditorDialog::setLayoutMargins(QLayout* layout)
{
    WuQtUtilities::setLayoutMargins(layout, 5, 3);
}

/**
 * @return Is the Do Not Replace selected.  If so, this instance of the
 * dialog should not be replaced.
 */
bool 
MapSettingsEditorDialog::isDoNotReplaceSelected() const
{
    const bool checked = (m_doNotReplaceCheckBox->checkState() == Qt::Checked);
    return checked;
}

/**
 * Called when the state of the do not reply checkbox is changed.
 * @param state
 *    New state of checkbox.
 */
void 
MapSettingsEditorDialog::doNotReplaceCheckBoxStateChanged(int /*state*/)
{
//    const bool checked = (state == Qt::Checked);
}

/**
 * @return A widget containing the window options.
 */
QWidget*
MapSettingsEditorDialog::createWindowOptionsSection()
{
    m_doNotReplaceCheckBox = new QCheckBox("Do Not Replace");
    m_doNotReplaceCheckBox->setToolTip("If checked: \n"
                                           "   (1) this window remains displayed until it is\n"
                                           "       closed.\n"
                                           "   (2) if the user selects editing of another map's\n"
                                           "       palette, it will not replace the content of\n"
                                           "       this window.\n"
                                           "If NOT checked:\n"
                                           "   If the user selects editing of another map's \n"
                                           "   palette, it will replace the content of this\n"
                                           "   window.");
    
    QGroupBox* optionsGroupBox = new QGroupBox("Window Options");
    QVBoxLayout* optionsLayout = new QVBoxLayout(optionsGroupBox);
    this->setLayoutMargins(optionsLayout);
    optionsLayout->addWidget(m_doNotReplaceCheckBox);
    optionsGroupBox->setSizePolicy(QSizePolicy(QSizePolicy::MinimumExpanding,
                                               QSizePolicy::Fixed));
    
    return optionsGroupBox;
}




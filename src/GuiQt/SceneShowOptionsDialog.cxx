
/*LICENSE_START*/
/*
 *  Copyright (C) 2017 Washington University School of Medicine
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

#define __SCENE_SHOW_OPTIONS_DIALOG_DECLARE__
#include "SceneShowOptionsDialog.h"
#undef __SCENE_SHOW_OPTIONS_DIALOG_DECLARE__

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>
#include <QVBoxLayout>

#include "CaretAssert.h"
#include "WuQDataEntryDialog.h"

using namespace caret;


    
/**
 * \class caret::SceneShowOptionsDialog 
 * \brief Dialog for setting show scene options
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param options
 *     Options displayed in dialog.
 * @param parent
 *     Optional parent for dialog.
 */
SceneShowOptionsDialog::SceneShowOptionsDialog(const Options& options,
                                               QWidget* parent)
: WuQDialogModal("Show Scene Options",
                 parent)
{
    /*
     * Use colors contained in the scene for background and foreground
     */
    const QString colorTip("Override foreground/background colors from preferences with those in scene");
    
    m_useSceneColorsCheckBox = new QCheckBox("Use background and foreground colors from scene");
    m_useSceneColorsCheckBox->setToolTip(colorTip);
    m_useSceneColorsCheckBox->setChecked(options.isUseSceneColorsSelected());
    
    m_useSceneColorsToolButton = new QToolButton();
    m_useSceneColorsToolButton->setText("Info...");
    QObject::connect(m_useSceneColorsToolButton, &QToolButton::clicked,
                     this, &SceneShowOptionsDialog::useSceneColorsInfoButtonClicked);
    
    QHBoxLayout* useSceneColorsLayout = new QHBoxLayout();
    useSceneColorsLayout->addWidget(m_useSceneColorsCheckBox);
    useSceneColorsLayout->addWidget(m_useSceneColorsToolButton);
    useSceneColorsLayout->addStretch();
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addLayout(useSceneColorsLayout);
    
    
    dialogWidget->setSizePolicy(QSizePolicy::Fixed,
                                QSizePolicy::Fixed);
    setCentralWidget(dialogWidget, WuQDialog::SCROLL_AREA_NEVER);
}

/**
 * Destructor.
 */
SceneShowOptionsDialog::~SceneShowOptionsDialog()
{
}

/**
 * @return Status of options in dialog.
 */
SceneShowOptionsDialog::Options
SceneShowOptionsDialog::getOptions() const
{
    return Options(m_useSceneColorsCheckBox->isChecked());
}

void
SceneShowOptionsDialog::useSceneColorsInfoButtonClicked()
{
    const QString infoText("Scenes created after 01 May 2016 contain the background and foreground from when the\n"
                            "scene was created.  If this box is checked, background and foreground colors from the\n"
                            "scene will override the foreground and background colors in the Preferences Dialog.\n"
                            "The background and foreground colors will revert to those on the Preferences Dialog\n"
                            "when any of these events occur:\n"
                            "   * This checkbox is unchecked\n"
                            "   * A scene without background and foreground colors is loaded\n"
                            "   * A Spec File and its data files are loaded\n"
                            "   * File Menu->Close All Files is selected\n"
                            "   * Any of the colors on the Preferences Dialog are changed\n"
                            "   * The user quits wb_view");
    
    WuQDataEntryDialog ded("Scene Colors Info",
                           m_useSceneColorsToolButton);
    ded.hideCancelButton();
    QLabel* label = new QLabel(infoText);
    ded.addWidget(label);
    ded.exec();
}

/**
 * Gets called if the user presses the OK button.
 */
void
SceneShowOptionsDialog::okButtonClicked()
{
    WuQDialogModal::okButtonClicked();
}


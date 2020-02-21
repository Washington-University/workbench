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

#define __PALETTE_EDITOR_DIALOG_DECLARE__
#include "PaletteEditorDialog.h"
#undef __PALETTE_EDITOR_DIALOG_DECLARE__

#include <QButtonGroup>
#include <QColorDialog>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>

#include "Brain.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventPaletteGetByName.h"
#include "GuiManager.h"
#include "Palette.h"
#include "PaletteFile.h"
#include "PaletteEditorControlPointGroupWidget.h"
#include "PalettePixmapPainter.h"
#include "WuQScrollArea.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::PaletteEditorDialog 
 * \brief Dialog for editing color palettes
 * \ingroup GuiQt
 */

/**
 * Constructor.
 * @param parent
 *    The parent dialog
 */
PaletteEditorDialog::PaletteEditorDialog(QWidget* parent)
: WuQDialogNonModal("Palette Editor",
                    parent)
{
    /*
     * No apply button
     */
    setApplyButtonText("");
    disableAutoDefaultForAllPushButtons();
    
    m_colorEditButtonGroup = new QButtonGroup(this);
    m_colorEditButtonGroup->setExclusive(true);
    
    QWidget* paletteWidget = createPaletteWidget();
    QWidget* paletteSelectionWidget = createPaletteSelectionWidget();
    QWidget* paletteMovementButtonsWidget = createPaletteMovementButtonsWidget();
    QWidget* controlPointsWidget = createControlPointsWidget();
    
    m_colorEditorDialog = NULL;
#if QT_VERSION >= 0x050900
    m_colorEditorDialog = new QColorDialog(this);
    m_colorEditorDialog->setWindowFlag(Qt::Widget);
    m_colorEditorDialog->setOption(QColorDialog::NoButtons);
    m_colorEditorDialog->setOption(QColorDialog::DontUseNativeDialog);
#endif
    
    QGroupBox* colorEditorGroupBox = new QGroupBox("Edit Color of Selected Control Point");
    QVBoxLayout* colorEditorLayout = new QVBoxLayout(colorEditorGroupBox);
    colorEditorLayout->setContentsMargins(0, 0, 0, 0);
    if (m_colorEditorDialog != NULL) {
        colorEditorLayout->addWidget(m_colorEditorDialog);
    }
    else {
        colorEditorLayout->addWidget(new QLabel("Qt 5.9 or later needed for color editor"));
    }
    
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(paletteWidget, 0);
    topLayout->addWidget(paletteMovementButtonsWidget, 0);
    topLayout->addWidget(paletteSelectionWidget, 0);
    topLayout->addStretch();
    
    QHBoxLayout* bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(controlPointsWidget, 0);
    bottomLayout->addWidget(colorEditorGroupBox, 0, Qt::AlignTop);
    
    QWidget* dialogWidget = new QWidget();
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addLayout(topLayout);
    dialogLayout->addLayout(bottomLayout);
    
    setCentralWidget(dialogWidget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    updateControlPointWidgets();

    /*
     * No resizing of dialog
     */
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
PaletteEditorDialog::~PaletteEditorDialog()
{
}

/**
 * Update the dialog.
 */
void
PaletteEditorDialog::updateDialog()
{
    EventPaletteGetByName paletteEvent("ROY-BIG-BL");
    EventManager::get()->sendEvent(paletteEvent.getPointer());
    const Palette* palette = paletteEvent.getPalette();
    if (palette != NULL) {
        PalettePixmapPainter palettePainter(palette,
                                            m_colorBarImageLabel->size());
        QPixmap pixmap = palettePainter.getPixmap();
        if ( ! pixmap.isNull()) {
            m_colorBarImageLabel->setPixmap(pixmap);
        }
    }
    
    updateControlPointWidgets();
}

/**
 * Slot for editing a color
 */
void
PaletteEditorDialog::editColor(const uint8_t rgb[3])
{
    if (m_colorEditorDialog != NULL) {
        m_colorEditorDialog->setCurrentColor(QColor(rgb[0], rgb[1], rgb[2]));
    }
}

QWidget*
PaletteEditorDialog::createControlPointsWidget()
{
    m_positiveControlPointsWidget = new PaletteEditorControlPointGroupWidget(this,
                                                                             m_colorEditButtonGroup);
    QObject::connect(m_positiveControlPointsWidget, &PaletteEditorControlPointGroupWidget::editColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QGroupBox* positiveWidget = new QGroupBox("Positive Mapping");
    QVBoxLayout* positiveLayout = new QVBoxLayout(positiveWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(positiveLayout, 0, 0);
    positiveLayout->addWidget(m_positiveControlPointsWidget);

    m_zeroControlPointsWidget = new PaletteEditorControlPointGroupWidget(this,
                                                                         m_colorEditButtonGroup);
    QObject::connect(m_zeroControlPointsWidget, &PaletteEditorControlPointGroupWidget::editColorRequested,
                     this, &PaletteEditorDialog::editColor);
    
    QGroupBox* zeroWidget = new QGroupBox("Zero Mapping");
    QVBoxLayout* zeroLayout = new QVBoxLayout(zeroWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(zeroLayout, 0, 0);
    zeroLayout->addWidget(m_zeroControlPointsWidget);
    
    m_negativeControlPointsWidget = new PaletteEditorControlPointGroupWidget(this,
                                                                             m_colorEditButtonGroup);
    QObject::connect(m_negativeControlPointsWidget, &PaletteEditorControlPointGroupWidget::editColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QGroupBox* negativeWidget = new QGroupBox("Negative Mapping");
    QVBoxLayout* negativeLayout = new QVBoxLayout(negativeWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(negativeLayout, 0, 0);
    negativeLayout->addWidget(m_negativeControlPointsWidget);

    QWidget* widget = new QWidget();
    widget->setSizePolicy(QSizePolicy::Fixed,
                          QSizePolicy::Fixed);
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(positiveWidget);
    layout->addSpacing(10);
    layout->addWidget(zeroWidget);
    layout->addSpacing(10);
    layout->addWidget(negativeWidget);

    m_scrollArea = WuQScrollArea::newInstance(240, -1);
    m_scrollArea->setWidget(widget);
    m_scrollArea->setWidgetResizable(true);
    m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_scrollArea->setSizeAdjustPolicy(QScrollArea::AdjustToContents);
    m_scrollArea->setSizePolicy(QSizePolicy::Fixed,
                              m_scrollArea->sizePolicy().verticalPolicy());
    
    return m_scrollArea;
}

/**
 * Update the control point widgets
 */
void
PaletteEditorDialog::updateControlPointWidgets()
{
    m_positiveControlPointsWidget->updateContent(NULL, 5);
    m_zeroControlPointsWidget->updateContent(NULL, 0);
    m_negativeControlPointsWidget->updateContent(NULL, -4);
    
//    m_positiveControlPointsWidget->adjustSize();
//    m_zeroControlPointsWidget->adjustSize();
//    m_negativeControlPointsWidget->adjustSize();
}

QWidget*
PaletteEditorDialog::createPaletteSelectionWidget()
{
    QSize iconSize(80, 18);
    
    m_paletteSelectionComboBox = new QComboBox();
    PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
    
    const int32_t numPalettes = paletteFile->getNumberOfPalettes();
    for (int32_t i = 0; i < numPalettes; i++) {
        Palette* palette = paletteFile->getPalette(i);
        const AString name = palette->getName();
        /*
         * Second parameter is user data.  In the future, there may be user-editable
         * palettes and it is possible there may be palettes with the same name.
         * Thus, the user-data may change to a unique-identifier that is different
         * than the palette name.
         */
        const AString paletteUniqueID(name);
        
        PalettePixmapPainter palettePainter(palette,
                                            iconSize);
        QPixmap pixmap = palettePainter.getPixmap();
        if (pixmap.isNull()) {
            m_paletteSelectionComboBox->addItem(name);
        }
        else {
            m_paletteSelectionComboBox->addItem(pixmap, name);
        }
    }
    m_paletteSelectionComboBox->setIconSize(iconSize);

    m_paletteTypeTabWidget = new QTabWidget();
    m_paletteTypeTabWidget->addTab(new QComboBox(), "Files");
    m_paletteTypeTabWidget->addTab(m_paletteSelectionComboBox, "Template");
    m_paletteTypeTabWidget->addTab(new QComboBox(), "User");
    m_paletteTypeTabWidget->setCurrentWidget(m_paletteSelectionComboBox);

    QPushButton* renamePushButton = new QPushButton("Rename...");
    QPushButton* deletePushButton = new QPushButton("Delete...");
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(renamePushButton);
    buttonsLayout->addWidget(deletePushButton);
    buttonsLayout->addStretch();

    QWidget* widget = new QGroupBox("Palettes");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(m_paletteTypeTabWidget, 100);
    layout->addLayout(buttonsLayout);
    
    return widget;
}

QWidget*
PaletteEditorDialog::createPaletteMovementButtonsWidget()
{
    QPushButton* addPushButton     = new QPushButton("Add-->");
    QPushButton* replacePushButton = new QPushButton("Replace-->");
    QPushButton* loadPushButton    = new QPushButton("<--Load");

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(addPushButton);
    layout->addWidget(replacePushButton);
    layout->addWidget(loadPushButton);
    return widget;
}

QWidget*
PaletteEditorDialog::createPaletteWidget()
{
    m_colorBarImageLabel = new QLabel();
    m_colorBarImageLabel->setFixedHeight(30);
    m_colorBarImageLabel->setFixedWidth(150);
    
    QPushButton* newPushButton = new QPushButton("New...");

    QWidget* widget = new QGroupBox("Palette");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel("Name: <name-of-palette>"), 0, Qt::AlignLeft);
    layout->addWidget(m_colorBarImageLabel);
    layout->addWidget(newPushButton, 0, Qt::AlignHCenter);
    
    widget->setFixedHeight(widget->sizeHint().height());

    return widget;
}

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
#include <QListWidget>
#include <QPainter>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QToolButton>

#include "Brain.h"
#include "CaretAssert.h"
#include "EventManager.h"
#include "EventPaletteGetByName.h"
#include "GuiManager.h"
#include "Palette.h"
#include "PaletteFile.h"
#include "PaletteEditorControlPointGroupWidget.h"
#include "PalettePixmapPainter.h"
#include "WuQColorEditorWidget.h"
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
    
    m_colorEditButtonGroup = new QButtonGroup(this);
    m_colorEditButtonGroup->setExclusive(true);
    
    QWidget* paletteBarWidget = createPaletteWidget();
    QWidget* paletteSelectionWidget = createPaletteSelectionWidget();
    QWidget* paletteMovementButtonsWidget = createPaletteMovementButtonsWidget();
    QWidget* controlPointsWidget = createControlPointsWidget();
    
    m_colorEditorWidget = new WuQColorEditorWidget();
    
    QGroupBox* colorEditorGroupBox = new QGroupBox("Edit Color of Selected Control Point");
    QVBoxLayout* colorEditorLayout = new QVBoxLayout(colorEditorGroupBox);
    colorEditorLayout->setContentsMargins(0, 0, 0, 0);
    colorEditorLayout->addWidget(m_colorEditorWidget);
     
    QGroupBox* paletteGroupBox = new QGroupBox("Palettes");
    QHBoxLayout* paletteLayout = new QHBoxLayout(paletteGroupBox);
    paletteLayout->addWidget(paletteSelectionWidget);
    
    m_colorDialog = new QColorDialog();
    m_colorDialog->setOption(QColorDialog::NoButtons);
    m_colorDialog->setOption(QColorDialog::DontUseNativeDialog);
    m_colorDialog->setWindowFlag(Qt::Widget);
    
    QWidget* dialogWidget = new QWidget();
    QGridLayout* dialogLayout = new QGridLayout(dialogWidget);
    dialogLayout->setColumnStretch(0,   0);
    dialogLayout->setColumnStretch(1,   0);
    dialogLayout->setColumnStretch(2,   0);
    dialogLayout->setColumnStretch(3,   0);
    dialogLayout->setColumnStretch(4, 100);
    dialogLayout->addWidget(paletteBarWidget, 0, 0, 1, 2);
    dialogLayout->addWidget(paletteGroupBox, 1, 0, Qt::AlignTop);
    dialogLayout->addWidget(paletteMovementButtonsWidget, 1, 1, Qt::AlignTop);
    dialogLayout->addWidget(controlPointsWidget, 0, 2, 2, 1);
    dialogLayout->addWidget(colorEditorGroupBox, 0, 3, 2, 1);
    dialogLayout->addWidget(m_colorDialog, 0, 4, 2, 1);

    setCentralWidget(dialogWidget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    updateControlPointWidgets();

    paletteTypeTabWidgetClicked(m_paletteTypeTabWidget->currentIndex());
    
    /*
     * No resizing of dialog
     */
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
    
    disableAutoDefaultForAllPushButtons();
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
    adjustSize();
}

/**
 * Slot for editing a color
 */
void
PaletteEditorDialog::editColor(const uint8_t red, const uint8_t green, const uint8_t blue)
{
    m_colorEditorWidget->setCurrentColor(red, green, blue);
    m_colorDialog->setCurrentColor(QColor(red, green, blue));
}

QWidget*
PaletteEditorDialog::createControlPointsWidget()
{
    m_positiveControlPointsWidget = new PaletteEditorControlPointGroupWidget(this,
                                                                             m_colorEditButtonGroup,
                                                                             true);
    QObject::connect(m_positiveControlPointsWidget, &PaletteEditorControlPointGroupWidget::editColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QGroupBox* positiveWidget = new QGroupBox("Positive Mapping");
    QVBoxLayout* positiveLayout = new QVBoxLayout(positiveWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(positiveLayout, 0, 0);
    positiveLayout->addWidget(m_positiveControlPointsWidget);

    m_zeroControlPointsWidget = new PaletteEditorControlPointGroupWidget(this,
                                                                         m_colorEditButtonGroup,
                                                                         false);
    QObject::connect(m_zeroControlPointsWidget, &PaletteEditorControlPointGroupWidget::editColorRequested,
                     this, &PaletteEditorDialog::editColor);
    
    QGroupBox* zeroWidget = new QGroupBox("Zero Mapping");
    QVBoxLayout* zeroLayout = new QVBoxLayout(zeroWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(zeroLayout, 0, 0);
    zeroLayout->addWidget(m_zeroControlPointsWidget);
    
    m_negativeControlPointsWidget = new PaletteEditorControlPointGroupWidget(this,
                                                                             m_colorEditButtonGroup,
                                                                             false);
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

    WuQScrollArea* scrollArea = WuQScrollArea::newInstance(240, -1);
    scrollArea->setWidget(widget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizeAdjustPolicy(QScrollArea::AdjustToContents);
    scrollArea->setSizePolicy(QSizePolicy::Fixed,
                              scrollArea->sizePolicy().verticalPolicy());
    
    return scrollArea;
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
}

QWidget*
PaletteEditorDialog::createPaletteSelectionWidget()
{
    m_filePaletteSelectionWidget = createFilePaletteWidget();
    m_templatePaletteSelectionWidget = createTemplatePaletteWidget();
    m_userPaletteSelectionWidget = createUserPaletteWidget();
    
    m_paletteTypeTabWidget = new QTabWidget();
    m_paletteTypeTabWidget->addTab(m_filePaletteSelectionWidget, "Files");
    m_paletteTypeTabWidget->addTab(m_templatePaletteSelectionWidget, "Template");
    m_paletteTypeTabWidget->addTab(m_userPaletteSelectionWidget, "User");
    m_paletteTypeTabWidget->setCurrentWidget(m_userPaletteSelectionWidget);
    QObject::connect(m_paletteTypeTabWidget, &QTabWidget::tabBarClicked,
                     this, &PaletteEditorDialog::paletteTypeTabWidgetClicked);

    return m_paletteTypeTabWidget;
}

QWidget*
PaletteEditorDialog::createPaletteMovementButtonsWidget()
{
    m_addPalettePushButton     = new QPushButton("<-- Add");

    m_replacePalettePushButton = new QPushButton("<-- Replace");

    m_editPalettePushButton    = new QPushButton("Load -->");

    WuQtUtilities::matchWidgetWidths(m_addPalettePushButton,
                                     m_replacePalettePushButton,
                                     m_editPalettePushButton);
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addStretch();
    layout->addWidget(m_addPalettePushButton);
    layout->addWidget(m_replacePalettePushButton);
    layout->addWidget(m_editPalettePushButton);
    layout->addStretch();
    
    return widget;
}

QWidget*
PaletteEditorDialog::createPaletteWidget()
{
    m_colorBarImageLabel = new QLabel();
    m_colorBarImageLabel->setFixedHeight(24);
    m_colorBarImageLabel->setFixedWidth(250);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(new QLabel("Name: <name-of-palette>"), 0, Qt::AlignLeft);
    layout->addWidget(m_colorBarImageLabel);
    layout->addStretch();
    
    widget->setFixedHeight(widget->sizeHint().height());

    return widget;
}

QWidget*
PaletteEditorDialog::createFilePaletteWidget()
{
    m_filePaletteSelectionListWidget = new QListWidget();
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(m_filePaletteSelectionListWidget);
    layout->addStretch();

    return widget;
}

QWidget*
PaletteEditorDialog::createTemplatePaletteWidget()
{
    QSize iconSize(80, 18);
    
    m_templatePaletteSelectionListWidget = new QListWidget();
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
            m_templatePaletteSelectionListWidget->addItem(name);
        }
        else {
            m_templatePaletteSelectionListWidget->addItem(new QListWidgetItem(pixmap, name));
        }
    }
    m_templatePaletteSelectionListWidget->setIconSize(iconSize);

    QWidget* widget = new QWidget();
    
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(m_templatePaletteSelectionListWidget);
    layout->addStretch();

    return widget;
}

QWidget*
PaletteEditorDialog::createUserPaletteWidget()
{
    m_userPaletteSelectionListWidget = new QListWidget();
    
    QSize iconSize(80, 18);
    PaletteFile* paletteFile = GuiManager::get()->getBrain()->getPaletteFile();
    
    const int32_t numPalettes = std::min(3, paletteFile->getNumberOfPalettes());
    for (int32_t i = 0; i < numPalettes; i++) {
        Palette* palette = paletteFile->getPalette(i);
        AString name = palette->getName();
        switch (i) {
            case 0:
                name = "Colors";
                break;
            case 1:
                name = "Rainbow";
                break;
            case 2:
                name = "Gray";
                break;
        }
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
            m_userPaletteSelectionListWidget->addItem(name);
        }
        else {
            m_userPaletteSelectionListWidget->addItem(new QListWidgetItem(pixmap, name));
        }
    }
    m_userPaletteSelectionListWidget->setIconSize(iconSize);
    
    QPushButton* newPushButton    = new QPushButton("New...");
    QPushButton* renamePushButton = new QPushButton("Rename...");
    QPushButton* deletePushButton = new QPushButton("Delete...");
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addStretch();
    buttonsLayout->addWidget(newPushButton);
    buttonsLayout->addWidget(renamePushButton);
    buttonsLayout->addWidget(deletePushButton);
    buttonsLayout->addStretch();
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    layout->addWidget(m_userPaletteSelectionListWidget);
    layout->addLayout(buttonsLayout);
    layout->addStretch();
    
    return widget;
}


void
PaletteEditorDialog::paletteTypeTabWidgetClicked(int index)
{
    bool addValid(false);
    bool editValid(false);
    bool replaceValid(false);
    
    QWidget* widget = m_paletteTypeTabWidget->widget(index);
    if (widget == m_filePaletteSelectionWidget) {
        editValid    = true;
        replaceValid = true;
    }
    else if (widget == m_templatePaletteSelectionWidget) {
        editValid = true;
    }
    else if (widget == m_userPaletteSelectionWidget) {
        addValid     = true;
        editValid    = true;
        replaceValid = true;
    }
    
    m_addPalettePushButton->setEnabled(addValid);
    m_editPalettePushButton->setEnabled(editValid);
    m_replacePalettePushButton->setEnabled(replaceValid);
}

QPixmap
PaletteEditorDialog::createIcon(QWidget* widget,
                                const IconType iconType)
{
    CaretAssert(widget);
    
    /*
     * Create a small, square pixmap that will contain
     * the foreground color around the pixmap's perimeter.
     */
    float width  = 24.0;
    float height = 24.0;
    
    QPixmap pixmap(static_cast<int>(width),
                   static_cast<int>(height));

    QSharedPointer<QPainter> painter = WuQtUtilities::createPixmapWidgetPainterOriginBottomLeft(widget,
                                                                                                pixmap);
    QPen pen = painter->pen();
    pen.setWidth(2);
    painter->setPen(pen);
    
    /*
     * Creates an icon for "up-to-right icon" as below
     *
     *  ---->
     *  |
     *  |
     */
    painter->drawLine(QLineF(2.0, 2.0,
                             2.0, 16.0));
    painter->drawLine(QLineF(2.0, 16.0,
                             22.0, 16.0));
    painter->drawLine(QLineF(22.0, 16.0,
                             12.0, 22.0));
    painter->drawLine(QLineF(22.0, 16.0,
                             12.0, 10.0));

    QPixmap pixmapOut = pixmap;
    
    switch (iconType) {
        case IconType::ARROW_LEFT_DOWN:
        {
            /*
             * Transforms to a "left-to-down icon" as below
             *
             *    ----
             *    |
             *   \/
             */
            QTransform transform;
            transform.scale(-1, 1);
            transform.rotate(90, Qt::ZAxis);
            transform.translate(width, height);
            pixmapOut = pixmap.transformed(transform);
        }
            break;
        case IconType::ARROW_UP_RIGHT:
            break;
    }
    
    return pixmapOut;
}

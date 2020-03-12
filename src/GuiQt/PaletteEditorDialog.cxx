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
#include "PaletteCreateNewDialog.h"
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
 *    The parent widget
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
    QWidget* controlPointsWidget = createControlPointsWidget();
    
    m_colorEditorWidget = new WuQColorEditorWidget();
    QObject::connect(m_colorEditorWidget, &WuQColorEditorWidget::colorChanged,
                     this, &PaletteEditorDialog::colorEditorColorChanged);
    
    QWidget* dialogWidget = new QWidget();
    QGridLayout* dialogLayout = new QGridLayout(dialogWidget);
    int32_t row(0);
    dialogLayout->addWidget(paletteSelectionWidget, row, 0, 1, 2, Qt::AlignHCenter);
    row++;
    dialogLayout->addWidget(paletteBarWidget, row, 0, 1, 2, Qt::AlignHCenter);
    row++;
    dialogLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 2);
    row++;
    dialogLayout->addWidget(controlPointsWidget, row, 0);
    dialogLayout->addWidget(m_colorEditorWidget, row, 1, Qt::AlignTop);
    row++;

    setCentralWidget(dialogWidget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    updateControlPointWidgets();
    
    /*
     * No resizing of dialog
     */
    setFixedSize(sizeHint());
    
    disableAutoDefaultForAllPushButtons();
}

/**
 * Destructor.
 */
PaletteEditorDialog::~PaletteEditorDialog()
{
}

/**
 * Color editor changed
 * @param red
 * New red component
 * @param green
 * New green component
 * @param blue
 * New blue component
 */
void
PaletteEditorDialog::colorEditorColorChanged(const uint8_t red,
                                             const uint8_t green,
                                             const uint8_t blue)
{
    m_positiveControlPointsWidget->updateControlPointColor(red,
                                                           green,
                                                           blue);
    m_zeroControlPointsWidget->updateControlPointColor(red,
                                                       green,
                                                       blue);
    m_negativeControlPointsWidget->updateControlPointColor(red,
                                                           green,
                                                           blue);
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
                                            m_colorBarImageLabel->size(),
                                            m_pixmapMode);
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
 * @param controlPointIndex
 * Index of the control point
 * @param red
 * The red color component
 * @param green
 * The green color component
 * @param blue
 * The blue color component
 */
void
PaletteEditorDialog::editColor(const int32_t controlPointIndex, const uint8_t red, const uint8_t green, const uint8_t blue)
{
    m_colorEditorWidget->setCurrentColor(red, green, blue);
}

/**
 * Create the control point editing widgets
 */
QWidget*
PaletteEditorDialog::createControlPointsWidget()
{
    m_positiveControlPointsWidget = new PaletteEditorControlPointGroupWidget(m_colorEditButtonGroup,
                                                                             true,
                                                                             this);
    QObject::connect(m_positiveControlPointsWidget, &PaletteEditorControlPointGroupWidget::editColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QGroupBox* positiveWidget = new QGroupBox("Positive Mapping");
    QVBoxLayout* positiveLayout = new QVBoxLayout(positiveWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(positiveLayout, 0, 0);
    positiveLayout->addWidget(m_positiveControlPointsWidget);

    m_zeroControlPointsWidget = new PaletteEditorControlPointGroupWidget(m_colorEditButtonGroup,
                                                                         false,
                                                                         this);
    QObject::connect(m_zeroControlPointsWidget, &PaletteEditorControlPointGroupWidget::editColorRequested,
                     this, &PaletteEditorDialog::editColor);
    
    QGroupBox* zeroWidget = new QGroupBox("Zero Mapping");
    QVBoxLayout* zeroLayout = new QVBoxLayout(zeroWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(zeroLayout, 0, 0);
    zeroLayout->addWidget(m_zeroControlPointsWidget);
    
    m_negativeControlPointsWidget = new PaletteEditorControlPointGroupWidget(m_colorEditButtonGroup,
                                                                             false,
                                                                             this);
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
    scrollArea->setFrameShape(QFrame::NoFrame);
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

/*
 * @return Create the palette widget that shows the current palette being edited
 */
QWidget*
PaletteEditorDialog::createPaletteWidget()
{
    m_colorBarImageLabel = new QLabel();
    m_colorBarImageLabel->setFixedHeight(24);
    m_colorBarImageLabel->setFixedWidth(500);
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_colorBarImageLabel);
    layout->addStretch();

    return widget;
}

/**
 * @return Create the widget for selection of palettes
 */
QWidget*
PaletteEditorDialog::createPaletteSelectionWidget()
{
    m_paletteSourceComboBox = new QComboBox();
    m_paletteSourceComboBox->addItem("User Palettes");
    
    m_userPaletteSelectionComboBox = new QComboBox();
    
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
                                            iconSize,
                                            m_pixmapMode);
        QPixmap pixmap = palettePainter.getPixmap();
        if (pixmap.isNull()) {
            m_userPaletteSelectionComboBox->addItem(name);
        }
        else {
            m_userPaletteSelectionComboBox->addItem(pixmap, name);
        }
    }
    m_userPaletteSelectionComboBox->setIconSize(iconSize);
    
    const QString newToolTip("Create a new palette by (1) copying an existing palette or "
                             "(2) from a new, empty palette ");
    QPushButton* newPushButton    = new QPushButton("New...");
    WuQtUtilities::setWordWrappedToolTip(newPushButton, newToolTip);
    QObject::connect(newPushButton, &QPushButton::clicked,
                     this, &PaletteEditorDialog::newPaletteButtonClicked);
    
    QPushButton* renamePushButton = new QPushButton("Rename...");
    renamePushButton->setToolTip("Rename the selected palette");
    
    QPushButton* deletePushButton = new QPushButton("Delete...");
    deletePushButton->setToolTip("Delete the selected palette");
    
    const bool showImportExportButtonsFlag(false);
    QPushButton* importPushButton(NULL);
    QPushButton* exportPushButton(NULL);
    
    if (showImportExportButtonsFlag) {
        importPushButton = new QPushButton("Import...");
        importPushButton->setToolTip("Import a palette from a wb_view palette file");
        
        exportPushButton = new QPushButton("Export...");
        exportPushButton->setToolTip("Export the selected palette to a wb_view palette file");
    }
    
    QHBoxLayout* sourceLayout = new QHBoxLayout();
    sourceLayout->addWidget(m_paletteSourceComboBox);
    sourceLayout->addWidget(m_userPaletteSelectionComboBox);
    
    QHBoxLayout* buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(newPushButton);
    buttonsLayout->addWidget(renamePushButton);
    buttonsLayout->addWidget(deletePushButton);
    if (importPushButton != NULL) {
        buttonsLayout->addWidget(importPushButton);
    }
    if (exportPushButton != NULL) {
        buttonsLayout->addWidget(exportPushButton);
    }
    
    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    layout->addLayout(sourceLayout, Qt::AlignHCenter);
    layout->addLayout(buttonsLayout, Qt::AlignHCenter);
    layout->addStretch();
    
    return widget;
}

/**
 * Called when button for creating a new palette is clicked
 */
void
PaletteEditorDialog::newPaletteButtonClicked()
{
    PaletteCreateNewDialog dialog(m_pixmapMode,
                                  this);
    dialog.exec();
}

/**
 * NOT USED
 */
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

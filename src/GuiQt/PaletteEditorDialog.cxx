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
#include "PaletteEditorRangeWidget.h"
#include "PaletteNew.h"
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
    
    /*
     * Create dummy user palettes
     */
    createUserPalettes();
    
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
PaletteEditorDialog::colorEditorColorChanged(const QColor& color)
{
    CaretRgb rgb(color.red(),
                 color.green(),
                 color.blue());
    
    m_positiveRangeWidget->updateControlPointColor(rgb);
    m_zeroRangeWidget->updateControlPointColor(rgb);
    m_negativeRangeWidget->updateControlPointColor(rgb);
//    updatePaletteColorBarImage();
}

/**
 * Update the dialog.
 */
void
PaletteEditorDialog::updateDialog()
{
    adjustSize();
}

/**
 * Load a copy of the palette into the dialog
 */
void
PaletteEditorDialog::loadPalette(const PaletteNew* palette)
{
    m_currentPaletteName.clear();
    
    std::vector<PaletteNew::ScalarColor> positiveScalars;
    std::vector<PaletteNew::ScalarColor> negativeScalars;
    std::vector<PaletteNew::ScalarColor> zeroScalars;

    if (palette != NULL) {
        m_currentPaletteName = palette->getName();
        positiveScalars = palette->getPosRange();
        negativeScalars = palette->getNegRange();
        
        float zeroRgb[3];
        palette->getZeroColor(zeroRgb);
        zeroScalars.emplace_back(0.0,
                                 zeroRgb);
    }
    
    m_positiveRangeWidget->updateContent(positiveScalars);
    m_negativeRangeWidget->updateContent(negativeScalars);
    m_zeroRangeWidget->updateContent(zeroScalars);

    updatePaletteColorBarImage();
}

/**
 * @return Palette created from current settings in editor
 */
std::unique_ptr<PaletteNew>
PaletteEditorDialog::getPaletteFromEditor() const
{
    std::vector<PaletteNew::ScalarColor> pos = m_positiveRangeWidget->getScalarColors();
    std::vector<PaletteNew::ScalarColor> neg = m_negativeRangeWidget->getScalarColors();
    std::vector<PaletteNew::ScalarColor> zero = m_zeroRangeWidget->getScalarColors();
    
    std::unique_ptr<PaletteNew> palettePtr;
    
    if ((pos.size() >= 2)
        && (neg.size() >= 2)
        && (zero.size() >= 1)) {
        PaletteNew* pal = new PaletteNew(pos,
                                         zero[0].color,
                                         neg);
        palettePtr.reset(pal);
    }

    return palettePtr;
}

/**
 * Update the palette color bar image
 */
void
PaletteEditorDialog::updatePaletteColorBarImage()
{
    std::unique_ptr<PaletteNew> palette = getPaletteFromEditor();
    if (palette) {
        PalettePixmapPainter palettePainter(palette.get(),
                                            m_colorBarImageLabel->size(),
                                            m_pixmapMode);
        QPixmap pixmap = palettePainter.getPixmap();
        if ( ! pixmap.isNull()) {
            m_colorBarImageLabel->setPixmap(pixmap);
        }
        else {
            m_colorBarImageLabel->setPixmap(QPixmap());
        }
    }
    else {
        m_colorBarImageLabel->setPixmap(QPixmap());
    }
}

/**
 * Slot for editing a color
 * @param rgb
 *    The color for editing
 */
void
PaletteEditorDialog::editColor(const CaretRgb& rgb)
{
    m_colorEditorWidget->setCurrentColor(QColor(rgb.red(),
                                                rgb.green(),
                                                rgb.blue()));
    updatePaletteColorBarImage();
}

/**
 * @return New instance of the control point editing widgets
 */
QWidget*
PaletteEditorDialog::createControlPointsWidget()
{
    m_positiveRangeWidget = new PaletteEditorRangeWidget(PaletteEditorRangeWidget::DataRangeMode::POSITIVE,
                                                                 m_colorEditButtonGroup,
                                                                 PaletteEditorRangeWidget::ColumnTitlesMode::SHOW_YES,
                                                                 this);
    QObject::connect(m_positiveRangeWidget, &PaletteEditorRangeWidget::signalEditColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QObject::connect(m_positiveRangeWidget, &PaletteEditorRangeWidget::signalDataChanged,
                     this, &PaletteEditorDialog::rangeWidgetDataChanged);
    QGroupBox* positiveWidget = new QGroupBox("Positive Mapping");
    QVBoxLayout* positiveLayout = new QVBoxLayout(positiveWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(positiveLayout, 0, 0);
    positiveLayout->addWidget(m_positiveRangeWidget);

    m_zeroRangeWidget = new PaletteEditorRangeWidget(PaletteEditorRangeWidget::DataRangeMode::ZERO,
                                                             m_colorEditButtonGroup,
                                                             PaletteEditorRangeWidget::ColumnTitlesMode::SHOW_NO,
                                                             this);
    QObject::connect(m_zeroRangeWidget, &PaletteEditorRangeWidget::signalEditColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QObject::connect(m_zeroRangeWidget, &PaletteEditorRangeWidget::signalDataChanged,
                     this, &PaletteEditorDialog::rangeWidgetDataChanged);

    QGroupBox* zeroWidget = new QGroupBox("Zero Mapping");
    QVBoxLayout* zeroLayout = new QVBoxLayout(zeroWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(zeroLayout, 0, 0);
    zeroLayout->addWidget(m_zeroRangeWidget);
    
    m_negativeRangeWidget = new PaletteEditorRangeWidget(PaletteEditorRangeWidget::DataRangeMode::NEGATIVE,
                                                                 m_colorEditButtonGroup,
                                                                 PaletteEditorRangeWidget::ColumnTitlesMode::SHOW_NO,
                                                                 this);
    QObject::connect(m_negativeRangeWidget, &PaletteEditorRangeWidget::signalEditColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QObject::connect(m_negativeRangeWidget, &PaletteEditorRangeWidget::signalDataChanged,
                     this, &PaletteEditorDialog::rangeWidgetDataChanged);
    QGroupBox* negativeWidget = new QGroupBox("Negative Mapping");
    QVBoxLayout* negativeLayout = new QVBoxLayout(negativeWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(negativeLayout, 0, 0);
    negativeLayout->addWidget(m_negativeRangeWidget);

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
 * Called when a range widgets data (scalar/color) is changed
 */
void
PaletteEditorDialog::rangeWidgetDataChanged()
{
    updatePaletteColorBarImage();
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
    QObject::connect(m_userPaletteSelectionComboBox, QOverload<int>::of(&QComboBox::activated),
                     this, &PaletteEditorDialog::userPaletteComboBoxActivated);
    
    QSize iconSize(80, 18);
    
    for (auto& pal : m_userPalettes) {
        /*
         * Second parameter is user data.  In the future, there may be user-editable
         * palettes and it is possible there may be palettes with the same name.
         * Thus, the user-data may change to a unique-identifier that is different
         * than the palette name.
         */
        //const AString paletteUniqueID(name);
        
        PalettePixmapPainter palettePainter(pal.get(),
                                            iconSize,
                                            m_pixmapMode);
        QPixmap pixmap = palettePainter.getPixmap();
        const QString name = pal->getName();
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
 * Called when a user palette is selected
 * @param index
 *     Index of the palette
 */
void
PaletteEditorDialog::userPaletteComboBoxActivated(int index)
{
    if ((index >= 0)
        && (index < m_userPaletteSelectionComboBox->count())) {
        loadPalette(m_userPalettes[index].get());
    }
    
    updateDialog();
}
    

/**
 * Called when button for creating a new palette is clicked
 */
void
PaletteEditorDialog::newPaletteButtonClicked()
{
    PaletteCreateNewDialog dialog(m_pixmapMode,
                                  this);
    if (dialog.exec() == PaletteCreateNewDialog::Accepted) {
        loadPalette(dialog.getPalette());
        updateDialog();
    }
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

/**
 * Create example palettes for testing
 */
void
PaletteEditorDialog::createUserPalettes()
{
    m_userPalettes.clear();
    
    std::unique_ptr<PaletteNew> pal55(PaletteCreateNewDialog::createPaletteNew("Pal55", 5, 5));
    std::unique_ptr<PaletteNew> pal34(PaletteCreateNewDialog::createPaletteNew("Pal34", 3, 4));
    
    m_userPalettes.push_back(std::move(pal55));
    m_userPalettes.push_back(std::move(pal34));
}

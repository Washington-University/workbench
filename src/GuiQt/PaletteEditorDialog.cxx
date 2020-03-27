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
#include <QListWidgetItem>
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
    
    QWidget* paletteBarWidget = createPaletteBarWidget();
    QWidget* paletteSelectionWidget = createPaletteSelectionWidget();
    QWidget* controlPointsWidget = createControlPointsWidget();
    QWidget* movePaletteButtonsWidget = createMovePaletteButtonsWidget();
    
    m_colorEditorWidget = new WuQColorEditorWidget();
    QObject::connect(m_colorEditorWidget, &WuQColorEditorWidget::colorChanged,
                     this, &PaletteEditorDialog::colorEditorColorChanged);
    
    QGroupBox* leftGroupBox = new QGroupBox("Palette Editing");
    QGridLayout* leftLayout = new QGridLayout(leftGroupBox);
    leftLayout->addWidget(paletteBarWidget, 0, 0, 1, 2, Qt::AlignHCenter);
    leftLayout->addWidget(controlPointsWidget, 1, 0);
    leftLayout->addWidget(m_colorEditorWidget, 1, 1);
    
    QWidget* dialogWidget = new QWidget();
    QHBoxLayout* dialogLayout = new QHBoxLayout(dialogWidget);
    dialogLayout->addWidget(leftGroupBox);
    dialogLayout->addWidget(movePaletteButtonsWidget);
    dialogLayout->addWidget(paletteSelectionWidget);

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
    std::vector<PaletteNew::ScalarColor> positiveScalars;
    std::vector<PaletteNew::ScalarColor> negativeScalars;
    std::vector<PaletteNew::ScalarColor> zeroScalars;

    if (palette != NULL) {
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
    
    m_unmodifiedPalette.m_negativeMapping = negativeScalars;
    m_unmodifiedPalette.m_positiveMapping = positiveScalars;
    m_unmodifiedPalette.m_zeroMapping     = zeroScalars;
}

//bool
//operator==(const PaletteNew::ScalarColor& lhs, const PaletteNew::ScalarColor& rhs)
//{
//    if (lhs.scalar != rhs.scalar) {
//        return false;
//    }
//    for (int32_t i = 0; i < 3; i++) {
//        if (lhs.color[i] != rhs.color[i]) {
//            return false;
//        }
//    }
//
//    return true;
//}

/**
 * @return True if the current palette has been modified since it was loaded, else false
 */
bool
PaletteEditorDialog::isPaletteModified() const
{
//    if (m_unmodifiedPalette.m_negativeMapping != m_negativeRangeWidget->getScalarColors()) {
//        return true;
//    }
//    if (m_unmodifiedPalette.m_positiveMapping != m_positiveRangeWidget->getScalarColors()) {
//        return true;
//    }
//    if (m_unmodifiedPalette.m_zeroMapping != m_zeroRangeWidget->getScalarColors()) {
//        return true;
//    }

    return false;
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
    positiveWidget->setFlat(true);
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

    QGroupBox* zeroWidget = new QGroupBox();
    zeroWidget->setFlat(true);
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
    negativeWidget->setFlat(true);
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
    if (isPaletteModified()) {
        std::cout << "Modified" << std::endl;
    }
    else {
        std::cout << "NOT Modified" << std::endl;
    }
}

/*
 * @return Create the palette widget that shows the current palette being edited
 */
QWidget*
PaletteEditorDialog::createPaletteBarWidget()
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
    
    m_userPaletteSelectionListWidget = new QListWidget();
    QObject::connect(m_userPaletteSelectionListWidget, &QListWidget::itemActivated,
                     this, &PaletteEditorDialog::userPaletteListWidgetActivated);
    
    QSize iconSize(80, 18);
    
    for (auto& pal : m_userPalettes) {
        PalettePixmapPainter palettePainter(pal.get(),
                                            iconSize,
                                            m_pixmapMode);
        QPixmap pixmap = palettePainter.getPixmap();
        const QString name = pal->getName();
        if (pixmap.isNull()) {

            m_userPaletteSelectionListWidget->addItem(name);
        }
        else {
            
            m_userPaletteSelectionListWidget->addItem(new QListWidgetItem(pixmap,
                                                                          name));
        }
    }
    m_userPaletteSelectionListWidget->setIconSize(iconSize);
    
    QPushButton* renamePushButton = new QPushButton("Rename...");
    renamePushButton->setToolTip("Rename the selected palette");
    
    QPushButton* deletePushButton = new QPushButton("Delete...");
    deletePushButton->setToolTip("Delete the selected palette");
    
    std::vector<QWidget*> buttonsForSizeMatching;
    buttonsForSizeMatching.push_back(renamePushButton);
    buttonsForSizeMatching.push_back(deletePushButton);
    
    const bool showImportExportButtonsFlag(true);
    QPushButton* importPushButton(NULL);
    QPushButton* exportPushButton(NULL);
    
    if (showImportExportButtonsFlag) {
        importPushButton = new QPushButton("Import...");
        importPushButton->setToolTip("Import a palette from a wb_view palette file");
        buttonsForSizeMatching.push_back(importPushButton);
        
        exportPushButton = new QPushButton("Export...");
        exportPushButton->setToolTip("Export the selected palette to a wb_view palette file");
        buttonsForSizeMatching.push_back(exportPushButton);
    }
    
    WuQtUtilities::matchWidgetWidths(buttonsForSizeMatching);

    QGroupBox* widget = new QGroupBox("Palette Selection");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 4, 0);
    layout->addWidget(m_paletteSourceComboBox);
    layout->addWidget(m_userPaletteSelectionListWidget);
    layout->addWidget(renamePushButton, 0, Qt::AlignHCenter);
    layout->addWidget(deletePushButton, 0, Qt::AlignHCenter);
    if (importPushButton != NULL) {
        layout->addWidget(importPushButton, 0, Qt::AlignHCenter);
    }
    if (exportPushButton != NULL) {
        layout->addWidget(exportPushButton, 0, Qt::AlignHCenter);
    }
    layout->addStretch();
    
    return widget;
}

/**
 * Called when a user palette is selected
 * @param item
 *     Item selected by the user
 */
void
PaletteEditorDialog::userPaletteListWidgetActivated(QListWidgetItem* /*item*/)
{
    /* Nothing */
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

/**
 * @return Instance of the buttons used to move palettes
 * into and out of the editor
 */
QWidget*
PaletteEditorDialog::createMovePaletteButtonsWidget()
{
    const QString addToolTip("Add a new palette containing content of the palette editor");
    m_addPalettePushButton = new QPushButton("Add -->");
    WuQtUtilities::setWordWrappedToolTip(m_addPalettePushButton,
                                         addToolTip);
    QObject::connect(m_addPalettePushButton, &QPushButton::clicked,
                     this, &PaletteEditorDialog::addPalettePushButtonClicked);
    
    const QString replaceToolTip("Replace the selected palette with the content of the palette editor");
    m_replacePalettePushButton = new QPushButton("Replace -->");
    WuQtUtilities::setWordWrappedToolTip(m_replacePalettePushButton,
                                         replaceToolTip);
    QObject::connect(m_replacePalettePushButton, &QPushButton::clicked,
                     this, &PaletteEditorDialog::replacePalettePushButtonClicked);

    const QString loadToolTip("Load palette editor with the selected palette");
    m_loadPalettePushButton = new QPushButton("<-- Load");
    WuQtUtilities::setWordWrappedToolTip(m_loadPalettePushButton,
                                         loadToolTip);
    QObject::connect(m_loadPalettePushButton, &QPushButton::clicked,
                     this, &PaletteEditorDialog::loadPalettePushButtonClicked);

    const QString newToolTip("Create a new palette by (1) copying an existing palette or "
                             "(2) create a new palette");
    m_newPalettePushButton = new QPushButton("<-- New...");
    WuQtUtilities::setWordWrappedToolTip(m_newPalettePushButton, newToolTip);
    QObject::connect(m_newPalettePushButton, &QPushButton::clicked,
                     this, &PaletteEditorDialog::newPaletteButtonClicked);

    QWidget* widget = new QWidget();
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addSpacing(50);
    layout->addWidget(m_addPalettePushButton);
    layout->addWidget(m_replacePalettePushButton);
    layout->addWidget(m_loadPalettePushButton);
    layout->addSpacing(25);
    layout->addWidget(m_newPalettePushButton);
    layout->addStretch();
    
    return widget;
}

/**
 * Called when Add palette push button is clicked
 */
void
PaletteEditorDialog::addPalettePushButtonClicked()
{
    
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
 * Called when Replace palette push button is clicked
 */
void
PaletteEditorDialog::replacePalettePushButtonClicked()
{
    
}

/**
 * Called when Load palette push button is clicked
 */
void
PaletteEditorDialog::loadPalettePushButtonClicked()
{
    const int32_t index = m_userPaletteSelectionListWidget->currentRow();
    if ((index >= 0)
        && (index < m_userPaletteSelectionListWidget->count())) {
        loadPalette(m_userPalettes[index].get());
    }
    
    updateDialog();
}

/**
 * Update the palette movement buttons
 */
void
PaletteEditorDialog::updatePaletteMovementButtons()
{
//    m_addPalettePushButton->setEnabled();
//    m_replacePalettePushButton->setEnabled();
//    m_loadPalettePushButton->setEnabled();
//    m_newPalettePushButton->setEnabled();
}


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
#include <QInputDialog>
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
#include "GuiManager.h"
#include "Palette.h"
#include "PaletteCreateNewDialog.h"
#include "PaletteEditorRangeWidget.h"
#include "PaletteFile.h"
#include "PaletteGroup.h"
#include "PaletteNew.h"
#include "PalettePixmapPainter.h"
#include "PaletteSelectionWidget.h"
#include "WuQColorEditorWidget.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBox.h"
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
    
    const bool demoModeFlag(false);
    if (demoModeFlag) {
        paletteSelectionWidget->setEnabled(false);
        movePaletteButtonsWidget->setEnabled(false);
        
        std::unique_ptr<PaletteNew> palette = m_paletteSelectionWidget->getSelectedPalette();
        loadPaletteIntoEditor(palette);
    }
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
    m_paletteSelectionWidget->updateContent();
    
    updatePaletteMovementButtons();
    
    adjustSize();
}

/**
 * Load a copy of the palette into the dialog
 * @param palette
 *     Palette to load in editor
 */
void
PaletteEditorDialog::loadPaletteIntoEditor(const std::unique_ptr<PaletteNew>& palette)
{
    std::vector<PaletteNew::ScalarColor> positiveScalars;
    std::vector<PaletteNew::ScalarColor> negativeScalars;
    std::vector<PaletteNew::ScalarColor> zeroScalars;

    if (palette) {
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

    clearEditorModified();

    m_positiveRangeWidget->selectFirstControlPoint();
    
    updatePaletteColorBarImage();
}

/**
 * Clear modified status of palette in the editor
 */
void
PaletteEditorDialog::clearEditorModified()
{
    /*
     * Save the initial values so that they can be used to determine if the
     * current palette has been modified.  We do not use the input scalar colors
     * but do use the scalar colors after they have been loaded in the range
     * widgets.  The range widgets have limited precision (3 decimals) due to
     * the spin boxes whereas the input scalar colors have more precision.
     * This difference in precesion would cause a differnce.
     */
    m_unmodifiedPalette.m_negativeMapping = m_negativeRangeWidget->getScalarColors();
    m_unmodifiedPalette.m_positiveMapping = m_positiveRangeWidget->getScalarColors();
    m_unmodifiedPalette.m_zeroMapping     = m_zeroRangeWidget->getScalarColors();

    updateModifiedLabel();
}

/**
 * @return True if the current palette has been modified since it was loaded, else false
 */
bool
PaletteEditorDialog::isPaletteModified() const
{
    if (m_unmodifiedPalette.m_negativeMapping != m_negativeRangeWidget->getScalarColors()) {
        return true;
    }
    if (m_unmodifiedPalette.m_positiveMapping != m_positiveRangeWidget->getScalarColors()) {
        return true;
    }
    if (m_unmodifiedPalette.m_zeroMapping != m_zeroRangeWidget->getScalarColors()) {
        return true;
    }

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
        && (zero.size() == 1)) {
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
    QPixmap colorBarPixmap;
    
    std::unique_ptr<PaletteNew> palette = getPaletteFromEditor();
    if (palette) {
        PalettePixmapPainter palettePainter(palette.get(),
                                            m_colorBarImageLabel->size(),
                                            m_pixmapMode);
        colorBarPixmap = palettePainter.getPixmap();
    }
    
    m_colorBarImageLabel->setPixmap(colorBarPixmap);

    updateModifiedLabel();
}

void
PaletteEditorDialog::updateModifiedLabel()
{
    if (isPaletteModified()) {
        m_colorBarModifiedLabel->setText("<html><font color=\"red\"><bold>MODIFIED</bold></font></html>");
    }
    else {
        m_colorBarModifiedLabel->setText("");
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
    
    m_colorBarModifiedLabel = new QLabel();
    m_colorBarModifiedLabel->setAlignment(Qt::AlignRight);
    m_colorBarModifiedLabel->setFixedWidth(120);

    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_colorBarImageLabel, 0, 0);
    layout->addWidget(m_colorBarModifiedLabel, 0, 1);

    return widget;
}

/**
 * @return Create the widget for selection of palettes
 */
QWidget*
PaletteEditorDialog::createPaletteSelectionWidget()
{
    m_paletteSelectionWidget = new PaletteSelectionWidget();
    QObject::connect(m_paletteSelectionWidget, &PaletteSelectionWidget::paletteSelectionChanged,
                     this, &PaletteEditorDialog::paletteSelected);
    
    m_renamePushButton = new QPushButton("Rename...");
    m_renamePushButton->setToolTip("Rename the selected palette");
    QObject::connect(m_renamePushButton, &QPushButton::clicked,
                     this, &PaletteEditorDialog::renamePushButtonClicked);

    m_deletePushButton = new QPushButton("Delete...");
    m_deletePushButton->setToolTip("Delete the selected palette");
    QObject::connect(m_deletePushButton, &QPushButton::clicked,
                     this, &PaletteEditorDialog::deletePushButtonClicked);
    
    std::vector<QWidget*> buttonsForSizeMatching;
    buttonsForSizeMatching.push_back(m_renamePushButton);
    buttonsForSizeMatching.push_back(m_deletePushButton);
    
    const bool showImportExportButtonsFlag(true);
    m_importPushButton = NULL;
    m_exportPushButton = NULL;
    
    if (showImportExportButtonsFlag) {
        m_importPushButton = new QPushButton("Import...");
        m_importPushButton->setToolTip("Import a palette from a wb_view palette file");
        QObject::connect(m_importPushButton, &QPushButton::clicked,
                         this, &PaletteEditorDialog::importPushButtonClicked);
        buttonsForSizeMatching.push_back(m_importPushButton);
        
        m_exportPushButton = new QPushButton("Export...");
        m_exportPushButton->setToolTip("Export the selected palette to a wb_view palette file");
        QObject::connect(m_exportPushButton, &QPushButton::clicked,
                         this, &PaletteEditorDialog::exportPushButtonClicked);
        buttonsForSizeMatching.push_back(m_exportPushButton);
    }
    
    WuQtUtilities::matchWidgetWidths(buttonsForSizeMatching);

    QGroupBox* widget = new QGroupBox("Palette Selection");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 8, 0);
    layout->addWidget(m_paletteSelectionWidget, 0, Qt::AlignHCenter);
    layout->addWidget(m_renamePushButton, 0, Qt::AlignHCenter);
    layout->addWidget(m_deletePushButton, 0, Qt::AlignHCenter);
    if (m_importPushButton != NULL) {
        layout->addWidget(m_importPushButton, 0, Qt::AlignHCenter);
    }
    if (m_exportPushButton != NULL) {
        layout->addWidget(m_exportPushButton, 0, Qt::AlignHCenter);
    }
    layout->addStretch();
    
    return widget;
}

/**
 * Called when a  palette is selected
 */
void
PaletteEditorDialog::paletteSelected()
{
    updatePaletteMovementButtons();
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
    std::unique_ptr<PaletteNew> palette = getPaletteFromEditor();
    if ( ! palette) {
        return;
    }
    
    PaletteGroup* paletteGroup = m_paletteSelectionWidget->getSelectedPaletteGroup();
    if (paletteGroup == NULL) {
        return;
    }
    
    WuQDataEntryDialog dialog("Add Palette",
                              m_addPalettePushButton);
    m_addPaletteDialogLineEdit = dialog.addLineEditWidget("Palette Name");
    QObject::connect(&dialog, &WuQDataEntryDialog::validateData,
                     this, &PaletteEditorDialog::addPaletteDialogValidateData);
    if (dialog.exec() == QInputDialog::Accepted) {
        palette->setName(m_addPaletteDialogLineEdit->text().trimmed());
        AString errorMessage;
        if ( ! paletteGroup->addPalette(*palette,
                                        errorMessage)) {
            WuQMessageBox::errorOk(m_addPalettePushButton,
                                   errorMessage);
        }
    }
    
    clearEditorModified();
    
    m_paletteSelectionWidget->updateContent();
    updatePaletteMovementButtons();
}

/**
 * Called to validate data when adding a palette from editor to a group
 * @param dataEntryDialog
 *
 */
void
PaletteEditorDialog::addPaletteDialogValidateData(WuQDataEntryDialog* addPaletteDialog)
{
    CaretAssert(addPaletteDialog);
    bool validFlag(false);
    QString errorMessage;
    
    const AString name = m_addPaletteDialogLineEdit->text().trimmed();
    if (name.isEmpty()) {
        errorMessage = "Palette name is missing";
    }
    else {
        PaletteGroup* paletteGroup = m_paletteSelectionWidget->getSelectedPaletteGroup();
        if (paletteGroup->hasPaletteWithName(name)) {
            errorMessage = ("Palette names must be unique.  Choose a "
                            "different palette name.");
        }
        else {
            validFlag = true;
        }

    }
    addPaletteDialog->setDataValid(validFlag,
                                   errorMessage);
}

/**
 * Called when button for creating a new palette is clicked
 */
void
PaletteEditorDialog::newPaletteButtonClicked()
{
    if ( ! modifiedPaletteWarningDialog()) {
        return;
    }
    
    PaletteCreateNewDialog dialog(m_pixmapMode,
                                  this);
    if (dialog.exec() == PaletteCreateNewDialog::Accepted) {
        loadPaletteIntoEditor(dialog.getPalette());
        updateDialog();
    }
}



/**
 * Called when Replace palette push button is clicked
 */
void
PaletteEditorDialog::replacePalettePushButtonClicked()
{
    std::unique_ptr<PaletteNew> palette = getPaletteFromEditor();
    if ( ! palette) {
        return;
    }
    
    PaletteGroup* paletteGroup = m_paletteSelectionWidget->getSelectedPaletteGroup();
    if (paletteGroup == NULL) {
        return;
    }
    
    std::unique_ptr<PaletteNew> oldPalette = m_paletteSelectionWidget->getSelectedPalette();
    if ( ! oldPalette) {
        return;
    }
    
    const QString name = oldPalette->getName();
    
    if ( ! WuQMessageBox::warningOkCancel(m_replacePalettePushButton,
                                       ("Replace palette "
                                        + name
                                        + " ?"))) {
        return;
    }

    palette->setName(name);
    
    AString errorMessage;
    if ( ! paletteGroup->replacePalette(*palette,
                                        errorMessage)) {
        WuQMessageBox::errorOk(m_replacePalettePushButton,
                               errorMessage);
    }
    clearEditorModified();
    
    m_paletteSelectionWidget->updateContent();
    updatePaletteMovementButtons();
}

/**
 * If the palette in the editor has a modified status, warn the user and
 * allow the user to continue or cancel.
 * @return True if the palette is NOT modified
 * True if the Palette is modified and the user chooses to discard the changes
 * False if the Palette is modified and the user DOES NOT want to continue
 */
bool
PaletteEditorDialog::modifiedPaletteWarningDialog()
{
    if (isPaletteModified()) {
        const QString text("The Palette in the editor has been modified.");
        const QString info("Click OK to discard the changes and continue adding/loading "
                           "the new palette.\n\n"
                           "Click Cancel to allow saving the palette in the editor.");
            if ( ! WuQMessageBox::warningOkCancel(this,
                                                  text,
                                                  info,
                                                  WuQMessageBox::DefaultButtonOkCancel::CANCEL)) {
            return false;
        }
    }
    
    return true;
}

/**
 * Called when Load palette push button is clicked
 */
void
PaletteEditorDialog::loadPalettePushButtonClicked()
{
    if ( ! modifiedPaletteWarningDialog()) {
        return;
    }
    
    loadPaletteIntoEditor(m_paletteSelectionWidget->getSelectedPalette());
    
    updateDialog();
}

/**
 * Called when the delete push button is clicked
 */
void
PaletteEditorDialog::deletePushButtonClicked()
{
    PaletteGroup* paletteGroup = m_paletteSelectionWidget->getSelectedPaletteGroup();
    if (paletteGroup != NULL) {
        if (paletteGroup->isEditable()) {
            std::unique_ptr<PaletteNew> palette = m_paletteSelectionWidget->getSelectedPalette();
            if (palette) {
                if (WuQMessageBox::warningOkCancel(m_deletePushButton,
                                                   ("Delete palette "
                                                    + palette->getName()
                                                    +" ?"))) {
                    AString errorMessage;
                    if ( ! paletteGroup->removePalette(palette->getName(),
                                                       errorMessage)) {
                        WuQMessageBox::errorOk(m_deletePushButton,
                                               errorMessage);
                    }
                    else {
                        updateDialog();
                    }
                }
            }
        }
    }
}

/**
 * Called when the rename push button is clicked
 */
void
PaletteEditorDialog::renamePushButtonClicked()
{
    PaletteGroup* paletteGroup = m_paletteSelectionWidget->getSelectedPaletteGroup();
    if (paletteGroup != NULL) {
        if (paletteGroup->isEditable()) {
            std::unique_ptr<PaletteNew> palette = m_paletteSelectionWidget->getSelectedPalette();
            if (palette) {
                bool validFlag(false);
                const AString newName = QInputDialog::getText(m_renamePushButton, "Rename Palette", "New Name",
                                                              QLineEdit::Normal,
                                                              palette->getName(),
                                                              &validFlag).trimmed();
                if ( ! newName.isEmpty()) {
                    AString errorMessage;
                    if ( ! paletteGroup->renamePalette(palette->getName(),
                                                       newName,
                                                       errorMessage)) {
                        WuQMessageBox::errorOk(m_renamePushButton,
                                               errorMessage);
                    }
                    else {
                        updateDialog();
                    }
                }
            }
        }
    }
}

/**
 * Called when the import push button is clicked
 */
void
PaletteEditorDialog::importPushButtonClicked()
{
    
}

/**
 * Called when the export push button is clicked
 */
void
PaletteEditorDialog::exportPushButtonClicked()
{
    
}

/**
 * Update the palette movement buttons
 */
void
PaletteEditorDialog::updatePaletteMovementButtons()
{
    bool editableGroupFlag(false);
    bool paletteSelectedFlag(false);
    const PaletteGroup* paletteGroup = m_paletteSelectionWidget->getSelectedPaletteGroup();
    if (paletteGroup != NULL) {
        editableGroupFlag = paletteGroup->isEditable();
        std::unique_ptr<PaletteNew> palette = m_paletteSelectionWidget->getSelectedPalette();
        paletteSelectedFlag = (palette.get() != NULL);
    }
    
    const bool validEditorPaletteFlag = (getPaletteFromEditor().get() != NULL);
    
    m_addPalettePushButton->setEnabled(editableGroupFlag
                                       && validEditorPaletteFlag);
    m_replacePalettePushButton->setEnabled(editableGroupFlag
                                           && paletteSelectedFlag
                                           && validEditorPaletteFlag);
    m_loadPalettePushButton->setEnabled(paletteSelectedFlag);
    m_newPalettePushButton->setEnabled(true);
    
    m_renamePushButton->setEnabled(editableGroupFlag
                                   && paletteSelectedFlag);
    m_deletePushButton->setEnabled(editableGroupFlag
                                   && paletteSelectedFlag);
    if (m_importPushButton != NULL) {
        m_importPushButton->setEnabled(editableGroupFlag);
    }
    if (m_exportPushButton != NULL) {
        m_exportPushButton->setEnabled(paletteSelectedFlag);
    }
}


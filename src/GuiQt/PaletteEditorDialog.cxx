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
#include <QCloseEvent>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPainter>
#include <QPushButton>
#include <QRadioButton>
#include <QScrollArea>
#include <QTabWidget>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretFileDialog.h"
#include "EventManager.h"
#include "EventPaletteNewOperation.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventSurfaceColoringInvalidate.h"
#include "EventUserInterfaceUpdate.h"
#include "EventVolumeColoringInvalidate.h"
#include "GuiManager.h"
#include "Palette.h"
#include "PaletteCreateNewDialog.h"
#include "PaletteEditorRangeRow.h"
#include "PaletteEditorRangeWidget.h"
#include "PaletteFile.h"
#include "PaletteNew.h"
#include "PalettePixmapPainter.h"
#include "PaletteSelectionWidget.h"
#include "WuQColorEditorWidget.h"
#include "WuQDataEntryDialog.h"
#include "WuQMessageBoxTwo.h"
#include "WuQScrollArea.h"
#include "WorkbenchToolButton.h"
#include "WuQtUtilities.h"
#include "WuQTextEditorDialog.h"

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
    QObject::connect(m_colorEditButtonGroup, QOverload<QAbstractButton *>::of(&QButtonGroup::buttonClicked),
                     this, &PaletteEditorDialog::controlPointButtonClicked);
    
    QWidget* paletteBarWidget = createPaletteBarWidget();
    QWidget* paletteSelectionWidget = createPaletteSelectionWidget();
    QWidget* controlPointsWidget = createControlPointsWidget();
    
    m_colorEditorWidget = new WuQColorEditorWidget();
    m_colorEditorWidget->setFixedSize(m_colorEditorWidget->sizeHint());
    QObject::connect(m_colorEditorWidget, &WuQColorEditorWidget::colorChanged,
                     this, &PaletteEditorDialog::colorEditorColorChanged);
    m_colorEditorWidget->setContentsMargins(0, 0, 0, 0);
    
    QVBoxLayout* paletteLayout(new QVBoxLayout());
    paletteLayout->addWidget(paletteSelectionWidget);
    paletteLayout->addWidget(paletteBarWidget);
    
    QWidget* dialogWidget = new QWidget();
    QGridLayout* dialogLayout = new QGridLayout(dialogWidget);
    int row(dialogLayout->rowCount());
    dialogLayout->addLayout(paletteLayout, row, 0, 1, 2);
    ++row;
    dialogLayout->addWidget(WuQtUtilities::createHorizontalLineWidget(), row, 0, 1, 2);
    ++row;
    dialogLayout->addWidget(new QLabel("Control Points"), row, 0, Qt::AlignHCenter);
    dialogLayout->addWidget(new QLabel("Selected Control Point Color"), row, 1, Qt::AlignHCenter);
    ++row;
    dialogLayout->addWidget(controlPointsWidget, row, 0);
    dialogLayout->addWidget(m_colorEditorWidget, row, 1);

    setCentralWidget(dialogWidget,
                     ScrollAreaStatus::SCROLL_AREA_NEVER);
    
    /*
     * No resizing of dialog
     */
    setFixedSize(sizeHint());
    
    const bool demoModeFlag(false);
    if (demoModeFlag) {
        paletteSelectionWidget->setEnabled(false);
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    const bool updatePaletteListFlag(false);
    updateDialogInternal(updatePaletteListFlag);

    const PaletteBase* paletteBase = m_paletteSelectionWidget->getSelectedPalette();
    paletteSelected(paletteBase);
    
    m_helpPushbutton = addUserPushButton("Help",
                                         QDialogButtonBox::HelpRole);

    disableAutoDefaultForAllPushButtons();    
}

/**
 * Destructor.
 */
PaletteEditorDialog::~PaletteEditorDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *     The event that the receive can respond to.
 */
void
PaletteEditorDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        if ( ! m_ingoreUserInterfaceUpdateEventFlag) {
            bool updatePaletteListFlag(false);
            
            EventUserInterfaceUpdate* updateEvent(dynamic_cast<EventUserInterfaceUpdate*>(event));
            CaretAssert(updateEvent);
            updateEvent->setEventProcessed();
            
            FunctionResultValue<std::vector<const PaletteNew*>> result(EventPaletteNewOperation::getUserPalettes());
            if (result.isOk()) {
                std::vector<const PaletteNew*> palettes = result.getValue();
                if (palettes != m_previouslyLoadedPalettes) {
                    updatePaletteListFlag = true;
                }
            }
            else {
                updatePaletteListFlag = true;
            }
            
            if (updatePaletteListFlag) {
                updateDialogInternal(updatePaletteListFlag);
            }
        }
    }
}

/**
 * Called when the close button is clicked
 */
void
PaletteEditorDialog::closeEvent(QCloseEvent* event)
{
    if (isPaletteModified()) {
        const AString text("The selected palette is modified.\n"
                           "\n"
                           "Click OK to close the dialog and discard changes to the palette.\n"
                           "\n"
                           "Click Cancel to return to the " + windowTitle() + " dialog and resume editing the palette.");
        if ( ! WuQMessageBoxTwo::warningOkCancel(this, "Warning", text)) {
            event->ignore();
        }
    }
}

/**
 * Issue events after palettes changed
 */
void
PaletteEditorDialog::updateAfterPalettesChanged()
{
    m_ingoreUserInterfaceUpdateEventFlag = true;
    EventManager::get()->sendEvent(EventSurfaceColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventVolumeColoringInvalidate().getPointer());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
    EventPaletteNewOperation::sendPalettesChangedNotification();
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
    m_ingoreUserInterfaceUpdateEventFlag = false;
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
 * Update the palette list widget
 * @param forceUpdate
 *    Update even if palettes may not have changed
 */
void
PaletteEditorDialog::updatePaletteListWidget(const bool forceUpdate)
{
    m_paletteBeingEdited = NULL;
    m_paletteSelectionWidget->updateContent(forceUpdate);
    const PaletteBase* paletteBase(m_paletteSelectionWidget->getSelectedPalette());
    if (paletteBase != NULL) {
        m_paletteBeingEdited = paletteBase->castToPaletteNew();
    }
}

/**
 * Update the dialog.
 */
void
PaletteEditorDialog::updateDialog()
{
    const bool updatePaletteListFlag(false);
    updateDialogInternal(updatePaletteListFlag);
}

/**
 * Update the dialog.
 * @param updatePaletteListFlag
 *    Update list of palettes
 */
void
PaletteEditorDialog::updateDialogInternal(const bool updatePaletteListFlag)
{
    updatePaletteListWidget(updatePaletteListFlag);
    
    updatePaletteMovementButtons();
    
    controlPointButtonClicked(NULL);
    
    adjustSize();
}

/**
 * Load a copy of the palette into the dialog
 * @param palette
 *     Palette to load in editor
 */
void
PaletteEditorDialog::loadPaletteIntoEditor()
{
    std::vector<PaletteNew::ScalarColor> positiveScalars;
    std::vector<PaletteNew::ScalarColor> negativeScalars;
    std::vector<PaletteNew::ScalarColor> zeroScalars;

    if (m_paletteBeingEdited != NULL) {
        positiveScalars = m_paletteBeingEdited->getPosRange();
        negativeScalars = m_paletteBeingEdited->getNegRange();
        
        float zeroRgb[3];
        m_paletteBeingEdited->getZeroColor(zeroRgb);
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
 * Update the palette color bar image
 */
void
PaletteEditorDialog::updatePaletteColorBarImage()
{
    QPixmap colorBarPixmap;
    
    if (m_paletteBeingEdited != NULL) {
        PalettePixmapPainter palettePainter(m_paletteBeingEdited,
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
        m_paletteRevertAction->setEnabled(true);
        m_paletteSaveAction->setEnabled(true);
        m_paletteSaveToolButton->setStyleSheet("color : rgb(255, 0, 0);");
    }
    else {
        m_paletteRevertAction->setEnabled(false);
        m_paletteSaveAction->setEnabled(false);
        m_paletteSaveToolButton->setStyleSheet("");
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
    if (m_colorEditorWidget != NULL) {
        m_colorEditorWidget->setCurrentColor(QColor(rgb.red(),
                                                    rgb.green(),
                                                    rgb.blue()));
    }
    updatePaletteColorBarImage();
}

/**
 * @return New instance of the control point editing widgets
 */
QWidget*
PaletteEditorDialog::createControlPointsWidget()
{
    const int32_t spinBoxWidth(100);
    m_positiveRangeWidget = new PaletteEditorRangeWidget(PaletteEditorRangeWidget::DataRangeMode::POSITIVE,
                                                         m_colorEditButtonGroup,
                                                         spinBoxWidth,
                                                         this);
    QObject::connect(m_positiveRangeWidget, &PaletteEditorRangeWidget::signalEditColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QObject::connect(m_positiveRangeWidget, &PaletteEditorRangeWidget::signalDataChanged,
                     this, &PaletteEditorDialog::rangeWidgetDataChanged);
    QWidget* positiveWidget(new QWidget());
    QVBoxLayout* positiveLayout = new QVBoxLayout(positiveWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(positiveLayout, 0, 0);
    positiveLayout->addWidget(m_positiveRangeWidget);

    m_zeroRangeWidget = new PaletteEditorRangeWidget(PaletteEditorRangeWidget::DataRangeMode::ZERO,
                                                     m_colorEditButtonGroup,
                                                     spinBoxWidth,
                                                     this);
    QObject::connect(m_zeroRangeWidget, &PaletteEditorRangeWidget::signalEditColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QObject::connect(m_zeroRangeWidget, &PaletteEditorRangeWidget::signalDataChanged,
                     this, &PaletteEditorDialog::rangeWidgetDataChanged);

    QWidget* zeroWidget(new QWidget());
    QVBoxLayout* zeroLayout = new QVBoxLayout(zeroWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(zeroLayout, 0, 0);
    zeroLayout->addWidget(m_zeroRangeWidget);
    
    m_negativeRangeWidget = new PaletteEditorRangeWidget(PaletteEditorRangeWidget::DataRangeMode::NEGATIVE,
                                                         m_colorEditButtonGroup,
                                                         spinBoxWidth,
                                                         this);
    QObject::connect(m_negativeRangeWidget, &PaletteEditorRangeWidget::signalEditColorRequested,
                     this, &PaletteEditorDialog::editColor);
    QObject::connect(m_negativeRangeWidget, &PaletteEditorRangeWidget::signalDataChanged,
                     this, &PaletteEditorDialog::rangeWidgetDataChanged);
    
    QWidget* negativeWidget(new QWidget());
    QVBoxLayout* negativeLayout = new QVBoxLayout(negativeWidget);
    WuQtUtilities::setLayoutSpacingAndMargins(negativeLayout, 0, 0);
    negativeLayout->addWidget(m_negativeRangeWidget);

    QWidget* cpWidget = new QWidget();
    cpWidget->setSizePolicy(QSizePolicy::Fixed,
                            QSizePolicy::Fixed);
    QVBoxLayout* cpLayout = new QVBoxLayout(cpWidget);
    cpLayout->setSpacing(0);
    cpLayout->addWidget(positiveWidget);
    cpLayout->addWidget(zeroWidget);
    cpLayout->addWidget(negativeWidget);

    m_insertControlPointAboveAction = new QAction();
    m_insertControlPointAboveAction->setText("Insert Above");
    m_insertControlPointAboveAction->setToolTip("Insert a control point above\n"
                                                "the selected control point");
    QObject::connect(m_insertControlPointAboveAction, &QAction::triggered,
                     this, &PaletteEditorDialog::insertControlPointAboveActionTriggered);
    WorkbenchToolButton* insertControlPointAboveToolButton(new WorkbenchToolButton());
    insertControlPointAboveToolButton->setDefaultAction(m_insertControlPointAboveAction);
    
    m_insertControlPointBelowAction = new QAction();
    m_insertControlPointBelowAction->setText("Insert Below");
    m_insertControlPointBelowAction->setToolTip("Insert a control point below\n"
                                                "the selected control point");
    QObject::connect(m_insertControlPointBelowAction, &QAction::triggered,
                     this, &PaletteEditorDialog::insertControlPointBelowActionTriggered);
    WorkbenchToolButton* insertControlPointBelowToolButton(new WorkbenchToolButton());
    insertControlPointBelowToolButton->setDefaultAction(m_insertControlPointBelowAction);

    m_removeControlPointAction = new QAction();
    m_removeControlPointAction->setText("Remove");
    m_removeControlPointAction->setToolTip("Remove the selected control point");
    QObject::connect(m_removeControlPointAction, &QAction::triggered,
                     this, &PaletteEditorDialog::removeControlPointActionTriggered);
    WorkbenchToolButton* removeControlPointToolButton(new WorkbenchToolButton());
    removeControlPointToolButton->setDefaultAction(m_removeControlPointAction);

    QGridLayout* buttonsLayout(new QGridLayout());
    buttonsLayout->addWidget(insertControlPointAboveToolButton, 0, 0, Qt::AlignRight);
    buttonsLayout->addWidget(insertControlPointBelowToolButton, 0, 1, Qt::AlignLeft);
    buttonsLayout->addWidget(removeControlPointToolButton, 1, 0, 1, 2, Qt::AlignHCenter);
    
    WuQScrollArea* scrollArea = WuQScrollArea::newInstance(240, -1);
    scrollArea->setFrameShape(QFrame::NoFrame);
    scrollArea->setWidget(cpWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setSizeAdjustPolicy(QScrollArea::AdjustToContents);
    scrollArea->setSizePolicy(QSizePolicy::Fixed,
                              scrollArea->sizePolicy().verticalPolicy());
    QMargins scrollAreaMargins(scrollArea->contentsMargins());
    scrollAreaMargins.setTop(0);
    scrollArea->setContentsMargins(scrollAreaMargins);
    
    QWidget* widget(new QWidget());
    QVBoxLayout* layout(new QVBoxLayout(widget));
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(2);
    layout->addLayout(buttonsLayout);
    layout->addWidget(scrollArea, 100); /* 100 = stretch */
    
    return widget;
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
    
    m_paletteRevertAction = new QAction("Revert");
    m_paletteRevertAction->setToolTip("Revert (discard changes) to palette");
    QObject::connect(m_paletteRevertAction, &QAction::triggered,
                     this, &PaletteEditorDialog::revertPaletteActionTriggered);

    WorkbenchToolButton* paletteRevertToolButton = new WorkbenchToolButton();
    paletteRevertToolButton->setDefaultAction(m_paletteRevertAction);
    
    m_paletteSaveAction = new QAction("Save");
    m_paletteSaveAction->setToolTip("Save the changes to the current palette");
    QObject::connect(m_paletteSaveAction, &QAction::triggered,
                     this, &PaletteEditorDialog::savePaletteActionTriggered);
    
    m_paletteSaveToolButton = new WorkbenchToolButton();
    m_paletteSaveToolButton->setDefaultAction(m_paletteSaveAction);
    
    QWidget* widget = new QWidget();
    QGridLayout* layout = new QGridLayout(widget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(m_colorBarImageLabel, 0, 0);
    layout->addWidget(m_paletteSaveToolButton, 0, 1);
    layout->addWidget(paletteRevertToolButton, 0, 2);

    return widget;
}

/**
 * @return Create the widget for selection of palettes
 */
QWidget*
PaletteEditorDialog::createPaletteSelectionWidget()
{
    std::vector<PaletteDesignTypeEnum::Enum> paletteTypes;
    paletteTypes.push_back(PaletteDesignTypeEnum::PALETTE_NEW);
    m_paletteSelectionWidget = new PaletteSelectionWidget(PaletteSelectionWidget::WidgetType::LIST_WIDGET,
                                                          paletteTypes);
    QObject::connect(m_paletteSelectionWidget, &PaletteSelectionWidget::paletteSelected,
                     this, &PaletteEditorDialog::paletteSelected);
    
    m_newPaletteAction = new QAction();
    m_newPaletteAction->setText("New...");
    m_newPaletteAction->setToolTip("Launch dialog to create a new palette");
    QObject::connect(m_newPaletteAction, &QAction::triggered,
                     this, &PaletteEditorDialog::newPaletteActionTriggered);
    QToolButton* newPaletteToolButton(new QToolButton());
    newPaletteToolButton->setDefaultAction(m_newPaletteAction);
    
    m_renamePaletteAction = new QAction();
    m_renamePaletteAction->setText("Rename...");
    m_renamePaletteAction->setToolTip("Rename the selected palette");
    QObject::connect(m_renamePaletteAction, &QAction::triggered,
                     this, &PaletteEditorDialog::renamePaletteActionTriggered);
    QToolButton* renamePaletteToolButton(new QToolButton());
    renamePaletteToolButton->setDefaultAction(m_renamePaletteAction);
    
    m_deletePaletteAction = new QAction();
    m_deletePaletteAction->setText("Delete...");
    m_deletePaletteAction->setToolTip("Delete the selected palette");
    QObject::connect(m_deletePaletteAction, &QAction::triggered,
                     this, &PaletteEditorDialog::deletePaletteActionTriggered);
    QToolButton* deletePaletteToolButton(new QToolButton());
    deletePaletteToolButton->setDefaultAction(m_deletePaletteAction);

    m_importPaletteAction = new QAction();
    m_importPaletteAction->setText("Import...");
    m_importPaletteAction->setToolTip("Import palette from a file");
    QObject::connect(m_importPaletteAction, &QAction::triggered,
                     this, &PaletteEditorDialog::importPaletteActionTriggered);
    QToolButton* importPaletteToolButton(new QToolButton());
    importPaletteToolButton->setDefaultAction(m_importPaletteAction);
    
    m_exportPaletteAction = new QAction();
    m_exportPaletteAction->setText("Export...");
    m_exportPaletteAction->setToolTip("Export palette to a file");
    QObject::connect(m_exportPaletteAction, &QAction::triggered,
                     this, &PaletteEditorDialog::exportPaletteActionTriggered);
    QToolButton* exportPaletteToolButton(new QToolButton());
    exportPaletteToolButton->setDefaultAction(m_exportPaletteAction);
    
    QWidget* widget(new QWidget());
    QGridLayout* layout(new QGridLayout(widget));
    layout->setVerticalSpacing(3);
    layout->addWidget(m_paletteSelectionWidget, 0, 0, 3, 1);
    layout->addWidget(newPaletteToolButton, 0, 1);
    layout->addWidget(renamePaletteToolButton, 1, 1);
    layout->addWidget(deletePaletteToolButton, 2, 1);
    layout->addWidget(importPaletteToolButton, 0, 2);
    layout->addWidget(exportPaletteToolButton, 1, 2);
    
    return widget;
}

/**
 * Called when new palette is selected
 */
void
PaletteEditorDialog::newPaletteActionTriggered()
{
    if ( ! modifiedPaletteWarningDialog()) {
        return;
    }
    
    PaletteCreateNewDialog dialog(PaletteCreateNewDialog::PaletteType::USER_CUSTOM_PALETTE,
                                  m_pixmapMode,
                                  this);
    if (dialog.exec() == PaletteCreateNewDialog::Accepted) {
        const PaletteNew* palette(dialog.getPalette());
        m_paletteBeingEdited = NULL;
        if (palette != NULL) {
            m_paletteBeingEdited = palette;
            m_paletteSelectionWidget->updateContent();
            m_paletteSelectionWidget->selectPalette(palette);
        }
        else {
            WuQMessageBoxTwo::critical(this, "ERROR", dialog.getErrorMessage());
        }

        const bool updatePaletteListFlag(true);
        updateDialogInternal(updatePaletteListFlag);
        
        const PaletteBase* paletteBase = m_paletteSelectionWidget->getSelectedPalette();
        paletteSelected(paletteBase);
        updateAfterPalettesChanged();
        updateModifiedLabel();
    }
}

/**
 * Called when rename  palette is selected
 */
void
PaletteEditorDialog::renamePaletteActionTriggered()
{
    if (m_paletteBeingEdited != NULL) {
        const AString paletteName(m_paletteBeingEdited->getName());
        const QString newName(QInputDialog::getText(this,
                                                    "Rename Palette",
                                                    "New Name for Palette",
                                                    QLineEdit::Normal,
                                                    paletteName));
        if ( ! newName.isEmpty()) {
            if (newName != paletteName) {
                const PaletteBase* paletteBase = m_paletteSelectionWidget->getSelectedPalette();
                FunctionResult result(EventPaletteNewOperation::renamePalette(m_paletteBeingEdited,
                                                                           newName));
                if (result.isError()) {
                    WuQMessageBoxTwo::critical(this, "ERROR", result.getErrorMessage());
                }

                const bool updatePaletteListFlag(true);
                updateDialogInternal(updatePaletteListFlag);

                m_paletteSelectionWidget->selectPalette(paletteBase);
                paletteSelected(paletteBase);
                updateAfterPalettesChanged();
            }
        }
    }
}

/**
 * Called when delete palette is selected
 */
void
PaletteEditorDialog::deletePaletteActionTriggered()
{
    if (m_paletteBeingEdited != NULL) {
        const AString message("Delete palette \""
                              + m_paletteBeingEdited->getName()
                              + "\" ?");
        if (WuQMessageBoxTwo::warningOkCancel(this, "Confirm", message)) {
            FunctionResult result(EventPaletteNewOperation::deletePalette(m_paletteBeingEdited));
            if (result.isError()) {
                WuQMessageBoxTwo::critical(this, "ERROR", result.getErrorMessage());
            }
            const bool updatePaletteListFlag(true);
            updateDialogInternal(updatePaletteListFlag);

            const PaletteBase* paletteBase = m_paletteSelectionWidget->getSelectedPalette();
            paletteSelected(paletteBase);
            updateAfterPalettesChanged();
        }
    }
}

/**
 * Called when the revert action is triggered
 */
void
PaletteEditorDialog::revertPaletteActionTriggered()
{
    loadPaletteIntoEditor();
}

/**
 * Called when the save palette action is triggered
 */
void
PaletteEditorDialog::savePaletteActionTriggered()
{
    if (m_paletteBeingEdited != NULL) {
        std::vector<PaletteNew::ScalarColor> zeroScalars(m_zeroRangeWidget->getScalarColors());
        if (zeroScalars.size() == 1) {
            CaretAssertVectorIndex(zeroScalars, 0);
            FunctionResult result(EventPaletteNewOperation::updatePalette(m_paletteBeingEdited,
                                                                       m_positiveRangeWidget->getScalarColors(),
                                                                       m_negativeRangeWidget->getScalarColors(),
                                                                       zeroScalars[0]));
            if (result.isError()) {
                WuQMessageBoxTwo::critical(this, "ERROR", result.getErrorMessage());
            }
            loadPaletteIntoEditor();
            updateAfterPalettesChanged();
        }
    }
}

/**
 * Called when a  palette is selected
 * @param item
 *    List widget item containing palette that was selected
 */
void
PaletteEditorDialog::paletteSelected(const PaletteBase* paletteBase)
{
    m_paletteBeingEdited = NULL;
    if (paletteBase != NULL) {
        m_paletteBeingEdited = paletteBase->castToPaletteNew();
    }
    
    loadPaletteIntoEditor();
    
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
        const QString info("The palette in the editor is modified.\n\n"
                           "Click DISCARD to discard the changes to the modified palette and continue adding/loading the new palette.\n\n"
                           "Click CANCEL to cancel this operation and return to the editor to save the modified palette.");
        WuQMessageBoxTwo messageBox(WuQMessageBoxTwo::IconType::Warning,
                                    "WARNING: Palette is Modified",
                                    info,
                                    WuQMessageBoxTwo::createButtonMask(WuQMessageBoxTwo::StandardButton::Ok,
                                                                       WuQMessageBoxTwo::StandardButton::Cancel),
                                    this);
        messageBox.setButtonText(WuQMessageBoxTwo::StandardButton::Ok, "Discard");
        messageBox.exec();
        if (messageBox.clickedStandardButton() == WuQMessageBoxTwo::StandardButton::Ok) {
            return true;   /* discard changes */
        }
        else {
            return false;  /* cancel operaion*/
        }
    }
    
    return true;
}

/**
 * Update the palette movement buttons
 */
void
PaletteEditorDialog::updatePaletteMovementButtons()
{
}

/**
 * Called when insert control point above button clicked
 */
void
PaletteEditorDialog::insertControlPointAboveActionTriggered()
{
    std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
    items = getSelectedControlPointInfo();
    
    PaletteEditorRangeWidget* rangeWidget(std::get<0>(items));
    if (rangeWidget != NULL) {
        const float newScalar(rangeWidget->performControlPointOperation(std::get<1>(items),
                                                                        PaletteEditorRangeWidget::ConstructionOperation::INSERT_CONTROL_POINT_ABOVE));
        std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
        newScalarItems(getControlPointWithScalar(newScalar));
        PaletteEditorRangeRow* rangeRow(std::get<1>(newScalarItems));
        QRadioButton* radioButton(std::get<2>(newScalarItems));
        if ((rangeRow != NULL)
            && (radioButton != NULL)) {
            radioButton->setChecked(true);
            editColor(rangeRow->getRgb());
        }
    }
    
    controlPointButtonClicked(NULL);
}

/**
 * Called when insert control point below button clicked
 */
void
PaletteEditorDialog::insertControlPointBelowActionTriggered()
{
    std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
    items = getSelectedControlPointInfo();
    
    PaletteEditorRangeWidget* rangeWidget(std::get<0>(items));
    if (rangeWidget != NULL) {
        const float newScalar(rangeWidget->performControlPointOperation(std::get<1>(items),
                                                                        PaletteEditorRangeWidget::ConstructionOperation::INSERT_CONTROL_POINT_BELOW));
        
        std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
        newScalarItems(getControlPointWithScalar(newScalar));
        PaletteEditorRangeRow* rangeRow(std::get<1>(newScalarItems));
        QRadioButton* radioButton(std::get<2>(newScalarItems));
        if ((rangeRow != NULL)
            && (radioButton != NULL)) {
            radioButton->setChecked(true);
            editColor(rangeRow->getRgb());
        }
    }

    controlPointButtonClicked(NULL);
}

/**
 * Called when remove control point button clicked
 */
void
PaletteEditorDialog::removeControlPointActionTriggered()
{
    std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
    items = getSelectedControlPointInfo();
    
    PaletteEditorRangeWidget* rangeWidget(std::get<0>(items));
    if (rangeWidget != NULL) {
        rangeWidget->performControlPointOperation(std::get<1>(items),
                                                  PaletteEditorRangeWidget::ConstructionOperation::REMOVE_CONTROL_POINT);
        std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
        items = getSelectedControlPointInfo();
        PaletteEditorRangeRow* rangeRow(std::get<1>(items));
        if (rangeRow != NULL) {
            editColor(rangeRow->getRgb());
        }
    }
    
    controlPointButtonClicked(NULL);
}

/**
 * @return Selected radio button and range row containing the button
 */
std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
PaletteEditorDialog::getSelectedControlPointInfo()
{
    PaletteEditorRangeWidget* rangeWidget(NULL);
    PaletteEditorRangeRow* rangeRow(NULL);
    QRadioButton* radioButton(NULL);
    
    QAbstractButton* button(m_colorEditButtonGroup->checkedButton());
    if (button != NULL) {
        rangeRow = m_negativeRangeWidget->getRangeRowFromButton(button);
        if (rangeRow != NULL) {
            rangeWidget = m_negativeRangeWidget;
        }
        if (rangeRow == NULL) {
            rangeRow = m_zeroRangeWidget->getRangeRowFromButton(button);
            if (rangeRow != NULL) {
                rangeWidget = m_zeroRangeWidget;
            }
        }
        if (rangeRow == NULL) {
            rangeRow = m_positiveRangeWidget->getRangeRowFromButton(button);
            if (rangeRow != NULL) {
                rangeWidget = m_positiveRangeWidget;
            }
        }
        
        if ((rangeWidget != NULL)
            && (rangeRow != NULL)) {
            radioButton = dynamic_cast<QRadioButton*>(button);
            CaretAssert(radioButton);
        }
    }
    
    return std::make_tuple(rangeWidget, rangeRow, radioButton);
}

/**
 * @return Info on range row containing the given scalar
 * @param scalar
 *    The scalar value
 */
std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
PaletteEditorDialog::getControlPointWithScalar(const float scalar)
{
    PaletteEditorRangeWidget* rangeWidget(NULL);
    PaletteEditorRangeRow* rangeRow(NULL);
    QRadioButton* radioButton(NULL);
    
    rangeRow = m_negativeRangeWidget->getRangeRowFromScalar(scalar);
    if (rangeRow != NULL) {
        rangeWidget = m_negativeRangeWidget;
    }
    if (rangeRow == NULL) {
        rangeRow = m_zeroRangeWidget->getRangeRowFromScalar(scalar);
        if (rangeRow != NULL) {
            rangeWidget = m_zeroRangeWidget;
        }
    }
    if (rangeRow == NULL) {
        rangeRow = m_positiveRangeWidget->getRangeRowFromScalar(scalar);
        if (rangeRow != NULL) {
            rangeWidget = m_positiveRangeWidget;
        }
    }
    
    if ((rangeWidget != NULL)
        && (rangeRow != NULL)) {
        radioButton = rangeRow->m_colorEditRadioButton;
        CaretAssert(radioButton);
    }
    
    return std::make_tuple(rangeWidget, rangeRow, radioButton);
}

/**
 * Called when a control point radio button is clicked
 * @param button
 *    Button that was clicked
 */
void
PaletteEditorDialog::controlPointButtonClicked(QAbstractButton* /*button*/)
{
    std::tuple<PaletteEditorRangeWidget*, PaletteEditorRangeRow*, QRadioButton*>
    items = getSelectedControlPointInfo();
    
    PaletteEditorRangeRow* row(std::get<1>(items));
    if (row != NULL) {
        m_insertControlPointAboveAction->setEnabled(row->m_insertAboveValidFlag);
        m_insertControlPointBelowAction->setEnabled(row->m_insertBelowValidFlag);
        m_removeControlPointAction->setEnabled(row->m_removeValidFlag);
    }
}

/**
 * Called when export palette action is triggered
 */
void
PaletteEditorDialog::exportPaletteActionTriggered()
{
    if (m_paletteBeingEdited != NULL) {
        AString defaultFileName;
        const std::vector<AString> extensions(DataFileTypeEnum::getAllFileExtensionsForWriting(DataFileTypeEnum::PALETTE));
        if ( ! extensions.empty()) {
            defaultFileName = (m_paletteBeingEdited->getName().replace(' ', '_')
                               + "."
                               + extensions.front());
        }
            
        const AString filename(CaretFileDialog::getSaveFileNameDialog(DataFileTypeEnum::PALETTE,
                                                                      defaultFileName,
                                                                      this,
                                                                      "Choose Palette File Name"));
        if ( ! filename.isEmpty()) {
            FunctionResult result(m_paletteBeingEdited->writeToFile(filename));
            if (result.isError()) {
                WuQMessageBoxTwo::critical(this,
                                           "ERROR",
                                           result.getErrorMessage());
            }
        }
    }
}

/**
 * Called when import palette action is triggered
 */
void
PaletteEditorDialog::importPaletteActionTriggered()
{
    const AString filename(CaretFileDialog::getOpenFileNameDialog(DataFileTypeEnum::PALETTE,
                                                                  this,
                                                                  "Choose Palette File Name"));
    if ( ! filename.isEmpty()) {
        FunctionResultValue<const PaletteNew*> result(EventPaletteNewOperation::readPalette(filename));
        if (result.isOk()) {
            const PaletteNew* palette(result.getValue());
            CaretAssert(palette);
            m_paletteBeingEdited = palette;
            m_paletteSelectionWidget->updateContent();
            m_paletteSelectionWidget->selectPalette(palette);

            const bool updatePaletteListFlag(true);
            updateDialogInternal(updatePaletteListFlag);

            const PaletteBase* paletteBase = m_paletteSelectionWidget->getSelectedPalette();
            paletteSelected(paletteBase);
            updateAfterPalettesChanged();
            updateModifiedLabel();
        }
        else {
            WuQMessageBoxTwo::critical(this, "ERROR", result.getErrorMessage());
        }
    }
}

WuQDialog::DialogUserButtonResult
PaletteEditorDialog::userButtonPressed(QPushButton* userPushButton)
{
    if (userPushButton == m_helpPushbutton) {
        WuQTextEditorDialog::runNonModal("Palette Editor Documentation",
                                         getDocumentationHtml(),
                                         WuQTextEditorDialog::TextMode::HTML,
                                         WuQTextEditorDialog::WrapMode::YES,
                                         this);
    }
    else {
        CaretAssert(0);
    }
    return WuQDialog::RESULT_NONE;
}


/**
 * @return the help information in a QString
 */
QString
PaletteEditorDialog::getDocumentationHtml() const
{
    const std::string text = R"(
<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Palette Editor &mdash; User Documentation</title>
<style type="text/css">
  body {
    font-family: Arial, Helvetica, sans-serif;
    font-size: 13px;
    color: #222222;
    background-color: #ffffff;
    margin: 16px 24px;
  }

  h1 {
    font-size: 22px;
    font-weight: bold;
    color: #1a1a2e;
    border-bottom: 2px solid #5a2d8a;
    padding-bottom: 6px;
    margin-top: 0;
    margin-bottom: 4px;
  }

  h2 {
    font-size: 16px;
    font-weight: bold;
    color: #3b1f6e;
    background-color: #f0eafa;
    padding: 5px 10px;
    margin-top: 24px;
    margin-bottom: 8px;
    border-left: 4px solid #7c3aed;
  }

  h3 {
    font-size: 13px;
    font-weight: bold;
    color: #4b2080;
    margin-top: 14px;
    margin-bottom: 6px;
  }

  p {
    margin-top: 4px;
    margin-bottom: 8px;
    line-height: 1.6;
  }

  ul {
    margin-top: 4px;
    margin-bottom: 8px;
    padding-left: 20px;
  }

  ol {
    margin-top: 4px;
    margin-bottom: 8px;
    padding-left: 20px;
  }

  li {
    margin-bottom: 4px;
    line-height: 1.6;
  }

  table {
    border-collapse: collapse;
    width: 100%;
    margin-top: 6px;
    margin-bottom: 12px;
    font-size: 12px;
  }

  th {
    background-color: #e8e0f5;
    color: #2d1a5e;
    font-weight: bold;
    text-align: left;
    padding: 6px 10px;
    border: 1px solid #c0aee0;
  }

  td {
    padding: 5px 10px;
    border: 1px solid #d8d0ec;
    vertical-align: top;
    color: #333333;
  }

  tr.alt td {
    background-color: #f7f4fd;
  }

  .tip {
    background-color: #eef3ff;
    border-left: 4px solid #3b5bdb;
    padding: 8px 12px;
    margin: 10px 0;
    font-size: 12px;
    color: #1e3a8a;
  }

  .note {
    background-color: #edfaf4;
    border-left: 4px solid #2f9e6a;
    padding: 8px 12px;
    margin: 10px 0;
    font-size: 12px;
    color: #1a5c3a;
  }

  .section-num {
    color: #9d77cc;
    font-size: 11px;
    font-weight: bold;
  }

  .toc {
    background-color: #f8f5ff;
    border: 1px solid #d0bff0;
    padding: 10px 16px;
    margin-bottom: 20px;
    font-size: 12px;
  }

  .toc-title {
    font-weight: bold;
    color: #3b1f6e;
    margin-bottom: 6px;
  }

  .toc a {
    color: #5a2d8a;
  }

  hr {
    border: none;
    border-top: 1px solid #ddd;
    margin: 20px 0;
  }
</style>
</head>
<body>

<h1>Palette Editor &mdash; User Documentation</h1>

<div class="toc">
  <div class="toc-title">Contents</div>
  <ol>
    <li><a href="#overview">Overview</a></li>
    <li><a href="#palette-list">Palette List</a></li>
    <li><a href="#gradient-preview">Gradient Preview Bar</a></li>
    <li><a href="#control-points">Control Points Panel</a></li>
    <li><a href="#color-panel">Selected Control Point Color Panel</a></li>
    <li><a href="#sliders">Color Value Sliders</a></li>
    <li><a href="#closing">Closing the Dialog</a></li>
    <li><a href="#workflow">Typical Workflow</a></li>
  </ol>
</div>

<!-- 1. Overview -->
<a name="overview"></a>
<h2><span class="section-num">01 &nbsp;</span>Overview</h2>
<p>
  The <b>Palette Editor</b> lets you create, manage, and fine-tune color palettes.
  Each palette is a gradient defined by a set of <b>control points</b>, where each
  point maps a scalar value to a specific color. The gradient is interpolated
  between control points to produce a smooth color ramp.  Custom palettes are
  stored in the user's preferences.
</p>

<hr>

<!-- 2. Palette List -->
<a name="palette-list"></a>
<h2><span class="section-num">02 &nbsp;</span>Palette List</h2>
<p>
  The panel at the top of the dialog displays all available palettes by name,
  each previewed with its color gradient. Click any row to select that palette
  for editing.
</p>

<h3>Palette Management Buttons</h3>
<table>
  <tr>
    <th>Button</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><b>New</b></td>
    <td>Creates a new palette that has a specified number of control points or imports a standard palette. 
        Note that (1) an imported standard palette may be modified to meet the editable palette criteria;
        and (2) the <i>none</i> color is not supported.</td>
  </tr>
  <tr class="alt">
    <td><b>Import</b></td>
    <td>Loads a palette from an external file.</td>
  </tr>
  <tr>
    <td><b>Rename</b></td>
    <td>Renames the currently selected palette.</td>
  </tr>
  <tr class="alt">
    <td><b>Export</b></td>
    <td>Saves the currently selected palette to a file.</td>
  </tr>
  <tr>
    <td><b>Delete</b></td>
    <td>Permanently removes the currently selected palette from the user's preferences.</td>
  </tr>
</table>

<hr>

<!-- 3. Gradient Preview Bar -->
<a name="gradient-preview"></a>
<h2><span class="section-num">03 &nbsp;</span>Gradient Preview Bar</h2>
<p>
  Directly below the palette list, a wide bar displays a live preview of the
  selected palette's full gradient, split into three segments:
</p>
<ul>
  <li><b>Left segment</b> &mdash; Shows the positive range of the gradient.</li>
  <li><b>Right segment</b> &mdash; Shows the negative range of the gradient.</li>
  <li><b>Zero segment</b> &mdash; Shows the zero color as a narrow rectangle.</li>
</ul>
<p>
  Small tick marks along the bottom of the bar indicate the positions of
  existing control points.
</p>

<h3>Save / Revert</h3>
<ul>
  <li><b>Save</b> &mdash; Commits all unsaved edits to the palette.</li>
  <li><b>Revert</b> &mdash; Discards all unsaved edits and restores the last saved state.</li>
</ul>

<hr>

<!-- 4. Control Points -->
<a name="control-points"></a>
<h2><span class="section-num">04 &nbsp;</span>Control Points Panel</h2>
<p>
  Control points define the colors at specific positions along the gradient.
  The list is divided into two sections by a <b>Zero</b> marker:
</p>
<ul>
  <li><b>Positive values</b> (above Zero) &mdash; listed from 1.000 down to 0.0.</li>
  <li><b>Negative values</b> (below Zero) &mdash; listed from 0.000 down to -1.0.</li>
</ul>
<p>Each row shows:</p>
<ul>
  <li>A <b>radio button</b> to select that control point for editing its color.</li>
  <li>The <b>scalar value</b> (a number along the gradient axis).</li>
  <li>A <b>color swatch</b> showing the color assigned to that point.</li>
  <li><b>Up/Down arrows</b> to adjust the scalar value.</li>
</ul>

<h3>Control Point Buttons</h3>
<table>
  <tr>
    <th>Button</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><b>Insert Above</b></td>
    <td>Adds a new control point just above the currently selected one.</td>
  </tr>
  <tr class="alt">
    <td><b>Insert Below</b></td>
    <td>Adds a new control point just below the currently selected one.</td>
  </tr>
  <tr>
    <td><b>Remove</b></td>
    <td>Deletes the currently selected control point. </td>
  </tr>
</table>

<div class="tip">
  <b>Tip:</b> Select a control point by clicking its radio button. The
  <b>Selected Control Point Color</b> panel on the right will update immediately
  so you can edit that point's color.
</div>

<hr>

<!-- 5. Color Panel -->
<a name="color-panel"></a>
<h2><span class="section-num">05 &nbsp;</span>Selected Control Point Color Panel</h2>
<p>
  When a control point is selected, this panel lets you adjust its color
  using multiple methods.
</p>

<h3>Color Square</h3>
<p>
  The large square displays the full saturation/brightness space for the
  current hue. Click or drag within it to pick a color:
</p>
<ul>
  <li><b>Horizontal axis</b> &mdash; Saturation (left = grey/white, right = fully saturated).</li>
  <li><b>Vertical axis</b> &mdash; Brightness (top = bright, bottom = dark).</li>
</ul>

<h3>Hue Slider (Vertical Bar)</h3>
<p>
  The narrow vertical bar to the right of the color square is the <b>hue slider</b>.
  The triangle marker (&#9658;) shows the current hue. Click in the vertical bar 
  (not the arrow) to change the hue.
</p>

<h3>Preset Color Swatches</h3>
<p>
  A grid of preset color swatches sits to the right of the hue slider.
  Click any swatch to instantly apply that color to the selected control point.
</p>

<h3>Color / Original / Revert</h3>
<ul>
  <li><b>Color swatch</b> &mdash; Shows the current (edited) color.</li>
  <li><b>Original swatch</b> &mdash; Shows the color before the current editing session began.</li>
  <li><b>Revert</b> &mdash; Resets the selected control point's color back to the original.</li>
</ul>

<hr>

<!-- 6. Sliders -->
<a name="sliders"></a>
<h2><span class="section-num">06 &nbsp;</span>Color Value Sliders</h2>
<p>
  Fine-tune the color numerically using the sliders. Values can also be typed
  directly or incremented with the numeric spin boxes to the right of each slider.
</p>

<h3>RGB Controls</h3>
<table>
  <tr>
    <th>Slider</th>
    <th>Range</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><b>Red</b></td>
    <td>0 &ndash; 255</td>
    <td>Red channel intensity.</td>
  </tr>
  <tr class="alt">
    <td><b>Green</b></td>
    <td>0 &ndash; 255</td>
    <td>Green channel intensity.</td>
  </tr>
  <tr>
    <td><b>Blue</b></td>
    <td>0 &ndash; 255</td>
    <td>Blue channel intensity.</td>
  </tr>
</table>

<h3>HSV Controls</h3>
<table>
  <tr>
    <th>Slider</th>
    <th>Range</th>
    <th>Description</th>
  </tr>
  <tr>
    <td><b>Hue</b></td>
    <td>0 &ndash; 360</td>
    <td>Color hue in degrees (0 = red, 60 = yellow, 120 = green, 240 = blue).</td>
  </tr>
  <tr class="alt">
    <td><b>Sat</b></td>
    <td>0 &ndash; 255</td>
    <td>Saturation (0 = grey, 255 = fully saturated).</td>
  </tr>
  <tr>
    <td><b>Value</b></td>
    <td>0 &ndash; 255</td>
    <td>Brightness (0 = black, 255 = full brightness).</td>
  </tr>
</table>

<div class="note">
  <b>Note:</b> RGB and HSV controls are linked &mdash; editing one set
  automatically updates the other in real time.
</div>

<hr>

<!-- 7. Closing -->
<a name="closing"></a>
<h2><span class="section-num">07 &nbsp;</span>Closing the Dialog</h2>
<ul>
  <li>Click <b>Save</b> before closing if you want to keep your changes.</li>
  <li>Click <b>Close</b> (bottom-right) to dismiss the dialog.  If there are
      unsaved changes, a dialog warns the user.</li>
</ul>

<hr>

<!-- 8. Workflow -->
<a name="workflow"></a>
<h2><span class="section-num">08 &nbsp;</span>Typical Workflow</h2>
<ol>
  <li>Select a palette from the list, or click <b>New</b> to create a new palette.</li>
  <li>Review the gradient in the <b>preview bar</b>.</li>
  <li>Select a control point from the <b>Control Points</b> list by clicking its radio button.</li>
  <li>Adjust its color using the <b>color square</b>, <b>hue slider</b>, <b>preset swatches</b>, or <b>RGB/HSV sliders</b>.</li>
  <li>Add or remove control points with <b>Insert Above</b>, <b>Insert Below</b>, or <b>Remove</b> as needed.</li>
  <li>Click <b>Save</b> to commit your changes.</li>
  <li>Click <b>Export</b> if you want to share or back up the palette.</li>
</ol>

</body>
</html>
)";
    
    
    return QString::fromStdString(text);
}



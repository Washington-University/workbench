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
    QObject::connect(m_colorEditButtonGroup, &QButtonGroup::buttonClicked,
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
    
    disableAutoDefaultForAllPushButtons();
    
    const bool demoModeFlag(false);
    if (demoModeFlag) {
        paletteSelectionWidget->setEnabled(false);
    }
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    
    updateDialog();
    const PaletteBase* paletteBase = m_paletteSelectionWidget->getSelectedPalette();
    paletteSelected(paletteBase);
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
            bool updateDialogFlag(false);
            
            EventUserInterfaceUpdate* updateEvent(dynamic_cast<EventUserInterfaceUpdate*>(event));
            CaretAssert(updateEvent);
            updateEvent->setEventProcessed();
            
            FunctionResultValue<std::vector<const PaletteNew*>> result(EventPaletteNewOperation::getUserPalettes());
            if (result.isOk()) {
                std::vector<const PaletteNew*> palettes = result.getValue();
                if (palettes != m_previouslyLoadedPalettes) {
                    updateDialogFlag = true;
                }
            }
            else {
                updateDialogFlag = true;
            }
            
            if (updateDialogFlag) {
                updateDialog();
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
 */
void
PaletteEditorDialog::updatePaletteListWidget()
{
    m_paletteBeingEdited = NULL;
    m_paletteSelectionWidget->updateContent();
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
    updatePaletteListWidget();
    
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

    QWidget* widget(new QWidget());
    QGridLayout* layout(new QGridLayout(widget));
    layout->addWidget(m_paletteSelectionWidget, 0, 0, 3, 1);
    layout->addWidget(newPaletteToolButton, 0, 1);
    layout->addWidget(renamePaletteToolButton, 1, 1);
    layout->addWidget(deletePaletteToolButton, 2, 1);
    
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
        updateDialog();
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
                updateDialog();
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
            updateDialog();
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

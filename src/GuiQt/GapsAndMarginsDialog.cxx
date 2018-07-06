
/*LICENSE_START*/
/*
 *  Copyright (C) 2015 Washington University School of Medicine
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

#define __GAPS_AND_MARGINS_DIALOG_DECLARE__
#include "GapsAndMarginsDialog.h"
#undef __GAPS_AND_MARGINS_DIALOG_DECLARE__

#include <QDoubleSpinBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>
#include <QSignalMapper>
#include <QToolButton>

#include "Brain.h"
#include "BrainBrowserWindow.h"
#include "BrainBrowserWindowComboBox.h"
#include "BrainConstants.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "CaretLogger.h"
#include "EventBrowserTabGet.h"
#include "EventBrowserTabGetAll.h"
#include "EventGetViewportSize.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "GapsAndMargins.h"
#include "GuiManager.h"
#include "WuQGridLayoutGroup.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::GapsAndMarginsDialog 
 * \brief Dialog for adjustment of tab margins.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
GapsAndMarginsDialog::GapsAndMarginsDialog(QWidget* parent)
: WuQDialogNonModal("Gaps and Margins",
                    parent)
{
    setApplyButtonText("");
 
    m_browserWindowComboBox = new BrainBrowserWindowComboBox(BrainBrowserWindowComboBox::STYLE_NAME_AND_NUMBER,
                                                             this);
    QObject::connect(m_browserWindowComboBox, SIGNAL(browserWindowIndexSelected(const int32_t)),
                     this, SLOT(browserWindowIndexChanged(const int32_t)));
    
    QWidget* gapsWidget = createGapsWidget();
    
    QWidget* marginsWidget = createMarginsWidget();
    
    QWidget* dialogWidget = new QWidget;
    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogWidget);
    dialogLayout->addWidget(m_browserWindowComboBox->getWidget(), 0, Qt::AlignHCenter);
    //dialogLayout->addWidget(WuQtUtilities::createHorizontalLineWidget());
    dialogLayout->addSpacing(8);
    dialogLayout->addWidget(gapsWidget, 0, Qt::AlignHCenter);
    dialogLayout->addWidget(marginsWidget);
    
    setCentralWidget(dialogWidget,
                     SCROLL_AREA_AS_NEEDED);

    updateDialog();
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
}

/**
 * Destructor.
 */
GapsAndMarginsDialog::~GapsAndMarginsDialog()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * @return Create the gaps widget.
 */
QWidget*
GapsAndMarginsDialog::createGapsWidget()
{
    QLabel* surfaceLabel    = new QLabel("Surface");
    QLabel* volumeLabel     = new QLabel("Volume");
    QLabel* horizontalLabel = new QLabel("Horizontal");
    QLabel* verticalLabel   = new QLabel("Vertical");
    
    m_surfaceMontageHorizontalGapSpinBox = createPercentageSpinBox();
    QObject::connect(m_surfaceMontageHorizontalGapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceMontageGapChanged()));
    
    m_surfaceMontageVerticalGapSpinBox = createPercentageSpinBox();
    QObject::connect(m_surfaceMontageVerticalGapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(surfaceMontageGapChanged()));
    
    m_surfaceMontageMatchPixelToolButton = new QToolButton;
    m_surfaceMontageMatchPixelToolButton->setText("Match Pixel Width\nto Pixel Height");
    m_surfaceMontageMatchPixelToolButton->setToolTip("Set the width to same pixel size as height");
    QObject::connect(m_surfaceMontageMatchPixelToolButton, SIGNAL(clicked(bool)),
                     this, SLOT(surfaceMontageMatchPixelButtonClicked()));
    
    
    m_volumeMontageHorizontalGapSpinBox = createPercentageSpinBox();
    QObject::connect(m_volumeMontageHorizontalGapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeMontageGapChanged()));
    
    m_volumeMontageVerticalGapSpinBox = createPercentageSpinBox();
    QObject::connect(m_volumeMontageVerticalGapSpinBox, SIGNAL(valueChanged(double)),
                     this, SLOT(volumeMontageGapChanged()));
    
    m_volumeMontageMatchPixelToolButton = new QToolButton;
    m_volumeMontageMatchPixelToolButton->setText("Match Pixel Width\nto Pixel Height");
    m_volumeMontageMatchPixelToolButton->setToolTip("Set the width to same pixel size as height");
    QObject::connect(m_volumeMontageMatchPixelToolButton, SIGNAL(clicked(bool)),
                     this, SLOT(volumeMontageMatchPixelButtonClicked()));
    
    const int COLUMN_LABEL      = 0;
    const int COLUMN_HORIZONTAL = 1;
    const int COLUMN_VERTICAL   = 2;
    const int COLUMN_SCALE      = 3;
    QWidget* widget = new QGroupBox("Montage Gaps");
    QGridLayout* gridLayout = new QGridLayout(widget);
    int row = gridLayout->rowCount();
    gridLayout->addWidget(horizontalLabel,
                          row, COLUMN_HORIZONTAL, Qt::AlignHCenter);
    gridLayout->addWidget(verticalLabel,
                          row, COLUMN_VERTICAL, Qt::AlignHCenter);
    row++;
    gridLayout->addWidget(surfaceLabel,
                          row, COLUMN_LABEL);
    gridLayout->addWidget(m_surfaceMontageHorizontalGapSpinBox,
                          row, COLUMN_HORIZONTAL);
    gridLayout->addWidget(m_surfaceMontageVerticalGapSpinBox,
                          row, COLUMN_VERTICAL);
    gridLayout->addWidget(m_surfaceMontageMatchPixelToolButton,
                          row, COLUMN_SCALE, Qt::AlignHCenter);
    row++;
    gridLayout->addWidget(volumeLabel,
                          row, COLUMN_LABEL);
    gridLayout->addWidget(m_volumeMontageHorizontalGapSpinBox,
                          row, COLUMN_HORIZONTAL);
    gridLayout->addWidget(m_volumeMontageVerticalGapSpinBox,
                          row, COLUMN_VERTICAL);
    gridLayout->addWidget(m_volumeMontageMatchPixelToolButton,
                          row, COLUMN_SCALE, Qt::AlignHCenter);
    row++;
    
    widget->setSizePolicy(QSizePolicy::Fixed,
                          QSizePolicy::Fixed);
    
    return widget;
}

/**
 * @return Create the margins widget.
 */
QWidget*
GapsAndMarginsDialog::createMarginsWidget()
{
    m_tabIndexSignalMapper = new QSignalMapper(this);
    QObject::connect(m_tabIndexSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(tabMarginChanged(int)));
    
    m_tabMarginMatchPixelsToolButtonSignalMapper = new QSignalMapper(this);
    QObject::connect(m_tabMarginMatchPixelsToolButtonSignalMapper, SIGNAL(mapped(int)),
                     this, SLOT(tabMarginMatchPixelButtonClicked(int)));
    
    QLabel* tabLabel    = new QLabel("Tab");
    QLabel* leftLabel   = new QLabel("Left");
    QLabel* rightLabel  = new QLabel("Right");
    QLabel* topLabel    = new QLabel("Top");
    QLabel* bottomLabel = new QLabel("Bottom");
    
    m_applyFirstTabToAllToolButton = new QToolButton();
    m_applyFirstTabToAllToolButton->setText("Apply First Row to All");
    QObject::connect(m_applyFirstTabToAllToolButton, SIGNAL(clicked(bool)),
                     this, SLOT(applyFirstTabToAllButtonClicked()));
    
    QWidget* tabsWidget = new QWidget();
    QGridLayout* gridLayout = new QGridLayout(tabsWidget);
    
    int32_t columnCounter = 0;
    const int32_t COLUMN_LABEL  = columnCounter++;
    const int32_t COLUMN_LEFT   = columnCounter++;
    const int32_t COLUMN_RIGHT  = columnCounter++;
    const int32_t COLUMN_BOTTOM = columnCounter++;
    const int32_t COLUMN_TOP    = columnCounter++;
    const int32_t COLUMN_MATCH  = columnCounter++;

    const int32_t applyToAllRow = gridLayout->rowCount();
    gridLayout->addWidget(m_applyFirstTabToAllToolButton, applyToAllRow, COLUMN_LABEL, 1, COLUMN_MATCH, Qt::AlignHCenter);
    
    const int32_t titlesRow = gridLayout->rowCount();
    gridLayout->addWidget(tabLabel, titlesRow, COLUMN_LABEL, Qt::AlignLeft);
    gridLayout->addWidget(leftLabel, titlesRow, COLUMN_LEFT,  Qt::AlignHCenter);
    gridLayout->addWidget(rightLabel, titlesRow, COLUMN_RIGHT,  Qt::AlignHCenter);
    gridLayout->addWidget(bottomLabel, titlesRow, COLUMN_BOTTOM,  Qt::AlignHCenter);
    gridLayout->addWidget(topLabel, titlesRow, COLUMN_TOP,  Qt::AlignHCenter);
    
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        QLabel* tabLabel = new QLabel("123456789012345");
        m_tabNumberLabels.push_back(tabLabel);
        
        QDoubleSpinBox* leftMarginSpinBox = createPercentageSpinBox();
        m_leftMarginSpinBoxes.push_back(leftMarginSpinBox);
        QObject::connect(leftMarginSpinBox, SIGNAL(valueChanged(double)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(leftMarginSpinBox, iTab);
        
        QDoubleSpinBox* rightMarginSpinBox = createPercentageSpinBox();
        m_rightMarginSpinBoxes.push_back(rightMarginSpinBox);
        QObject::connect(rightMarginSpinBox, SIGNAL(valueChanged(double)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(rightMarginSpinBox, iTab);
        
        QDoubleSpinBox* bottomMarginSpinBox = createPercentageSpinBox();
        m_bottomMarginSpinBoxes.push_back(bottomMarginSpinBox);
        QObject::connect(bottomMarginSpinBox, SIGNAL(valueChanged(double)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(bottomMarginSpinBox, iTab);
        
        QDoubleSpinBox* topMarginSpinBox = createPercentageSpinBox();
        m_topMarginSpinBoxes.push_back(topMarginSpinBox);
        QObject::connect(topMarginSpinBox, SIGNAL(valueChanged(double)),
                         m_tabIndexSignalMapper, SLOT(map()));
        m_tabIndexSignalMapper->setMapping(topMarginSpinBox, iTab);
        
        QToolButton* matchPixelsToolButton = new QToolButton;
        matchPixelsToolButton->setText("Match Pixel\nSize to Top");
        matchPixelsToolButton->setToolTip("When clicked, percentages of Left, Right, and\n"
                                          "Top are set to match the pixel height of Top");
        m_tabMarginMatchPixelToolButtons.push_back(matchPixelsToolButton);
        QObject::connect(matchPixelsToolButton, SIGNAL(clicked(bool)),
                         m_tabMarginMatchPixelsToolButtonSignalMapper, SLOT(map()));
        m_tabMarginMatchPixelsToolButtonSignalMapper->setMapping(matchPixelsToolButton, iTab);
        
        const int32_t gridRow = gridLayout->rowCount();
        gridLayout->addWidget(tabLabel,            gridRow, COLUMN_LABEL); //, Qt::AlignRight);
        gridLayout->addWidget(leftMarginSpinBox,   gridRow, COLUMN_LEFT);
        gridLayout->addWidget(rightMarginSpinBox,  gridRow, COLUMN_RIGHT);
        gridLayout->addWidget(bottomMarginSpinBox, gridRow, COLUMN_BOTTOM);
        gridLayout->addWidget(topMarginSpinBox,    gridRow, COLUMN_TOP);
        gridLayout->addWidget(matchPixelsToolButton, gridRow, COLUMN_MATCH);
    }
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(tabsWidget);
    scrollArea->setWidgetResizable(true);
    scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    /*
     * Ensure scroll are width is set so widget is fully visible in the horizontal direction
     */
    const int width = tabsWidget->sizeHint().width() + scrollArea->verticalScrollBar()->sizeHint().width();
    scrollArea->setMinimumWidth(width);

    QWidget* widget = new QGroupBox("Tab Margins");
    QVBoxLayout* layout = new QVBoxLayout(widget);
    layout->addWidget(scrollArea);
    
    return widget;
}

/**
 * @return Create a double spin box for percentage values.
 */
QDoubleSpinBox*
GapsAndMarginsDialog::createPercentageSpinBox()
{
    const double minValue   = 0.0;
    const double maxValue   = 100.0;
    const int decimals      = 1;
    const double singleStep = 0.1;
    
    QDoubleSpinBox* doubleSpinBox = new QDoubleSpinBox();
    doubleSpinBox->setMinimum(minValue);
    doubleSpinBox->setMaximum(maxValue);
    doubleSpinBox->setSingleStep(singleStep);
    doubleSpinBox->setDecimals(decimals);
    doubleSpinBox->setKeyboardTracking(false);
    doubleSpinBox->setSuffix("%");
    
    return doubleSpinBox;
}


/**
 * Update the content of the dialog.
 */
void
GapsAndMarginsDialog::updateDialog()
{
    m_browserWindowComboBox->updateComboBox();
    
    const int32_t windowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
    
    updateGapsSpinBoxes(windowIndex);
    
    updateMarginSpinBoxes(windowIndex);
    
    if (windowIndex >= 0) {
        setEnabled(true);
    }
    else {
        setDisabled(true);
    }
}

/**
 * Update the gaps spin boxes.
 *
 * @param windowIndex
 *     Index of window whose gaps are being updated.
 */
void
GapsAndMarginsDialog::updateGapsSpinBoxes(const int32_t windowIndex)
{
    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    
    m_surfaceMontageHorizontalGapSpinBox->blockSignals(true);
    m_surfaceMontageHorizontalGapSpinBox->setValue(gapsAndMargins->getSurfaceMontageHorizontalGapForWindow(windowIndex));
    m_surfaceMontageHorizontalGapSpinBox->blockSignals(false);
    
    m_surfaceMontageVerticalGapSpinBox->blockSignals(true);
    m_surfaceMontageVerticalGapSpinBox->setValue(gapsAndMargins->getSurfaceMontageVerticalGapForWindow(windowIndex));
    m_surfaceMontageVerticalGapSpinBox->blockSignals(false);
    
    m_volumeMontageHorizontalGapSpinBox->blockSignals(true);
    m_volumeMontageHorizontalGapSpinBox->setValue(gapsAndMargins->getVolumeMontageHorizontalGapForWindow(windowIndex));
    m_volumeMontageHorizontalGapSpinBox->blockSignals(false);
    
    m_volumeMontageVerticalGapSpinBox->blockSignals(true);
    m_volumeMontageVerticalGapSpinBox->setValue(gapsAndMargins->getVolumeMontageVerticalGapForWindow(windowIndex));
    m_volumeMontageVerticalGapSpinBox->blockSignals(false);
    
}

/**
 * Update the margin spin boxes.
 *
 * @param windowIndex
 *     Index of window whose margins are being updated.
 */
void
GapsAndMarginsDialog::updateMarginSpinBoxes(const int32_t windowIndex)
{
    std::vector<BrowserTabContent*> tabContents;
//    std::vector<int32_t> tabIndices;
    const BrainBrowserWindow* browserWindow = GuiManager::get()->getBrowserWindowByWindowIndex(windowIndex);
    if (browserWindow != NULL) {
//        browserWindow->getAllTabContentIndices(tabIndices);
        browserWindow->getAllTabContent(tabContents);
    }
    
    const GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    
    const int32_t numValidTabs = static_cast<int32_t>(tabContents.size());
    for (int32_t iTab = 0; iTab < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iTab++) {
        
        bool tabValid = false;
        if (iTab < numValidTabs) {
            CaretAssertVectorIndex(tabContents, iTab);
            const BrowserTabContent* browserTab = tabContents[iTab];
            CaretAssert(browserTab);
            
            const int32_t tabIndex = browserTab->getTabNumber();
            
            CaretAssertArrayIndex(m_tabIndexInTabMarginRow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, iTab);
            m_tabIndexInTabMarginRow[iTab] = tabIndex;
            
            const float leftMargin   = gapsAndMargins->getMarginLeftForTab(tabIndex);
            const float rightMargin  = gapsAndMargins->getMarginRightForTab(tabIndex);
            const float bottomMargin = gapsAndMargins->getMarginBottomForTab(tabIndex);
            const float topMargin    = gapsAndMargins->getMarginTopForTab(tabIndex);
            
            CaretAssertVectorIndex(m_tabNumberLabels, iTab);
//            m_tabNumberLabels[iTab]->setText("Tab " + QString::number(tabIndex + 1));
            m_tabNumberLabels[iTab]->setText(browserTab->getTabName());
            
            CaretAssertVectorIndex(m_leftMarginSpinBoxes, iTab);
            m_leftMarginSpinBoxes[iTab]->blockSignals(true);
            m_leftMarginSpinBoxes[iTab]->setValue(leftMargin);
            m_leftMarginSpinBoxes[iTab]->blockSignals(false);
            
            CaretAssertVectorIndex(m_rightMarginSpinBoxes, iTab);
            m_rightMarginSpinBoxes[iTab]->blockSignals(true);
            m_rightMarginSpinBoxes[iTab]->setValue(rightMargin);
            m_rightMarginSpinBoxes[iTab]->blockSignals(false);
            
            CaretAssertVectorIndex(m_bottomMarginSpinBoxes, iTab);
            m_bottomMarginSpinBoxes[iTab]->blockSignals(true);
            m_bottomMarginSpinBoxes[iTab]->setValue(bottomMargin);
            m_bottomMarginSpinBoxes[iTab]->blockSignals(false);
            
            CaretAssertVectorIndex(m_topMarginSpinBoxes, iTab);
            m_topMarginSpinBoxes[iTab]->blockSignals(true);
            m_topMarginSpinBoxes[iTab]->setValue(topMargin);
            m_topMarginSpinBoxes[iTab]->blockSignals(false);
            
            tabValid = true;
        }
        else {
            m_tabIndexInTabMarginRow[iTab] = -1;
        }
        
        CaretAssertVectorIndex(m_tabNumberLabels, iTab);
        m_tabNumberLabels[iTab]->setVisible(tabValid);
        
        CaretAssertVectorIndex(m_leftMarginSpinBoxes, iTab);
        m_leftMarginSpinBoxes[iTab]->setVisible(tabValid);
        
        CaretAssertVectorIndex(m_rightMarginSpinBoxes, iTab);
        m_rightMarginSpinBoxes[iTab]->setVisible(tabValid);
        
        CaretAssertVectorIndex(m_bottomMarginSpinBoxes, iTab);
        m_bottomMarginSpinBoxes[iTab]->setVisible(tabValid);
        
        CaretAssertVectorIndex(m_topMarginSpinBoxes, iTab);
        m_topMarginSpinBoxes[iTab]->setVisible(tabValid);
        
        CaretAssertVectorIndex(m_tabMarginMatchPixelToolButtons, iTab);
        m_tabMarginMatchPixelToolButtons[iTab]->setVisible(tabValid);
    }
}

/**
 * Find the horizontal percentage that is the equivalent pixel
 * size as the vertical percentage.
 *
 * @param verticalPercentage
 *     Vertical percentage size.
 * @param viewportWidth
 *     Width of viewport in pixels.
 * @param viewportHeight
 *     Height of viewport in pixels.
 * @return
 *     The horizontal percentage.
 */
float
GapsAndMarginsDialog::matchHorizontalPercentageFromVerticalPercentage(const float verticalPercentage,
                                                              const float viewportWidth,
                                                              const float viewportHeight) const
{
    float horizontalPercentageOut = 0.0;
    
    const float marginPixelSize = verticalPercentage * viewportHeight;
    if (viewportWidth > 0) {
        horizontalPercentageOut = marginPixelSize / viewportWidth;
    }
    
    return horizontalPercentageOut;
}

/**
 * Called when match pixel button is clicked.
 *
 * @param setVisible(tabValid);
 *    Index of row that is clicked
 */
void
GapsAndMarginsDialog::tabMarginMatchPixelButtonClicked(int rowIndex)
{
    CaretAssertArrayIndex(m_tabIndexInTabMarginRow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, rowIndex);
    const int32_t tabIndex = m_tabIndexInTabMarginRow[rowIndex];
    if (tabIndex >= 0) {
        EventGetViewportSize viewportSizeEvent(EventGetViewportSize::MODE_TAB_BEFORE_MARGINS_INDEX,
                                                         tabIndex);
        EventManager::get()->sendEvent(viewportSizeEvent.getPointer());
        if (viewportSizeEvent.isViewportSizeValid()) {
            int32_t viewport[4];
            viewportSizeEvent.getViewportSize(viewport);
            
            CaretAssertVectorIndex(m_topMarginSpinBoxes, rowIndex);
            const float bottomTopPercentage = m_topMarginSpinBoxes[rowIndex]->value();
            const float leftRightMarginPercentage = matchHorizontalPercentageFromVerticalPercentage(bottomTopPercentage,
                                                                                                    viewport[2],
                                                                                                    viewport[3]);
            
            CaretAssertVectorIndex(m_leftMarginSpinBoxes, rowIndex);
            CaretAssertVectorIndex(m_rightMarginSpinBoxes, rowIndex);
            CaretAssertVectorIndex(m_bottomMarginSpinBoxes, rowIndex);
            CaretAssertVectorIndex(m_topMarginSpinBoxes, rowIndex);
            
            m_leftMarginSpinBoxes[rowIndex]->blockSignals(true);
            m_leftMarginSpinBoxes[rowIndex]->setValue(leftRightMarginPercentage);
            m_leftMarginSpinBoxes[rowIndex]->blockSignals(false);
            
            m_rightMarginSpinBoxes[rowIndex]->blockSignals(true);
            m_rightMarginSpinBoxes[rowIndex]->setValue(leftRightMarginPercentage);
            m_rightMarginSpinBoxes[rowIndex]->blockSignals(false);
            
            m_bottomMarginSpinBoxes[rowIndex]->blockSignals(true);
            m_bottomMarginSpinBoxes[rowIndex]->setValue(bottomTopPercentage);
            m_bottomMarginSpinBoxes[rowIndex]->blockSignals(false);
            
            m_topMarginSpinBoxes[rowIndex]->blockSignals(true);
            m_topMarginSpinBoxes[rowIndex]->setValue(bottomTopPercentage);
            m_topMarginSpinBoxes[rowIndex]->blockSignals(false);
            
            tabMarginChanged(rowIndex);
        }
        else {
            CaretLogWarning("Unable to get tab "
                            + AString::number(tabIndex + 1)
                            + " viewport size.");
        }
    }
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
GapsAndMarginsDialog::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        EventUserInterfaceUpdate* eventUpdate = dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(eventUpdate);

        updateDialog();
        
        eventUpdate->setEventProcessed();
    }
}

/**
 * Gets called when the browser window index is changed.
 *
 * @param browserWindowIndex
 *     Index of the browser window.
 */
void
GapsAndMarginsDialog::browserWindowIndexChanged(const int32_t /*browserWindowIndex*/)
{
    updateDialog();
}

/**
 * Gets called when a tab's margin is changed.
 *
 * @param rowIndex
 *    Index of the tab.
 */
void
GapsAndMarginsDialog::tabMarginChanged(int rowIndex)
{
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    
    CaretAssertArrayIndex(m_tabIndexInTabMarginRow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, rowIndex);
    const int32_t tabIndex = m_tabIndexInTabMarginRow[rowIndex];
    CaretAssert(tabIndex >= 0);
    
    CaretAssertVectorIndex(m_leftMarginSpinBoxes, rowIndex);
    CaretAssertVectorIndex(m_rightMarginSpinBoxes, rowIndex);
    CaretAssertVectorIndex(m_bottomMarginSpinBoxes, rowIndex);
    CaretAssertVectorIndex(m_topMarginSpinBoxes, rowIndex);
    
    gapsAndMargins->setMarginLeftForTab(tabIndex,
                                        m_leftMarginSpinBoxes[rowIndex]->value());
    gapsAndMargins->setMarginRightForTab(tabIndex,
                                         m_rightMarginSpinBoxes[rowIndex]->value());
    gapsAndMargins->setMarginBottomForTab(tabIndex,
                                          m_bottomMarginSpinBoxes[rowIndex]->value());
    gapsAndMargins->setMarginTopForTab(tabIndex,
                                       m_topMarginSpinBoxes[rowIndex]->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when apply first tab to all button is clicked.
 */
void
GapsAndMarginsDialog::applyFirstTabToAllButtonClicked()
{
    const int32_t rowZero = 0;
    CaretAssertVectorIndex(m_leftMarginSpinBoxes, rowZero);
    const float leftMargin = m_leftMarginSpinBoxes[rowZero]->value();
    
    CaretAssertVectorIndex(m_rightMarginSpinBoxes, rowZero);
    const float rightMargin = m_rightMarginSpinBoxes[rowZero]->value();
    
    CaretAssertVectorIndex(m_bottomMarginSpinBoxes, rowZero);
    const float bottomMargin = m_bottomMarginSpinBoxes[rowZero]->value();
    
    CaretAssertVectorIndex(m_topMarginSpinBoxes, rowZero);
    const float topMargin = m_topMarginSpinBoxes[rowZero]->value();
    
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    
    for (int32_t iRow = 1; iRow < BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS; iRow++) {
        CaretAssertArrayIndex(m_tabIndexInTabMarginRow, BrainConstants::MAXIMUM_NUMBER_OF_BROWSER_TABS, iRow);
        const int32_t tabIndex = m_tabIndexInTabMarginRow[iRow];
        if (tabIndex >= 0) {
            gapsAndMargins->setMarginLeftForTab(tabIndex, leftMargin);
            gapsAndMargins->setMarginRightForTab(tabIndex, rightMargin);
            gapsAndMargins->setMarginBottomForTab(tabIndex, bottomMargin);
            gapsAndMargins->setMarginTopForTab(tabIndex, topMargin);
        }
    }
    
    /*
     * Update dialog since "select all" will change all margins to the first margin value
     */
    updateDialog();
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when a surface montage gap is changed.
 */
void
GapsAndMarginsDialog::surfaceMontageGapChanged()
{
    const int32_t windowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
    if (windowIndex < 0) {
        return;
    }
    
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setSurfaceMontageHorizontalGapForWindow(windowIndex,
                                                            m_surfaceMontageHorizontalGapSpinBox->value());
    gapsAndMargins->setSurfaceMontageVerticalGapForWindow(windowIndex,
                                                          m_surfaceMontageVerticalGapSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when a volume montage gap is changed.
 */
void
GapsAndMarginsDialog::volumeMontageGapChanged()
{
    const int32_t windowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
    if (windowIndex < 0) {
        return;
    }
    
    GapsAndMargins* gapsAndMargins = GuiManager::get()->getBrain()->getGapsAndMargins();
    gapsAndMargins->setVolumeMontageHorizontalGapForWindow(windowIndex,
                                                           m_volumeMontageHorizontalGapSpinBox->value());
    gapsAndMargins->setVolumeMontageVerticalGapForWindow(windowIndex,
                                                         m_volumeMontageVerticalGapSpinBox->value());
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the surface montage match pixels tool button is clicked.
 */
void
GapsAndMarginsDialog::surfaceMontageMatchPixelButtonClicked()
{
    const int32_t windowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
    if (windowIndex < 0) {
        return;
    }
    
    EventGetViewportSize viewportSizeEvent(EventGetViewportSize::MODE_SURFACE_MONTAGE,
                                                     windowIndex);
    EventManager::get()->sendEvent(viewportSizeEvent.getPointer());
    if (viewportSizeEvent.isViewportSizeValid()) {
        int32_t viewport[4];
        viewportSizeEvent.getViewportSize(viewport);
        
        const float verticalPercentage = m_surfaceMontageVerticalGapSpinBox->value();
        const float leftRightMarginPercentage = matchHorizontalPercentageFromVerticalPercentage(verticalPercentage,
                                                                                                viewport[2],
                                                                                                viewport[3]);
        m_surfaceMontageHorizontalGapSpinBox->blockSignals(true);
        m_surfaceMontageHorizontalGapSpinBox->setValue(leftRightMarginPercentage);
        m_surfaceMontageHorizontalGapSpinBox->blockSignals(false);
        
        surfaceMontageGapChanged();
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}

/**
 * Gets called when the volume montage match pixels tool button is clicked.
 */
void
GapsAndMarginsDialog::volumeMontageMatchPixelButtonClicked()
{
    const int32_t windowIndex = m_browserWindowComboBox->getSelectedBrowserWindowIndex();
    if (windowIndex < 0) {
        return;
    }
    
    EventGetViewportSize viewportSizeEvent(EventGetViewportSize::MODE_VOLUME_MONTAGE,
                                                     windowIndex);
    EventManager::get()->sendEvent(viewportSizeEvent.getPointer());
    if (viewportSizeEvent.isViewportSizeValid()) {
        int32_t viewport[4];
        viewportSizeEvent.getViewportSize(viewport);
        
        const float verticalPercentage = m_volumeMontageVerticalGapSpinBox->value();
        const float horizontalPercentage = matchHorizontalPercentageFromVerticalPercentage(verticalPercentage,
                                                                                                viewport[2],
                                                                                                viewport[3]);
        m_volumeMontageHorizontalGapSpinBox->blockSignals(true);
        m_volumeMontageHorizontalGapSpinBox->setValue(horizontalPercentage);
        m_volumeMontageHorizontalGapSpinBox->blockSignals(false);
        
        volumeMontageGapChanged();
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
}






/*LICENSE_START*/
/*
 *  Copyright (C) 2019 Washington University School of Medicine
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

#define __IDENTIFICATION_DISPLAY_WIDGET_DECLARE__
#include "IdentificationDisplayWidget.h"
#undef __IDENTIFICATION_DISPLAY_WIDGET_DECLARE__

#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QGuiApplication>
#include <QRadioButton>
#include <QScrollArea>
#include <QScrollBar>
#include <QSpinBox>
#include <QTabWidget>
#include <QTextBrowser>
#include <QToolButton>
#include <QVBoxLayout>

#include "Brain.h"
#include "CaretAssert.h"
#include "CaretColorEnumComboBox.h"
#include "CaretLogger.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "EventUpdateInformationWindows.h"
#include "EventUserInterfaceUpdate.h"
#include "GuiManager.h"
#include "IdentificationFileFilteringTableWidget.h"
#include "IdentificationFilter.h"
#include "IdentificationHistoryManager.h"
#include "IdentificationManager.h"
#include "SceneClass.h"
#include "SceneClassAssistant.h"
#include "WuQValueChangedSignalWatcher.h"
#include "WuQtUtilities.h"
using namespace caret;

/**
 * \class caret::IdentificationDisplayWidget
 * \brief Widget for display of identification data
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param location
 *    Location of widget
 * @param
 *     The parent widget
 */
IdentificationDisplayWidget::IdentificationDisplayWidget(const Location location,
                                                         QWidget* parent)
: QWidget(parent),
m_location(location)
{
    m_infoWidget = createInfoWidget();
    
    m_filteringFilesWidget = createFilteringFilesWidget();
    m_filteringSettingsWidget = createFilteringSettingsWidget();
    m_symbolsWidget = createSymbolsWidget();
    m_chartLineLayerSymbolWidget = createChartLineLayerSymbolsWidget();
    
    m_tabWidget = new QTabWidget();
    const int32_t infoIndex    = m_tabWidget->addTab(m_infoWidget,                 "Info");
    const int32_t chartIndex   = m_tabWidget->addTab(m_chartLineLayerSymbolWidget, "Chart");
    const int32_t filesIndex   = m_tabWidget->addTab(m_filteringFilesWidget,       "Files");
    const int32_t filterIndex  = m_tabWidget->addTab(m_filteringSettingsWidget,    "Filter");
    const int32_t symbolsIndex = m_tabWidget->addTab(m_symbolsWidget,              "Symbols");
    
    m_tabWidget->setTabToolTip(infoIndex,
                               "<html><body>Display results of identification operations</body></hmtl>");
    m_tabWidget->setTabToolTip(chartIndex,
                               "<html><body>Controls for chart identification</body></html>");
    m_tabWidget->setTabToolTip(filesIndex,
                               "<html><body>Select specific files for identification; Enables "
                               "identification of files that are NOT in an overlay</body></html>");
    m_tabWidget->setTabToolTip(filterIndex,
                               "<html><body>Select tabs (all or selected)for identification data; "
                               "Enable feature (borders/foci) identification</body></html>");
    m_tabWidget->setTabToolTip(symbolsIndex,
                               "<html><body></body>Control the size/color/types of symbols shown "
                               "on surfaces and volume slices</html>");
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(m_tabWidget, 100);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
    updateContent(true);
}

/**
 * Destructor.
 */
IdentificationDisplayWidget::~IdentificationDisplayWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Receive an event.
 *
 * @param event
 *    An event for which this instance is listening.
 */
void
IdentificationDisplayWidget::receiveEvent(Event* event)
{
    if (event->getEventType() == EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS) {
        EventUpdateInformationWindows* textEvent =
        dynamic_cast<EventUpdateInformationWindows*>(event);
        CaretAssert(textEvent);
        textEvent->setEventProcessed();
        
        updateContent(true);
    }
    else if (event->getEventType() == EventTypeEnum::EVENT_USER_INTERFACE_UPDATE) {
        /*
         * Need to update list of files
         */
        updateFilteringWidget();
    }
}

/**
 * Update the widget's content
 * @param scrollTextToEndFlag
 *   If true, scroll text to bottom
 */
void
IdentificationDisplayWidget::updateContent(const bool scrollTextToEndFlag)
{
    updateSymbolsWidget();
    updateFilteringWidget();
    updateInfoWidget(scrollTextToEndFlag);
    updateChartLineLayerSymbolsWidget();
}

/**
 * Update the display widget's content
 * @param scrollTextToEndFlag
 *   If true, scroll text to bottom
 */
void
IdentificationDisplayWidget::updateInfoWidget(const bool scrollTextToEndFlag)
{
    IdentificationHistoryManager* historyManager = getHistoryManager();
    QSignalBlocker lastBlocker(m_infoShowHistoryCountSpinBox);
    m_infoShowHistoryCountSpinBox->setValue(historyManager->getShowLastHistoryCount());
    
    /*
     * Maintain or move scroll bar position
     */
    QScrollBar* sb = m_infoTextBrowser->verticalScrollBar();
    CaretAssert(sb);
    int32_t positionValue = sb->value();
    m_infoTextBrowser->setHtml(historyManager->getHtml());
    if (scrollTextToEndFlag) {
        positionValue = sb->maximum();
        positionValue = std::max(positionValue,
                                 0);
    }
    positionValue = std::min(positionValue,
                             sb->maximum());
    sb->setValue(positionValue);
}

/**
 * @return Instance of the display widget
 */
QWidget*
IdentificationDisplayWidget::createInfoWidget()
{
    m_infoTextBrowser = new QTextBrowser();
    m_infoTextBrowser->setLineWrapMode(QTextEdit::NoWrap);
    m_infoTextBrowser->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
    
    QToolButton* clearHistoryToolButton = new QToolButton();
    clearHistoryToolButton->setText("Clear");
    clearHistoryToolButton->setToolTip("Remove all information text");
    QObject::connect(clearHistoryToolButton, &QToolButton::clicked,
                     this, &IdentificationDisplayWidget::infoClearHistoryToolButtonClicked);
    
    QToolButton* copyHistoryToolButton = new QToolButton();
    copyHistoryToolButton->setText("Copy");
    copyHistoryToolButton->setToolTip("Copy the SELECTED text to the Clipboard");
    QObject::connect(copyHistoryToolButton, &QToolButton::clicked,
                     m_infoTextBrowser, &QTextBrowser::copy);
    QObject::connect(m_infoTextBrowser, &QTextBrowser::copyAvailable,
                     copyHistoryToolButton, &QToolButton::setEnabled);
    copyHistoryToolButton->setEnabled(false);
    
    AString removeButtonText;
    switch (m_location) {
        case Location::Dialog:
        case Location::HorizontalToolBox:
            removeButtonText = "Remove\nSymbols";
            break;
        case Location::VerticalToolBox:
            removeButtonText = "Remove Symbols";
            break;
    }
    QToolButton* removeSymbolsButton = new QToolButton();
    removeSymbolsButton->setText(removeButtonText);
    
    removeSymbolsButton->setToolTip("Remove identification symbols on surfaces and volume slices");
    QObject::connect(removeSymbolsButton, &QToolButton::clicked,
                     this, &IdentificationDisplayWidget::infoRemoveSymbolsButtonClicked);
    
    const QString showToolTip("<html>Selects number of recent identification operations displayed in the information text. "
                              "Selections are All, 1, 2, etc.</html>");
    m_infoShowHistoryCountSpinBox = new QSpinBox();
    m_infoShowHistoryCountSpinBox->setRange(0, 99);
    m_infoShowHistoryCountSpinBox->setSingleStep(1);
    m_infoShowHistoryCountSpinBox->setSpecialValueText("Show All");
    m_infoShowHistoryCountSpinBox->setToolTip(showToolTip);
    m_infoShowHistoryCountSpinBox->setPrefix("Show ");
    QObject::connect(m_infoShowHistoryCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &IdentificationDisplayWidget::infoShowHistoryCountSpinBoxValueChanged);
    m_infoShowHistoryCountSpinBox->setSizePolicy(QSizePolicy::Fixed,
                                                 QSizePolicy::Fixed);
    
    QGroupBox* historyGroupBox = new QGroupBox();
    historyGroupBox->setAlignment(Qt::AlignHCenter);
    QLabel* historyLabel(NULL);
    Qt::Alignment historyAlignment = Qt::Alignment();
    switch (m_location) {
        case Location::Dialog:
        {
            historyGroupBox->setTitle("History");
            QVBoxLayout* historyLayout = new QVBoxLayout(historyGroupBox);
            WuQtUtilities::setLayoutSpacingAndMargins(historyLayout, 2, 4);
            historyLayout->addWidget(clearHistoryToolButton, 0, Qt::AlignHCenter);
            historyLayout->addWidget(copyHistoryToolButton, 0, Qt::AlignHCenter);
            historyLayout->addWidget(m_infoShowHistoryCountSpinBox, 0, Qt::AlignHCenter);
        }
            break;
        case Location::HorizontalToolBox:
        {
            historyGroupBox->setTitle("History");
            QVBoxLayout* historyLayout = new QVBoxLayout(historyGroupBox);
            WuQtUtilities::setLayoutSpacingAndMargins(historyLayout, 2, 4);
            historyLayout->addWidget(clearHistoryToolButton, 0, Qt::AlignHCenter);
            historyLayout->addWidget(copyHistoryToolButton, 0, Qt::AlignHCenter);
            historyLayout->addWidget(m_infoShowHistoryCountSpinBox, 0, Qt::AlignHCenter);
        }
            break;
        case Location::VerticalToolBox:
        {
            historyGroupBox->setTitle("History");
            QHBoxLayout* historyLayout = new QHBoxLayout(historyGroupBox);
            WuQtUtilities::setLayoutSpacingAndMargins(historyLayout, 5, 6);
            historyLayout->addWidget(clearHistoryToolButton, 0);
            historyLayout->addWidget(copyHistoryToolButton, 0);
            historyLayout->addWidget(m_infoShowHistoryCountSpinBox, 0);
            historyAlignment = Qt::AlignLeft;
        }
            break;
    }
    
    QBoxLayout* historySymbolLayout(NULL);
    Qt::Alignment symbolAlignment = Qt::Alignment();
    switch (m_location) {
        case Location::Dialog:
        case Location::HorizontalToolBox:
            historySymbolLayout = new QVBoxLayout();
            symbolAlignment = Qt::AlignHCenter;
            break;
        case Location::VerticalToolBox:
            historySymbolLayout = new QVBoxLayout();
            break;
    }
    CaretAssert(historySymbolLayout);
    WuQtUtilities::setLayoutSpacingAndMargins(historySymbolLayout, 4, 0);
    if (historyLabel != NULL) {
        historySymbolLayout->addWidget(historyLabel);
    }
    historySymbolLayout->addWidget(historyGroupBox, 0, historyAlignment);
    historySymbolLayout->addWidget(removeSymbolsButton, 0, symbolAlignment);
    historySymbolLayout->addStretch();
    
    QWidget* widget = new QWidget();
    QBoxLayout* layout(NULL);
    switch (m_location) {
        case Location::Dialog:
        case Location::HorizontalToolBox:
            layout = new QHBoxLayout(widget);
            break;
        case Location::VerticalToolBox:
            layout = new QVBoxLayout(widget);
            break;
    }
    CaretAssert(layout);
    WuQtUtilities::setLayoutSpacingAndMargins(layout,
                                              0,
                                              2);
    layout->addWidget(m_infoTextBrowser, 100);
    layout->addLayout(historySymbolLayout, 0);
    
    return widget;
}

/**
 * Called when history spin box is changed.
 *
 * @param value
 *   New value
 */
void
IdentificationDisplayWidget::infoShowHistoryCountSpinBoxValueChanged(int value)
{
    IdentificationHistoryManager* historyManager = getHistoryManager();
    historyManager->setShowLastHistoryCount(value);
    updateInfoWidget(false);
}

/**
 * Called when clear history tool button is clicked
 */
void IdentificationDisplayWidget::infoClearHistoryToolButtonClicked()
{
    IdentificationHistoryManager* historyManager = getHistoryManager();
    historyManager->clearHistory();
    updateInfoWidget(false);
}

/**
 * Called when remove symbols button is clicked.
 */
void
IdentificationDisplayWidget::infoRemoveSymbolsButtonClicked()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* idManager = brain->getIdentificationManager();
    idManager->removeAllIdentifiedSymbols();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * @return The identification history manager
 */
IdentificationHistoryManager*
IdentificationDisplayWidget::getHistoryManager()
{
    return GuiManager::get()->getBrain()->getIdentificationManager()->getIdentificationHistoryManager();
}

/**
 * @return Instance of the filtering files widget
 */
QWidget*
IdentificationDisplayWidget::createFilteringFilesWidget()
{
    m_fileFilteringTableWidget = new IdentificationFileFilteringTableWidget();
    QGroupBox* filesGroupBox = new QGroupBox("File Identification");
    QVBoxLayout* filesLayout = new QVBoxLayout(filesGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(filesLayout, 0, 0);
    filesLayout->addWidget(m_fileFilteringTableWidget, 100);
    
    
    return filesGroupBox;
}


/**
 * @return Instance of the filtering widget
 */
QWidget*
IdentificationDisplayWidget::createFilteringSettingsWidget()
{
    QWidget* tabFilterWidget = new QGroupBox("Show Files from Enabled Overlays");
    QVBoxLayout* tabFilterLayout = new QVBoxLayout(tabFilterWidget);
    m_tabFilterButtonGroup = new QButtonGroup(this);
#if QT_VERSION >= 0x060000
    QObject::connect(m_tabFilterButtonGroup, QOverload<int>::of(&QButtonGroup::idClicked),
                     this, &IdentificationDisplayWidget::tabFilterRadioButtonClicked);
#else
    QObject::connect(m_tabFilterButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
                     this, &IdentificationDisplayWidget::tabFilterRadioButtonClicked);
#endif
    
    std::vector<IdentificationFilterTabSelectionEnum::Enum> tabFilterEnums;
    IdentificationFilterTabSelectionEnum::getAllEnums(tabFilterEnums);
    for (auto tf : tabFilterEnums) {
        QRadioButton* rb = new QRadioButton(IdentificationFilterTabSelectionEnum::toGuiName(tf));
        WuQtUtilities::setWordWrappedToolTip(rb, IdentificationFilterTabSelectionEnum::toToolTip(tf));
        /* second argument is integer ID that encodes the integer for tab filtering enum */
        m_tabFilterButtonGroup->addButton(rb,
                                          IdentificationFilterTabSelectionEnum::toIntegerCode(tf));
        tabFilterLayout->addWidget(rb);
    }
    
    m_filteringCiftiLoadingCheckBox = new QCheckBox("CIFTI Row/Column");
    m_filteringCiftiLoadingCheckBox->setToolTip("<html>Show the row/column for any CIFTI File data loaded "
                                                "in response to an identification operation</html>");
    
    m_filteringBorderCheckBox = new QCheckBox("Border");
    m_filteringBorderCheckBox->setToolTip("<html>Enable identification of borders</html>");
    
    m_filteringFociCheckBox  = new QCheckBox("Focus");
    m_filteringFociCheckBox->setToolTip("<html>Enable identification of foci</html>");
    
    m_filteringVertexVoxelCheckBox = new QCheckBox("Vertex/Voxel Geometry");
    m_filteringVertexVoxelCheckBox->setToolTip("<html>"
                                               "Enable display of geometry for the identified "
                                               "vertex and/or voxel"
                                               "</html>");
    
    WuQValueChangedSignalWatcher* signalWatcher = new WuQValueChangedSignalWatcher(this);
    QObject::connect(signalWatcher, &WuQValueChangedSignalWatcher::valueChanged,
                     this, &IdentificationDisplayWidget::filteringChanged);
    signalWatcher->addObject(m_filteringCiftiLoadingCheckBox);
    signalWatcher->addObject(m_filteringBorderCheckBox);
    signalWatcher->addObject(m_filteringFociCheckBox);
    signalWatcher->addObject(m_filteringVertexVoxelCheckBox);
    
    QWidget* showDataWidget = new QGroupBox("Data Identification");
    QVBoxLayout* showLayout = new QVBoxLayout(showDataWidget);
    showLayout->addWidget(m_filteringBorderCheckBox);
    showLayout->addWidget(m_filteringCiftiLoadingCheckBox);
    showLayout->addWidget(m_filteringFociCheckBox);
    showLayout->addWidget(m_filteringVertexVoxelCheckBox);
    
    Qt::Alignment alignment = Qt::Alignment();
    QWidget* widget = new QWidget();
    QBoxLayout* layout(NULL);
    switch (m_location) {
        case Location::Dialog:
            alignment = Qt::AlignTop;
            layout = new QHBoxLayout(widget);
            break;
        case Location::HorizontalToolBox:
        case Location::VerticalToolBox:
            alignment = Qt::AlignLeft;
            layout = new QVBoxLayout(widget);
            break;
    }
    
    CaretAssert(layout);
    layout->setSpacing(2);
    layout->addWidget(tabFilterWidget, 0, alignment);
    layout->addWidget(showDataWidget, 0, alignment);
    layout->addStretch();
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    return scrollArea;
}

/**
 * Called when a tab filtering radio button is clicked
 * @param buttonID
 * ID of button that was clicked
 */
void
IdentificationDisplayWidget::tabFilterRadioButtonClicked(int32_t buttonID)
{
    bool validFlag(false);
    const IdentificationFilterTabSelectionEnum::Enum tabFilter = IdentificationFilterTabSelectionEnum::fromIntegerCode(buttonID,
                                                                                                                       &validFlag);
    IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    IdentificationFilter* filter = idManager->getIdentificationFilter();
    filter->setTabFiltering(tabFilter);
}

/**
 * Update the filtering widget's content
 */
void
IdentificationDisplayWidget::updateFilteringWidget()
{
    const IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    const IdentificationFilter* filter = idManager->getIdentificationFilter();
    
    const IdentificationFilterTabSelectionEnum::Enum tabFilter = filter->getTabFiltering();
    const int32_t integerCode = IdentificationFilterTabSelectionEnum::toIntegerCode(tabFilter);
    QAbstractButton* filterAbstractButton = m_tabFilterButtonGroup->button(integerCode);
    CaretAssert(filterAbstractButton);
    QRadioButton* filterRadioButton = qobject_cast<QRadioButton*>(filterAbstractButton);
    CaretAssert(filterRadioButton);
    filterRadioButton->setChecked(true);
    
    m_filteringCiftiLoadingCheckBox->setChecked(filter->isShowCiftiLoadingEnabled());
    m_filteringBorderCheckBox->setChecked(filter->isShowBorderEnabled());
    m_filteringFociCheckBox->setChecked(filter->isShowFociEnabled());
    m_filteringVertexVoxelCheckBox->setChecked(filter->isShowVertexVoxelEnabled());
    
    m_fileFilteringTableWidget->updateContent();
}

/**
 * Called when a filtering item is changed
 */
void
IdentificationDisplayWidget::filteringChanged()
{
    IdentificationManager* idManager = GuiManager::get()->getBrain()->getIdentificationManager();
    IdentificationFilter* filter = idManager->getIdentificationFilter();
    filter->setShowCiftiLoadingEnabled(m_filteringCiftiLoadingCheckBox->isChecked());
    filter->setShowBorderEnabled(m_filteringBorderCheckBox->isChecked());
    filter->setShowFociEnabled(m_filteringFociCheckBox->isChecked());
    filter->setShowVertexVoxelEnabled(m_filteringVertexVoxelCheckBox->isChecked());
}

/**
 * Update the symbols widget's content
 */
void
IdentificationDisplayWidget::updateSymbolsWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    
    m_symbolsShowHistologyCheckBox->setChecked(info->isShowHistologyIdentificationSymbols());
    m_symbolsShowMediaCheckbox->setChecked(info->isShowMediaIdentificationSymbols());
    m_symbolsShowSurfaceIdCheckBox->setChecked(info->isShowSurfaceIdentificationSymbols());
    m_symbolsShowVolumeIdCheckBox->setChecked(info->isShowVolumeIdentificationSymbols());
    m_symbolsSurfaceContralateralVertexCheckBox->setChecked(info->isContralateralIdentificationEnabled());
    
    m_symbolsIdColorComboBox->setSelectedColor(info->getIdentificationSymbolColor());
    m_symbolsContralateralIdColorComboBox->setSelectedColor(info->getIdentificationContralateralSymbolColor());
    
    QSignalBlocker symbolSizeBlocker(m_symbolsMillimetersDiameterSpinBox);
    m_symbolsMillimetersDiameterSpinBox->setValue(info->getIdentificationSymbolSize());
    QSignalBlocker recentSymbolSizeBlocker(m_symbolsMillimetersMostRecentDiameterSpinBox);
    m_symbolsMillimetersMostRecentDiameterSpinBox->setValue(info->getMostRecentIdentificationSymbolSize());
    QSignalBlocker symbolPercentageSizeBlocker(m_symbolsPercentageDiameterSpinBox);
    m_symbolsPercentageDiameterSpinBox->setValue(info->getIdentificationSymbolPercentageSize());
    QSignalBlocker symbolPercentageMostRecentSizeBlocker(m_symbolsPercentageMostRecentDiameterSpinBox);
    m_symbolsPercentageMostRecentDiameterSpinBox->setValue(info->getMostRecentIdentificationSymbolPercentageSize());
    
    QSignalBlocker histologySymbolSizeBlocker(m_symbolsHistologyPercentageDiameterSpinBox);
    m_symbolsHistologyPercentageDiameterSpinBox->setValue(info->getHistologyIdentificationPercentageSymbolSize());
    QSignalBlocker histologyMostRecentSymbolSizeBlocker(m_symbolsHistologyPercentageMostRecentDiameterSpinBox);
    m_symbolsHistologyPercentageMostRecentDiameterSpinBox->setValue(info->getHistologyIdentificationMostRecentPercentageSymbolSize());

    QSignalBlocker mediaSymbolSizeBlocker(m_symbolsMediaPercentageDiameterSpinBox);
    m_symbolsMediaPercentageDiameterSpinBox->setValue(info->getMediaIdentificationPercentageSymbolSize());
    QSignalBlocker mediaMostRecentSymbolSizeBlocker(m_symbolsMediaPercentageMostRecentDiameterSpinBox);
    m_symbolsMediaPercentageMostRecentDiameterSpinBox->setValue(info->getMediaIdentificationMostRecentPercentageSymbolSize());
    
    bool mmFlag(false);
    bool pctFlag(false);
    switch (info->getIdentificationSymbolSizeType()) {
        case IdentificationSymbolSizeTypeEnum::MILLIMETERS:
            m_symbolSizeMillimeterRadioButton->setChecked(true);
            mmFlag = true;
            break;
        case IdentificationSymbolSizeTypeEnum::PERCENTAGE:
            m_symbolSizePercentageRadioButton->setChecked(true);
            pctFlag = true;
            break;
    }
    
    m_symbolsMillimetersDiameterSpinBox->setEnabled(mmFlag);
    m_symbolsMillimetersMostRecentDiameterSpinBox->setEnabled(mmFlag);
    m_symbolsPercentageDiameterSpinBox->setEnabled(pctFlag);
    m_symbolsPercentageMostRecentDiameterSpinBox->setEnabled(pctFlag);
}

/**
 * @return Instance of the symbols widget
 */
QWidget*
IdentificationDisplayWidget::createSymbolsWidget()
{
    m_symbolsShowHistologyCheckBox = new QCheckBox(IdentificationManager::getShowSymbolOnTypeLabel(IdentifiedItemUniversalTypeEnum::HISTOLOGY_PLANE_COORDINATE));
    m_symbolsShowHistologyCheckBox->setToolTip(IdentificationManager::getShowSymbolOnTypeToolTip(IdentifiedItemUniversalTypeEnum::HISTOLOGY_PLANE_COORDINATE));

    m_symbolsShowMediaCheckbox = new QCheckBox(IdentificationManager::getShowSymbolOnTypeLabel(IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE));
    m_symbolsShowMediaCheckbox->setToolTip(IdentificationManager::getShowSymbolOnTypeToolTip(IdentifiedItemUniversalTypeEnum::MEDIA_LOGICAL_COORDINATE));
    
    m_symbolsShowSurfaceIdCheckBox = new QCheckBox(IdentificationManager::getShowSymbolOnTypeLabel(IdentifiedItemUniversalTypeEnum::SURFACE));
    m_symbolsShowSurfaceIdCheckBox->setToolTip(IdentificationManager::getShowSymbolOnTypeToolTip(IdentifiedItemUniversalTypeEnum::SURFACE));
    
    m_symbolsShowVolumeIdCheckBox = new QCheckBox(IdentificationManager::getShowSymbolOnTypeLabel(IdentifiedItemUniversalTypeEnum::VOLUME_SLICES));
    m_symbolsShowVolumeIdCheckBox->setToolTip(IdentificationManager::getShowSymbolOnTypeToolTip(IdentifiedItemUniversalTypeEnum::VOLUME_SLICES));
    
    m_symbolsSurfaceContralateralVertexCheckBox = new QCheckBox("Show Surface Contralateral");
    m_symbolsSurfaceContralateralVertexCheckBox->setToolTip("<html>Enable display of contralateral surface identification symbols</html>");
    
    QLabel* idSymbolColorLabel = new QLabel("ID Color:");
    m_symbolsIdColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::DISABLED,
                                                          CaretColorEnumComboBox::NoneColorModeEnum::DISABLED,
                                                          this);
    m_symbolsIdColorComboBox->setToolTip("<html>Set color of identification symbols shown on surfaces and volumes</html>");
    
    QLabel* contralateralIdSymbolColorLabel = new QLabel("Contralateral:");
    m_symbolsContralateralIdColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::DISABLED,
                                                                       CaretColorEnumComboBox::NoneColorModeEnum::DISABLED,
                                                                       this);
    m_symbolsContralateralIdColorComboBox->setToolTip("<html>Set color of identification symbol on contralateral surface</html>");
    
    QLabel* symbolSizeTypeLabel = new QLabel("Surf/Vol Diameter:");
    
    m_symbolSizeMillimeterRadioButton = new QRadioButton("mm");
    m_symbolSizePercentageRadioButton = new QRadioButton("pct");
    
    QButtonGroup* sizeButtonGroup = new QButtonGroup(this);
    sizeButtonGroup->addButton(m_symbolSizeMillimeterRadioButton,
                               IdentificationSymbolSizeTypeEnum::toIntegerCode(IdentificationSymbolSizeTypeEnum::MILLIMETERS));
    sizeButtonGroup->addButton(m_symbolSizePercentageRadioButton,
                               IdentificationSymbolSizeTypeEnum::toIntegerCode(IdentificationSymbolSizeTypeEnum::PERCENTAGE));
#if QT_VERSION >= 0x060000
        QObject::connect(sizeButtonGroup, &QButtonGroup::idClicked,
                         this, &IdentificationDisplayWidget::symbolSizeTypeButtonIdClicked);
#else
        QObject::connect(sizeButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
                         this, &IdentificationDisplayWidget::symbolSizeTypeButtonIdClicked);
#endif
    
    QString sizeTypeToolTip(IdentificationSymbolSizeTypeEnum::getToolTip("identification"));
    m_symbolSizeMillimeterRadioButton->setToolTip(sizeTypeToolTip);
    m_symbolSizePercentageRadioButton->setToolTip(sizeTypeToolTip);
    
    
    const int spinBoxWidth(70);
    
    QLabel* symbolDiameterLabel = new QLabel("Diameter:");
    m_symbolsMillimetersDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsMillimetersDiameterSpinBox->setRange(0.1, 1000.0);
    m_symbolsMillimetersDiameterSpinBox->setSingleStep(0.1);
    m_symbolsMillimetersDiameterSpinBox->setDecimals(1);
    m_symbolsMillimetersDiameterSpinBox->setSuffix("mm");
    m_symbolsMillimetersDiameterSpinBox->setToolTip("<html>Set millimeter diameter of identification symbols</html>");
    m_symbolsMillimetersDiameterSpinBox->setFixedWidth(spinBoxWidth);
    
    m_symbolsPercentageDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsPercentageDiameterSpinBox->setRange(0.1, 100.0);
    m_symbolsPercentageDiameterSpinBox->setSingleStep(0.1);
    m_symbolsPercentageDiameterSpinBox->setDecimals(1);
    m_symbolsPercentageDiameterSpinBox->setSuffix("%");
    m_symbolsPercentageDiameterSpinBox->setToolTip("<html>Set percentage diameter of identification symbols</html>");
    m_symbolsPercentageDiameterSpinBox->setFixedWidth(spinBoxWidth);
    
    QLabel* mostRecentSymbolDiameterLabel = new QLabel("Most Recent:");
    m_symbolsMillimetersMostRecentDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsMillimetersMostRecentDiameterSpinBox->setRange(0.1, 1000.0);
    m_symbolsMillimetersMostRecentDiameterSpinBox->setSingleStep(0.1);
    m_symbolsMillimetersMostRecentDiameterSpinBox->setDecimals(1);
    m_symbolsMillimetersMostRecentDiameterSpinBox->setSuffix("mm");
    m_symbolsMillimetersMostRecentDiameterSpinBox->setToolTip("<html>Set millimeter diamater of most recent identification symbol</html>");
    m_symbolsMillimetersMostRecentDiameterSpinBox->setFixedWidth(spinBoxWidth);
    
    m_symbolsPercentageMostRecentDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsPercentageMostRecentDiameterSpinBox->setRange(0.1, 100.0);
    m_symbolsPercentageMostRecentDiameterSpinBox->setSingleStep(0.1);
    m_symbolsPercentageMostRecentDiameterSpinBox->setDecimals(1);
    m_symbolsPercentageMostRecentDiameterSpinBox->setSuffix("%");
    m_symbolsPercentageMostRecentDiameterSpinBox->setToolTip("<html>Set percentage diamater of most recent identification symbol</html>");
    m_symbolsPercentageMostRecentDiameterSpinBox->setFixedWidth(spinBoxWidth);
    
    QLabel* histologySymbolLabel = new QLabel("Histology");
    
    m_symbolsHistologyPercentageDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsHistologyPercentageDiameterSpinBox->setRange(0.1, 100.0);
    m_symbolsHistologyPercentageDiameterSpinBox->setSingleStep(0.1);
    m_symbolsHistologyPercentageDiameterSpinBox->setDecimals(1);
    m_symbolsHistologyPercentageDiameterSpinBox->setSuffix("%");
    m_symbolsHistologyPercentageDiameterSpinBox->setToolTip("<html>Set percentage diameter of histology identification symbols</html>");
    m_symbolsHistologyPercentageDiameterSpinBox->setFixedWidth(spinBoxWidth);
    
    m_symbolsHistologyPercentageMostRecentDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsHistologyPercentageMostRecentDiameterSpinBox->setRange(0.1, 100.0);
    m_symbolsHistologyPercentageMostRecentDiameterSpinBox->setSingleStep(0.1);
    m_symbolsHistologyPercentageMostRecentDiameterSpinBox->setDecimals(1);
    m_symbolsHistologyPercentageMostRecentDiameterSpinBox->setSuffix("%");
    m_symbolsHistologyPercentageMostRecentDiameterSpinBox->setToolTip("<html>Set percentage diameter of most recent histology identification symbols</html>");
    m_symbolsHistologyPercentageMostRecentDiameterSpinBox->setFixedWidth(spinBoxWidth);

    QLabel* mediaSymbolLabel = new QLabel("Media");
    
    m_symbolsMediaPercentageDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsMediaPercentageDiameterSpinBox->setRange(0.1, 100.0);
    m_symbolsMediaPercentageDiameterSpinBox->setSingleStep(0.1);
    m_symbolsMediaPercentageDiameterSpinBox->setDecimals(1);
    m_symbolsMediaPercentageDiameterSpinBox->setSuffix("%");
    m_symbolsMediaPercentageDiameterSpinBox->setToolTip("<html>Set percentage diameter of media identification symbols</html>");
    m_symbolsMediaPercentageDiameterSpinBox->setFixedWidth(spinBoxWidth);
    
    m_symbolsMediaPercentageMostRecentDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsMediaPercentageMostRecentDiameterSpinBox->setRange(0.1, 100.0);
    m_symbolsMediaPercentageMostRecentDiameterSpinBox->setSingleStep(0.1);
    m_symbolsMediaPercentageMostRecentDiameterSpinBox->setDecimals(1);
    m_symbolsMediaPercentageMostRecentDiameterSpinBox->setSuffix("%");
    m_symbolsMediaPercentageMostRecentDiameterSpinBox->setToolTip("<html>Set percentage diameter of most recent media identification symbols</html>");
    m_symbolsMediaPercentageMostRecentDiameterSpinBox->setFixedWidth(spinBoxWidth);
    
    WuQValueChangedSignalWatcher* signalWatcher = new WuQValueChangedSignalWatcher(this);
    QObject::connect(signalWatcher, &WuQValueChangedSignalWatcher::valueChanged,
                     this, &IdentificationDisplayWidget::symbolChanged);
    signalWatcher->addObject(m_symbolsShowHistologyCheckBox);
    signalWatcher->addObject(m_symbolsShowMediaCheckbox);
    signalWatcher->addObject(m_symbolsShowSurfaceIdCheckBox);
    signalWatcher->addObject(m_symbolsShowVolumeIdCheckBox);
    signalWatcher->addObject(m_symbolsSurfaceContralateralVertexCheckBox);
    signalWatcher->addObject(m_symbolsIdColorComboBox);
    signalWatcher->addObject(m_symbolsContralateralIdColorComboBox);
    signalWatcher->addObject(m_symbolsMillimetersDiameterSpinBox);
    signalWatcher->addObject(m_symbolsMillimetersMostRecentDiameterSpinBox);
    signalWatcher->addObject(m_symbolsPercentageDiameterSpinBox);
    signalWatcher->addObject(m_symbolsPercentageMostRecentDiameterSpinBox);
    signalWatcher->addObject(m_symbolsHistologyPercentageDiameterSpinBox);
    signalWatcher->addObject(m_symbolsHistologyPercentageMostRecentDiameterSpinBox);
    signalWatcher->addObject(m_symbolsMediaPercentageDiameterSpinBox);
    signalWatcher->addObject(m_symbolsMediaPercentageMostRecentDiameterSpinBox);

    QVBoxLayout* showLayout = new QVBoxLayout();
    showLayout->addWidget(m_symbolsShowHistologyCheckBox);
    showLayout->addWidget(m_symbolsShowMediaCheckbox);
    showLayout->addWidget(m_symbolsShowSurfaceIdCheckBox);
    showLayout->addWidget(m_symbolsShowVolumeIdCheckBox);
    showLayout->addWidget(m_symbolsSurfaceContralateralVertexCheckBox);
    showLayout->addStretch();
    
    QGridLayout* symbolLayout = new QGridLayout();
    int32_t row(0);
    symbolLayout->addWidget(idSymbolColorLabel,
                            row, 0);
    symbolLayout->addWidget(m_symbolsIdColorComboBox->getWidget(),
                            row, 1, 1, 2);
    row++;
    symbolLayout->addWidget(contralateralIdSymbolColorLabel,
                            row, 0);
    symbolLayout->addWidget(m_symbolsContralateralIdColorComboBox->getWidget(),
                            row, 1, 1, 2);
    row++;
    symbolLayout->addWidget(symbolSizeTypeLabel,
                            row, 0);
    symbolLayout->addWidget(m_symbolSizeMillimeterRadioButton,
                            row, 1, Qt::AlignHCenter);
    symbolLayout->addWidget(m_symbolSizePercentageRadioButton,
                            row, 2, Qt::AlignHCenter);
    symbolLayout->addWidget(histologySymbolLabel,
                            row, 3, Qt::AlignHCenter);
    symbolLayout->addWidget(mediaSymbolLabel,
                            row, 4, Qt::AlignHCenter);
    row++;
    symbolLayout->addWidget(symbolDiameterLabel,
                            row, 0);
    symbolLayout->addWidget(m_symbolsMillimetersDiameterSpinBox,
                            row, 1);
    symbolLayout->addWidget(m_symbolsPercentageDiameterSpinBox,
                            row, 2);
    symbolLayout->addWidget(m_symbolsHistologyPercentageDiameterSpinBox,
                            row, 3);
    symbolLayout->addWidget(m_symbolsMediaPercentageDiameterSpinBox,
                            row, 4);
    row++;
    symbolLayout->addWidget(mostRecentSymbolDiameterLabel,
                            row, 0);
    symbolLayout->addWidget(m_symbolsMillimetersMostRecentDiameterSpinBox,
                            row, 1);
    symbolLayout->addWidget(m_symbolsPercentageMostRecentDiameterSpinBox,
                            row, 2);
    symbolLayout->addWidget(m_symbolsHistologyPercentageMostRecentDiameterSpinBox,
                            row, 3);
    symbolLayout->addWidget(m_symbolsMediaPercentageMostRecentDiameterSpinBox,
                            row, 4);
    row++;
    symbolLayout->setRowStretch(row, 100);
    
    Qt::Alignment alignment = Qt::Alignment();
    QWidget* widget = new QWidget();
    QBoxLayout* boxLayout(NULL);
    switch (m_location) {
        case Location::Dialog:
            alignment = Qt::AlignTop;
            boxLayout = new QHBoxLayout(widget);
            break;
        case Location::HorizontalToolBox:
        case Location::VerticalToolBox:
            alignment = Qt::AlignLeft;
            boxLayout = new QVBoxLayout(widget);
            break;
    }
    CaretAssert(boxLayout);
    
    WuQtUtilities::setLayoutSpacingAndMargins(boxLayout, 4, 4);
    boxLayout->addLayout(symbolLayout);
    boxLayout->addSpacing(10);
    boxLayout->addLayout(showLayout);
    boxLayout->addStretch();
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    
    return scrollArea;
}

/**
 * Called when a symbols item is changed
 */
void
IdentificationDisplayWidget::symbolChanged()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    
    info->setShowHistologyIdentificationSymbols(m_symbolsShowHistologyCheckBox->isChecked());
    info->setShowMediaIdentificationSymbols(m_symbolsShowMediaCheckbox->isChecked());
    info->setShowSurfaceIdentificationSymbols(m_symbolsShowSurfaceIdCheckBox->isChecked());
    info->setShowVolumeIdentificationSymbols(m_symbolsShowVolumeIdCheckBox->isChecked());
    info->setContralateralIdentificationEnabled(m_symbolsSurfaceContralateralVertexCheckBox->isChecked());
    info->setIdentificationSymbolColor(m_symbolsIdColorComboBox->getSelectedColor());
    info->setIdentificationContralateralSymbolColor(m_symbolsContralateralIdColorComboBox->getSelectedColor());
    info->setIdentificationSymbolSize(m_symbolsMillimetersDiameterSpinBox->value());
    info->setMostRecentIdentificationSymbolSize(m_symbolsMillimetersMostRecentDiameterSpinBox->value());
    info->setIdentificationSymbolPercentageSize(m_symbolsPercentageDiameterSpinBox->value());
    info->setMostRecentIdentificationSymbolPercentageSize(m_symbolsPercentageMostRecentDiameterSpinBox->value());
    info->setHistologyIdentificationPercentageSymbolSize(m_symbolsHistologyPercentageDiameterSpinBox->value());
    info->setHistologyIdentificationMostRecentPercentageSymbolSize(m_symbolsHistologyPercentageMostRecentDiameterSpinBox->value());
    info->setMediaIdentificationPercentageSymbolSize(m_symbolsMediaPercentageDiameterSpinBox->value());
    info->setMediaIdentificationMostRecentPercentageSymbolSize(m_symbolsMediaPercentageMostRecentDiameterSpinBox->value());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Called when symbol size type combo box changed
 */
void
IdentificationDisplayWidget::symbolSizeTypeButtonIdClicked(int id)
{
    bool validFlag(false);
    const IdentificationSymbolSizeTypeEnum::Enum symbolSizeType(IdentificationSymbolSizeTypeEnum::fromIntegerCode(id,
                                                                                                                  &validFlag));
    if (validFlag) {
        
    }
    else {
        const AString msg("Invalid identification symbol size id="
                          + AString::number(id));
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
    
    
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    info->setIdentificationSymbolSizeType(symbolSizeType);
    updateSymbolsWidget();
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}


/**
 * Save information specific to this type of model to the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    saving the scene.
 *
 * @param instanceName
 *    Name of instance in the scene.
 */
SceneClass*
IdentificationDisplayWidget::saveToScene(const SceneAttributes* sceneAttributes,
                                         const AString& instanceName)
{
    SceneClass* sceneClass = new SceneClass(instanceName,
                                            "IdentificationDisplayWidget",
                                            1);
    m_sceneAssistant->saveMembers(sceneAttributes,
                                  sceneClass);
    
    // Uncomment if sub-classes must save to scene
    //saveSubClassDataToScene(sceneAttributes,
    //                        sceneClass);
    
    return sceneClass;
}

/**
 * Restore information specific to the type of model from the scene.
 *
 * @param sceneAttributes
 *    Attributes for the scene.  Scenes may be of different types
 *    (full, generic, etc) and the attributes should be checked when
 *    restoring the scene.
 *
 * @param sceneClass
 *     sceneClass from which model specific information is obtained.
 */
void
IdentificationDisplayWidget::restoreFromScene(const SceneAttributes* sceneAttributes,
                                              const SceneClass* sceneClass)
{
    if (sceneClass == NULL) {
        return;
    }
    
    m_sceneAssistant->restoreMembers(sceneAttributes,
                                     sceneClass);
    
    //Uncomment if sub-classes must restore from scene
    //restoreSubClassDataFromScene(sceneAttributes,
    //                             sceneClass);
    
}

/**
 * @return Instance of the line layer chart symbols widget
 */
QWidget*
IdentificationDisplayWidget::createChartLineLayerSymbolsWidget()
{
    QLabel* symbolDiameterLabel = new QLabel("Symbol Diameter:");
    m_chartLineLayerSymbolSizeSpinBox = new QDoubleSpinBox();
    m_chartLineLayerSymbolSizeSpinBox->setRange(0.1, 1000.0);
    m_chartLineLayerSymbolSizeSpinBox->setSingleStep(0.1);
    m_chartLineLayerSymbolSizeSpinBox->setDecimals(1);
    m_chartLineLayerSymbolSizeSpinBox->setSuffix("%");
    m_chartLineLayerSymbolSizeSpinBox->setToolTip("Size of selected chart lines point symbol as a percentage\n"
                                                  "of viewport height");
    
    QLabel* toolTipTextSizeLabel = new QLabel("Text Height:");
    m_chartLineLayerToolTipTextSizeSpinBox = new QDoubleSpinBox();
    m_chartLineLayerToolTipTextSizeSpinBox->setRange(0.1, 1000.0);
    m_chartLineLayerToolTipTextSizeSpinBox->setSingleStep(0.1);
    m_chartLineLayerToolTipTextSizeSpinBox->setDecimals(1);
    m_chartLineLayerToolTipTextSizeSpinBox->setSuffix("%");
    m_chartLineLayerToolTipTextSizeSpinBox->setToolTip("Size of selected chart lines point tooltip text as a\n"
                                                       "percentage of viewport height");
    
    WuQValueChangedSignalWatcher* signalWatcher = new WuQValueChangedSignalWatcher(this);
    QObject::connect(signalWatcher, &WuQValueChangedSignalWatcher::valueChanged,
                     this, &IdentificationDisplayWidget::chartLineLayerSymbolChanged);
    signalWatcher->addObject(m_chartLineLayerSymbolSizeSpinBox);
    signalWatcher->addObject(m_chartLineLayerToolTipTextSizeSpinBox);
    
    QGridLayout* symbolLayout = new QGridLayout();
    int32_t row(0);
    symbolLayout->addWidget(symbolDiameterLabel,
                            row, 0);
    symbolLayout->addWidget(m_chartLineLayerSymbolSizeSpinBox,
                            row, 1);
    row++;
    symbolLayout->addWidget(toolTipTextSizeLabel,
                            row, 0);
    symbolLayout->addWidget(m_chartLineLayerToolTipTextSizeSpinBox,
                            row, 1);
    row++;
    
    Qt::Alignment alignment = Qt::Alignment();
    QWidget* widget = new QWidget();
    QBoxLayout* boxLayout(NULL);
    switch (m_location) {
        case Location::Dialog:
            alignment = Qt::AlignTop;
            boxLayout = new QHBoxLayout(widget);
            break;
        case Location::HorizontalToolBox:
        case Location::VerticalToolBox:
            alignment = Qt::AlignLeft;
            boxLayout = new QVBoxLayout(widget);
            break;
    }
    CaretAssert(boxLayout);
    
    WuQtUtilities::setLayoutSpacingAndMargins(boxLayout, 4, 4);
    boxLayout->addLayout(symbolLayout);
    boxLayout->addStretch();
    
    QScrollArea* scrollArea = new QScrollArea();
    scrollArea->setWidget(widget);
    
    return scrollArea;
}

/**
 * Called when a symbols item is changed
 */
void
IdentificationDisplayWidget::chartLineLayerSymbolChanged()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    
    info->setChartLineLayerSymbolSize(m_chartLineLayerSymbolSizeSpinBox->value());
    info->setChartLineLayerToolTipTextSize(m_chartLineLayerToolTipTextSizeSpinBox->value());
    EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
}

/**
 * Update the chart line layer symbols widget
 */
void
IdentificationDisplayWidget::updateChartLineLayerSymbolsWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    
    QSignalBlocker symbolBlocker(m_chartLineLayerSymbolSizeSpinBox);
    m_chartLineLayerSymbolSizeSpinBox->setValue(info->getChartLineLayerSymbolSize());
    QSignalBlocker toolTipBlocker(m_chartLineLayerToolTipTextSizeSpinBox);
    m_chartLineLayerToolTipTextSizeSpinBox->setValue(info->getChartLineLayerToolTipTextSize());
}





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
#include "EventGraphicsUpdateAllWindows.h"
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
 * @parent
 *  The parent widget
 */
IdentificationDisplayWidget::IdentificationDisplayWidget(QWidget* parent)
: QWidget(parent)
{
    m_infoWidget = createInfoWidget();
    
    m_filteringWidget = createFilteringWidget();
    
    m_symbolsWidget = createsymbolsWidget();
    
    m_tabWidget = new QTabWidget();
    m_tabWidget->addTab(m_infoWidget,      "Information");
    m_tabWidget->addTab(m_filteringWidget, "Filtering");
    m_tabWidget->addTab(m_symbolsWidget,   "Symbols");
    
    m_sceneAssistant = std::unique_ptr<SceneClassAssistant>(new SceneClassAssistant());
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(m_tabWidget, 100);
    
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_UPDATE_INFORMATION_WINDOWS);
    EventManager::get()->addEventListener(this, EventTypeEnum::EVENT_USER_INTERFACE_UPDATE);
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
        EventUserInterfaceUpdate* uiUpdateEvent =
        dynamic_cast<EventUserInterfaceUpdate*>(event);
        CaretAssert(uiUpdateEvent);
        uiUpdateEvent->setEventProcessed();

        updateContent(false);
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
    copyHistoryToolButton->setToolTip("Copy the SELECTED information text to the Clipboard");
    QObject::connect(copyHistoryToolButton, &QToolButton::clicked,
                     m_infoTextBrowser, &QTextBrowser::copy);
    QObject::connect(m_infoTextBrowser, &QTextBrowser::copyAvailable,
                     copyHistoryToolButton, &QToolButton::setEnabled);
    copyHistoryToolButton->setEnabled(false);
    
    QToolButton* removeSymbolsButton = new QToolButton();
    removeSymbolsButton->setText("Remove Symbols");
    removeSymbolsButton->setToolTip("Remove identification symbols on surfaces and volume slices");
    QObject::connect(removeSymbolsButton, &QToolButton::clicked,
                     this, &IdentificationDisplayWidget::infoRemoveSymbolsButtonClicked);
    
    const QString showToolTip(WuQtUtilities::createWordWrappedToolTipText("Selects number of identification operations displayed in the information text"));
    QLabel* showLastHistoryCountLabel = new QLabel("Show");
    showLastHistoryCountLabel->setToolTip(showToolTip);
    m_infoShowHistoryCountSpinBox = new QSpinBox();
    m_infoShowHistoryCountSpinBox->setRange(0, 10);
    m_infoShowHistoryCountSpinBox->setSingleStep(1);
    m_infoShowHistoryCountSpinBox->setSpecialValueText("All");
    m_infoShowHistoryCountSpinBox->setToolTip(showToolTip);
    QObject::connect(m_infoShowHistoryCountSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &IdentificationDisplayWidget::infoShowHistoryCountSpinBoxValueChanged);
    
    QGroupBox* historyGroupBox = new QGroupBox("History");
    QGridLayout* historyLayout = new QGridLayout(historyGroupBox);
    WuQtUtilities::setLayoutSpacingAndMargins(historyLayout, 5, 6);
    int32_t row(0);
    historyLayout->addWidget(clearHistoryToolButton, row, 0);
    historyLayout->addWidget(copyHistoryToolButton, row, 1);
    row++;
    historyLayout->addWidget(showLastHistoryCountLabel, row, 0, Qt::AlignRight);
    historyLayout->addWidget(m_infoShowHistoryCountSpinBox, row, 1, Qt::AlignLeft);
    row++;
    
    QVBoxLayout* rightLayout = new QVBoxLayout();
    WuQtUtilities::setLayoutSpacingAndMargins(rightLayout, 0, 2);
    rightLayout->addWidget(historyGroupBox);
    rightLayout->addSpacing(15);
    rightLayout->addWidget(removeSymbolsButton, 0, Qt::AlignHCenter);
    rightLayout->addStretch();
    
    QWidget* widget = new QWidget();
    QHBoxLayout* layout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, layout->spacing(), 2);
    layout->addWidget(m_infoTextBrowser, 100);
    layout->addLayout(rightLayout, 0);
    
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
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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
 * @return Instance of the filtering widget
 */
QWidget*
IdentificationDisplayWidget::createFilteringWidget()
{
    QWidget* tabFilterWidget = new QWidget(); //new QGroupBox("Overlays Identification");
    QVBoxLayout* tabFilterLayout = new QVBoxLayout(tabFilterWidget);
    m_tabFilterButtonGroup = new QButtonGroup(this);
    QObject::connect(m_tabFilterButtonGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
                     this, &IdentificationDisplayWidget::tabFilterRadioButtonClicked);
    
    std::vector<IdentificationFilterTabSelectionEnum::Enum> tabFilterEnums;
    IdentificationFilterTabSelectionEnum::getAllEnums(tabFilterEnums);
    for (auto tf : tabFilterEnums) {
        QRadioButton* rb = new QRadioButton(IdentificationFilterTabSelectionEnum::toGuiName(tf));
        rb->setToolTip(IdentificationFilterTabSelectionEnum::toToolTip(tf));
        /* second argument is integer ID that encodes the integer for tab filtering enum */
        m_tabFilterButtonGroup->addButton(rb,
                                          IdentificationFilterTabSelectionEnum::toIntegerCode(tf));
        tabFilterLayout->addWidget(rb);
    }
    tabFilterLayout->addStretch();
    
    m_filteringCiftiLoadingCheckBox = new QCheckBox("Show CIFTI Loading Row/Column");
    
    m_filteringBorderCheckBox = new QCheckBox("Show Border Identification");
    
    m_filteringFociCheckBox  = new QCheckBox("Show Foci Identification");
    
    WuQValueChangedSignalWatcher* signalWatcher = new WuQValueChangedSignalWatcher(this);
    QObject::connect(signalWatcher, &WuQValueChangedSignalWatcher::valueChanged,
                     this, &IdentificationDisplayWidget::filteringChanged);
    signalWatcher->addObject(m_filteringCiftiLoadingCheckBox);
    signalWatcher->addObject(m_filteringBorderCheckBox);
    signalWatcher->addObject(m_filteringFociCheckBox);

    QWidget* showDataWidget = new QWidget(); //new QGroupBox("Show Data");
    QVBoxLayout* showLayout = new QVBoxLayout(showDataWidget);
    showLayout->addWidget(m_filteringCiftiLoadingCheckBox);
    showLayout->addWidget(m_filteringBorderCheckBox);
    showLayout->addWidget(m_filteringFociCheckBox);
    showLayout->addStretch();
    
    m_fileFilteringTableWidget = new IdentificationFileFilteringTableWidget();
    
    QTabWidget* tabWidget = new QTabWidget();
    tabWidget->addTab(showDataWidget, "Data");
    tabWidget->addTab(m_fileFilteringTableWidget, "Files");
    tabWidget->addTab(tabFilterWidget, "Overlays");
    tabWidget->setCurrentWidget(m_fileFilteringTableWidget);
    
    return tabWidget;
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
}

/**
 * Update the symbols widget's content
 */
void
IdentificationDisplayWidget::updateSymbolsWidget()
{
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    
    m_symbolsShowSurfaceIdCheckBox->setChecked(info->isShowSurfaceIdentificationSymbols());
    m_symbolsShowVolumeIdCheckBox->setChecked(info->isShowVolumeIdentificationSymbols());
    m_symbolsSurfaceContralateralVertexCheckBox->setChecked(info->isContralateralIdentificationEnabled());

    m_symbolsIdColorComboBox->setSelectedColor(info->getIdentificationSymbolColor());
    m_symbolsContralateralIdColorComboBox->setSelectedColor(info->getIdentificationContralateralSymbolColor());
    QSignalBlocker symbolSizeBlocker(m_symbolsIdDiameterSpinBox);
    m_symbolsIdDiameterSpinBox->setValue(info->getIdentificationSymbolSize());
    QSignalBlocker recentSymbolSizeBlocker(m_symbolsMostRecentIdDiameterSpinBox);
    m_symbolsMostRecentIdDiameterSpinBox->setValue(info->getMostRecentIdentificationSymbolSize());
}

/**
 * @return Instance of the symbols widget
 */
QWidget*
IdentificationDisplayWidget::createsymbolsWidget()
{
    m_symbolsShowSurfaceIdCheckBox = new QCheckBox("Show Surface ID Symbols");

    m_symbolsShowVolumeIdCheckBox = new QCheckBox("Show Volume ID Symbols");
    
    m_symbolsSurfaceContralateralVertexCheckBox = new QCheckBox("Show Surface Contralateral Vertex");
    
    QLabel* idSymbolColorLabel = new QLabel("ID Symbol Color:");
    m_symbolsIdColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::DISABLED,
                                                                   CaretColorEnumComboBox::NoneColorModeEnum::DISABLED,
                                                                   this);
    
    QLabel* contralateralIdSymbolColorLabel = new QLabel("Contralateral ID Symbol Color:");
    m_symbolsContralateralIdColorComboBox = new CaretColorEnumComboBox(CaretColorEnumComboBox::CustomColorModeEnum::DISABLED,
                                                                                CaretColorEnumComboBox::NoneColorModeEnum::DISABLED,
                                                                                this);
    
    QLabel* symbolDiameterLabel = new QLabel("ID Symbol Diameter:");
    m_symbolsIdDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsIdDiameterSpinBox->setRange(0.1, 1000.0);
    m_symbolsIdDiameterSpinBox->setSingleStep(0.1);
    m_symbolsIdDiameterSpinBox->setDecimals(1);
    m_symbolsIdDiameterSpinBox->setSuffix("mm");
    
    QLabel* mostRecentSymbolDiameterLabel = new QLabel("Most Recent ID Symbol Diameter:");
    m_symbolsMostRecentIdDiameterSpinBox = new QDoubleSpinBox();
    m_symbolsMostRecentIdDiameterSpinBox->setRange(0.1, 1000.0);
    m_symbolsMostRecentIdDiameterSpinBox->setSingleStep(0.1);
    m_symbolsMostRecentIdDiameterSpinBox->setDecimals(1);
    m_symbolsMostRecentIdDiameterSpinBox->setSuffix("mm");
    
    WuQValueChangedSignalWatcher* signalWatcher = new WuQValueChangedSignalWatcher(this);
    QObject::connect(signalWatcher, &WuQValueChangedSignalWatcher::valueChanged,
                     this, &IdentificationDisplayWidget::symbolChanged);
    signalWatcher->addObject(m_symbolsShowSurfaceIdCheckBox);
    signalWatcher->addObject(m_symbolsShowVolumeIdCheckBox);
    signalWatcher->addObject(m_symbolsSurfaceContralateralVertexCheckBox);
    signalWatcher->addObject(m_symbolsIdColorComboBox);
    signalWatcher->addObject(m_symbolsContralateralIdColorComboBox);
    signalWatcher->addObject(m_symbolsIdDiameterSpinBox);
    signalWatcher->addObject(m_symbolsMostRecentIdDiameterSpinBox);

    QVBoxLayout* rightLayout = new QVBoxLayout();
    rightLayout->addWidget(m_symbolsShowSurfaceIdCheckBox);
    rightLayout->addWidget(m_symbolsShowVolumeIdCheckBox);
    rightLayout->addWidget(m_symbolsSurfaceContralateralVertexCheckBox);
    rightLayout->addStretch();

    QGridLayout* leftLayout = new QGridLayout();
    int32_t row(0);
    leftLayout->addWidget(idSymbolColorLabel,
                      row, 0);
    leftLayout->addWidget(m_symbolsIdColorComboBox->getWidget(),
                      row, 1);
    row++;
    leftLayout->addWidget(contralateralIdSymbolColorLabel,
                      row, 0);
    leftLayout->addWidget(m_symbolsContralateralIdColorComboBox->getWidget(),
                      row, 1);
    row++;
    leftLayout->addWidget(symbolDiameterLabel,
                      row, 0);
    leftLayout->addWidget(m_symbolsIdDiameterSpinBox,
                      row, 1);
    row++;
    leftLayout->addWidget(mostRecentSymbolDiameterLabel,
                      row, 0);
    leftLayout->addWidget(m_symbolsMostRecentIdDiameterSpinBox,
                      row, 1);
    row++;
    leftLayout->setRowStretch(row, 100);

    QWidget* widget = new QWidget();
    QHBoxLayout* boxLayout = new QHBoxLayout(widget);
    WuQtUtilities::setLayoutSpacingAndMargins(boxLayout, 0, 0);
    boxLayout->addLayout(leftLayout);
    boxLayout->addSpacing(10);
    boxLayout->addLayout(rightLayout);
    boxLayout->addStretch();
    return widget;
}

/**
 * Called when a symbols item is changed
 */
void
IdentificationDisplayWidget::symbolChanged()
{
    std::cout << "symbols changed" << std::endl;
    
    Brain* brain = GuiManager::get()->getBrain();
    IdentificationManager* info = brain->getIdentificationManager();
    
    info->setShowSurfaceIdentificationSymbols(m_symbolsShowSurfaceIdCheckBox->isChecked());
    info->setShowVolumeIdentificationSymbols(m_symbolsShowVolumeIdCheckBox->isChecked());
    info->setContralateralIdentificationEnabled(m_symbolsSurfaceContralateralVertexCheckBox->isChecked());
    info->setIdentificationSymbolColor(m_symbolsIdColorComboBox->getSelectedColor());
    info->setIdentificationContralateralSymbolColor(m_symbolsContralateralIdColorComboBox->getSelectedColor());
    info->setIdentificationSymbolSize(m_symbolsIdDiameterSpinBox->value());
    info->setMostRecentIdentificationSymbolSize(m_symbolsMostRecentIdDiameterSpinBox->value());
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
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


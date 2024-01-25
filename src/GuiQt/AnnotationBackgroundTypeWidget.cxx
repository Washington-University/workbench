
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

#define __ANNOTATION_BACKGROUND_TYPE_WIDGET_DECLARE__
#include "AnnotationBackgroundTypeWidget.h"
#undef __ANNOTATION_BACKGROUND_TYPE_WIDGET_DECLARE__

#include <QGridLayout>
#include <QLabel>
#include <QSpinBox>

#include "AnnotationBrowserTab.h"
#include "AnnotationManager.h"
#include "AnnotationRedoUndoCommand.h"
#include "Brain.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventGraphicsPaintSoonAllWindows.h"
#include "EventManager.h"
#include "GuiManager.h"
#include "ModelChartTwo.h"
#include "TileTabsLayoutBackgroundTypeEnum.h"
#include "TileTabsManualTabGeometryWidget.h"
#include "WuQMessageBox.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationBackgroundTypeWidget
 * \brief Widget for editing annotation coordinate
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param
 * @param whichCoordinate
 *     Which coordinate, one (or only), or two
 * @param browserWindowIndex
 *     Index of browser window
 * @param parent
 *     Parent widget
 */
AnnotationBackgroundTypeWidget::AnnotationBackgroundTypeWidget(const UserInputModeEnum::Enum userInputMode,
                                                       const AnnotationWidgetParentEnum::Enum parentWidgetType,
                                                       const int32_t browserWindowIndex,
                                                       QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_parentWidgetType(parentWidgetType),
m_browserWindowIndex(browserWindowIndex)
{
    QLabel* backgroundLabel(NULL);
    switch (m_parentWidgetType) {
        case AnnotationWidgetParentEnum::ANNOTATION_TOOL_BAR_WIDGET:
            break;
        case AnnotationWidgetParentEnum::PARENT_ENUM_FOR_LATER_USE:
            backgroundLabel = new QLabel("Background");
            CaretAssert(0);
            break;
    }
    
    m_TileTabsLayoutBackgroundTypeEnumComboBox = new EnumComboBoxTemplate(this);
    m_TileTabsLayoutBackgroundTypeEnumComboBox->setup<TileTabsLayoutBackgroundTypeEnum,TileTabsLayoutBackgroundTypeEnum::Enum>();
    QObject::connect(m_TileTabsLayoutBackgroundTypeEnumComboBox, SIGNAL(itemActivated()),
                     this, SLOT(tileTabsLayoutBackgroundTypeEnumComboBoxItemActivated()));

    QLabel* stackingOrderLabel = new QLabel("Order:");
    m_stackingOrderSpinBox = new QSpinBox();
    m_stackingOrderSpinBox->setRange(0, 1000);
    m_stackingOrderSpinBox->setSingleStep(1);
    m_stackingOrderSpinBox->setToolTip(TileTabsManualTabGeometryWidget::getStackOrderToolTipText());
    QObject::connect(m_stackingOrderSpinBox, QOverload<int>::of(&QSpinBox::valueChanged),
                     this, &AnnotationBackgroundTypeWidget::stackingOrderValueChanged);                                                                
    
    QGridLayout* layout = new QGridLayout(this);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 100);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    int32_t row(0);
    if (backgroundLabel != NULL) {
        layout->addWidget(backgroundLabel,
                          row, 0, 1, 2, Qt::AlignHCenter);
        row++;
    }
    layout->addWidget(m_TileTabsLayoutBackgroundTypeEnumComboBox->getWidget(),
                      row, 0, 1, 2);
    row++;
    layout->addWidget(stackingOrderLabel,
                      row, 0);
    layout->addWidget(m_stackingOrderSpinBox,
                      row, 1);

    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationBackgroundTypeWidget::~AnnotationBackgroundTypeWidget()
{
}

/**
 * Update with the given annotation coordinate.
 *
 * @param coordinate.
 */
void
AnnotationBackgroundTypeWidget::updateContent(std::vector<AnnotationBrowserTab*>& selectedBrowserTabAnnotations)
{
    m_annotationBrowserTabs = selectedBrowserTabAnnotations;
    
    bool firstFlag(true);
    int32_t firstOrderValue(false);
    bool haveMultipleStackingOrderValuesFlag(false);
    bool haveMultipleBackgroundValuesFlag(false);
    TileTabsLayoutBackgroundTypeEnum::Enum firstBackgroundValue = TileTabsLayoutBackgroundTypeEnum::OPAQUE_BG;
    
    if (! m_annotationBrowserTabs.empty()) {
        for (auto ann : m_annotationBrowserTabs) {
            if (firstFlag) {
                firstBackgroundValue = ann->getBackgroundType();
                firstFlag = false;
                firstOrderValue = ann->getStackingOrder();
            }
            else {
                if (firstBackgroundValue != ann->getBackgroundType()) {
                    haveMultipleBackgroundValuesFlag = true;
                }
                if (firstOrderValue != ann->getStackingOrder()) {
                    haveMultipleStackingOrderValuesFlag = true;
                }
            }
        }
        setEnabled(true);
    }
    else {
        setEnabled(false);
    }
    
    m_TileTabsLayoutBackgroundTypeEnumComboBox->setSelectedItem<TileTabsLayoutBackgroundTypeEnum,TileTabsLayoutBackgroundTypeEnum::Enum>(firstBackgroundValue);
    
    const int32_t opaqueIndex = TileTabsLayoutBackgroundTypeEnum::toIntegerCode(TileTabsLayoutBackgroundTypeEnum::OPAQUE_BG);
    AString opaqueText(TileTabsLayoutBackgroundTypeEnum::toGuiName(TileTabsLayoutBackgroundTypeEnum::OPAQUE_BG));
    const int32_t transparentIndex = TileTabsLayoutBackgroundTypeEnum::toIntegerCode(TileTabsLayoutBackgroundTypeEnum::TRANSPARENT_BG);
    AString transparentText(TileTabsLayoutBackgroundTypeEnum::toGuiName(TileTabsLayoutBackgroundTypeEnum::TRANSPARENT_BG));
    
    if (haveMultipleBackgroundValuesFlag) {
        switch (firstBackgroundValue) {
            case TileTabsLayoutBackgroundTypeEnum::OPAQUE_BG:
                opaqueText.append("+");
                break;
            case TileTabsLayoutBackgroundTypeEnum::TRANSPARENT_BG:
                transparentText.append("+");
                break;
        }
    }
    
    /*
     * Need to block signals as 'setValue' causes signal change
     */
    QSignalBlocker spinBlocker(m_stackingOrderSpinBox);
    m_stackingOrderSpinBox->setValue(firstOrderValue);
    if (haveMultipleStackingOrderValuesFlag) {
        m_stackingOrderSpinBox->setSuffix("+");
    }
    else {
        m_stackingOrderSpinBox->setSuffix("");
    }
    
    m_TileTabsLayoutBackgroundTypeEnumComboBox->getComboBox()->setItemText(opaqueIndex,
                                                                           opaqueText);
    m_TileTabsLayoutBackgroundTypeEnumComboBox->getComboBox()->setItemText(transparentIndex,
                                                                           transparentText);
}

/**
 * Called when background type is changed
 */
void
AnnotationBackgroundTypeWidget::tileTabsLayoutBackgroundTypeEnumComboBoxItemActivated()
{
    if ( ! m_annotationBrowserTabs.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        std::vector<Annotation*> annotations(m_annotationBrowserTabs.begin(),
                                             m_annotationBrowserTabs.end());
        
        const TileTabsLayoutBackgroundTypeEnum::Enum newValue = m_TileTabsLayoutBackgroundTypeEnumComboBox->getSelectedItem<TileTabsLayoutBackgroundTypeEnum,TileTabsLayoutBackgroundTypeEnum::Enum>();
        undoCommand->setBrowserTabBackground(newValue, annotations);

        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING);
        
        AString errorMessage;
        if ( ! annMan->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        
        /*
         * Need to update since the annotations may have had different backgrounds
         * and need to remove the "+" symbol
         */
        std::vector<AnnotationBrowserTab*> annCopy = m_annotationBrowserTabs;
        updateContent(annCopy);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    }
}

/**
 * Called when stacking order value changed for one browser tab annotation
 *
 * @param value
 *     New stacking order value
 */
void
AnnotationBackgroundTypeWidget::stackingOrderValueChanged(int value)
{
    if ( ! m_annotationBrowserTabs.empty()) {
        AnnotationRedoUndoCommand* undoCommand = new AnnotationRedoUndoCommand();
        std::vector<Annotation*> annotations(m_annotationBrowserTabs.begin(),
                                             m_annotationBrowserTabs.end());
        
        undoCommand->setModeStackingOrderBrowserTab(value, annotations);
        
        AnnotationManager* annMan = GuiManager::get()->getBrain()->getAnnotationManager(UserInputModeEnum::Enum::TILE_TABS_LAYOUT_EDITING);
        
        AString errorMessage;
        if ( ! annMan->applyCommand(undoCommand,
                                    errorMessage)) {
            WuQMessageBox::errorOk(this,
                                   errorMessage);
        }
        
        /*
         * Need to update since the annotations may have had different stacking orders
         * and need to remove the "+" symbol
         */
        std::vector<AnnotationBrowserTab*> annCopy = m_annotationBrowserTabs;
        updateContent(annCopy);
        EventManager::get()->sendEvent(EventGraphicsPaintSoonAllWindows().getPointer());
        EventManager::get()->sendSimpleEvent(EventTypeEnum::EVENT_ANNOTATION_TOOLBAR_UPDATE);
    }
}



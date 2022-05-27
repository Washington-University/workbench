
/*LICENSE_START*/
/*
 *  Copyright (C) 2022 Washington University School of Medicine
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

#define __VOLUME_OBLIQUE_OPTIONS_WIDGET_DECLARE__
#include "VolumeObliqueOptionsWidget.h"
#undef __VOLUME_OBLIQUE_OPTIONS_WIDGET_DECLARE__

#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>
#include <QLabel>
#include <QVBoxLayout>

#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EnumComboBoxTemplate.h"
#include "EventBrowserTabGet.h"
#include "EventGraphicsUpdateAllWindows.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "VolumeObliqueOptionsWidget.h"
#include "WuQMacroManager.h"
using namespace caret;

/**
 * \class caret::VolumeObliqueOptionsWidget
 * \brief Widget containing Volume MPR Settings
 * \ingroup GuiQt
 */

/**
 * Constructor.
 */
VolumeObliqueOptionsWidget::VolumeObliqueOptionsWidget(const QString& objectNamePrefix,
                                                 QWidget* parent)
: QWidget(parent),
m_objectNamePrefix(objectNamePrefix
                   + ":ObliqueOptionsWidget")
{
    setObjectName(m_objectNamePrefix);

    VolumeSliceInterpolationEdgeEffectsMaskingEnum::getAllEnums(m_maskingEnums);
    
    QButtonGroup* maskingButtonGroup(new QButtonGroup(this));
    maskingButtonGroup->setExclusive(true);
    QObject::connect(maskingButtonGroup, &QButtonGroup::buttonClicked,
                     this, &VolumeObliqueOptionsWidget::maskingButtonClicked);

    QVBoxLayout* buttonLayout(new QVBoxLayout());
    for (auto maskEnum : m_maskingEnums) {
        const QString text(VolumeSliceInterpolationEdgeEffectsMaskingEnum::toGuiName(maskEnum));
        QRadioButton* radioButton(new QRadioButton(text));
        radioButton->setObjectName(m_objectNamePrefix
                                   + ":"
                                   + VolumeSliceInterpolationEdgeEffectsMaskingEnum::toName(maskEnum));
        radioButton->setToolTip(VolumeSliceInterpolationEdgeEffectsMaskingEnum::toToolTip(maskEnum));
        WuQMacroManager::instance()->addMacroSupportToObject(radioButton,
                                                             "Select " + text + " oblique sampling");
        maskingButtonGroup->addButton(radioButton);
        m_maskingRadioButtons.push_back(radioButton);
        buttonLayout->addWidget(radioButton);
    }
    
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addLayout(buttonLayout);

    //    EventManager::get()->addEventListener(this, EventTypeEnum::);
}

/**
 * Destructor.
 */
VolumeObliqueOptionsWidget::~VolumeObliqueOptionsWidget()
{
    EventManager::get()->removeAllEventsFromListener(this);
}

/**
 * Update the selected graphics window.
 */
void
VolumeObliqueOptionsWidget::updateGraphicsWindow()
{
    EventManager::get()->sendEvent(EventGraphicsUpdateAllWindows().getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

/**
 * @return Browser tab content for current tab index or NULL if invalid tab index
 */
BrowserTabContent*
VolumeObliqueOptionsWidget::getBrowserTabContent()
{
    EventBrowserTabGet tabEvent(m_tabIndex);
    EventManager::get()->sendEvent(tabEvent.getPointer());
    BrowserTabContent* tabContent(tabEvent.getBrowserTab());
    return tabContent;
}


/**
 * Update the content in the dialog
 * @param browserWindowIndexIn
 *    Index of the browser window.
 */
void
VolumeObliqueOptionsWidget::updateContent(const int32_t tabIndex)
{
    setEnabled(false);
    
    m_tabIndex = tabIndex;
    
    BrowserTabContent* btc = getBrowserTabContent();
    if (btc == NULL) {
        return;
    }
    
    const VolumeSliceInterpolationEdgeEffectsMaskingEnum::Enum maskingType(btc->getVolumeSliceInterpolationEdgeEffectsMaskingType());
    
    CaretAssert(m_maskingEnums.size() == m_maskingRadioButtons.size());
    const int32_t numButtons = static_cast<int32_t>(m_maskingRadioButtons.size());
    for (int32_t i = 0; i < numButtons; i++) {
        CaretAssertVectorIndex(m_maskingEnums, i);
        if (maskingType == m_maskingEnums[i]) {
            CaretAssertVectorIndex(m_maskingRadioButtons, i);
            m_maskingRadioButtons[i]->setChecked(true);
        }
    }
    
    setEnabled(true);
}

/**
 * Called when masking button is clicked
 */
void
VolumeObliqueOptionsWidget::maskingButtonClicked(QAbstractButton*)
{
    BrowserTabContent* btc(getBrowserTabContent());
    if (btc == NULL) {
        return;
    }
    
    CaretAssert(m_maskingEnums.size() == m_maskingRadioButtons.size());
    const int32_t numButtons = static_cast<int32_t>(m_maskingRadioButtons.size());
    for (int32_t i = 0; i < numButtons; i++) {
        CaretAssertVectorIndex(m_maskingRadioButtons, i);
        if (m_maskingRadioButtons[i]->isChecked()) {
            CaretAssertVectorIndex(m_maskingEnums, i);
            btc->setVolumeSliceInterpolationEdgeEffectsMaskingType(m_maskingEnums[i]);
            break;
        }
    }

    updateGraphicsWindow();
}

/**
 * Receive event
 * @param event
 *    The event
 */
void
VolumeObliqueOptionsWidget::receiveEvent(Event* event)
{
    
}

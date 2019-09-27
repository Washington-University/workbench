
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

#define __ANNOTATION_NAME_WIDGET_DECLARE__
#include "AnnotationNameWidget.h"
#undef __ANNOTATION_NAME_WIDGET_DECLARE__

#include <QCheckBox>
#include <QLabel>
#include <QVBoxLayout>

#include "AnnotationBrowserTab.h"
#include "AnnotationMenuArrange.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
#include "EventGraphicsUpdateOneWindow.h"
#include "EventManager.h"
#include "EventUserInterfaceUpdate.h"
#include "WuQtUtilities.h"

using namespace caret;

/**
 * \class caret::AnnotationNameWidget
 * \brief Widget for formatting annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param userInputMode
 *     The user input mode
 * @param browserWindowIndex
 *     Index of the browser window.
 * @param parent
 *     Parent of this widget.
 */
AnnotationNameWidget::AnnotationNameWidget(const UserInputModeEnum::Enum userInputMode,
                                               const int32_t browserWindowIndex,
                                               QWidget* parent)
: QWidget(parent),
m_userInputMode(userInputMode),
m_browserWindowIndex(browserWindowIndex)
{
    m_nameLabel = new QLabel();
    
    m_visibilityCheckBox = NULL;
    if (m_userInputMode == UserInputModeEnum::TILE_TABS_MANUAL_LAYOUT_EDITING) {
        m_visibilityCheckBox = new QCheckBox("Draw\nContent");
        m_visibilityCheckBox->setTristate(false);
        QObject::connect(m_visibilityCheckBox, &QCheckBox::stateChanged,
                         this, &AnnotationNameWidget::visibilityCheckStateChanged);
    }
    
    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->setContentsMargins(2, 2, 2, 2);
    layout->addWidget(m_nameLabel, 0, Qt::AlignLeft);
    if (m_visibilityCheckBox != NULL) {
        layout->addWidget(m_visibilityCheckBox, 0, Qt::AlignLeft);
    }
    layout->addStretch();
    
    setSizePolicy(QSizePolicy::Fixed,
                  QSizePolicy::Fixed);
}

/**
 * Destructor.
 */
AnnotationNameWidget::~AnnotationNameWidget()
{
}

/**
 * Update with the given annotation.
 *
 * @param annotations
 *     The selected annotations
 */
void
AnnotationNameWidget::updateContent(const std::vector<Annotation*>& annotations)
{
    m_browserTabAnnotations.clear();
    for (auto ann : annotations) {
        if (ann != NULL) {
            AnnotationBrowserTab* abt = dynamic_cast<AnnotationBrowserTab*>(ann);
            if (abt != NULL) {
                m_browserTabAnnotations.push_back(abt);
            }
        }
    }
    
    int32_t visibleCount(0);
    int32_t hiddenCount(0);
    AString nameText;
    const int32_t numberOfAnnotations = static_cast<int32_t>(m_browserTabAnnotations.size());
    
    for (auto browserTabAnnotation : m_browserTabAnnotations) {
        CaretAssert(browserTabAnnotation);
        const BrowserTabContent* tabContent = browserTabAnnotation->getBrowserTabContent();
        if (tabContent != NULL) {
            if (numberOfAnnotations == 1) {
                nameText = tabContent->getTabName();
            }
            else {
                if ( ! nameText.isEmpty()) {
                    nameText.append(",");
                }
                nameText.append(QString::number(tabContent->getTabNumber() + 1));
            }
        }
        
        if (browserTabAnnotation->isBrowserTabDisplayed()) {
            visibleCount++;
        }
        else {
            hiddenCount++;
        }
    }

    setEnabled(numberOfAnnotations > 0);
    
    m_nameLabel->setText(nameText);
    
    if (m_visibilityCheckBox != NULL) {
        /*
         * Need to block signals as QCheckBox::setCheckState()
         * causes a emission of a signal
         */
        QSignalBlocker checkBoxBlocker(m_visibilityCheckBox);
        if ((visibleCount > 0)
            && (hiddenCount > 0)) {
            /* Unchecked if status is mixed */
            m_visibilityCheckBox->setCheckState(Qt::Unchecked);
        }
        else  if (visibleCount > 0) {
            m_visibilityCheckBox->setCheckState(Qt::Checked);
        }
        else {
            m_visibilityCheckBox->setCheckState(Qt::Unchecked);
        }
    }
}

/**
 * Called when state of visibility checkbox is changed
 */
void
AnnotationNameWidget::visibilityCheckStateChanged(int state)
{
    Qt::CheckState checkState = static_cast<Qt::CheckState>(state);
    switch (checkState) {
        case Qt::Unchecked:
            for (auto bta : m_browserTabAnnotations) {
                bta->setBrowserTabDisplayed(false);
            }
            break;
        case Qt::PartiallyChecked:
            break;
        case Qt::Checked:
            for (auto bta : m_browserTabAnnotations) {
                bta->setBrowserTabDisplayed(true);
            }
            break;
    }
    
    EventManager::get()->sendEvent(EventGraphicsUpdateOneWindow(m_browserWindowIndex).getPointer());
    EventManager::get()->sendEvent(EventUserInterfaceUpdate().getPointer());
}

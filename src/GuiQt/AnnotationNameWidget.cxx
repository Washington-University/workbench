
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

#include <QLabel>
#include <QHBoxLayout>

#include "AnnotationBrowserTab.h"
#include "AnnotationMenuArrange.h"
#include "BrowserTabContent.h"
#include "CaretAssert.h"
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
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 2, 2);
    layout->addWidget(m_nameLabel, 100, Qt::AlignVCenter);
    
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
    AString name;
    const int32_t numSelected = static_cast<int32_t>(annotations.size());
    if (numSelected > 1) {
        name = "+";
    }
    else if (numSelected == 1) {
        CaretAssertVectorIndex(annotations, 0);
        AnnotationBrowserTab* browserTabAnnotation = dynamic_cast<AnnotationBrowserTab*>(annotations[0]);
        if (browserTabAnnotation != NULL) {
            const BrowserTabContent* tabContent = browserTabAnnotation->getBrowserTabContent();
            if (tabContent != NULL) {
                name = tabContent->getTabName();
            }
        }
    }

    setEnabled(numSelected > 0);
    m_nameLabel->setText(name);
}


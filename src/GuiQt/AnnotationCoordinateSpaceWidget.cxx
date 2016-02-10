
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

#define __ANNOTATION_COORDINATE_SPACE_WIDGET_DECLARE__
#include "AnnotationCoordinateSpaceWidget.h"
#undef __ANNOTATION_COORDINATE_SPACE_WIDGET_DECLARE__

#include <set>

#include <QHBoxLayout>

#include "Annotation.h"
#include "CaretAssert.h"
#include "WuQtUtilities.h"

using namespace caret;


    
/**
 * \class caret::AnnotationCoordinateSpaceWidget 
 * \brief Widget that displays the coordinate space of selected annotations
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param browserWindowIndex
 *     Index of window in which this instance is displayed
 * @param parent
 *     Parent for this widget.
 */
AnnotationCoordinateSpaceWidget::AnnotationCoordinateSpaceWidget(const int32_t browserWindowIndex,
                                                                 QWidget* parent)
: QWidget(parent),
m_browserWindowIndex(browserWindowIndex)
{
    m_spaceLabel = new QLabel("  ");
    m_spaceLabel->setToolTip("Selection annotation(s) space.\n"
                             "Mouse dragging to move/resize\n"
                             "annotations allowed in Tab or \n"
                             "Window space only.\n"
                             "   St : Stereotaxic\n"
                             "   Sf : Surface\n"
                             "   T  : Tab\n"
                             "   W  : Window\n"
                             "   +  : Multiple Spaces");
    
                             
    QHBoxLayout* layout = new QHBoxLayout(this);
    WuQtUtilities::setLayoutSpacingAndMargins(layout, 0, 0);
    layout->addWidget(m_spaceLabel);
}

/**
 * Destructor.
 */
AnnotationCoordinateSpaceWidget::~AnnotationCoordinateSpaceWidget()
{
}

/**
 * Update the content of this widget with the given text annotation.
 *
 * @param annotationTexts
 *     Text annotations for display (may be NULL).
 */
void
AnnotationCoordinateSpaceWidget::updateContent(std::vector<Annotation*> annotations)
{
    QString text;
    
    if ( ! annotations.empty()) {
        CaretAssertVectorIndex(annotations, 0);
        CaretAssert(annotations[0]);
        AnnotationCoordinateSpaceEnum::Enum space = annotations[0]->getCoordinateSpace();
        
        std::set<int32_t> tabIndices;
        bool haveMultipleSpacesFlag = false;
        for (int32_t i = 0; i < static_cast<int32_t>(annotations.size()); i++) {
            CaretAssertVectorIndex(annotations, i);
            CaretAssert(annotations[i]);
            const Annotation* ann = annotations[i];
            const AnnotationCoordinateSpaceEnum::Enum annSpace = ann->getCoordinateSpace();
            
            if (annSpace!= space) {
                haveMultipleSpacesFlag = true;
            }
            
            if (annotations[i]->getCoordinateSpace() == AnnotationCoordinateSpaceEnum::TAB) {
                tabIndices.insert(ann->getTabIndex());
            }
        }
        
        if (haveMultipleSpacesFlag) {
            text = "+";
        }
        else {
            text = AnnotationCoordinateSpaceEnum::toGuiAbbreviatedName(space);
            switch (space) {
                case AnnotationCoordinateSpaceEnum::STEREOTAXIC:
                    break;
                case AnnotationCoordinateSpaceEnum::PIXELS:
                    break;
                case AnnotationCoordinateSpaceEnum::SURFACE:
                    break;
                case AnnotationCoordinateSpaceEnum::TAB:
                {
                    QString tabString;
                    for (std::set<int32_t>::iterator iter = tabIndices.begin();
                         iter != tabIndices.end();
                         iter++) {
                        if (tabString.isEmpty()) {
                            tabString.append(":");
                        }
                        else {
                            tabString.append(",");
                        }
                        tabString.append(AString::number(*iter + 1));
                    }
                    
                    text.append(tabString);
                }
                    break;
                case AnnotationCoordinateSpaceEnum::WINDOW:
                    break;
            }
        }
        
    }
    
    m_spaceLabel->setText(text);
}


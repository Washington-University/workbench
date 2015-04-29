
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

#define __ANNOTATION_MENU_ARRANGE_DECLARE__
#include "AnnotationMenuArrange.h"
#undef __ANNOTATION_MENU_ARRANGE_DECLARE__

#include "AnnotationAlignmentEnum.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::AnnotationMenuArrange 
 * \brief Menu for arranging annotations.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent widget.
 * @param browserWindowIndex
 *     Index of the browser window.
 */
AnnotationMenuArrange::AnnotationMenuArrange(const int32_t browserWindowIndex,
                                             QWidget* parent)
: QMenu(parent),
m_browserWindowIndex(browserWindowIndex)
{
    addAction("Group");
    addAction("Ungroup");
    addSeparator();
    addMenu(createAlignMenu());
}

/**
 * Destructor.
 */
AnnotationMenuArrange::~AnnotationMenuArrange()
{
}

/**
 * @return The Align menu.
 */
QMenu*
AnnotationMenuArrange::createAlignMenu()
{
    std::vector<AnnotationAlignmentEnum::Enum> alignments;
    AnnotationAlignmentEnum::getAllEnums(alignments);
    
    QMenu* alignMenu = new QMenu("Align");
    
    for (std::vector<AnnotationAlignmentEnum::Enum>::iterator iter = alignments.begin();
         iter != alignments.end();
         iter++) {
        const AnnotationAlignmentEnum::Enum annAlign = *iter;
        const AString text = AnnotationAlignmentEnum::toGuiName(annAlign);
        
        QAction* action = alignMenu->addAction(text);
        action->setData((int)AnnotationAlignmentEnum::toIntegerCode(annAlign));
    }
    
    QObject::connect(alignMenu, SIGNAL(triggered(QAction*)),
                     this, SLOT(alignMenuActionTriggered(QAction*)));
    
    return alignMenu;
}

/**
 * Gets called when the user selects a menu item.
 */
void
AnnotationMenuArrange::alignMenuActionTriggered(QAction* action)
{
    CaretAssert(action);
    const int32_t integerCode = action->data().toInt();
    
    bool valid = false;
    const AnnotationAlignmentEnum::Enum annAlign = AnnotationAlignmentEnum::fromIntegerCode(integerCode,
                                                                                 &valid);
    if (valid) {
        std::cout << "Insert: " << AnnotationAlignmentEnum::toGuiName(annAlign) << std::endl;
    }
    else {
        const AString msg("Invalid integer code="
                          + QString::number(integerCode)
                          + " for AnnotationTypeEnum");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
}



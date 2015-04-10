
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

#define __ANNOTATION_MENU_INSERT_DECLARE__
#include "AnnotationMenuInsert.h"
#undef __ANNOTATION_MENU_INSERT_DECLARE__

#include "AnnotationTypeEnum.h"
#include "CaretAssert.h"
#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class caret::AnnotationMenuInsert 
 * \brief Menu for inserting annotation.
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param parent
 *     The parent widget.
 */
AnnotationMenuInsert::AnnotationMenuInsert(QWidget* parent)
: QMenu(parent)
{
    setTitle("Insert");
    
    std::vector<AnnotationTypeEnum::Enum> annotationTypes;
    AnnotationTypeEnum::getAllEnums(annotationTypes);
    
    for (std::vector<AnnotationTypeEnum::Enum>::iterator iter = annotationTypes.begin();
         iter != annotationTypes.end();
         iter++) {
        const AnnotationTypeEnum::Enum annType = *iter;
        const AString text = AnnotationTypeEnum::toGuiName(annType);
        
        QAction* action = addAction(text);
        action->setData((int)AnnotationTypeEnum::toIntegerCode(annType));
    }
    
    QObject::connect(this, SIGNAL(triggered(QAction*)),
                     this, SLOT(menuActionTriggered(QAction*)));
}

/**
 * Destructor.
 */
AnnotationMenuInsert::~AnnotationMenuInsert()
{
}

/**
 * Gets called when the user selects a menu item.
 */
void
AnnotationMenuInsert::menuActionTriggered(QAction* action)
{
    CaretAssert(action);
    const int32_t integerCode = action->data().toInt();
    
    bool valid = false;
    const AnnotationTypeEnum::Enum annType = AnnotationTypeEnum::fromIntegerCode(integerCode,
                                                                                 &valid);
    if (valid) {
        std::cout << "Insert: " << AnnotationTypeEnum::toGuiName(annType) << std::endl;
    }
    else {
        const AString msg("Invalid integer code="
                          + QString::number(integerCode)
                          + " for AnnotationTypeEnum");
        CaretAssertMessage(0, msg);
        CaretLogSevere(msg);
    }
}


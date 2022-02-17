
/*LICENSE_START*/
/*
 *  Copyright (C) 2018 Washington University School of Medicine
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

#define __SCENE_DATA_FILE_TREE_ITEM_DECLARE__
#include "SceneDataFileTreeItem.h"
#undef __SCENE_DATA_FILE_TREE_ITEM_DECLARE__

#include "CaretAssert.h"
#include "FileInformation.h"

using namespace caret;


    
/**
 * \class caret::SceneDataFileTreeItem 
 * \brief Item for insertion in a SceneDataFileTreeItemModel
 * \ingroup GuiQt
 */

/**
 * Constructor.
 *
 * @param text
 *     Text for the model item
 * @param absolutePathName
 *     Absolute path name including name of file (if a data file)
 * @param itemType
 *     Item type indicating file or directory.
 *     Use getItemTypeDirectory() or getItemTypeFile().
 */
SceneDataFileTreeItem::SceneDataFileTreeItem(const AString& text,
                                             const AString& absolutePathName,
                                             const int32_t itemType)
: QStandardItem(text),
m_absolutePathName(absolutePathName),
m_itemType(itemType)
{
    FileInformation fileInfo(absolutePathName);
    if ( ! fileInfo.exists()) {
        setText(text
                + "  --NOT FOUND--");
        QBrush fg(foreground());
        fg.setColor(Qt::red);
        setForeground(fg);
    }
    
    setColumnCount(1);
}

/**
 * Destructor.
 */
SceneDataFileTreeItem::~SceneDataFileTreeItem()
{
}

/**
 * Copy constructor.
 * @param obj
 *    Object that is copied.
 */
SceneDataFileTreeItem::SceneDataFileTreeItem(const SceneDataFileTreeItem& obj)
: QStandardItem(obj)
{
    this->copyHelperSceneDataFileTreeItem(obj);
}

/**
 * Assignment operator.
 * @param obj
 *    Data copied from obj to this.
 * @return
 *    Reference to this object.
 */
SceneDataFileTreeItem&
SceneDataFileTreeItem::operator=(const SceneDataFileTreeItem& obj)
{
    if (this != &obj) {
        QStandardItem::operator=(obj);
        this->copyHelperSceneDataFileTreeItem(obj);
    }
    return *this;
}

/**
 * Helps with copying an object of this type.
 * @param obj
 *    Object that is copied.
 */
void
SceneDataFileTreeItem::copyHelperSceneDataFileTreeItem(const SceneDataFileTreeItem& obj)
{
    m_absolutePathName = obj.m_absolutePathName;
    m_itemType         = obj.m_itemType;
}

/**
 * @return Full absolute path name
 */
AString
SceneDataFileTreeItem::getAbsolutePathName() const
{
    return m_absolutePathName;
}

/**
 * @return The type of this item. The type is used to distinguish custom items from the base class
 */
int
SceneDataFileTreeItem::type() const
{
    return m_itemType;
}


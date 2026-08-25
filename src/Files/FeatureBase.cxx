
/*LICENSE_START*/
/*
 *  Copyright (C) 2026 Washington University School of Medicine
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

#define __FEATURE_BASE_DECLARE__
#include "FeatureBase.h"
#undef __FEATURE_BASE_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::FeatureBase 
 * \brief Base class for some Feature classes that are placed in Qt model classes
 * \ingroup Files
 */

/**
 * Constructor.
 * @param baseType
 *   The base type
 */
FeatureBase::FeatureBase(const BaseType baseType)
: QStandardItem(),
m_baseType(baseType)
{
    
}

/**
 * Destructor.
 */
FeatureBase::~FeatureBase()
{
}

/**
 * @return The base type
 */
FeatureBase::BaseType
FeatureBase::getBaseType() const
{
    return m_baseType;
}

/**
 * Set all children to the given checked status
 * @param checked
 *    New checked status
 */
void
FeatureBase::setAllChildrenChecked(const bool checked)
{
    const Qt::CheckState checkState(checked
                                    ? Qt::Checked
                                    : Qt::Unchecked);
    setCheckedStatusOfAllChildren(this,
                                  checkState);
}

/**
 * Set the checked status of all children
 * @param item
 *    The item
 * @param checkState
 *    The check state
 */
void
FeatureBase::setCheckedStatusOfAllChildren(QStandardItem* item,
                                           const Qt::CheckState checkState)
{
    CaretAssert(item);
    const int32_t numChildren(item->rowCount());
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* child(item->child(iRow));
        child->setCheckState(checkState);
        setCheckedStatusOfAllChildren(child,
                                      checkState);
    }
}

/**
 * Set the check state for this instance based upon its children's check states
 * There are three checked status for items WITH CHILDREN
 * - Checked: This instance and ALL children are checked
 * - Unchecked: This instance and ALL children and unchecked
 * - PartiallyChecked: There are children that are both checked and unchecked
 * @return
 *    Check state for this instance
 */
Qt::CheckState
FeatureBase::setCheckStateFromChildren()
{
    const int32_t numChildren(rowCount());
    
    /*
     * If no children, use this instance's check state
     */
    if (numChildren == 0) {
        return checkState();
    }
    
    int32_t numChildrenChecked = 0;
    bool partiallyCheckedFlag(false);
    
    /*
     * Examine checked status of children
     */
    for (int32_t iRow = 0; iRow < numChildren; iRow++) {
        QStandardItem* myChild(child(iRow));
        CaretAssert(myChild);
        CaretAssert(myChild != this); /* should never happen and would cause infinite loop */
        
        FeatureBase* featureBaseChild(dynamic_cast<FeatureBase*>(myChild));
        CaretAssert(featureBaseChild);
        const Qt::CheckState childCheckState(featureBaseChild->setCheckStateFromChildren());
        switch (childCheckState) {
            case Qt::Unchecked:
                break;
            case Qt::Checked:
                ++numChildrenChecked;
                break;
            case Qt::PartiallyChecked:
                /*
                 * If a child is partially checked, then this instance will be partially checked
                 */
                partiallyCheckedFlag = true;
                break;
        }
        
        if (partiallyCheckedFlag) {
            break;
        }
    }
    
    Qt::CheckState childrenCheckState(Qt::Unchecked);
    
    if (partiallyCheckedFlag) {
        /*
         * At least one child is partially checked so this instance is partially checked
         */
        childrenCheckState = Qt::PartiallyChecked;
    }
    else if (numChildrenChecked == numChildren) {
        /*
         * All children are checked so this instance is checked
         */
        childrenCheckState = Qt::Checked;
    }
    else if (numChildrenChecked == 0) {
        /*
         * All children and unchecked so this instance is unchecked
         */
        childrenCheckState = Qt::Unchecked;
    }
    else if (numChildrenChecked > 0) {
        /*
         * Have children that are both checked and unchecked so this
         * instance is partially checked
         */
        childrenCheckState = Qt::PartiallyChecked;;
    }
    else {
        /*
         * Should not get here
         */
        CaretAssert(0);
    }
    
    Qt::CheckState checkStateOut(childrenCheckState);
    
    /*
     * Set check state of this instance
     */
    setCheckState(checkStateOut);
    
    return checkStateOut;
}

/**
 * Destroy a feature and properties.  List is empty upon exit.
 * @param featureAndProperties
 *    Items that are destroyed.
 */
void
FeatureBase::destroyFeatureAndProperties(QList<QStandardItem*>& featureAndProperties)
{
    for (QStandardItem* qsi : featureAndProperties) {
        CaretAssert(qsi);
        delete qsi;
    }
    featureAndProperties.clear();
}


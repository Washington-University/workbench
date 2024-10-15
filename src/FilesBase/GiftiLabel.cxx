/*LICENSE_START*/
/*
 *  Copyright (C) 2014  Washington University School of Medicine
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

#include <sstream>

#define __GIFTI_LABEL_DECLARE__
#include "GiftiLabel.h"
#undef __GIFTI_LABEL_DECLARE__

#include "CaretLogger.h"

using namespace caret;

/**
 * Constructor.
 *
 * @param key - key of the label.
 * @param name - name of label.
 *
 */
GiftiLabel::GiftiLabel(
                   const int32_t key,
                   const AString& name)
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    setNamePrivate(name);
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param red - red color component, zero to one.
 * @param green - green color component, zero to one.
 * @param blue - blue color component, zero to one.
 * @param alpha - alpha color component, zero to one.
 *
 */
GiftiLabel::GiftiLabel(
                   const int32_t key,
                   const AString& name,
                   const float red,
                   const float green,
                   const float blue,
                   const float alpha)
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    setNamePrivate(name);
    this->red = colorClamp(red);
    this->green = colorClamp(green);
    this->blue = colorClamp(blue);
    this->alpha = colorClamp(alpha);
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param red - red color component, zero to one.
 * @param green - green color component, zero to one.
 * @param blue - blue color component, zero to one.
 * @param alpha - alpha color component, zero to one.
 *
 */
GiftiLabel::GiftiLabel(const int32_t key,
                       const AString& name,
                       const float red,
                       const float green,
                       const float blue,
                       const float alpha,
                       const float x,
                       const float y,
                       const float z)
: CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    setNamePrivate(name);
    this->red = colorClamp(red);
    this->green = colorClamp(green);
    this->blue = colorClamp(blue);
    this->alpha = colorClamp(alpha);
    this->x = x;
    this->y = y;
    this->z = z;
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param red - red color component, zero to one.
 * @param green - green color component, zero to one.
 * @param blue - blue color component, zero to one.
 * @param alpha - alpha color component, zero to one.
 *
 */
GiftiLabel::GiftiLabel(
                       const int32_t key,
                       const AString& name,
                       const double red,
                       const double green,
                       const double blue,
                       const double alpha)
: CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    setNamePrivate(name);
    this->red = colorClamp(red);
    this->green = colorClamp(green);
    this->blue = colorClamp(blue);
    this->alpha = colorClamp(alpha);
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param rgba - red, green, blue, alpha color componenents, zero to one.
 *
 */
GiftiLabel::GiftiLabel(
                   const int32_t key,
                   const AString& name,
                   const float rgba[])
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    setNamePrivate(name);
    this->red = colorClamp(rgba[0]);
    this->green = colorClamp(rgba[1]);
    this->blue = colorClamp(rgba[2]);
    this->alpha = colorClamp(rgba[3]);
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param red - red color component, zero to two-fifty-five.
 * @param green - green color component, zero to two-fifty-five.
 * @param blue - blue color component, zero to two-fifty-five.
 * @param alpha - alpha color component, zero to two-fifty-five.
 *
 */
GiftiLabel::GiftiLabel(
                   const int32_t key,
                   const AString& name,
                   const int32_t red,
                   const int32_t green,
                   const int32_t blue,
                   const int32_t alpha)
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    setNamePrivate(name);
    this->red = colorClamp(red / 255.0);
    this->green = colorClamp(green / 255.0);
    this->blue = colorClamp(blue / 255.0);
    this->alpha = colorClamp(alpha / 255.0);
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 * @param name - name of label.
 * @param rgba - red, green, blue, alpha color componenents, zero to 255.
 *
 */
GiftiLabel::GiftiLabel(
                   const int32_t key,
                   const AString& name,
                   const int32_t rgba[])
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    setNamePrivate(name);
    this->red = colorClamp(rgba[0] / 255.0);
    this->green = colorClamp(rgba[1] / 255.0);
    this->blue = colorClamp(rgba[2] / 255.0);
    this->alpha = colorClamp(rgba[3] / 255.0);
}

/**
 * Constructor.
 *
 * @param key - Key of the label.
 *
 */
GiftiLabel::GiftiLabel(
                   const int32_t key)
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    if (this->key == 0) {
        setNamePrivate("???");
    }
    else {
        std::stringstream str;
        str << "???" << this->key;
        setNamePrivate(AString::fromStdString(str.str()));
    }
}

/**
 * Destructor
 */
GiftiLabel::~GiftiLabel()
{
}

float GiftiLabel::colorClamp(const float& in)
{
    if (in < 0.0f) return 0.0f;
    if (in > 1.0f) return 1.0f;
    if (in != in)
    {
        CaretLogWarning("GiftiLabel was given NaN as a color, changing to 1.0");
        return 1.0f;
    }
    return in;
}

/**
 * Copy Constructor
 * @param Object that is copied.
 */
GiftiLabel::GiftiLabel(const GiftiLabel& o)
    : CaretObject(o), TracksModificationInterface()
{
    this->initializeMembersGiftiLabel();
    this->copyHelper(o);
}

/**
 * Assignment operator.
 */
GiftiLabel&
GiftiLabel::operator=(const GiftiLabel& o)
{
    if (this != &o) {
        CaretObject::operator=(o);
        this->copyHelper(o);
    };
    return *this;
}

/**
 * Helps with copy constructor and assignment operator.
 */
void
GiftiLabel::copyHelper(const GiftiLabel& gl)
{
    this->initializeMembersGiftiLabel();
    setNamePrivate(gl.name);
    this->key = gl.key;
    this->selected = gl.selected;
    this->red = gl.red;
    this->green = gl.green;    
    this->blue = gl.blue;    
    this->alpha = gl.alpha;
    this->x = gl.x;
    this->y = gl.y;
    this->z = gl.z;
    this->count = 0;
    m_groupNameSelectionItem = gl.m_groupNameSelectionItem;
}

/**
 * Initialize data members.
 */
void
GiftiLabel::initializeMembersGiftiLabel() 
{
    this->modifiedFlag = false;
    this->medialWallNameFlag = false;
    this->name = "";
    this->key = s_invalidLabelKey;
    this->selected = true;
    this->red = 1.0;
    this->green = 1.0;    
    this->blue = 1.0;    
    this->alpha = 1.0;
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
    this->count = 0;
    m_groupNameSelectionItem = NULL;
}

/**
 * Determine if two objects are equal.  Two GiftiLabels are equal if they 
 * have the same "key".
 * @param obj Object for comparison.
 * @return true if equal, else false.
 *
 */
bool
GiftiLabel::equals(const GiftiLabel& gl)
{
    return (this->key == gl.key);
}

/**
 * Compare this label to another label using the indices of the labels.
 * @param gl - Compare to this GiftiLabel.
 * @return negative if "this" is less, positive if "this" is greater,
 * else zero.
 *
 */
int32_t
GiftiLabel::operator<(const GiftiLabel& gl)
{
    return (this->key < gl.key);
}

/**
 * Get the key of this label.
 * @return key of the label.
 *
 */
int32_t
GiftiLabel::getKey() const
{
    return this->key;
}

/**
 * Set the key of this label.  DO NOT call this method on a label
 * retrieved from the label table.
 * 
 * @param key - New key for this label.
 *
 */
void
GiftiLabel::setKey(const int32_t key)
{
    this->key = key;
    this->setModified();
}

/**
 * Get the name.
 * @return Name of label.
 *
 */
AString
GiftiLabel::getName() const
{
    return this->name;
}

/**
 * Set the name.
 * @param name - new name for label.
 *
 */
void
GiftiLabel::setName(const AString& name)
{
    setNamePrivate(name);
    this->setModified();
}

/**
 * (1) Sets the name of the label.
 * (2) Examines the name of the label to see if the label
 * is a "medial wall" name which is defined as a name that
 * contains substring "medial", followed by zero or more
 * characters, followed by the substring "wall".
 * (3) DOES NOT change the modfified
 * status for this label.
 *
 * @param name 
 *    New name for label.
 */
void
GiftiLabel::setNamePrivate(const AString& name)
{
    this->name = name;
    
    this->medialWallNameFlag = false;
    
    const int32_t medialIndex = name.indexOf("medial", 0, Qt::CaseInsensitive);
    if (medialIndex >= 0) {
        const int32_t wallIndex = name.indexOf("wall", 6, Qt::CaseInsensitive);
        if (wallIndex > medialIndex) {
            this->medialWallNameFlag = true;
        }
    }
}

/**
 * @return A string that contains both the key and name
 * for use in the label editor.
 */
AString
GiftiLabel::getNameAndKeyForLabelEditor() const
{
    const AString keyAndNameText(QString::number(this->key).rightJustified(4,
                                                                           ' ',
                                                                           false)
                                 + ":   "
                                 + (this->name));
    return keyAndNameText;
}

/**
 * Is this label selected (for display)?
 *
 * @return  true if label selected for display, else false.
 *
 */
bool
GiftiLabel::isSelected() const
{
    return this->selected;
}

/**
 * Set the label selected (for display).
 *
 * @param selected - new selection status.
 *
 */
void
GiftiLabel::setSelected(const bool selected)
{
    this->selected = selected;
}

/**
 * Get the color components.
 * @param rgbaOut four dimensional array into which are loaded,
 * red, green, blue, and alpha components ranging 0.0. to 1.0.
 *
 */
void
GiftiLabel::getColor(float rgbaOut[4]) const
{
    rgbaOut[0] = this->red;
    rgbaOut[1] = this->green;
    rgbaOut[2] = this->blue;
    rgbaOut[3] = this->alpha;
}

/**
 * Set the color components.
 *
 * @param rgba - A four-dimensional array of floats containing the red,
 * green, blue, and alpha components with values ranging from 0.0 to 1.0.
 *
 */
void
GiftiLabel::setColor(const float rgba[4])
{
    this->red = colorClamp(rgba[0]);
    this->green = colorClamp(rgba[1]);
    this->blue = colorClamp(rgba[2]);
    this->alpha = colorClamp(rgba[3]);
    this->setModified();
}

/**
 * Get the default color.
 *
 * @param Output with a four-dimensional array of floats
 * containing the red, green, blue, and alpha components with values
 * ranging from 0.0 to 1.0.
 */
void
GiftiLabel::getDefaultColor(float rgbaOut[4])
{
    rgbaOut[0] = 1.0;
    rgbaOut[1] = 1.0;
    rgbaOut[2] = 1.0;
    rgbaOut[3] = 1.0;
}

/**
 * Get the red color component for this label.
 * @return red color component.
 *
 */
float
GiftiLabel::getRed() const
{
    return this->red;
}

/**
 * Get the green color component for this label.
 * @return green color component.
 *
 */
float
GiftiLabel::getGreen() const
{
    return this->green;
}

/**
 * Get the blue color component for this label.
 * @return blue color component.
 *
 */
float
GiftiLabel::getBlue() const
{
    return this->blue;
}

/**
 * Get the alpha color component for this label.
 * @return alpha color component.
 *
 */
float
GiftiLabel::getAlpha() const
{
    return this->alpha;
}

/**
 * Get the X-Coordinate.
 * @return
 *    The X-coordinate.
 */
float 
GiftiLabel::getX() const 
{ 
    return this->x; 
}

/**
 * Get the Y-Coordinate.
 * @return
 *    The Y-coordinate.
 */
float 
GiftiLabel::getY() const 
{ return this->y; 
}

/**
 * Get the Z-Coordinate.
 * @return
 *    The Z-coordinate.
 */
float 
GiftiLabel::getZ() const 
{ 
    return this->z; 
}

/**
 * Get the XYZ coordiantes.
 * @param  xyz
 *   Array into which coordinates are loaded.
 */
void 
GiftiLabel::getXYZ(float xyz[3]) const
{
    xyz[0] = this->x;
    xyz[1] = this->y;
    xyz[2] = this->z;
}

/**
 * Set the X-coordinate.
 * @param x
 *    New value for X-coordinate.
 */
void 
GiftiLabel::setX(const float x)
{
    this->x = x;
    this->setModified();
}

/**
 * Set the Y-coordinate.
 * @param y
 *    New value for Y-coordinate.
 */
void 
GiftiLabel::setY(const float y)
{
    this->y = y;
    this->setModified();
}

/**
 * Set the Z-coordinate.
 * @param z
 *    New value for Z-coordinate.
 */
void 
GiftiLabel::setZ(const float z)
{
    this->z = z;
    this->setModified();
}

/**
 * Set the XYZ coordinates.
 * @param xyz
 *   Array containing XYZ coordiantes.
 */
void 
GiftiLabel::setXYZ(const float xyz[3])
{
    this->x = xyz[0];
    this->y = xyz[1];
    this->z = xyz[2];
    this->setModified();
}

/**
 * Set this object has been modified.
 *
 */
void
GiftiLabel::setModified()
{
    this->modifiedFlag = true;
}

/**
 * Set this object as not modified.  Object should also
 * clear the modification status of its children.
 *
 */
void
GiftiLabel::clearModified()
{
    this->modifiedFlag = false;
}

/**
 * Get the modification status.  Returns true if this object or
 * any of its children have been modified.
 * @return - The modification status.
 *
 */
bool
GiftiLabel::isModified() const
{
    return this->modifiedFlag;
}

/**
 * Get information about this label.
 * 
 * @return Information about the label.
 *
 */
AString
GiftiLabel::toString() const
{
    AString s;
    
    s += "[GiftiLabel=(key=" 
    + AString::number(this->getKey()) 
    + ","
    + this->getName()
    + ","
    + AString::number(this->getRed())
    + ","
    + AString::number(this->getGreen())
    + ","
    + AString::number(this->getBlue())
    + ","
    + AString::number(this->getAlpha())
    + ","
    + AString::number(this->getX())
    + ","
    + AString::number(this->getY())
    + ","
    + AString::number(this->getZ())
    + ") ";
    return s;
}

/**
 * Get the count.
 * @return Count value.
 *
 */
int32_t
GiftiLabel::getCount() const
{
    return this->count;
}

/**
 * Set the count.
 * @param count - new value for count.
 *
 */
void
GiftiLabel::setCount(const int32_t count)
{
    this->count = count;
}

/**
 * Increment the count.
 *
 */
void
GiftiLabel::incrementCount()
{
    this->count++;
}

bool
GiftiLabel::matches(const GiftiLabel& rhs, const bool checkColor, const bool checkCoord) const
{
    if (key != rhs.key) return false;
    if (name != rhs.name) return false;
    if (checkColor)
    {
        if (red != rhs.red) return false;
        if (green != rhs.green) return false;
        if (blue != rhs.blue) return false;
        if (alpha != rhs.alpha) return false;
    }
    if (checkCoord)
    {
        if (x != rhs.x) return false;
        if (y != rhs.y) return false;
        if (z != rhs.z) return false;
    }
    return true;
}

/**
 * Set the selection item for the group/name hierarchy.
 *
 * @param item
 *     The selection item from the group/name hierarchy.
 */
void
GiftiLabel::setGroupNameSelectionItem(GroupAndNameHierarchyItem* item)
{
    m_groupNameSelectionItem = item;
}

/**
 * @return The selection item for the Group/Name selection hierarchy.
 *      May be NULL in some circumstances.
 */
const GroupAndNameHierarchyItem*
GiftiLabel::getGroupNameSelectionItem() const
{
    return m_groupNameSelectionItem;
}

/**
 * @return The selection item for the Group/Name selection hierarchy.
 *      May be NULL in some circumstances.
 */
GroupAndNameHierarchyItem*
GiftiLabel::getGroupNameSelectionItem()
{
    return m_groupNameSelectionItem;
}



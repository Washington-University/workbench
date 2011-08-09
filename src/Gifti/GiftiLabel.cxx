/*LICENSE_START*/ 
/* 
 *  Copyright 1995-2002 Washington University School of Medicine 
 * 
 *  http://brainmap.wustl.edu 
 * 
 *  This file is part of CARET. 
 * 
 *  CARET is free software; you can redistribute it and/or modify 
 *  it under the terms of the GNU General Public License as published by 
 *  the Free Software Foundation; either version 2 of the License, or 
 *  (at your option) any later version. 
 * 
 *  CARET is distributed in the hope that it will be useful, 
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of 
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the 
 *  GNU General Public License for more details. 
 * 
 *  You should have received a copy of the GNU General Public License 
 *  along with CARET; if not, write to the Free Software 
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA 
 * 
 */ 

#include <sstream>

#include "GiftiLabel.h"
#include "StringUtilities.h"

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
                   const QString& name)
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    this->name = name;
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
                   const QString& name,
                   const float red,
                   const float green,
                   const float blue,
                   const float alpha)
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    this->name = name;
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
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
                       const QString& name,
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
    this->name = name;
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
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
                       const QString& name,
                       const double red,
                       const double green,
                       const double blue,
                       const double alpha)
: CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    this->name = name;
    this->red = red;
    this->green = green;
    this->blue = blue;
    this->alpha = alpha;
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
                   const QString& name,
                   const float rgba[])
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    this->name = name;
    this->red = rgba[0];
    this->green = rgba[1];
    this->blue = rgba[2];
    this->alpha = rgba[3];
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
                   const QString& name,
                   const int32_t red,
                   const int32_t green,
                   const int32_t blue,
                   const int32_t alpha)
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    this->name = name;
    this->red = red / 255.0;
    this->green = green / 255.0;
    this->blue = blue / 255.0;
    this->alpha = alpha / 255.0;
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
                   const QString& name,
                   const int32_t rgba[])
    : CaretObject()
{
    this->initializeMembersGiftiLabel();
    this->key = key;
    this->name = name;
    this->red = rgba[0] / 255.0;
    this->green = rgba[1] / 255.0;
    this->blue = rgba[2] / 255.0;
    this->alpha = rgba[3] / 255.0;
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
        this->name = "???";    
    }
    else {
        std::stringstream str;
        str << "???" << this->key;
        this->name = QString::fromStdString(str.str());
    }
}

/**
 * Destructor
 */
GiftiLabel::~GiftiLabel()
{
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
    this->modifiedFlag = false;
    this->name = gl.name;
    this->key = gl.key;
    this->selected = false;
    this->red = gl.red;
    this->green = gl.green;    
    this->blue = gl.blue;    
    this->alpha = gl.alpha;
}

/**
 * Initialize data members.
 */
void
GiftiLabel::initializeMembersGiftiLabel() 
{
    this->modifiedFlag = false;
    this->name = "";
    this->key = -1;
    this->selected = true;
    this->red = 1.0;
    this->green = 1.0;    
    this->blue = 1.0;    
    this->alpha = 1.0;
    this->x = 0.0;
    this->y = 0.0;
    this->z = 0.0;
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
QString
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
GiftiLabel::setName(const QString& name)
{
    this->name = name;
    this->setModified();
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
 *
 * @return  A four-dimensional array of floats containing the red, green,
 * blue, and alpha components with values ranging from 0.0 to 1.0.
 * User MUST delete[] returned array.
 *
 */
float*
GiftiLabel::getColor() const
{
    float* rgba = new float[4];
    rgba[0] = this->red;
    rgba[1] = this->green;
    rgba[2] = this->blue;
    rgba[3] = this->alpha;
    return rgba;
}

/**
 * Get the color components.
 * @param rgbaOut four dimensional array into which are loaded,
 * red, green, blue, and alpha components ranging 0.0. to 1.0.
 *
 */
void
GiftiLabel::getColor(float rgbaOut[]) const
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
GiftiLabel::setColor(const float rgba[])
{
    this->red = rgba[0];
    this->green = rgba[1];
    this->blue = rgba[2];
    this->alpha = rgba[3];
    this->setModified();
}

/**
 * Get the colors as integers ranging 0 to 255.
 * @return  Four-dimensional array containing color components.
 * User must delete[] the returned array.
 *
 */
int32_t*
GiftiLabel::getColorInt() const
{
    int32_t* rgbaOut = new int32_t[4];
    rgbaOut[0] = this->red * 255;
    rgbaOut[1] = this->green * 255;
    rgbaOut[2] = this->blue * 255;
    rgbaOut[3] = this->alpha * 255;
    return rgbaOut;
}

/**
 * Set the colors with integers ranging 0 to 255.
 * @param rgba - four-dimensional array with colors ranging 0 to 255.
 *
 */
void
GiftiLabel::setColorInt(const int32_t rgba[])
{
    this->red = rgba[0] / 255.0;
    this->green = rgba[1] / 255.0;
    this->blue = rgba[2] / 255.0;
    this->alpha = rgba[3] / 255.0;
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
GiftiLabel::setY(const float x)
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
GiftiLabel::setZ(const float x)
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
QString
GiftiLabel::toString() const
{
    QString s;
    
    s += "[GiftiLabel=(key=" 
    + QString::number(this->getKey()) 
    + ","
    + this->getName()
    + ","
    + QString::number(this->getRed())
    + ","
    + QString::number(this->getGreen())
    + ","
    + QString::number(this->getBlue())
    + ","
    + QString::number(this->getAlpha())
    + ","
    + QString::number(this->getX())
    + ","
    + QString::number(this->getY())
    + ","
    + QString::number(this->getZ())
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


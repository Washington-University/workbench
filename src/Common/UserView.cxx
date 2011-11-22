
/*LICENSE_START*/
/* 
 *  Copyright 1995-2011 Washington University School of Medicine 
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

#define __USER_VIEW_DECLARE__
#include "UserView.h"
#undef __USER_VIEW_DECLARE__

#include <QStringList>

#include "CaretLogger.h"

using namespace caret;


    
/**
 * \class UserView 
 * \brief Translation, Rotation, and Scaling for a view set by the user.
 *
 * Translation, Rotation, and Scaling for a view set by the user.
 */
/**
 * Constructor.
 */
UserView::UserView()
: CaretObject()
{
    this->name = "";
    
    this->translation[0] = 0.0;
    this->translation[1] = 0.0;
    this->translation[2] = 0.0;
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            if (i == j) {
                this->rotation[i][j] = 1.0;
            }
            else {
                this->rotation[i][j] = 0.0;
            }
        }
    }
    
    this->scaling = 1.0;
}

/**
 * Destructor.
 */
UserView::~UserView()
{
    
}

/**
 * Copy constructor.
 * @param userView
 *   View that is copied.
 */
UserView::UserView(const UserView& userView)
: CaretObject(userView)
{
    this->copyHelper(userView);
}

/**
 * Assignment operator.
 * @param userView
 *    View that is copied to this view.
 * @return 
 *    Reference to this object.
 */
UserView& 
UserView::operator=(const UserView& userView)
{
    if (this != &userView) {
        CaretObject::operator=(userView);
        this->copyHelper(userView);
    }
    
    return *this;
}

/**
 * Less than operator.
 * @param view
 *    View compared to this view.
 * @return
 *    Returns result of a name comparison.
 */
bool 
UserView::operator<(const UserView& view) const
{
    return (this->name < view.name);
}

/**
 * Get the translation
 * @param translation
 *   Output translation.
 */
void 
UserView::getTranslation(float translation[3]) const
{
    translation[0] = this->translation[0];
    translation[1] = this->translation[1];
    translation[2] = this->translation[2];
}

/**
 * Get the rotation matrix.
 * @param rotation
 *   Output rotation matrix.
 */
void 
UserView::getRotation(float rotation[4][4]) const
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            rotation[i][j] = this->rotation[i][j];
        }
    }
}

/**
 * @return The scaling.
 */
float 
UserView::getScaling() const
{
    return this->scaling;
}

/**
 * @return Name of the view.
 */
AString 
UserView::getName() const
{
    return this->name;
}

/**
 * Set the name of the view.
 * @param name
 *   New name for view.
 */
void 
UserView::setName(const AString& name)
{
    this->name = name;
}

/**
 * Set the translation
 * @param translation
 *   New translation.
 */
void 
UserView::setTranslation(const float translation[3])
{
    this->translation[0] = translation[0];
    this->translation[1] = translation[1];
    this->translation[2] = translation[2];
}

/**
 * Set the rotation matrix.
 * @param rotation
 *   New rotation matrix.
 */
void 
UserView::setRotation(const float rotation[4][4])
{
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            this->rotation[i][j] = rotation[i][j];
        }
    }
}

/**
 * Set the scaling
 * @param scaling
 *    New value for scaling.
 */
void 
UserView::setScaling(const float scaling)
{
    this->scaling = scaling;
}

/**
 * Returns the user view in a string that contains,
 * separated by commas: View Name, translation[3],
 * rotation[4][4], and scaling.
 */
AString 
UserView::getAsString() const
{
    AString s = (this->name
                 + "," + AString::number(this->translation[0])
                 + "," + AString::number(this->translation[1])
                 + "," + AString::number(this->translation[2]));
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            s += ("," + AString::number(this->rotation[i][j]));
        }
    }
    
    s += ("," + AString::number(this->scaling));
    
    return s;
}

/**
 * Set the user view from a string that contains,
 * separated by commas: View Name, translation[3],
 * rotation[4][4], and scaling.
 */
bool 
UserView::setFromString(const AString& s)
{
    QStringList sl = s.split(",", QString::KeepEmptyParts);
    const int numElements = sl.count();
    if (numElements != 21) {
        CaretLogSevere("User view string does not contain 21 elements");
        return false;
    }
    
    int ctr = 0;
    this->name = sl.at(ctr++);
    this->translation[0] = sl.at(ctr++).toFloat();
    this->translation[1] = sl.at(ctr++).toFloat();
    this->translation[2] = sl.at(ctr++).toFloat();
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            this->rotation[i][j] = sl.at(ctr++).toFloat();
        }
    }
    
    this->scaling = sl.at(ctr++).toFloat();

    return true;
}

/**
 * Copy all data from the given user view to this user view.
 * @param userView
 *    View from which data is copied.
 */
void 
UserView::copyHelper(const UserView& userView)
{
    this->name = userView.name;
    
    this->translation[0] = userView.translation[0];
    this->translation[1] = userView.translation[1];
    this->translation[2] = userView.translation[2];
    
    for (int32_t i = 0; i < 4; i++) {
        for (int32_t j = 0; j < 4; j++) {
            this->rotation[i][j] = userView.rotation[i][j];
        }
    }
    
    this->scaling = userView.scaling;
}

/**
 * Get a description of this object's content.
 * @return String describing this object's content.
 */
AString 
UserView::toString() const
{
    return ("UserView: " + this->getAsString());
}

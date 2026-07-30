
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

#define __NEUROGLANCER_ANNOTATION_LABEL_DECLARE__
#include "NeuroglancerAnnotationLabel.h"
#undef __NEUROGLANCER_ANNOTATION_LABEL_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::NeuroglancerAnnotationLabel 
 * \brief Annotation index and label
 * \ingroup Files
 */

/**
 * Constructor.
 * @param value
 *    The label value
 * @param text
 *    Text of the label
 */
NeuroglancerAnnotationLabel::NeuroglancerAnnotationLabel(const int32_t value,
                                                         const AString& text)
: NeuroglancerAnnotationBase(NeuroglancerAnnotationBase::BaseType::LABEL),
m_value(value)
{
    setText(text);
    
    setFlags(Qt::ItemIsSelectable
             | Qt::ItemIsEnabled);
    setCheckable(true);
    setCheckState(Qt::Checked);
}

/**
 * Destructor.
 */
NeuroglancerAnnotationLabel::~NeuroglancerAnnotationLabel()
{
}

/**
 * @return Value of the label
 */
int32_t
NeuroglancerAnnotationLabel::getValue() const
{
    return m_value;
}

/**
 * @return String showing content.
 */
AString
NeuroglancerAnnotationLabel::toString() const
{
    return ("Value=" + AString::number(m_value)
            + ", Text=" + text());
}


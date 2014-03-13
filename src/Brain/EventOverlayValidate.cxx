
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

#define __EVENT_OVERLAY_VALIDATE_DECLARE__
#include "EventOverlayValidate.h"
#undef __EVENT_OVERLAY_VALIDATE_DECLARE__

#include "EventTypeEnum.h"

using namespace caret;


    
/**
 * \class caret::EventOverlayValidate 
 * \brief Test an overlay for validity (it exists).
 * \ingroup Brain
 */

/**
 * Constructor.
 */
EventOverlayValidate::EventOverlayValidate(const Overlay* overlay)
: Event(EventTypeEnum::EVENT_OVERLAY_VALIDATE),
  m_overlay(overlay)
{
    m_valid = false;
}

/**
 * Destructor.
 */
EventOverlayValidate::~EventOverlayValidate()
{
    
}

/**
 * @return true if the overlay was found to be valid.
 */
bool
EventOverlayValidate::isValidOverlay() const
{
    return m_valid;
}

/**
 * Set the validity if the given overlay is the overlay
 * that was passed to the constructor.
 *
 * @param overlay
 *    Overlay tested for match.
 */
void
EventOverlayValidate::testValidOverlay(const Overlay* overlay)
{
    if (m_overlay == overlay) {
        m_valid = true;
    }
}


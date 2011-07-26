#ifndef __TRACKSMODIFICATION_H__
#define __TRACKSMODIFICATION_H__

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

namespace caret {

/**
 * Interface for tracking an objects modification status.
 */
class TracksModificationInterface {

public:
    /**
     * Constructor.
     */
    TracksModificationInterface() { }

    /**
     * Destructor.
     */
    virtual ~TracksModificationInterface() { }

private:
    TracksModificationInterface(const TracksModificationInterface&);
    
    TracksModificationInterface& operator=(const TracksModificationInterface&);
    
public:
    /**
     * Set the status to modified.
     */
    virtual void setModified() = 0;

    /**
     * Set the status to unmodified.
     */
    virtual void clearModified() = 0;

    /**
     * Is the object modified?
     * @return true if modified, else false.
     */
    virtual bool isModified() const = 0;

};

} // namespace

#endif // __TRACKSMODIFICATION_H__

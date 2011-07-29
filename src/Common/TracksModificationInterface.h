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
 * <br>
 * When the class DOES NOT extend a class that implements
 * this interface, the class must implmenent the three
 * methods setModified(), clearModified(), and isModified().
 * <br>
 * setModified() should just set a boolean that indicates
 * the modified status.
 * <br>
 * clearModified() should clear the boolean that indicates
 * the modified status AND is should call clearModified()
 * on any members that implement this interface.
 * <br>
 * isModified() should return the boolean that indicates
 * the modified status.
 * <br>
 * <br>
 * When a class DOES extend a class that implements this
 * interface, it must implement the clearModified()
 * and the isModified() ONLY IF it contains members
 * that implement this interface.
 * <br>
 * The clearMethod() must call the parent's clearModified()
 * method and the clearModified() method on any member
 * classes that implement this interface.
 * <br>
 * The isModified() method must first call the parent's
 * isModified() method, and, if true, true true.  Otherwise,
 * return true if a member is modified.
 *
 * <br>
 * An alternative model of this interface is to add
 * methods such as addTrackable(TracksModificationInterface)
 * and removeTrackable(TracksModificationInterface) that
 * could be used to query and reset the modification status
 * of any members in the implementing class and subclasses.
 * In this case, the isModified(), setModified(), and
 * clearModified() methods would only need to be implemented
 * in the top-level class.  In this case the 
 * addTrackable() and removeTrackable() classes may be
 * better off in a separate interface.  
 */
class TracksModificationInterface {

protected:
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

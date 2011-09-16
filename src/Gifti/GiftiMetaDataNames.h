#ifndef __GIFTI_META_DATA_NAMES__H_
#define __GIFTI_META_DATA_NAMES__H_

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


#include "CaretObject.h"

namespace caret {

    
    /**
     * \brief GIFTI Metadata Names
     *
     * Names for GIFTI Metadata elements.
     */
    class GiftiMetaDataNames : public CaretObject {
        
    public:
        /** GIFTI primary anatomical structure */
        static const AString ANATOMICAL_STRUCTURE_PRIMARY;
        
        /** GIFTI secondary anatomical structure */
        static const AString ANATOMICAL_STRUCTURE_SECONDARY;
        
    private:
        GiftiMetaDataNames();
        
        virtual ~GiftiMetaDataNames();
        
        GiftiMetaDataNames(const GiftiMetaDataNames&);

        GiftiMetaDataNames& operator=(const GiftiMetaDataNames&);
        
    public:
        virtual AString toString() const;
        
    private:
    };
    
#ifdef __GIFTI_META_DATA_NAMES_DECLARE__
    const AString GiftiMetaDataNames::ANATOMICAL_STRUCTURE_PRIMARY = "AnatomicalStructurePrimary";
    const AString GiftiMetaDataNames::ANATOMICAL_STRUCTURE_SECONDARY = "AnatomicalStructureSecondary";
#endif // __GIFTI_META_DATA_NAMES_DECLARE__

} // namespace
#endif  //__GIFTI_META_DATA_NAMES__H_


#ifndef __STUDY_META_DATA_LINK_SET_H__
#define __STUDY_META_DATA_LINK_SET_H__

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

#include <vector>

#include "CaretObject.h"
#include "StudyMetaDataLink.h"
#include "XmlException.h"

namespace caret {
    class XmlWriter;
    
    /// class for accessing and storing a group of StudyMetaDataLink
    class StudyMetaDataLinkSet : public CaretObject {
    public:
        // constructor
        StudyMetaDataLinkSet();
        
        // destructor
        ~StudyMetaDataLinkSet();
        
        // add a StudyMetaDataLink
        void addStudyMetaDataLink(const StudyMetaDataLink& smdl);
        
        // remove all links
        void clear();
        
        /// get the number of study meta data links
        int getNumberOfStudyMetaDataLinks() const { return m_links.size(); }
        
        // get a StudyMetaDataLink
        StudyMetaDataLink getStudyMetaDataLink(const int indx) const;
        
        // get a pointer to a StudyMetaDataLink
        StudyMetaDataLink* getStudyMetaDataLinkPointer(const int indx);
        
        // get all linked PubMed IDs
        void getAllLinkedPubMedIDs(std::vector<AString>& pmidsOut) const;
        
        // remove a study meta data link
        void removeStudyMetaDataLink(const int indx);
        
        // set a study meta data link
        void setStudyMetaDataLink(const int indx, const StudyMetaDataLink& smdl);
        
        /// get the entire link set in an "coded" text form
        AString getLinkSetAsCodedText() const;
        
        /// set the link set from "coded" text form
        void setLinkSetFromCodedText(const AString& txt);
        
        // called to write XML
        void writeXML(XmlWriter& xmlWriter) const;
        
        //
        //----- tags for reading and writing 
        //
        /// tag for reading and writing study metadata
        static const AString XML_TAG_STUDY_META_DATA_LINK_SET;
        
        /// get the link separator for when stored as a string
        static const AString encodedTextLinkSeparator;  
        
    protected:
        /// the StudyMetaDataLink
        std::vector<StudyMetaDataLink> m_links;
        
        friend class CellBase;
    };
    
#ifdef __STUDY_META_DATA_LINK_SET_MAIN__
    const AString StudyMetaDataLinkSet::XML_TAG_STUDY_META_DATA_LINK_SET = "StudyMetaDataLinkSet";
    const AString StudyMetaDataLinkSet::encodedTextLinkSeparator = ":::::"; 
#endif // __STUDY_META_DATA_LINK_SET_MAIN__
    
} // namespace 

#endif // __STUDY_META_DATA_LINK_SET_H__



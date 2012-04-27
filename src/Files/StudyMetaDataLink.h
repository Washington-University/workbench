
#ifndef __STUDY_META_DATA_LINK_H__
#define __STUDY_META_DATA_LINK_H__

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
/*LICENSE_END*/

#include "CaretObject.h"

#include "XmlException.h"


namespace caret {
    class XmlWriter;
    
    class StudyMetaDataLink : public CaretObject {
    public:
        // constructor
        StudyMetaDataLink();
        
        // destructor
        ~StudyMetaDataLink();
        
        // copy constructor
        StudyMetaDataLink(const StudyMetaDataLink& smdl);
        
        // assignment opertator
        StudyMetaDataLink& operator=(const StudyMetaDataLink& smdl);
        
        // equality operator
        bool operator==(const StudyMetaDataLink& smdl) const;
        
        // clear the link
        void clear();
        
        /// get the PubMed ID (negative if project ID, zero if invalid)
        AString getPubMedID() const { return m_pubMedID; }
        
        /// set the PubMed ID (negative if project ID)
        void setPubMedID(const AString& pmid) { m_pubMedID = pmid; }
        
        /// get the table number (blank if invalid)
        AString getTableNumber() const { return m_tableNumber; }
        
        /// set the table number (blank if invalid)
        void setTableNumber(const AString& tn);
        
        /// get the table sub header number (blank if invalid)
        AString getTableSubHeaderNumber() const { return m_tableSubHeaderNumber; }
        
        /// set the table sub header number (blank if invalid)
        void setTableSubHeaderNumber(const AString& tshn);
        
        /// get the figure number (blank if invalid)
        AString getFigureNumber() const { return m_figureNumber; }
        
        /// set the figure number (blank if invalid)
        void setFigureNumber(const AString& fn);
        
        /// get the panel letter/number (blank if invalid)
        AString getFigurePanelNumberOrLetter() const { return m_panelNumberOrLetter; }
        
        /// set the panel letter/number (blank if invalid)
        void setFigurePanelNumberOrLetter(const AString& pnl);
        
        /// get the page reference page number (blank if invalid)
        AString getPageReferencePageNumber() const { return m_pageReferencePageNumber; }
        
        /// set the page reference page number (blank if invalid)
        void setPageReferencePageNumber(const AString& prpn);
        
        /// get the page reference sub header number (blank if invalid)
        AString getPageReferenceSubHeaderNumber() const { return m_pageReferenceSubHeaderNumber; }
        
        /// set the page reference sub header number (blank if invalid)
        void setPageReferenceSubHeaderNumber(const AString& tshn);
        
        /// get the entire link in an "coded" text form
        AString getLinkAsCodedText() const;
        
        /// set the link from "coded" text form
        void setLinkFromCodedText(const AString& txt);
        
        // called to write XML
        void writeXML(XmlWriter& xmlWriter) const throw (XmlException);
        
        /// set element from text (used by SAX XML parser)
        void setElementFromText(const AString& elementName,
                                const AString& textValue);
        
        //
        //----- tags for reading and writing 
        //
        /// tag for reading and writing study metadata
        static const AString tagStudyMetaDataLink;
        
        /// tag for reading and writing study metadata
        static const AString tagPubMedID;
        
        /// tag for reading and writing study metadata
        static const AString tagTableNumber;
        
        /// tag for reading and writing study metadata
        static const AString tagTableSubHeaderNumber;
        
        /// tag for reading and writing study metadata
        static const AString tagFigureNumber;
        
        /// tag for reading and writing study metadata
        static const AString tagPanelNumberOrLetter;
        
        /// tag for reading and writing study metadata
        //static const AString tagPageNumber;
        
        /// tag for reading and writing study metadata
        static const AString tagPageReferencePageNumber;
        
        /// tag for reading and writing study metadata
        static const AString tagPageReferenceSubHeaderNumber;
        
    protected:
        /// copy helper
        void copyHelper(const StudyMetaDataLink& smdl);
        
        /// the PubMed ID (negative if project ID, 0 if invalid)
        AString m_pubMedID;
        
        /// the table number (blank if invalid)
        AString m_tableNumber;
        
        /// the table sub header number (blank if invalid)
        AString m_tableSubHeaderNumber;
        
        /// the figure number (blank if invalid)
        AString m_figureNumber;
        
        /// the panel letter/number (blank if invalid)
        AString m_panelNumberOrLetter;
        
        /// page reference page number (blank if invalid)
        AString m_pageReferencePageNumber;
        
        /// page reference sub header number (blank if invalid)
        AString m_pageReferenceSubHeaderNumber;
        
        // NOTE: IF MEMBERS ADDED UPDATE THE COPY HELPER
        
        
        friend class StudyMetaDataLinkSet;
    };

#ifdef __STUDY_META_DATA_LINK_MAIN__
    const AString StudyMetaDataLink::tagStudyMetaDataLink = "StudyMetaDataLink";
    const AString StudyMetaDataLink::tagPubMedID = "pubMedID";
    const AString StudyMetaDataLink::tagTableNumber = "tableNumber";
    const AString StudyMetaDataLink::tagTableSubHeaderNumber = "tableSubHeaderNumber";
    const AString StudyMetaDataLink::tagFigureNumber = "figureNumber";
    const AString StudyMetaDataLink::tagPanelNumberOrLetter = "panelNumberOrLetter";
    //const AString StudyMetaDataLink::tagPageNumber = "pageNumber";
    const AString StudyMetaDataLink::tagPageReferencePageNumber = "pageReferencePageNumber";
    const AString StudyMetaDataLink::tagPageReferenceSubHeaderNumber = "pageReferenceSubHeaderNumber";
#endif // __STUDY_META_DATA_LINK_MAIN__
} // namespace caret

#endif // __STUDY_META_DATA_LINK_H__


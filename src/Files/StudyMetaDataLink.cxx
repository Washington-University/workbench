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

#include <iostream>

#include <QStringList>

#define __STUDY_META_DATA_LINK_MAIN__
#include "StudyMetaDataLink.h"
#undef __STUDY_META_DATA_LINK_MAIN__
#include "XmlWriter.h"

using namespace caret;

/**
 * \class caret::StudyMetaDataLink 
 * \brief A link to study data.
 */
/**
 * constructor.
 */
StudyMetaDataLink::StudyMetaDataLink()
{
   clear();
}

/**
 * destructor.
 */
StudyMetaDataLink::~StudyMetaDataLink()
{
}

/**
 * copy constructor.
 */
StudyMetaDataLink::StudyMetaDataLink(const StudyMetaDataLink& smdl)
: CaretObject(smdl){
   copyHelper(smdl);
}

/**
 * assignment opertator.
 */
StudyMetaDataLink& 
StudyMetaDataLink::operator=(const StudyMetaDataLink& smdl)
{
   if (this != &smdl) {
       CaretObject::operator=(smdl);
       copyHelper(smdl);
   }
   
   return *this;
}
      
/**
 * copy helper.
 */
void 
StudyMetaDataLink::copyHelper(const StudyMetaDataLink& smdl)
{
   m_pubMedID = smdl.m_pubMedID;
   m_tableNumber = smdl.m_tableNumber;
   m_tableSubHeaderNumber = smdl.m_tableSubHeaderNumber;
   m_figureNumber = smdl.m_figureNumber;
   m_panelNumberOrLetter = smdl.m_panelNumberOrLetter;
   m_pageReferencePageNumber = smdl.m_pageReferencePageNumber;
   m_pageReferenceSubHeaderNumber = smdl.m_pageReferenceSubHeaderNumber;
}

/**
 * equality operator.
 */
bool 
StudyMetaDataLink::operator==(const StudyMetaDataLink& smdl) const
{
   const bool theSame = 
      ((m_pubMedID == smdl.m_pubMedID) &&
       (m_tableNumber == smdl.m_tableNumber) &&
       (m_tableSubHeaderNumber == smdl.m_tableSubHeaderNumber) &&
       (m_figureNumber == smdl.m_figureNumber) &&
       (m_panelNumberOrLetter == smdl.m_panelNumberOrLetter) &&
       (m_pageReferencePageNumber == smdl.m_pageReferencePageNumber) &&
       (m_pageReferenceSubHeaderNumber == smdl.m_pageReferenceSubHeaderNumber));
   return theSame;
}
      
/**
 * clear the link.
 */
void 
StudyMetaDataLink::clear()
{
   m_pubMedID = "0";
   m_tableNumber = "";
   m_tableSubHeaderNumber = "";
   m_figureNumber = "";
   m_panelNumberOrLetter = "";
   m_pageReferencePageNumber = "";
   m_pageReferenceSubHeaderNumber = "";
}
      
/**
 * set the table number (blank if invalid).
 */
void 
StudyMetaDataLink::setTableNumber(const AString& tn) 
{ 
   if (tn == "-1") {
      m_tableNumber = "";
   }
   else {
      m_tableNumber = tn; 
   }
}

/**
 * set the table sub header number (blank if invalid).
 */
void 
StudyMetaDataLink::setTableSubHeaderNumber(const AString& tshn) 
{ 
   if (tshn == "-1") {
      m_tableSubHeaderNumber = "";
   }
   else {
      m_tableSubHeaderNumber = tshn; 
   }
}

/**
 * set the panel letter/number (blank if invalid).
 */
void 
StudyMetaDataLink::setFigurePanelNumberOrLetter(const AString& pnl)
{
   if (pnl == "-1") {
      m_panelNumberOrLetter = "";
   }
   else {
      m_panelNumberOrLetter = pnl; 
   }
}
      
/**
 * set the figure number (blank if invalid).
 */
void 
StudyMetaDataLink::setFigureNumber(const AString& fn) 
{ 
   if (fn == "-1") {
      m_figureNumber = "";
   }
   else {
      m_figureNumber = fn; 
   }
}

/**
 * set the page reference page number (blank if invalid).
 */
void 
StudyMetaDataLink::setPageReferencePageNumber(const AString& prpn) 
{ 
   if (prpn == "-1") {
      m_pageReferencePageNumber = "";
   }
   else {
      m_pageReferencePageNumber = prpn; 
   }
}

/**
 * set the page reference sub header number (blank if invalid).
 */
void 
StudyMetaDataLink::setPageReferenceSubHeaderNumber(const AString& tshn) 
{ 
   if (tshn == "-1") {
      m_pageReferenceSubHeaderNumber = "";
   }
   else {
      m_pageReferenceSubHeaderNumber = tshn; 
   }
}

/**
 * set element from text (used by SAX XML parser).
 */
void 
StudyMetaDataLink::setElementFromText(const AString& elementName,
                                      const AString& textValue)
{
   if (elementName == tagPubMedID) {
      setPubMedID(textValue);
   }
   else if (elementName == tagTableNumber) {
      setTableNumber(textValue);
   }
   else if (elementName == tagTableSubHeaderNumber) {
      setTableSubHeaderNumber(textValue);
   }
   else if (elementName == tagFigureNumber) {
      setFigureNumber(textValue);
   }
   else if (elementName == tagPanelNumberOrLetter) {
      setFigurePanelNumberOrLetter(textValue);
   }
   else if (elementName == tagPageReferencePageNumber) {
      setPageReferencePageNumber(textValue);
   }
   else if (elementName == tagPageReferenceSubHeaderNumber) {
      setPageReferenceSubHeaderNumber(textValue);
   }
   else {
      std::cout << "WARNING: unrecognized StudyMetaDataLink element ignored: "
                << elementName.toAscii().constData()
                << std::endl;
   }
}
      
/**
 * called to write XML.
 */
void
StudyMetaDataLink::writeXML(XmlWriter& xmlWriter) const throw (XmlException)
{
   xmlWriter.writeStartElement(tagStudyMetaDataLink);
   xmlWriter.writeElementCData(tagPubMedID, m_pubMedID);
   xmlWriter.writeElementCData(tagTableNumber, m_tableNumber);
   xmlWriter.writeElementCData(tagTableSubHeaderNumber, m_tableSubHeaderNumber);
   xmlWriter.writeElementCData(tagFigureNumber, m_figureNumber);
   xmlWriter.writeElementCData(tagPanelNumberOrLetter, m_panelNumberOrLetter);
   xmlWriter.writeElementCData(tagPageReferencePageNumber, m_pageReferencePageNumber);
   xmlWriter.writeElementCData(tagPageReferenceSubHeaderNumber, m_pageReferenceSubHeaderNumber);
   xmlWriter.writeEndElement();
}

/**
 * get the entire link in an "coded" text form.
 */
AString 
StudyMetaDataLink::getLinkAsCodedText() const
{
   //
   // Assemble into one string containing key/value pairs separated by a semi-colon
   //
   QStringList sl;
   sl << ("pubMedID=" + m_pubMedID)
      << ("tableNumber=" + m_tableNumber)
      << ("tableSubHeaderNumber=" + m_tableSubHeaderNumber)
      << ("figureNumber=" + m_figureNumber)
      << ("panelNumberOrLetter=" + m_panelNumberOrLetter)
      << ("pageReferencePageNumber=" + m_pageReferencePageNumber)
      << ("pageReferenceSubHeaderNumber=" + m_pageReferenceSubHeaderNumber);

   const AString s = sl.join(";");

   return s;
}

/**
 * set the link from "coded" text form.
 */
void 
StudyMetaDataLink::setLinkFromCodedText(const AString& txt)
{
   //
   // Clear this link
   //
   clear();

   //
   // Extract the key/value pairs that are separated by a semi-colon
   //
   const QStringList sl = txt.split(";", AString::SkipEmptyParts);
   for (int i = 0; i < sl.size(); i++) {
      const AString keyValueString = sl.at(i);
      //
      // Split with "=" into key/value pairs
      //
      const QStringList keyValueList = keyValueString.split("=", AString::SkipEmptyParts);
      if (keyValueList.size() == 2) {
         const AString key = keyValueList.at(0);
         const AString value = keyValueList.at(1).trimmed();
         
         if (key == "pubMedID") {
            setPubMedID(value);
         }
         else if (key == "tableNumber") {
            setTableNumber(value);
         }
         else if (key == "tableSubHeaderNumber") {
            setTableSubHeaderNumber(value);
         }
         else if (key == "figureNumber") {
            setFigureNumber(value);
         }
         else if (key == "panelNumberOrLetter") {
            setFigurePanelNumberOrLetter(value);
         }
         else if (key == "pageReferencePageNumber") {
            setPageReferencePageNumber(value);
         }
         else if (key == "pageReferenceSubHeaderNumber") {
            setPageReferenceSubHeaderNumber(value);
         }
         else {
            std::cout << "Unrecognized StudyMetaDataLink key: " 
                      << key.toAscii().constData() << std::endl;
         }
      }
   }
}
      

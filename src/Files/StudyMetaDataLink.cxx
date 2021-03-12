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

#include <iostream>

#include <QStringList>

#define __STUDY_META_DATA_LINK_MAIN__
#include "StudyMetaDataLink.h"
#undef __STUDY_META_DATA_LINK_MAIN__

#include "CaretLogger.h"
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
   if (elementName == XML_TAG_PUBMED_ID) {
      setPubMedID(textValue);
   }
   else if (elementName == XML_TAG_TABLE_NUMBER) {
      setTableNumber(textValue);
   }
   else if (elementName == XML_TAG_TABLE_SUB_HEADER_NUMBER) {
      setTableSubHeaderNumber(textValue);
   }
   else if (elementName == XML_TAG_FIGURE_NUMBER) {
      setFigureNumber(textValue);
   }
   else if (elementName == XML_TAG_PANEL_NUMBER_OR_LETTER) {
      setFigurePanelNumberOrLetter(textValue);
   }
   else if (elementName == XML_TAG_PAGE_REFERENCE_PAGE_NUMBER) {
      setPageReferencePageNumber(textValue);
   }
   else if (elementName == XML_TAG_PAGE_REFERENCE_SUB_HEADER_NUMBER) {
      setPageReferenceSubHeaderNumber(textValue);
   }
   else {
      CaretLogWarning("Unrecognized StudyMetaDataLink element ignored: "
                      + elementName);
   }
}
      
/**
 * called to write XML.
 */
void
StudyMetaDataLink::writeXML(XmlWriter& xmlWriter) const
{
   xmlWriter.writeStartElement(XML_TAG_STUDY_META_DATA_LINK);
   xmlWriter.writeElementCData(XML_TAG_PUBMED_ID, m_pubMedID);
   xmlWriter.writeElementCData(XML_TAG_TABLE_NUMBER, m_tableNumber);
   xmlWriter.writeElementCData(XML_TAG_TABLE_SUB_HEADER_NUMBER, m_tableSubHeaderNumber);
   xmlWriter.writeElementCData(XML_TAG_FIGURE_NUMBER, m_figureNumber);
   xmlWriter.writeElementCData(XML_TAG_PANEL_NUMBER_OR_LETTER, m_panelNumberOrLetter);
   xmlWriter.writeElementCData(XML_TAG_PAGE_REFERENCE_PAGE_NUMBER, m_pageReferencePageNumber);
   xmlWriter.writeElementCData(XML_TAG_PAGE_REFERENCE_SUB_HEADER_NUMBER, m_pageReferenceSubHeaderNumber);
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
#if QT_VERSION >= 0x060000
    const QStringList sl = txt.split(";", Qt::SkipEmptyParts);
#else
    const QStringList sl = txt.split(";", QString::SkipEmptyParts);
#endif
   for (int i = 0; i < sl.size(); i++) {
      const AString keyValueString = sl.at(i);
      //
      // Split with "=" into key/value pairs
      //
#if QT_VERSION >= 0x060000
       const QStringList keyValueList = keyValueString.split("=", Qt::SkipEmptyParts);
#else
      const QStringList keyValueList = keyValueString.split("=", QString::SkipEmptyParts);
#endif
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
                      << key.toLatin1().constData() << std::endl;
         }
      }
   }
}
      

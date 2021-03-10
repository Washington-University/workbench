
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
#include <set>

#include <QStringList>

#define __STUDY_META_DATA_LINK_SET_MAIN__
#include "StudyMetaDataLinkSet.h"
#undef __STUDY_META_DATA_LINK_SET_MAIN__

#include "XmlWriter.h"

using namespace caret;

/**
 * constructor.
 */
StudyMetaDataLinkSet::StudyMetaDataLinkSet()
{
   clear();
}

/**
 * destructor.
 */
StudyMetaDataLinkSet::~StudyMetaDataLinkSet()
{
   clear();
}

/**
 * add a StudyMetaDataLink.
 */
void 
StudyMetaDataLinkSet::addStudyMetaDataLink(const StudyMetaDataLink& smdl)
{
   m_links.push_back(smdl);
}

/**
 * remove all links.
 */
void 
StudyMetaDataLinkSet::clear()
{
   m_links.clear();
}

/**
 * get a StudyMetaDataLink.
 */
StudyMetaDataLink 
StudyMetaDataLinkSet::getStudyMetaDataLink(const int indx) const
{
   return m_links[indx];
}

/**
 * get a pointer to a StudyMetaDataLink.
 */
StudyMetaDataLink* 
StudyMetaDataLinkSet::getStudyMetaDataLinkPointer(const int indx)
{
   if ((indx >= 0) &&
       (indx < getNumberOfStudyMetaDataLinks())) {
      return &m_links[indx];
   }
   
   return NULL;
}
      
/**
 * set a study meta data link.
 */
void 
StudyMetaDataLinkSet::setStudyMetaDataLink(const int indx, const StudyMetaDataLink& smdl)
{
   m_links[indx] = smdl;
}
      
/**
 * get all linked PubMed IDs.
 */
void 
StudyMetaDataLinkSet::getAllLinkedPubMedIDs(std::vector<AString>& pmidsOut) const
{
   std::set<AString> pmidSet;
   const int num = getNumberOfStudyMetaDataLinks();
   for (int i = 0; i < num; i++) {
      const AString pmid = getStudyMetaDataLink(i).getPubMedID();
      pmidSet.insert(pmid);
   }
   pmidsOut.clear();
   pmidsOut.insert(pmidsOut.end(),
                   pmidSet.begin(), pmidSet.end());
}
      
/**
 * remove a study meta data link.
 */
void 
StudyMetaDataLinkSet::removeStudyMetaDataLink(const int indx)
{
   m_links.erase(m_links.begin() + indx);
}

/**
 * get the entire link set in an "coded" text form.
 */
AString 
StudyMetaDataLinkSet::getLinkSetAsCodedText() const
{
   QStringList sl;
   const int num = getNumberOfStudyMetaDataLinks();
   for (int i = 0; i < num; i++) {
      sl << getStudyMetaDataLink(i).getLinkAsCodedText();
   }

   const AString s = sl.join(encodedTextLinkSeparator);
      
   return s;
}

/**
 * set the link set from "coded" text form.
 */
void 
StudyMetaDataLinkSet::setLinkSetFromCodedText(const AString& txt)
{
   clear();
   const QStringList sl = txt.split(encodedTextLinkSeparator, Qt::SkipEmptyParts);
   
   for (int i = 0; i < sl.count(); i++) {
      StudyMetaDataLink smdl;
      smdl.setLinkFromCodedText(sl.at(i));
      m_links.push_back(smdl);
   }
}

/**
 * called to write XML.
 */
void
StudyMetaDataLinkSet::writeXML(XmlWriter& xmlWriter) const
{
   xmlWriter.writeStartElement(XML_TAG_STUDY_META_DATA_LINK_SET);
   const int num = getNumberOfStudyMetaDataLinks();
   for (int i = 0; i < num; i++) {
      StudyMetaDataLink smdl = getStudyMetaDataLink(i);
      smdl.writeXML(xmlWriter);
   }
   xmlWriter.writeEndElement();
}

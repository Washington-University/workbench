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
/*LICENSE_END*/
#ifndef __CIFTI_XML
#define __CIFTI_XML
#include "CiftiXMLElements.h"
#include "CiftiXMLReader.h"
#include "CiftiXMLWriter.h"
#include <QtCore>
/// Simple Container class for storing Cifti XML meta data
class CiftiXML {
public:
   //TODO create initializers for various types of XML meta data (Dense Connectivity, Dense Time Series, etc)
   /**
    * Default Constructor
    * 
    * Default Constructor
    */
   CiftiXML() { }
   /**
    * Constructor
    * 
    * Constructor, create class using already existing Cifti xml tree
    * @param xml_root 
    */
   CiftiXML(CiftiRootElement &xml_root) { m_root = xml_root; }
   /**
    * Constructor
    * 
    * Constructor, create class using ASCII formatted byte array that 
    * containes xml meta data text
    * @param bytes
    */
   CiftiXML(const QByteArray &bytes) { readXML(bytes); }
   /**
    * Constructor
    * 
    * Constructor, create class using QString that contains xml
    * meta data text
    * @param xml_string
    */
   CiftiXML(const QString &xml_string) { readXML(xml_string); }
   /**
    * Constructor
    * 
    * Constructor, create class using QXmlStreamReader.  
    * QXmlStreamReader is assumed to be reading from Cifti XML
    * Text.
    * @param xml_stream
    */
   CiftiXML(QXmlStreamReader &xml_stream) { readXML(xml_stream); }   
   /**
    * readXML
    * 
    * readXML, replacing the currently Cifti XML Root, if it exists
    * @param bytes an ASCII formatted byte array that contains Cifti XML data
    */
   void readXML(const QByteArray &bytes) { QString text(bytes);readXML(text);}
   /**
    * readXML
    * 
    * readXML, replacing the currently Cifti XML Root, if it exists
    * @param text QString that contains Cifti XML data
    */
   void readXML(const QString &text) {QXmlStreamReader xml(text); readXML(xml);}
   /**
    * readXML
    * 
    * readXML, replacing the currently Cifti XML Root, if it exists
    * @param xml_stream
    */
   void readXML(QXmlStreamReader &xml_stream) { parseCiftiXML(xml_stream,m_root);}
   /**
    * writeXML
    * 
    * write the Cifti XML data to the supplied QString
    * @param text
    */
   void writeXML(QString &text) { QXmlStreamWriter xml(&text); writeCiftiXML(xml,m_root);}
   /**
    * writeXML
    * 
    * write the Cifti XML data to the supplied byte array. 
    * @param bytes
    */
   void writeXML(QByteArray &bytes) { QXmlStreamWriter xml(&bytes); writeCiftiXML(xml,m_root);}
   
   /**
    * setXMLRoot
    * 
    * set the Cifti XML root
    * @param xml_root
    */
   void setXMLRoot (CiftiRootElement &xml_root) { m_root = xml_root; }
   /**
    * getXMLRoot
    * 
    * get a copy of the Cifti XML Root
    * @param xml_root
    */
   void getXMLRoot (CiftiRootElement &xml_root) { xml_root = m_root; }
protected:
   CiftiRootElement m_root;
};
#endif//__CIFTI_XML

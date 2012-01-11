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

#include <QFile>
#include <AString.h>
#include <QXmlParseException>
#include <QXmlSimpleReader> 
#include <QXmlAttributes>

#include "XmlAttributes.h"
#include "XmlSaxParserHandlerInterface.h"
#include "XmlSaxParserWithQt.h"

using namespace caret;

/**
 * Constructor.
 */
XmlSaxParserWithQt::XmlSaxParserWithQt()
{
    this->initializeMembersXmlSaxParserWithQt();
}

/**
 * Destructor.
 */
XmlSaxParserWithQt::~XmlSaxParserWithQt()
{
    
}

/**
 * Initialize members.
 */
void
XmlSaxParserWithQt::initializeMembersXmlSaxParserWithQt() 
{
    
}

/**
 * Parse the contents of the specified file using
 * the specified handler.
 * 
 * @param filename
 *    Name of file that is to be parsed.
 * @param handler
 *    Handler that will be called to process XML
 *    as it is read.
 * @throws XmlSaxParserException
 *    If an error occurs.
 */
void 
XmlSaxParserWithQt::parseFile(const QString& filename,
                   XmlSaxParserHandlerInterface* handler) throw (XmlSaxParserException)
{
    PrivateHandler privateHandler(handler);
    
    QXmlSimpleReader reader;
    reader.setContentHandler(&privateHandler);
    reader.setErrorHandler(&privateHandler);

    /*
     * buffer for data read
     */
    const qint64 oneMegaByte = 1048576;
    const qint64 bufferSize = oneMegaByte;
    char buffer[bufferSize];
    
    /*
     * Open file for reading.
     */
    QFile file(filename);
    if (file.open(QFile::ReadOnly) == false) {
        throw XmlSaxParserException("Unable to open file " + filename);
    }
    
    /*
     * the XML input source
     */
    QXmlInputSource xmlInput;
    
    int totalRead = 0;
    
    bool firstTime = true;
    while (file.atEnd() == false) {
        int numRead = file.read(buffer, bufferSize);
        if (numRead <= 0) {
            break;
        }
        
        totalRead += numRead;
        
        /*
         * Place the input data into the XML input
         */
        xmlInput.setData(QByteArray(buffer, numRead));
        
        /*
         * Process the data that was just read
         */
        if (firstTime) {
            if (reader.parse(&xmlInput, true) == false) {
                throw XmlSaxParserException(privateHandler.errorString());            
            }
        }
        else {
            if (reader.parseContinue() == false) {
                throw XmlSaxParserException(privateHandler.errorString());
            }
        }
        
        firstTime = false;
    }
    
    /*
     * Tells parser that there is no more data
     */
    xmlInput.setData(QByteArray());
    if (reader.parseContinue() == false) {
        throw XmlSaxParserException(privateHandler.errorString());
    }
}

/**
 * Parse the contents of the string using
 * the specified handler.
 * 
 * @param xmlString
 *    String whose contents is parsed.
 * @param handler
 *    Handler that will be called to process XML
 *    as it is read.
 * @throws XmlSaxParserException
 *    If an error occurs.
 */
void 
XmlSaxParserWithQt::parseString(const QString& xmlString,
                         XmlSaxParserHandlerInterface* handler) throw (XmlSaxParserException)
{
    PrivateHandler privateHandler(handler);
    
    QXmlSimpleReader reader;
    reader.setContentHandler(&privateHandler);
    reader.setErrorHandler(&privateHandler);
    
    /*
     * the XML input source
     */
    QXmlInputSource xmlInput;
    xmlInput.setData(xmlString);
    
    /*
     * Process the data that was just read
     */
    if (reader.parse(&xmlInput, true) == false) {
        throw XmlSaxParserException(privateHandler.errorString());            
    }
}


//=========================================================================================

XmlSaxParserWithQt::PrivateHandler::PrivateHandler(XmlSaxParserHandlerInterface* handler)
: QXmlDefaultHandler(), handler(handler)
{
}

XmlSaxParserWithQt::PrivateHandler::~PrivateHandler()
{
}

bool 
XmlSaxParserWithQt::PrivateHandler::error(const QXmlParseException& exception)
{
    try {
        XmlSaxParserException spe = fromQXmlParseException(exception);
        this->handler->error(spe);
    }
    catch (const XmlSaxParserException& e) {
        this->errorMessage = e.whatString();
        return false;
    }
    return true;
}

bool 
XmlSaxParserWithQt::PrivateHandler::fatalError(const QXmlParseException& exception)
{
    try {
        XmlSaxParserException spe = fromQXmlParseException(exception);
        this->handler->fatalError(spe);
    }
    catch (const XmlSaxParserException& e) {
        this->errorMessage = e.whatString();
        return false;
    }
    return true;
}

bool 
XmlSaxParserWithQt::PrivateHandler::warning(const QXmlParseException& exception)
{
    try {
        XmlSaxParserException spe = fromQXmlParseException(exception);
        this->handler->warning(spe);
    }
    catch (const XmlSaxParserException& e) {
        this->errorMessage = e.whatString();
        return false;
    }
    return true;
}


bool 
XmlSaxParserWithQt::PrivateHandler::startDocument()
{
    try {
        this->handler->startDocument();
    }
    catch (const XmlSaxParserException& e) {
        //this->errorMessage = e.whatString();
        this->errorMessage = e.whatString();
        return false;
    }
    return true;
}

bool 
XmlSaxParserWithQt::PrivateHandler::startElement(const QString& namespaceURI,
                  const QString& localName,
                  const QString& qName,
                  const QXmlAttributes& atts)

{
    try {
        XmlAttributes xmlAtts = fromQXmlAttributes(atts);
        this->handler->startElement(namespaceURI, 
                                    localName, 
                                    qName, 
                                    xmlAtts);
    }
    catch (const XmlSaxParserException& e) {
        //this->errorMessage = e.whatString();
        this->errorMessage = e.whatString();
        return false;
    }
    return true;
}

bool 
XmlSaxParserWithQt::PrivateHandler::endDocument()
{
    try {
        this->handler->endDocument();
    }
    catch (const XmlSaxParserException& e) {
        //this->errorMessage = QString::fromStdString(e.whatString());
        this->errorMessage = e.whatString();
        return false;
    }
    return true;
}

bool 
XmlSaxParserWithQt::PrivateHandler::endElement(const QString& namespaceURI,
                const QString& localName,
                const QString& qName)
{
    try {
        this->handler->endElement(namespaceURI, localName, qName);
    }
    catch (const XmlSaxParserException& e) {
        //this->errorMessage = QString::fromStdString(e.whatString());
        this->errorMessage = e.whatString();
        return false;
    }
    return true;
}


bool 
XmlSaxParserWithQt::PrivateHandler::characters(const QString& ch)
{
    try {
        this->handler->characters(ch.toStdString().c_str());
    }
    catch (const XmlSaxParserException& e) {
        //this->errorMessage = QString::fromStdString(e.whatString());
        this->errorMessage = e.whatString();
        return false;
    }
    return true;
}

QString
XmlSaxParserWithQt::PrivateHandler::errorString() const 
{ 
    return this->errorMessage; 
}
 
XmlSaxParserException 
XmlSaxParserWithQt::PrivateHandler::fromQXmlParseException(const QXmlParseException& e)
{
    XmlSaxParserException spe(e.message(), e.lineNumber(), e.columnNumber());
    return spe;
}

XmlAttributes 
XmlSaxParserWithQt::PrivateHandler::fromQXmlAttributes(const QXmlAttributes& atts)
{
    XmlAttributes attsOut;
    for (int32_t i = 0; i < atts.count(); i++) {
        attsOut.addAttribute(atts.qName(i), atts.value(i));
    }
    return attsOut;
}

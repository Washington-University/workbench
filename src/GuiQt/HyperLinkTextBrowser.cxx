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

#include <QKeyEvent>
#include <QScrollBar>

#include "AString.h"
#include "HyperLinkTextBrowser.h"

using namespace caret;

/**
 * \class HyperLinkTextBrowser
 * \brief Displays HTML text.
 *
 * Displays HTML text.  Input text may contain URLs
 * which are converted into hyperlinks.
 */

/**
 * Constructor
 */ 
HyperLinkTextBrowser::HyperLinkTextBrowser(QWidget* parent)
   : QTextBrowser(parent)
{
   //QT4setTextFormat(QTextBrowser::RichText);
}

/**
 * Destructor
 */
HyperLinkTextBrowser::~HyperLinkTextBrowser()
{
}

/**
 * Override of QT's QTextBrowser method.
 * Called when the user clicks a link in this browser.
 * @param url
 *    User for display.
 */
void
HyperLinkTextBrowser::setSource(const QUrl& url)
{
   const AString s(url.toString());
   
   if (s.startsWith("vocabulary://")) {
       this->appendHtml("Vocabulary links not supported yet.");
//      const AString name = s.mid(13);
//      BrainModelIdentification* bmi = theMainWindow->getBrainSet()->getBrainModelIdentification();
//      const AString idString(bmi->getIdentificationTextForVocabulary(true, name));
//      if (idString.isEmpty() == false) {
//         appendHtml(idString);
//      }
   }
   else {
       this->appendHtml("Displaying of web pages not supported yet.");
//      theMainWindow->displayWebPage(s);
   }
}

/**
 * Append text (Override of QT's QTextBrowser method).  
 * Any URLs in text are converted to hyperlinks.
 */
void
HyperLinkTextBrowser::append(const AString& textIn)
{
   //
   // See if string contains a URL
   //
   AString text;
   if (textIn.indexOf("http://") >= 0) {
      //
      // Insert the string with hyperlinks into text browser
      //
      text = textIn.convertURLsToHyperlinks(); 
   }
   else {
      text = textIn;
   }
   text.replace("\n", "<br>");
   
   AString displayText = toHtml();
   displayText.append("<br>");
   displayText.append(text);
   setHtml(displayText);

   //
   // Scroll to newest text (at end of scroll bar)
   //
   QScrollBar* vsb = verticalScrollBar();
   vsb->setValue(vsb->maximum());
}

/**
 * Append html.
 * @param html 
 *   HTML that is appended.
 */
void 
HyperLinkTextBrowser::appendHtml(const AString& html)
{
   AString displayText = toHtml();
   displayText.append("<br>");
   displayText.append(html);
   setHtml(displayText);

   //
   // Scroll to newest text (at end of scroll bar)
   //
   QScrollBar* vsb = verticalScrollBar();
   vsb->setValue(vsb->maximum());
}
      
/**
 * Set the text in the browser.
 * @param textIn
 *   Text for the browser.
 */
void
HyperLinkTextBrowser::setText(const AString& textIn)
{
   //
   // See if string contains a URL
   //
   AString displayText;
   if (textIn.indexOf("http://") != -1) {
      //
      // Insert the string with hyperlinks into text browser
      //
      displayText = textIn.convertURLsToHyperlinks(); 
   }
   else {
      displayText = textIn;
   }   
   displayText.replace("\n", "<br>");
   setHtml(displayText);

   //
   // Scroll to newest text (at end of scroll bar)
   //
   QScrollBar* vsb = verticalScrollBar();
   vsb->setValue(vsb->maximum());
}

/**
 * called if a key is pressed over the text browser.
 * @param key
 *    Key that is pressed.
 */
void 
HyperLinkTextBrowser::keyPressEvent(QKeyEvent* /*e*/)
{
   emit keyPressed();
}


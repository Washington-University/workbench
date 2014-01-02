#ifndef __PLAIN_TEXT_STRING_BUILDER_H__
#define __PLAIN_TEXT_STRING_BUILDER_H__

/*LICENSE_START*/
/*
 * Copyright 2013 Washington University,
 * All rights reserved.
 *
 * Connectome DB and Connectome Workbench are part of the integrated Connectome 
 * Informatics Platform.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the names of Washington University nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR  
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
 * SUBSTITUTE GOODS OR  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 * POSSIBILITY OF SUCH DAMAGE.
 */
/*LICENSE_END*/


#include "CaretObject.h"



namespace caret {

    class PlainTextStringBuilder : public CaretObject {
        
    public:
        PlainTextStringBuilder();
        
        PlainTextStringBuilder(const int32_t indentationNumberOfSpaces);
        
        virtual ~PlainTextStringBuilder();
        
        void clear();
        
        void pushIndentation();
        
        void popIndentation();
        
        /** Add the text preceded by indentation and followed by a newline */
        void addLine(const AString& text);
        
        /** Get the text */
        AString getText() const;
        
    private:
        PlainTextStringBuilder(const PlainTextStringBuilder&);

        PlainTextStringBuilder& operator=(const PlainTextStringBuilder&);
        
        AString m_text;
        
        int32_t m_indentationNumberOfSpaces;
        
        int32_t m_currentIndentationCount;
        
        // ADD_NEW_MEMBERS_HERE

    };
    
#ifdef __PLAIN_TEXT_STRING_BUILDER_DECLARE__
    // <PLACE DECLARATIONS OF STATIC MEMBERS HERE>
#endif // __PLAIN_TEXT_STRING_BUILDER_DECLARE__

} // namespace
#endif  //__PLAIN_TEXT_STRING_BUILDER_H__

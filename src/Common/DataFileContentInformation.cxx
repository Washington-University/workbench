
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

#include <algorithm>

#define __DATA_FILE_CONTENT_INFORMATION_DECLARE__
#include "DataFileContentInformation.h"
#undef __DATA_FILE_CONTENT_INFORMATION_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::DataFileContentInformation 
 * \brief Assembles and provides information about a data file.
 * \ingroup Common
 */

/**
 * Constructor.
 */
DataFileContentInformation::DataFileContentInformation()
: CaretObject()
{
    
}

/**
 * Destructor.
 */
DataFileContentInformation::~DataFileContentInformation()
{
}

/**
 * Add a name and value pair.
 * 
 * @param name
 *    The name.
 * @param value
 *    The value.
 */
void
DataFileContentInformation::addNameAndValue(const AString& name,
                                     const AString& value)
{
    m_namesAndValues.push_back(std::make_pair((name + ":"),
                                              value));
}

/**
 * Add a name and value pair.
 *
 * @param name
 *    The name.
 * @param value
 *    The value.
 */
void
DataFileContentInformation::addNameAndValue(const AString& name,
                                            const int32_t value)
{
    m_namesAndValues.push_back(std::make_pair((name + ":"),
                                              AString::number(value)));
}

/**
 * Add a name and value pair.
 *
 * @param name
 *    The name.
 * @param value
 *    The value.
 */
void
DataFileContentInformation::addNameAndValue(const AString& name,
                                            const int64_t value)
{
    m_namesAndValues.push_back(std::make_pair((name + ":"),
                                              AString::number(value)));
}

/**
 * Add a name and value pair.
 *
 * @param name
 *    The name.
 * @param value
 *    The value.
 */
void
DataFileContentInformation::addNameAndValue(const AString& name,
                                            const double value,
                                            const int32_t precision)
{
    m_namesAndValues.push_back(std::make_pair((name + ":"),
                                              AString::number(value, 'f', precision)));
}

/**
 * Add a name and value pair.
 *
 * @param name
 *    The name.
 * @param value
 *    The value.
 */
void
DataFileContentInformation::addNameAndValue(const AString& name,
                                            const bool value)
{
    m_namesAndValues.push_back(std::make_pair((name + ":"),
                                              AString::fromBool(value)));
}

/**
 * Add freeform text.
 *
 * @param text
 *     Text that is added.
 */
void
DataFileContentInformation::addText(const AString& text)
{
    m_text.append(text);
}

/**
 * @return All of the information in a string.
 * 
 * The names and labels are formatted so that the labels and
 * values are aligned.
 */
AString
DataFileContentInformation::getInformationInString() const
{
    AString textOut;
    
    const int32_t numNamesAndValues = static_cast<int32_t>(m_namesAndValues.size());
    if (numNamesAndValues > 0) {
        int32_t longestLabelLength = 0;
        for (int32_t i = 0; i < numNamesAndValues; i++) {
            longestLabelLength = std::max(m_namesAndValues[i].first.length(),
                                          longestLabelLength);
        }
        longestLabelLength += 3;
        
        for (int32_t i = 0; i < numNamesAndValues; i++) {
            AString label = m_namesAndValues[i].first;
            textOut.appendWithNewLine(label.leftJustified(longestLabelLength)
                                      + m_namesAndValues[i].second);
        }
        
        textOut.append("\n");
    }
    
    textOut.append(m_text);
    
    return textOut;
}


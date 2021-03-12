
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
    /*
     * Initialize options
     */
    setOptionFlag(OPTION_SHOW_MAP_INFORMATION, true);
    setOptionFlag(OPTION_SHOW_CIFTI_LABEL_MAPPING, false);
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
    QChar colonChar = ' ';
    if (name.indexNotOf(' ') >= 0) {
        colonChar = ':';
    }
    
    m_namesAndValues.push_back(std::make_pair((name + colonChar),
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
    addNameAndValue(name,
                    AString::number(value));
//    m_namesAndValues.push_back(std::make_pair((name + ":"),
//                                              AString::number(value)));
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
    addNameAndValue(name,
                    AString::number(value));
//    m_namesAndValues.push_back(std::make_pair((name + ":"),
//                                              AString::number(value)));
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
    addNameAndValue(name,
                    AString::number(value, 'f', precision));
//    m_namesAndValues.push_back(std::make_pair((name + ":"),
//                                              AString::number(value, 'f', precision)));
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
    addNameAndValue(name,
                    AString::fromBool(value));
//    m_namesAndValues.push_back(std::make_pair((name + ":"),
//                                              AString::fromBool(value)));
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
            longestLabelLength = std::max(static_cast<int32_t>(m_namesAndValues[i].first.length()),
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

/**
 * Is an option flag on?
 *
 * @param optionFlag
 *    The option flag.
 * @return 
 *    True if the option flag is on, else false.
 */
bool
DataFileContentInformation::isOptionFlag(const OptionFlag optionFlag) const
{
    std::map<OptionFlag, bool>::const_iterator iter = m_optionFlags.find(optionFlag);
    if (iter != m_optionFlags.end()) {
        return iter->second;
    }
    
    return false;
}

/**
 * Set an option flag.
 *
 * @param optionFlag
 *    The option flag.
 * @param flagValue
 *    New value for option flag.
 */
void
DataFileContentInformation::setOptionFlag(const OptionFlag optionFlag,
                                          const bool flagValue)
{
    std::map<OptionFlag, bool>::iterator iter = m_optionFlags.find(optionFlag);
    if (iter != m_optionFlags.end()) {
        iter->second = flagValue;
    }
    else {
        m_optionFlags.insert(std::pair<OptionFlag, bool>(optionFlag,
                                                              flagValue));
    }
}


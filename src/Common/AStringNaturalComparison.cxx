
/*LICENSE_START*/
/*
 *  Copyright (C) 2014 Washington University School of Medicine
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

#define __A_STRING_NATURAL_COMPARISON_DECLARE__
#include "AStringNaturalComparison.h"
#undef __A_STRING_NATURAL_COMPARISON_DECLARE__

#include "CaretAssert.h"
using namespace caret;


    
/**
 * \class caret::AStringNaturalComparison 
 * \brief Class for performing a "natural comparison" of strings.
 * \ingroup Common
 *
 * This class is designed for use as a function object with the
 * Standard Template Library.  However, it also contains a static
 * method for naturally comparing" two strings.
 *
 * Normal string comparison just compares the ASCII values of characters.
 * However, when the string contains numeric sequences, the strings are
 * sorted as expected.  This class will perform string comparison where
 * any sequence of numbers is treated as a 'single character'.
 * 
 * When the two 'characters' being compared are both non-numeric, they are
 * compared by their ASCII codes.  When the two 'characters' being compared
 * are both numbers, a numeric comparison is performed.  When one 'character'
 * is numeric and the other 'character' is non-numeric, the number is
 * considered "less than".
 *
 * Sequence produced by normal string comparison (note the position of 32abc):
 *   1xyz, 32abc, 4ab
 *
 * Sequence produced by natural string comparison (note the position of 32abc):
 *   1xyz, 4ab, 32abc
 */

/**
 * Constructor.
 */
AStringNaturalComparison::AStringNaturalComparison()
{
    
}

/**
 * Destructor.
 */
AStringNaturalComparison::~AStringNaturalComparison()
{
}

/**
 * Function object so that this class can be used as for 
 * comparison in Standard Template Library containers.
 *
 * Performs a NATURAL COMPARISON where a contiguous sequence
 * of digits is treated as a single character so that text
 * string with numbers in them are properly sorted.
 *
 * @param s1
 *   First string for comparison.
 * @param s2
 *   Second string for comparison.
 * @return
 *   Negative value if (s1 < s2), Positive if (s1 > s2), and 
 *   Zero if (s1 == s2).
 */
bool
AStringNaturalComparison::operator() (const AString& s1,
                                      const AString& s2) const
{
    const int32_t result = AStringNaturalComparison::compare(s1, s2);
    
//    std::cout << "Compare ("
//    << qPrintable(s1)
//    << ", "
//    << qPrintable(s2)
//    << "): "
//    << AString::fromBool(result)
//    << std::endl;

    if (result < 0) {
        return true;
    }
    
    
    return false;
}

/**
 * Static method for natural comparison of two strings.
 *
 * Performs a NATURAL COMPARISON where a contiguous sequence
 * of digits is treated as a single character so that text
 * string with numbers in them are properly sorted.
 *
 * @param string1
 *   First string for comparison.
 * @param string2
 *   Second string for comparison.
 * @return
 *   Negative value if (string1 < string2), Positive if (string1 > string2), and
 *   Zero if (string1 == string2).
 */
int32_t
AStringNaturalComparison::compare(const AString& string1,
                                  const AString& string2)
{
    const StringParser s1(string1);
    const StringParser s2(string2);
    
    bool s1IsNumber = false;
    bool s2IsNumber = false;

    /*
     * Loop through the 'characters' until corresponding
     * 'characters' do not match.
     *
     * Note that a consecutive sequence of digits is
     * considered a single 'character'.
     */
    while (s1.hasMore()
           && s2.hasMore()) {
        const int64_t ch1 = s1.nextChar(s1IsNumber);
        const int64_t ch2 = s2.nextChar(s2IsNumber);
        
        CaretAssert(ch1 >= 0);
        CaretAssert(ch2 >= 0);
        if (s1IsNumber && s2IsNumber) {
            /*
             * Both 'character's are numbers
             */
            if (ch1 < ch2) {
                return -1;
            }
            else if (ch1 > ch2) {
                return 1;
            }
        }
        else if (s1IsNumber) {
            return -1;
        }
        else if (s2IsNumber) {
            return 1;
        }
        else {
            /*
             * Both 'characters' are NOT numbers
             */
            if (ch1 < ch2) {
                return -1;
            }
            else if (ch1 > ch2) {
                return 1;
            }
        }
    }
    
    /*
     * The shorter string is considered "less than"
     */
    if (s1.hasMore()) {
        return 1;
    }
    else if (s2.hasMore()) {
        return -1;
    }
    
    /*
     * Strings must be identical.
     */
    return 0;
}


/* ===================================================================== */
/**
 * \class caret::AStringNaturalComparison::StringParser
 * \brief Class for "String parsing" that treats any consecutive sequence
 * of numbers as a single character.
 * \ingroup Common
 */

/**
 * Constructor.
 *
 * @param s
 *    String that will be parsed.
 */
AStringNaturalComparison::StringParser::StringParser(const AString& s)
: m_s(s),
m_pos(0),
m_len(s.length())
{
    
}

/**
 * Returns the "next character" in the string.  Any consecutive sequence of
 * digits is considered a single character.
 *
 * @param isNumberOut
 *    If the value returned is a number, this parameter will be true, 
 *    else false.
 * @return
 *    The unicode value for the next character or numeric value os a 
 *    sequence of digits.
 */
int64_t
AStringNaturalComparison::StringParser::nextChar(bool& isNumberOut) const
{
    isNumberOut = false;
    
    if (m_pos >= m_len) {
        return -1;
    }
    
    const QChar ch = m_s[m_pos];
    ++m_pos;
    
    if (ch.isDigit()) {
        int64_t numericValue = ch.digitValue();
        
        while (m_pos < m_len) {
            const QChar nextChar = m_s[m_pos];
            if (nextChar.isDigit()) {
                ++m_pos;
                numericValue = (numericValue * 10) + nextChar.digitValue();
            }
            else {
                break;
            }
        }
        
        isNumberOut = true;
        return numericValue;
    }
    else {
        return ch.unicode();
    }
    
    return -1;
}

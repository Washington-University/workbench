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


#ifndef __STRING_UTILITIES_H__
#define __STRING_UTILITIES_H__

#include <string>
#include <vector>
#include <stdint.h>

namespace caret {
    
/**
 * class contains static methods for handling C++ strings.
 */
class StringUtilities {

private:
    StringUtilities();
    
public:
    ~StringUtilities();
    
    
    static std::string toStdString(const std::string& qs);
    
    static std::string fromStdString(const std::string& ss);

    static std::string fromNumber(const int32_t i);
    
    static std::string fromNumber(const uint32_t i);
    
    static std::string fromNumber(const int64_t i);
    
    static std::string fromNumber(const uint64_t l);
    
//    static std::string fromNumber(const std::string::size_t st);
    
    static std::string fromNumber(const float f);
    
    static std::string fromNumber(const double d);
    
    static std::string fromNumbers(const std::vector<int32_t>& v, const std::string& separator);
    
    static std::string fromNumbers(const std::vector<uint32_t>& v, const std::string& separator);
    
    static std::string fromNumbers(const std::vector<int64_t>& v, const std::string& separator);
    
    static std::string fromNumbers(const std::vector<uint64_t>& v, const std::string& separator);
    
//    static std::string fromNumbers(const std::vector<std::string::size_t>& v, const std::string& separator);
    
    static std::string fromNumbers(const std::vector<float>& v, const std::string& separator);
    
    static std::string fromNumbers(const std::vector<double>& v, const std::string& separator);
    
    static int32_t toInt(const std::string& s);

    static float toFloat(const std::string& s);
    
    static int64_t toLong(const std::string& s);
    
    static std::vector<int32_t> toIntVector(const std::string& s);
    
    // set the digits right of decimal for float/string conversions
    static void setFloatDigitsRightOfDecimal(const int numRightOfDecimal);
    
    // replace occurances of string "findThis" in "s" with "replaceWith"
    static std::string replace(const std::string& s, const std::string& findThis, 
                                const std::string& replaceWith);
/*
      // sort elements case insensitive
      static void sortCaseInsensitive(std::vector<std::string>& elements,
                                      const bool reverseOrderFlag,
                                      const bool removeDuplicatesFlag);
      
   
      //  replace occurances of character "findThis" in "s" with "replaceWith"
      static std::string replace(const std::string& s, const char findThis, 
                        const char replaceWith);
   
      // split "s" into "tokens" splitting with any characters in "separators"
      static void token(const std::string& s, 
                  const std::string& separators,
                  std::vector<std::string>& tokens);
   
   
      // split "s" into "float tokens" splitting with any characters in "separators"
      static void token(const std::string& s, 
                  const std::string& separators,
                  std::vector<float>& tokenFloats);
   
      // split "s" into "double tokens" splitting with any characters in "separators"
      static void token(const std::string& s, 
                  const std::string& separators,
                  std::vector<double>& tokenDoubles);
   
      // split "s" into "int tokens" splitting with any characters in "separators"
      static void token(const std::string& s, 
                  const std::string& separators,
                  std::vector<int>& tokenInts);
      
      // split "s" into "bool tokens" splitting with any characters in "separators"
      static void token(const std::string& s,
                  const std::string& separators,
                  std::vector<bool>& tokenBools);

      // split "s" into tokens breaking at whitespace but maintaining string in double quotes
      static void tokenStringsWithQuotes(const std::string& s,
                                         std::stringList& stringsOut);
                                         
      // split "s" into "tokens" splitting with the literal string "separatorString"
      static void tokenSingleSeparator(const std::string& stin,
                                      const std::string& separatorString,
                                      std::vector<std::string>& tokens);
                                      
      // combine "tokens" into a string delinated by "separators"
      static std::string combine(const std::vector<std::string>& tokens,
                        const std::string& separator);
                        
      // combine "token floats" into a string delinated by "separators"
      static std::string combine(const std::vector<float>& tokenFloats,
                        const std::string& separator);
                        
      // combine "token floats" into a string delinated by "separators"
      static std::string combine(const float* tokenFloats,
                             const int numFloats,
                             const std::string& separator);

      // combine "token doubles" into a string delinated by "separators"
      static std::string combine(const std::vector<double>& tokenDoubles,
                        const std::string& separator);
                        
      // combine "token ints" into a string delinated by "separators"
      static std::string combine(const std::vector<int>& tokenInts,
                        const std::string& separator);
                        
      // combine "token ints" into a string delinated by "separators"
      static std::string combine(const int* tokenInts,
                             const int numInts,
                             const std::string& separator);

      // combine "token bools" into a string delinated by "separators"
      static std::string combine(const std::vector<bool>& tokenBools,
                             const std::string& separator);

      // determine if the string "s1" ends with the string "s2"
      static bool endsWith(const std::string& s1, const std::string& s2);
   
      // determine if the string "s1" starts with the string "s2"
      static bool startsWith(const std::string& s1, const std::string& s2);
   
      // return a string made lowercase
      static std::string makeLowerCase(const std::string& s);
   
      // return a string made uppercase
      static std::string makeUpperCase(const std::string& s);
   
   
      // setup a comment for storage (replace tabs with spaces, replace newlines with tabs)
      static std::string setupCommentForStorage(const std::string& stin);

      // setup a comment for display (replace tabs with newlines)
      static std::string setupCommentForDisplay(const std::string& stin);
      
      // trim the blanks and tabs from both ends of a string
      static std::string trimWhitespace(const std::string& stin);

      // right justify a string by prepending blanks to it so the
      // string contains "totalSize" characters.
      static std::string rightJustify(const std::string& s, 
                                      const int totalSize);

      // left justify a string by adding blanks to it so the
      // string contains "totalSize" characters.
      static std::string leftJustify(const std::string& s, 
                                      const int totalSize);

      // convert any URLs found in the string into an HTML hyperlink
      static std::string convertURLsToHyperlinks(const std::string& sIn);
      
      // convert a string to an unsigned byte
      static unsigned char toUnsignedByte(const std::string& s);
      
      // convert a string to an unsigned byte
      static void toNumber(const std::string& s, unsigned char& b);
   
      // convert a string to an integer
      static int toInt(const std::string& s);

      // convert a string to an int
      static void toNumber(const std::string& s, int& i);
   
      // convert an integer to a string
      static std::string fromNumber(const int i);
      
      // Convert an string to a float.  Handles "em" or "en" dash (various width dashes)
      static float toFloat(const std::string& s);

      // convert a string to a float
      static void toNumber(const std::string& s, float& f);
   
      // convert a float to a string
      static std::string fromNumber(const float i);
      
      // Convert an string to a double.  Handles "em" or "en" dash (various width dashes)
      static double toDouble(const std::string& s);

      // convert a string to a double
      static void toNumber(const std::string& s, double& d);
   
      // convert a double to a string
      static std::string fromNumber(const double d);
      
      // convert a string to an bool
      static bool toBool(const std::string& s);

      // convert an bool to a string
      static std::string fromBool(const bool b);
      
      // get the white space characters
      static std::string getWhiteSpaceCharacters();
      
      // find the first of any of the characters in "lookForChars"
      static int findFirstOf(const std::string& s, const std::string& lookForChars,
                             const int startPos = 0);
                             
      // linewrap a string with no more than maxCharsPerLine 
      static void lineWrapString(const int maxCharsPerLine, std::string& s);
*/
   protected:
      /// digits right of decimal for float/string conversions
      static int digitsRightOfDecimal;
};

#ifdef __STRING_UTILITIES_MAIN_H__
   int StringUtilities::digitsRightOfDecimal = 6;
#endif // __STRING_UTILITIES_MAIN_H__

} // namespace

#endif  // __STRING_UTILITIES_H__

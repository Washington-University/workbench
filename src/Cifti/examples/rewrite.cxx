/*LICENSE_START*/
/*
 *  Copyright (C) 2015  Washington University School of Medicine
 *
 *  Permission is hereby granted, free of charge, to any person obtaining
 *  a copy of this software and associated documentation files (the
 *  "Software"), to deal in the Software without restriction, including
 *  without limitation the rights to use, copy, modify, merge, publish,
 *  distribute, sublicense, and/or sell copies of the Software, and to
 *  permit persons to whom the Software is furnished to do so, subject to
 *  the following conditions:
 *
 *  The above copyright notice and this permission notice shall be included
 *  in all copies or substantial portions of the Software.
 *
 *  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 *  EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 *  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *  IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 *  CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 *  TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 *  SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
/*LICENSE_END*/

#include "CaretException.h"
#include "CiftiFile.h"
#include "MultiDimIterator.h"

#include <iostream>
#include <vector>

using namespace std;
using namespace caret;

/**\file rewrite.cxx
This program reads a Cifti file from argv[1], and writes it out to argv[2] with a second CiftiFile object.
It uses on-disk reading and writing, so DO NOT have both filenames point to the same file,
CiftiFile truncates without any warning when told to write to an existing file.

\include rewrite.cxx
*/

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "this program requires two arguments: an input cifti file, and an output filename to write it to" << endl;
        return 1;
    }
    try
    {
        CiftiFile inputFile(argv[1]);//on-disk reading by default
        //inputFile.convertToInMemory();//if you want to read it into memory first
        CiftiFile outputFile;
        outputFile.setWritingFile(argv[2]);//sets up on-disk writing with default writing version, from CiftiVersion's default constructor
        outputFile.setCiftiXML(inputFile.getCiftiXML());//the CiftiXML is how you access all the mapping information
        const vector<int64_t>& dims = inputFile.getDimensions();
        vector<float> scratchRow(dims[0]);//read/write a row at a time
        for (MultiDimIterator<int64_t> iter(vector<int64_t>(dims.begin() + 1, dims.end())); !iter.atEnd(); ++iter)
        {//helper class to iterate over 2D and 3D cifti with the same code - the "+ 1" is to drop the first dimension (row length)
            inputFile.getRow(scratchRow.data(), *iter);
            outputFile.setRow(scratchRow.data(), *iter);
        }
        outputFile.writeFile(argv[2]);//because we called setWritingFile with this filename (and default cifti version), this will return immediately
        //NOTE: if you call writeFile with a different writing version (takes its default from CiftiVersion constructor) than setWritingFile, it will rewrite the entire file after reading it into memory
    } catch (CiftiException& e) {
        cerr << "Caught CaretException: " + e.whatString() << endl;
        return 1;
    }
    return 0;
}

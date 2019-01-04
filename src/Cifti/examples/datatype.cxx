/*LICENSE_START*/
/*
 *  Copyright (C) 2017  Washington University School of Medicine
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

#include <iostream>
#include <vector>

using namespace std;
using namespace cifti;

/**\file datatype.cxx
This program reads a Cifti file from argv[1], and writes it out to argv[2] using 8-bit unsigned integer and data scaling.
It uses a single CiftiFile object to do this, for simplicity - to see how to do something similar with two objects,
which is more relevant for how you would do processing on cifti files, see rewrite.cxx.

\include datatype.cxx
*/

int main(int argc, char** argv)
{
    if (argc < 3)
    {
        cout << "usage: " << argv[0] << " <input cifti> <output cifti>" << endl;
        cout << "  rewrite the input cifti file to the output filename, using uint8 and data scaling." << endl;
        return 1;
    }
    try
    {
        CiftiFile inputFile(argv[1]);//on-disk reading by default
        inputFile.setWritingDataTypeAndScaling(NIFTI_TYPE_UINT8, -1.0, 6.0);//tells it to use this datatype to best represent this specified range of values [-1.0, 6.0] whenever this instance is written
        inputFile.writeFile(argv[2]);//if the output filename is the same as the input filename, CiftiFile actually detects this and reads the input into memory first
        //otherwise, it will read and write one row at a time, using very little memory
        //inputFile.setWritingDataTypeNoScaling(NIFTI_TYPE_FLOAT32);//this is how you would revert back to writing as float32 without rescaling
    } catch (CaretException& e) {
        cerr << "Caught CaretException: " + e.whatString() << endl;
        return 1;
    }
    return 0;
}

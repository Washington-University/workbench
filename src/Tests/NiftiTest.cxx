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

#include "NiftiTest.h"
using namespace caret;

NiftiTest::NiftiTest(const AString &identifier) : TestInterface(identifier)
{
}

void NiftiTest::execute()
{

}

NiftiHeaderTest::NiftiHeaderTest(const AString &identifier) : TestInterface(identifier)
{
}

void NiftiHeaderTest::execute()
{
 {   NiftiHeaderIO *headerIO = new NiftiHeaderIO("../../../wb_files/nifti/fcMRI1_nonlin_Subcortical_Smoothed_s6.nii");
    //test version check
    if(headerIO->getNiftiVersion() == 1)
    {
        Nifti1Header header;
        headerIO->getHeader(header);
        std::cout << header.getHeaderAsString()->toStdString() << std::endl;
    }
    else
    {
        //report error
        AString message;
        message = AString("Incorrect Nifti version detected, expected version 1, got ") + AString::number(headerIO->getNiftiVersion()) + AString(".\n");
        setFailed(message);
    }

}
       NiftiHeaderIO *headerIO = new NiftiHeaderIO("../../../wb_files/nifti/test.cii");

    if(headerIO->getNiftiVersion() == 2)
    {
        Nifti2Header header;
        headerIO->getHeader(header);
        std::cout << header.getHeaderAsString()->toStdString() << std::endl;
    }
    else
    {
        //report error
        AString message;
        message = AString("Incorrect Nifti version detected, expected version 2, got ") + AString::number(headerIO->getNiftiVersion()) + AString(".\n");
        setFailed(message);
    }

}


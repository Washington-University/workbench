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

#ifndef NIFTITEST_H
#define NIFTITEST_H

#include "TestInterface.h"
#include "NiftiHeaderIO.h"
#include "NiftiFile.h"

namespace caret {

class NiftiFileTest : public TestInterface
{
public:
    NiftiFileTest(const AString& identifier);
    virtual void execute();
    void testObjectCreateDestroy();
    void testNifti1ReadWrite();
};

class NiftiHeaderTest : public TestInterface
{
public:
    NiftiHeaderTest(const AString& identifier);
    virtual void execute();
    void readNifti1Header(AString filename, Nifti1Header &header);
    void readNifti2Header(AString filename, Nifti2Header &header);
    void writeNifti1Header(AString filename, Nifti1Header &header);
    void writeNifti2Header(AString filename, Nifti2Header &header);
};


}

#endif // NIFTITEST_H

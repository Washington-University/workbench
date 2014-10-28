#ifndef __PROGRAMPARAMETERS_H__
#define __PROGRAMPARAMETERS_H__

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


#include "CaretObject.h"

#include "ProgramParametersException.h"

#include <stdint.h>

#include <string>
#include <vector>

namespace caret {

/**
 * Simplifies access to program parameters.
 */
class ProgramParameters : public CaretObject {

public:
    ProgramParameters(int argc, const char *const argv[]);

    ProgramParameters();

    virtual ~ProgramParameters();
    
private:
    ProgramParameters(const ProgramParameters&);
    ProgramParameters& operator=(const ProgramParameters&);

private:
    void initializeMembersProgramParameters();

public:
    void addParameter(const AString& p);

    bool hasNext() const;

    void verifyAllParametersProcessed();

    AString nextString(const AString& parameterName);

    bool nextBoolean(const AString& parameterName);

    int32_t nextInt(const AString& parameterName);

    int64_t nextLong(const AString& parameterName);

    float nextFloat(const AString& parameterName);

    double nextDouble(const AString& parameterName);

    void backup();

    void remove();

    int32_t getParameterIndex() const;

    void setParameterIndex(const int32_t index);

    int32_t getNumberOfParameters() const;

    AString getParameter(const int32_t index) const;

    AString getAllParametersInString() const;

    AString getAllParametersQuotedInString() const;

    AString getProgramName() const;
    
private:
    /**The parameters. */
    std::vector<AString> parameters;

    /**Current index in parameters. */
    int32_t parameterIndex;

    AString programName;
};

} // namespace

#endif // __PROGRAMPARAMETERS_H__

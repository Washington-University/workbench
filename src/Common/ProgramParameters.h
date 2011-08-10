#ifndef __PROGRAMPARAMETERS_H__
#define __PROGRAMPARAMETERS_H__

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
    ProgramParameters(int argc, char* argv[]);

    ProgramParameters();

    virtual ~ProgramParameters();
    
private:
    ProgramParameters(const ProgramParameters&);
    ProgramParameters& operator=(const ProgramParameters&);

private:
    void initializeMembersProgramParameters();

public:
    void addParameter(const QString& p);

    bool hasNext() const;

    void verifyAllParametersProcessed()
            throw (ProgramParametersException);

    QString nextString(const QString& parameterName)
            throw (ProgramParametersException);

    bool nextBoolean(const QString& parameterName)
            throw (ProgramParametersException);

    int32_t nextInt(const QString& parameterName)
            throw (ProgramParametersException);

    int64_t nextLong(const QString& parameterName)
            throw (ProgramParametersException);

    float nextFloat(const QString& parameterName)
            throw (ProgramParametersException);

    double nextDouble(const QString& parameterName)
            throw (ProgramParametersException);

    void backup();

    void remove();

    int32_t getParameterIndex() const;

    void setParameterIndex(const int32_t index);

    int32_t getNumberOfParameters() const;

    QString getParameter(const int32_t index) const;

    QString getAllParametersInString() const;

    QString getProgramName() const;
    
private:
    /**The parameters. */
    std::vector<QString> parameters;

    /**Current index in parameters. */
    int32_t parameterIndex;

    QString programName;
};

} // namespace

#endif // __PROGRAMPARAMETERS_H__

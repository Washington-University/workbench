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

#include <sstream>
#include "CaretAssert.h"
#include "ProgramParameters.h"

using namespace caret;

/**
 * Constructor.
 * 
 * @param argc
 *    Number of parameters.
 * @param argv
 *    Array containing the parameters.
 */
ProgramParameters::ProgramParameters(int argc, char* argv[])
    : CaretObject()
{
    this->initializeMembersProgramParameters();
    
    if (argc > 0) {
        this->programName = argv[0];
    }
    for (int32_t i = 1; i < argc; i++) {
        this->addParameter(argv[i]);
    }
}

/**
 * Constructor.
 */
ProgramParameters::ProgramParameters()
    : CaretObject()
{
    this->initializeMembersProgramParameters();
}

/**
 * Destructor
 */
ProgramParameters::~ProgramParameters()
{
    this->initializeMembersProgramParameters();
}

void
ProgramParameters::initializeMembersProgramParameters()
{
    this->parameterIndex = 0;
    this->parameters.clear();
}
/**
 * Add a parameter.
 * @param p New parameter.
 *
 */
void
ProgramParameters::addParameter(const QString& p)
{
    this->parameters.push_back(p);
}

/**
 * See if there are more parameters available.
 * 
 * @return  true if more parameters available else false.
 *
 */
bool
ProgramParameters::hasNext() const
{
    return (this->parameterIndex < static_cast<int64_t>(this->parameters.size()));
}

/**
 * Verifies that all parameters have been processed.
 * 
 * @throws ProgramParametersException If there are more parameters.
 *
 */
void
ProgramParameters::verifyAllParametersProcessed()
            throw (ProgramParametersException)
{
    if (this->hasNext()) {
        throw new ProgramParametersException("There are too many parameters.");
    }
}

/**
 * Get the next parameter as a string.
 * 
 * @param parameterName - name of the parameter.
 * @return The parameter.
 * @throws ProgramParametersException If there are no more parameters.
 *
 */
QString
ProgramParameters::nextString(const QString& parameterName)
            throw (ProgramParametersException)
{
    if (this->hasNext() == false) {
        throw ProgramParametersException(parameterName + " is missing (No more parameters).");
    }
    
    CaretAssertVectorIndex(this->parameters, this->parameterIndex);
    
    QString s = this->parameters[this->parameterIndex];
    this->parameterIndex++;
    
    return s;
}

/**
 * Get the next parameter as a boolean
 * 
 * @param parameterName - name of the parameter.
 * @return boolean value of parameter.
 * 
 * @throws ProgramParametersException If there are no more parameters or the
 *           next parameter does not represent a boolean value.
 *
 */
bool
ProgramParameters::nextBoolean(const QString& parameterName)
            throw (ProgramParametersException)
{
    QString s = this->nextString(parameterName).toLower();
    if ((s == "true")
        || (s == "t")) {
        return true;
    }
    else if ((s == "false")
             || (s == "f")) {
        return false;
    }
    throw ProgramParametersException(parameterName 
                                         + " is not a boolean value (true/false) but is \""
                                         + s + "\".");
    return false;
}

/**
 * Get the next parameter as a int.
 * 
 * @param parameterName - name of the parameter.
 * @return  int value of parameter.
 * @throws ProgramParametersException  If there are no more parameters or the
 *           next parameter does not represent an integer value.
 *
 */
int32_t
ProgramParameters::nextInt(const QString& parameterName)
            throw (ProgramParametersException)
{
    QString s = this->nextString(parameterName);
    int32_t i = s.toInt();
    return i;
}

/**
 * Get the next parameter as a long.
 * 
 * @param parameterName - name of the parameter.
 * @return  inlongt value of parameter.
 * @throws ProgramParametersException  If there are no more parameters or the
 *           next parameter does not represent an integer value.
 *
 */
int64_t
ProgramParameters::nextLong(const QString& parameterName)
            throw (ProgramParametersException)
{
    QString s = this->nextString(parameterName);
    int64_t i = s.toLong();
    return i;
}

/**
 * Get the next parameter as a float.
 * 
 * @param parameterName - name of the parameter.
 * @return  float value of parameter.
 * @throws ProgramParametersException  If there are no more parameters or the
 *           next parameter does not represent an float value.
 *
 */
float
ProgramParameters::nextFloat(const QString& parameterName)
            throw (ProgramParametersException)
{
    QString s = this->nextString(parameterName);
    float f = s.toFloat();
    return f;
}

/**
 * Get the next parameter as a double.
 * 
 * @param parameterName - name of the parameter.
 * @return  double value of parameter.
 * @throws ProgramParametersException  If there are no more parameters or the
 *           next parameter does not represent an double value.
 *
 */
double
ProgramParameters::nextDouble(const QString& parameterName)
            throw (ProgramParametersException)
{
    QString s = this->nextString(parameterName);
    double d = s.toDouble();
    return d;
}

/**
 * Backup to the previous parameter.  If currently at the first 
 * parameter, this command does nothing.
 *
 */
void
ProgramParameters::backup()
{
    if (this->parameterIndex > 0) {
        this->parameterIndex--;
    }
}

/**
 * Remove the parameter that was last retrieved with one of the 
 * "next()" methods.
 */
void
ProgramParameters::remove()
{
    this->backup();
    CaretAssertVectorIndex(this->parameters, this->parameterIndex);
    this->parameters.erase(this->parameters.begin() + this->parameterIndex);
}

/**
 * Get the current index of the parameter iterator.
 * @return Index of parameter iterator.
 *
 */
int32_t
ProgramParameters::getParameterIndex() const
{
    return this->parameterIndex;
}

/**
 * Set the current index of the parameter iterator.
 * @param indx New index of parameter iterator.
 * @throws ParameterException If index is not valid for parameters
 * unless the index is zero and there are no parameters.
 *
 */
void
ProgramParameters::setParameterIndex(const int32_t indx)
{
    CaretAssertVectorIndex(this->parameters, indx);
    this->parameterIndex = indx;
    
}

/**
 * Get the number of parameters.
 *
 * @return  The number of parameters.
 *
 */
int32_t
ProgramParameters::getNumberOfParameters() const
{
    return this->parameters.size();
}

/**
 * Get a parameter at the specified index.
 * 
 * @param index - index of parameter.
 * @return  The parameter at the specified index.
 *
 */
QString
ProgramParameters::getParameter(const int32_t indx) const
{
    CaretAssertVectorIndex(this->parameters, indx);
    return this->parameters[indx];
}

/**
 * Get all of the parameters, separated by a space.
 * @return  All parameters in a String.
 *
 */
QString
ProgramParameters::getAllParametersInString() const
{
    QString str;
    
    int num = this->getNumberOfParameters();
    for (int i = 0; i < num; i++) {
        if (i > 0) {
            str += " ";
        }
        str += this->getParameter(i);
    }
    
    return str;
}

/**
 * Get the name of the program.
 * @return
 *   Name of the program.
 */
QString 
ProgramParameters::getProgramName() const
{
    return this->programName;
}


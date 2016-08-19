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
ProgramParameters::ProgramParameters(int argc, const char *const argv[])
    : CaretObject()
{
    this->initializeMembersProgramParameters();
    
    if (argc > 0) {
        this->programName = AString::fromLocal8Bit(argv[0]);
    }
    for (int32_t i = 1; i < argc; i++) {
        this->addParameter(AString::fromLocal8Bit(argv[i]));
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
ProgramParameters::addParameter(const AString& p)
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
           
{
    if (this->hasNext()) {
        throw ProgramParametersException("Unexpected parameter: " + nextString("unexpected"));
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
AString
ProgramParameters::nextString(const AString& parameterName)
           
{
    if (this->hasNext() == false) {
        throw ProgramParametersException(parameterName + " is missing (No more parameters).");
    }
    
    CaretAssertVectorIndex(this->parameters, this->parameterIndex);
    
    AString s = this->parameters[this->parameterIndex];
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
ProgramParameters::nextBoolean(const AString& parameterName)
           
{
    AString s = this->nextString(parameterName).toLower();
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
ProgramParameters::nextInt(const AString& parameterName)
           
{
    AString s = this->nextString(parameterName);
    bool ok = false;
    int32_t i = s.toInt(&ok);
    if (!ok)
    {
        throw ProgramParametersException(parameterName + " needs an integer, got \"" + s + "\".");
    }
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
ProgramParameters::nextLong(const AString& parameterName)
           
{
    AString s = this->nextString(parameterName);
    bool ok = false;
    int64_t i = s.toLong(&ok);
    if (!ok)
    {
        throw ProgramParametersException(parameterName + " needs an integer, got \"" + s + "\".");
    }
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
ProgramParameters::nextFloat(const AString& parameterName)
           
{
    AString s = this->nextString(parameterName);
    bool ok = false;
    float f = s.toFloat(&ok);
    if (!ok)
    {
        throw ProgramParametersException(parameterName + " needs a floating point, got \"" + s + "\".");
    }
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
ProgramParameters::nextDouble(const AString& parameterName)
           
{
    AString s = this->nextString(parameterName);
    bool ok = false;
    double d = s.toDouble(&ok);
    if (!ok)
    {
        throw ProgramParametersException(parameterName + " needs a floating point, got \"" + s + "\".");
    }
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
    CaretAssert(indx >= 0 && indx <= (int32_t)this->parameters.size());//less or equal to size is intentional, having an index of one past the end is a valid state, notably needed by setParameterIndex(0) on empty ProgramParameters
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
AString
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
AString
ProgramParameters::getAllParametersInString() const
{
    AString str;
    
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
 * Get all of the parameters, separated by a space.
 * @return  All parameters in a String.
 *
 */
AString
ProgramParameters::getAllParametersQuotedInString() const
{
    AString str;
    
    int num = this->getNumberOfParameters();
    for (int i = 0; i < num; i++) {
        if (i > 0) {
            str += " ";
        }
        str += "\"" + this->getParameter(i) + "\"";
    }
    
    return str;
}

/**
 * Get the name of the program.
 * @return
 *   Name of the program.
 */
AString 
ProgramParameters::getProgramName() const
{
    return this->programName;
}


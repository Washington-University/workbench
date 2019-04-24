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

#include "CaretAssert.h"
#include "CaretException.h"
#include "CaretLogger.h"
#include "CaretMathExpression.h"

#include <cmath>

using namespace caret;
using namespace std;

CaretMathExpression::CaretMathExpression(const AString& expression)
{
    m_input = expression;
    m_position = 0;
    m_end = m_input.size();
    m_root = orExpr();
    if (skipWhitespace())//if we DON'T hit the end of input, there are extra characters - like if the input is "x + 1 $blah"
    {
        throw CaretException("extra characters on end of expression input: '" + m_input.mid(m_position) + "'");
    }
    CaretLogFiner("parsed '" + expression + "' as '" + toString() + "'");
}

double CaretMathExpression::evaluate(const vector<float>& variableValues) const
{
    CaretAssert(variableValues.size() == m_varNames.size());
    return m_root->eval(variableValues);
}

vector<AString> CaretMathExpression::getVarNames() const
{
    vector<AString> ret(m_varNames.size());
    for (map<AString, int>::const_iterator iter = m_varNames.begin(); iter != m_varNames.end(); ++iter)
    {
        CaretAssert(iter->second < (int)ret.size());
        ret[iter->second] = iter->first;
    }
    return ret;
}

AString CaretMathExpression::getExpressionHelpInfo()
{
    AString ret = AString("Expressions consist of constants, variables, operators, parentheses, and functions, in infix notation, such as 'exp(-x + 3) * scale'.  ") +
        "Variables are strings of any length, using the characters a-z, A-Z, 0-9, and _, but may not take the name of a named constant.  " +
        "Currently, there is only one named constant, PI.  " +
        "The operators are +, -, *, /, ^, >, <, >=, <=, ==, !=, !, &&, ||.  " +
        "These behave as in C, except that ^ is exponentiation, i.e. pow(x, y), and takes higher precedence than other binary operators (also, '-3^-4^-5' means '-(3^(-(4^-5)))').  " +
        "The <=, >=, ==, and != operators are given a small amount of wiggle room, equal to one millionth of the smaller of the absolute values of the values being compared.\n\n" +
        "Comparison and logical operators return 0 or 1, you can do masking with expressions like 'x * (mask > 0)'.  " +
        "For all logical operators, an input is considered true iff it is greater than 0.  " +
        "The expression '0 < x < 5' is not syntactically wrong, but it will NOT do what is desired, because it is evaluated left to right, i.e. '((0 < x) < 5)', " +
        "which will always return 1, as both possible results of a comparison are less than 5.  A warning is generated if an expression of this type is detected.  " +
        "Use something like 'x > 0 && x < 5' to get the desired behavior.\n\n" +
        "Whitespace between elements is ignored, ' sin ( 2 * x ) ' is equivalent to 'sin(2*x)', but 's in(2*x)' is an error.  " +
        "Implied multiplication is not allowed, the expression '2x' will be parsed as a variable.  " +
        "Parentheses are (), do not use [] or {}.  " +
        "Functions require parentheses, the expression 'sin x' is an error.\n\n" +
        "The following functions are supported:\n\n";
    vector<MathFunctionEnum::Enum> myFuncs;
    MathFunctionEnum::getAllEnums(myFuncs);
    for (int i = 0; i < (int)myFuncs.size(); ++i)
    {
        ret += "   " + MathFunctionEnum::toName(myFuncs[i]) + ": " + MathFunctionEnum::toExplanation(myFuncs[i]) + "\n";
    }
    return ret;
}

double CaretMathExpression::MathNode::eval(const vector<float>& values) const
{
    double ret = 0.0;
    switch (m_type)
    {
        case OR:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            bool temp = (m_arguments[0]->eval(values) > 0.0);
            for (int i = 1; i < end; ++i)
            {
                if (temp) break;//lazy evaluation
                temp = (m_arguments[i]->eval(values) > 0.0);//don't need to actually do ||, we already know all the ones to the left are false
            }
            ret = temp ? 1.0 : 0.0;
            break;
        }
        case AND:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            bool temp = (m_arguments[0]->eval(values) > 0.0);
            for (int i = 1; i < end; ++i)
            {
                if (!temp) break;//lazy evaluation
                temp = (m_arguments[i]->eval(values) > 0.0);//don't need to actually do &&, we already know all the ones to the left are true
            }
            ret = temp ? 1.0 : 0.0;
            break;
        }
        case EQUAL:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            CaretAssert((int)m_invert.size() == end);
            ret = m_arguments[0]->eval(values);
            for (int i = 1; i < end; ++i)
            {
                double temp = m_arguments[i]->eval(values);
                float adjust = min(abs(ret), abs(temp)) / 1000000;//because == doesn't always work as expected, include a fudge factor based on the approximate precision of float
                bool equal = (ret >= m_arguments[i]->eval(values) - adjust) && (ret <= m_arguments[i]->eval(values) + adjust);
                if (m_invert[i])
                {
                    ret = equal ? 0.0 : 1.0;
                } else {
                    ret = equal ? 1.0 : 0.0;
                }
            }
            break;
        }
        case GREATERLESS:
        {
            int end = (int)m_arguments.size();//yes, you can chain < and >, but it will produce a parsing warning, as it evaluates left to right
            CaretAssert(end > 1);
            CaretAssert((int)m_invert.size() == end);
            CaretAssert((int)m_inclusive.size() == end);
            ret = m_arguments[0]->eval(values);
            for (int i = 1; i < end; ++i)
            {
                double temp = m_arguments[i]->eval(values);
                if (m_inclusive[i])
                {
                    float adjust = min(abs(ret), abs(temp)) / 1000000;//because == doesn't always work as expected, include a fudge factor based on the approximate precision of float
                    if (m_invert[i])
                    {
                        ret = (ret <= temp + adjust ? 1.0 : 0.0);//don't trust booleans to cast to 0 and 1, just because
                    } else {
                        ret = (ret >= temp - adjust ? 1.0 : 0.0);
                    }
                } else {
                    if (m_invert[i])
                    {
                        ret = (ret < temp ? 1.0 : 0.0);
                    } else {
                        ret = (ret > temp ? 1.0 : 0.0);
                    }
                }
            }
            break;
        }
        case ADDSUB:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            CaretAssert((int)m_invert.size() == end);
            ret = m_arguments[0]->eval(values);
            for (int i = 1; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret -= m_arguments[i]->eval(values);
                } else {
                    ret += m_arguments[i]->eval(values);
                }
            }
            break;
        }
        case MULTDIV:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            CaretAssert((int)m_invert.size() == end);
            ret = m_arguments[0]->eval(values);
            for (int i = 1; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret /= m_arguments[i]->eval(values);
                } else {
                    ret *= m_arguments[i]->eval(values);
                }
            }
            break;
        }
        case NOT:
            CaretAssert(m_arguments.size() == 1);
            ret = ((m_arguments[0]->eval(values)) > 0.0) ? 0.0 : 1.0;
            break;
        case NEGATE:
            CaretAssert(m_arguments.size() == 1);
            ret = -(m_arguments[0]->eval(values));
            break;
        case POW:
        {
            CaretAssert(m_arguments.size() == 2);//a^b^c always gets parsed into 2 power nodes, because -a^-b is -(a^(-b))
            ret = pow(m_arguments[0]->eval(values), m_arguments[1]->eval(values));
            break;
        }
        case FUNC:
        {
            switch (m_function)//this could be (partly) moved into MathFunctionEnum, but it wouldn't strictly be an enum class then
            {
                case MathFunctionEnum::SIN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = sin(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::COS:
                    CaretAssert(m_arguments.size() == 1);
                    ret = cos(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::TAN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = tan(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::ASIN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = asin(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::ACOS:
                    CaretAssert(m_arguments.size() == 1);
                    ret = acos(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::ATAN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = atan(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::SINH:
                    CaretAssert(m_arguments.size() == 1);
                    ret = sinh(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::COSH:
                    CaretAssert(m_arguments.size() == 1);
                    ret = cosh(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::TANH:
                    CaretAssert(m_arguments.size() == 1);
                    ret = tanh(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::ASINH:
                {
                    CaretAssert(m_arguments.size() == 1);
                    //ret = asinh(m_arguments[0].eval(values));//will work, and be preferred, when we use c++11, but doesn't work on windows with previous standard
                    double arg = m_arguments[0]->eval(values);
                    if (arg > 0)
                    {
                        ret = log(arg + sqrt(arg * arg + 1));
                    } else {
                        ret = -log(-arg + sqrt(arg * arg + 1));//special case negative for stability in large negatives
                    }
                    break;
                }
                case MathFunctionEnum::ACOSH:
                {
                    CaretAssert(m_arguments.size() == 1);
                    //ret = acosh(m_arguments[0].eval(values));
                    double arg = m_arguments[0]->eval(values);
                    ret = log(arg + sqrt(arg * arg - 1));
                    break;
                }
                case MathFunctionEnum::ATANH:
                {
                    CaretAssert(m_arguments.size() == 1);
                    //ret = atanh(m_arguments[0].eval(values));
                    double arg = m_arguments[0]->eval(values);
                    ret = 0.5 * log((1 + arg) / (1 - arg));
                    break;
                }
                case MathFunctionEnum::LN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = log(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::EXP:
                    CaretAssert(m_arguments.size() == 1);
                    ret = exp(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::LOG:
                    CaretAssert(m_arguments.size() == 1);
                    ret = log10(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::SQRT:
                    CaretAssert(m_arguments.size() == 1);
                    ret = sqrt(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::ABS:
                    CaretAssert(m_arguments.size() == 1);
                    ret = abs(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::FLOOR:
                    CaretAssert(m_arguments.size() == 1);
                    ret = floor(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::ROUND:
                {
                    CaretAssert(m_arguments.size() == 1);
                    double temp = m_arguments[0]->eval(values);//windows doesn't use c99 when compiling c++ earlier than c++11, so implement manually
                    if (temp > 0.0)
                    {
                        ret = floor(temp + 0.5);
                    } else {
                        ret = ceil(temp - 0.5);
                    }
                    break;
                }
                case MathFunctionEnum::CEIL:
                    CaretAssert(m_arguments.size() == 1);
                    ret = ceil(m_arguments[0]->eval(values));
                    break;
                case MathFunctionEnum::ATAN2:
                    CaretAssert(m_arguments.size() == 2);
                    ret = atan2(m_arguments[0]->eval(values), m_arguments[1]->eval(values));
                    break;
                case MathFunctionEnum::MIN:
                {
                    CaretAssert(m_arguments.size() == 2);
                    ret = m_arguments[0]->eval(values);
                    double other = m_arguments[1]->eval(values);
                    if (ret > other) ret = other;
                    break;
                }
                case MathFunctionEnum::MAX:
                {
                    CaretAssert(m_arguments.size() == 2);
                    ret = m_arguments[0]->eval(values);
                    double other = m_arguments[1]->eval(values);
                    if (ret < other) ret = other;
                    break;
                }
                case MathFunctionEnum::MOD:
                {
                    CaretAssert(m_arguments.size() == 2);
                    double second = m_arguments[1]->eval(values);
                    if (second == 0.0)
                    {
                        ret = 0.0;
                    } else {
                        double first = m_arguments[0]->eval(values);
                        ret = first - second * floor(first / second);
                    }
                    break;
                }
                case MathFunctionEnum::CLAMP:
                {
                    CaretAssert(m_arguments.size() == 3);
                    ret = m_arguments[0]->eval(values);
                    double low = m_arguments[1]->eval(values);
                    double high = m_arguments[2]->eval(values);
                    if (ret < low)
                    {
                        ret = low;
                    }
                    if (ret > high)
                    {
                        ret = high;
                    }
                    break;
                }
                case MathFunctionEnum::INVALID:
                    CaretAssertMessage(0, "MathNode is type FUNC but INVALID function");
                    throw CaretException("parsing problem in CaretMathExpression");
            }
            break;
        }
        case VAR:
            CaretAssertVectorIndex(values, m_varIndex);
            ret = values[m_varIndex];
            break;
        case CONST:
            ret = m_constVal;
            break;
        case INVALID:
            CaretAssertMessage(0, "parsing left INVALID MathNode");
            throw CaretException("parsing problem in CaretMathExpression");
    }
    return ret;
}

AString CaretMathExpression::MathNode::toString(const std::vector<AString>& varNames) const
{
    AString ret = "";
    bool addParens = true;
    switch (m_type)
    {
        case OR:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            ret = m_arguments[0]->toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                ret += "||" + m_arguments[i]->toString(varNames);
            }
            break;
        }
        case AND:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            ret = m_arguments[0]->toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                ret += "&&" + m_arguments[i]->toString(varNames);
            }
            break;
        }
        case EQUAL:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            CaretAssert((int)m_invert.size() == end);
            ret = m_arguments[0]->toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret += "!=" + m_arguments[i]->toString(varNames);
                } else {
                    ret += "==" + m_arguments[i]->toString(varNames);
                }
            }
            break;
        }
        case GREATERLESS:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            CaretAssert((int)m_invert.size() == end);
            CaretAssert((int)m_inclusive.size() == end);
            ret = m_arguments[0]->toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                if (m_inclusive[i])
                {
                    if (m_invert[i])
                    {
                        ret += "<=" + m_arguments[i]->toString(varNames);
                    } else {
                        ret += ">=" + m_arguments[i]->toString(varNames);
                    }
                } else {
                    if (m_invert[i])
                    {
                        ret += "<" + m_arguments[i]->toString(varNames);
                    } else {
                        ret += ">" + m_arguments[i]->toString(varNames);
                    }
                }
            }
            break;
        }
        case ADDSUB:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            CaretAssert((int)m_invert.size() == end);
            ret = m_arguments[0]->toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret += "-" + m_arguments[i]->toString(varNames);
                } else {
                    ret += "+" + m_arguments[i]->toString(varNames);
                }
            }
            break;
        }
        case MULTDIV:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 1);
            CaretAssert((int)m_invert.size() == end);
            ret = m_arguments[0]->toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret += "/" + m_arguments[i]->toString(varNames);
                } else {
                    ret += "*" + m_arguments[i]->toString(varNames);
                }
            }
            break;
        }
        case NOT:
            CaretAssert(m_arguments.size() == 1);
            ret = "!" + m_arguments[0]->toString(varNames);
            break;
        case NEGATE:
            CaretAssert(m_arguments.size() == 1);
            ret = "-" + m_arguments[0]->toString(varNames);
            break;
        case POW:
            CaretAssert(m_arguments.size() == 2);
            ret = m_arguments[0]->toString(varNames) + "^" + m_arguments[1]->toString(varNames);
            break;
        case FUNC:
        {
            addParens = false;
            int end = (int)m_arguments.size();
            ret = MathFunctionEnum::toName(m_function) + "(";
            if (end > 0) ret += m_arguments[0]->toString(varNames);//allow for functions that take 0 arguments
            for (int i = 1; i < end; ++i)
            {
                ret += "," + m_arguments[i]->toString(varNames);
            }
            ret += ")";
            break;
        }
        case VAR:
            addParens = false;
            CaretAssertVectorIndex(varNames, m_varIndex);
            ret = varNames[m_varIndex];
            break;
        case CONST:
            addParens = false;
            if (m_constName != "")
            {
                ret = m_constName;
            } else {
                ret = AString::number(m_constVal, 'g', 15);
            }
            break;
        case INVALID:
            CaretAssertMessage(0, "toString called on invalid MathNode");
            throw CaretException("parsing problem in CaretMathExpression");
    }
    if (addParens) ret = "(" + ret + ")";//parenthesize almost everything
    return ret;
}

AString CaretMathExpression::toString() const
{
    return m_root->toString(getVarNames());
}

bool CaretMathExpression::skipWhitespace()//return false if end of input
{
    while (m_position < m_end && m_input[m_position].isSpace()) ++m_position;
    return m_position < m_end;
}
bool CaretMathExpression::accept(const char& c)
{
    if (!skipWhitespace()) return false;//end of input
    if (m_input[m_position] == c)
    {
        ++m_position;
        return true;
    }
    return false;
}
void CaretMathExpression::expect(const char& c, const int& exprStart)
{
    CaretAssert(exprStart < m_end);
    if (!skipWhitespace()) throw CaretException("unexpected end of input while parsing '" + m_input.mid(exprStart) + "', expected '" + AString(c) + "'");
    if (m_input[m_position] != c) throw CaretException("expected '" + AString(c) + "', got '" + m_input[m_position] + "' while parsing '" + m_input.mid(exprStart, m_position - exprStart + 1) + "'");
    ++m_position;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::orExpr()
{
    int start = m_position;//for error reporting
    CaretPointer<MathNode> temp = andExpr();
    if (accept('|'))
    {
        CaretPointer<MathNode> ret(new MathNode(MathNode::OR));
        ret->m_arguments.push_back(temp);
        do
        {
            expect('|', start);
            ret->m_arguments.push_back(andExpr());
        } while (accept('|'));
        return ret;
    }
    return temp;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::andExpr()
{
    int start = m_position;//for error reporting
    CaretPointer<MathNode> temp = equalExpr();
    if (accept('&'))
    {
        CaretPointer<MathNode> ret(new MathNode(MathNode::AND));
        ret->m_arguments.push_back(temp);
        do
        {
            expect('&', start);
            ret->m_arguments.push_back(equalExpr());
        } while (accept('&'));
        return ret;
    }
    return temp;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::equalExpr()
{
    int start = m_position;//for error reporting
    CaretPointer<MathNode> temp = greaterLessExpr();
    bool good = false, invert;//means not equal
    if (accept('='))
    {
        good = true;
        invert = false;
    } else if (accept('!')) {
        good = true;
        invert = true;
    }
    if (good)
    {
        CaretPointer<MathNode> ret(new MathNode(MathNode::EQUAL));
        ret->m_arguments.push_back(temp);
        ret->m_invert.push_back(false);//first one isn't used, they apply to the operator to the left of the argument
        do
        {
            expect('=', start);
            ret->m_arguments.push_back(greaterLessExpr());
            ret->m_invert.push_back(invert);
            good = false;
            if (accept('='))
            {
                good = true;
                invert = false;
            } else if (accept('!')) {
                good = true;
                invert = true;
            }
        } while (good);
        if (ret->m_arguments.size() > 2)
        {
            CaretLogWarning("expression '" + m_input.mid(start, m_position - start) + "' will do equality comparison left to right");
        }
        return ret;
    }
    return temp;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::greaterLessExpr()
{
    int start = m_position;//for error reporting
    CaretPointer<MathNode> temp = addSubExpr();
    bool good = false, invert, inclusive = false;//invert means less than
    if (accept('>'))
    {
        good = true;
        invert = false;
    } else if (accept('<')) {
        good = true;
        invert = true;
    }
    if (good && accept('=')) inclusive = true;
    if (good)
    {
        CaretPointer<MathNode> ret(new MathNode(MathNode::GREATERLESS));
        ret->m_arguments.push_back(temp);
        ret->m_invert.push_back(false);//first one isn't used, they apply to the operator to the left of the argument
        ret->m_inclusive.push_back(false);//ditto
        do
        {
            ret->m_arguments.push_back(addSubExpr());
            ret->m_invert.push_back(invert);
            ret->m_inclusive.push_back(inclusive);
            good = false;
            inclusive = false;
            if (accept('>'))
            {
                good = true;
                invert = false;
            } else if (accept('<')) {
                good = true;
                invert = true;
            }
            if (good && accept('=')) inclusive = true;
        } while (good);
        if (ret->m_arguments.size() > 2)
        {
            CaretLogWarning("expression '" + m_input.mid(start, m_position - start) + "' will do inequality comparison left to right");
        }
        return ret;
    }
    return temp;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::addSubExpr()
{
    CaretPointer<MathNode> temp = multDivExpr();
    bool good = false, invert;//means subtract
    if (accept('+'))
    {
        good = true;
        invert = false;
    } else if (accept('-')) {
        good = true;
        invert = true;
    }
    if (good)
    {
        CaretPointer<MathNode> ret(new MathNode(MathNode::ADDSUB));
        ret->m_arguments.push_back(temp);
        ret->m_invert.push_back(false);//first one isn't used, they apply to the operator to the left of the argument
        do
        {
            ret->m_arguments.push_back(multDivExpr());
            ret->m_invert.push_back(invert);
            good = false;
            if (accept('+'))
            {
                good = true;
                invert = false;
            } else if (accept('-')) {
                good = true;
                invert = true;
            }
        } while (good);
        return ret;
    }
    return temp;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::multDivExpr()
{
    CaretPointer<MathNode> temp = unaryExpr();
    bool good = false, invert;//means divide
    if (accept('*'))
    {
        good = true;
        invert = false;
    } else if (accept('/')) {
        good = true;
        invert = true;
    }
    if (good)
    {
        CaretPointer<MathNode> ret(new MathNode(MathNode::MULTDIV));
        ret->m_arguments.push_back(temp);
        ret->m_invert.push_back(false);//first one isn't used, they apply to the operator to the left of the argument
        do
        {
            ret->m_arguments.push_back(unaryExpr());
            ret->m_invert.push_back(invert);
            good = false;
            if (accept('*'))
            {
                good = true;
                invert = false;
            } else if (accept('/')) {
                good = true;
                invert = true;
            }
        } while (good);
        return ret;
    }
    return temp;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::unaryExpr()
{
    CaretPointer<MathNode> ret;
    if (accept('-'))
    {
        ret.grabNew(new MathNode(MathNode::NEGATE));
        ret->m_arguments.push_back(unaryExpr());//allow -----x and other silliness, because it is more complicated to try to prevent it
        return ret;
    } else if (accept('!')) {
        ret.grabNew(new MathNode(MathNode::NOT));
        ret->m_arguments.push_back(unaryExpr());//similarly, !!!!x...and !!--!---!!!!-!!-!!!1, etc
        return ret;
    } else if (accept('+')) {//unary plus does nothing
        return unaryExpr();//this is not infinitely recursive, accept() removes a char of input
    }
    return powExpr();
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::powExpr()
{
    CaretPointer<MathNode> temp = funcExpr();
    if (accept('^'))
    {
        CaretPointer<MathNode> ret(new MathNode(MathNode::POW));
        ret->m_arguments.push_back(temp);
        ret->m_arguments.push_back(unaryExpr());//because -x^y is -(x^y), but x^-y is x^(-y)
        return ret;
    }
    return temp;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::funcExpr()
{
    int start = m_position;
    if (accept('('))
    {
        CaretPointer<MathNode> ret = orExpr();
        expect(')', start);
        return ret;
    }//and now a non-predictive bit - checking for valid function name
    int funcnameEnd = m_input.indexOf('(', m_position);
    if (funcnameEnd != -1)
    {
        bool ok = false;
        MathFunctionEnum::Enum myfunc = MathFunctionEnum::fromName(m_input.mid(m_position, funcnameEnd - m_position).trimmed(), &ok);
        if (ok)
        {
            int numArgs = -1;
            switch(myfunc)
            {
                case MathFunctionEnum::SIN:
                case MathFunctionEnum::COS:
                case MathFunctionEnum::TAN:
                case MathFunctionEnum::ASIN:
                case MathFunctionEnum::ACOS:
                case MathFunctionEnum::ATAN:
                case MathFunctionEnum::SINH:
                case MathFunctionEnum::COSH:
                case MathFunctionEnum::TANH:
                case MathFunctionEnum::ASINH:
                case MathFunctionEnum::ACOSH:
                case MathFunctionEnum::ATANH:
                case MathFunctionEnum::LN:
                case MathFunctionEnum::EXP:
                case MathFunctionEnum::LOG:
                case MathFunctionEnum::SQRT:
                case MathFunctionEnum::ABS:
                case MathFunctionEnum::FLOOR:
                case MathFunctionEnum::ROUND:
                case MathFunctionEnum::CEIL:
                    numArgs = 1;
                    break;
                case MathFunctionEnum::ATAN2:
                case MathFunctionEnum::MIN:
                case MathFunctionEnum::MAX:
                case MathFunctionEnum::MOD:
                    numArgs = 2;
                    break;
                case MathFunctionEnum::CLAMP:
                    numArgs = 3;
                    break;
                case MathFunctionEnum::INVALID://this should not happen
                    CaretAssert(false);
            }
            CaretPointer<MathNode> ret(new MathNode(MathNode::FUNC));
            ret->m_function = myfunc;
            m_position = funcnameEnd + 1;//skip the ( of the function
            if (!accept(')'))//zero arguments case
            {
                ret->m_arguments.push_back(orExpr());
                while (accept(','))
                {
                    ret->m_arguments.push_back(orExpr());
                }
                expect(')', start);
            }
            if ((int)ret->m_arguments.size() != numArgs) throw CaretException("function '" + MathFunctionEnum::toName(myfunc) +
                                                            "' takes " + AString::number(numArgs) + " argument(s), but was given " + AString::number(ret->m_arguments.size()) + ": '" +
                                                            m_input.mid(start, m_position - start) + "'");
            return ret;
        }
    }
    return terminal();//if it isn't parenthesis or a function, the only thing left is terminal
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::terminal()
{
    CaretPointer<MathNode> ret = tryLiteral();//try literal first, our relaxed rules for variable names overlap with integers
    if (ret != NULL) return ret;
    if (!skipWhitespace()) throw CaretException("unexpected end of input, expected operand");//now, try named constant/variable
    int varEnd = m_position;
    bool onlydigits = true;
    while (varEnd < m_end)
    {
        QChar thisChar = m_input[varEnd];//allow letters, numbers, and underscore
        if (!thisChar.isLetterOrNumber() && thisChar != '_')
        {
            break;
        }
        if (!thisChar.isDigit()) onlydigits = false;//if there are only digits, this is not a variable, tryLiteral failed on something that was intended to be a literal
        ++varEnd;
    }
    double constVal = 0.0f;
    AString identifier = m_input.mid(m_position, varEnd - m_position);
    if (identifier.size() == 0)//hit an invalid character or end of input before getting any valid characters
    {//figure out why we stopped, give appropriate error
        if (varEnd >= m_end) throw CaretException("unexpected end of input, expected operand");
        throw CaretException("unexpected character '" + AString(m_input[varEnd]) + "' at beginning of operand");//if it fails for all prefix unary, parens, function, literal, variable, then this character can never start an operand
    }
    if (onlydigits) throw CaretException("error parsing literal value beginning with '" + identifier + "'");
    m_position = varEnd;
    if (getNamedConstant(identifier, constVal))
    {
        ret.grabNew(new MathNode(MathNode::CONST));
        ret->m_constName = identifier;
        ret->m_constVal = constVal;
        return ret;
    }
    ret.grabNew(new MathNode(MathNode::VAR));
    map<AString, int>::const_iterator iter = m_varNames.find(identifier);
    if (iter == m_varNames.end())
    {
        int index = (int)m_varNames.size();
        m_varNames.insert(make_pair(identifier, index));
        ret->m_varIndex = index;
    } else {
        ret->m_varIndex = iter->second;
    }
    return ret;
}

CaretPointer<CaretMathExpression::MathNode> CaretMathExpression::tryLiteral()
{
    if (!skipWhitespace()) throw CaretException("unexpected end of input, expected operand");//now, try literal
    int litstart = m_position, litend = m_position;
    if (m_input[litend] == '-' || m_input[litend] == '+') ++litend;//allow literals to start with - or + : however, - will not happen, due to unary - (which does that so that -2^-2 works as -(2^(-2))
    bool havedot = false, haveexp = false;
    while (litend < m_end)
    {
        QChar mychar = m_input[litend];
        if (mychar.isDigit() || mychar == '.')//manually test for multiple '.' for better error messages, it can never be correct as a non-literal
        {
            ++litend;
            if (mychar == '.')
            {
                if (havedot) throw CaretException("multiple '.' characters in literal: '" + m_input.mid(litstart, litend - litstart) + "'");
                havedot = true;
            }
            continue;
        }
        if (mychar == 'e' || mychar == 'E') {//scientific notation, don't throw on multiple 'e', because '2e-3e' is subtraction of two variables
            if (haveexp) return CaretPointer<MathNode>();//but do stop early, as it won't be a literal
            haveexp = true;
            ++litend;
            if (litend >= m_end) break;
            if (m_input[litend] == '-' || m_input[litend] == '+') ++litend;//allow + or - after e
            continue;
        }//spaces are not allowed inside literals
        break;//yes, break at the end of a while loop - anything we don't recognize as valid ends the loop
    }
    bool ok = false;
    double litVal = m_input.mid(litstart, litend - litstart).toDouble(&ok);
    if (!ok) return CaretPointer<MathNode>();
    m_position = litend;
    CaretPointer<MathNode> ret(new MathNode(MathNode::CONST));
    ret->m_constVal = litVal;
    return ret;
}

bool CaretMathExpression::getNamedConstant(const AString& name, double& valueOut)
{
    if (name == "PI")
    {
        valueOut = 3.1415926535897932;//double can do about 16 decimal digits, give 17 for rounding
        return true;
    }
    return false;//presumably we will have more named constants later
}

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
    int end = expression.size();
    if (!parse(m_root, expression, 0, end))
    {
        throw CaretException("error parsing expression '" + expression + "'");
    }
    CaretLogInfo("parsed '" + expression + "' as '" + toString() + "'");
}

double CaretMathExpression::evaluate(const vector<float>& variableValues) const
{
    CaretAssert(variableValues.size() >= m_varNames.size());
    return m_root.eval(variableValues);
}

CaretMathExpression::MathNode::MathNode()
{
    m_type = INVALID;
    m_negate = false;
}

AString CaretMathExpression::getExpressionHelpInfo()
{
    AString ret = AString("Expressions consist of constants, variables, operators, parentheses, and functions, in infix notation, such as 'exp(-x + 3) * scale'.  ") +
        "Variables are strings of any length, using the characters a-z, A-Z, 0-9, and _, but may not take the name of a named constant.  " +
        "Currently, there is only one named constant, PI.  " +
        "The operators are +, -, *, /, ^, >, <, >=, <=.  These behave as in C, except that ^ is exponentiation, i.e. pow(x, y), and takes higher precedence than the rest.  " +
        "The <= and >= operators are given a small amount of wiggle room, equal to one millionth of the smaller of the absolute values of the values being compared.\n\n" +
        "Comparison operators return 0 or 1, you can do masking with expressions like 'x * (mask > 0)'.  " +
        "The expression '0 < x < 5' is not syntactically wrong, but it will NOT do what is desired, because it is evaluated left to right, i.e. '((0 < x) < 5)', " +
        "which will always return 1, as both possible results of a comparison are less than 5.  A warning is generated if an expression of this type is detected.  " +
        "Use '(x > 0) * (x < 5)' to get the desired behavior.\n\n" +
        "Whitespace between elements is ignored, ' sin ( 2 * x ) ' is equivalent to 'sin(2*x)', but 's in(2*x)' is an error.  " +
        "Implied multiplication is not allowed, the expression '2x' will be parsed as a variable, use '2 * x'.  " +
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
        case GREATERLESS:
        {
            int end = (int)m_arguments.size();//yes, you can chain < and >, but it will produce a parsing warning, as it evaluates left to right
            CaretAssert(end > 0);
            ret = m_arguments[0].eval(values);
            for (int i = 1; i < end; ++i)
            {
                double temp = m_arguments[i].eval(values);
                if (m_inclusive[i])
                {
                    float adjust = min(abs(ret), abs(temp)) / (1<<20);//because = doesn't always work as expected, include a fudge factor based on the approximate precision of float
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
            ret = 0.0;
            for (int i = 0; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret -= m_arguments[i].eval(values);
                } else {
                    ret += m_arguments[i].eval(values);
                }
            }
            break;
        }
        case MULTDIV:
        {
            int end = (int)m_arguments.size();
            ret = 1.0;
            for (int i = 0; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret /= m_arguments[i].eval(values);
                } else {
                    ret *= m_arguments[i].eval(values);
                }
            }
            break;
        }
        case POW:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 0);
            ret = m_arguments[0].eval(values);
            for (int i = 1; i < end; ++i)
            {
                ret = pow(ret, m_arguments[i].eval(values));
            }
            break;
        }
        case FUNC:
        {
            switch (m_function)//this COULD be moved into MathFunctionEnum, but it wouldn't strictly be an enum class then
            {
                case MathFunctionEnum::SIN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = sin(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::COS:
                    CaretAssert(m_arguments.size() == 1);
                    ret = cos(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::TAN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = tan(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::ASIN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = asin(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::ACOS:
                    CaretAssert(m_arguments.size() == 1);
                    ret = acos(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::ATAN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = atan(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::SINH:
                    CaretAssert(m_arguments.size() == 1);
                    ret = sinh(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::COSH:
                    CaretAssert(m_arguments.size() == 1);
                    ret = cosh(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::TANH:
                    CaretAssert(m_arguments.size() == 1);
                    ret = tanh(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::ASINH:
                {
                    CaretAssert(m_arguments.size() == 1);
                    //ret = asinh(m_arguments[0].eval(values));//will work, and be preferred, when we use c++11, but doesn't work on windows with previous standard
                    double arg = m_arguments[0].eval(values);
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
                    double arg = m_arguments[0].eval(values);
                    ret = log(arg + sqrt(arg * arg - 1));
                    break;
                }
                case MathFunctionEnum::ATANH:
                {
                    CaretAssert(m_arguments.size() == 1);
                    //ret = atanh(m_arguments[0].eval(values));
                    double arg = m_arguments[0].eval(values);
                    ret = 0.5 * log((1 + arg) / (1 - arg));
                    break;
                }
                case MathFunctionEnum::LN:
                    CaretAssert(m_arguments.size() == 1);
                    ret = log(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::EXP:
                    CaretAssert(m_arguments.size() == 1);
                    ret = exp(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::LOG:
                    CaretAssert(m_arguments.size() == 1);
                    ret = log10(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::SQRT:
                    CaretAssert(m_arguments.size() == 1);
                    ret = sqrt(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::ABS:
                    CaretAssert(m_arguments.size() == 1);
                    ret = abs(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::FLOOR:
                    CaretAssert(m_arguments.size() == 1);
                    ret = floor(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::CEIL:
                    CaretAssert(m_arguments.size() == 1);
                    ret = ceil(m_arguments[0].eval(values));
                    break;
                case MathFunctionEnum::ATAN2:
                    CaretAssert(m_arguments.size() == 2);
                    ret = atan2(m_arguments[0].eval(values), m_arguments[1].eval(values));
                    break;
                case MathFunctionEnum::MIN:
                {
                    CaretAssert(m_arguments.size() == 2);
                    ret = m_arguments[0].eval(values);
                    double other = m_arguments[1].eval(values);
                    if (ret > other) ret = other;
                    break;
                }
                case MathFunctionEnum::MAX:
                {
                    CaretAssert(m_arguments.size() == 2);
                    ret = m_arguments[0].eval(values);
                    double other = m_arguments[1].eval(values);
                    if (ret < other) ret = other;
                    break;
                }
                case MathFunctionEnum::MOD:
                {
                    CaretAssert(m_arguments.size() == 2);
                    double second = m_arguments[1].eval(values);
                    if (second == 0.0)
                    {
                        ret = 0.0;
                    } else {
                        double first = m_arguments[0].eval(values);
                        ret = first - second * floor(first / second);
                    }
                    break;
                }
                case MathFunctionEnum::CLAMP:
                {
                    CaretAssert(m_arguments.size() == 3);
                    ret = m_arguments[0].eval(values);
                    double low = m_arguments[1].eval(values);
                    double high = m_arguments[2].eval(values);
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
                case INVALID:
                    CaretAssertMessage(0, "MathNode is type FUNC but INVALID function");
                    throw CaretException("parsing problem in CaretMathExpression");
                    return 0.0;
            }
            break;
        }
        case VAR:
            CaretAssertVectorIndex(values, m_varIndex);
            ret = values[m_varIndex];
            break;
        case CONST:
            ret = m_constVal;//this should never need negating, but keep the pattern
            break;
        case INVALID:
            CaretAssertMessage(0, "parsing left INVALID MathNode");
            throw CaretException("parsing problem in CaretMathExpression");
            return 0.0;
    }
    if (m_negate)
    {
        return -ret;
    } else {
        return ret;
    }
}

AString CaretMathExpression::MathNode::toString(const std::vector<AString>& varNames) const
{
    AString ret = "";
    bool addParens = true;
    switch (m_type)
    {
        case GREATERLESS:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 0);
            ret = m_arguments[0].toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                if (m_inclusive[i])
                {
                    if (m_invert[i])
                    {
                        ret += "<=" + m_arguments[i].toString(varNames);
                    } else {
                        ret += ">=" + m_arguments[i].toString(varNames);
                    }
                } else {
                    if (m_invert[i])
                    {
                        ret += "<" + m_arguments[i].toString(varNames);
                    } else {
                        ret += ">" + m_arguments[i].toString(varNames);
                    }
                }
            }
            break;
        }
        case ADDSUB:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 0);
            ret = m_arguments[0].toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret += "-" + m_arguments[i].toString(varNames);
                } else {
                    ret += "+" + m_arguments[i].toString(varNames);
                }
            }
            break;
        }
        case MULTDIV:
        {
            int end = (int)m_arguments.size();
            CaretAssert(end > 0);
            ret = m_arguments[0].toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                if (m_invert[i])
                {
                    ret += "/" + m_arguments[i].toString(varNames);
                } else {
                    ret += "*" + m_arguments[i].toString(varNames);
                }
            }
            break;
        }
        case POW:
            CaretAssert(m_arguments.size() == 2);
            ret = m_arguments[0].toString(varNames) + "^" + m_arguments[1].toString(varNames);
            break;
        case FUNC:
        {
            addParens = false;
            int end = (int)m_arguments.size();
            CaretAssert(end > 0);
            ret = MathFunctionEnum::toName(m_function) + "(" + m_arguments[0].toString(varNames);
            for (int i = 1; i < end; ++i)
            {
                ret += "," + m_arguments[i].toString(varNames);
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
            ret = "???";
    }
    if (m_negate) addParens = true;
    if (addParens) ret = "(" + ret + ")";//parenthesize almost everything
    if (m_negate)
    {
        ret = "(-" + ret + ")";
    }
    return ret;
}

AString CaretMathExpression::toString() const
{
    return m_root.toString(m_varNames);
}

bool CaretMathExpression::parse(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    CaretAssert(start >= 0 && start <= end && end <= input.size());
    if (end - start == 0) return false;
    if (tryConst(node, input, start, end)) return true;
    if (tryGreaterLess(node, input, start, end)) return true;
    if (tryAddSub(node, input, start, end)) return true;
    if (tryMultDiv(node, input, start, end)) return true;
    if (tryUnaryMinus(node, input, start, end)) return true;
    if (tryPow(node, input, start, end)) return true;
    if (tryParen(node, input, start, end)) return true;
    if (tryFunc(node, input, start, end)) return true;
    if (tryVar(node, input, start, end)) return true;
    throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
    return false;
}

bool CaretMathExpression::tryUnaryMinus(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    int mystart = start;
    while (mystart < end && input[mystart].isSpace()) ++mystart;//trim whitespace
    if (mystart >= end) return false;
    if (input[mystart] != '-') return false;
    node.m_negate = !node.m_negate;//flip the sign and recurse without generating a new node, automatically collapse double or more negations in parenthesis, but don't allow "--x"
    if (tryPow(node, input, mystart + 1, end)) return true;
    if (tryParen(node, input, mystart + 1, end)) return true;
    if (tryFunc(node, input, mystart + 1, end)) return true;
    if (tryVar(node, input, mystart + 1, end)) return true;
    throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");//since first character is a -, failure to parse any of these is fatal, generate a generic error message rather than letting tryvar fail on it in parse
    return false;
}

bool CaretMathExpression::tryGreaterLess(CaretMathExpression::MathNode& node, const caret::AString& input, const int& start, const int& end)
{
    node.m_arguments.clear();//reset the node, in case it was previously partially attempted by something else
    node.m_invert.clear();
    node.m_inclusive.clear();
    int parenDepth = 0;
    bool ret = false;
    bool invertElement = false;//these initial values don't actually matter, they aren't used
    bool inclusive = false;
    int nextStart = start;
    int nextEnd = start;
    for (int i = start; i < end; ++i)
    {
        if (parenDepth == 0)
        {
            switch (input[i].toAscii())
            {
                case '<':
                    ret = true;//prepare to say successful parse, because we found a less than operator
                    nextEnd = i;
                    node.m_arguments.push_back(MathNode());
                    node.m_invert.push_back(invertElement);
                    node.m_inclusive.push_back(inclusive);
                    ++i;//now, search for an = sign
                    while (i < end && input[i].isSpace()) ++i;
                    if (i < end && input[i] == '=')
                    {
                        inclusive = true;
                    } else {
                        inclusive = false;
                        --i;
                    }
                    invertElement = true;//inversion applies to the element AFTER the less than sign, this is the element BEFORE it
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))//NOTE: this uneccessarily does tryGreaterLess on the element
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    nextStart = i + 1;
                    break;
                case '>':
                    ret = true;//prepare to say successful parse, because we found a greather than operator
                    nextEnd = i;
                    node.m_arguments.push_back(MathNode());
                    node.m_invert.push_back(invertElement);
                    node.m_inclusive.push_back(inclusive);
                    ++i;//now, search for an = sign
                    while (i < end && input[i].isSpace()) ++i;
                    if (i < end && input[i] == '=')
                    {
                        inclusive = true;
                    } else {
                        inclusive = false;
                        --i;
                    }
                    invertElement = false;//inversion applies to the element AFTER the greater than sign, this is the element BEFORE it
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))//NOTE: this uneccessarily does tryGreaterLess on the element
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    nextStart = i + 1;
                    break;
                case '('://NOTE: do not test for ')', so that we generate (hopefully) more specific error messages about unbalanced parens
                    ++parenDepth;
                    break;
                default:
                    break;
            }
        } else {
            switch (input[i].toAscii())
            {
                case '(':
                    ++parenDepth;
                    break;
                case ')':
                    --parenDepth;
                    break;
                default:
                    break;
            }
        }
    }
    if (ret)//don't try to parse the inside if we didn't find a greater or less operator, to avoid infinite recursion
    {
        node.m_arguments.push_back(MathNode());//and parse the final element
        node.m_invert.push_back(invertElement);
        node.m_inclusive.push_back(inclusive);
        if (!parse(node.m_arguments.back(), input, nextStart, end))//NOTE: this uneccessarily does tryGreaterLess on the element
        {
            throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
        }
        if (node.m_arguments.size() != 2)
        {
            CaretLogWarning("WARNING: expression '" + input.mid(start, end - start) + "' does a comparison on the result of a comparison, this is not likely to be useful.  " +
                            "To test whether a value lies between two values, use ((x > low) * (x < high)).");
        }
        node.m_type = MathNode::GREATERLESS;
    }
    return ret;
}

bool CaretMathExpression::tryAddSub(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    node.m_arguments.clear();//reset the node, in case it was previously partially attempted by something else
    node.m_invert.clear();
    int parenDepth = 0;
    bool ret = false;
    bool afterOp = true;
    bool invertElement = false;
    int nextStart = start;
    int nextEnd = start;
    for (int i = start; i < end; ++i)
    {
        if (parenDepth == 0)
        {
            if (input[i].isSpace()) continue;//ignore whitespace
            switch (input[i].toAscii())
            {
                case '-':
                    if (afterOp)//allow unary minus after operation
                    {
                        afterOp = false;
                        break;
                    }//otherwise, we have a new subelement to try to parse
                    afterOp = true;
                    ret = true;//prepare to say successful parse, because we found a minus operator
                    nextEnd = i;
                    node.m_arguments.push_back(MathNode());
                    node.m_invert.push_back(invertElement);
                    invertElement = true;//negation applies to the element AFTER the minus sign, this is the element BEFORE it
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))//NOTE: this uneccessarily does tryGreaterLess and tryAddSub on the element
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    nextStart = i + 1;
                    break;
                case '+':
                    if (afterOp)//allow + as part of a constant, ie (+13.6), let tryConst sort it out
                    {
                        afterOp = false;
                        break;
                    }
                    afterOp = true;
                    ret = true;//prepare to say successful parse, because we found a plus operator
                    nextEnd = i;
                    node.m_arguments.push_back(MathNode());
                    node.m_invert.push_back(invertElement);
                    invertElement = false;//negation applies to the element AFTER the plus sign, this is the element BEFORE it
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))//NOTE: this uneccessarily does tryGreaterLess and tryAddSub on the element
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    nextStart = i + 1;
                    break;
                case '*':
                case '/':
                case '^':
                case '<':
                case '=':
                    afterOp = true;//check for unary negation/negative constant after other operators
                    break;
                case '('://NOTE: we do not test for ')' in order to generate (hopefully) more specific error messages about unbalanced parens
                    afterOp = false;
                    ++parenDepth;
                    break;
                case ' ':
                    break;//spaces don't change state
                default:
                    afterOp = false;
                    break;
            }
        } else {
            switch (input[i].toAscii())
            {
                case '(':
                    ++parenDepth;
                    break;
                case ')':
                    --parenDepth;
                    break;
                default:
                    break;
            }
        }
    }
    if (ret)//don't try to parse the inside if we didn't find a plus or minus operator, to avoid infinite recursion
    {
        node.m_arguments.push_back(MathNode());//and parse the final element
        node.m_invert.push_back(invertElement);
        if (!parse(node.m_arguments.back(), input, nextStart, end))//NOTE: this uneccessarily does tryAddSub on the element
        {
            throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
        }
        node.m_type = MathNode::ADDSUB;
    }
    return ret;
}

bool CaretMathExpression::tryMultDiv(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    node.m_arguments.clear();//reset the node, in case it was previously partially attempted by something else
    node.m_invert.clear();
    int parenDepth = 0;
    bool ret = false;
    bool invertElement = false;
    int nextStart = start;
    int nextEnd = start;
    for (int i = start; i < end; ++i)
    {
        if (parenDepth == 0)
        {
            switch (input[i].toAscii())
            {
                case '/':
                    ret = true;//prepare to say successful parse, because we found a divide operator
                    nextEnd = i;
                    node.m_arguments.push_back(MathNode());
                    node.m_invert.push_back(invertElement);
                    invertElement = true;//negation applies to the element AFTER the divide sign, this is the element BEFORE it
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))//NOTE: this uneccessarily does tryGreaterLess, tryAddSub and tryMultDiv on the element
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    nextStart = i + 1;
                    break;
                case '*':
                    ret = true;//prepare to say successful parse, because we found a times operator
                    nextEnd = i;
                    node.m_arguments.push_back(MathNode());
                    node.m_invert.push_back(invertElement);
                    invertElement = false;//negation applies to the element AFTER the multiply sign, this is the element BEFORE it
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))//NOTE: this uneccessarily does tryGreaterLess, tryAddSub and tryMultDiv on the element
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    nextStart = i + 1;
                    break;
                case '('://NOTE: do not test for ')', so that we generate (hopefully) more specific error messages about unbalanced parens
                    ++parenDepth;
                    break;
                default:
                    break;
            }
        } else {
            switch (input[i].toAscii())
            {
                case '(':
                    ++parenDepth;
                    break;
                case ')':
                    --parenDepth;
                    break;
                default:
                    break;
            }
        }
    }
    if (ret)//don't try to parse the inside if we didn't find a times or divide operator, to avoid infinite recursion
    {
        node.m_arguments.push_back(MathNode());//and parse the final element
        node.m_invert.push_back(invertElement);
        if (!parse(node.m_arguments.back(), input, nextStart, end))//NOTE: this uneccessarily does tryAddSub and tryMultDiv on the element
        {
            throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
        }
        node.m_type = MathNode::MULTDIV;
    }
    return ret;
}

bool CaretMathExpression::tryPow(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    node.m_arguments.clear();//reset the node, in case it was previously partially attempted by something else
    int parenDepth = 0;
    bool ret = false;
    int nextStart = start;
    int nextEnd = start;
    for (int i = start; i < end; ++i)
    {
        if (parenDepth == 0)
        {
            switch (input[i].toAscii())
            {
                case '^':
                    ret = true;//prepare to say successful parse, because we found an exponentiation operator
                    nextEnd = i;
                    node.m_arguments.push_back(MathNode());
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))//NOTE: this uneccessarily does tryAddSub and tryMultDiv on the element
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    nextStart = i + 1;
                    break;
                case '('://NOTE: do not test for ')', so that we generate (hopefully) more specific error messages about unbalanced parens
                    ++parenDepth;
                    break;
                default:
                    break;
            }
        } else {
            switch (input[i].toAscii())
            {
                case '(':
                    ++parenDepth;
                    break;
                case ')':
                    --parenDepth;
                    break;
                default:
                    break;
            }
        }
    }
    if (ret)//don't try to parse the inside if we didn't find an exponentiation operator, to avoid infinite recursion
    {
        node.m_arguments.push_back(MathNode());//and parse the final element
        if (!parse(node.m_arguments.back(), input, nextStart, end))//NOTE: this uneccessarily does tryAddSub and tryMultDiv on the element
        {
            throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
        }
        node.m_type = MathNode::POW;
    }
    return ret;
}

bool CaretMathExpression::tryParen(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    int mystart = start;
    while (mystart < end && input[mystart].isSpace()) ++mystart;//skip whitespace
    if (mystart >= end) return false;
    if (input[mystart] != '(') return false;//all operators have already been parsed out
    int myend = mystart + 1;
    bool found = false;
    int parendepth = 0;
    while (!found && myend < end)
    {
        if (parendepth == 0)
        {
            switch (input[myend].toAscii())
            {
                case '(':
                    ++parendepth;
                    break;
                case ')':
                    found = true;
                    break;
                default:
                    break;
            }
        } else {
            switch (input[myend].toAscii())
            {
                case '(':
                    ++parendepth;
                    break;
                case ')':
                    --parendepth;
                    break;
                default:
                    break;
            }
        }
        ++myend;
    }
    if (!found) throw CaretException("missing close parenthesis in expression '" + input.mid(start, end - start) + "'");//so anything else here is an error
    for (int i = myend; i < end; ++i)
    {
        if (!input[i].isSpace()) throw CaretException("trailing characters after close parenthesis in expression '" + input.mid(start, end - start));
    }
    if (!parse(node, input, mystart + 1, myend - 1))
    {
        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
    }
    return true;
}

bool CaretMathExpression::tryFunc(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    node.m_arguments.clear();//reset the node, in case it was previously partially attempted by something else
    int firstParen = input.indexOf("(", start);
    if (firstParen <= start || firstParen >= end) return false;//catch -1 and first character (, and going past the current substring
    AString funcName = input.mid(start, firstParen - start).trimmed();
    if (funcName.size() == 0) return false;//if there are only spaces before the paren, return false to generate the more generic error
    bool ok = false;
    MathFunctionEnum::Enum myFunc = MathFunctionEnum::fromName(funcName, &ok);
    if (!ok) throw CaretException("unknown function: '" + funcName + "'");//since we know there are nonspace characters, throw instead of returning false, to give a specific error message
    int numArgs = 0;
    switch(myFunc)
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
            return false;
    }
    node.m_arguments.clear();//reset the node, in case it was previously partially attempted by something else
    int parenDepth = 0;
    int nextStart = firstParen + 1;//we are now committed to the function, don't return false, but throw instead, there is no other way to parse this string
    int nextEnd = nextStart;
    for (int i = firstParen + 1; i < end; ++i)
    {
        if (parenDepth == 0)
        {
            switch (input[i].toAscii())
            {
                case ',':
                    nextEnd = i;
                    if ((int)(node.m_arguments.size() + 2) > numArgs) throw CaretException("function '" + funcName + "' takes " + AString::number(numArgs) + " arguments, error parsing '" +
                                                                                        input.mid(start, end - start) + "'");
                    node.m_arguments.push_back(MathNode());
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    nextStart = i + 1;
                    break;
                case '(':
                    ++parenDepth;
                    break;
                case ')':
                    nextEnd = i;
                    if ((int)(node.m_arguments.size() + 1) != numArgs) throw CaretException("function '" + funcName + "' takes " + AString::number(numArgs) + " arguments, error parsing '" +
                                                                                        input.mid(start, end - start) + "'");
                    for (int j = i + 1; j < end; ++j) if (!input[j].isSpace()) throw CaretException("trailing characters on function expression '" +
                                                                                            input.mid(start, end - start) + "'");
                    node.m_arguments.push_back(MathNode());
                    if (!parse(node.m_arguments.back(), input, nextStart, nextEnd))
                    {
                        throw CaretException("error parsing expression '" + input.mid(start, end - start) + "'");
                    }
                    node.m_type = MathNode::FUNC;
                    node.m_function = myFunc;
                    return true;
                    break;
                default:
                    break;
            }
        } else {
            switch (input[i].toAscii())
            {
                case '(':
                    ++parenDepth;
                    break;
                case ')':
                    --parenDepth;
                    break;
                default:
                    break;
            }
        }
    }
    throw CaretException("missing close parenthesis in function expression '" + input.mid(start, end - start) + "'");
    return false;
}

bool CaretMathExpression::tryConst(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    node.m_arguments.clear();
    if (getNamedConstant(input.mid(start, end - start).trimmed(), node.m_constVal))
    {
        node.m_type = MathNode::CONST;
        node.m_constName = input.mid(start, end - start).trimmed();
        return true;
    }
    bool ok = false;
    node.m_constVal = input.mid(start, end - start).trimmed().toDouble(&ok);
    if (ok)
    {
        node.m_type = MathNode::CONST;
        return true;
    }
    return false;
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

bool CaretMathExpression::tryVar(CaretMathExpression::MathNode& node, const AString& input, const int& start, const int& end)
{
    node.m_arguments.clear();
    AString varName = input.mid(start, end - start).trimmed();
    int numChars = varName.size();
    if (numChars == 0) return false;
    for (int i = 0; i < numChars; ++i)
    {
        char inchar = varName[i].toAscii();
        switch (inchar)
        {
            case '<':
            case '>':
            case '+':
            case '-':
            case '*':
            case '/':
            case '^':
            case '(':
            case ')'://these shouldn't happen, tryVar comes last, so just return false to generate the generic error
                //throw CaretException("found operator character in tryVar for '" + input.mid(start, end - start) + "'");
                return false;
            default:
                break;
        }//allow a-z, A-Z, 0-9, _
        if ((inchar < 'a' || inchar > 'z') && (inchar < 'A' || inchar > 'Z') && (inchar < '0' || inchar > '9') &&
            inchar != '_')
        {
            throw CaretException(AString("the character '") + inchar + "' is not allowed in variable name '" + input.mid(start, end - start) + "'");//instead of returning false, since we know tryVar is last
        }
    }
    int i;
    for (i = 0; i < (int)m_varNames.size(); ++i)
    {
        if (varName == m_varNames[i]) break;
    }
    if (i == (int)m_varNames.size()) m_varNames.push_back(varName);
    node.m_varIndex = i;
    node.m_type = MathNode::VAR;
    return true;
}

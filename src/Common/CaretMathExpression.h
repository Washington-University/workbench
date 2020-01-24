#ifndef __CARET_MATH_EXPRESSION_H__
#define __CARET_MATH_EXPRESSION_H__

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

#include "AString.h"
#include "CaretPointer.h"
#include "MathFunctionEnum.h"

#include <map>
#include <vector>

namespace caret {

class CaretMathExpression
{
    struct MathNode
    {
        enum ExprType
        {
            INVALID,
            OR,
            AND,
            EQUAL,
            GREATERLESS,
            ADDSUB,
            MULTDIV,
            NOT,
            NEGATE,
            POW,
            FUNC,
            VAR,
            CONST
        };
        ExprType m_type;
        MathFunctionEnum::Enum m_function;
        AString m_constName;
        double m_constVal;
        int m_varIndex;
        std::vector<bool> m_invert;//whether it is subtract rather than add, or divide instead of multiply, or less instead of greater
        std::vector<bool> m_inclusive;//whether greater/less includes equals
        std::vector<CaretPointer<MathNode> > m_arguments;
        MathNode() { m_type = INVALID; m_function = MathFunctionEnum::INVALID; }
        MathNode(const ExprType& type) { m_type = type; m_function = MathFunctionEnum::INVALID; }
        double eval(const std::vector<float>& values) const;
        AString toString(const std::vector<AString>& varNames, bool addParens = true) const;
    };
    std::map<AString, int> m_varNames;
    AString m_input;
    int m_position, m_end;
    CaretPointer<MathNode> m_root;
    bool skipWhitespace();
    bool accept(const char& c);
    void expect(const char& c, const int& exprStart);
    CaretPointer<MathNode> orExpr();//hopefully we have enough stack space that we won't overflow without a hundred or so levels of parenthesis/functions/exponents
    CaretPointer<MathNode> andExpr();
    CaretPointer<MathNode> equalExpr();
    CaretPointer<MathNode> greaterLessExpr();
    CaretPointer<MathNode> addSubExpr();
    CaretPointer<MathNode> multDivExpr();
    CaretPointer<MathNode> unaryExpr();//accepts --x,  !!1, !--!-!!PI, etc
    CaretPointer<MathNode> powExpr();
    CaretPointer<MathNode> funcExpr();//also parenthesis
    CaretPointer<MathNode> terminal();//literal, const, variable
    CaretPointer<MathNode> tryLiteral();//NOTE: does not throw except on early end of input, returns NULL on failure
public:
    static AString getExpressionHelpInfo();
    static bool getNamedConstant(const AString& name, double& valueOut);
    CaretMathExpression(const AString& expression);
    double evaluate(const std::vector<float>& variableValues) const;
    std::vector<AString> getVarNames() const;
    AString toString() const;//the expression, with a lot of parentheses added
};

} // namespace

#endif //__CARET_MATH_EXPRESSION_H__

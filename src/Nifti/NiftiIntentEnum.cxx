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

#define __NIFTI_INTENT_DECLARE__
#include "NiftiIntentEnum.h"
#undef __NIFTI_INTENT_DECLARE__

#include "CaretAssert.h"

using namespace caret;


NiftiIntentEnum::NiftiIntentEnum(Enum e, 
                         const QString& enumName,
                         const int32_t integerCode,
                         const QString& name,
                         const QString& p1Name,
                         const QString& p2Name,
                         const QString& p3Name)
{
    this->e = e;
    this->enumName = enumName;
    this->integerCode = integerCode;
    this->name = name;
    this->p1Name = p1Name;
    this->p2Name = p2Name;
    this->p3Name = p3Name;
}

NiftiIntentEnum::NiftiIntentEnum()
{
    
}

NiftiIntentEnum::~NiftiIntentEnum()
{
    
}

void 
NiftiIntentEnum::initializeIntents()
{
    if (intentsCreatedFlag) {
        return;
    }
    intentsCreatedFlag = true;
    
    intents.push_back(
                      NiftiIntentEnum());
    
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_NONE,"NIFTI_INTENT_NONE", 0,"None","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CORREL,"NIFTI_INTENT_CORREL", 2,"Correlation Statistic","DOF","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_TTEST,"NIFTI_INTENT_TTEST", 3,"T-Statistic","DOF","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_FTEST,"NIFTI_INTENT_FTEST", 4,"F-Statistic","Numberator DOF","Denominator DOF",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_ZSCORE,"NIFTI_INTENT_ZSCORE", 5,"Z-Score","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CHISQ,"NIFTI_INTENT_CHISQ", 6,"Chi-Squared Distribution","DOF","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_BETA,"NIFTI_INTENT_BETA", 7,"Beta Distribution","a","b",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_BINOM,"NIFTI_INTENT_BINOM", 8,"Binomial Distribution",
                      "Number of Trials","Probability per Trial",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_GAMMA,"NIFTI_INTENT_GAMMA", 9,"Gamma Distribution","Shape","Scale",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_POISSON,"NIFTI_INTENT_POISSON", 10,"Poisson Distribution","Mean","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_NORMAL,"NIFTI_INTENT_NORMAL", 11,"Normal Distribution","Mean","Standard Deviation",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_FTEST_NONC,"NIFTI_INTENT_FTEST_NONC", 12,"F-Statistic Non-Central",
                           "Numerator DOF",
                           "Denominator DOF",
                           "Numerator Noncentrality Parameter"));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CHISQ_NONC,"NIFTI_INTENT_CHISQ_NONC", 13,"Chi-Squared Non-Central",
                           "DOF",
                           "Noncentrality Parameter",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_LOGISTIC,"NIFTI_INTENT_LOGISTIC", 14,"Logistic Distribution","Location","Scale",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_LAPLACE,"NIFTI_INTENT_LAPLACE", 15,"Laplace Distribution","Location","Scale",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_UNIFORM,"NIFTI_INTENT_UNIFORM", 16,"Uniform Distribution","Lower End","Upper End",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_TTEST_NONC,"NIFTI_INTENT_TTEST_NONC", 17,"T-Statistic Non-Central",
                           "DOF", 
                           "Noncentrality Parameter",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_WEIBULL,"NIFTI_INTENT_WEIBULL", 18,"Weibull Distribution","Location","Scale","Power"));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CHI,"NIFTI_INTENT_CHI", 19,"Chi Distribution",
                    "Half Normal Distribution",
                    "Rayleigh Distribution",
                    "Maxwell-Boltzmann Distribution"));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_INVGAUSS,"NIFTI_INTENT_INVGAUSS", 20,"Inverse Gaussian Distribution",
                         "MU",
                         "Lambda",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_EXTVAL,"NIFTI_INTENT_EXTVAL", 21,"Extreme Value Distribution",
                       "Location",
                       "Scale",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_PVAL,"NIFTI_INTENT_PVAL", 22,"P-Value","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_LOGPVAL,"NIFTI_INTENT_LOGPVAL", 23,"Log P-Value","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_LOG10PVAL,"NIFTI_INTENT_LOG10PVAL", 24,"Logn10 P-Value","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_ESTIMATE,"NIFTI_INTENT_ESTIMATE", 1001,"Estimate","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_LABEL,"NIFTI_INTENT_LABEL", 1002,"Label Indices","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_NEURONAME,"NIFTI_INTENT_NEURONAME", 1003,"Neuronames Indices","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_GENMATRIX,"NIFTI_INTENT_GENMATRIX", 1004,"General Matrix","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_SYMMATRIX,"NIFTI_INTENT_SYMMATRIX", 1005,"Symmetric Matrix","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_DISPVECT,"NIFTI_INTENT_DISPVECT", 1006,"Displacement Vector","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_VECTOR,"NIFTI_INTENT_VECTOR", 1007,"Vector","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_POINTSET,"NIFTI_INTENT_POINTSET", 1008,"Point Set","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_TRIANGLE,"NIFTI_INTENT_TRIANGLE", 1009,"Triangle","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_QUATERNION,"NIFTI_INTENT_QUATERNION", 1010,"Quaternion","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_DIMLESS,"NIFTI_INTENT_DIMLESS", 1011,"Dimensionless Number","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_TIME_SERIES,"NIFTI_INTENT_TIME_SERIES", 2001,"Time Series","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_NODE_INDEX,"NIFTI_INTENT_NODE_INDEX", 2002,"Node Index","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_RGB_VECTOR,"NIFTI_INTENT_RGB_VECTOR", 2003,"RGB","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_RGBA_VECTOR,"NIFTI_INTENT_RGBA_VECTOR", 2004,"RGBA","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_SHAPE,"NIFTI_INTENT_SHAPE", 2005,"Shape","","",""));
    
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CARET_DEFORMATION_NODE_INDICES,"NIFTI_INTENT_CARET_DEFORMATION_NODE_INDICES", 25000,"Deformation Node Indices","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CARET_DEFORMATION_NODE_AREAS,"NIFTI_INTENT_CARET_DEFORMATION_NODE_AREAS", 25001,"Deformation Node Areas","","",""));
    
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CONNECTIVITY_DENSE,"NIFTI_INTENT_CONNECTIVITY_DENSE", 3001,"Connectivity - Dense","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CONNECTIVITY_DENSE_TIME,"NIFTI_INTENT_CONNECTIVITY_DENSE_TIME", 3002,"Connectivity - Dense Time Series","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CONNECTIVITY_PARCELLATED,"NIFTI_INTENT_CONNECTIVITY_PARCELLATED", 3003,"Connectivity - Parcellated","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CONNECTIVITY_PARCELLATED_TIME,"NIFTI_INTENT_CONNECTIVITY_PARCELLATED_TIME", 3004,"Connectivity - Parcellated Time Series","","",""));
    intents.push_back(NiftiIntentEnum(NIFTI_INTENT_CONNECTIVITY_TRAJECTORY,"NIFTI_INTENT_CONNECTIVITY_TRAJECTORY", 3005,"Connectivity - Trajectory","","",""));
}

/**
 * Get a string representition of the enumerated type.
 * @param e
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
QString
NiftiIntentEnum::toName(Enum e) {
    initializeIntents();
    
    const NiftiIntentEnum* ni = findData(e);
    return ni->name;
}

/**
 * Get an enumerated value corresponding to its name.
 * @param s
 *     Name of enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     enum value exists for the input name.
 * @return
 *     Enumerated value.
 */
NiftiIntentEnum::Enum
NiftiIntentEnum::fromName(const QString& s, bool* isValidOut)
{
    initializeIntents();
    
    bool validFlag = false;
    Enum e;
    
    for (std::vector<NiftiIntentEnum>::const_iterator iter = intents.begin();
         iter != intents.end();
         iter++) {
        const NiftiIntentEnum& intent = *iter;
        if (intent.enumName == s) {
            e = intent.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

/**
 * Find the Intent object corresponding to the enum.
 * @param e
 *    The enum
 * @return 
 *    The Intent or NULL if enum does not match an intent.
 */
const 
NiftiIntentEnum* 
NiftiIntentEnum::findData(Enum e)
{
    initializeIntents();
    for (std::vector<NiftiIntentEnum>::const_iterator iter = intents.begin();
         iter != intents.end();
         iter++) {
        const NiftiIntentEnum& intent = *iter;
        return &intent;
    }
    
    CaretAssertMessage(0, "Intent enum failed to match.");
    
    return NULL;
}

/**
 * Get the "P1" name associated with an intent.
 * @param e
 *   The enum.
 * @return 
 *   P1 name associated with intent (may be empty string).
 */
QString 
NiftiIntentEnum::toNameP1(Enum e)
{
    initializeIntents();
    const NiftiIntentEnum* ni = findData(e);
    return ni->p1Name;
}

/**
 * Get the "P2" name associated with an intent.
 * @param e
 *   The enum.
 * @return 
 *   P2 name associated with intent (may be empty string).
 */
QString 
NiftiIntentEnum::toNameP2(Enum e)
{
    initializeIntents();
    const NiftiIntentEnum* ni = findData(e);
    return ni->p2Name;
}

/**
 * Get the "P3" name associated with an intent.
 * @param e
 *   The enum.
 * @return 
 *   P3 name associated with intent (may be empty string).
 */
QString 
NiftiIntentEnum::toNameP3(Enum e)
{
    initializeIntents();
    const NiftiIntentEnum* ni = findData(e);
    return ni->p3Name;
}

/**
 * Get the integer code associated with an intent.
 * @param e
 *   The enum.
 * @return 
 *   Integer code associated with intent.
 */
int32_t 
NiftiIntentEnum::toIntegerCode(Enum e)
{
    initializeIntents();
    const NiftiIntentEnum* ni = findData(e);
    return ni->integerCode;
}

/**
 * Find enum corresponding to integer code.
 * @param integerCode
 *    The integer code.
 * @param isValidOut
 *    If not NULL, on exit it indicates valid integer code.
 * @return
 *    Enum corresponding to integer code.
 */
NiftiIntentEnum::Enum 
NiftiIntentEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initializeIntents();

    bool validFlag = false;
    Enum e;
    
    for (std::vector<NiftiIntentEnum>::const_iterator iter = intents.begin();
         iter != intents.end();
         iter++) {
        const NiftiIntentEnum& intent = *iter;
        if (intent.integerCode == integerCode) {
            e = intent.e;
            validFlag = true;
            break;
        }
    }
    
    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}



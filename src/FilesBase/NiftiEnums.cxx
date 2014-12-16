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

#define __NIFTI_ENUMS_DECLARE__
#include "NiftiEnums.h"
#undef __NIFTI_ENUMS_DECLARE__


using namespace caret;

///Nifti Data Type Enum
NiftiDataTypeEnum::NiftiDataTypeEnum()
{

}

NiftiDataTypeEnum::NiftiDataTypeEnum(Enum e, const AString& name, const int32_t integerCode)
{
    this->e = e;
    this->name = name;
    this->integerCode = integerCode;
}

NiftiDataTypeEnum::~NiftiDataTypeEnum()
{

}

void 
NiftiDataTypeEnum::createDataTypes()
{
    if (dataTypesCreatedFlag) {
        return;
    }
    dataTypesCreatedFlag = true;

    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INVALID,
                                          "NIFTI_DATA_TYPE_NONE",
                                          0));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_UINT8,
                                          "NIFTI_TYPE_UINT8",
                                          2));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INT16,
                                          "NIFTI_TYPE_INT16",
                                          4));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INT32,
                                          "NIFTI_TYPE_INT32",
                                          8));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_FLOAT32,
                                          "NIFTI_TYPE_FLOAT32",
                                          16));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_COMPLEX64,
                                          "NIFTI_TYPE_COMPLEX64",
                                          32));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_FLOAT64,
                                          "NIFTI_TYPE_FLOAT64",
                                          64));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_RGB24,
                                          "NIFTI_TYPE_RGB24",
                                          128));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INT8,
                                          "NIFTI_TYPE_INT8",
                                          256));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_UINT16,
                                          "NIFTI_TYPE_UINT16",
                                          512));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_UINT32,
                                          "NIFTI_TYPE_UINT32",
                                          768));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_INT64,
                                          "NIFTI_TYPE_INT64",
                                          1024));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_UINT64,
                                          "NIFTI_TYPE_UINT64",
                                          1280));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_FLOAT128,
                                          "NIFTI_TYPE_FLOAT128",
                                          1792));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_COMPLEX128,
                                          "NIFTI_TYPE_COMPLEX128",
                                          1792));
    dataTypes.push_back(NiftiDataTypeEnum(NIFTI_TYPE_COMPLEX256,
                                          "NIFTI_TYPE_COMPLEX256",
                                          2048));
}

/**
 * Get a string representition of the enumerated type.
 * @param e
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString 
NiftiDataTypeEnum::toName(Enum e) {
    createDataTypes();

    const NiftiDataTypeEnum* ndt = findData(e);
    return ndt->name;
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
NiftiDataTypeEnum::Enum 
NiftiDataTypeEnum::fromName(const AString& s, bool* isValidOut)
{
    createDataTypes();

    bool validFlag = false;
    Enum e = NIFTI_TYPE_FLOAT32;

    for (std::vector<NiftiDataTypeEnum>::iterator iter = dataTypes.begin();
         iter != dataTypes.end();
         iter++) {
        const NiftiDataTypeEnum& ndt = *iter;
        if (ndt.name == s) {
            e = ndt.e;
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
 * Find the Data Type using the enum.
 *
 * @return
 *    Object using enum or NULL if not found.
 */
const NiftiDataTypeEnum* 
NiftiDataTypeEnum::findData(Enum e)
{
    createDataTypes();

    for (std::vector<NiftiDataTypeEnum>::iterator iter = dataTypes.begin();
         iter != dataTypes.end();
         iter++) {
        const NiftiDataTypeEnum& ndt = *iter;
        if (ndt.e == e) {
            return &ndt;
        }
    }
    return NULL;
}

/**
 * Get the integer code for a data type.
 *
 * @return
 *    Integer code for data type.
 */
int32_t 
NiftiDataTypeEnum::toIntegerCode(Enum e)
{
    createDataTypes();
    const NiftiDataTypeEnum* ndt = findData(e);
    return ndt->integerCode;
}

/**
 * Find the data type corresponding to an integer code.
 *
 * @param integerCode
 *     Integer code for enum.
 * @param isValidOut
 *     If not NULL, on exit isValidOut will indicate if
 *     integer code is valid.
 * @return
 *     Enum for integer code.
 */
NiftiDataTypeEnum::Enum 
NiftiDataTypeEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    createDataTypes();

    bool validFlag = false;
    Enum e = NIFTI_TYPE_INVALID;

    for (std::vector<NiftiDataTypeEnum>::iterator iter = dataTypes.begin();
         iter != dataTypes.end();
         iter++) {
        const NiftiDataTypeEnum& ndt = *iter;
        if (ndt.integerCode == integerCode) {
            e = ndt.e;
            validFlag = true;
            break;
        }
    }

    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

///Nifti Intent Enum
NiftiIntentEnum::NiftiIntentEnum(Enum e, 
                                 const AString& enumName,
                                 const int32_t integerCode,
                                 const AString& name,
                                 const AString& p1Name,
                                 const AString& p2Name,
                                 const AString& p3Name)
{
    this->e = e;
    this->enumName = enumName;
    this->integerCode = integerCode;
    this->name = name;
    this->p1Name = p1Name;
    this->p2Name = p2Name;
    this->p3Name = p3Name;
}

//NiftiIntentEnum::NiftiIntentEnum()
//{
//   
//}

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

    //intents.push_back(
    //   NiftiIntentEnum());

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
AString
NiftiIntentEnum::toName(Enum e) {
    initializeIntents();

    const NiftiIntentEnum* ni = findData(e);
    return ni->enumName;
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
NiftiIntentEnum::fromName(const AString& s, bool* isValidOut)
{
    initializeIntents();

    bool validFlag = false;
    Enum e = NIFTI_INTENT_NONE;

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
        if (intent.e == e) {
            return &intent;
        }
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
AString 
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
AString 
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
AString 
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
    Enum e = NIFTI_INTENT_NONE;

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

///Nifti Spacing Units Enum

NiftiSpacingUnitsEnum::NiftiSpacingUnitsEnum(Enum e, 
                                             const int32_t integerCode,
                                             const AString& name)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
}

NiftiSpacingUnitsEnum::~NiftiSpacingUnitsEnum()
{

}

void 
NiftiSpacingUnitsEnum::initializeSpacingUnits()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    spacingUnits.push_back(NiftiSpacingUnitsEnum(NIFTI_UNITS_UNKNOWN,
                                                 0,
                                                 "NIFTI_UNITS_UNKNOWN"));
    spacingUnits.push_back(NiftiSpacingUnitsEnum(NIFTI_UNITS_METER,
                                                 1,
                                                 "NIFTI_UNITS_METER"));
    spacingUnits.push_back(NiftiSpacingUnitsEnum(NIFTI_UNITS_MM,
                                                 2,
                                                 "NIFTI_UNITS_MM"));
    spacingUnits.push_back(NiftiSpacingUnitsEnum(NIFTI_UNITS_MICRON,
                                                 3,
                                                 "NIFTI_UNITS_MICRON"));
}

/**
 * Get a string representition of the enumerated type.
 * @param e
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString 
NiftiSpacingUnitsEnum::toName(Enum e) {
    initializeSpacingUnits();

    const NiftiSpacingUnitsEnum* nsu = findData(e);
    return nsu->name;
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
NiftiSpacingUnitsEnum::Enum 
NiftiSpacingUnitsEnum::fromName(const AString& s, bool* isValidOut)
{
    initializeSpacingUnits();

    bool validFlag = false;
    Enum e = NIFTI_UNITS_UNKNOWN;

    for (std::vector<NiftiSpacingUnitsEnum>::iterator iter = spacingUnits.begin();
         iter != spacingUnits.end();
         iter++) {
        const NiftiSpacingUnitsEnum& ndt = *iter;
        if (ndt.name == s) {
            e = ndt.e;
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
NiftiSpacingUnitsEnum* 
NiftiSpacingUnitsEnum::findData(Enum e)
{
    initializeSpacingUnits();

    for (std::vector<NiftiSpacingUnitsEnum>::const_iterator iter = spacingUnits.begin();
         iter != spacingUnits.end();
         iter++) {
        const NiftiSpacingUnitsEnum& nsu = *iter;
        if (nsu.e == e) {
            return &nsu;
        }
        return &nsu;
    }

    CaretAssertMessage(0, "Spacing Units enum failed to match.");

    return NULL;
}

/**
 * Get the integer code associated with an spacing units.
 * @param e
 *   The enum.
 * @return
 *   Integer code associated with spacing units.
 */
int32_t 
NiftiSpacingUnitsEnum::toIntegerCode(Enum e)
{
    initializeSpacingUnits();
    const NiftiSpacingUnitsEnum* nsu = findData(e);
    return nsu->integerCode;
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
NiftiSpacingUnitsEnum::Enum 
NiftiSpacingUnitsEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initializeSpacingUnits();

    bool validFlag = false;
    Enum e = NIFTI_UNITS_UNKNOWN;

    for (std::vector<NiftiSpacingUnitsEnum>::const_iterator iter = spacingUnits.begin();
         iter != spacingUnits.end();
         iter++) {
        const NiftiSpacingUnitsEnum& nsu = *iter;
        if (nsu.integerCode == integerCode) {
            e = nsu.e;
            validFlag = true;
            break;
        }
    }

    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

///Nifti Time Units Enum

/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 * @param guiName
 *    Name in GUI of enumberated value.
 */
NiftiTimeUnitsEnum::NiftiTimeUnitsEnum(const Enum e,
                                       const int32_t integerCode,
                                       const AString& name,
                                       const AString& guiName)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
    this->guiName = guiName;
}

/**
 * Destructor.
 */
NiftiTimeUnitsEnum::~NiftiTimeUnitsEnum()
{
}

void
NiftiTimeUnitsEnum::initializeTimeUnits()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(NiftiTimeUnitsEnum(NIFTI_UNITS_UNKNOWN, 0,"NIFTI_UNITS_UNKNOWN","Unknown"));
    enumData.push_back(NiftiTimeUnitsEnum(NIFTI_UNITS_SEC, 8,"NIFTI_UNITS_SEC","Seconds"));
    enumData.push_back(NiftiTimeUnitsEnum(NIFTI_UNITS_MSEC, 16,"NIFTI_UNITS_MSEC","Milliseconds"));
    enumData.push_back(NiftiTimeUnitsEnum(NIFTI_UNITS_USEC, 24,"NIFTI_UNITS_USEC","Microseconds"));
    enumData.push_back(NiftiTimeUnitsEnum(NIFTI_UNITS_HZ, 32,"NIFTI_UNITS_HZ","Hertz"));
    enumData.push_back(NiftiTimeUnitsEnum(NIFTI_UNITS_PPM, 40,"NIFTI_UNITS_PPM","Parts Per Million"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const NiftiTimeUnitsEnum*
NiftiTimeUnitsEnum::findData(const Enum e)
{
    initializeTimeUnits();
    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const NiftiTimeUnitsEnum* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }
    CaretAssertMessage(0, "Time Units enum failed to match.");

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e
 *     Enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     label exists for the input enum value.
 * @return
 *     String representing enumerated value.
 */
AString 
NiftiTimeUnitsEnum::toName(Enum e) {
    initializeTimeUnits();

    const NiftiTimeUnitsEnum* ntu = findData(e);
    return ntu->name;
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
NiftiTimeUnitsEnum::Enum 
NiftiTimeUnitsEnum::fromName(const AString& s, bool* isValidOut)
{
    initializeTimeUnits();

    bool validFlag = false;
    Enum e = NIFTI_UNITS_UNKNOWN;

    for (std::vector<NiftiTimeUnitsEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiTimeUnitsEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
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
 * Get the integer code associated with an time units.
 * @param e
 *   The enum.
 * @return
 *   Integer code associated with time units.
 */
int32_t 
NiftiTimeUnitsEnum::toIntegerCode(Enum e)
{
    initializeTimeUnits();
    const NiftiTimeUnitsEnum* nsu = findData(e);
    return nsu->integerCode;
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
NiftiTimeUnitsEnum::Enum 
NiftiTimeUnitsEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initializeTimeUnits();

    bool validFlag = false;
    Enum e = NIFTI_UNITS_UNKNOWN;

    for (std::vector<NiftiTimeUnitsEnum>::const_iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiTimeUnitsEnum& nsu = *iter;
        if (nsu.integerCode == integerCode) {
            e = nsu.e;
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
 * Get a string representation for GUI of the enumerated type.
 * @param e
 *     Enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     label exists for the input enum value.
 * @return
 *     String representing enumerated value for GUI.
 */
AString
NiftiTimeUnitsEnum::toGuiName(Enum e) {
    initializeTimeUnits();
    
    const NiftiTimeUnitsEnum* ntu = findData(e);
    return ntu->guiName;
}



/**
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
NiftiTransformEnum::NiftiTransformEnum(
        const Enum e,
        const int32_t integerCode,
        const AString& name)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
}

/**
 * Destructor.
 */
NiftiTransformEnum::~NiftiTransformEnum()
{
}

void
NiftiTransformEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_UNKNOWN, 0,"NIFTI_XFORM_UNKNOWN"));
    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_SCANNER_ANAT, 1,"NIFTI_XFORM_SCANNER_ANAT"));
    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_ALIGNED_ANAT, 2,"NIFTI_XFORM_ALIGNED_ANAT"));
    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_TALAIRACH, 3,"NIFTI_XFORM_TALAIRACH"));
    enumData.push_back(NiftiTransformEnum(NIFTI_XFORM_MNI_152, 4,"NIFTI_XFORM_MNI_152"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const NiftiTransformEnum*
NiftiTransformEnum::findData(const Enum e)
{
    initialize();
    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const NiftiTransformEnum* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e
 *     Enumerated value.
 * @param isValidOut
 *     If not NULL, it is set indicating that a
 *     label exists for the input enum value.
 * @return
 *     String representing enumerated value.
 */
AString 
NiftiTransformEnum::toName(Enum e) {
    initialize();

    const NiftiTransformEnum* nt = findData(e);
    return nt->name;
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
NiftiTransformEnum::Enum 
NiftiTransformEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();

    bool validFlag = false;
    Enum e = NIFTI_XFORM_UNKNOWN;

    for (std::vector<NiftiTransformEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiTransformEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
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
 * Get the integer code associated with a transform.
 * @param e
 *   The enum.
 * @return
 *   Integer code associated with a transform.
 */
int32_t 
NiftiTransformEnum::toIntegerCode(Enum e)
{
    initialize();
    const NiftiTransformEnum* nsu = findData(e);
    return nsu->integerCode;
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
NiftiTransformEnum::Enum 
NiftiTransformEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();

    bool validFlag = false;
    Enum e = NIFTI_XFORM_UNKNOWN;

    for (std::vector<NiftiTransformEnum>::const_iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiTransformEnum& nsu = *iter;
        if (nsu.integerCode == integerCode) {
            e = nsu.e;
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
 * Constructor.
 *
 * @param e
 *    An enumerated value.
 * @param name
 *    Name of enumberated value.
 */
NiftiVersionEnum::NiftiVersionEnum(
        const Enum e,
        const int32_t integerCode,
        const AString& name)
{
    this->e = e;
    this->integerCode = integerCode;
    this->name = name;
}

/**
 * Destructor.
 */
NiftiVersionEnum::~NiftiVersionEnum()
{
}

void
NiftiVersionEnum::initialize()
{
    if (initializedFlag) {
        return;
    }
    initializedFlag = true;

    enumData.push_back(NiftiVersionEnum(NIFTI_VERSION_1, 348, "NIFTI_VERSION_1"));
    enumData.push_back(NiftiVersionEnum(NIFTI_VERSION_2, 540, "NIFTI_VERSION_2"));
}

/**
 * Find the data for and enumerated value.
 * @param e
 *     The enumerated value.
 * @return Pointer to data for this enumerated type
 * or NULL if no data for type or if type is invalid.
 */
const NiftiVersionEnum*
NiftiVersionEnum::findData(const Enum e)
{
    initialize();
    int64_t num = enumData.size();
    for (int64_t i = 0; i < num; i++) {
        const NiftiVersionEnum* d = &enumData[i];
        if (d->e == e) {
            return d;
        }
    }

    CaretAssertMessage(0, "NIFTI Version enum failed to match.");

    return NULL;
}

/**
 * Get a string representation of the enumerated type.
 * @param e
 *     Enumerated value.
 * @return
 *     String representing enumerated value.
 */
AString 
NiftiVersionEnum::toName(Enum e) {
    initialize();

    const NiftiVersionEnum* nv = findData(e);
    return nv->name;
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
NiftiVersionEnum::Enum 
NiftiVersionEnum::fromName(const AString& s, bool* isValidOut)
{
    initialize();

    bool validFlag = false;
    Enum e = NIFTI_VERSION_1;

    for (std::vector<NiftiVersionEnum>::iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiVersionEnum& d = *iter;
        if (d.name == s) {
            e = d.e;
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
 * Get the integer code associated with a transform.
 * @param e
 *   The enum.
 * @return
 *   Integer code associated with a transform.
 */
int32_t 
NiftiVersionEnum::toIntegerCode(Enum e)
{
    initialize();
    const NiftiVersionEnum* nsu = findData(e);
    return nsu->integerCode;
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
NiftiVersionEnum::Enum 
NiftiVersionEnum::fromIntegerCode(const int32_t integerCode, bool* isValidOut)
{
    initialize();

    bool validFlag = false;
    Enum e = NIFTI_VERSION_1;

    for (std::vector<NiftiVersionEnum>::const_iterator iter = enumData.begin();
         iter != enumData.end();
         iter++) {
        const NiftiVersionEnum& nsu = *iter;
        if (nsu.integerCode == integerCode) {
            e = nsu.e;
            validFlag = true;
            break;
        }
    }

    if (isValidOut != 0) {
        *isValidOut = validFlag;
    }
    return e;
}

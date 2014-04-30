#ifndef __NIFTI_ENUMS_H__
#define __NIFTI_ENUMS_H__

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

#include <stdint.h>

#include <vector>
#include <AString.h>
#include <CaretAssert.h>
#include "nifti2.h"

namespace caret {

//we specify here whether to write in native byte order, or to honor the original
//byte order, we could also get clever and try to determine whether or not we are
//a big or little-endian machine, but the key concept to keep track of is whether or
//not we want to honor the byte order of the original file (for in place read/write)
//not the actual byte order.  This should also function portably.
enum NIFTI_BYTE_ORDER {
    NATIVE_BYTE_ORDER,
    SWAPPED_BYTE_ORDER
};
/**
    * NIFTI Data Types.  Note that only a small subset are used
    * in GIFTI data files.
    */
class NiftiDataTypeEnum {

public:
    /**  NIFTI Data Types.  Note that only a small subset are used
       * in GIFTI data files.
       */
    enum Enum {
        /** invalid data type.  */
        NIFTI_TYPE_INVALID = 0,
        /** unsigned byte.  */
        NIFTI_TYPE_UINT8 = ::NIFTI_TYPE_UINT8,
        /** signed short.  */
        NIFTI_TYPE_INT16 = ::NIFTI_TYPE_INT16,
        /** signed int.  */
        NIFTI_TYPE_INT32 = ::NIFTI_TYPE_INT32,
        /** 32 bit float.  */
        NIFTI_TYPE_FLOAT32 = ::NIFTI_TYPE_FLOAT32,
        /** 64 bit complex = 2 32 bit floats.  */
        NIFTI_TYPE_COMPLEX64 = ::NIFTI_TYPE_COMPLEX64,
        /** 64 bit float = double.  */
        NIFTI_TYPE_FLOAT64 = ::NIFTI_TYPE_FLOAT64,
        /** 3 8 bit bytes.  */
        NIFTI_TYPE_RGB24 = ::NIFTI_TYPE_RGB24,
        /** signed char.  */
        NIFTI_TYPE_INT8 = ::NIFTI_TYPE_INT8,
        /** unsigned short.  */
        NIFTI_TYPE_UINT16 = ::NIFTI_TYPE_UINT16,
        /** unsigned int.  */
        NIFTI_TYPE_UINT32 = ::NIFTI_TYPE_UINT32,
        /** signed long long.  */
        NIFTI_TYPE_INT64 = ::NIFTI_TYPE_INT64,
        /** unsigned long long.  */
        NIFTI_TYPE_UINT64 = ::NIFTI_TYPE_UINT64,
        /** 128 bit float = long double.  */
        NIFTI_TYPE_FLOAT128 = ::NIFTI_TYPE_FLOAT128,
        /** 128 bit complex = 2 64 bit floats.  */
        NIFTI_TYPE_COMPLEX128 = ::NIFTI_TYPE_COMPLEX128,
        /** 256 bit complex = 2 128 bit floats  */
        NIFTI_TYPE_COMPLEX256 = ::NIFTI_TYPE_COMPLEX256
    };


    ~NiftiDataTypeEnum();
private:
    NiftiDataTypeEnum();
    NiftiDataTypeEnum(Enum e, const AString& name, const int32_t integerCode);
    static const NiftiDataTypeEnum* findData(Enum e);
    static std::vector<NiftiDataTypeEnum> dataTypes;
    static void createDataTypes();
    static bool dataTypesCreatedFlag;

    Enum e;
    AString name;
    int32_t integerCode;

public:
    static AString toName(Enum e);
    static Enum fromName(const AString& s, bool* isValidOut);
    static int32_t toIntegerCode(Enum e);
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

};

/**
    * NIFTI Intent codes and related parameters.
    */
class NiftiIntentEnum {

public:
    /**  NIFTI Intent codes and related parameters.
       */
    enum Enum {
        /**  */
        NIFTI_INTENT_NONE = ::NIFTI_INTENT_NONE,
        /**  */
        NIFTI_INTENT_CORREL = ::NIFTI_INTENT_CORREL,
        /**  */
        NIFTI_INTENT_TTEST = ::NIFTI_INTENT_FTEST,
        /**  */
        NIFTI_INTENT_FTEST = ::NIFTI_INTENT_FTEST,
        /**  */
        NIFTI_INTENT_ZSCORE = ::NIFTI_INTENT_ZSCORE,
        /**  */
        NIFTI_INTENT_CHISQ = ::NIFTI_INTENT_CHISQ,
        /**  */
        NIFTI_INTENT_BETA = ::NIFTI_INTENT_BETA,
        /**  */
        NIFTI_INTENT_BINOM = ::NIFTI_INTENT_BINOM,
        /**  */
        NIFTI_INTENT_GAMMA = ::NIFTI_INTENT_GAMMA,
        /**  */
        NIFTI_INTENT_POISSON = ::NIFTI_INTENT_POISSON,
        /**  */
        NIFTI_INTENT_NORMAL = ::NIFTI_INTENT_NORMAL,
        /**  */
        NIFTI_INTENT_FTEST_NONC = ::NIFTI_INTENT_FTEST_NONC,
        /**  */
        NIFTI_INTENT_CHISQ_NONC = ::NIFTI_INTENT_CHISQ_NONC,
        /**  */
        NIFTI_INTENT_LOGISTIC = ::NIFTI_INTENT_LOGISTIC,
        /**  */
        NIFTI_INTENT_LAPLACE = ::NIFTI_INTENT_LAPLACE,
        /**  */
        NIFTI_INTENT_UNIFORM = ::NIFTI_INTENT_UNIFORM,
        /**  */
        NIFTI_INTENT_TTEST_NONC = ::NIFTI_INTENT_TTEST_NONC,
        /**  */
        NIFTI_INTENT_WEIBULL = ::NIFTI_INTENT_WEIBULL,
        /**  */
        NIFTI_INTENT_CHI = ::NIFTI_INTENT_CHI,
        /**  */
        NIFTI_INTENT_INVGAUSS = ::NIFTI_INTENT_INVGAUSS,
        /**  */
        NIFTI_INTENT_EXTVAL = ::NIFTI_INTENT_EXTVAL,
        /**  */
        NIFTI_INTENT_PVAL = ::NIFTI_INTENT_PVAL,
        /**  */
        NIFTI_INTENT_LOGPVAL = ::NIFTI_INTENT_LOGPVAL,
        /**  */
        NIFTI_INTENT_LOG10PVAL = ::NIFTI_INTENT_LOG10PVAL,
        /**  */
        NIFTI_INTENT_ESTIMATE = ::NIFTI_INTENT_ESTIMATE,
        /**  */
        NIFTI_INTENT_LABEL = ::NIFTI_INTENT_LABEL,
        /**  */
        NIFTI_INTENT_NEURONAME = ::NIFTI_INTENT_NEURONAME,
        /**  */
        NIFTI_INTENT_GENMATRIX = ::NIFTI_INTENT_GENMATRIX,
        /**  */
        NIFTI_INTENT_SYMMATRIX = ::NIFTI_INTENT_SYMMATRIX,
        /**  */
        NIFTI_INTENT_DISPVECT = ::NIFTI_INTENT_DISPVECT,
        /**  */
        NIFTI_INTENT_VECTOR = ::NIFTI_INTENT_VECTOR,
        /**  */
        NIFTI_INTENT_POINTSET = ::NIFTI_INTENT_POINTSET,
        /**  */
        NIFTI_INTENT_TRIANGLE = ::NIFTI_INTENT_TRIANGLE,
        /**  */
        NIFTI_INTENT_QUATERNION = ::NIFTI_INTENT_QUATERNION,
        /**  */
        NIFTI_INTENT_DIMLESS = ::NIFTI_INTENT_DIMLESS,
        /**  */
        NIFTI_INTENT_TIME_SERIES = 2001,
        /**  */
        NIFTI_INTENT_NODE_INDEX = 2002,
        /**  */
        NIFTI_INTENT_RGB_VECTOR = 2003,
        /**  */
        NIFTI_INTENT_RGBA_VECTOR = 2004,
        /**  */
        NIFTI_INTENT_SHAPE = 2005,
        /**  */
        NIFTI_INTENT_CARET_DEFORMATION_NODE_INDICES = 25000,
        /**  */
        NIFTI_INTENT_CARET_DEFORMATION_NODE_AREAS = 25001,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_DENSE = ::NIFTI_INTENT_CONNECTIVITY_DENSE,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_DENSE_TIME = ::NIFTI_INTENT_CONNECTIVITY_DENSE_TIME,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_PARCELLATED = ::NIFTI_INTENT_CONNECTIVITY_PARCELLATED,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_PARCELLATED_TIME = ::NIFTI_INTENT_CONNECTIVITY_PARCELLATED_TIME,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_TRAJECTORY = ::NIFTI_INTENT_CONNECTIVITY_DENSE_TRAJECTORY
    };


    ~NiftiIntentEnum();

private:
    NiftiIntentEnum(const Enum e,
                    const AString& enumName,
                    const int32_t integerCode,
                    const AString& name,
                    const AString& p1Name,
                    const AString& p2Name,
                    const AString& p3Name);

    //NiftiIntentEnum();

    Enum e;
    AString enumName;
    int32_t integerCode;
    AString name;
    AString p1Name;
    AString p2Name;
    AString p3Name;

    static void initializeIntents();
    static const NiftiIntentEnum* findData(Enum e);

    static std::vector<NiftiIntentEnum> intents;
    static bool intentsCreatedFlag;

public:
    static AString toName(Enum e);
    static AString toNameP1(Enum e);
    static AString toNameP2(Enum e);
    static AString toNameP3(Enum e);
    static Enum fromName(const AString& s, bool* isValidOut);
    static int32_t toIntegerCode(Enum e);
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

};

/**
    * NIFTI Spacing Units
    */
class NiftiSpacingUnitsEnum {

public:
    /**  NIFTI Spacing Units
       */
    enum Enum {
        /**  */
        NIFTI_UNITS_UNKNOWN = ::NIFTI_UNITS_UNKNOWN,
        /**  */
        NIFTI_UNITS_METER = ::NIFTI_UNITS_METER,
        /**  */
        NIFTI_UNITS_MM = ::NIFTI_UNITS_MM,
        /**  */
        NIFTI_UNITS_MICRON  = ::NIFTI_UNITS_MICRON
    };


    ~NiftiSpacingUnitsEnum();
private:
    NiftiSpacingUnitsEnum(Enum e, const int32_t integerCode, const AString& name);
    static const NiftiSpacingUnitsEnum* findData(Enum e);
    static std::vector<NiftiSpacingUnitsEnum> spacingUnits;
    static void initializeSpacingUnits();
    static bool initializedFlag;

    Enum e;
    int32_t integerCode;
    AString name;
public:
    static AString toName(Enum e);
    static Enum fromName(const AString& s, bool* isValidOut);
    static int32_t toIntegerCode(Enum e);
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

};

/**
    * NIFTI Time Units
    */
class NiftiTimeUnitsEnum {

public:
    /**  NIFTI Time Units
       */
    enum Enum {
        /**  */
        NIFTI_UNITS_UNKNOWN = ::NIFTI_UNITS_UNKNOWN,
        /**  */
        NIFTI_UNITS_SEC = ::NIFTI_UNITS_SEC,
        /**  */
        NIFTI_UNITS_MSEC = ::NIFTI_UNITS_MSEC,
        /**  */
        NIFTI_UNITS_USEC = ::NIFTI_UNITS_USEC,
        /**  */
        NIFTI_UNITS_HZ = ::NIFTI_UNITS_HZ,
        /**  */
        NIFTI_UNITS_PPM = ::NIFTI_UNITS_PPM
    };


    ~NiftiTimeUnitsEnum();

    static AString toName(Enum e);

    static Enum fromName(const AString& s, bool* isValidOut);

    static int32_t toIntegerCode(Enum e);

    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

private:
    NiftiTimeUnitsEnum(const Enum e, const int32_t integerCode, const AString& name);

    static const NiftiTimeUnitsEnum* findData(const Enum e);

    static std::vector<NiftiTimeUnitsEnum> enumData;

    static void initializeTimeUnits();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;


};

/**
    * NIFTI Transform.
    */
class NiftiTransformEnum {

public:
    /**  NIFTI Transform.
       */
    enum Enum {
        /** Arbitrary Coordinates  */
        NIFTI_XFORM_UNKNOWN  = ::NIFTI_XFORM_UNKNOWN,
        /** Scanner-base anatomical coordinates  */
        NIFTI_XFORM_SCANNER_ANAT  = ::NIFTI_XFORM_SCANNER_ANAT,
        /** Coordinates aligned to another file's or anatomial "truth"  */
        NIFTI_XFORM_ALIGNED_ANAT = ::NIFTI_XFORM_ALIGNED_ANAT,
        /** Coordinates aligned to Talairach-Tournoux Atlas: (0,0,0) = Anterior Commissure  */
        NIFTI_XFORM_TALAIRACH  = ::NIFTI_XFORM_TALAIRACH,
        /** MNI 152 Normalize Coordinates  */
        NIFTI_XFORM_MNI_152  = ::NIFTI_XFORM_MNI_152
    };


    ~NiftiTransformEnum();

    static AString toName(Enum e);

    static Enum fromName(const AString& s, bool* isValidOut);

    static int32_t toIntegerCode(Enum e);

    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

private:
    NiftiTransformEnum(const Enum e, const int32_t integerCode, const AString& name);

    static std::vector<NiftiTransformEnum> enumData;

    static const NiftiTransformEnum* findData(const Enum e);

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;
};

/**
    * The NIFTI version
    */
class NiftiVersionEnum {

public:
    /**  The NIFTI version
       */
    enum Enum {
        /** NIFTI-1  */
        NIFTI_VERSION_1,
        /** NIFTI-2  */
        NIFTI_VERSION_2
    };


    ~NiftiVersionEnum();

    static AString toName(Enum e);

    static Enum fromName(const AString& s, bool* isValidOut);

    static int32_t toIntegerCode(Enum e);

    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);

private:
    NiftiVersionEnum(const Enum e, const int32_t integerCode, const AString& name);

    static std::vector<NiftiVersionEnum> enumData;

    static void initialize();

    static bool initializedFlag;

    Enum e;

    int32_t integerCode;

    AString name;

    static const NiftiVersionEnum* findData(const Enum e);

};

#ifdef __NIFTI_ENUMS_DECLARE__
std::vector<NiftiDataTypeEnum> NiftiDataTypeEnum::dataTypes;
bool NiftiDataTypeEnum::dataTypesCreatedFlag = false;
std::vector<NiftiIntentEnum> NiftiIntentEnum::intents;
bool NiftiIntentEnum::intentsCreatedFlag = false;
std::vector<NiftiSpacingUnitsEnum> NiftiSpacingUnitsEnum::spacingUnits;
bool NiftiSpacingUnitsEnum::initializedFlag = false;
std::vector<NiftiTimeUnitsEnum> NiftiTimeUnitsEnum::enumData;
bool NiftiTimeUnitsEnum::initializedFlag = false;
std::vector<NiftiTransformEnum> NiftiTransformEnum::enumData;
bool NiftiTransformEnum::initializedFlag = false;
std::vector<NiftiVersionEnum> NiftiVersionEnum::enumData;
bool NiftiVersionEnum::initializedFlag = false;
#endif // __NIFTI_ENUMS_DECLARE__
} // namespace

#endif // __NIFTI_ENUMS_H

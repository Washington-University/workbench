#ifndef __NIFTIINTENT_H__
#define __NIFTIINTENT_H__

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


#include <stdint.h>

#include <string>
#include <vector>

namespace caret {

/**
 * NIFTI Intent codes and related parameters.
 */
class NiftiIntent {

public:
    /**  NIFTI Intent codes and related parameters.
 */
    enum Enum {
        /**  */
        NIFTI_INTENT_NONE,
        /**  */
        NIFTI_INTENT_CORREL,
        /**  */
        NIFTI_INTENT_TTEST,
        /**  */
        NIFTI_INTENT_FTEST,
        /**  */
        NIFTI_INTENT_ZSCORE,
        /**  */
        NIFTI_INTENT_CHISQ,
        /**  */
        NIFTI_INTENT_BETA,
        /**  */
        NIFTI_INTENT_BINOM,
        /**  */
        NIFTI_INTENT_GAMMA,
        /**  */
        NIFTI_INTENT_POISSON,
        /**  */
        NIFTI_INTENT_NORMAL,
        /**  */
        NIFTI_INTENT_FTEST_NONC,
        /**  */
        NIFTI_INTENT_CHISQ_NONC,
        /**  */
        NIFTI_INTENT_LOGISTIC,
        /**  */
        NIFTI_INTENT_LAPLACE,
        /**  */
        NIFTI_INTENT_UNIFORM,
        /**  */
        NIFTI_INTENT_TTEST_NONC,
        /**  */
        NIFTI_INTENT_WEIBULL,
        /**  */
        NIFTI_INTENT_CHI,
        /**  */
        NIFTI_INTENT_INVGAUSS,
        /**  */
        NIFTI_INTENT_EXTVAL,
        /**  */
        NIFTI_INTENT_PVAL,
        /**  */
        NIFTI_INTENT_LOGPVAL,
        /**  */
        NIFTI_INTENT_LOG10PVAL,
        /**  */
        NIFTI_INTENT_ESTIMATE,
        /**  */
        NIFTI_INTENT_LABEL,
        /**  */
        NIFTI_INTENT_NEURONAME,
        /**  */
        NIFTI_INTENT_GENMATRIX,
        /**  */
        NIFTI_INTENT_SYMMATRIX,
        /**  */
        NIFTI_INTENT_DISPVECT,
        /**  */
        NIFTI_INTENT_VECTOR,
        /**  */
        NIFTI_INTENT_POINTSET,
        /**  */
        NIFTI_INTENT_TRIANGLE,
        /**  */
        NIFTI_INTENT_QUATERNION,
        /**  */
        NIFTI_INTENT_DIMLESS,
        /**  */
        NIFTI_INTENT_TIME_SERIES,
        /**  */
        NIFTI_INTENT_NODE_INDEX,
        /**  */
        NIFTI_INTENT_RGB_VECTOR,
        /**  */
        NIFTI_INTENT_RGBA_VECTOR,
        /**  */
        NIFTI_INTENT_SHAPE,
        /**  */
        NIFTI_INTENT_CARET_DEFORMATION_NODE_INDICES,
        /**  */
        NIFTI_INTENT_CARET_DEFORMATION_NODE_AREAS,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_DENSE,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_DENSE_TIME,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_PARCELLATED,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_PARCELLATED_TIME,
        /**  */
        NIFTI_INTENT_CONNECTIVITY_TRAJECTORY
    };


    ~NiftiIntent();

private:
    NiftiIntent(const Enum e, 
                const std::string& enumName,
                const int32_t integerCode,
                const std::string& name,
                const std::string& p1Name,
                const std::string& p2Name,
                const std::string& p3Name);
    
    NiftiIntent();
    
    Enum e;
    std::string enumName;
    int32_t integerCode;
    std::string name;
    std::string p1Name;
    std::string p2Name;
    std::string p3Name;
    
    static void initializeIntents();
    static const NiftiIntent* findData(Enum e);

    static std::vector<NiftiIntent> intents;
    static bool intentsCreatedFlag;

public:
    static std::string toName(Enum e);
    static std::string toNameP1(Enum e);
    static std::string toNameP2(Enum e);
    static std::string toNameP3(Enum e);
    static Enum fromName(const std::string& s, bool* isValidOut);
    static int32_t toIntegerCode(Enum e);
    static Enum fromIntegerCode(const int32_t integerCode, bool* isValidOut);
    
};
    
#ifdef __NIFTI_INTENT_DECLARE__
    std::vector<NiftiIntent> NiftiIntent::intents;
    bool NiftiIntent::intentsCreatedFlag = false;
#endif // __NIFTI_INTENT_DECLARE__


} // namespace

#endif // __NIFTIINTENT_H__
